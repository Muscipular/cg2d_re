#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace cg::network {
class CgConnection;
}

namespace cg::login {

namespace detail { struct SavedStateStorage; }

// 登录草稿、当前记录与槽映射由会话独占；磁盘格式仍留在存档实现。
struct SavedLoginRuntimeState {
    SavedLoginRuntimeState();
    ~SavedLoginRuntimeState();
    SavedLoginRuntimeState(const SavedLoginRuntimeState&) = delete;
    SavedLoginRuntimeState& operator=(const SavedLoginRuntimeState&) = delete;
    SavedLoginRuntimeState(SavedLoginRuntimeState&&) = delete;
    SavedLoginRuntimeState& operator=(SavedLoginRuntimeState&&) = delete;

    std::unique_ptr<detail::SavedStateStorage> data;
    std::int32_t selectedSlot = 0; // 原105C394，登录控件的可写选择。
    std::int32_t visibleSlotCount = 0; // 原105C364，所属登录入口重建。
    std::uint8_t loadErrorCode = 0; // 原FF4E28，载入入口清零、初始化读取。
};

// 原 sub_45D980@0x45D980；依次清零两个 16 位存储并返回 0。
std::int32_t ResetValues_45D980(network::CgConnection& connection);

// 原 sub_45D990@0x45D990；推进服务器连接阶段并按条件绘制连接提示。
std::int32_t UpdateServerConnection(network::CgConnection& connection);

// 原 sub_45E960@0x45E960；顺序处理当前场景子状态 0、1、100、101。
std::int32_t ProcessState_45E960(network::CgConnection& connection);

// 原 sub_45EA50@0x45EA50；只清进度阶段，不清进度对象。
void ResetLoginProgress(network::CgConnection& connection);

// 原 sub_45EA60@0x45EA60；推进登录进度并按条件绘制“登入中”。
std::int32_t UpdateLoginProgress(network::CgConnection& connection);

} // namespace cg::login
