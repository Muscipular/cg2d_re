#pragma once

namespace cg::core {

// UTE01：原仅单RET，无读取/写入，真实空操作保持；其它父残值机器ABI待。
// 原函数：0x00509080。
inline void NullSub_509080() noexcept
{
}

} // namespace cg::core
