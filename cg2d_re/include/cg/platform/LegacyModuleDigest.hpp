#pragma once

namespace cg::platform {

// 原 sub_5202E0@[0x5202E0,0x5203A3)：计算当前可执行模块的 MD5 小写文本。
// 使用Windows CNG分块计算；output须有至少33字节可写空间。
// output为nullptr时无副作用；文件打开或CNG失败时返回nullptr，并清空非空output。
char* ComputeExecutableDigest_5202E0(char* output);

} // namespace cg::platform
