#include "LuaUI.hpp"

namespace cg::script {
void UpdateLuaScene() { ui::WindowRuntime_UpdateScene(); }
void ShutdownLuaUI() {
    ShutdownLuaUIEvents();
    ui::ShutdownExtensionWindows();
}
int LuaPeekMouseWheelDelta() { return ui::WindowPeekMouseWheelDelta(); }
void LuaConsumeMouseWheelDelta() { ui::WindowConsumeMouseWheelDelta(); }
} // namespace cg::script
