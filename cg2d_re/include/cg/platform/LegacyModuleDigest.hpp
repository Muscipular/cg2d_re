#pragma once
#include <string>

namespace cg::platform {

// 原 sub_5202E0@[0x5202E0,0x5203A3)：计算当前可执行模块的 MD5 小写文本。
// 使用Windows CNG分块计算；成功写入32位宽字符文本，失败清空并返回nullptr。
const wchar_t* ComputeExecutableDigest_5202E0(std::wstring& output);

} // namespace cg::platform
