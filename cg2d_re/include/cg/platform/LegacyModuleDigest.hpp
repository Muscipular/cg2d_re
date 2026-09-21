#pragma once

namespace cg::platform {

// 原 sub_5202E0@[0x5202E0,0x5203A3)：计算当前可执行模块的 MD5 小写文本。
// output 为 nullptr 时无副作用；其他值须满足证据文档记录的可写容量前提。
char* ComputeExecutableDigest_5202E0(char* output);

} // namespace cg::platform
