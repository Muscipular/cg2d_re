#pragma once

#include "LegacyExternalKeyboardDependencies.hpp"

namespace cg::platform {

// JoyparkKeyboard 的唯一原生对象定义；Create/Delete 共用此 owner，不套用 x86 布局。
// 此私有具体对象不作为可继承基类；保留删除和尺寸虚方法，不为此新增空虚析构。
struct ExternalKeyboard final {
    // 原 sub_535090：仅声明真实构造入口，不提供默认构造或伪造字段初值。
    ExternalKeyboard(
        std::uintptr_t instance, std::uintptr_t window, const char* initialText);

    // 原 sub_535310：真实清理仍未实现；原 EH 仅清理不捕获，允许异常继续传播。
    ~ExternalKeyboard() noexcept(false);

    // 原虚表首项 sub_535290：先析构，flags 低字节 bit 0 决定释放，返回原对象指针。
    virtual ExternalKeyboard* Delete(std::uint32_t flags);

    // 原虚表 +0x20 的 sub_537DC0；通过 native 虚调用返回实际输出指针。
    virtual ExternalKeyboardSize* QuerySize(ExternalKeyboardSize* output);

    // 原 +0x17C/+0x180 两个 W32；真实写者 sub_537DE0 仅在 MoveWindow 成功时赋值。
    ExternalKeyboardSize size;
    // 原 [+0x190,+0x294) 的 0x104 字节物理保留区，不是已证逻辑输入上限。
    char result[0x104];
};

} // namespace cg::platform
