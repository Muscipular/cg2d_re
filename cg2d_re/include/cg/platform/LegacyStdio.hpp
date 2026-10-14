#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cwchar>

namespace cg::platform {

// MC01-B：所有文件指针直接借用MSVC FILE，禁止读取或复制其私有布局。
using LegacyFile = std::FILE;

// 原CRT提供者：活动调用已直接使用标准库；无消费者服务不补业务调用。
// 原0x0053B2F1：std::fopen。
// 原0x0053B2D1：::_fsopen，当前无CPP消费者。
// 原0x0053C8B3：std::fgetc。
// 原0x0053B226：std::fgets。
// 原0x0053BCCD：std::fread。
// 原0x0053C680：std::fwrite。
// 原0x0053CFDC：std::fputc。
// 原0x0053B356：std::fputs。
// 原0x0053CB78：std::ftell。
// 原0x0053BC41：std::fseek。
// 原0x0053B27D：std::rewind。
// 原0x0054ACF1：std::setvbuf，当前无CPP消费者。
// 原0x0054463F：std::ungetc，当前无CPP消费者。
// 原0x0054B239：::_ftelli64，当前无CPP消费者。
// 原0x0054B3C0：::_fseeki64，当前无CPP消费者。
// 原0x0054ACB7：std::fgetpos，当前无CPP消费者。
// 原0x0054ACD9：std::fsetpos，当前无CPP消费者。
// 原0x0054AD92：std::fputwc，当前无CPP消费者。
// 原0x0054AF70：std::fgetwc，当前无CPP消费者。
// 原0x0054AE87：std::ungetwc，当前无CPP消费者。
// 原0x0053BB02：std::fflush。
// 原0x0053B1D0：std::fclose。
// 原0x0053BB99：::_flushall，当前无CPP消费者。
// 原0x005446AD：::_fcloseall，当前无CPP消费者。

} // namespace cg::platform
