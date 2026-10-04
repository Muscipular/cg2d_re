#pragma once

#include <cstddef>
#include <cstdint>

#include "LegacyLocale.hpp"

namespace cg::platform {

// 原streambuf已证+00..+33视图：真实指针拓宽，余量S32；不含未决locale/派生字段。
// 不自动初始化/构析/分配，不接宿主streambuf；六间接槽可指向自身或实际外部字段。
template <typename Character>
struct LegacyStreamBufferStorage final {
    const void* virtualTable;             // 原+00；实际原生方法表，本组仅调用槽1/4/5。
    Character* readBase;                  // 原+04。
    Character* writeBase;                 // 原+08。
    Character** readBaseSlot;             // 原+0C。
    Character** writeBaseSlot;            // 原+10。
    Character* readCurrent;               // 原+14。
    Character* writeCurrent;              // 原+18。
    Character** readCurrentSlot;          // 原+1C。
    Character** writeCurrentSlot;         // 原+20。
    std::int32_t readRemaining;           // 原+24；字符数。
    std::int32_t writeRemaining;          // 原+28；字符数。
    std::int32_t* readRemainingSlot;      // 原+2C。
    std::int32_t* writeRemainingSlot;     // 原+30。
};
using LegacyNarrowStreamBuffer = LegacyStreamBufferStorage<char>;
using LegacyWideStreamBuffer = LegacyStreamBufferStorage<std::uint16_t>;
static_assert(offsetof(LegacyNarrowStreamBuffer, readCurrentSlot) == 56);
static_assert(offsetof(LegacyNarrowStreamBuffer, writeCurrentSlot) == 64);
static_assert(offsetof(LegacyNarrowStreamBuffer, readRemaining) == 72);
static_assert(offsetof(LegacyNarrowStreamBuffer, writeRemaining) == 76);
static_assert(offsetof(LegacyNarrowStreamBuffer, readRemainingSlot) == 80);
static_assert(offsetof(LegacyNarrowStreamBuffer, writeRemainingSlot) == 88);
static_assert(sizeof(LegacyNarrowStreamBuffer) == 96);
static_assert(offsetof(LegacyWideStreamBuffer, readCurrentSlot) == 56);
static_assert(offsetof(LegacyWideStreamBuffer, readRemainingSlot) == 80);
static_assert(sizeof(LegacyWideStreamBuffer) == 96);

// 四个真实原表仅声明，完整槽接口/本体绑定待；不定义空表/牌值或接SDK vft。
struct LegacyNarrowStreamBufferMethods;
struct LegacyWideStreamBufferMethods;
extern const LegacyNarrowStreamBufferMethods gLegacyNarrowStreamBufferMethods55000C;
extern const LegacyNarrowStreamBufferMethods gLegacyNarrowFileBufferMethods54FFD4;
extern const LegacyWideStreamBufferMethods gLegacyWideStreamBufferMethods5500A8;
extern const LegacyWideStreamBufferMethods gLegacyWideFileBufferMethods550070;

// 原streambuf完整已证前缀+00..+37，作为真实首成员子对象承载基类locale。
// 仅显式构析，不使用C++虚函数/SDK私有ABI，也不新增影子locale槽。
template <typename Character>
struct LegacyStreamBufferObject final {
    LegacyStreamBufferStorage<Character> buffer;    // 原+00..+33，实际首成员。
    LegacyLocaleImplementation* localeImplementation; // 原+34，单指针locale。
};
using LegacyNarrowStreamBufferObject = LegacyStreamBufferObject<char>;
using LegacyWideStreamBufferObject = LegacyStreamBufferObject<std::uint16_t>;
static_assert(offsetof(LegacyNarrowStreamBufferObject, buffer) == 0);
static_assert(offsetof(LegacyNarrowStreamBufferObject, localeImplementation) == 96);
static_assert(sizeof(LegacyNarrowStreamBufferObject) == 104);
static_assert(offsetof(LegacyWideStreamBufferObject, buffer) == 0);
static_assert(offsetof(LegacyWideStreamBufferObject, localeImplementation) == 96);
static_assert(sizeof(LegacyWideStreamBufferObject) == 104);

// 原13函数地址槽；本组只消费槽1/4/5，其余只承载地址，不通过该类型调用。
using LegacyStreamMethodAddress = void (*)();
static_assert(sizeof(LegacyStreamMethodAddress) == 8);
struct LegacyNarrowStreamBufferMethods final {
    LegacyStreamMethodAddress entry0;
    std::int32_t (*overflow)(LegacyNarrowStreamBufferObject* storage, std::int32_t character);
    LegacyStreamMethodAddress entries2To3[2];
    std::int32_t (*underflow)(LegacyNarrowStreamBufferObject* storage);
    std::int32_t (*uflow)(LegacyNarrowStreamBufferObject* storage);
    LegacyStreamMethodAddress entries6To12[7];
};
struct LegacyWideStreamBufferMethods final {
    LegacyStreamMethodAddress entry0;
    std::uint16_t (*overflow)(LegacyWideStreamBufferObject* storage, std::uint16_t character);
    LegacyStreamMethodAddress entries2To3[2];
    std::uint16_t (*underflow)(LegacyWideStreamBufferObject* storage);
    std::uint16_t (*uflow)(LegacyWideStreamBufferObject* storage);
    LegacyStreamMethodAddress entries6To12[7];
};
static_assert(offsetof(LegacyNarrowStreamBufferMethods, overflow) == 8);
static_assert(offsetof(LegacyNarrowStreamBufferMethods, underflow) == 32);
static_assert(offsetof(LegacyNarrowStreamBufferMethods, uflow) == 40);
static_assert(sizeof(LegacyNarrowStreamBufferMethods) == 104);
static_assert(offsetof(LegacyWideStreamBufferMethods, overflow) == 8);
static_assert(offsetof(LegacyWideStreamBufferMethods, underflow) == 32);
static_assert(offsetof(LegacyWideStreamBufferMethods, uflow) == 40);
static_assert(sizeof(LegacyWideStreamBufferMethods) == 104);

// 原默认字符叶不读取this/栈参；未消费W32实参保持不透明，不推定业务符号性。
std::int32_t DefaultLegacyNarrowCharacter_5486BF(
    LegacyNarrowStreamBuffer* storage, std::uint32_t ignoredStackValue);
std::int32_t DefaultLegacyNarrowRead_5486C5(LegacyNarrowStreamBuffer* storage);
// 原只规定AX，返回U16；不补充EAX高W16的约定。
std::uint16_t DefaultLegacyWideCharacter_549778(
    LegacyWideStreamBuffer* storage, std::uint32_t ignoredStackValue);
std::uint16_t DefaultLegacyWideRead_549782(LegacyWideStreamBuffer* storage);
// 原窄宽共用默认槽；只实例化已证char/U16，不装入虚表。
template <typename Character>
std::int32_t DefaultLegacyStreamZero_54977F(LegacyStreamBufferStorage<Character>* storage);
template <typename Character>
LegacyStreamBufferStorage<Character>* DefaultLegacySetBuffer_549925(
    LegacyStreamBufferStorage<Character>* storage,
    Character* ignoredBuffer, std::uint32_t ignoredCount);

// 原Gndec返回移动后的指针，Gninc/Pninc返回移动前指针，不校验余量/空槽。
char* DecrementLegacyReadCursor_548EA5(LegacyNarrowStreamBuffer* storage);
char* IncrementLegacyReadCursor_548EB5(LegacyNarrowStreamBuffer* storage);
char* IncrementLegacyWriteCursor_548EC5(LegacyNarrowStreamBuffer* storage);
std::uint16_t* DecrementLegacyWideReadCursor_549E1C(LegacyWideStreamBuffer* storage);
std::uint16_t* IncrementLegacyWideReadCursor_549E2D(LegacyWideStreamBuffer* storage);
std::uint16_t* IncrementLegacyWideWriteCursor_549E51(LegacyWideStreamBuffer* storage);
// 原549E08/549E3D；余量按S32字符数扣减，U16*按2B字符步长推进。
void BumpLegacyWideReadCursor_549E08(
    LegacyWideStreamBuffer* storage, std::int32_t characterCount);
void BumpLegacyWideWriteCursor_549E3D(
    LegacyWideStreamBuffer* storage, std::int32_t characterCount);

// 原549EDE窄宽共用重绑定/清六值；只实例化已证char/U16，实际owner另行接线。
template <typename Character>
void ResetLegacyStreamBuffer_549EDE(LegacyStreamBufferStorage<Character>* storage);

// 原24B位置返回：已证四W32；+04/+14未写，不推其原字段意义/补值。
// 原生布局两处自然填充，整对象表示复制；不互转SDK streampos或fpos_t。
struct alignas(8) LegacyStreamPosition final {
    std::uint32_t offsetBits;                       // 原+00，默认入口写FFFFFFFF。
    alignas(8) std::uint32_t filePositionLowBits;    // 原+08，先取5500E0。
    std::uint32_t filePositionHighBits;             // 原+0C，后取5500E4。
    std::uint32_t stateBits;                        // 原+10，后取1087B04。
};
static_assert(offsetof(LegacyStreamPosition, filePositionLowBits) == 8);
static_assert(offsetof(LegacyStreamPosition, filePositionHighBits) == 12);
static_assert(offsetof(LegacyStreamPosition, stateBits) == 16);
static_assert(sizeof(LegacyStreamPosition) == 24);

// 两个位置半字仅已加载初值0；状态种子仅声明，不把未加载读值当初值。
// volatile保持原不同槽读取顺序，不增加原子/锁或SDK状态镜像。
extern volatile std::uint32_t gLegacyDefaultPositionLow5500E0;
extern volatile std::uint32_t gLegacyDefaultPositionHigh5500E4;
extern volatile std::uint32_t gLegacyDefaultPositionState1087B04;

// 原默认位置叶不读this或位置/模式实参；各W32不透明，不推未消费符号性。
template <typename Character>
LegacyStreamPosition* DefaultLegacySeekOffset_5498B7(
    LegacyStreamBufferStorage<Character>* storage, LegacyStreamPosition* result,
    std::uint32_t ignoredOffset, std::uint32_t ignoredOrigin, std::uint32_t ignoredMode);
template <typename Character>
LegacyStreamPosition* DefaultLegacySeekPosition_5498EE(
    LegacyStreamBufferStorage<Character>* storage, LegacyStreamPosition* result,
    LegacyStreamPosition ignoredPosition, std::uint32_t ignoredMode);
// 原5487D0唯一ret4，void/不写状态，不规定EAX/不固定返回值；栈字暂保持W32。
template <typename Character>
void DefaultLegacyStreamNoOp_5487D0(
    LegacyStreamBufferStorage<Character>* storage, std::uint32_t ignoredStackValue);

// 原基类析构与scalar deleting；独立对象/filebuf实际基类子对象使用相同存储。
void DestroyLegacyNarrowStreamBuffer_5486B1(LegacyNarrowStreamBufferObject* storage);
void DestroyLegacyWideStreamBuffer_54976A(LegacyWideStreamBufferObject* storage);
LegacyNarrowStreamBufferObject* DeleteLegacyNarrowStreamBuffer_548D31(
    LegacyNarrowStreamBufferObject* storage, std::uint32_t flags);
LegacyWideStreamBufferObject* DeleteLegacyWideStreamBuffer_549D86(
    LegacyWideStreamBufferObject* storage, std::uint32_t flags);

// 原uflow：先实际underflow，再真实Gninc，读取旧游标；窄全W32/宽仅U16。
std::int32_t ConsumeLegacyNarrowStreamBuffer_5486C9(LegacyNarrowStreamBufferObject* storage);
std::uint16_t ConsumeLegacyWideStreamBuffer_549787(LegacyWideStreamBufferObject* storage);
// 原xsgetn/xsputn；S32请求/完成字符数，六间接槽和真实虚调用保持。
std::int32_t ReadLegacyNarrowStreamBuffer_5486E6(
    LegacyNarrowStreamBufferObject* storage, char* destination, std::int32_t requested);
std::int32_t WriteLegacyNarrowStreamBuffer_54875A(
    LegacyNarrowStreamBufferObject* storage, const char* source, std::int32_t requested);
std::int32_t ReadLegacyWideStreamBuffer_5497A8(
    LegacyWideStreamBufferObject* storage, std::uint16_t* destination, std::int32_t requested);
std::int32_t WriteLegacyWideStreamBuffer_54982F(
    LegacyWideStreamBufferObject* storage, const std::uint16_t* source, std::int32_t requested);

} // namespace cg::platform
