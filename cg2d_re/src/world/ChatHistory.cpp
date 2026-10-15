#include "ChatHistory.hpp"
#include "../ui/WindowRuntimeState.hpp"
#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/render/Font.hpp"
#include "cg/world/Blacklist.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include "../input/InputKeyDispatchDependencies.hpp"
#include "../platform/LegacyFormat.hpp"
#include "../platform/LegacyStdio.hpp"
#include "../platform/WindowProcedureState.hpp"
#include "WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <cstring>
#include <format>
#include <type_traits>
#include <algorithm>
#include <string>
#include <system_error>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BB02、0x0053BC41、0x0053BCCD、0x0053C680；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。
#include <cctype>
#include <direct.h> // 原CRT接口：0x0053C654、0x0053BEC0、0x0053C790、0x0053BE40、0x0053BE17、0x0053BDEF；原参数／返回合同由直接调用承接。


namespace cg::world::detail {

Record5FB7F8 gRecords5FB7F8[512] {}; // 原全局：0x005FB7F8。
volatile std::int32_t gWriteIndex626C08 = 0; // 原全局：0x00626C08。
char gLogPath62699C[260] {}; // 原全局：0x0062699C。
platform::LegacyFile* volatile gLogStream626C1C = nullptr; // 原全局：0x00626C1C。
volatile std::int32_t gCountC9A944 = 0; // 原全局：0x00C9A944。
volatile std::uint32_t gValue626C10 = 0; // 原全局：0x00626C10。
input::InputContext gInputContext626840 {}; // 原全局：0x00626840。
std::uint32_t gValue626C18 = 0; // 原全局：0x00626C18。
std::uint32_t gValue626BFC = 0; // 原全局：0x00626BFC。
char gLogFileName554648[16] = "chat_000000.txt"; // 原全局：0x00554648。



ChatHistory621FF8 gChatHistory621FF8 {}; // 原全局：0x00621FF8。
std::int32_t gCursor62683C = 0; // 原全局：0x0062683C。

static_assert(sizeof(gRecords5FB7F8) == 512 * 0x134);
static_assert(std::is_standard_layout_v<ChatHistory621FF8>);
static_assert(std::is_trivially_copyable_v<ChatHistory621FF8>);
static_assert(sizeof(gChatHistory621FF8.records[0]) == 0x121);
static_assert(sizeof(gChatHistory621FF8.records) == 0x4840);
static_assert(offsetof(ChatHistory621FF8, tail626838) == 0x4840);
static_assert(sizeof(ChatHistory621FF8) == 0x4844);

namespace {

constexpr char kLogDirectory_554658[] = "Log"; // 原常量参考：0x00554658。
constexpr char kPathFormat_553514[] = "%s\\%s"; // 原常量参考：0x00553514。
constexpr char kAppendMode_554904[] = "a"; // 原常量参考：0x00554904。
constexpr char kChatHistoryName554910[] = "chathis.dat"; // 原常量参考：0x00554910。




} // namespace

// 原 [0x44A090,0x44A15B)：保留短读已写入的字节及 rb 失败后创建文件的顺序。
std::int32_t Sub_44A090()
{
    gCursor62683C = -1;
    char path[128];
    std::sprintf(
        path, kPathFormat_553514, platform::gDataPath, kChatHistoryName554910);

    platform::LegacyFile* const readStream = std::fopen(path, "rb");
    if (readStream != nullptr) {
        const std::uint32_t count =
            static_cast<std::uint32_t>(std::fread(&gChatHistory621FF8, 0x4844, 1, readStream));
        std::fclose(readStream);
        return count >= 1 ? 1 : 0;
    }

    platform::LegacyFile* const writeStream = std::fopen(path, "wb");
    if (writeStream != nullptr) {
        static_cast<std::uint32_t>(std::fwrite(&gChatHistory621FF8, 0x4844, 1, writeStream));
        std::fclose(writeStream);
    }
    gCursor62683C = 63;
    return 0;
}

// 原 [0x449FA0,0x44A084)：按索引原位写入聊天历史记录及尾部 W32。
std::int32_t Sub_449FA0(const std::int32_t index)
{
    char path[128];
    std::sprintf(
        path, kPathFormat_553514, platform::gDataPath, kChatHistoryName554910);

    platform::LegacyFile* const stream = std::fopen(path, "r+b");
    if (stream == nullptr) {
        return 0;
    }

    std::fseek(stream, static_cast<std::int32_t>(index * 0x121), 0);
    if (static_cast<std::uint32_t>(std::fwrite(gChatHistory621FF8.records[index], 0x121, 1, stream)) < 1) {
        std::fclose(stream);
        return 0;
    }

    std::fseek(stream, 0x4840, 0);
    const std::uint32_t written = static_cast<std::uint32_t>(std::fwrite(&index, static_cast<std::uint32_t>(sizeof(index)), 1, stream));
    std::fclose(stream);
    return written >= 1 ? 1 : 0;
}

// 原 sub_449DA0@[0x449DA0,0x449F9C)：输入记录与聊天日志初始化。
void __cdecl Sub_449DA0()
{
    std::memset(gRecords5FB7F8, 0, sizeof(gRecords5FB7F8));
    cg::ui::detail::gInputContext626AA0.reset();
    input::InitializeContext_449CF0(
        &cg::ui::detail::gInputContext626AA0, 8, 440, 0, 1, 0, {}, 1, 78, 0, 0);
    gInputContext626840 = cg::ui::detail::gInputContext626AA0;

    for (std::size_t index = 0; index < 19; ++index) {
        gRecords5FB7F8[index].state = 1;
    }
    gValue626C18 = 0;
    gValue626BFC = 0;

    std::int32_t timeValue = 0;
    platform::LegacyFile* const existingStream = gLogStream626C1C;
    if (existingStream != nullptr) {
        return;
    }

    ::_mkdir(kLogDirectory_554658);
    platform::ReadLegacyTime32_53BA26(&timeValue);
    const platform::LegacyTm53B8C6* const legacyTime =
        platform::LocalTime_53B8C6(&timeValue);
    const auto* const timeBytes =
        reinterpret_cast<const unsigned char*>(legacyTime);
    const auto readTimeField = [timeBytes](const std::size_t offset) {
        std::int32_t value;
        std::memcpy(&value, timeBytes + offset, sizeof(value));
        return value;
    };

    const std::int32_t fileMonth = readTimeField(0x10);
    const std::int32_t fileDay = readTimeField(0x0C);
    const std::int32_t fileYear = readTimeField(0x14);
    std::sprintf(
        gLogFileName554648,
        "chat_%02d%02d%02d.txt" /* 原常量参考：0x0055351C。 */,
        fileYear % 100, fileMonth + 1, fileDay);
    std::sprintf(
        gLogPath62699C,
        kPathFormat_553514,
        kLogDirectory_554658,
        gLogFileName554648);

    platform::LegacyFile* const readStream =
        std::fopen(gLogPath62699C, "r" /* 原常量参考：0x00554908。 */);
    gLogStream626C1C = readStream;
    platform::LegacyFile* gate;
    if (readStream == nullptr) {
        platform::LegacyFile* const appendStream =
            std::fopen(gLogPath62699C, kAppendMode_554904);
        gLogStream626C1C = appendStream;
        std::fprintf(appendStream, "        >>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<\n" /* 原常量参考：0x005548CC。 */);

        const std::int32_t titleMonth = readTimeField(0x10);
        const std::int32_t titleDay = readTimeField(0x0C);
        const std::int32_t titleYear = readTimeField(0x14);
        std::fprintf(
            gLogStream626C1C,
            cg::text::ToGbk(LR"(        >>  魔 力 寶 貝 冒 險 者 日 誌 [ %02d/%02d/%02d ]  <<
)" /* 原常量参考：0x0055488C。 */).c_str(),
            titleYear % 100, titleMonth + 1, titleDay);
        std::fprintf(gLogStream626C1C, "        >>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<\n" /* 原常量参考：0x005548CC。 */);
        gate = gLogStream626C1C;
    } else {
        std::fclose(readStream);
        platform::LegacyFile* const appendStream =
            std::fopen(gLogPath62699C, kAppendMode_554904);
        gLogStream626C1C = appendStream;
        gate = appendStream;
    }

    if (gate != nullptr) {
        std::fprintf(gate, "\n  ------------------------------------------------------------\n" /* 原常量参考：0x00554848。 */);

        const std::int32_t sessionSecond = readTimeField(0x00);
        const std::int32_t sessionMinute = readTimeField(0x04);
        const std::int32_t sessionHour = readTimeField(0x08);
        const std::int32_t sessionDay = readTimeField(0x0C);
        const std::int32_t sessionMonth = readTimeField(0x10);
        const std::int32_t sessionYear = readTimeField(0x14);
        std::fprintf(
            gLogStream626C1C,
            cg::text::ToGbk(LR"( ■ %02d/%02d/%02d %02d:%02d:%02d 紀錄
)" /* 原常量参考：0x00554820。 */).c_str(),
            sessionYear % 100, sessionMonth + 1, sessionDay,
            sessionHour, sessionMinute, sessionSecond);
        std::fprintf(gLogStream626C1C, "  -------------------------------------------------------------\n" /* 原常量参考：0x005547DC。 */);
    }
    std::fflush(gLogStream626C1C);
}

// 原 sub_4A76E0@[0x4A76E0,0x4A76FD)：先写未封顶值，超过 512 再写封顶值。
std::int32_t Sub_4A76E0()
{
    const std::int32_t increased = gCountC9A944 + 1;
    gCountC9A944 = increased;
    if (increased > 512) {
        gCountC9A944 = 512;
    }
    return increased;
}

// 原 sub_44B050@[0x44B050,0x44B170)。
void Sub_44B050(
    const std::wstring_view message,
    const std::int32_t color,
    const std::int32_t fontType)
{
    if (color >= 13) {
        return;
    }
    if (fontType >= 14) {
        return;
    }

    std::string stored;
    try {
        stored = cg::text::ToGbk(message);
        if (stored.size() >= sizeof(gRecords5FB7F8[0].text)) {
            throw cg::text::GbkStorageError(122, std::system_category(), "文字超过聊天记录GBK容量");
        }
    }
    catch (const std::system_error&) {
        platform::ShowWideMessage(L"这项文字无法保存为GBK，或超过字段容量。", L"文字编码", 0x10U);
        return;
    }
    const char* const text = stored.c_str();
    std::int32_t index = gWriteIndex626C08;
    char* const destination = gRecords5FB7F8[index].text;
    const std::size_t length = std::strlen(text) + 1;
    const std::size_t wordCount = length / sizeof(std::uint32_t);
    for (std::size_t word = 0; word < wordCount; ++word) {
        const std::size_t offset = word * sizeof(std::uint32_t);
        std::uint32_t copied;
        std::memcpy(&copied, text + offset, sizeof(copied));
        std::memcpy(destination + offset, &copied, sizeof(copied));
    }

    const platform::LegacyFile* const gate = gLogStream626C1C;
    for (std::size_t offset = wordCount * sizeof(std::uint32_t);
         offset < length; ++offset) {
        destination[offset] = text[offset];
    }

    if (gate != nullptr) {
        std::int32_t timeValue = color;
        platform::ReadLegacyTime32_53BA26(&timeValue);
        const platform::LegacyTm53B8C6* const legacyTime =
            platform::LocalTime_53B8C6(&timeValue);
        index = gWriteIndex626C08;
        const auto* const timeBytes =
            reinterpret_cast<const unsigned char*>(legacyTime);
        std::int32_t second;
        std::int32_t minute;
        std::int32_t hour;
        std::memcpy(&second, timeBytes, sizeof(second));
        std::memcpy(&minute, timeBytes + 4, sizeof(minute));
        std::memcpy(&hour, timeBytes + 8, sizeof(hour));

        std::fprintf(
            gLogStream626C1C,
            " %02d:%02d:%02d\x81\x40%s\n",
            hour, minute, second, gRecords5FB7F8[index].text);
        std::fclose(gLogStream626C1C);
        platform::LegacyFile* const newStream = std::fopen(gLogPath62699C, "a");
        index = gWriteIndex626C08;
        gLogStream626C1C = newStream;
    }

    Record5FB7F8& record = gRecords5FB7F8[index];
    record.color = color;
    record.fontType = fontType;
    record.state = 0;
    const std::int32_t nextIndex = index + 1;
    gWriteIndex626C08 = nextIndex;
    if (nextIndex >= 512) {
        gWriteIndex626C08 = 0;
    }
    Sub_4A76E0();
    gValue626C10 = 20;
}

} // namespace cg::world::detail
