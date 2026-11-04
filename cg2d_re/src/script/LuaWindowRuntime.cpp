#include "LuaUI.hpp"

namespace cg::script {
void UpdateLuaScene(session::ClientSession& session) { ui::WindowRuntime_UpdateScene(session); }
void ShutdownLuaUI(session::ClientSession& session) {
    ShutdownLuaUIEvents(session);
    ui::ShutdownExtensionWindows(session);
}
int LuaPeekMouseWheelDelta(session::ClientSession& session) { return ui::WindowPeekMouseWheelDelta(session); }
void LuaConsumeMouseWheelDelta(session::ClientSession& session) { ui::WindowConsumeMouseWheelDelta(session); }
} // namespace cg::script
