#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

struct LegacyIosBaseStorage;
struct LegacyLocaleImplementation;

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
    const void* virtualTable;                         // 原+00；只保留，未路由。
    std::uint32_t stateBits;                          // 原+04；clear父另行接线。
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

} // namespace cg::platform
