#include "LuaUI.hpp"

namespace cg::script {
void UpdateLuaScene(session::ClientSession& session) { ui::WindowRuntime_UpdateScene(session); }
void ShutdownLuaUI(session::ClientSession& session) {
    ShutdownLuaUIEvents(session);
    ui::ShutdownExtensionWindows(session);
}
int LuaPeekMouseWheelDelta() { return ui::WindowPeekMouseWheelDelta(); }
void LuaConsumeMouseWheelDelta() { ui::WindowConsumeMouseWheelDelta(); }
} // namespace cg::script
