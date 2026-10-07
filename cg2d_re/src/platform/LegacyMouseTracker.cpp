#include "cg/platform/LegacyMouseTracker.hpp"

#include <array>
#include <cstdlib>
#include <cstring>

namespace cg::platform {

std::int32_t gLegacyMouseTrackerThreshold = 50;
std::int32_t gLegacyMouseTrackerCapacity = 100;

namespace {

using Node = LegacyMouseTrackerNode;

constexpr std::int32_t kDefaultThreshold = 50;
constexpr std::int32_t kDefaultCapacity = 100;
constexpr std::int32_t kMinimumDynamicCapacity = 10;
constexpr std::int32_t kMaximumDynamicCapacity = 5000;

// 原固定池字节观察采用 x86 步长，不能使用本机指针拓宽后的 sizeof(Node)。
constexpr std::size_t kOriginalNodeSize = 0x18;
constexpr std::size_t kOriginalNextOffset = 0x10;
constexpr std::size_t kOriginalPrevOffset = 0x14;
static_assert(kOriginalNodeSize * kDefaultCapacity == 0x960);

struct LegacyMouseTrackerStorage {
    std::array<Node, kDefaultCapacity> fixedPool {};
    Node* nativePool {fixedPool.data()};
    Node freeHead {};
    Node freeTail {};
    Node activeHead {};
    Node activeTail {};

    LegacyMouseTrackerStorage() = default;
    LegacyMouseTrackerStorage(const LegacyMouseTrackerStorage&) = delete;
    LegacyMouseTrackerStorage& operator=(const LegacyMouseTrackerStorage&) = delete;
};

LegacyMouseTrackerStorage gTracker;

// 原 sub_5066F0@[0x5066F0,0x506720)：把第二个节点插到第一个节点之后。
std::int32_t InsertAfter_5066F0(Node* const anchor, Node* const inserted)
{
    if (anchor == nullptr || inserted == nullptr) {
        return 0;
    }

    Node* const next = anchor->next;
    if (next == nullptr) {
        return 0;
    }

    next->prev = inserted;
    anchor->next = inserted;
    inserted->prev = anchor;
    inserted->next = next;
    return 1;
}

// 原 sub_506720@[0x506720,0x506757)：从双链中摘除节点并清空两个链接槽。
std::int32_t Detach_506720(Node* const node)
{
    if (node == nullptr) {
        return 0;
    }

    Node* const next = node->next;
    if (next == nullptr) {
        return 0;
    }

    Node* const prev = node->prev;
    if (prev == nullptr) {
        return 0;
    }

    next->prev = prev;
    prev->next = next;
    node->prev = nullptr;
    node->next = nullptr;
    return 1;
}

// 原 sub_506760@[0x506760,0x506780)：取空闲链首节点。
Node* TakeFreeHead_506760()
{
    Node* const node = gTracker.freeHead.next;
    if (node == &gTracker.freeTail) {
        return nullptr;
    }
    static_cast<void>(Detach_506720(node));
    return node;
}

// 原 sub_506780@[0x506780,0x5067A0)：取活动链尾节点作为淘汰项。
Node* EvictActiveTail_506780()
{
    Node* const node = gTracker.activeTail.prev;
    if (node == &gTracker.activeHead) {
        return nullptr;
    }
    static_cast<void>(Detach_506720(node));
    return node;
}

// 原 sub_5067A0@[0x5067A0,0x5067E2)：分配、初始化并插入活动链首。
std::int32_t AllocateAndActivate_5067A0(
    const std::int32_t x,
    const std::int32_t y,
    const std::int32_t tag)
{
    Node* node = TakeFreeHead_506760();
    if (node == nullptr) {
        node = EvictActiveTail_506780();
    }
    if (node == nullptr) {
        return 0;
    }

    node->x = x;
    node->y = y;
    node->tag = tag;
    node->count = 1;
    static_cast<void>(InsertAfter_5066F0(&gTracker.activeHead, node));
    return 1;
}

} // namespace

std::uint8_t ReadLegacyMouseTrackerFixedByte(const std::size_t byteOffset)
{
    const Node& node = gTracker.fixedPool[byteOffset / kOriginalNodeSize];
    const std::size_t fieldOffset = byteOffset % kOriginalNodeSize;
    if (fieldOffset < kOriginalNextOffset) {
        return reinterpret_cast<const std::uint8_t*>(&node)[fieldOffset];
    }

    // 原 24B 观察视图只投影固定池/哨兵身份，不截断或写回本机链接。
    const Node* const link = fieldOffset < kOriginalPrevOffset
        ? node.next : node.prev;
    std::uint32_t originalLink;
    if (link == nullptr) {
        originalLink = 0;
    } else if (link == &gTracker.freeHead) {
        originalLink = 0xD68AD0u;
    } else if (link == &gTracker.freeTail) {
        originalLink = 0xD69730u;
    } else if (link == &gTracker.activeHead) {
        originalLink = 0xD69750u;
    } else if (link == &gTracker.activeTail) {
        originalLink = 0xD69770u;
    } else {
        const std::uint32_t fixedIndex = static_cast<std::uint32_t>(
            link - gTracker.fixedPool.data());
        originalLink = 0xD68DD0u
            + fixedIndex * static_cast<std::uint32_t>(kOriginalNodeSize);
    }
    const std::size_t linkByte =
        (fieldOffset - kOriginalNextOffset) % sizeof(std::uint32_t);
    return static_cast<std::uint8_t>(originalLink >> (linkByte * 8));
}

std::int32_t ResetLegacyMouseTracker_506580()
{
    Node* const oldPool = gTracker.nativePool;
    if (oldPool != gTracker.fixedPool.data() && oldPool != nullptr) {
        std::free(oldPool);
    }

    const std::int32_t capacity = gLegacyMouseTrackerCapacity;
    Node* selectedPool = nullptr;
    if (capacity != kDefaultCapacity
        && capacity >= kMinimumDynamicCapacity
        && capacity < kMaximumDynamicCapacity) {
        const std::size_t byteCount =
            sizeof(Node) * static_cast<std::size_t>(capacity);
        selectedPool = static_cast<Node*>(std::malloc(byteCount));
        // 原 5065C6 在判定分配结果前发布，失败后才回退固定池。
        gTracker.nativePool = selectedPool;
    }

    std::int32_t clearingCapacity;
    if (selectedPool == nullptr) {
        selectedPool = gTracker.fixedPool.data();
        clearingCapacity = kDefaultCapacity;
        gTracker.nativePool = selectedPool;
        gLegacyMouseTrackerCapacity = clearingCapacity;
    } else {
        clearingCapacity = gLegacyMouseTrackerCapacity;
    }

    std::memset(selectedPool, 0,
        sizeof(Node) * static_cast<std::size_t>(clearingCapacity));

    if (gLegacyMouseTrackerThreshold < 5) {
        gLegacyMouseTrackerThreshold = kDefaultThreshold;
    }

    const std::int32_t initialCapacity = gLegacyMouseTrackerCapacity;
    Node* sampledPool = gTracker.nativePool;
    gTracker.activeHead.prev = nullptr;
    gTracker.activeHead.next = &gTracker.activeTail;
    gTracker.activeTail.next = nullptr;
    gTracker.activeTail.prev = &gTracker.activeHead;
    gTracker.freeHead.prev = nullptr;
    gTracker.freeTail.next = nullptr;
    gTracker.freeTail.prev = &gTracker.freeHead;
    gTracker.freeHead.next = sampledPool;

    std::int32_t index = 0;
    if (initialCapacity > 0) {
        do {
            Node* const node = sampledPool + index;
            node->prev = index == 0 ? &gTracker.freeHead : node - 1;
            const std::int32_t lastIndex = gLegacyMouseTrackerCapacity - 1;
            if (index == lastIndex) {
                gTracker.nativePool[index].next = &gTracker.freeTail;
            } else {
                Node* const nextPool = gTracker.nativePool;
                nextPool[index].next = nextPool + index + 1;
            }
            const std::int32_t nextCapacity = gLegacyMouseTrackerCapacity;
            ++index;
            if (index >= nextCapacity) {
                break;
            }
            sampledPool = gTracker.nativePool;
        } while (true);
    }
    return index;
}

std::int32_t ResetLegacyMouseTrackerDefaults_5066D0()
{
    gLegacyMouseTrackerThreshold = kDefaultThreshold;
    gLegacyMouseTrackerCapacity = kDefaultCapacity;
    return ResetLegacyMouseTracker_506580();
}

std::int32_t ConfigureLegacyMouseTracker_50E7B0(
    std::uintptr_t,
    const std::int32_t capacity,
    const std::int32_t threshold)
{
    gLegacyMouseTrackerCapacity = capacity;
    gLegacyMouseTrackerThreshold = threshold;
    return ResetLegacyMouseTracker_506580();
}

std::int32_t TrackLegacyMouseDown_5067F0(
    const std::int32_t x,
    const std::int32_t y,
    const std::int32_t tag)
{
    Node* current = gTracker.activeHead.next;
    bool suppressIncrement = tag == 0;
    if (current == nullptr) {
        return 0;
    }

    Node* next = current->next;
    while (next != nullptr) {
        if (current->x == x && current->y == y && current->tag == tag) {
            if (!suppressIncrement) {
                ++current->count;
            }
            static_cast<void>(Detach_506720(current));
            static_cast<void>(InsertAfter_5066F0(
                &gTracker.activeHead, current));
            return current->count;
        }

        current = next;
        suppressIncrement = false;
        next = current->next;
    }

    static_cast<void>(AllocateAndActivate_5067A0(x, y, tag));
    return 1;
}

} // namespace cg::platform
