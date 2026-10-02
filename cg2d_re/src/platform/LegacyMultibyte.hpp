#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::platform {

// 原[1086764,1086988)；字节映射/分类和多字节locale的唯一存储，生产链见局部SPEC。
struct LegacyMultibyteStorage {
    std::uint32_t codePage;                         // 1086764；5468EA发布。
    std::array<std::uint8_t, 8> remaining04;         // 未解释原字节间隔。
    std::array<std::uint32_t, 3> codePageParameters; // 1086770/74/78；固定代码页记录参数。
    std::int32_t isMultibyte;                       // 108677C。
    std::array<std::uint8_t, 256> caseMap;           // 1086780；546B29生产。
    std::array<std::uint8_t, 257> classification;    // 1086880；字符value读取value+1项。
    std::array<std::uint8_t, 3> remaining21D;        // 原locale前的字节间隔。
    std::uint32_t locale;                           // 1086984；与1086160的C locale不同。
};
static_assert(std::is_standard_layout_v<LegacyMultibyteStorage>);
static_assert(std::is_trivially_copyable_v<LegacyMultibyteStorage>);
static_assert(offsetof(LegacyMultibyteStorage, codePageParameters) == 0x0C);
static_assert(offsetof(LegacyMultibyteStorage, isMultibyte) == 0x18);
static_assert(offsetof(LegacyMultibyteStorage, caseMap) == 0x1C);
static_assert(offsetof(LegacyMultibyteStorage, classification) == 0x11C);
static_assert(offsetof(LegacyMultibyteStorage, locale) == 0x220);
static_assert(sizeof(LegacyMultibyteStorage) == 0x224);

extern LegacyMultibyteStorage gLegacyMultibyte1086764;
extern std::uint32_t gLegacyMultibyteTypeBackend10862E4;   // 分类A/W探测，独立于LCMap后端。
extern std::uint32_t gLegacyMultibyteSystemCodePage10862E8; // 原getSystemCP的回退标记。
extern std::uint32_t gLegacyMultibyteInitGuard1087AEC;     // 初始化调用后置1，不按失败重试。

// 原546A83/5468EA；保留特殊U32代码页参数、固定表和系统CPINFO路径。
std::uint32_t ResolveLegacyMultibyteCodePage_546A83(std::uint32_t requested);
std::int32_t SetLegacyMultibyteCodePage_5468EA(std::uint32_t requested);

// 原546B29；生产分类/大小写表，原NLS返回不检查，失败未写局部内容仍未验。
std::uint32_t RefreshLegacySingleByteCase_546B29();

// 原546CAE；当前CRT553178调用不消费返回，不伪造门禁已置时的未定义EAX。
void InitializeLegacyMultibyte_546CAE();

// 原546ACD；只有原四种代码页返回特定locale，其他均0。
std::uint32_t MultibyteCodePageToLocale_546ACD(std::uint32_t codePage);

// 原546B00；清分类及代码页状态，保留大小写映射和间隔字节，返回0。
std::int32_t ResetLegacySingleByte_546B00();

// 原543D37在唯一caller 53DD2F的U8输入域；不公开一般宽字符转换。
std::uint32_t UpperLegacyDirectoryByte_543D37(std::uint8_t value);

} // namespace cg::platform
