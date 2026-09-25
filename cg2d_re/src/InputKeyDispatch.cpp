#include "InputKeyDispatchDependencies.hpp"

#include "LoginMessageDependencies.hpp"

#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"

namespace cg::input {

// 原 sub_4596D0，主块 [0x4596D0,0x45975F)，尾块 [0x401920,0x401953)。
void ProcessKeyInput_4596D0()
{
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
        cg::NullSub_509080();
    }
    if ((platform::gKeyStates[13] & 0x10u) != 0) {
        detail::Sub_44AB60();
    }

    if ((platform::gKeyStates[35] & 0x04u) == 0) {
        return;
    }

    if (audio::Sub_5043D0(2) != 0) {
        static_cast<void>(audio::Sub_5042D0(100u));
        static_cast<void>(login::detail::PlaySe(57, 320, 240));
        return;
    }
    static_cast<void>(audio::Sub_504180(99u));
}

} // namespace cg::input
