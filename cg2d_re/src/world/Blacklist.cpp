#include "cg/world/Blacklist.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/text/Encoding.hpp"
#include <system_error>
#include "cg/input/Input.hpp"

#include <bit>
#include <cstring>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B226、0x0053B27D、0x0053B2F1、0x0053B356；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::world::blacklist {

// 原函数：0x004011A0。
static std::int8_t PersistBlacklistFile_4011A0(session::ClientSession& session, const std::int32_t fileId)
{
    const auto localPath = std::format(L"data\\blacklist_{}.txt", fileId);

    if (session.WorldScene().blacklist.path != localPath) {
        return 0;
    }

    std::FILE* const file = _wfopen(session.WorldScene().blacklist.path.c_str(), L"w");
    for (const Record& record : session.WorldScene().blacklist.records) {
        if (record[0] == 0) {
            break;
        }

        std::fputs(cg::text::ToGbk(record).c_str(), file);
        std::fputs("\n", file);
    }

    std::fclose(file);
    return std::int8_t {1};
}

// 原函数：0x00401030。
std::int8_t LoadBlacklistFile(session::ClientSession& session, const std::int32_t fileId)
{
    session.WorldScene().blacklist.path.clear();
    session.WorldScene().blacklist.records.fill({});
    session.WorldScene().blacklist.path = std::format(L"data\\blacklist_{}.txt", fileId);

    std::FILE* const file = _wfopen(session.WorldScene().blacklist.path.c_str(), L"rb+");
    if (file != nullptr) {
        std::rewind(file);
        if (std::feof(file) == 0) {
            for (;;) {
                if (session.WorldScene().blacklist.cursor < 0 || session.WorldScene().blacklist.cursor >= static_cast<std::int32_t>(kRecordCount)) { break; }
                char record[kRecordSize]{};
                std::fgets(record, static_cast<int>(kRecordSize), file);

                const std::int32_t cursor = session.WorldScene().blacklist.cursor;
                if (record[0] == 0
                    || record[0] == '\n'
                    || (record[0] == '\r' && record[1] == '\n')) {
                    const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(cursor);
                    session.WorldScene().blacklist.cursor = std::bit_cast<std::int32_t>(cursorBits - std::uint32_t {1});
                    session.WorldScene().blacklist.records[static_cast<std::size_t>(cursor)].clear();
                } else {
                    for (std::size_t i = 0; i + 1 < kRecordSize; ++i) {
                        if (record[i] == 0) {
                            break;
                        }
                        if (record[i] == '\r' && record[i + 1] == '\n') {
                            record[i] = 0;
                            record[i + 1] = 0;
                            break;
                        }
                        if (record[i] == '\n') {
                            record[i] = 0;
                        }
                    }

                    session.WorldScene().blacklist.records[static_cast<std::size_t>(cursor)] = cg::text::FromGbkWide(record);
                    const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(cursor);
                    session.WorldScene().blacklist.cursor = std::bit_cast<std::int32_t>(cursorBits + std::uint32_t {1});
                    if (session.WorldScene().blacklist.cursor == static_cast<std::int32_t>(kRecordCount)) {
                        break;
                    }
                }

                if (std::feof(file) != 0) {
                    break;
                }
            }
        }
        std::fclose(file);
    }

    session.WorldScene().blacklist.currentFileId = fileId;
    return std::int8_t {1};
}

// 原函数：0x00401000。
std::int8_t PersistAndReset_401000(session::ClientSession& session, const std::int32_t fileId)
{
    if (PersistBlacklistFile_4011A0(session, fileId) == 0) {
        return 0;
    }

    session.WorldScene().blacklist.currentFileId = -1;
    session.WorldScene().blacklist.cursor = 0;
    return 1;
}

// 原 sub_401190@0x401190：直接返回黑名单记录游标的完整有符号 32 位值。
std::int32_t GetBlacklistCount_401190(session::ClientSession& session)
{
    return session.WorldScene().blacklist.cursor;
}

// 原函数：0x004013D0。
std::uint8_t ContainsBlacklistRecord_4013D0(session::ClientSession& session, const std::wstring_view name)
{
    if (name.empty()) { return 0; }
    for (const Record& record : session.WorldScene().blacklist.records) {
        if (record == name) { return 1; }
    }
    return 0;
}

// 原函数：0x00401270。
std::int8_t AddBlacklistRecord_401270(session::ClientSession& session, const std::wstring_view name)
{
    if (session.WorldScene().blacklist.cursor < 0 || session.WorldScene().blacklist.cursor >= static_cast<std::int32_t>(kRecordCount)) {
        return 0;
    }
    for (Record& record : session.WorldScene().blacklist.records) {
        if (!record.empty()) { continue; }
        const std::int32_t nextCursor = session.WorldScene().blacklist.cursor + 1;
        record = name;
        session.WorldScene().blacklist.cursor = nextCursor;
        break;
    }

    const std::int32_t fileId = session.WorldScene().blacklist.currentFileId;
    if (fileId >= 0) {
        PersistBlacklistFile_4011A0(session, fileId);
    }
    return 1;
}

// 原函数：0x004012F0。
std::int8_t RemoveBlacklistRecord_4012F0(session::ClientSession& session, const std::wstring_view name)
{
    if (session.WorldScene().blacklist.cursor < 0 || session.WorldScene().blacklist.cursor > static_cast<std::int32_t>(kRecordCount)) { return 0; }
    std::size_t i = 0;
    for (; i < kRecordCount; ++i) {
        if (session.WorldScene().blacklist.records[i] == name) { break; }
    }
    if (i >= kRecordCount) { return 0; }
    const std::int32_t oldCursor = session.WorldScene().blacklist.cursor;
    while (static_cast<std::int32_t>(i) != oldCursor) {
        // 原表尾下一槽借用路径；显式对象赋值，不跨越字符串对象内存。
        session.WorldScene().blacklist.records[i] = i + 1 == kRecordCount ? session.WorldScene().blacklist.path : session.WorldScene().blacklist.records[i + 1];
        if (++i >= kRecordCount) { return 0; }
    }
    session.WorldScene().blacklist.cursor = oldCursor - 1;
    session.WorldScene().blacklist.records[i].clear();

    const std::int32_t fileId = session.WorldScene().blacklist.currentFileId;
    if (fileId >= 0) {
        PersistBlacklistFile_4011A0(session, fileId);
    }
    return 1;
}

// 原函数：0x00401470。
std::int8_t ClearBlacklistRecords_401470(session::ClientSession& session)
{
    if (session.WorldScene().blacklist.records[0][0] == 0 || session.WorldScene().blacklist.cursor == 0) {
        return 0;
    }
    session.WorldScene().blacklist.records.fill({});
    session.WorldScene().blacklist.cursor = 0;
    const std::int32_t fileId = session.WorldScene().blacklist.currentFileId;
    if (fileId >= 0) {
        static_cast<void>(PersistBlacklistFile_4011A0(session, fileId));
    }
    return 1;
}

} // namespace cg::world::blacklist
