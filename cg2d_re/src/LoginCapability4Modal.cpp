#include "LoginCapability4Modal.hpp"

#include <cstdint>

namespace {

// 原 dword_9A57E4@0x9A57E4，PE 虚拟零填充。
std::int32_t gCapability4ModalInitialized = 0;

} // namespace

namespace cg::login::detail {

void ResetCapability4Modal_464440()
{
    gCapability4ModalInitialized = 0;
}

} // namespace cg::login::detail
