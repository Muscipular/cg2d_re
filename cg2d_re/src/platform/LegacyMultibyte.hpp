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
extern std::int32_t gLegacyCtypeMaxBytes58FE3C; // 原MB_CUR_MAX；543A1A决定分类分支。
extern std::uint32_t gLegacyMultibyteTypeBackend10862E4;   // 分类A/W探测，独立于LCMap后端。
extern std::uint32_t gLegacyMultibyteSystemCodePage10862E8; // 原getSystemCP的回退标记。
extern std::uint32_t gLegacyMultibyteInitGuard1087AEC;     // 初始化调用后置1，不按失败重试。

// 原545208的CT_CTYPE1调用域；复用共享A/W后端，types按实际字符数写出。
std::int32_t GetLegacyCharacterTypes_545208(const char* source, std::int32_t sourceCount,
    std::uint16_t* types, std::uint32_t codePage, std::uint32_t locale, bool strict);

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

// 原543A1A；S32输入，供53DA5C解析字母数字，保留旧ctype/NLS分支。
std::int32_t UpperLegacyCharacter_543A1A(std::int32_t value);

// 原53DC64；与大写转换共用ctype/NLS状态，失败返回原S32输入。
std::int32_t LowerLegacyCharacter_53DC64(std::int32_t value);

// 原541955；供浮点数字分类复用既有本体，保留可变ctype及NLS输出别名。
std::uint32_t ClassifyLegacyCharacter_541955(std::int32_t value, std::uint32_t mask);

// 原543D37在唯一caller 53DD2F的U8输入域；不公开一般宽字符转换。
std::uint32_t UpperLegacyDirectoryByte_543D37(std::uint8_t value);

// 原54A9C3可空locale参数只读取两个W32；不包含MB_CUR_MAX或lead-byte表。
struct LegacyConversionLocale54A9C3 final {
    std::uint32_t ctypeLocale; // +0，非零进入系统转换。
    std::uint32_t codePage;    // +4，直接传给MultiByteToWideChar。
};
static_assert(sizeof(LegacyConversionLocale54A9C3) == 8);
static_assert(offsetof(LegacyConversionLocale54A9C3, codePage) == 4);

// 原544552；数量保留W32，目标可空；失败沿原共享errno，不预清目标。
std::int32_t Mbtowc_544552(wchar_t* destination, const char* source,
    std::uint32_t sourceCount);

// 原54472B；一个W16到旧字节串，容量取共享MB_CUR_MAX，目标可空。
std::int32_t Wctomb_54472B(char* destination, std::uint16_t character);

// 原54A9C3；调用者持有W32待续状态，可空locale只覆盖locale/代码页两个值。
std::int32_t Mbrtowc_54A9C3(wchar_t* destination, const char* source,
    std::uint32_t sourceCount, std::uint32_t* state,
    const LegacyConversionLocale54A9C3* locale);

// 原54AAD5；S32仅-1作EOF，其余取低U8，负转换结果返回W16 FFFF。
std::uint16_t Btowc_54AAD5(std::int32_t character);

// 原53C9DD；目标可空，数量/返回保持U32，旧locale/代码页/ctype和errno同源。
std::uint32_t Mbstowcs_53C9DD(wchar_t* destination, const char* source,
    std::uint32_t destinationCount);

// 原53BDEF/53BE17；保留S32输入和mask8/20H返回，不使用宿主ctype。
std::int32_t ClassifyLegacySpace_53BDEF(std::int32_t character);
std::int32_t ClassifyLegacyControl_53BE17(std::int32_t character);

// 原53E919；数字不变，其他作W32位变换，不是直接返回0..15。
std::int32_t NormalizeLegacyHexCharacter_53E919(std::int32_t character);

// 原546CCA；读取现有多字节分类表的lead位，仅输入低U8参与。
std::int32_t IsLegacyMultibyteLead_546CCA(std::uint32_t character);

} // namespace cg::platform
