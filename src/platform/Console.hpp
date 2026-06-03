#pragma once

namespace cg::platform {

// ConfigureConsoleUtf8 将 Windows 控制台输入/输出切到 UTF-8；非 Windows 平台为空操作。
void ConfigureConsoleUtf8() noexcept;

} // namespace cg::platform
