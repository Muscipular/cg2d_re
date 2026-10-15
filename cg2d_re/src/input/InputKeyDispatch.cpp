#include "cg/core/LegacyNoOp.hpp"
#include "InputKeyDispatchDependencies.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/input/Input.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/KeyState.hpp"

namespace cg::input {

// 原 sub_4596D0，主块 [0x4596D0,0x45975F)，尾块 [0x401920,0x401953)。
void ProcessKeyInput_4596D0()
{
    try {
        if ((platform::gKeyStates[17] & 0x02u) == 0
            && (platform::gKeyStates[16] & 0x02u) == 0) {
            if ((platform::gKeyStates[38] & 0x10u) != 0) {
                detail::Sub_44A5D0();
            }
            if ((platform::gKeyStates[40] & 0x10u) != 0) {
                detail::Sub_44A7D0();
            }
            if ((platform::gKeyStates[37] & 0x10u) != 0) {
                detail::Sub_44A540();
            }
            if ((platform::gKeyStates[39] & 0x10u) != 0) {
                detail::Sub_44A580();
            }
        }

        if ((platform::gKeyStates[8] & 0x10u) != 0) {
            detail::Sub_44A990();
        }
        if ((platform::gKeyStates[46] & 0x10u) != 0) {
            detail::Sub_44AA90();
        }
        if ((platform::gKeyStates[9] & 0x10u) != 0) {
            cg::core::NullSub_509080();
        }
        if ((platform::gKeyStates[13] & 0x10u) != 0) {
            detail::Sub_44AB60();
        }

    } catch (const cg::text::GbkStorageError&) {
        platform::ShowWideMessage(L"文字无法按GBK保存，或已超出字段容量。", L"文字编码", 0x10U);
    }
}

} // namespace cg::input
