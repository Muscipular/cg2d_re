#include "LegacyIos.hpp"

#include "LegacyHeap.hpp"

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

} // namespace cg::platform
