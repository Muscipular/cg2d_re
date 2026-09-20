#pragma once

namespace cg::platform {

// 原 sub_458FD0@[0x458FD0,0x459032)：初始化并关联 IME context。
int InitializeIme_458FD0();

// 原 sub_459040@[0x459040,0x459081)：恢复旧 context 并释放候选表。
void ShutdownIme_459040();

} // namespace cg::platform
