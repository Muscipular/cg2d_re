#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace cg::session { class ClientSession; }

namespace cg::world::blacklist {

inline constexpr std::size_t kRecordCount = 100;
inline constexpr std::size_t kRecordSize = 17;
inline constexpr std::size_t kPathSize = 512;

using Record = std::wstring;
using RecordTable = std::array<Record, kRecordCount>;
using PathBuffer = std::wstring;

// 同一会话的增删/查询、窗口与文件存取共用实际记录和路径。
struct RuntimeState {
    RecordTable records {}; // 原592FA8，固定100条。
    PathBuffer path {}; // 原59364C，原文件路径借用。
    std::int32_t cursor = 0; // 原59384C，S32记录游标。
    std::int32_t currentFileId = -1; // 原5531A0，S32文件编号。
};

// 原 sub_401030@0x401030：从指定文件加载黑名单记录。
std::int8_t LoadBlacklistFile(session::ClientSession& session, std::int32_t fileId);

// 原 sub_401000@0x401000：callee 返回非零后依次重置文件 ID 与游标。
std::int8_t PersistAndReset_401000(session::ClientSession& session, std::int32_t fileId);

// 原 sub_401190@0x401190：直接返回黑名单记录游标的完整有符号 32 位值。
std::int32_t GetBlacklistCount_401190(session::ClientSession& session);

// 原 [0x4013D0,0x401463)：固定扫 100 槽，先排除 593850 当前文本，命中只返回 AL=1。
std::uint8_t ContainsBlacklistRecord_4013D0(session::ClientSession& session, const std::wstring_view name);

// 原 sub_401270@0x401270：向首个空槽插入含 NUL 的记录，非早退路径按当前文件 ID 持久化。
std::int8_t AddBlacklistRecord_401270(session::ClientSession& session, const std::wstring_view name);

// 原 sub_4012F0@0x4012F0：删除首个同名记录，按旧游标压移，正常完成后仅当文件 ID 非负时调用持久化，忽略其返回值。
std::int8_t RemoveBlacklistRecord_4012F0(session::ClientSession& session, const std::wstring_view name);

// 原401470：首记录和游标均非零才清表；非负fileId持久化，忽略保存返回。
std::int8_t ClearBlacklistRecords_401470(session::ClientSession& session);

} // namespace cg::world::blacklist
