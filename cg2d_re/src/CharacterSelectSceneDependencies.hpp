#pragma once

#include "LoginMessageDependencies.hpp"
#include "world/MailState.hpp"

#include "cg/LegacyAudio.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace cg::character_select::detail {

// 原 UI 记录严格由 7 个 32 位整数组成；接口保留强类型原生指针。
struct CharacterSelectUiRecord {
    std::array<std::int32_t, 7> values; // 原成员偏移：+0x00。
};

static_assert(sizeof(CharacterSelectUiRecord) == 0x1C);

// sub_462090 的六个定宽坐标字段和一个原生文本指针。
struct CharacterModalUiRecord {
    std::int32_t textX; // 原成员偏移：+0x00。
    std::int32_t textY; // 原成员偏移：+0x04。
    std::int32_t left; // 原成员偏移：+0x08。
    std::int32_t top; // 原成员偏移：+0x0C。
    std::int32_t width; // 原成员偏移：+0x10。
    std::int32_t height; // 原成员偏移：+0x14。
    const wchar_t* text; // 原成员偏移：+0x18；只借用UTF-16静态正文。
};

static_assert(offsetof(CharacterModalUiRecord, textX) == 0x00);
static_assert(offsetof(CharacterModalUiRecord, textY) == 0x04);
static_assert(offsetof(CharacterModalUiRecord, left) == 0x08);
static_assert(offsetof(CharacterModalUiRecord, top) == 0x0C);
static_assert(offsetof(CharacterModalUiRecord, width) == 0x10);
static_assert(offsetof(CharacterModalUiRecord, height) == 0x14);
static_assert(offsetof(CharacterModalUiRecord, text) == 0x18);
static_assert(sizeof(CharacterModalUiRecord) == 0x20);

// sub_503990@0x503990 与 sub_503A20@0x503A20 的还原入口。
std::int32_t Sub_503990(const CharacterModalUiRecord* uiRecord);
// 原函数：0x00503A20。
std::int32_t Sub_503A20(
    const CharacterModalUiRecord* uiRecord,
    std::uint8_t renderMode,
    std::int8_t color,
    std::int32_t hoverMode,
    std::uint32_t hoverImageSequence,
    std::uint8_t layer);

using cg::login::Sub_51B3B0;
// 原 sub_520200@0x520200：共享当前记录持久化核心的角色选择侧薄包装。
std::int32_t Sub_520200();

// sub_460D00@0x460D00 与 sub_461B30@0x461B30 共享的真实直接目标。
using cg::login::Sub_51B110;

// 原函数：0x0045E7B0。
std::int32_t Sub_45E7B0(); // 创建请求前局部重置当前资料并保存，透传保存结果。
// 原函数：0x0045E7D0。
std::int32_t Sub_45E7D0(); // 创建请求消息父入口；0等待、1成功、2失败。

// 原4626A0完整本体；WorldUpdate按D01显式传入口gate=1，不使用默认参数。
void ProcessCreationScene_4626A0(std::int32_t entryGate);

// 原 [0x464290,0x4642B4)：四项 left/top/width/height 转换为鼠标命中矩形。
std::int32_t Sub_464290(const std::int32_t* rectangle);

// 原 [0x462870,0x46287C)：清理 Yobi 后递增共享场景状态。
void Sub_462870();

// 原 [0x462880,0x462926)：创建两枚角色创建界面 action，写共享五槽并清输入上下文。
void Sub_462880(std::uint8_t resetSelection);

// 原函数：0x00462930。
std::int32_t Sub_462930(); // 创建角色选择页，完整命中/版本/变体/双预览与绘制。
// 原函数：0x00463100。
std::uint32_t Sub_463100(); // 标死两预览对象但不清槽，初始化创建输入与字段，返回采样的PUK。
// 原函数：0x004631D0。
std::int32_t Sub_4631D0(); // 创建页完整交互/确认/绘制，返回0继续、1确认、2返回。
// 原函数：0x004642C0。
std::int32_t Sub_4642C0(); // 保存14个W32及完整15CH输入上下文，返回最后有符号字节快照。
// 原函数：0x00464380。
std::uint8_t Sub_464380(); // 从备份低字节恢复创建字段，整块恢复输入，返回末次AL。

// 原 [0x50E770,0x50E77A)：忽略 socket，取第二个 S32 的低字节写 Puk 版本 W8 并返回。
std::uint8_t Sub_50E770(std::uintptr_t unusedSocket, std::int32_t pukVersion);

using cg::world::detail::Sub_519290;
using cg::world::detail::Sub_5194C0;
using cg::world::detail::Sub_519300;
using cg::world::detail::Sub_519550;
// 原 [0x51BA10,0x51BAB5)：按原区间顺序返回图号类别 0..5。
std::int32_t Sub_51BA10(std::int32_t imageNo);
// 原 [0x51BF00,0x51BFEF)：三段早退后按 signed 商索引 14 项局部图号表。
std::int32_t Sub_51BF00(std::int32_t imageNo);
// 原 [0x51BAC0,0x51BB76)：旧图号逆变换，仅在有效区间分解。
std::int32_t Sub_51BAC0(std::int32_t imageNo);
// 原 [0x51BD00,0x51BDA8)：按图号区间映射 33100/33101 或 -1。
std::int32_t Sub_51BD00(std::int32_t imageNo);
// 原 [0x51BFF0,0x51C0CA)：14 项类别查找及特殊区间转换。
std::int32_t Sub_51BFF0(std::int32_t imageNo);
// 原 [0x51BB80,0x51BC4E)：旧图号的分段换算，类别 3/4 交换。
std::int32_t Sub_51BB80(std::int32_t imageNo);
// 原 [0x51BC50,0x51BCFF)：按覆盖/Puk允许类别回退，独立于51B740。
std::int32_t Sub_51BC50(std::int32_t imageNo);
// 原 [0x51B7D0,0x51B90F)：图号的有序区间类别判定，供 51B740 消费。
std::int32_t Sub_51B7D0(std::int32_t imageNo);
// 原 [0x51B740,0x51B7CE)：采样覆盖/Puk版本后按类别决定保留或回退图号。
// LDT01：以当前版本低8值选择图号；原图/分类3减5000/既有51B910回退。
std::int32_t Sub_51B740(std::int32_t imageNo);
// 原 [0x51B910,0x51BA08)：图号映射至 100900/100901/100902/110598 或 -1。
std::int32_t Sub_51B910(std::int32_t imageNo);
// 原 [0x4C56E0,0x4C5789)：依序写类别、余数及三层分解，返回首个输出指针。
std::int32_t* Sub_4C56E0(
    std::int32_t imageNo,
    std::int32_t* outBase,
    std::int32_t* outClass,
    std::int32_t* outQuarter,
    std::int32_t* outFifth,
    std::int32_t* outOnes);
// 原 [0x4C5650,0x4C56DA)：按类别组合五项图号分量，其他类别透传首参。
std::int32_t Sub_4C5650(
    std::int32_t base,
    std::int32_t imageClass,
    std::int32_t quarter,
    std::int32_t fifth,
    std::int32_t ones);
using cg::audio::Sub_52B990;

// 原函数：0x0045F100。
std::int32_t Sub_45F100(const CharacterSelectUiRecord* uiRecord);
// 原函数：0x0045EFD0。
void Sub_45EFD0(
    const CharacterSelectUiRecord* uiRecord,
    std::uint8_t layer,
    std::int32_t overlayMode,
    std::int32_t overlayImageNo,
    std::int32_t style);
using cg::render::Sub_45F090;
using cg::network::Sub_512D30;

// BRT01：原9A5C10正常只读首NUL；新端1B owner静态存活，由创建/升级共享。
// 1B是已证读足迹对应容量，不冒称原未使用预留尾部长度。
extern const wchar_t gVersionPayload9A5C10[1];

// 原9A4EAC唯一128B提示正文；508800真实回复生产，角色选择显示借用同一存储。
// 容量不适用于独立9A5C10版本正文；SDB01合同见角色选择文档。
extern char gText9A4EAC[128]; // 原全局：0x009A4EAC。
static_assert(sizeof(gText9A4EAC) == 128);

} // namespace cg::character_select::detail
