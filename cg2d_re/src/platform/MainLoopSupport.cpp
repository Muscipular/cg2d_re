#include "StartupNative.hpp"
#include "cg/text/Encoding.hpp"
#include "MainLoopDependencies.hpp"

#include "cg/audio/LegacyAudio.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/SurfaceState.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。
#include <cstdlib>
#include <cwchar>
#include <cstring> // 原CRT接口：0x0053BEC0、0x0053BFC0、0x0054AB30、0x0053CF50；原参数／返回合同由直接调用承接。
#include <random>
#include <tlhelp32.h>

namespace cg::platform {


namespace {

using LegacyRandomEngine =
    std::linear_congruential_engine<std::uint32_t, 0x343FDu, 0x269EC3u, 0>;

// 原0x0058F870：唯一共享W32状态，初始种子1；首次使用初始化不推进序列。
LegacyRandomEngine& SharedRandomEngine()
{
    static LegacyRandomEngine engine{1};
    return engine;
}

// 原 0x73ED1C、0x73ED20；volatile 保留两个独立 W32 全局写入。
volatile std::uint64_t gKeepAliveBaselineTick = 0; // 原全局：0x0073ED1C。
volatile std::uint64_t gKeepAliveRecentTick = 0; // 原全局：0x0073ED20。

// 原 W32 0x59E544、0x593FD0；供未来 0x4043B0/0x4048E0 在本文件共享。
volatile std::uint32_t gValue59E544 = 0; // 原全局：0x0059E544。
volatile std::uint32_t gValue593FD0 = 0; // 原全局：0x00593FD0。

// 原 W32 0x59E564；0x4043B0 独立使用，不能与其他防护计数槽复用。
volatile std::uint32_t gProtectionCheckCounter = 0;

// 原 W32 0x59E558；0x4041E0 仅更新低 W8。
std::uint32_t gProtectionPlatformState = 0;

// 原 W32 0x73ED38；能力探测包装入口先写此槽，再比较返回码。
volatile std::uint32_t gDxCapabilityCode73ED38 = 0;

// 原 R32 0x59E55C；使用原生句柄宽度，供未来 0x4041E0/0x4043B0 在本文件共享。
HMODULE gProtectionModule = nullptr;

using IsDebuggerPresentFunction = BOOL (WINAPI*)();
IsDebuggerPresentFunction gIsDebuggerPresent = nullptr;

std::uint8_t gProtectionBuffer1[0x5000]{};
std::uint8_t gProtectionBuffer2[0x5000]{};

struct ProtectionAuxiliaryRecord {
    char text[0x20];
    std::uint32_t number;
    std::uint16_t flagA;
    std::uint16_t flagB;
};
static_assert(offsetof(ProtectionAuxiliaryRecord, number) == 0x20);
static_assert(offsetof(ProtectionAuxiliaryRecord, flagA) == 0x24);
static_assert(offsetof(ProtectionAuxiliaryRecord, flagB) == 0x26);

ProtectionAuxiliaryRecord gProtectionAuxiliaryRecords[30]{};
std::uint32_t gProtectionAuxiliaryRecordCount = 0;
OSVERSIONINFOA gProtectionVersionInfo{};

static_assert(sizeof(gProtectionProcessState) == 16);
static_assert(sizeof(gProtectionChecksum1) == 16);
static_assert(sizeof(gProtectionChecksum2) == 16);
static_assert(sizeof(gProtectionBuffer1) == 0x5000);
static_assert(sizeof(gProtectionBuffer2) == 0x5000);
static_assert(sizeof(ProtectionAuxiliaryRecord) == 0x28);
static_assert(sizeof(gProtectionVersionInfo) == 0x94);

std::uint32_t RandomSymmetricW32(const std::uint32_t amplitudeBits)
{
    const std::uint32_t lowerBits = std::uint32_t{0} - amplitudeBits;
    const std::uint32_t upperBits = amplitudeBits;
    if (upperBits + std::uint32_t{1} == 0) {
        return 0;
    }

    // 对称 W32 区间宽度恒为奇数，因此 signed32 除数不可能为 0。
    const std::uint32_t divisorBits =
        upperBits - lowerBits + std::uint32_t{1};
    const std::int32_t divisor =
        std::bit_cast<std::int32_t>(divisorBits);
    const std::int32_t remainder =
        static_cast<std::int32_t>(Rand16()) % divisor;
    return std::bit_cast<std::uint32_t>(remainder) + lowerBits;
}

void UpdateProtectionChecksum(
    LegacyCryptInt* checksum,
    const std::uint8_t* table)
{
    for (std::size_t slotIndex = 0; slotIndex < 20; ++slotIndex) {
        const std::uint8_t* slot = table + slotIndex * 0x400;
        if (slot[0] == 0) {
            break;
        }

        for (std::size_t byteIndex = 0; byteIndex < 0x400; ++byteIndex) {
            const std::uint8_t byte = slot[byteIndex];
            if (byte == 0) {
                break;
            }

            ((*checksum) += static_cast<std::int32_t>(
                    std::bit_cast<std::int8_t>(byte)));
        }
    }
}

// 原 0x4042D0：按快照模块所属 PID 选择对应的模块名表。
[[maybe_unused]] char CheckProcessModules_4042D0(const DWORD processId)
{
    MODULEENTRY32 moduleEntry{};
    const HANDLE snapshot =
        CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    char result = 0;
    moduleEntry.dwSize = static_cast<DWORD>(sizeof(moduleEntry));
    BOOL hasModule = Module32First(snapshot, &moduleEntry);
    while (hasModule != FALSE) {
        const DWORD currentProcessId = static_cast<DWORD>(
            gProtectionProcessState.value());
        const std::uint8_t* moduleNameTable =
            moduleEntry.th32ProcessID == currentProcessId
            ? gProtectionBuffer2
            : gProtectionBuffer1;

        for (std::size_t index = 0; index < 20; ++index) {
            const char* moduleName = reinterpret_cast<const char*>(
                moduleNameTable + index * 0x400);
            if (moduleName[0] == '\0') {
                break;
            }
            if (::_stricmp(moduleEntry.szModule, moduleName) == 0) {
                result = 1;
                break;
            }
        }

        if (result != 0) {
            break;
        }
        hasModule = Module32Next(snapshot, &moduleEntry);
    }

    CloseHandle(snapshot);
    return result;
}

// 原 0x404880：初始化失败后不重试，缓存使用本机函数指针宽度。
std::int32_t CheckDebugger_404880()
{
    auto* const platformStateBytes =
        reinterpret_cast<std::uint8_t*>(&gProtectionPlatformState);
    if (platformStateBytes[1] == 0) {
        platformStateBytes[1] = 1;
        const HMODULE kernel32 = GetModuleHandleA("KERNEL32");
        if (kernel32 != nullptr) {
            gIsDebuggerPresent =
                reinterpret_cast<IsDebuggerPresentFunction>(
                    GetProcAddress(kernel32, "IsDebuggerPresent"));
        }
    }

    if (gIsDebuggerPresent == nullptr) {
        return 0;
    }
    return gIsDebuggerPresent() != FALSE ? 3 : 0;
}

// 原函数：0x00404940。
BOOL CALLBACK CheckWindowsCallback_404940(HWND window, LPARAM)
{
    wchar_t wideTitle[128] {};
    static_cast<void>(GetWindowTextW(
        window,
        wideTitle,
        static_cast<int>(sizeof(wideTitle) / sizeof(wchar_t))));
    const wchar_t* const title = wideTitle;
    const std::uint32_t titleLength = static_cast<std::uint32_t>(std::wcslen(title));
    if (titleLength == 0) {
        return TRUE;
    }

    std::int32_t recordCount =
        std::bit_cast<std::int32_t>(gProtectionAuxiliaryRecordCount);
    if (recordCount <= 0) {
        return TRUE;
    }

    for (std::int32_t index = 0; index < recordCount; ++index) {
        const ProtectionAuxiliaryRecord& record =
            gProtectionAuxiliaryRecords[index];
        const std::wstring recordText = cg::text::FromGbkWide(record.text);
        const std::uint16_t flag = record.flagA;
        bool matched = false;
        if (flag == 0) {
            const std::uint32_t length = static_cast<std::uint32_t>(recordText.size());
            matched = ::_wcsnicmp(title, recordText.c_str(), static_cast<std::uint32_t>(length)) == 0;
        } else if (flag == 1) {
            const std::uint32_t length = static_cast<std::uint32_t>(recordText.size());
            matched = std::wcsncmp(title, recordText.c_str(), static_cast<std::uint32_t>(length)) == 0
                && static_cast<std::uint32_t>(recordText.size()) == titleLength;
        }

        if (matched) {
            gValue59E544 = record.number;
            return FALSE;
        }
        recordCount = std::bit_cast<std::int32_t>(gProtectionAuxiliaryRecordCount);
    }

    return TRUE;
}

} // namespace

// 原函数：0x004041E0。
void InitializeProtection_4041E0(const char* moduleName)
{
    if (gProtectionModule == nullptr) {
        gProtectionModule = LoadLibraryA(moduleName);
    }

    std::memset(gProtectionBuffer1, 0, sizeof(gProtectionBuffer1));
    std::memset(gProtectionBuffer2, 0, sizeof(gProtectionBuffer2));
    std::memset(
        &gProtectionVersionInfo,
        0,
        sizeof(gProtectionVersionInfo));
    gProtectionVersionInfo.dwOSVersionInfoSize = 0x94;
    static_cast<void>(GetVersionExA(&gProtectionVersionInfo));

    const DWORD platformId = gProtectionVersionInfo.dwPlatformId;
    const DWORD majorVersion = gProtectionVersionInfo.dwMajorVersion;
    const DWORD minorVersion = gProtectionVersionInfo.dwMinorVersion;
    const std::uint32_t isLegacyWindows =
        platformId == DWORD{1}
        && majorVersion <= DWORD{4}
        && (majorVersion != DWORD{4} || minorVersion == DWORD{0})
        ? std::uint32_t{1}
        : std::uint32_t{0};
    gProtectionPlatformState =
        (gProtectionPlatformState & 0xFFFFFF00u) | isLegacyWindows;

    const std::uint32_t processIdBits =
        static_cast<std::uint32_t>(GetCurrentProcessId());
    (gProtectionProcessState = static_cast<std::int32_t>(std::bit_cast<std::int32_t>(processIdBits)));
}

// 原函数：0x004043B0。
std::uint8_t CheckProtection_4043B0(const char* functionName)
{
    const std::uint32_t nextCounter =
        gProtectionCheckCounter + std::uint32_t{1};
    gProtectionCheckCounter = nextCounter;
    if (nextCounter < std::uint32_t{1380}) {
        return 0;
    }
    gProtectionCheckCounter = 0;

    constexpr double kFrameThreshold = 1000.0 / 60.0;
    if (network::ReadNetworkRuntimeActive() == 1
        && (gFrameThreshold != kFrameThreshold
            || gFrameIncrement < gFrameThreshold)) {
        static_cast<void>(Report_512CA0(gConnectionSocket, 3));
        gFrameThreshold = kFrameThreshold;
        gFrameIncrement = kFrameThreshold;
    }

    if (gProtectionModule != nullptr) {
        const FARPROC procedure =
            GetProcAddress(gProtectionModule, functionName);
        const auto* const code = reinterpret_cast<const std::uint8_t*>(
            reinterpret_cast<std::uintptr_t>(procedure));
        if (code[0] != 0) {
            for (std::size_t index = 0; index < 10; ++index) {
                if (code[index] == 0xE9) {
                    return 1;
                }
            }
        }
    }

    const HANDLE mutex =
        OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "XZYDRaEvyPQC");
    if (mutex == nullptr) {
        return 1;
    }
    CloseHandle(mutex);

    PROCESSENTRY32 processEntry{};
    std::uint8_t processResult = 0;
    const HANDLE snapshot =
        CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    processEntry.dwSize = static_cast<DWORD>(sizeof(processEntry));
    BOOL hasProcess = Process32First(snapshot, &processEntry);
    while (hasProcess != FALSE) {
        if (CheckProcessModules_4042D0(processEntry.th32ProcessID) == 1) {
            processResult = 1;
            break;
        }
        hasProcess = Process32Next(snapshot, &processEntry);
    }
    CloseHandle(snapshot);

    std::int32_t checksum = 0;
    for (std::size_t slotIndex = 0; slotIndex < 20; ++slotIndex) {
        const std::uint8_t* const slot =
            gProtectionBuffer1 + slotIndex * 0x400;
        if (slot[0] == 0) {
            break;
        }

        for (std::size_t byteIndex = 0; byteIndex < 0x400; ++byteIndex) {
            const std::uint8_t byte = slot[byteIndex];
            if (byte == 0) {
                break;
            }
            checksum += static_cast<std::int32_t>(
                std::bit_cast<std::int8_t>(byte));
        }
    }
    if (gProtectionChecksum1.value() != checksum) {
        return 1;
    }

    checksum = 0;
    for (std::size_t slotIndex = 0; slotIndex < 20; ++slotIndex) {
        const std::uint8_t* const slot =
            gProtectionBuffer2 + slotIndex * 0x400;
        if (slot[0] == 0) {
            break;
        }

        for (std::size_t byteIndex = 0; byteIndex < 0x400; ++byteIndex) {
            const std::uint8_t byte = slot[byteIndex];
            if (byte == 0) {
                break;
            }
            checksum += static_cast<std::int32_t>(
                std::bit_cast<std::int8_t>(byte));
        }
    }
    if (gProtectionChecksum2.value() != checksum) {
        return 1;
    }

    if (network::ReadNetworkRuntimeActive() == 1) {
        const std::int32_t debuggerResult = CheckDebugger_404880();
        if (debuggerResult == 3) {
            static_cast<void>(Report_512CA0(gConnectionSocket, 2));
        } else if (debuggerResult == 1) {
            OutputDebugStringA("Scan Error 2");
            return processResult;
        }
    }

    return processResult;
}

// 原0x404600：508800传入分隔符地址，复制两表并生产共享窗口防护记录。
void ParseProtectionTables_404600(const char* input)
{
    std::memset(gProtectionBuffer1, 0, sizeof(gProtectionBuffer1));
    std::memset(gProtectionBuffer2, 0, sizeof(gProtectionBuffer2));
    (gProtectionChecksum1 = 0);
    (gProtectionChecksum2 = 0);
    gProtectionAuxiliaryRecordCount = 0;
    for (ProtectionAuxiliaryRecord& record : gProtectionAuxiliaryRecords) {
        record.flagA = 0;
        record.flagB = 1;
    }

    std::uint32_t table1Index = 0;
    std::uint32_t table2Index = 0;
    const char* segmentStart = input + 1;
    if (*segmentStart != '\n') {
        while (*segmentStart != '\n' && *segmentStart != '\0') {
            const char* cursor = segmentStart;
            while (*cursor != ','
                   && *cursor != ';'
                   && *cursor != '\n'
                   && *cursor != '\0') {
                ++cursor;
            }

            const std::uint32_t segmentLength =
                static_cast<std::uint32_t>(cursor - segmentStart);
            if (*cursor == ',') {
                std::strncpy(reinterpret_cast<char*>(
                        gProtectionBuffer1 + table1Index * 0x400), segmentStart, static_cast<std::uint32_t>(segmentLength));
                ++table1Index;

                if (gProtectionAuxiliaryRecordCount < 30) {
                    const std::uint32_t recordIndex =
                        gProtectionAuxiliaryRecordCount;
                    ProtectionAuxiliaryRecord& record =
                        gProtectionAuxiliaryRecords[recordIndex];
                    std::memset(record.text, 0, sizeof(record.text));
                    std::strncpy(record.text, segmentStart, static_cast<std::uint32_t>(segmentLength));
                    record.number = 1001 + recordIndex;
                    if (record.flagB == 0) {
                        record.number += 10000;
                    }
                    ++gProtectionAuxiliaryRecordCount;
                }

                segmentStart = cursor + 1;
                continue;
            }

            if (*cursor == ';') {
                std::strncpy(reinterpret_cast<char*>(
                        gProtectionBuffer2 + table2Index * 0x400), segmentStart, static_cast<std::uint32_t>(segmentLength));
                ++table2Index;

                ProtectionAuxiliaryRecord& record =
                    gProtectionAuxiliaryRecords[
                        gProtectionAuxiliaryRecordCount];
                if (cursor[-1] == '1') {
                    record.flagB = 0;
                }
                if (cursor[-1] == '2') {
                    record.flagA = 1;
                }
                if (cursor[-1] == '3') {
                    record.flagB = 0;
                    record.flagA = 1;
                }

                segmentStart = cursor + 1;
                continue;
            }

            std::strncpy(reinterpret_cast<char*>(
                    gProtectionBuffer1 + table1Index * 0x400), segmentStart, static_cast<std::uint32_t>(segmentLength));
            ++table1Index;
            break;
        }
    }

    UpdateProtectionChecksum(&gProtectionChecksum1, gProtectionBuffer1);
    UpdateProtectionChecksum(&gProtectionChecksum2, gProtectionBuffer2);
}

// 原函数：0x004042B0。
int ReleaseProtection_4042B0()
{
    const HMODULE module = gProtectionModule;
    if (module == nullptr) {
        return 0;
    }
    return static_cast<int>(FreeLibrary(module));
}

// 原函数：0x0044B5A0。
std::uint32_t ResetSystemLog_44B5A0()
{
    std::FILE* const file = std::fopen("systemlog.txt", "w");
    if (file == nullptr) {
        return 0;
    }
    return static_cast<std::uint32_t>(std::fclose(file));
}

// 原 0x52CD30：先调用 0x52CCA0，再将 0x103DE80 置 1；不读取实参。
int Sub_52CD30(std::int32_t)
{
    cg::audio::Sub_52CCA0();
    cg::audio::gValue103DE80 = 1;
    return 1;
}

// 原 0x4048D0：依次清零两个共享 W32 状态。
int ResetProtection_4048D0()
{
    gValue59E544 = 0;
    gValue593FD0 = 0;
    return 0;
}

std::uint64_t ReadTickCount64() noexcept
{
    static const std::uint64_t start = ::GetTickCount64();
    return ::GetTickCount64() - start;
}

// 原函数：0x00457010。
void RequestRender_457010() noexcept
{
    gForceRender = 1;
}


// 原 [0x458DC0,0x458DD5)：先发布完整 EAX32，再作无符号 <=0x500 比较。
std::int32_t CheckLegacyDxCapability_458DC0()
{
    const std::uint32_t code = std::bit_cast<std::uint32_t>(cg::audio::Sub_458AE0());
    gDxCapabilityCode73ED38 = code;
    return code <= 0x500u ? 1 : 0;
}

// 原函数：0x004048E0。
std::int32_t CheckWindows_4048E0()
{
    const std::uint32_t counter = gValue593FD0;
    const std::uint32_t incremented = counter + std::uint32_t{1};
    gValue593FD0 = incremented;
    if (std::bit_cast<std::int32_t>(incremented) < 1020) {
        return 0;
    }

    const std::uint64_t tick = ReadTickCount64();
    static_cast<void>(SeedRandom_53B4BF(static_cast<std::uint32_t>(tick)));
    const std::int32_t randomizedCounter =
        static_cast<std::int32_t>(Rand16()) % 300;
    gValue593FD0 = std::bit_cast<std::uint32_t>(randomizedCounter);
    static_cast<void>(EnumWindows(CheckWindowsCallback_404940, 0));

    const std::uint32_t resultBits = gValue59E544;
    const std::int32_t result = std::bit_cast<std::int32_t>(resultBits);
    if (result <= 0) {
        return 0;
    }

    gValue59E544 = 0;
    return result;
}

// 原KeepAlive@[0x457AB0,0x457B03)；CTM01基线／差值64位，零哨兵与60000ms门槛保持。
void KeepAlive_457AB0()
{
    if (network::ReadNetworkRuntimeActive() == 0) {
        return;
    }

    if (gKeepAliveBaselineTick == 0) {
        const std::uint64_t tick = ReadTickCount64();
        gKeepAliveRecentTick = tick;
        gKeepAliveBaselineTick = tick;
        return;
    }

    const std::uint64_t tick = ReadTickCount64();
    const std::uint64_t baselineTick = gKeepAliveBaselineTick;
    const std::uint64_t elapsedBits = tick - baselineTick;
    gKeepAliveRecentTick = tick;
    if (std::bit_cast<std::int64_t>(elapsedBits) < 60000) {
        return;
    }

    const std::uintptr_t socket = gConnectionSocket;
    gKeepAliveBaselineTick = tick;
    static_cast<void>(network::Sub_512C60(socket));
}

// 原函数：0x0053B4BF。
std::uint32_t SeedRandom_53B4BF(std::uint32_t seed)
{
    SharedRandomEngine().seed(seed);
    return seed;
}

// 原函数：0x0053B4C9。
int Rand16()
{
    const std::uint32_t seed = SharedRandomEngine()();
    // 原 sar 后的 0x7FFF 掩码只保留原位 16..30，无符号移位得到相同结果。
    return static_cast<int>((seed >> 16) & 0x7FFFu);
}

// 原 0x46FAF0：上界为 -1 时不消费随机数；否则先取随机数，再计算有符号区间。
std::int32_t RandomRange_46FAF0(std::int32_t lower, std::int32_t upper)
{
    if (upper == -1) {
        return 0;
    }

    const std::int32_t random = Rand16();
    const std::int32_t divisor = upper - lower + 1;
    return random % divisor + lower;
}

// 原 sub_423640@[0x423640,0x4237D5)：更新偏移/幅度后，将共享 Temp 裁剪回拷 Back。
void Sub_423640()
{
    std::uint32_t offsetYBits;
    if (cg::yobi::detail::gYobiUpdateGateMode == 1
        && cg::yobi::detail::gYobiUpdatePhaseCounter != 0) {
        offsetYBits = cg::yobi::detail::gYobiScreenOffsetY;
    } else {
        const std::uint32_t amplitudeX =
            cg::yobi::detail::gYobiScreenOffsetAmplitude;
        const std::uint32_t randomXBits = RandomSymmetricW32(amplitudeX);
        cg::yobi::detail::gYobiScreenOffsetX = randomXBits;

        const std::uint32_t amplitudeY =
            cg::yobi::detail::gYobiScreenOffsetAmplitude;
        offsetYBits = RandomSymmetricW32(amplitudeY);
        cg::yobi::detail::gYobiScreenOffsetY = offsetYBits;
    }

    const std::uint32_t rising =
        cg::yobi::detail::gYobiScreenOffsetRising;
    const std::uint32_t offsetXBits =
        cg::yobi::detail::gYobiScreenOffsetX;
    const std::uint32_t amplitude =
        cg::yobi::detail::gYobiScreenOffsetAmplitude;
    if (rising == 1) {
        const std::uint32_t maximum =
            cg::yobi::detail::gYobiScreenOffsetMaximumAmplitude;
        const std::uint32_t incremented = amplitude + std::uint32_t{1};
        const bool exceedsMaximum =
            std::bit_cast<std::int32_t>(incremented)
            > std::bit_cast<std::int32_t>(maximum);
        cg::yobi::detail::gYobiScreenOffsetAmplitude = incremented;
        if (exceedsMaximum) {
            cg::yobi::detail::gYobiScreenOffsetAmplitude = maximum;
        }
    } else {
        const std::uint32_t decremented = amplitude - std::uint32_t{1};
        cg::yobi::detail::gYobiScreenOffsetAmplitude = decremented;
        if (std::bit_cast<std::int32_t>(decremented) < 0) {
            cg::yobi::detail::gYobiScreenOffsetAmplitude = 0;
        }
    }

    const std::int32_t offsetX = static_cast<std::int16_t>(offsetXBits);
    const std::int32_t offsetY = static_cast<std::int16_t>(offsetYBits);
    std::int32_t destinationX = offsetX;
    std::int32_t destinationY = offsetY;
    std::int32_t sourceLeft = 0;
    std::int32_t sourceTop = 0;
    std::int32_t sourceRight = render::GetCpuSurfaceWidth();
    std::int32_t sourceBottom = render::GetCpuSurfaceHeight();

    if (destinationX < 0) {
        sourceLeft = -destinationX;
        destinationX = 0;
    }
    if (destinationY < 0) {
        sourceTop = -destinationY;
        destinationY = 0;
    }
    if (destinationX >= render::GetCpuSurfaceWidth()
        || destinationY >= render::GetCpuSurfaceHeight()
        || sourceLeft >= sourceRight
        || sourceTop >= sourceBottom) {
        return;
    }

    const std::int32_t availableWidth =
        render::GetCpuSurfaceWidth() - destinationX;
    if (sourceRight - sourceLeft > availableWidth) {
        sourceRight = sourceLeft + availableWidth;
    }
    const std::int32_t availableHeight =
        render::GetCpuSurfaceHeight() - destinationY;
    if (sourceBottom - sourceTop > availableHeight) {
        sourceBottom = sourceTop + availableHeight;
    }
    if (sourceLeft >= sourceRight || sourceTop >= sourceBottom) {
        return;
    }

    static_cast<void>(render::BlitTempToBack(
        destinationX,
        destinationY,
        sourceLeft,
        sourceTop,
        sourceRight,
        sourceBottom));
}

// 原函数：0x004589D0。
std::uint64_t ReadSystemSeconds_4589D0()
{
    SYSTEMTIME systemTime;
    FILETIME fileTime;
    GetSystemTime(&systemTime);
    SystemTimeToFileTime(&systemTime, &fileTime);
    const std::uint32_t high = fileTime.dwHighDateTime;
    const std::uint32_t low = fileTime.dwLowDateTime;
    // 原53CB10：保持U64整除，直接由MSVC生成对应运算。
    return ((static_cast<std::uint64_t>(high) << 32) | low) / 10000000u;
}

// 原函数：0x00458A10。
int CheckSystemSeconds_458A10(std::int32_t seconds)
{
    const std::uint64_t now = ReadSystemSeconds_4589D0();
    const std::uint64_t previous =
        (static_cast<std::uint64_t>(gLastSystemTime.high) << 32) | gLastSystemTime.low;
    const std::uint64_t elapsed = now - previous;
    return std::bit_cast<std::int64_t>(elapsed) >= seconds && seconds > 0 ? 1 : 0;
}

} // namespace cg::platform
