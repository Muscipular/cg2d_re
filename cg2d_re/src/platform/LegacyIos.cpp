#include "LegacyIos.hpp"

#include "LegacyHeap.hpp"
#include "LegacyLocale.hpp"
#include "LegacyMultibyte.hpp"

#include <bit>

namespace cg::platform {

void CallLegacyIosCallbacks_54A23C(
    LegacyIosBaseStorage* const storage, const std::int32_t event)
{
    LegacyIosCallbackNode* node = storage->callbackHead;
    while (node != nullptr) {
        const std::int32_t index = node->index;
        const LegacyIosCallback callback = node->callback;
        callback(event, storage, index);
        // 原54A252在callback返回后取next，允许回调改变当前节点链向。
        node = node->next;
    }
}

void TidyLegacyIos_54A2CB(LegacyIosBaseStorage* const storage)
{
    CallLegacyIosCallbacks_54A23C(storage, 0);
    // 原回调完成后才读取实际值链，不能提前快照两条链。
    LegacyIosValueNodeHeader* value = storage->valueHead;
    while (value != nullptr) {
        LegacyIosValueNodeHeader* const next = value->next;
        // 原53B4E7只转53C0A9；仅同域原堆节点，不释放宿主节点。
        FreeLegacyHeapMemory_53C0A9(value);
        value = next;
    }
    // 原54A2EC先取回调链头，54A2EF才清值链头，次序不能交换。
    LegacyIosCallbackNode* callback = storage->callbackHead;
    storage->valueHead = nullptr;
    while (callback != nullptr) {
        LegacyIosCallbackNode* const next = callback->next;
        FreeLegacyHeapMemory_53C0A9(callback);
        callback = next;
    }
    storage->callbackHead = nullptr;
}

// 原实际550144方法槽0，显式this真实函数地址；不生成SDK抛出/RTTI元数据。
const LegacyIosBaseMethods gLegacyIosBaseMethods550144{
    DeleteLegacyIosBase_54A220
};

void RegisterLegacyStandardStream_54A25E(LegacyIosBaseStorage* const storage)
{
    if (storage->registryIndex < 8) {
        for (;;) {
            std::uint32_t index = storage->registryIndex;
            LegacyIosBaseStorage* const registered = gLegacyStandardStreamRegistry10866F8[index];
            if (registered == nullptr || registered == storage) {
                break;
            }
            ++index;
            storage->registryIndex = index;
            if (index >= 8) {
                break;
            }
        }
    }
    const std::uint32_t index = storage->registryIndex;
    gLegacyStandardStreamRegistry10866F8[index] = storage;
    // 原发布槽后重新取对象索引，不复用先前局部索引。
    const std::uint32_t referenceIndex = storage->registryIndex;
    ++gLegacyStandardStreamReferences108671C[referenceIndex];
}

void InitializeLegacyIosBase_54A293(LegacyIosBaseStorage* const storage)
{
    LegacyLocaleImplementation** const locale = &storage->localeImplementation;
    if (locale != nullptr) {
        ConstructLegacyLocale_54865E(locale);
    }
    storage->exceptionBits = 0;
    storage->formatBits = 0x201;
    storage->precisionBits = 6;
    storage->widthBits = 0;
    storage->valueHead = nullptr;
    storage->callbackHead = nullptr;
    ClearLegacyIos_549F62(storage, 0, 0);
}

void DestroyLegacyIosBase_54A1D0(LegacyIosBaseStorage* const storage)
{
    storage->virtualTable = &gLegacyIosBaseMethods550144;
    const std::uint32_t initialIndex = storage->registryIndex;
    try {
        if (initialIndex == 0) {
            TidyLegacyIos_54A2CB(storage);
        } else {
            --gLegacyStandardStreamReferences108671C[initialIndex];
            const std::uint32_t currentIndex = storage->registryIndex;
            const std::int8_t references = std::bit_cast<std::int8_t>(
                gLegacyStandardStreamReferences108671C[currentIndex]);
            if (references <= 0) {
                TidyLegacyIos_54A2CB(storage);
            }
        }
        // 原正常locale清理前仍为state0，清理失败也走同一展开动作。
        DestroyLegacyLocale_54867D(&storage->localeImplementation);
    } catch (...) {
        DestroyLegacyLocale_54867D(&storage->localeImplementation);
        throw;
    }
}

LegacyIosBaseStorage* DeleteLegacyIosBase_54A220(
    LegacyIosBaseStorage* const storage, const std::uint32_t flags)
{
    DestroyLegacyIosBase_54A1D0(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

namespace {

// 登记表承载实际base地址；本组只按对应basic_ios表还原真实首成员，不覆盖独立ios_base。
LegacyIosBaseStorage* NarrowIosDeleteMethod(
    LegacyIosBaseStorage* const storage, const std::uint32_t flags)
{
    return reinterpret_cast<LegacyIosBaseStorage*>(DeleteLegacyNarrowIos_548CF6(
        reinterpret_cast<LegacyNarrowBasicIos*>(storage), flags));
}

LegacyIosBaseStorage* WideIosDeleteMethod(
    LegacyIosBaseStorage* const storage, const std::uint32_t flags)
{
    return reinterpret_cast<LegacyIosBaseStorage*>(DeleteLegacyWideIos_549D4B(
        reinterpret_cast<LegacyWideBasicIos*>(storage), flags));
}

} // namespace

const LegacyIosBaseMethods gLegacyNarrowBasicIosMethods54FFBC{
    NarrowIosDeleteMethod
};
const LegacyIosBaseMethods gLegacyWideBasicIosMethods550058{
    WideIosDeleteMethod
};

void InitializeLegacyNarrowIos_548E65(
    LegacyNarrowBasicIos* const storage, LegacyStreamBufferObject<char>* const buffer,
    const std::uint32_t registerBits)
{
    storage->tiedStream = nullptr;
    storage->buffer = buffer;
    storage->fill = 0x20;
    InitializeLegacyIosBase_54A293(&storage->base);
    if (storage->buffer == nullptr) {
        ClearLegacyIos_549F62(&storage->base, storage->base.stateBits | 4u, 0);
    }
    if (static_cast<std::uint8_t>(registerBits) != 0) {
        RegisterLegacyStandardStream_54A25E(&storage->base);
    }
}

void InitializeLegacyWideIos_549DBE(
    LegacyWideBasicIos* const storage,
    LegacyStreamBufferObject<std::uint16_t>* const buffer, const std::uint32_t registerBits)
{
    storage->tiedStream = nullptr;
    storage->buffer = buffer;
    storage->fill = Btowc_54AAD5(0x20);
    InitializeLegacyIosBase_54A293(&storage->base);
    if (storage->buffer == nullptr) {
        ClearLegacyIos_549F62(&storage->base, storage->base.stateBits | 4u, 0);
    }
    if (static_cast<std::uint8_t>(registerBits) != 0) {
        RegisterLegacyStandardStream_54A25E(&storage->base);
    }
}

void DestroyLegacyNarrowIos_54852C(LegacyNarrowBasicIos* const storage)
{
    storage->base.virtualTable = &gLegacyNarrowBasicIosMethods54FFBC;
    DestroyLegacyIosBase_54A1D0(&storage->base);
}

void DestroyLegacyWideIos_549638(LegacyWideBasicIos* const storage)
{
    storage->base.virtualTable = &gLegacyWideBasicIosMethods550058;
    DestroyLegacyIosBase_54A1D0(&storage->base);
}

LegacyNarrowBasicIos* DeleteLegacyNarrowIos_548CF6(
    LegacyNarrowBasicIos* const storage, const std::uint32_t flags)
{
    DestroyLegacyNarrowIos_54852C(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

LegacyWideBasicIos* DeleteLegacyWideIos_549D4B(
    LegacyWideBasicIos* const storage, const std::uint32_t flags)
{
    DestroyLegacyWideIos_549638(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

} // namespace cg::platform
