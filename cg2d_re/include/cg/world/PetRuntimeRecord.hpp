#pragma once

#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/world/PetTechniqueRecord.hpp"
#include "cg/world/PlayerItemRecord.hpp"
#include "cg/world/PlayerSlots.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

namespace cg::session { class ClientSession; }
namespace cg::world {

// 原F633F8五条宠物数据，不是FF02E0地图槽；未消费间隔不计最终类型冻结。
struct PetRuntimeRecord {
    // 原0x004CD360：成员正序构造及失败前缀清理由C++承接。
    PetRuntimeRecord() = default;
    // 原0x004CD2D0：成员自动逆序析构。
    ~PetRuntimeRecord() = default;
    PetRuntimeRecord(const PetRuntimeRecord&) = default;
    PetRuntimeRecord& operator=(const PetRuntimeRecord&) = default;
    void ClearRaw() noexcept; // 原零字节回复／场景清理，不构造整数或消费RNG。
    std::uint16_t used; // 原+0占用。
    std::uint16_t value02; // 原成员偏移：+0x02。 主回复掩码200001。
    std::uint16_t value04; // 原成员偏移：+0x04。 主回复掩码5。
    std::array<std::byte, 2> bytes06; // 原成员偏移：+0x06。
    std::int32_t value08; // 原成员偏移：+0x08。 主回复掩码201。
    std::array<platform::LegacyCryptInt, 6> values0C; // HP/最大HP/FP/最大FP/+4C/+5C。
    std::int32_t lvFlag6C; // 原成员偏移：+0x6C。 原F63464；50CF70写入完整W32。
    std::array<std::int32_t, 26> values70; // 原+70..+D4，两回复共享。
    std::array<detail::PetTechniqueRecord, kPetTechniqueSlotCount> techniquesD8; // 原成员偏移：+0xD8。
    std::array<SkillPairRecord, kPetTechniqueSlotCount> techniquePairs650; // 原成员偏移：+0x650。 5185C0只生成索引，不排序。
    std::int32_t imageNo6A0; // 原成员偏移：+0x6A0。
    std::int32_t value6A4; // 原成员偏移：+0x6A4。
    std::array<std::byte, 2> bytes6A8; // 原成员偏移：+0x6A8。
    std::uint16_t flags6AA; // 原成员偏移：+0x6AA。 不含跟随bit0x10的状态。
    std::wstring name6AC; // 原成员偏移：+0x6AC。
    std::wstring name6BD; // 原成员偏移：+0x6BD。
    std::array<std::byte, 2> bytes6CE; // 原成员偏移：+0x6CE。
    std::int32_t value6D0; // 原成员偏移：+0x6D0。 518580复制到索引对。
    std::int32_t follow6D4; // 原成员偏移：+0x6D4。
    std::array<std::int32_t, 3> values6D8; // 原成员偏移：+0x6D8。
    std::array<PlayerItemRecord, 6> items6E4; // 原成员偏移：+0x6E4。 原51B630复用的六条C5C物品。
    std::uint8_t updated510C; // 原成员偏移：+0x510C。 仅50A6B0尾部置1。
    std::array<std::byte, 3> bytes510D; // 原成员偏移：+0x510D。
};

static_assert(!std::is_trivially_destructible_v<PetRuntimeRecord>);
static_assert(offsetof(PetRuntimeRecord, used) == 0);
static_assert(offsetof(PetRuntimeRecord, value02) == 2);
static_assert(offsetof(PetRuntimeRecord, value04) == 4);
static_assert(offsetof(PetRuntimeRecord, value08) == 8);
static_assert(offsetof(PetRuntimeRecord, values0C) == 0x0C);
static_assert(offsetof(PetRuntimeRecord, lvFlag6C) == 0x6C);
static_assert(offsetof(PetRuntimeRecord, values70) == 0x70);
static_assert(offsetof(PetRuntimeRecord, techniquesD8) == 0xD8);
// 原0x650／0x6A0之后的成员随两张技能表扩容，旧偏移保留在成员注释中。
static_assert(offsetof(PetRuntimeRecord, techniquePairs650) == 0xD8 + sizeof(detail::PetTechniqueRecord) * kPetTechniqueSlotCount);
static_assert(offsetof(PetRuntimeRecord, imageNo6A0) == offsetof(PetRuntimeRecord, techniquePairs650) + sizeof(SkillPairRecord) * kPetTechniqueSlotCount);
static_assert(offsetof(PetRuntimeRecord, value6A4) == offsetof(PetRuntimeRecord, imageNo6A0) + 4);
static_assert(offsetof(PetRuntimeRecord, flags6AA) == offsetof(PetRuntimeRecord, imageNo6A0) + 0xA);

// 原DE0B08，每项8B；索引是数值，不是地址。
struct PetPairRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t index; // 原成员偏移：+0x02。
    std::int32_t value; // 原成员偏移：+0x04。
};

static_assert(offsetof(PetPairRecord, used) == 0);
static_assert(offsetof(PetPairRecord, index) == 2);
static_assert(offsetof(PetPairRecord, value) == 4);
static_assert(sizeof(PetPairRecord) == 8);

// 首会话沿原中央顺序初始化，后续会话由宿主初始化；不重复构造。
void InitializePetRuntimeStorage(session::ClientSession& session);

} // namespace cg::world
