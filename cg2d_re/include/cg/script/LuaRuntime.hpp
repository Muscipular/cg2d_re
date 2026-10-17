#pragma once

#include <cstdint>

namespace cg::script {

// LUM01：仅主线程调用；在资源/UI准备后初始化，在资源销毁前关闭。
bool InitializeLua();
void ShutdownLua();

} // namespace cg::script
