#pragma once

#include "CharacterSelectSceneDependencies.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login::detail {

// 原5035E0的32B图形按钮；前六字段与503990矩形合同相同，末尾是图号而非文本。
struct LegacyPanelButtonRecord {
    std::int32_t imageX;
    std::int32_t imageY;
    std::int32_t left;
    std::int32_t top;
    std::int32_t width;
    std::int32_t height;
    std::int32_t normalImage;
    std::int32_t pressedImage;
};

// 原面板+28H起五条24B页签，只有末槽为地址。
struct LegacyPanelTabRecord {
    std::int32_t image;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    const char* hint;
};

// 原503160/503440共用196B记录；新端不使用原偏移遍历指针字段。
struct LegacyPanelRecord {
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::uint32_t id;
    std::array<std::int32_t, 5> backgrounds;
    std::array<LegacyPanelTabRecord, 5> tabs;
    std::int8_t selected;
    std::int8_t hovered;
    std::array<std::byte, 2> reservedA2;
    LegacyPanelButtonRecord closeButton;
};

// 原5030A0/503120共用40B文字按钮；三个原指针分别保留正文/回调/提示身份。
struct LegacyPanelTextButtonRecord {
    character_select::detail::CharacterModalUiRecord label;
    std::int32_t (*callback)();
    std::uint8_t color;
    std::array<std::byte, 3> reserved21;
    const char* hint;
};

static_assert(sizeof(LegacyPanelButtonRecord) == 32);
static_assert(offsetof(LegacyPanelTabRecord, hint) == 24);
static_assert(sizeof(LegacyPanelTabRecord) == 32);
static_assert(offsetof(LegacyPanelRecord, tabs) == 40);
static_assert(offsetof(LegacyPanelRecord, selected) == 200);
static_assert(offsetof(LegacyPanelRecord, closeButton) == 204);
static_assert(sizeof(LegacyPanelRecord) == 240);
static_assert(offsetof(LegacyPanelTextButtonRecord, callback) == 32);
static_assert(offsetof(LegacyPanelTextButtonRecord, color) == 40);
static_assert(offsetof(LegacyPanelTextButtonRecord, hint) == 48);
static_assert(sizeof(LegacyPanelTextButtonRecord) == 56);

extern std::uint32_t gLegacyPanelHitIdD046C0; // 503160写入，旧面板父函数借用。
extern LegacyPanelRecord gLegacySkillList585708; // 选择编号18；5857A8是同源selected。
extern LegacyPanelRecord gLegacyProfilePanel5857D0; // 旧选择编号2；585870是三页共享selected。
extern std::uint8_t gLegacySkillPendingD67C48; // 500370清零，旧技能/加工父入口共享。
std::uint32_t Sub_4ECE00(std::int32_t opened); // 旧点数结果页，选择编号41。
std::uint32_t Sub_4ED020(std::int32_t opened); // 旧战斗结果页，选择编号40。
std::int32_t Sub_500370(std::int32_t opened); // 旧技能列表完整交互/绘制，不同于窗口主表18。
std::uintptr_t Sub_5006F0(std::int32_t opened); // 三子页总入口；混合W32/上下文T*仅在返回边界拓宽。
std::int32_t Sub_5008C0(std::int32_t x, std::int32_t y); // 旧人物基本资料、属性条及前后排按钮。
std::int32_t Sub_5014A0(std::int32_t x, std::int32_t y); // 旧人物面板完整属性页，返回0。
std::int32_t Sub_501FD0(std::int32_t x, std::int32_t y); // 旧称号输入、选择/删除、八行分页。
std::int32_t Sub_502930(); // 原CRT 5530C8入口，尾跳旧地图面板构造。
std::int32_t Sub_502940(); // D68868同源面板初始化，不触碰玩家坐标或地图数据。
std::uint32_t Sub_5029B0(std::int32_t opened); // 旧地图面板；正常尾返回D046C0而非绘图结果。
std::int32_t Sub_5030A0(LegacyPanelTextButtonRecord* records, std::int32_t count);
void Sub_503120(LegacyPanelTextButtonRecord* records, std::int32_t count);
std::int32_t Sub_503160(
    std::int32_t opened, LegacyPanelRecord* panel, void (*initialize)(), std::int32_t flags);
std::uint32_t Sub_503440(LegacyPanelRecord* panel, std::int32_t flags);
std::int32_t Sub_503590(LegacyPanelRecord* panel);
void Sub_5035E0(
    LegacyPanelButtonRecord* button, std::uint8_t layer, std::int32_t hoverMode,
    std::uint32_t hoverImage, std::int32_t style);
void Sub_5038B0(
    LegacyPanelButtonRecord* button, std::uint8_t layer, std::int32_t hoverMode,
    std::uint32_t hoverImage, std::int32_t selected); // 非零选中图号，不屏蔽hover。

} // namespace cg::login::detail
