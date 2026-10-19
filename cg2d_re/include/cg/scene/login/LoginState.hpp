#pragma once

#include <cstdint>
#include <string>

namespace cg::login {

// 原45D980复位、45D990推进的16位连接阶段。
extern std::uint16_t gServerConnectionPhase_9A6E3C; // 原 0x9A6E3C，映像初值 0。
extern std::int32_t gSelectedLoginServerGroup_55A054;  // 原 0x55A054，映像初值 -1；跨登录场景共享。
extern std::wstring gLoginProcessingErrorMessage_9A5348;      // 原 0x9A5348，映像零填充；内部持有动态 UTF-16。

// 原 sub_45D980@0x45D980；依次清零两个 16 位存储并返回 0。
std::int32_t ResetValues_45D980();

// 原 sub_45D990@0x45D990；推进服务器连接阶段并按条件绘制连接提示。
std::int32_t UpdateServerConnection();

// 原 sub_45E960@0x45E960；顺序处理当前场景子状态 0、1、100、101。
std::int32_t ProcessState_45E960();

// 原 sub_45EA50@0x45EA50；只清进度阶段，不清进度对象。
void ResetLoginProgress();

// 原 sub_45EA60@0x45EA60；推进登录进度并按条件绘制“登入中”。
std::int32_t UpdateLoginProgress();

} // namespace cg::login
