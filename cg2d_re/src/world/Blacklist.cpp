#include "cg/world/Blacklist.hpp"
#include "cg/text/Encoding.hpp"
#include <system_error>
#include "cg/input/Input.hpp"

#include <bit>
#include <cstring>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B226、0x0053B27D、0x0053B2F1、0x0053B356；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::world::blacklist {

// 原函数：0x004011A0。
static std::int8_t PersistBlacklistFile_4011A0(const std::int32_t fileId)
{
    const auto localPath = std::format(L"data\\blacklist_{}.txt", fileId);

    if (gPath != localPath) {
        return 0;
    }

    std::FILE* const file = _wfopen(gPath.c_str(), L"w");
    for (const Record& record : gRecords) {
        if (record[0] == 0) {
            break;
        }

        std::fputs(cg::text::ToGbk(record).c_str(), file);
        std::fputs("\n", file);
    }

    std::fclose(file);
    return std::int8_t {1};
}

RecordTable gRecords {}; // 原全局：0x00592FA8。
PathBuffer gPath {}; // 原全局：0x0059364C。
std::int32_t gCursor = 0; // 原全局：0x0059384C。
std::int32_t gCurrentFileId = -1; // 原全局：0x005531A0。


// 原函数：0x00401030。
std::int8_t LoadBlacklistFile(const std::int32_t fileId)
{
    gPath.clear();
    gRecords.fill({});
    gPath = std::format(L"data\\blacklist_{}.txt", fileId);

    std::FILE* const file = _wfopen(gPath.c_str(), L"rb+");
    if (file != nullptr) {
        std::rewind(file);
        if (std::feof(file) == 0) {
            for (;;) {
                if (gCursor < 0 || gCursor >= static_cast<std::int32_t>(kRecordCount)) { break; }
                char record[kRecordSize]{};
                std::fgets(record, static_cast<int>(kRecordSize), file);

                const std::int32_t cursor = gCursor;
                if (record[0] == 0
                    || record[0] == '\n'
                    || (record[0] == '\r' && record[1] == '\n')) {
                    const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(cursor);
                    gCursor = std::bit_cast<std::int32_t>(cursorBits - std::uint32_t {1});
                    gRecords[static_cast<std::size_t>(cursor)].clear();
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

                    gRecords[static_cast<std::size_t>(cursor)] = cg::text::FromGbkWide(record);
                    const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(cursor);
                    gCursor = std::bit_cast<std::int32_t>(cursorBits + std::uint32_t {1});
                    if (gCursor == static_cast<std::int32_t>(kRecordCount)) {
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

    gCurrentFileId = fileId;
    return std::int8_t {1};
}

// 原函数：0x00401000。
std::int8_t PersistAndReset_401000(const std::int32_t fileId)
{
    if (PersistBlacklistFile_4011A0(fileId) == 0) {
        return 0;
    }

    gCurrentFileId = -1;
    gCursor = 0;
    return 1;
}

// 原 sub_401190@0x401190：直接返回黑名单记录游标的完整有符号 32 位值。
std::int32_t GetBlacklistCount_401190()
{
    return gCursor;
}

// 原函数：0x004013D0。
std::uint8_t ContainsBlacklistRecord_4013D0(const std::wstring_view name)
{
    if (name.empty()) { return 0; }
    for (const Record& record : gRecords) {
        if (record == name) { return 1; }
    }
    return 0;
}

// 原函数：0x00401270。
std::int8_t AddBlacklistRecord_401270(const std::wstring_view name)
{
    if (gCursor < 0 || gCursor >= static_cast<std::int32_t>(kRecordCount)) {
        return 0;
    }
    for (Record& record : gRecords) {
        if (!record.empty()) { continue; }
        const std::int32_t nextCursor = gCursor + 1;
        record = name;
        gCursor = nextCursor;
        break;
    }

    const std::int32_t fileId = gCurrentFileId;
    if (fileId >= 0) {
        PersistBlacklistFile_4011A0(fileId);
    }
    return 1;
}

// 原函数：0x004012F0。
std::int8_t RemoveBlacklistRecord_4012F0(const std::wstring_view name)
{
    if (gCursor < 0 || gCursor > static_cast<std::int32_t>(kRecordCount)) { return 0; }
    std::size_t i = 0;
    for (; i < kRecordCount; ++i) {
        if (gRecords[i] == name) { break; }
    }
    if (i >= kRecordCount) { return 0; }
    const std::int32_t oldCursor = gCursor;
    while (static_cast<std::int32_t>(i) != oldCursor) {
        // 原表尾下一槽借用路径；显式对象赋值，不跨越字符串对象内存。
        gRecords[i] = i + 1 == kRecordCount ? gPath : gRecords[i + 1];
        if (++i >= kRecordCount) { return 0; }
    }
    gCursor = oldCursor - 1;
    gRecords[i].clear();

    const std::int32_t fileId = gCurrentFileId;
    if (fileId >= 0) {
        PersistBlacklistFile_4011A0(fileId);
    }
    return 1;
}

// 原函数：0x00401470。
std::int8_t ClearBlacklistRecords_401470()
{
    if (gRecords[0][0] == 0 || gCursor == 0) {
        return 0;
    }
    gRecords.fill({});
    gCursor = 0;
    const std::int32_t fileId = gCurrentFileId;
    if (fileId >= 0) {
        static_cast<void>(PersistBlacklistFile_4011A0(fileId));
    }
    return 1;
}

} // namespace cg::world::blacklist
