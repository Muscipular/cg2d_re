#include "ChatHistory.hpp"
#include "cg/session/ClientSession.hpp"
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
#include "../scene/login/LoginSavedState.hpp"

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

namespace cg::world::detail {

// 文件边界：64项GBK记录和尾索引保持原磁盘布局。
struct ChatHistoryDisk {
    char records[64][0x121];
    std::uint32_t tail;
};
static_assert(std::is_trivially_copyable_v<ChatHistoryDisk>);
static_assert(offsetof(ChatHistoryDisk, tail) == 0x4840);
static_assert(sizeof(ChatHistoryDisk) == 0x4844);

static ChatHistoryDisk EncodeChatHistory(const ChatInputHistoryState& history)
{
    ChatHistoryDisk disk{};
    for (std::size_t i = 0; i < history.records.size(); ++i)
        text::StoreGbk(disk.records[i], sizeof(disk.records[i]), history.records[i]);
    disk.tail = history.tail626838;
    return disk;
}

static void DecodeChatHistory(const ChatHistoryDisk& disk, ChatInputHistoryState& history)
{
    for (std::size_t i = 0; i < history.records.size(); ++i)
        history.records[i] = text::FromGbkWide(disk.records[i]);
    history.tail626838 = disk.tail;
}

// 原 [0x44A090,0x44A15B)：保留短读已写入的字节及 rb 失败后创建文件的顺序。
std::int32_t LoadChatInputHistory_44A090(session::ClientSession& session)
{
    auto& history = session.WorldScene().chatInputHistory;
    history.cursor62683C = -1;
    const auto* const directory = login::detail::ReadSelectedSavedDirectory(session);
    if (directory[0] == L'\0') return 0;
    const auto path = std::format(L"{}\\chathis.dat", directory);

    auto disk = EncodeChatHistory(history); // 先保存旧值，短读仍保留未覆盖的历史。
    std::FILE* const readStream = _wfopen(path.c_str(), L"rb");
    if (readStream != nullptr) {
        const std::uint32_t count =
            static_cast<std::uint32_t>(std::fread(&disk, sizeof(disk), 1, readStream));
        std::fclose(readStream);
        DecodeChatHistory(disk, history);
        return count >= 1 ? 1 : 0;
    }

    std::FILE* const writeStream = _wfopen(path.c_str(), L"wb");
    if (writeStream != nullptr) {
        std::fwrite(&disk, sizeof(disk), 1, writeStream);
        std::fclose(writeStream);
    }
    history.cursor62683C = 63;
    return 0;
}

// 原 sub_449DA0@[0x449DA0,0x449F9C)：输入记录与聊天日志初始化。
void __cdecl InitializeChatInputAndLog_449DA0(session::ClientSession& session)
{
    auto& chatLog = session.WorldScene().chatLog;
    auto& chatDisplay = session.WorldScene().chatDisplay;
    auto& inputs = session.UiFrame().native.inputs;
    for (auto& record : chatDisplay.records) record = {};
    inputs.chat.reset();
    input::InitializeContext_449CF0(
        session,
        &inputs.chat, 8, 440, 0, 1, 0, {}, 1, 78, 0, 0);
    inputs.chatDefaults = inputs.chat;

    for (std::size_t index = 0; index < 19; ++index) {
        chatDisplay.records[index].state = 1;
    }
    chatDisplay.rangeOverlayFrames = 0;

    __time64_t timeValue = 0;
    std::FILE* const existingStream = chatLog.stream;
    if (existingStream != nullptr) {
        return;
    }

    ::_wmkdir(L"Log");
    timeValue = ::_time64(nullptr);
    const std::tm* const legacyTime =
        session.LocalTime(&timeValue);
    const std::int32_t fileMonth = legacyTime->tm_mon;
    const std::int32_t fileDay = legacyTime->tm_mday;
    const std::int32_t fileYear = legacyTime->tm_year;
    chatLog.fileName = std::format(L"chat_{:02}{:02}{:02}.txt", fileYear % 100, fileMonth + 1, fileDay);
    chatLog.path = std::format(L"Log\\{}", chatLog.fileName);

    std::FILE* const readStream =
        _wfopen(chatLog.path.c_str(), L"r" /* 原常量参考：0x00554908。 */);
    chatLog.stream = readStream;
    std::FILE* gate;
    if (readStream == nullptr) {
        std::FILE* const appendStream =
            _wfopen(chatLog.path.c_str(), L"a");
        chatLog.stream = appendStream;
        std::fprintf(appendStream, "        >>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<\n" /* 原常量参考：0x005548CC。 */);

        const std::int32_t titleMonth = legacyTime->tm_mon;
        const std::int32_t titleDay = legacyTime->tm_mday;
        const std::int32_t titleYear = legacyTime->tm_year;
        std::fprintf(
            chatLog.stream,
            cg::text::ToGbk(L"        >>  魔 力 宝 贝 冒 险 者 日 志 [ %02d/%02d/%02d ]  <<\n" /* 原常量参考：0x0055488C。 */).c_str(),
            titleYear % 100, titleMonth + 1, titleDay);
        std::fprintf(chatLog.stream, "        >>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<\n" /* 原常量参考：0x005548CC。 */);
        gate = chatLog.stream;
    } else {
        std::fclose(readStream);
        std::FILE* const appendStream =
            _wfopen(chatLog.path.c_str(), L"a");
        chatLog.stream = appendStream;
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
            chatLog.stream,
            cg::text::ToGbk(L" ■ %02d/%02d/%02d %02d:%02d:%02d 纪录\n" /* 原常量参考：0x00554820。 */).c_str(),
            sessionYear % 100, sessionMonth + 1, sessionDay,
            sessionHour, sessionMinute, sessionSecond);
        std::fprintf(chatLog.stream, "  -------------------------------------------------------------\n" /* 原常量参考：0x005547DC。 */);
    }
    std::fflush(chatLog.stream);
}

// 原 sub_4A76E0@[0x4A76E0,0x4A76FD)：先写未封顶值，超过 512 再写封顶值。
std::int32_t IncrementChatRecordCount_4A76E0(session::ClientSession& session)
{
    auto& chatDisplay = session.WorldScene().chatDisplay;
    const std::int32_t increased = chatDisplay.count + 1;
    chatDisplay.count = increased;
    if (increased > 512) {
        chatDisplay.count = 512;
    }
    return increased;
}

} // namespace cg::world::detail

namespace cg::world {

// 原 [0x449FA0,0x44A084)：按索引原位写入聊天历史记录及尾部 W32。
std::int32_t SaveChatInputHistoryEntry_449FA0(session::ClientSession& session, const std::int32_t index)
{
    const auto* const directory = login::detail::ReadSelectedSavedDirectory(session);
    if (directory[0] == L'\0') return 0;
    const auto path = std::format(L"{}\\chathis.dat", directory);

    std::FILE* const stream = _wfopen(path.c_str(), L"r+b");
    if (stream == nullptr) {
        return 0;
    }

    ::_fseeki64(stream, static_cast<std::int64_t>(static_cast<std::uint32_t>(index * 0x121)), SEEK_SET);
    char diskRecord[0x121]{};
    text::StoreGbk(diskRecord, sizeof(diskRecord), session.WorldScene().chatInputHistory.records[index]);
    if (static_cast<std::uint32_t>(std::fwrite(diskRecord, sizeof(diskRecord), 1, stream)) < 1) {
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
    session::ClientSession& session,
    const std::wstring_view message,
    const std::int32_t color,
    const std::int32_t fontType)
{
    auto& chatLog = session.WorldScene().chatLog;
    auto& chatDisplay = session.WorldScene().chatDisplay;
    if (color >= 13) {
        return;
    }

    // 显示记录只持有宽文本；日志GBK字节限制在文件写入边界检查。
    const std::FILE* const gate = chatLog.stream;
    std::string logBytes;
    if (gate != nullptr) {
        try {
            logBytes = text::ToGbk(message);
            if (logBytes.size() >= 0x124)
                throw text::GbkStorageError(122, std::system_category(), "文字超过聊天日志GBK容量");
        } catch (const std::system_error&) {
            m_log_iw(L"这项文字无法保存为GBK，或超过字段容量。");
            return;
        }
    }
    std::int32_t index = chatDisplay.writeIndex;
    chatDisplay.records[index].text = message;

    if (gate != nullptr) {
        __time64_t timeValue = color;
        timeValue = ::_time64(nullptr);
        const std::tm* const legacyTime =
            session.LocalTime(&timeValue);
        index = chatDisplay.writeIndex;
        const std::int32_t second = legacyTime->tm_sec;
        const std::int32_t minute = legacyTime->tm_min;
        const std::int32_t hour = legacyTime->tm_hour;

        std::fprintf(
            chatLog.stream,
            " %02d:%02d:%02d\x81\x40%s\n",
            hour, minute, second, logBytes.c_str());
        std::fclose(chatLog.stream);
        std::FILE* const newStream = _wfopen(chatLog.path.c_str(), L"a");
        index = chatDisplay.writeIndex;
        chatLog.stream = newStream;
    }

    Record5FB7F8& record = chatDisplay.records[index];
    record.color = color;
    record.fontType = render::ResolveFontId(fontType);
    record.state = 0;
    const std::int32_t nextIndex = index + 1;
    chatDisplay.writeIndex = nextIndex;
    if (nextIndex >= 512) {
        chatDisplay.writeIndex = 0;
    }
    detail::IncrementChatRecordCount_4A76E0(session);
    chatDisplay.lifetime = 20;
}

} // namespace cg::world
