#pragma once

#include <cstdint>

namespace cg::platform {

// 原 JoyparkKeyboard：布局和虚表由真实对象层实现，本边界不套用 x86 对象字节布局。
struct ExternalKeyboard;

// 原 sub_537DC0 输出的两个 W32；供 DialogFunc 两次独立尺寸虚调用使用。
struct ExternalKeyboardSize {
    std::int32_t width;  // 原返回指针 [0]：对象宽度。
    std::int32_t height; // 原返回指针 [1]：对象高度。
};

// 组合原 0x534D56 的 raw 分配、sub_535090 构造及 0x54BC30 的构造异常释放。
// x64 分配真实原生对象，不硬套原 0x2C8；分配空值仍返回空值，构造异常清理后继续传播。
ExternalKeyboard* CreateExternalKeyboard_535090(
    std::uintptr_t instance, std::uintptr_t window, const char* initialText);

// 原虚表首项 sub_535290：先析构，flags bit 0 决定释放，返回原对象指针。
// 必须保留对象的真实虚调用；DialogFunc 传 flags=1，忽略返回且不清共享对象指针。
ExternalKeyboard* DeleteExternalKeyboardVirtual_535290(
    ExternalKeyboard* object, std::uint32_t flags);

// 原虚表 +0x20 项 sub_537DC0：写出宽高并返回实际结果指针，不假定返回值等于 output。
ExternalKeyboardSize* QueryExternalKeyboardSizeVirtual_537DC0(
    ExternalKeyboard* object, ExternalKeyboardSize* output);

// 原 sub_5354C0：返回对象真实结果旧字节串；不以空串或固定值代替。
char* GetExternalKeyboardResult_5354C0(ExternalKeyboard* object);

} // namespace cg::platform
