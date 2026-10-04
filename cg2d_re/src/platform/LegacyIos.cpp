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
namespace {

void* IosBaseDeleteMethod(LegacyIosBaseStorage* const storage, const std::uint32_t flags)
{
    return DeleteLegacyIosBase_54A220(storage, flags);
}

} // namespace

const LegacyIosBaseMethods gLegacyIosBaseMethods550144{
    IosBaseDeleteMethod
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
void* NarrowIosDeleteMethod(
    LegacyIosBaseStorage* const storage, const std::uint32_t flags)
{
    return DeleteLegacyNarrowIos_548CF6(reinterpret_cast<LegacyNarrowBasicIos*>(storage), flags);
}

void* WideIosDeleteMethod(
    LegacyIosBaseStorage* const storage, const std::uint32_t flags)
{
    return DeleteLegacyWideIos_549D4B(reinterpret_cast<LegacyWideBasicIos*>(storage), flags);
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

namespace {

// 每次原投影都重新读实际vbtable/偏移，不缓存跨父调用的虚基地址或增范围保护。
template <typename Character>
LegacyBasicIosStorage<Character>* ProjectStreamIos(LegacyOutputStreamStorage<Character>* const storage)
{
    const std::int32_t offset = storage->virtualBaseLayout->iosByteOffset;
    return reinterpret_cast<LegacyBasicIosStorage<Character>*>(
        reinterpret_cast<char*>(storage) + offset);
}

template <typename Character>
LegacyBasicIosStorage<Character>* ProjectStreamIos(LegacyInputStreamStorage<Character>* const storage)
{
    const std::int32_t offset = storage->virtualBaseLayout->iosByteOffset;
    return reinterpret_cast<LegacyBasicIosStorage<Character>*>(
        reinterpret_cast<char*>(storage) + offset);
}

} // namespace

const LegacyStreamVirtualBaseLayout gLegacyNarrowOutputVirtualBase54FFC0{
    0, static_cast<std::int32_t>(offsetof(LegacyNarrowOutputStream, ios))
};
const LegacyIosBaseMethods gLegacyNarrowOutputMethods54FFB4{
    DeleteLegacyNarrowOutputFromIos_548CD7
};

LegacyNarrowOutputStream* ConstructLegacyNarrowOutput_548463(
    LegacyNarrowOutputStream* const storage, LegacyStreamBufferObject<char>* const buffer,
    const std::uint32_t registerBits, const std::uint32_t initializeBits,
    const std::uint32_t virtualBaseBits)
{
    const bool ownsVirtualBase = virtualBaseBits != 0;
    if (ownsVirtualBase) {
        storage->virtualBaseLayout = &gLegacyNarrowOutputVirtualBase54FFC0;
        storage->ios.base.registryIndex = 0;
        storage->ios.base.virtualTable = &gLegacyNarrowBasicIosMethods54FFBC;
    }
    try {
        ProjectStreamIos(storage)->base.virtualTable = &gLegacyNarrowOutputMethods54FFB4;
        if (static_cast<std::uint8_t>(initializeBits) != 0) {
            InitializeLegacyNarrowIos_548E65(ProjectStreamIos(storage), buffer, registerBits);
        }
    } catch (...) {
        if (ownsVirtualBase) {
            DestroyLegacyNarrowIos_54852C(&storage->ios);
        }
        throw;
    }
    return storage;
}

LegacyNarrowOutputStream* RegisterConstructLegacyNarrowOutput_5484CD(
    LegacyNarrowOutputStream* const storage, const std::uint32_t /*ignoredStackWord*/, const std::uint32_t virtualBaseBits)
{
    const bool ownsVirtualBase = virtualBaseBits != 0;
    if (ownsVirtualBase) {
        storage->virtualBaseLayout = &gLegacyNarrowOutputVirtualBase54FFC0;
        storage->ios.base.registryIndex = 0;
        storage->ios.base.virtualTable = &gLegacyNarrowBasicIosMethods54FFBC;
    }
    try {
        ProjectStreamIos(storage)->base.virtualTable = &gLegacyNarrowOutputMethods54FFB4;
        RegisterLegacyStandardStream_54A25E(&ProjectStreamIos(storage)->base);
    } catch (...) {
        if (ownsVirtualBase) {
            DestroyLegacyNarrowIos_54852C(&storage->ios);
        }
        throw;
    }
    return storage;
}

void DestroyLegacyNarrowOutput_54823D(LegacyNarrowOutputStream* const storage)
{
    ProjectStreamIos(storage)->base.virtualTable = &gLegacyNarrowOutputMethods54FFB4;
    // 原析构尾传固定本类虚基；只表写使用实际vbtable投影，不把两地址合并。
    DestroyLegacyNarrowIos_54852C(&storage->ios);
}

void* DeleteLegacyNarrowOutputFromIos_548CD7(
    LegacyIosBaseStorage* const virtualBase, const std::uint32_t flags)
{
    auto* const storage = reinterpret_cast<LegacyNarrowOutputStream*>(
        reinterpret_cast<char*>(virtualBase) - offsetof(LegacyNarrowOutputStream, ios));
    DestroyLegacyNarrowOutput_54823D(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

const LegacyStreamVirtualBaseLayout gLegacyNarrowInputVirtualBase54FFC8{
    0, static_cast<std::int32_t>(offsetof(LegacyNarrowInputStream, ios))
};
const LegacyIosBaseMethods gLegacyNarrowInputMethods54FFAC{
    DeleteLegacyNarrowInputFromIos_548D12
};

LegacyNarrowInputStream* ConstructLegacyNarrowInput_548537(
    LegacyNarrowInputStream* const storage, LegacyStreamBufferObject<char>* const buffer,
    const std::uint32_t registerBits, const std::uint32_t virtualBaseBits)
{
    const bool ownsVirtualBase = virtualBaseBits != 0;
    if (ownsVirtualBase) {
        storage->virtualBaseLayout = &gLegacyNarrowInputVirtualBase54FFC8;
        storage->ios.base.registryIndex = 0;
        storage->ios.base.virtualTable = &gLegacyNarrowBasicIosMethods54FFBC;
    }
    try {
        storage->countBits = 0;
        ProjectStreamIos(storage)->base.virtualTable = &gLegacyNarrowInputMethods54FFAC;
        InitializeLegacyNarrowIos_548E65(ProjectStreamIos(storage), buffer, registerBits);
    } catch (...) {
        if (ownsVirtualBase) {
            DestroyLegacyNarrowIos_54852C(&storage->ios);
        }
        throw;
    }
    return storage;
}

LegacyNarrowInputStream* RegisterConstructLegacyNarrowInput_54859F(
    LegacyNarrowInputStream* const storage, const std::uint32_t /*ignoredStackWord*/, const std::uint32_t virtualBaseBits)
{
    const bool ownsVirtualBase = virtualBaseBits != 0;
    if (ownsVirtualBase) {
        storage->virtualBaseLayout = &gLegacyNarrowInputVirtualBase54FFC8;
        storage->ios.base.registryIndex = 0;
        storage->ios.base.virtualTable = &gLegacyNarrowBasicIosMethods54FFBC;
    }
    try {
        ProjectStreamIos(storage)->base.virtualTable = &gLegacyNarrowInputMethods54FFAC;
        RegisterLegacyStandardStream_54A25E(&ProjectStreamIos(storage)->base);
    } catch (...) {
        if (ownsVirtualBase) {
            DestroyLegacyNarrowIos_54852C(&storage->ios);
        }
        throw;
    }
    return storage;
}

void DestroyLegacyNarrowInput_5481F9(LegacyNarrowInputStream* const storage)
{
    ProjectStreamIos(storage)->base.virtualTable = &gLegacyNarrowInputMethods54FFAC;
    // 原析构尾传固定本类虚基；只表写使用实际vbtable投影，不把两地址合并。
    DestroyLegacyNarrowIos_54852C(&storage->ios);
}

void* DeleteLegacyNarrowInputFromIos_548D12(
    LegacyIosBaseStorage* const virtualBase, const std::uint32_t flags)
{
    auto* const storage = reinterpret_cast<LegacyNarrowInputStream*>(
        reinterpret_cast<char*>(virtualBase) - offsetof(LegacyNarrowInputStream, ios));
    DestroyLegacyNarrowInput_5481F9(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

const LegacyStreamVirtualBaseLayout gLegacyWideOutputVirtualBase55005C{
    0, static_cast<std::int32_t>(offsetof(LegacyWideOutputStream, ios))
};
const LegacyIosBaseMethods gLegacyWideOutputMethods550050{
    DeleteLegacyWideOutputFromIos_549D2C
};

LegacyWideOutputStream* ConstructLegacyWideOutput_549525(
    LegacyWideOutputStream* const storage, LegacyStreamBufferObject<std::uint16_t>* const buffer,
    const std::uint32_t registerBits, const std::uint32_t initializeBits,
    const std::uint32_t virtualBaseBits)
{
    const bool ownsVirtualBase = virtualBaseBits != 0;
    if (ownsVirtualBase) {
        storage->virtualBaseLayout = &gLegacyWideOutputVirtualBase55005C;
        storage->ios.base.registryIndex = 0;
        storage->ios.base.virtualTable = &gLegacyWideBasicIosMethods550058;
    }
    try {
        ProjectStreamIos(storage)->base.virtualTable = &gLegacyWideOutputMethods550050;
        if (static_cast<std::uint8_t>(initializeBits) != 0) {
            InitializeLegacyWideIos_549DBE(ProjectStreamIos(storage), buffer, registerBits);
        }
    } catch (...) {
        if (ownsVirtualBase) {
            DestroyLegacyWideIos_549638(&storage->ios);
        }
        throw;
    }
    return storage;
}

LegacyWideOutputStream* RegisterConstructLegacyWideOutput_54958F(
    LegacyWideOutputStream* const storage, const std::uint32_t /*ignoredStackWord*/, const std::uint32_t virtualBaseBits)
{
    const bool ownsVirtualBase = virtualBaseBits != 0;
    if (ownsVirtualBase) {
        storage->virtualBaseLayout = &gLegacyWideOutputVirtualBase55005C;
        storage->ios.base.registryIndex = 0;
        storage->ios.base.virtualTable = &gLegacyWideBasicIosMethods550058;
    }
    try {
        ProjectStreamIos(storage)->base.virtualTable = &gLegacyWideOutputMethods550050;
        RegisterLegacyStandardStream_54A25E(&ProjectStreamIos(storage)->base);
    } catch (...) {
        if (ownsVirtualBase) {
            DestroyLegacyWideIos_549638(&storage->ios);
        }
        throw;
    }
    return storage;
}

void DestroyLegacyWideOutput_549324(LegacyWideOutputStream* const storage)
{
    ProjectStreamIos(storage)->base.virtualTable = &gLegacyWideOutputMethods550050;
    // 原析构尾传固定本类虚基；只表写使用实际vbtable投影，不把两地址合并。
    DestroyLegacyWideIos_549638(&storage->ios);
}

void* DeleteLegacyWideOutputFromIos_549D2C(
    LegacyIosBaseStorage* const virtualBase, const std::uint32_t flags)
{
    auto* const storage = reinterpret_cast<LegacyWideOutputStream*>(
        reinterpret_cast<char*>(virtualBase) - offsetof(LegacyWideOutputStream, ios));
    DestroyLegacyWideOutput_549324(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

const LegacyStreamVirtualBaseLayout gLegacyWideInputVirtualBase550064{
    0, static_cast<std::int32_t>(offsetof(LegacyWideInputStream, ios))
};
const LegacyIosBaseMethods gLegacyWideInputMethods550048{
    DeleteLegacyWideInputFromIos_549D67
};

LegacyWideInputStream* ConstructLegacyWideInput_549643(
    LegacyWideInputStream* const storage, LegacyStreamBufferObject<std::uint16_t>* const buffer,
    const std::uint32_t registerBits, const std::uint32_t virtualBaseBits)
{
    const bool ownsVirtualBase = virtualBaseBits != 0;
    if (ownsVirtualBase) {
        storage->virtualBaseLayout = &gLegacyWideInputVirtualBase550064;
        storage->ios.base.registryIndex = 0;
        storage->ios.base.virtualTable = &gLegacyWideBasicIosMethods550058;
    }
    try {
        storage->countBits = 0;
        ProjectStreamIos(storage)->base.virtualTable = &gLegacyWideInputMethods550048;
        InitializeLegacyWideIos_549DBE(ProjectStreamIos(storage), buffer, registerBits);
    } catch (...) {
        if (ownsVirtualBase) {
            DestroyLegacyWideIos_549638(&storage->ios);
        }
        throw;
    }
    return storage;
}

LegacyWideInputStream* RegisterConstructLegacyWideInput_5496AB(
    LegacyWideInputStream* const storage, const std::uint32_t /*ignoredStackWord*/, const std::uint32_t virtualBaseBits)
{
    const bool ownsVirtualBase = virtualBaseBits != 0;
    if (ownsVirtualBase) {
        storage->virtualBaseLayout = &gLegacyWideInputVirtualBase550064;
        storage->ios.base.registryIndex = 0;
        storage->ios.base.virtualTable = &gLegacyWideBasicIosMethods550058;
    }
    try {
        ProjectStreamIos(storage)->base.virtualTable = &gLegacyWideInputMethods550048;
        RegisterLegacyStandardStream_54A25E(&ProjectStreamIos(storage)->base);
    } catch (...) {
        if (ownsVirtualBase) {
            DestroyLegacyWideIos_549638(&storage->ios);
        }
        throw;
    }
    return storage;
}

void DestroyLegacyWideInput_5492E0(LegacyWideInputStream* const storage)
{
    ProjectStreamIos(storage)->base.virtualTable = &gLegacyWideInputMethods550048;
    // 原析构尾传固定本类虚基；只表写使用实际vbtable投影，不把两地址合并。
    DestroyLegacyWideIos_549638(&storage->ios);
}

void* DeleteLegacyWideInputFromIos_549D67(
    LegacyIosBaseStorage* const virtualBase, const std::uint32_t flags)
{
    auto* const storage = reinterpret_cast<LegacyWideInputStream*>(
        reinterpret_cast<char*>(virtualBase) - offsetof(LegacyWideInputStream, ios));
    DestroyLegacyWideInput_5492E0(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

} // namespace cg::platform
