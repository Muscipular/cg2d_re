#include "cg/session/ClientSession.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "InputKeyDispatchDependencies.hpp"
#include "cg/core/m_log.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/input/Input.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/KeyState.hpp"

namespace cg::input {

// 原 sub_4596D0，主块 [0x4596D0,0x45975F)，尾块 [0x401920,0x401953)。
void ProcessKeyInput_4596D0(session::ClientSession& session)
{
    try {
        if ((session.UiFrame().keyStates[17] & 0x02u) == 0
            && (session.UiFrame().keyStates[16] & 0x02u) == 0) {
            if ((session.UiFrame().keyStates[38] & 0x10u) != 0) {
                detail::MoveInputCaretUp_44A5D0(session);
            }
            if ((session.UiFrame().keyStates[40] & 0x10u) != 0) {
                detail::MoveInputCaretDown_44A7D0(session);
            }
            if ((session.UiFrame().keyStates[37] & 0x10u) != 0) {
                detail::MoveInputCaretLeft_44A540(session);
            }
            if ((session.UiFrame().keyStates[39] & 0x10u) != 0) {
                detail::MoveInputCaretRight_44A580(session);
            }
        }

        if ((session.UiFrame().keyStates[8] & 0x10u) != 0) {
            detail::DeleteCharacterBeforeInputCaret_44A990(session);
        }
        if ((session.UiFrame().keyStates[46] & 0x10u) != 0) {
            detail::DeleteCharacterAtInputCaret_44AA90(session);
        }
        if ((session.UiFrame().keyStates[9] & 0x10u) != 0) {
            cg::core::NullSub_509080();
        }
        if ((session.UiFrame().keyStates[13] & 0x10u) != 0) {
            detail::HandleInputEnterKey_44AB60(session);
        }

    } catch (const cg::text::GbkStorageError&) {
        m_log_iw(L"文字无法按GBK保存，或已超出字段容量。");
    }
}

} // namespace cg::input
