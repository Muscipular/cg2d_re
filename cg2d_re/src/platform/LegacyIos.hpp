#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

struct LegacyIosBaseStorage;
struct LegacyLocaleImplementation;

// 原550144仅已证删除槽0；8B为消费前缀，不推完整SDK虚表/分配规模。
struct LegacyIosBaseMethods final {
    LegacyIosBaseStorage* (*deletingDestructor)(
        LegacyIosBaseStorage* storage, std::uint32_t flags);
};
static_assert(sizeof(LegacyIosBaseMethods) == 8);
extern const LegacyIosBaseMethods gLegacyIosBaseMethods550144;

// 原登记地址数组/独立U8计数，仅真实外部声明；未加载初值/实际容量不补证为零或9/12。
extern LegacyIosBaseStorage* gLegacyStandardStreamRegistry10866F8[];
extern std::uint8_t gLegacyStandardStreamReferences108671C[];

// 原回调call后由caller清12B栈：event/this/index三参，返回寄存器不消费。
using LegacyIosCallback = void (__cdecl *)(
    std::int32_t event, LegacyIosBaseStorage* storage, std::int32_t index);

// 原值链本组仅消费+00 next；这是节点头视图，不断言整节点容量或分配规模。
struct LegacyIosValueNodeHeader final {
    LegacyIosValueNodeHeader* next;
};

// 原回调节点已证+00..+0B视图；仅两地址拓宽，index保持S32，不推整节点分配规模。
struct LegacyIosCallbackNode final {
    LegacyIosCallbackNode* next; // 原+00。
    std::int32_t index;          // 原+04；传回调第三参数。
    LegacyIosCallback callback; // 原+08；cdecl三参，原不检查空函数。
};
static_assert(offsetof(LegacyIosCallbackNode, index) == 8);
static_assert(offsetof(LegacyIosCallbackNode, callback) == 16);
static_assert(sizeof(LegacyIosCallbackNode) == 24);

// 原ios_base已证+00..+27；数字保持W32位型，未消费符号/格式语义不另推定。
// 不自动构析/初始化，不与宿主std::ios_base互转，不建立登记表/回调影子状态。
struct LegacyIosBaseStorage final {
    const void* virtualTable;                         // 原+00；真实原生方法表前缀，不接SDK虚表。
    std::uint32_t stateBits;                          // 原+04；clear保持真实声明，抛出承接待。
    std::uint32_t exceptionBits;                      // 原+08。
    std::uint32_t formatBits;                         // 原+0C。
    std::uint32_t precisionBits;                      // 原+10；本组不解释符号性。
    std::uint32_t widthBits;                          // 原+14；本组不解释符号性。
    LegacyIosValueNodeHeader* valueHead;              // 原+18；实际值链节点头。
    LegacyIosCallbackNode* callbackHead;              // 原+1C；实际回调链。
    LegacyLocaleImplementation* localeImplementation; // 原+20；locale单指针字段。
    std::uint32_t registryIndex;                      // 原+24；原jnb/jb按U32比较。
};
static_assert(offsetof(LegacyIosBaseStorage, stateBits) == 8);
static_assert(offsetof(LegacyIosBaseStorage, valueHead) == 32);
static_assert(offsetof(LegacyIosBaseStorage, callbackHead) == 40);
static_assert(offsetof(LegacyIosBaseStorage, localeImplementation) == 48);
static_assert(offsetof(LegacyIosBaseStorage, registryIndex) == 56);
static_assert(sizeof(LegacyIosBaseStorage) == 64);

// 原54A23C：回调返回后才读当前node.next；不重取表头、预取next或增保护。
void CallLegacyIosCallbacks_54A23C(LegacyIosBaseStorage* storage, std::int32_t event);
// 原54A2CB：event0→值链释放→缓存回调头/清值头→回调链释放/清回调头。
void TidyLegacyIos_54A2CB(LegacyIosBaseStorage* storage);

// 原54A25E：按对象U32索引扫描至8，仍写最终槽/增U8计数，不增容量门禁。
void RegisterLegacyStandardStream_54A25E(LegacyIosBaseStorage* storage);
// 原54A293：locale先构造，再六字段/两链初始化，最后真实clear(0,0)。
void InitializeLegacyIosBase_54A293(LegacyIosBaseStorage* storage);
// 原clear未实现；W32 state与重抛栈字低U8保持，不能替固定成功/宿主异常。
void ClearLegacyIos_549F62(
    LegacyIosBaseStorage* storage, std::uint32_t stateBits, std::uint32_t rethrowBits);
// 原完整析构及删除；共享U8按S8门禁，正常/展开locale次序与原堆域保持。
void DestroyLegacyIosBase_54A1D0(LegacyIosBaseStorage* storage);
LegacyIosBaseStorage* DeleteLegacyIosBase_54A220(
    LegacyIosBaseStorage* storage, std::uint32_t flags);

// 原basic_ios在ios_base之后的已消费+28/+2C/+30字段；仅已证地址拓宽。
// tie为真实ostream对象地址，尚未定义其虚基布局，不当作basic_ios地址互转。
template <typename Character>
struct LegacyStreamBufferObject;
template <typename Character>
struct LegacyOutputStreamStorage;
template <typename Character>
struct LegacyBasicIosStorage final {
    LegacyIosBaseStorage base;                         // 原+00..+27，真实首成员。
    LegacyStreamBufferObject<Character>* buffer;       // 原+28，真实非拥有streambuf。
    LegacyOutputStreamStorage<Character>* tiedStream;  // 原+2C，真实非拥有ostream。
    Character fill;                                   // 原+30，窄低U8/宽U16。
};
using LegacyNarrowBasicIos = LegacyBasicIosStorage<char>;
using LegacyWideBasicIos = LegacyBasicIosStorage<std::uint16_t>;
static_assert(offsetof(LegacyNarrowBasicIos, base) == 0);
static_assert(offsetof(LegacyNarrowBasicIos, buffer) == 64);
static_assert(offsetof(LegacyNarrowBasicIos, tiedStream) == 72);
static_assert(offsetof(LegacyNarrowBasicIos, fill) == 80);
static_assert(sizeof(LegacyNarrowBasicIos) == 88);
static_assert(offsetof(LegacyWideBasicIos, base) == 0);
static_assert(offsetof(LegacyWideBasicIos, buffer) == 64);
static_assert(offsetof(LegacyWideBasicIos, tiedStream) == 72);
static_assert(offsetof(LegacyWideBasicIos, fill) == 80);
static_assert(sizeof(LegacyWideBasicIos) == 88);

// 原两basic_ios表只已证删除槽0；沿真实ios_base首成员接口适配，8B为消费前缀。
// 不推完整SDK/虚基或分配布局，不擦除函数地址。
extern const LegacyIosBaseMethods gLegacyNarrowBasicIosMethods54FFBC;
extern const LegacyIosBaseMethods gLegacyWideBasicIosMethods550058;

// 原init：base返回后重取buffer、空才clear；registerBits只检查低U8。
void InitializeLegacyNarrowIos_548E65(
    LegacyNarrowBasicIos* storage, LegacyStreamBufferObject<char>* buffer,
    std::uint32_t registerBits);
void InitializeLegacyWideIos_549DBE(
    LegacyWideBasicIos* storage, LegacyStreamBufferObject<std::uint16_t>* buffer,
    std::uint32_t registerBits);
// 原析构先写各自basic_ios表再真实ios_base析构；本组this不作-4/-8调整。
void DestroyLegacyNarrowIos_54852C(LegacyNarrowBasicIos* storage);
void DestroyLegacyWideIos_549638(LegacyWideBasicIos* storage);
LegacyNarrowBasicIos* DeleteLegacyNarrowIos_548CF6(
    LegacyNarrowBasicIos* storage, std::uint32_t flags);
LegacyWideBasicIos* DeleteLegacyWideIos_549D4B(
    LegacyWideBasicIos* storage, std::uint32_t flags);

} // namespace cg::platform
