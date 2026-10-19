#include "ChatHistory.hpp"
#include "../ui/WindowRuntimeState.hpp"
#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/render/Font.hpp"
#include "cg/world/Blacklist.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include "../input/InputKeyDispatchDependencies.hpp"
#include "../platform/WindowProcedureState.hpp"

#include <ctime>
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

#include "cg/core/m_log.hpp"

namespace cg::world {

Record5FB7F8 gRecords5FB7F8[512] {}; // 原全局：0x005FB7F8。
volatile std::int32_t gChatWriteIndex_626C08 = 0; // 原全局：0x00626C08。

} // namespace cg::world

namespace cg::world::detail {
char gLogPath62699C[260] {}; // 原全局：0x0062699C。
std::FILE* volatile gLogStream626C1C = nullptr; // 原全局：0x00626C1C。

} // namespace cg::world::detail

namespace cg::world {
volatile std::int32_t gChatRecordCount_C9A944 = 0; // 原全局：0x00C9A944。

} // namespace cg::world

namespace cg::world::detail {
volatile std::uint32_t gValue626C10 = 0; // 原全局：0x00626C10。

} // namespace cg::world::detail

namespace cg::world {
input::InputContext gInputContext626840 {}; // 原全局：0x00626840。

} // namespace cg::world

namespace cg::world {
std::uint32_t gChatRangeOverlayFrames_626BFC = 0; // 原全局：0x00626BFC。
char gLogFileName554648[16] = "chat_000000.txt"; // 原全局：0x00554648。

} // namespace cg::world

namespace cg::world::detail {

ChatHistory621FF8 gChatHistory621FF8 {}; // 原全局：0x00621FF8。
std::int32_t gCursor62683C = 0; // 原全局：0x0062683C。

static_assert(sizeof(gRecords5FB7F8) == 512 * 0x134);
static_assert(std::is_standard_layout_v<ChatHistory621FF8>);
static_assert(std::is_trivially_copyable_v<ChatHistory621FF8>);
static_assert(sizeof(gChatHistory621FF8.records[0]) == 0x121);
static_assert(sizeof(gChatHistory621FF8.records) == 0x4840);
static_assert(offsetof(ChatHistory621FF8, tail626838) == 0x4840);
static_assert(sizeof(ChatHistory621FF8) == 0x4844);

static constexpr char kLogDirectory_554658[] = "Log"; // 原常量参考：0x00554658。
static constexpr char kPathFormat_553514[] = "%s\\%s"; // 原常量参考：0x00553514。
static constexpr char kAppendMode_554904[] = "a"; // 原常量参考：0x00554904。
static constexpr char kChatHistoryName554910[] = "chathis.dat"; // 原常量参考：0x00554910。

// 原 [0x44A090,0x44A15B)：保留短读已写入的字节及 rb 失败后创建文件的顺序。
std::int32_t LoadChatInputHistory_44A090()
{
    gCursor62683C = -1;
    char path[128];
    std::sprintf(
        path, kPathFormat_553514, platform::gDataPath, kChatHistoryName554910);

    std::FILE* const readStream = std::fopen(path, "rb");
    if (readStream != nullptr) {
        const std::uint32_t count =
            static_cast<std::uint32_t>(std::fread(&gChatHistory621FF8, 0x4844, 1, readStream));
        std::fclose(readStream);
        return count >= 1 ? 1 : 0;
    }

    std::FILE* const writeStream = std::fopen(path, "wb");
    if (writeStream != nullptr) {
        std::fwrite(&gChatHistory621FF8, 0x4844, 1, writeStream);
        std::fclose(writeStream);
    }
    gCursor62683C = 63;
    return 0;
}

// 原 sub_449DA0@[0x449DA0,0x449F9C)：输入记录与聊天日志初始化。
void __cdecl InitializeChatInputAndLog_449DA0()
{
    std::memset(gRecords5FB7F8, 0, sizeof(gRecords5FB7F8));
    cg::ui::detail::gInputContext626AA0.reset();
    input::InitializeContext_449CF0(
        &cg::ui::detail::gInputContext626AA0, 8, 440, 0, 1, 0, {}, 1, 78, 0, 0);
    gInputContext626840 = cg::ui::detail::gInputContext626AA0;

    for (std::size_t index = 0; index < 19; ++index) {
        gRecords5FB7F8[index].state = 1;
    }
    gChatRangeOverlayFrames_626BFC = 0;

    __time64_t timeValue = 0;
    std::FILE* const existingStream = gLogStream626C1C;
    if (existingStream != nullptr) {
        return;
    }

    ::_mkdir(kLogDirectory_554658);
    timeValue = ::_time64(nullptr);
    const std::tm* const legacyTime =
        ::_localtime64(&timeValue);
    const std::int32_t fileMonth = legacyTime->tm_mon;
    const std::int32_t fileDay = legacyTime->tm_mday;
    const std::int32_t fileYear = legacyTime->tm_year;
    std::sprintf(
        gLogFileName554648,
        "chat_%02d%02d%02d.txt" /* 原常量参考：0x0055351C。 */,
        fileYear % 100, fileMonth + 1, fileDay);
    std::sprintf(
        gLogPath62699C,
        kPathFormat_553514,
        kLogDirectory_554658,
        gLogFileName554648);

    std::FILE* const readStream =
        std::fopen(gLogPath62699C, "r" /* 原常量参考：0x00554908。 */);
    gLogStream626C1C = readStream;
    std::FILE* gate;
    if (readStream == nullptr) {
        std::FILE* const appendStream =
            std::fopen(gLogPath62699C, kAppendMode_554904);
        gLogStream626C1C = appendStream;
        std::fprintf(appendStream, "        >>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<\n" /* 原常量参考：0x005548CC。 */);

        const std::int32_t titleMonth = legacyTime->tm_mon;
        const std::int32_t titleDay = legacyTime->tm_mday;
        const std::int32_t titleYear = legacyTime->tm_year;
        std::fprintf(
            gLogStream626C1C,
            cg::text::ToGbk(L"        >>  魔 力 宝 贝 冒 险 者 日 志 [ %02d/%02d/%02d ]  <<\n" /* 原常量参考：0x0055488C。 */).c_str(),
            titleYear % 100, titleMonth + 1, titleDay);
        std::fprintf(gLogStream626C1C, "        >>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<\n" /* 原常量参考：0x005548CC。 */);
        gate = gLogStream626C1C;
    } else {
        std::fclose(readStream);
        std::FILE* const appendStream =
            std::fopen(gLogPath62699C, kAppendMode_554904);
        gLogStream626C1C = appendStream;
        gate = appendStream;
    }

    if (gate != nullptr) {
        std::fprintf(gate, "\n  ------------------------------------------------------------\n" /* 原常量参考：0x00554848。 */);

        const std::int32_t sessionSecond = legacyTime->tm_sec;
        const std::int32_t sessionMinute = legacyTime->tm_min;
        const std::int32_t sessionHour = legacyTime->tm_hour;
        const std::int32_t sessionDay = legacyTime->tm_mday;
        const std::int32_t sessionMonth = legacyTime->tm_mon;
        const std::int32_t sessionYear = legacyTime->tm_year;
        std::fprintf(
            gLogStream626C1C,
            cg::text::ToGbk(L" ■ %02d/%02d/%02d %02d:%02d:%02d 纪录\n" /* 原常量参考：0x00554820。 */).c_str(),
            sessionYear % 100, sessionMonth + 1, sessionDay,
            sessionHour, sessionMinute, sessionSecond);
        std::fprintf(gLogStream626C1C, "  -------------------------------------------------------------\n" /* 原常量参考：0x005547DC。 */);
    }
    std::fflush(gLogStream626C1C);
}

// 原 sub_4A76E0@[0x4A76E0,0x4A76FD)：先写未封顶值，超过 512 再写封顶值。
std::int32_t IncrementChatRecordCount_4A76E0()
{
    const std::int32_t increased = gChatRecordCount_C9A944 + 1;
    gChatRecordCount_C9A944 = increased;
    if (increased > 512) {
        gChatRecordCount_C9A944 = 512;
    }
    return increased;
}

} // namespace cg::world::detail

namespace cg::world {

// 原 [0x449FA0,0x44A084)：按索引原位写入聊天历史记录及尾部 W32。
std::int32_t SaveChatInputHistoryEntry_449FA0(const std::int32_t index)
{
    char path[128];
    std::sprintf(
        path, detail::kPathFormat_553514, platform::gDataPath, detail::kChatHistoryName554910);

    std::FILE* const stream = std::fopen(path, "r+b");
    if (stream == nullptr) {
        return 0;
    }

    ::_fseeki64(stream, static_cast<std::int64_t>(static_cast<std::uint32_t>(index * 0x121)), SEEK_SET);
    if (static_cast<std::uint32_t>(std::fwrite(detail::gChatHistory621FF8.records[index], 0x121, 1, stream)) < 1) {
        std::fclose(stream);
        return 0;
    }

    ::_fseeki64(stream, 0x4840, SEEK_SET);
    const std::uint32_t written = static_cast<std::uint32_t>(std::fwrite(&index, static_cast<std::uint32_t>(sizeof(index)), 1, stream));
    std::fclose(stream);
    return written >= 1 ? 1 : 0;
}

// 原 sub_44B050@[0x44B050,0x44B170)。
void AppendChatMessage_44B050(
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
        m_log_iw(L"这项文字无法保存为GBK，或超过字段容量。");
        return;
    }
    const char* const text = stored.c_str();
    std::int32_t index = gChatWriteIndex_626C08;
    char* const destination = gRecords5FB7F8[index].text;
    const std::size_t length = std::strlen(text) + 1;
    const std::size_t wordCount = length / sizeof(std::uint32_t);
    for (std::size_t word = 0; word < wordCount; ++word) {
        const std::size_t offset = word * sizeof(std::uint32_t);
        std::uint32_t copied;
        std::memcpy(&copied, text + offset, sizeof(copied));
        std::memcpy(destination + offset, &copied, sizeof(copied));
    }

    const std::FILE* const gate = detail::gLogStream626C1C;
    for (std::size_t offset = wordCount * sizeof(std::uint32_t);
         offset < length; ++offset) {
        destination[offset] = text[offset];
    }

    if (gate != nullptr) {
        __time64_t timeValue = color;
        timeValue = ::_time64(nullptr);
        const std::tm* const legacyTime =
            ::_localtime64(&timeValue);
        index = gChatWriteIndex_626C08;
        const std::int32_t second = legacyTime->tm_sec;
        const std::int32_t minute = legacyTime->tm_min;
        const std::int32_t hour = legacyTime->tm_hour;

        std::fprintf(
            detail::gLogStream626C1C,
            " %02d:%02d:%02d\x81\x40%s\n",
            hour, minute, second, gRecords5FB7F8[index].text);
        std::fclose(detail::gLogStream626C1C);
        std::FILE* const newStream = std::fopen(detail::gLogPath62699C, "a");
        index = gChatWriteIndex_626C08;
        detail::gLogStream626C1C = newStream;
    }

    Record5FB7F8& record = gRecords5FB7F8[index];
    record.color = color;
    record.fontType = fontType;
    record.state = 0;
    const std::int32_t nextIndex = index + 1;
    gChatWriteIndex_626C08 = nextIndex;
    if (nextIndex >= 512) {
        gChatWriteIndex_626C08 = 0;
    }
    detail::IncrementChatRecordCount_4A76E0();
    detail::gValue626C10 = 20;
}

} // namespace cg::world
