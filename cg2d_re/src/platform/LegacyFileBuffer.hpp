#pragma once

#include <cstddef>
#include <cstdint>

#include "LegacyLocale.hpp"
#include "LegacyStdio.hpp"
#include "LegacyStreamBuffer.hpp"

namespace cg::platform {

// 原两个独立W32状态种子；PE节头/载入规则已证初值0，真实存储见LegacyFileBuffer.cpp。
// volatile只保持原每入口两次独立读取，不增加锁或原子操作。
extern volatile std::uint32_t gLegacyNarrowFileState1087B00;
extern volatile std::uint32_t gLegacyWideFileState1087AF8;

// 原转换器本组仅调方法槽4/+10；首成员是实际facet槽0表，其他三槽只承载地址。
// 实际完整转换器表/生产者待，不据40B已消费前缀推完整分配或SDK私有ABI。
template <typename Character>
struct LegacyCodecvtInputMethods final {
    LegacyLocaleFacetMethods base;
    void (*entries1To3[3])();
    std::int32_t (*input)(
        LegacyLocaleFacet* storage, std::uint32_t* state,
        const char* first, const char* last, const char** next,
        Character* output, Character* outputLast, Character** outputNext);
};
using LegacyNarrowCodecvtInputMethods = LegacyCodecvtInputMethods<char>;
using LegacyWideCodecvtInputMethods = LegacyCodecvtInputMethods<std::uint16_t>;
static_assert(offsetof(LegacyNarrowCodecvtInputMethods, base) == 0);
static_assert(offsetof(LegacyNarrowCodecvtInputMethods, input) == 32);
static_assert(sizeof(LegacyNarrowCodecvtInputMethods) == 40);
static_assert(offsetof(LegacyWideCodecvtInputMethods, base) == 0);
static_assert(offsetof(LegacyWideCodecvtInputMethods, input) == 32);
static_assert(sizeof(LegacyWideCodecvtInputMethods) == 40);

// 原转换器输出槽5/+14；输入40B前缀作为真实首成员，读消费视图保持。
// 48B仅已消费方法前缀，不推完整转换器分配或其余槽/SDK私有ABI。
template <typename Character>
struct LegacyCodecvtOutputMethods final {
    LegacyCodecvtInputMethods<Character> inputPrefix;
    std::int32_t (*output)(
        LegacyLocaleFacet* storage, std::uint32_t* state,
        const Character* first, const Character* last, const Character** next,
        char* output, char* outputLast, char** outputNext);
};
using LegacyNarrowCodecvtOutputMethods = LegacyCodecvtOutputMethods<char>;
using LegacyWideCodecvtOutputMethods = LegacyCodecvtOutputMethods<std::uint16_t>;
static_assert(offsetof(LegacyNarrowCodecvtOutputMethods, inputPrefix) == 0);
static_assert(offsetof(LegacyNarrowCodecvtOutputMethods, output) == 40);
static_assert(sizeof(LegacyNarrowCodecvtOutputMethods) == 48);
static_assert(offsetof(LegacyWideCodecvtOutputMethods, inputPrefix) == 0);
static_assert(offsetof(LegacyWideCodecvtOutputMethods, output) == 40);
static_assert(sizeof(LegacyWideCodecvtOutputMethods) == 48);

// 原basic_filebuf已证+00..+53视图；+38转换器/+44窄字符串均真实地址拓宽。
// 仅显式函数构析，不自动构造owner或接宿主streambuf/FILE。
template <typename Character>
struct LegacyFileBufferStorage final {
    LegacyStreamBufferObject<Character> base;     // 原+00..+37；实际首成员基类子对象。
    LegacyLocaleFacet* codecvtFacet;               // 原+38；真实非拥有转换器facet地址。
    std::uint32_t state3CBits;                    // 原+3C；第二次种子读取。
    std::uint32_t state40Bits;                    // 原+40；第一次种子读取。
    LegacyNarrowString* scratchString;            // 原+44；窄宽均窄字符串，mode0才清空。
    std::uint8_t modeOneFlag;                     // 原+48；mode==1的0/1 U8。
    LegacyLocaleImplementation* privateLocale;    // 原+4C；mode0显式重构。
    LegacyFile* file;                              // 原+50；实际原FILE。
};
using LegacyNarrowFileBuffer = LegacyFileBufferStorage<char>;
using LegacyWideFileBuffer = LegacyFileBufferStorage<std::uint16_t>;
static_assert(offsetof(LegacyNarrowFileBuffer, base) == 0);
static_assert(offsetof(LegacyWideFileBuffer, base) == 0);
static_assert(offsetof(LegacyNarrowFileBuffer, codecvtFacet) == 104);
static_assert(offsetof(LegacyNarrowFileBuffer, state3CBits) == 112);
static_assert(offsetof(LegacyNarrowFileBuffer, state40Bits) == 116);
static_assert(offsetof(LegacyNarrowFileBuffer, scratchString) == 120);
static_assert(offsetof(LegacyNarrowFileBuffer, modeOneFlag) == 128);
static_assert(offsetof(LegacyNarrowFileBuffer, privateLocale) == 136);
static_assert(offsetof(LegacyNarrowFileBuffer, file) == 144);
static_assert(sizeof(LegacyNarrowFileBuffer) == 152);
static_assert(offsetof(LegacyWideFileBuffer, codecvtFacet) == 104);
static_assert(offsetof(LegacyWideFileBuffer, state3CBits) == 112);
static_assert(offsetof(LegacyWideFileBuffer, state40Bits) == 116);
static_assert(offsetof(LegacyWideFileBuffer, scratchString) == 120);
static_assert(offsetof(LegacyWideFileBuffer, modeOneFlag) == 128);
static_assert(offsetof(LegacyWideFileBuffer, privateLocale) == 136);
static_assert(offsetof(LegacyWideFileBuffer, file) == 144);
static_assert(sizeof(LegacyWideFileBuffer) == 152);

// 原两_Init void方法；mode保持原W32/S32位型，仅比较0/1，不增范围限制。
void InitializeLegacyNarrowFileBuffer_548EFC(
    LegacyNarrowFileBuffer* storage, LegacyFile* file, std::int32_t mode);
void InitializeLegacyWideFileBuffer_549E88(
    LegacyWideFileBuffer* storage, LegacyFile* file, std::int32_t mode);
// 原两close；FILE成功关闭返回0才_Init(nullptr,2)/返this，其他返空。
LegacyNarrowFileBuffer* CloseLegacyNarrowFileBuffer_548ED5(LegacyNarrowFileBuffer* storage);
LegacyWideFileBuffer* CloseLegacyWideFileBuffer_549E61(LegacyWideFileBuffer* storage);

// 原完整构造/显式析构及基类析构；阶段清理、方法表写和原堆域保持。
LegacyNarrowFileBuffer* ConstructLegacyNarrowFileBuffer_5485FE(
    LegacyNarrowFileBuffer* storage, LegacyFile* file);
LegacyWideFileBuffer* ConstructLegacyWideFileBuffer_54970A(
    LegacyWideFileBuffer* storage, LegacyFile* file);
void DestroyLegacyNarrowFileBuffer_5487DE(LegacyNarrowFileBuffer* storage);
void DestroyLegacyWideFileBuffer_549935(LegacyWideFileBuffer* storage);

// 原scalar deleting入口；先显式析构，再flags bit0同域释放，返原地址。
LegacyNarrowFileBuffer* DeleteLegacyNarrowFileBuffer_548D4D(
    LegacyNarrowFileBuffer* storage, std::uint32_t flags);
LegacyWideFileBuffer* DeleteLegacyWideFileBuffer_549DA2(
    LegacyWideFileBuffer* storage, std::uint32_t flags);

// 原派生读取/回推；underflow缓存同一表先uflow再pbackfail，转换沿实际facet槽4。
std::int32_t PutBackLegacyNarrowFileBuffer_548979(
    LegacyNarrowFileBuffer* storage, std::int32_t character);
std::uint16_t PutBackLegacyWideFileBuffer_549AE2(
    LegacyWideFileBuffer* storage, std::uint16_t character);
std::int32_t PeekLegacyNarrowFileBuffer_548A26(LegacyNarrowFileBuffer* storage);
std::uint16_t PeekLegacyWideFileBuffer_549B99(LegacyWideFileBuffer* storage);
std::int32_t ReadLegacyNarrowFileBuffer_548A53(LegacyNarrowFileBuffer* storage);
std::uint16_t ReadLegacyWideFileBuffer_549BC7(LegacyWideFileBuffer* storage);

// 原overflow：EOF输入返回0，转换沿实际输出槽5/8B窄串及原短写判定。
std::int32_t WriteLegacyNarrowFileBuffer_548843(
    LegacyNarrowFileBuffer* storage, std::int32_t character);
std::uint16_t WriteLegacyWideFileBuffer_54999A(
    LegacyWideFileBuffer* storage, std::uint16_t character);
// 原窄宽同入口sync；FILE空返回0，仅完整S32 fflush负值为失败。
template <typename Character>
std::int32_t SyncLegacyFileBuffer_549D13(LegacyFileBufferStorage<Character>* storage);

// 原窄宽共用定位；offset消费S32/origin W32，mode未读；位置为24B原生对象表示。
template <typename Character>
LegacyStreamPosition* SeekLegacyFileBufferOffset_548B68(
    LegacyFileBufferStorage<Character>* storage, LegacyStreamPosition* result,
    std::int32_t offset, std::uint32_t origin, std::uint32_t modeBits);
template <typename Character>
LegacyStreamPosition* SeekLegacyFileBufferPosition_548BEA(
    LegacyFileBufferStorage<Character>* storage, LegacyStreamPosition* result,
    LegacyStreamPosition position, std::uint32_t modeBits);
// 原setbuf成功返真实基类首成员；宽U16缓冲借用为字节地址，W32字符数相加翻倍。
LegacyNarrowStreamBufferObject* SetLegacyNarrowFileBuffer_548CAC(
    LegacyNarrowFileBuffer* storage, char* buffer, std::uint32_t countBits);
LegacyWideStreamBufferObject* SetLegacyWideFileBuffer_549CE5(
    LegacyWideFileBuffer* storage, std::uint16_t* buffer, std::uint32_t countBits);

// 原晚段构造只恢复实际派生表/返this；未消费W32保留，不重复_Init或资源构造。
LegacyNarrowFileBuffer* ConstructLegacyNarrowFileBufferLate_5487D3(
    LegacyNarrowFileBuffer* storage, std::uint32_t ignoredStackWord);
LegacyWideFileBuffer* ConstructLegacyWideFileBufferLate_54992A(
    LegacyWideFileBuffer* storage, std::uint32_t ignoredStackWord);

} // namespace cg::platform
