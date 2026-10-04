#include "LegacyIos.hpp"

#include "LegacyHeap.hpp"
#include "LegacyFileBuffer.hpp"
#include "LegacyLocale.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyRuntimeExit.hpp"

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

volatile std::int32_t gLegacyNarrowStandardInitCount592894 = -1;
volatile std::int32_t gLegacyWideStandardInitCount5929EC = -1;

void* InitializeLegacyNarrowStandardStreams_5482DD(void* const initializer)
{
    void* currentObject = nullptr; // 原var_14入口明确清0。
    std::int32_t state = -1;
    if (gLegacyNarrowStandardInitCount592894 >= 0) {
        gLegacyNarrowStandardInitCount592894 = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(gLegacyNarrowStandardInitCount592894) + 1u);
        return initializer;
    }
    currentObject = &gLegacyNarrowStandardInputBuffer1086470;
    gLegacyNarrowStandardInitCount592894 = 1;
    state = 0;
    try {
        ConstructLegacyNarrowFileBuffer_5485FE(
            &gLegacyNarrowStandardInputBuffer1086470, &gLegacyStaticFiles58F8B8[0]);
        currentObject = &gLegacyNarrowStandardOutputBuffer1086348;
        state = 1;
        ConstructLegacyNarrowFileBuffer_5485FE(
            &gLegacyNarrowStandardOutputBuffer1086348, &gLegacyStaticFiles58F8B8[1]);
        currentObject = &gLegacyNarrowStandardInput1086430;
        state = 2;
        ConstructLegacyNarrowInput_548537(
            &gLegacyNarrowStandardInput1086430, &gLegacyNarrowStandardInputBuffer1086470.base, 1, 1);
        currentObject = &gLegacyNarrowStandardOutput10863A0;
        state = 3;
        ConstructLegacyNarrowOutput_548463(
            &gLegacyNarrowStandardOutput10863A0, &gLegacyNarrowStandardOutputBuffer1086348.base, 1, 1, 1);
        currentObject = &gLegacyNarrowStandardErrorBuffer10863D8;
        ProjectStreamIos(&gLegacyNarrowStandardInput1086430)->tiedStream = &gLegacyNarrowStandardOutput10863A0;
        state = 4;
        ConstructLegacyNarrowFileBuffer_5485FE(
            &gLegacyNarrowStandardErrorBuffer10863D8, &gLegacyStaticFiles58F8B8[2]);
        currentObject = &gLegacyNarrowStandardError10864C8;
        state = 5;
        ConstructLegacyNarrowOutput_548463(
            &gLegacyNarrowStandardError10864C8, &gLegacyNarrowStandardErrorBuffer10863D8.base, 1, 1, 1);
        currentObject = &gLegacyNarrowStandardLog1086310;
        ProjectStreamIos(&gLegacyNarrowStandardError10864C8)->tiedStream = &gLegacyNarrowStandardOutput10863A0;
        ProjectStreamIos(&gLegacyNarrowStandardError10864C8)->base.formatBits |= 2u;
        state = 6;
        ConstructLegacyNarrowOutput_548463(
            &gLegacyNarrowStandardLog1086310, &gLegacyNarrowStandardErrorBuffer10863D8.base, 1, 1, 1);
        ProjectStreamIos(&gLegacyNarrowStandardLog1086310)->tiedStream = &gLegacyNarrowStandardOutput10863A0;
    } catch (...) {
        // 原七个prev均-1：只当前state调用一叶，不清已完成对象/恢复计数。
        switch (state) {
        case 0:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyNarrowStandardInputBuffer1086470);
            break;
        case 1:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyNarrowStandardOutputBuffer1086348);
            break;
        case 2:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyNarrowStandardInput1086430);
            break;
        case 3:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyNarrowStandardOutput10863A0);
            break;
        case 4:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyNarrowStandardErrorBuffer10863D8);
            break;
        case 5:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyNarrowStandardError10864C8);
            break;
        case 6:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyNarrowStandardLog1086310);
            break;
        }
        throw;
    }
    return initializer;
}

void* InitializeLegacyWideStandardStreams_549397(void* const initializer)
{
    void* currentObject = nullptr; // 原var_14入口明确清0。
    std::int32_t state = -1;
    if (gLegacyWideStandardInitCount5929EC >= 0) {
        gLegacyWideStandardInitCount5929EC = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(gLegacyWideStandardInitCount5929EC) + 1u);
        return initializer;
    }
    currentObject = &gLegacyWideStandardInputBuffer1086610;
    gLegacyWideStandardInitCount5929EC = 1;
    state = 0;
    try {
        ConstructLegacyWideFileBuffer_54970A(
            &gLegacyWideStandardInputBuffer1086610, &gLegacyStaticFiles58F8B8[0]);
        currentObject = &gLegacyWideStandardOutputBuffer10865B8;
        state = 1;
        ConstructLegacyWideFileBuffer_54970A(
            &gLegacyWideStandardOutputBuffer10865B8, &gLegacyStaticFiles58F8B8[1]);
        currentObject = &gLegacyWideStandardErrorBuffer10866A0;
        state = 2;
        ConstructLegacyWideFileBuffer_54970A(
            &gLegacyWideStandardErrorBuffer10866A0, &gLegacyStaticFiles58F8B8[2]);
        currentObject = &gLegacyWideStandardInput1086578;
        state = 3;
        ConstructLegacyWideInput_549643(
            &gLegacyWideStandardInput1086578, &gLegacyWideStandardInputBuffer1086610.base, 1, 1);
        currentObject = &gLegacyWideStandardOutput1086668;
        state = 4;
        ConstructLegacyWideOutput_549525(
            &gLegacyWideStandardOutput1086668, &gLegacyWideStandardOutputBuffer10865B8.base, 1, 1, 1);
        currentObject = &gLegacyWideStandardError1086500;
        ProjectStreamIos(&gLegacyWideStandardInput1086578)->tiedStream = &gLegacyWideStandardOutput1086668;
        state = 5;
        ConstructLegacyWideOutput_549525(
            &gLegacyWideStandardError1086500, &gLegacyWideStandardErrorBuffer10866A0.base, 1, 1, 1);
        currentObject = &gLegacyWideStandardLog1086540;
        ProjectStreamIos(&gLegacyWideStandardError1086500)->tiedStream = &gLegacyWideStandardOutput1086668;
        ProjectStreamIos(&gLegacyWideStandardError1086500)->base.formatBits |= 2u;
        state = 6;
        ConstructLegacyWideOutput_549525(
            &gLegacyWideStandardLog1086540, &gLegacyWideStandardErrorBuffer10866A0.base, 1, 1, 1);
        ProjectStreamIos(&gLegacyWideStandardLog1086540)->tiedStream = &gLegacyWideStandardOutput1086668;
    } catch (...) {
        // 原七个prev均-1：只当前state调用一叶，不清已完成对象/恢复计数。
        switch (state) {
        case 0:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyWideStandardInputBuffer1086610);
            break;
        case 1:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyWideStandardOutputBuffer10865B8);
            break;
        case 2:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyWideStandardErrorBuffer10866A0);
            break;
        case 3:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyWideStandardInput1086578);
            break;
        case 4:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyWideStandardOutput1086668);
            break;
        case 5:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyWideStandardError1086500);
            break;
        case 6:
            UnwindLegacyStandardStream_54843B(currentObject, &gLegacyWideStandardLog1086540);
            break;
        }
        throw;
    }
    return initializer;
}

namespace {

std::int32_t SynchronizeOutputBuffer(LegacyNarrowStreamBufferObject* const buffer)
{
    const auto* const methods = reinterpret_cast<const LegacyNarrowStreamBufferMethods*>(
        buffer->buffer.virtualTable);
    return methods->sync(buffer);
}

std::int32_t SynchronizeOutputBuffer(LegacyWideStreamBufferObject* const buffer)
{
    const auto* const methods = reinterpret_cast<const LegacyWideStreamBufferMethods*>(
        buffer->buffer.virtualTable);
    return methods->sync(buffer);
}

} // namespace

template <typename Character>
LegacyOutputStreamStorage<Character>* FlushLegacyOutput_5495EE(
    LegacyOutputStreamStorage<Character>* const storage)
{
    std::uint32_t errorBits = 0;
    LegacyBasicIosStorage<Character>* const initialIos = ProjectStreamIos(storage);
    if ((initialIos->base.stateBits & 6u) == 0) {
        // 原无buffer空保护；仅实际sync完整S32 -1为失败。
        if (SynchronizeOutputBuffer(initialIos->buffer) == -1) {
            errorBits = 4;
        }
    }
    LegacyBasicIosStorage<Character>* const currentIos = ProjectStreamIos(storage);
    if (errorBits != 0) {
        std::uint32_t stateBits = currentIos->base.stateBits | errorBits;
        if (currentIos->buffer == nullptr) {
            stateBits |= 4u;
        }
        ClearLegacyIos_549F62(&currentIos->base, stateBits, 0);
    }
    return storage;
}

template LegacyNarrowOutputStream* FlushLegacyOutput_5495EE<char>(LegacyNarrowOutputStream*);
template LegacyWideOutputStream* FlushLegacyOutput_5495EE<std::uint16_t>(LegacyWideOutputStream*);

void ReleaseLegacyNarrowStandardStreams_5491F6()
{
    const std::int32_t count = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(gLegacyNarrowStandardInitCount592894) - 1u);
    gLegacyNarrowStandardInitCount592894 = count;
    if (count != 0) {
        return;
    }
    FlushLegacyOutput_5495EE(&gLegacyNarrowStandardOutput10863A0);
    FlushLegacyOutput_5495EE(&gLegacyNarrowStandardError10864C8);
    FlushLegacyOutput_5495EE(&gLegacyNarrowStandardLog1086310);
}

void ReleaseLegacyWideStandardStreams_549220()
{
    const std::int32_t count = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(gLegacyWideStandardInitCount5929EC) - 1u);
    gLegacyWideStandardInitCount5929EC = count;
    if (count != 0) {
        return;
    }
    FlushLegacyOutput_5495EE(&gLegacyWideStandardOutput1086668);
    FlushLegacyOutput_5495EE(&gLegacyWideStandardError1086500);
    FlushLegacyOutput_5495EE(&gLegacyWideStandardLog1086540);
}

LegacyNarrowFileBuffer* ConstructLegacyNarrowErrorBufferLate_54825C()
{
    return ConstructLegacyNarrowFileBufferLate_5487D3(
        &gLegacyNarrowStandardErrorBuffer10863D8, 0);
}

void DestroyLegacyNarrowErrorBufferAtExit_548275()
{
    DestroyLegacyNarrowFileBuffer_5487DE(&gLegacyNarrowStandardErrorBuffer10863D8);
}

LegacyWideOutputStream* RegisterLegacyWideErrorStreamLate_549343()
{
    return RegisterConstructLegacyWideOutput_54958F(&gLegacyWideStandardError1086500, 0, 1);
}

void DestroyLegacyWideErrorStreamAtExit_54935E()
{
    DestroyLegacyWideOutput_549324(&gLegacyWideStandardError1086500);
}

std::int32_t InitializeLegacyNarrowErrorBuffer_548252()
{
    ConstructLegacyNarrowErrorBufferLate_54825C();
    return RegisterLegacyAtExit_53B406(DestroyLegacyNarrowErrorBufferAtExit_548275);
}

std::int32_t InitializeLegacyWideErrorStream_549339()
{
    RegisterLegacyWideErrorStreamLate_549343();
    return RegisterLegacyAtExit_53B406(DestroyLegacyWideErrorStreamAtExit_54935E);
}

LegacyNarrowFileBuffer* ConstructLegacyNarrowInputBufferLate_54817A()
{
    return ConstructLegacyNarrowFileBufferLate_5487D3(&gLegacyNarrowStandardInputBuffer1086470, 0);
}

void DestroyLegacyNarrowInputBufferAtExit_548193()
{
    DestroyLegacyNarrowFileBuffer_5487DE(&gLegacyNarrowStandardInputBuffer1086470);
}

std::int32_t InitializeLegacyNarrowInputBuffer_548170()
{
    ConstructLegacyNarrowInputBufferLate_54817A();
    return RegisterLegacyAtExit_53B406(DestroyLegacyNarrowInputBufferAtExit_548193);
}

LegacyNarrowFileBuffer* ConstructLegacyNarrowOutputBufferLate_5481A7()
{
    return ConstructLegacyNarrowFileBufferLate_5487D3(&gLegacyNarrowStandardOutputBuffer1086348, 0);
}

void DestroyLegacyNarrowOutputBufferAtExit_5481C0()
{
    DestroyLegacyNarrowFileBuffer_5487DE(&gLegacyNarrowStandardOutputBuffer1086348);
}

std::int32_t InitializeLegacyNarrowOutputBuffer_54819D()
{
    ConstructLegacyNarrowOutputBufferLate_5481A7();
    return RegisterLegacyAtExit_53B406(DestroyLegacyNarrowOutputBufferAtExit_5481C0);
}

LegacyWideFileBuffer* ConstructLegacyWideInputBufferLate_549234()
{
    return ConstructLegacyWideFileBufferLate_54992A(&gLegacyWideStandardInputBuffer1086610, 0);
}

void DestroyLegacyWideInputBufferAtExit_54924D()
{
    DestroyLegacyWideFileBuffer_549935(&gLegacyWideStandardInputBuffer1086610);
}

std::int32_t InitializeLegacyWideInputBuffer_54922A()
{
    ConstructLegacyWideInputBufferLate_549234();
    return RegisterLegacyAtExit_53B406(DestroyLegacyWideInputBufferAtExit_54924D);
}

LegacyWideFileBuffer* ConstructLegacyWideOutputBufferLate_549261()
{
    return ConstructLegacyWideFileBufferLate_54992A(&gLegacyWideStandardOutputBuffer10865B8, 0);
}

void DestroyLegacyWideOutputBufferAtExit_54927A()
{
    DestroyLegacyWideFileBuffer_549935(&gLegacyWideStandardOutputBuffer10865B8);
}

std::int32_t InitializeLegacyWideOutputBuffer_549257()
{
    ConstructLegacyWideOutputBufferLate_549261();
    return RegisterLegacyAtExit_53B406(DestroyLegacyWideOutputBufferAtExit_54927A);
}

LegacyWideFileBuffer* ConstructLegacyWideErrorBufferLate_54928E()
{
    return ConstructLegacyWideFileBufferLate_54992A(&gLegacyWideStandardErrorBuffer10866A0, 0);
}

void DestroyLegacyWideErrorBufferAtExit_5492A7()
{
    DestroyLegacyWideFileBuffer_549935(&gLegacyWideStandardErrorBuffer10866A0);
}

std::int32_t InitializeLegacyWideErrorBuffer_549284()
{
    ConstructLegacyWideErrorBufferLate_54928E();
    return RegisterLegacyAtExit_53B406(DestroyLegacyWideErrorBufferAtExit_5492A7);
}

void* ConstructLegacyNarrowStandardInitializer_5491E0()
{
    return InitializeLegacyNarrowStandardStreams_5482DD(&gLegacyNarrowStandardInitializer1086539);
}

std::int32_t RegisterLegacyNarrowStandardInitializer_5491D6()
{
    ConstructLegacyNarrowStandardInitializer_5491E0();
    return RegisterLegacyAtExit_53B406(ReleaseLegacyNarrowStandardStreams_5491F6);
}

void* ConstructLegacyWideStandardInitializer_54920A()
{
    return InitializeLegacyWideStandardStreams_549397(&gLegacyWideStandardInitializer1086538);
}

std::int32_t RegisterLegacyWideStandardInitializer_549200()
{
    ConstructLegacyWideStandardInitializer_54920A();
    return RegisterLegacyAtExit_53B406(ReleaseLegacyWideStandardStreams_549220);
}

} // namespace cg::platform
