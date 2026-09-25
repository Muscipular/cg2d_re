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

// 原 sub_401030@0x401030：从指定文件加载黑名单记录。
std::int8_t LoadBlacklistFile(std::int32_t fileId);

// 原 sub_401000@0x401000：callee 返回非零后依次重置文件 ID 与游标。
std::int8_t PersistAndReset_401000(std::int32_t fileId);

// 原 sub_401190@0x401190：直接返回黑名单记录游标的完整有符号 32 位值。
std::int32_t GetBlacklistCount_401190();

// 原 [0x4013D0,0x401463)：固定扫 100 槽，先排除 593850 当前文本，命中只返回 AL=1。
std::uint8_t ContainsBlacklistRecord_4013D0(const char* name);

// 原 sub_401270@0x401270：向首个空槽插入含 NUL 的记录，非早退路径按当前文件 ID 持久化。
std::int8_t AddBlacklistRecord_401270(const char* name);

// 原 sub_4012F0@0x4012F0：删除首个同名记录，按旧游标压移，正常完成后仅当文件 ID 非负时调用持久化，忽略其返回值。
std::int8_t RemoveBlacklistRecord_4012F0(const char* name);

} // namespace cg::blacklist
