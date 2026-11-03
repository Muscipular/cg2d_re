#pragma once

#include "base.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

// BCPY01：原5538F4十行S8序列；后八行全0，59F9DC按已证PE映像静态零初值。
inline constexpr std::array<std::int8_t, 200> kBattleActionSequence5538F4 { // 原常量参考：0x005538F4。
    CharacterAnimation_Throw, CharacterAnimation_Damage, CharacterAnimation_Nod,
    CharacterAnimation_Sad, CharacterAnimation_Down, CharacterAnimation_Sit,
    CharacterAnimation_Joy, CharacterAnimation_Wave, CharacterAnimation_Angry,
    -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CharacterAnimation_Down, CharacterAnimation_Down, CharacterAnimation_Down, CharacterAnimation_Down,
    CharacterAnimation_Down, CharacterAnimation_Down, CharacterAnimation_Down, CharacterAnimation_Down,
    CharacterAnimation_Down, CharacterAnimation_Down, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// 原函数：0x0041F450。
void UpdateBattleFloatingIcon_41F450(YobiAction* action); // 未舍入速度推进Y，停留60帧后标死并投影。
// 原函数：0x0041F110。
void UpdateBattleNumber_41F110(YobiAction* action); // 延迟后整数累加屏幕Y，停留50帧；独立投影文字坐标。
// 原函数：0x0041F1A0。
std::int32_t NewBattleNumber_41F1A0(
    YobiAction* source, std::int32_t x, std::int32_t y, std::int32_t value, std::int32_t style,
    std::int32_t fontType, std::uint32_t flags); // 逐字符创建，失败保留已建对象，恒返回0。
// 原函数：0x0041F4C0。
YobiAction* NewBattleFloatingIcon_41F4C0(
    YobiAction* parent, std::int32_t imageNo, std::int32_t offsetX, std::int32_t offsetY);
// 原函数：0x00420100。
void NewBattleSequence_420100(YobiAction* source, std::int8_t value, std::int32_t position); // 逐个创建9/10项，失败保留已建对象。
// 原函数：0x00420300。
void UpdateBattleSequence_420300(YobiAction* action); // 原2A8六态，CryptInt自增和比较分别消费读取。
// 原函数：0x00420590。
void MoveBattleSequence_420590(YobiAction* action); // X从F32投影，Y从未舍入计算值投影。
// 原函数：0x00421750。
void UpdateBattleText_421750(YobiAction* action); // 面板就绪才绘制/扣帧，逐行释放后依次标死。
// 原函数：0x004217F0。
YobiAction* NewBattleText_4217F0(
    YobiAction* source, std::int8_t style, const wchar_t* text, std::int32_t fontType, std::int32_t duration);
// 原函数：0x004206C0。
std::int32_t NewBattleTimeNumber_4206C0(YobiAction* source, std::int32_t x, std::int32_t y, std::int32_t seconds);

} // namespace detail

} // namespace cg::yobi
