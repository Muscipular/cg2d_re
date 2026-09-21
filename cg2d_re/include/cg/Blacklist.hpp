#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::blacklist {

inline constexpr std::size_t kRecordCount = 100;
inline constexpr std::size_t kRecordSize = 17;
inline constexpr std::size_t kPathSize = 512;

using Record = std::array<std::uint8_t, kRecordSize>;
using RecordTable = std::array<Record, kRecordCount>;
using PathBuffer = std::array<char, kPathSize>;

// 原 0x592FA8，固定 100 条、每条 17 字节的黑名单记录。
extern RecordTable gRecords;

// 原 0x59364C，固定 512 字节的黑名单文件路径。
extern PathBuffer gPath;

// 原 0x59384C，按有符号 32 位访问的记录游标。
extern std::int32_t gCursor;

// 原 0x5531A0，按有符号 32 位访问的当前文件 ID。
extern std::int32_t gCurrentFileId;

// 原 sub_401000@0x401000：callee 返回非零后依次重置文件 ID 与游标。
std::int8_t PersistAndReset_401000(std::int32_t fileId);

} // namespace cg::blacklist
