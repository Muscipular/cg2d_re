#include "LegacyIos.hpp"

#include "LegacyHeap.hpp"
#include "LegacyException.hpp"
#include "NativeCppException.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "LegacyFileBuffer.hpp"
#include "LegacyLocale.hpp"
#include "LegacyMultibyte.hpp"

#include <bit>
#include <cstdlib>
#include <type_traits>

namespace cg::platform {

// 原PE已证14个标准流对象初始填零；这里只承载真实被动存储，构析仍沿显式原函数。
static_assert(std::is_trivial_v<LegacyFileBufferStorage<char>>);
static_assert(std::is_trivial_v<LegacyNarrowInputStream>);
static_assert(std::is_trivial_v<LegacyNarrowOutputStream>);
static_assert(std::is_trivial_v<LegacyFileBufferStorage<std::uint16_t>>);
static_assert(std::is_trivial_v<LegacyWideInputStream>);
static_assert(std::is_trivial_v<LegacyWideOutputStream>);
LegacyFileBufferStorage<char> gLegacyNarrowStandardInputBuffer1086470{};
LegacyFileBufferStorage<char> gLegacyNarrowStandardOutputBuffer1086348{};
LegacyFileBufferStorage<char> gLegacyNarrowStandardErrorBuffer10863D8{};
LegacyNarrowInputStream gLegacyNarrowStandardInput1086430{};
LegacyNarrowOutputStream gLegacyNarrowStandardOutput10863A0{};
LegacyNarrowOutputStream gLegacyNarrowStandardError10864C8{};
LegacyNarrowOutputStream gLegacyNarrowStandardLog1086310{};
LegacyFileBufferStorage<std::uint16_t> gLegacyWideStandardInputBuffer1086610{};
LegacyFileBufferStorage<std::uint16_t> gLegacyWideStandardOutputBuffer10865B8{};
LegacyFileBufferStorage<std::uint16_t> gLegacyWideStandardErrorBuffer10866A0{};
LegacyWideInputStream gLegacyWideStandardInput1086578{};
LegacyWideOutputStream gLegacyWideStandardOutput1086668{};
LegacyWideOutputStream gLegacyWideStandardError1086500{};
LegacyWideOutputStream gLegacyWideStandardLog1086540{};

// SR01仅补原0..8消费前缀，原PE载入初始零；9不是完整原容量证明。
// 指针非拥有且仅地址拓宽，U8仍原宽；不自动构析/清槽或增加索引保护。
LegacyIosBaseStorage* gLegacyStandardStreamRegistry10866F8[kLegacyStandardStreamRegistryPrefixSlots]{};
std::uint8_t gLegacyStandardStreamReferences108671C[kLegacyStandardStreamRegistryPrefixSlots]{};
static_assert(sizeof(gLegacyStandardStreamRegistry10866F8) == 72);
static_assert(sizeof(gLegacyStandardStreamReferences108671C) == 9);

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

// SI01：原Init只保存/返回地址；本类型只承接四个独立对象的身份。
// 原完整对象尺寸未知，本线sizeof不用于推原布局或相邻字节；计数/流状态仍在原owner。
static_assert(std::is_trivially_default_constructible_v<LegacyStandardStreamInitializer>);
static_assert(std::is_trivially_destructible_v<LegacyStandardStreamInitializer>);
static_assert(std::is_empty_v<LegacyStandardStreamInitializer>);
LegacyStandardStreamInitializer gLegacyNarrowEarlyStandardInitializer628FE9 {};
LegacyStandardStreamInitializer gLegacyWideEarlyStandardInitializer628FE8 {};
LegacyStandardStreamInitializer gLegacyNarrowStandardInitializer1086539 {};
LegacyStandardStreamInitializer gLegacyWideStandardInitializer1086538 {};

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
        if (currentObject != nullptr) {
            ConstructLegacyNarrowFileBuffer_5485FE(
                &gLegacyNarrowStandardInputBuffer1086470, &gLegacyStaticFiles58F8B8[0]);
        }
        currentObject = &gLegacyNarrowStandardOutputBuffer1086348;
        state = 1;
        if (currentObject != nullptr) {
            ConstructLegacyNarrowFileBuffer_5485FE(
                &gLegacyNarrowStandardOutputBuffer1086348, &gLegacyStaticFiles58F8B8[1]);
        }
        currentObject = &gLegacyNarrowStandardInput1086430;
        state = 2;
        if (currentObject != nullptr) {
            ConstructLegacyNarrowInput_548537(
                &gLegacyNarrowStandardInput1086430, &gLegacyNarrowStandardInputBuffer1086470.base, 1, 1);
        }
        currentObject = &gLegacyNarrowStandardOutput10863A0;
        state = 3;
        if (currentObject != nullptr) {
            ConstructLegacyNarrowOutput_548463(
                &gLegacyNarrowStandardOutput10863A0, &gLegacyNarrowStandardOutputBuffer1086348.base, 1, 1, 1);
        }
        currentObject = &gLegacyNarrowStandardErrorBuffer10863D8;
        ProjectStreamIos(&gLegacyNarrowStandardInput1086430)->tiedStream = &gLegacyNarrowStandardOutput10863A0;
        state = 4;
        if (currentObject != nullptr) {
            ConstructLegacyNarrowFileBuffer_5485FE(
                &gLegacyNarrowStandardErrorBuffer10863D8, &gLegacyStaticFiles58F8B8[2]);
        }
        currentObject = &gLegacyNarrowStandardError10864C8;
        state = 5;
        if (currentObject != nullptr) {
            ConstructLegacyNarrowOutput_548463(
                &gLegacyNarrowStandardError10864C8, &gLegacyNarrowStandardErrorBuffer10863D8.base, 1, 1, 1);
        }
        currentObject = &gLegacyNarrowStandardLog1086310;
        ProjectStreamIos(&gLegacyNarrowStandardError10864C8)->tiedStream = &gLegacyNarrowStandardOutput10863A0;
        ProjectStreamIos(&gLegacyNarrowStandardError10864C8)->base.formatBits |= 2u;
        state = 6;
        if (currentObject != nullptr) {
            ConstructLegacyNarrowOutput_548463(
                &gLegacyNarrowStandardLog1086310, &gLegacyNarrowStandardErrorBuffer10863D8.base, 1, 1, 1);
        }
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
        if (currentObject != nullptr) {
            ConstructLegacyWideFileBuffer_54970A(
                &gLegacyWideStandardInputBuffer1086610, &gLegacyStaticFiles58F8B8[0]);
        }
        currentObject = &gLegacyWideStandardOutputBuffer10865B8;
        state = 1;
        if (currentObject != nullptr) {
            ConstructLegacyWideFileBuffer_54970A(
                &gLegacyWideStandardOutputBuffer10865B8, &gLegacyStaticFiles58F8B8[1]);
        }
        currentObject = &gLegacyWideStandardErrorBuffer10866A0;
        state = 2;
        if (currentObject != nullptr) {
            ConstructLegacyWideFileBuffer_54970A(
                &gLegacyWideStandardErrorBuffer10866A0, &gLegacyStaticFiles58F8B8[2]);
        }
        currentObject = &gLegacyWideStandardInput1086578;
        state = 3;
        if (currentObject != nullptr) {
            ConstructLegacyWideInput_549643(
                &gLegacyWideStandardInput1086578, &gLegacyWideStandardInputBuffer1086610.base, 1, 1);
        }
        currentObject = &gLegacyWideStandardOutput1086668;
        state = 4;
        if (currentObject != nullptr) {
            ConstructLegacyWideOutput_549525(
                &gLegacyWideStandardOutput1086668, &gLegacyWideStandardOutputBuffer10865B8.base, 1, 1, 1);
        }
        currentObject = &gLegacyWideStandardError1086500;
        ProjectStreamIos(&gLegacyWideStandardInput1086578)->tiedStream = &gLegacyWideStandardOutput1086668;
        state = 5;
        if (currentObject != nullptr) {
            ConstructLegacyWideOutput_549525(
                &gLegacyWideStandardError1086500, &gLegacyWideStandardErrorBuffer10866A0.base, 1, 1, 1);
        }
        currentObject = &gLegacyWideStandardLog1086540;
        ProjectStreamIos(&gLegacyWideStandardError1086500)->tiedStream = &gLegacyWideStandardOutput1086668;
        ProjectStreamIos(&gLegacyWideStandardError1086500)->base.formatBits |= 2u;
        state = 6;
        if (currentObject != nullptr) {
            ConstructLegacyWideOutput_549525(
                &gLegacyWideStandardLog1086540, &gLegacyWideStandardErrorBuffer10866A0.base, 1, 1, 1);
        }
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
    return std::atexit(DestroyLegacyNarrowErrorBufferAtExit_548275);
}

std::int32_t InitializeLegacyWideErrorStream_549339()
{
    RegisterLegacyWideErrorStreamLate_549343();
    return std::atexit(DestroyLegacyWideErrorStreamAtExit_54935E);
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
    return std::atexit(DestroyLegacyNarrowInputBufferAtExit_548193);
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
    return std::atexit(DestroyLegacyNarrowOutputBufferAtExit_5481C0);
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
    return std::atexit(DestroyLegacyWideInputBufferAtExit_54924D);
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
    return std::atexit(DestroyLegacyWideOutputBufferAtExit_54927A);
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
    return std::atexit(DestroyLegacyWideErrorBufferAtExit_5492A7);
}

void* ConstructLegacyNarrowStandardInitializer_5491E0()
{
    return InitializeLegacyNarrowStandardStreams_5482DD(&gLegacyNarrowStandardInitializer1086539);
}

std::int32_t RegisterLegacyNarrowStandardInitializer_5491D6()
{
    ConstructLegacyNarrowStandardInitializer_5491E0();
    return std::atexit(ReleaseLegacyNarrowStandardStreams_5491F6);
}

void* ConstructLegacyWideStandardInitializer_54920A()
{
    return InitializeLegacyWideStandardStreams_549397(&gLegacyWideStandardInitializer1086538);
}

std::int32_t RegisterLegacyWideStandardInitializer_549200()
{
    ConstructLegacyWideStandardInitializer_54920A();
    return std::atexit(ReleaseLegacyWideStandardStreams_549220);
}

LegacyNarrowInputStream* RegisterLegacyNarrowInputStreamLate_5481D4()
{
    return RegisterConstructLegacyNarrowInput_54859F(&gLegacyNarrowStandardInput1086430, 0, 1);
}

void DestroyLegacyNarrowInputStreamAtExit_5481EF()
{
    DestroyLegacyNarrowInput_5481F9(&gLegacyNarrowStandardInput1086430);
}

std::int32_t InitializeLegacyNarrowInputStream_5481CA()
{
    RegisterLegacyNarrowInputStreamLate_5481D4();
    return std::atexit(DestroyLegacyNarrowInputStreamAtExit_5481EF);
}

LegacyNarrowOutputStream* RegisterLegacyNarrowOutputStreamLate_548218()
{
    return RegisterConstructLegacyNarrowOutput_5484CD(&gLegacyNarrowStandardOutput10863A0, 0, 1);
}

void DestroyLegacyNarrowOutputStreamAtExit_548233()
{
    DestroyLegacyNarrowOutput_54823D(&gLegacyNarrowStandardOutput10863A0);
}

std::int32_t InitializeLegacyNarrowOutputStream_54820E()
{
    RegisterLegacyNarrowOutputStreamLate_548218();
    return std::atexit(DestroyLegacyNarrowOutputStreamAtExit_548233);
}

LegacyNarrowOutputStream* RegisterLegacyNarrowErrorStreamLate_548289()
{
    return RegisterConstructLegacyNarrowOutput_5484CD(&gLegacyNarrowStandardError10864C8, 0, 1);
}

void DestroyLegacyNarrowErrorStreamAtExit_5482A4()
{
    DestroyLegacyNarrowOutput_54823D(&gLegacyNarrowStandardError10864C8);
}

std::int32_t InitializeLegacyNarrowErrorStream_54827F()
{
    RegisterLegacyNarrowErrorStreamLate_548289();
    return std::atexit(DestroyLegacyNarrowErrorStreamAtExit_5482A4);
}

LegacyNarrowOutputStream* RegisterLegacyNarrowLogStreamLate_5482B8()
{
    return RegisterConstructLegacyNarrowOutput_5484CD(&gLegacyNarrowStandardLog1086310, 0, 1);
}

void DestroyLegacyNarrowLogStreamAtExit_5482D3()
{
    DestroyLegacyNarrowOutput_54823D(&gLegacyNarrowStandardLog1086310);
}

std::int32_t InitializeLegacyNarrowLogStream_5482AE()
{
    RegisterLegacyNarrowLogStreamLate_5482B8();
    return std::atexit(DestroyLegacyNarrowLogStreamAtExit_5482D3);
}

LegacyWideInputStream* RegisterLegacyWideInputStreamLate_5492BB()
{
    return RegisterConstructLegacyWideInput_5496AB(&gLegacyWideStandardInput1086578, 0, 1);
}

void DestroyLegacyWideInputStreamAtExit_5492D6()
{
    DestroyLegacyWideInput_5492E0(&gLegacyWideStandardInput1086578);
}

std::int32_t InitializeLegacyWideInputStream_5492B1()
{
    RegisterLegacyWideInputStreamLate_5492BB();
    return std::atexit(DestroyLegacyWideInputStreamAtExit_5492D6);
}

LegacyWideOutputStream* RegisterLegacyWideOutputStreamLate_5492FF()
{
    return RegisterConstructLegacyWideOutput_54958F(&gLegacyWideStandardOutput1086668, 0, 1);
}

void DestroyLegacyWideOutputStreamAtExit_54931A()
{
    DestroyLegacyWideOutput_549324(&gLegacyWideStandardOutput1086668);
}

std::int32_t InitializeLegacyWideOutputStream_5492F5()
{
    RegisterLegacyWideOutputStreamLate_5492FF();
    return std::atexit(DestroyLegacyWideOutputStreamAtExit_54931A);
}

LegacyWideOutputStream* RegisterLegacyWideLogStreamLate_549372()
{
    return RegisterConstructLegacyWideOutput_54958F(&gLegacyWideStandardLog1086540, 0, 1);
}

void DestroyLegacyWideLogStreamAtExit_54938D()
{
    DestroyLegacyWideOutput_549324(&gLegacyWideStandardLog1086540);
}

std::int32_t InitializeLegacyWideLogStream_549368()
{
    RegisterLegacyWideLogStreamLate_549372();
    return std::atexit(DestroyLegacyWideLogStreamAtExit_54938D);
}

namespace {

// 原三段已加载ASCII消息，含NUL字节保持；不是宿主异常消息替换。
const char gLegacyIosBadBitMessage55011C[16] = "ios::badbit set";
const char gLegacyIosFailBitMessage550108[17] = "ios::failbit set";
const char gLegacyIosEofBitMessage5500F8[16] = "ios::eofbit set";

} // namespace

void ClearLegacyIos_549F62(
    LegacyIosBaseStorage* const storage, const std::uint32_t stateBits,
    const std::uint32_t rethrowBits)
{
    const std::uint32_t publishedState = stateBits & 7u;
    storage->stateBits = publishedState;
    const std::uint32_t matchedExceptions = storage->exceptionBits & publishedState;
    if (matchedExceptions == 0u) {
        return;
    }
    if (static_cast<std::uint8_t>(rethrowBits) != 0u) {
        throw;
    }

    const char* messageText;
    if ((matchedExceptions & 4u) != 0u) {
        messageText = gLegacyIosBadBitMessage55011C;
    } else if ((matchedExceptions & 2u) != 0u) {
        messageText = gLegacyIosFailBitMessage550108;
    } else {
        messageText = gLegacyIosEofBitMessage5500F8;
    }
    LegacyNarrowString message;
    message.prefix00[0] = static_cast<std::uint8_t>(rethrowBits >> 24u);
    TidyLegacyNarrowString_548E29(&message, false);
    const std::uint32_t messageBytes = StrLen_53F9E0(messageText);
    AssignLegacyNarrowString_549055(&message, messageText, messageBytes);

    LegacyStreamFailureObject failure;
    try {
        // 原state0从549FD1开始，assign失败尚未进入此临时串展开保护区。
        ConstructLegacyRuntimeError_549FFE(&failure, &message);
        failure.base.methods = &gLegacyStreamFailureMethods5500EC.base;
        throw native_cpp::StreamFailure(native_cpp::AdoptPayload{}, failure);
    } catch (...) {
        // 原54C318→共享54A30D→548E29(true)，只清临时串，不额外析构failure。
        TidyLegacyNarrowString_548E29(&message, true);
        throw;
    }
    // NCE01本机throw无正常返回；临时串清理仍由上方原catch完成。
}

void* ConstructLegacyNarrowEarlyStandardInitializer_453E10()
{
    return InitializeLegacyNarrowStandardStreams_5482DD(&gLegacyNarrowEarlyStandardInitializer628FE9);
}

void ReleaseLegacyNarrowEarlyStandardStreamsAtExit_453E30()
{
    // 原this地址仅mov后尾跳共享计数/刷新chunk，未被共享本体消费；复用已过真实实现。
    ReleaseLegacyNarrowStandardStreams_5491F6();
}

std::int32_t InitializeLegacyNarrowEarlyStandardStreams_453E00()
{
    ConstructLegacyNarrowEarlyStandardInitializer_453E10();
    return std::atexit(ReleaseLegacyNarrowEarlyStandardStreamsAtExit_453E30);
}

void* ConstructLegacyWideEarlyStandardInitializer_453E50()
{
    return InitializeLegacyWideStandardStreams_549397(&gLegacyWideEarlyStandardInitializer628FE8);
}

void ReleaseLegacyWideEarlyStandardStreamsAtExit_453E70()
{
    // 原this地址仅mov后尾跳共享计数/刷新chunk，未被共享本体消费；复用已过真实实现。
    ReleaseLegacyWideStandardStreams_549220();
}

std::int32_t InitializeLegacyWideEarlyStandardStreams_453E40()
{
    ConstructLegacyWideEarlyStandardInitializer_453E50();
    return std::atexit(ReleaseLegacyWideEarlyStandardStreamsAtExit_453E70);
}

} // namespace cg::platform
