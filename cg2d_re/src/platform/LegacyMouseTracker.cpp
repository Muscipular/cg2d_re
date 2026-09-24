#include "cg/platform/LegacyMouseTracker.hpp"

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>

namespace cg::platform {

std::int32_t gLegacyMouseTrackerThreshold = 50;
std::int32_t gLegacyMouseTrackerCapacity = 100;

namespace {

using Node = LegacyMouseTrackerNode32;
using NodeHandle = std::uint32_t;

constexpr std::int32_t kDefaultThreshold = 50;
constexpr std::int32_t kDefaultCapacity = 100;
constexpr std::int32_t kMinimumDynamicCapacity = 10;
constexpr std::int32_t kMaximumDynamicCapacity = 5000;

constexpr NodeHandle kNullHandle = 0;
constexpr NodeHandle kFreeHeadHandle = 0xFFFFFFFCu;
constexpr NodeHandle kFreeTailHandle = 0xFFFFFFFDu;
constexpr NodeHandle kActiveHeadHandle = 0xFFFFFFFEu;
constexpr NodeHandle kActiveTailHandle = 0xFFFFFFFFu;

struct LegacyMouseTrackerStorage {
    std::array<Node, kDefaultCapacity> fixedPool {};
    Node* nativePool {fixedPool.data()};
    // 仅用于校验 W32 handle 的当前本机分配范围，不是第二份 legacy capacity。
    std::uint32_t nativePoolNodeCount {
        static_cast<std::uint32_t>(kDefaultCapacity)
    };
    Node freeHead {};
    Node freeTail {};
    Node activeHead {};
    Node activeTail {};

    LegacyMouseTrackerStorage() = default;
    LegacyMouseTrackerStorage(const LegacyMouseTrackerStorage&) = delete;
    LegacyMouseTrackerStorage& operator=(const LegacyMouseTrackerStorage&) = delete;
};

static_assert(sizeof(std::array<Node, kDefaultCapacity>) == 0x960);

LegacyMouseTrackerStorage gTracker;

NodeHandle NodeHandleAt(const std::int32_t index)
{
    return static_cast<NodeHandle>(index) + 1u;
}

Node* ResolveNode(const NodeHandle handle)
{
    switch (handle) {
    case kFreeHeadHandle:
        return &gTracker.freeHead;
    case kFreeTailHandle:
        return &gTracker.freeTail;
    case kActiveHeadHandle:
        return &gTracker.activeHead;
    case kActiveTailHandle:
        return &gTracker.activeTail;
    default:
        break;
    }

    if (handle == kNullHandle || gTracker.nativePool == nullptr
        || handle > gTracker.nativePoolNodeCount) {
        return nullptr;
    }
    return &gTracker.nativePool[handle - 1u];
}

// 原 sub_5066F0@[0x5066F0,0x506720)：把第二个节点插到第一个节点之后。
std::int32_t InsertAfter_5066F0(
    const NodeHandle anchorHandle,
    const NodeHandle insertedHandle)
{
    Node* const anchor = ResolveNode(anchorHandle);
    Node* const inserted = ResolveNode(insertedHandle);
    if (anchor == nullptr || inserted == nullptr) {
        return 0;
    }

    const NodeHandle nextHandle = anchor->next;
    Node* const next = ResolveNode(nextHandle);
    if (next == nullptr) {
        return 0;
    }

    next->prev = insertedHandle;
    anchor->next = insertedHandle;
    inserted->prev = anchorHandle;
    inserted->next = nextHandle;
    return 1;
}

// 原 sub_506720@[0x506720,0x506757)：从双链中摘除节点并清空两个链接槽。
std::int32_t Detach_506720(const NodeHandle handle)
{
    Node* const node = ResolveNode(handle);
    if (node == nullptr) {
        return 0;
    }

    const NodeHandle nextHandle = node->next;
    Node* const next = ResolveNode(nextHandle);
    if (next == nullptr) {
        return 0;
    }

    const NodeHandle prevHandle = node->prev;
    Node* const prev = ResolveNode(prevHandle);
    if (prev == nullptr) {
        return 0;
    }

    next->prev = prevHandle;
    prev->next = nextHandle;
    node->prev = kNullHandle;
    node->next = kNullHandle;
    return 1;
}

// 原 sub_506760@[0x506760,0x506780)：取空闲链首节点。
NodeHandle TakeFreeHead_506760()
{
    const NodeHandle handle = gTracker.freeHead.next;
    if (handle == kFreeTailHandle || ResolveNode(handle) == nullptr) {
        return kNullHandle;
    }
    static_cast<void>(Detach_506720(handle));
    return handle;
}

// 原 sub_506780@[0x506780,0x5067A0)：取活动链尾节点作为淘汰项。
NodeHandle EvictActiveTail_506780()
{
    const NodeHandle handle = gTracker.activeTail.prev;
    if (handle == kActiveHeadHandle || ResolveNode(handle) == nullptr) {
        return kNullHandle;
    }
    static_cast<void>(Detach_506720(handle));
    return handle;
}

// 原 sub_5067A0@[0x5067A0,0x5067E2)：分配、初始化并插入活动链首。
std::int32_t AllocateAndActivate_5067A0(
    const std::int32_t x,
    const std::int32_t y,
    const std::int32_t tag)
{
    NodeHandle handle = TakeFreeHead_506760();
    if (handle == kNullHandle) {
        handle = EvictActiveTail_506780();
    }

    Node* const node = ResolveNode(handle);
    if (node == nullptr) {
        return 0;
    }

    node->x = x;
    node->y = y;
    node->tag = tag;
    node->count = 1;
    static_cast<void>(InsertAfter_5066F0(kActiveHeadHandle, handle));
    return 1;
}

void IncrementCount(Node& node)
{
    const std::uint32_t bits = std::bit_cast<std::uint32_t>(node.count);
    node.count = std::bit_cast<std::int32_t>(bits + 1u);
}

} // namespace

std::int32_t ResetLegacyMouseTracker_506580()
{
    if (gTracker.nativePool != gTracker.fixedPool.data()
        && gTracker.nativePool != nullptr) {
        std::free(gTracker.nativePool);
    }

    Node* allocatedPool = nullptr;
    if (gLegacyMouseTrackerCapacity != kDefaultCapacity
        && gLegacyMouseTrackerCapacity >= kMinimumDynamicCapacity
        && gLegacyMouseTrackerCapacity < kMaximumDynamicCapacity) {
        const std::size_t byteCount =
            sizeof(Node)
            * static_cast<std::size_t>(gLegacyMouseTrackerCapacity);
        allocatedPool = static_cast<Node*>(std::malloc(byteCount));
    }

    if (allocatedPool != nullptr) {
        gTracker.nativePool = allocatedPool;
        gTracker.nativePoolNodeCount =
            static_cast<std::uint32_t>(gLegacyMouseTrackerCapacity);
    } else {
        gTracker.nativePool = gTracker.fixedPool.data();
        gTracker.nativePoolNodeCount =
            static_cast<std::uint32_t>(kDefaultCapacity);
        gLegacyMouseTrackerCapacity = kDefaultCapacity;
    }

    const std::size_t byteCount =
        sizeof(Node)
        * static_cast<std::size_t>(gLegacyMouseTrackerCapacity);
    std::memset(gTracker.nativePool, 0, byteCount);

    if (gLegacyMouseTrackerThreshold < 5) {
        gLegacyMouseTrackerThreshold = kDefaultThreshold;
    }

    gTracker.activeHead.prev = kNullHandle;
    gTracker.activeHead.next = kActiveTailHandle;
    gTracker.activeTail.next = kNullHandle;
    gTracker.activeTail.prev = kActiveHeadHandle;
    gTracker.freeHead.prev = kNullHandle;
    gTracker.freeTail.next = kNullHandle;
    gTracker.freeTail.prev = kFreeHeadHandle;
    gTracker.freeHead.next = NodeHandleAt(0);

    for (std::int32_t index = 0;
         index < gLegacyMouseTrackerCapacity;
         ++index) {
        Node& node = gTracker.nativePool[index];
        node.prev = index == 0 ? kFreeHeadHandle : NodeHandleAt(index - 1);
        node.next = index == gLegacyMouseTrackerCapacity - 1
            ? kFreeTailHandle
            : NodeHandleAt(index + 1);
    }

    return gLegacyMouseTrackerCapacity;
}

std::int32_t ResetLegacyMouseTrackerDefaults_5066D0()
{
    gLegacyMouseTrackerThreshold = kDefaultThreshold;
    gLegacyMouseTrackerCapacity = kDefaultCapacity;
    return ResetLegacyMouseTracker_506580();
}

std::int32_t TrackLegacyMouseDown_5067F0(
    const std::int32_t x,
    const std::int32_t y,
    const std::int32_t tag)
{
    NodeHandle currentHandle = gTracker.activeHead.next;
    bool suppressIncrement = tag == 0;
    if (currentHandle == kNullHandle) {
        return 0;
    }

    Node* current = ResolveNode(currentHandle);
    if (current == nullptr) {
        return 0;
    }

    NodeHandle nextHandle = current->next;
    while (nextHandle != kNullHandle) {
        if (current->x == x && current->y == y && current->tag == tag) {
            if (!suppressIncrement) {
                IncrementCount(*current);
            }
            static_cast<void>(Detach_506720(currentHandle));
            static_cast<void>(InsertAfter_5066F0(
                kActiveHeadHandle, currentHandle));
            return current->count;
        }

        currentHandle = nextHandle;
        suppressIncrement = false;
        current = ResolveNode(currentHandle);
        if (current == nullptr) {
            return 0;
        }
        nextHandle = current->next;
    }

    static_cast<void>(AllocateAndActivate_5067A0(x, y, tag));
    return 1;
}

} // namespace cg::platform
