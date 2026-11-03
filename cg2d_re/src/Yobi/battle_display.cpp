#include "battle_display.hpp"
#include "battle_base.hpp"
#include "base.hpp"
#include "../scene/login/LoginStateDependencies.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include "animation.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"
#include "../world/WorldMapState.hpp"

#include <array>
#include <format>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>
#include <cwchar>
#include <string>

namespace cg::yobi {

namespace detail {

// 原420100生产imageNo阶段，420300消费；不是角色图号或phase。
enum BattleSequenceStage : std::int32_t {
    BattleSequenceStage_WaitingForEntry = 0, // 消耗逐项延迟，再等待原入场计数。
    BattleSequenceStage_EnteringAndSlowing = 1, // 按创建方向入场，速度每轮减0.7。
    BattleSequenceStage_Holding = 2, // 停留，沿原flags及计数选择后续或标死。
    BattleSequenceStage_RisingAndSlowing = 3, // angle0向上移动，速度每轮减1。
    BattleSequenceStage_FallingAndAccelerating = 4, // angle180向下移动，速度每轮加1。
    BattleSequenceStage_WaitingForRemoval = 5, // 停止移动，按原总计数标死。
};

// 原41F110/41F450共享phase语义；数字延迟及50帧、图标60帧分别保持。
enum BattleFloatingDisplayPhase : std::int32_t {
    BattleFloatingDisplayPhase_Moving = 0, // 沿原分配初始0进入，不增加显式初始化。
    BattleFloatingDisplayPhase_Holding = 1, // 停止移动，计数到各自阈值后标死。
};

// 原54D4E8表第二维；41F1A0/4206C0按字符选择，第一维style与本枚举独立。
enum BattleNumberGraphicColumn : std::int32_t {
    BattleNumberGraphicColumn_DigitBase = 0, // 加字符减'0'得到数字图。
    BattleNumberGraphicColumn_Plus = 1, // '+'字形。
    BattleNumberGraphicColumn_Minus = 2, // '-'字形。
    BattleNumberGraphicColumn_Colon = 3, // ':'字形。
};

// 原4217F0的18H（24B）文本载荷；末4B用途未定，行表/行字节由421750释放。
struct BattleTextPayload {
    YobiAction* panel00; // 原成员偏移：+0x00。
    std::wstring* lines04; // 原成员偏移：+0x04。
    std::int32_t lineCount08; // 原成员偏移：+0x08。
    std::int32_t fontType0C; // 原成员偏移：+0x0C。
    std::int32_t remaining10; // 原成员偏移：+0x10。
    std::array<std::byte, 4> bytes14; // 原+14..+17H：仅显式保留存储，不猜新状态或地址类型。
};

static_assert(sizeof(BattleTextPayload) == sizeof(YobiAction*) + sizeof(std::wstring*) + 16);

// 原54D4E8起十行：数字基图、加号、减号、冒号；41F1A0按style取行。
static constexpr std::array<std::array<std::int32_t, 4>, 10> kBattleNumberGraphics54D4E8 {{
    {0x3B7F4, 0x3B858, 0x3B859, 0x3B85A},
    {0x3B7FE, 0x3B862, 0x3B863, 0x3B864},
    {0x3B808, 0x3B86C, 0x3B86D, 0x3B86E},
    {0x3B812, 0x3B876, 0x3B877, 0x3B878},
    {0x3B81C, 0x3B880, 0x3B881, 0x3B882},
    {0x3B826, 0x3B88A, 0x3B88B, 0x3B88C},
    {0x3B830, 0x3B894, 0x3B895, 0x3B896},
    {0x3B83A, 0x3B89E, 0x3B89F, 0x3B8A0},
    {0x3B844, 0x3B8A8, 0x3B8A9, 0x3B8AA},
    {0x3B84E, 0x3B8B2, 0x3B8B3, 0x3B8B4}}};

// 420100只消费这两段40B，不冻结相邻未消费的原表布局。
static constexpr std::array<std::int32_t, 10> kBattleSequenceOffsets553BF4 { // 原常量参考：0x00553BF4。
    0, 34, 54, 66, 88, 108, 131, 152, 171, 0,
};

static constexpr std::array<std::int32_t, 10> kBattleSequenceOrder553C1C { // 原常量参考：0x00553C1C。
    0, 8, 7, 6, 5, 4, 3, 2, 1, 9,
};

// 原函数：0x00421750。
void UpdateBattleText_421750(YobiAction* const action)
{
    auto* const payload = action->payload.battleText;
    if (!(payload->panel00->readiness > 0)) {
        return;
    }
    for (std::int32_t index = 0; index < payload->lineCount08; ++index) {
        const std::wstring* const lines = payload->lines04;
        YobiAction* const panel = payload->panel00;
        const std::wstring& line = lines[index];
        const std::int32_t y = panel->y + 40;
        const std::int32_t x = panel->x + 40;
        // 原421793各行坐标相同，创建时的fontType只用于测量，不改为带行距绘制。
        static_cast<void>(render::DrawText3_456560(
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 2, 0, line, 0));
    }
    const std::int32_t remaining = payload->remaining10 - 1;
    payload->remaining10 = remaining;
    if (remaining < 0) {
        delete[] payload->lines04;
        payload->lines04 = nullptr;
        static_cast<void>(MarkDeath(payload->panel00));
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x004217F0。
YobiAction* NewBattleText_4217F0(
    YobiAction* const source, const std::int8_t style, const wchar_t* const text,
    const std::int32_t fontType, const std::int32_t duration)
{
    YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 2, sizeof(BattleTextPayload), YobiPayloadKind::BattleText);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    auto* const payload = action->payload.battleText;
    action->callback = UpdateBattleText_421750;
    action->flags = flags | YobiActionFlag_FourPhaseUpdateGate;
    payload->lineCount08 = 0;
    const wchar_t* current = text;
    payload->remaining10 = duration;
    payload->fontType0C = fontType;
    for (const wchar_t* newline = std::wcschr(current, L'\n'); newline != nullptr;
         newline = std::wcschr(newline + 1, L'\n')) {
        ++payload->lineCount08;
    }
    ++payload->lineCount08;
    payload->lines04 = new std::wstring[static_cast<std::size_t>(payload->lineCount08)];
    std::int32_t longestLength = 0;
    std::int32_t longestIndex = -1;
    std::int32_t index = 0;
    if (current != nullptr) {
        do {
            const wchar_t* const newline = std::wcschr(current, L'\n');
            const std::int32_t length = newline != nullptr
                ? static_cast<std::int32_t>(newline - current)
                : static_cast<std::int32_t>(std::wcslen(current));
            const auto byteLength = static_cast<std::int32_t>(cg::text::ToGbk(
                std::wstring_view(current, static_cast<std::size_t>(length))).size());
            if (longestLength < byteLength) {
                longestLength = byteLength;
                longestIndex = index;
            }
            payload->lines04[index].assign(current, static_cast<std::size_t>(length));
            if (newline == nullptr) {
                break;
            }
            current = newline + 1;
            ++index;
        } while (current != nullptr);
    }
    std::int32_t width = 80;
    if (longestIndex >= 0) {
        width = cg::render::TextMeasure(payload->lines04[longestIndex], payload->fontType0C) + 80;
    }
    const std::int32_t count = payload->lineCount08;
    std::int32_t height = 80;
    if (count > 0) {
        height = render::GetFontMetrics(payload->fontType0C).sHeight + 80;
    }
    if (count > 1) {
        height += count * (render::GetFontMetrics(payload->fontType0C).sHeight + 1);
    }
    payload->panel00 = CreateModalAction_5044E0(*source->ownerSession, (640 - width) >> 1, (480 - height) >> 1, width, height, style, 0);
    static_cast<void>(audio::PlaySe_52C450(56, 320));
    return action;
}

// 原函数：0x00420100。
void NewBattleSequence_420100(YobiAction* const source, const std::int8_t value, const std::int32_t position)
{
    const std::int8_t limited = value > 10 ? 10 : value;
    const bool directOrder = position < 10;
    const std::int32_t baseX = directOrder ? 672 : -203;
    const float angle = directOrder ? 270.0F : 90.0F;
    const std::int32_t baseY = directOrder ? 384 : 96;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t slot = directOrder ? index : kBattleSequenceOrder553C1C[index];
        YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 11, 0);
        if (action == nullptr) {
            return;
        }
        action->callback = UpdateBattleSequence_420300;
        action->displayPriority = 0xCD;
        action->mapX1E0 = static_cast<std::int32_t>(index);
        action->imageNo = BattleSequenceStage_WaitingForEntry;
        action->motionMagnitude54 = 18.0F;
        action->angle50 = angle;
        std::int32_t mapNo;
        if (slot == 0) {
            action->flags = static_cast<std::int32_t>(YobiActionFlag_Hidden);
            mapNo = 23138 + limited;
        } else if (slot == 8) {
            mapNo = 23162;
        } else if (slot == 9) {
            action->flags = static_cast<std::int32_t>(YobiActionFlag_Hidden);
            mapNo = 23165;
        } else {
            mapNo = 23157 + slot;
        }
        std::int32_t graphic;
        static_cast<void>(resource::GetGraphicMapNoIndex(mapNo, &graphic));
        std::int16_t graphicX;
        std::int16_t graphicY;
        static_cast<void>(resource::GetGraphicXY(static_cast<std::uint32_t>(graphic), &graphicX, &graphicY));
        const std::int32_t offset = kBattleSequenceOffsets553BF4[slot == 9 ? 4 : slot];
        action->origin_x_offset40 = static_cast<float>(static_cast<double>(offset) + graphicX + baseX);
        double y = static_cast<double>(graphicY) + baseY;
        if (slot == 9) {
            y -= 32.0;
        }
        action->origin_y_offset44 = static_cast<float>(y);
        action->x = static_cast<std::int32_t>(action->origin_x_offset40);
        action->y = static_cast<std::int32_t>(action->origin_y_offset44);
        action->graphicIndex = graphic;
        action->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
        action->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
        action->field1D0 = 4 * index;
        action->counter1DC = 4 * (8 - slot);
        if (limited != 10 && index == 8) {
            return;
        }
    }
}

// 原函数：0x00420590。
void MoveBattleSequence_420590(YobiAction* const action)
{
    float angle = static_cast<float>(270.0 - action->angle50);
    world::NormalizeAngleDegrees_46F8F0(&angle);
    const double xFactor = world::LookupCosineDegrees_46F950(angle);
    const double xStep = xFactor * action->motionMagnitude54;
    action->origin_x_offset40 = static_cast<float>(action->origin_x_offset40 - xStep);
    const double yFactor = world::LookupSineDegrees_46F930(angle);
    const double yStep = yFactor * action->motionMagnitude54;
    const double y = yStep + action->origin_y_offset44;
    action->origin_y_offset44 = static_cast<float>(y);
    action->x = static_cast<std::int32_t>(action->origin_x_offset40);
    // 原4205D2是FST；屏幕Y使用仍留在寄存器中的值，而非刚存回的F32。
    action->y = static_cast<std::int32_t>(y);
}

// 原函数：0x00420300。
void UpdateBattleSequence_420300(YobiAction* const action)
{
    switch (action->imageNo) {
    case BattleSequenceStage_WaitingForEntry: {
        const std::int32_t count = action->counter1DC;
        const std::int32_t delay = action->field1D0;
        action->counter1DC = count + 1;
        if (delay != 0) {
            action->field1D0 = delay - 1;
            return;
        }
        static_cast<void>((++action->mapY1F0));
        if ((action->mapY1F0 > 60)) {
            ++action->imageNo;
        }
        return;
    }
    case BattleSequenceStage_EnteringAndSlowing: {
        ++action->counter1DC;
        MoveBattleSequence_420590(action);
        const double speed = static_cast<double>(action->motionMagnitude54) - 0.7F;
        action->motionMagnitude54 = static_cast<float>(speed);
        // 原FST后比较未舍入值，FCOMP的无序也走阶段推进分支。
        if (!(speed > 0.0)) {
            ++action->imageNo;
            action->mapY1F0 = std::int32_t{0};
        }
        return;
    }
    case BattleSequenceStage_Holding:
        ++action->counter1DC;
        static_cast<void>((++action->mapY1F0));
        if (action->flags != 0) {
            if ((action->mapY1F0 > 50)) {
                const std::int32_t next = action->imageNo + 1;
                action->flags = 0;
                action->imageNo = next;
                action->motionMagnitude54 = 10.0F;
                action->angle50 = 0.0F;
                if ((action->mapX1E0 == 9)) {
                    static_cast<void>(audio::PlaySe_52C450(64, 320));
                }
            }
        } else if (action->counter1DC > 240) {
            static_cast<void>(MarkDeath(action));
        }
        return;
    case BattleSequenceStage_RisingAndSlowing: {
        ++action->counter1DC;
        MoveBattleSequence_420590(action);
        static_cast<void>((++action->mapY1F0));
        const double speed = action->motionMagnitude54 - 1.0;
        action->motionMagnitude54 = static_cast<float>(speed);
        if (!(speed > 0.0)) {
            const std::int32_t next = action->imageNo + 1;
            action->angle50 = 180.0F;
            action->imageNo = next;
        }
        return;
    }
    case BattleSequenceStage_FallingAndAccelerating: {
        ++action->counter1DC;
        static_cast<void>((++action->mapY1F0));
        const double speed = action->motionMagnitude54 + 1.0;
        action->motionMagnitude54 = static_cast<float>(speed);
        if (speed >= 10.0) {
            ++action->imageNo;
        }
        MoveBattleSequence_420590(action);
        return;
    }
    case BattleSequenceStage_WaitingForRemoval:
        ++action->counter1DC;
        static_cast<void>((++action->mapY1F0));
        if (action->counter1DC > 240) {
            static_cast<void>(MarkDeath(action));
        }
        return;
    default:
        return;
    }
}

// 原函数：0x0041F110。
void UpdateBattleNumber_41F110(YobiAction* const action)
{
    const std::int32_t initialPhase = action->phase;
    if (initialPhase == BattleFloatingDisplayPhase_Moving) {
        const std::int32_t oldDelay = action->field58;
        if (oldDelay > 0) {
            action->field58 = oldDelay - 1;
        } else {
            const double speed = static_cast<double>(action->numberVelocityB8)
                + 0.8F;
            const std::uint32_t flags = action->flags & ~YobiActionFlag_Hidden;
            const bool reachedPeak = speed >= 7.2F;
            action->numberVelocityB8 = static_cast<float>(speed);
            action->flags = flags;
            if (reachedPeak) {
                action->phase = BattleFloatingDisplayPhase_Holding;
            }
            action->y += static_cast<std::int32_t>(speed);
        }
    } else if (initialPhase == BattleFloatingDisplayPhase_Holding) {
        const std::int32_t count = action->deltaX + 1;
        action->deltaX = count;
        if (count >= 50) {
            static_cast<void>(MarkDeath(action));
        }
    }
    const std::int32_t projectedNameX = static_cast<std::int32_t>(action->nameOffsetAC);
    const float projectedNameY = action->nameOffsetB0;
    action->nameX = projectedNameX;
    action->nameY = static_cast<std::int32_t>(projectedNameY);
}

// 原函数：0x0041F1A0。
std::int32_t NewBattleNumber_41F1A0(
    YobiAction* const source, const std::int32_t x, const std::int32_t y, const std::int32_t value,
    const std::int32_t style, const std::int32_t fontType, const std::uint32_t flags)
{
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", value);
    if (world::gBattleModeFlag5ABFCC == 0) {
        if ((flags & BattleResultFlag_NumberSuffixPlus) != 0) {
            buffer += L"+";
        }
        if ((flags & BattleResultFlag_NumberSuffixMinus) != 0) {
            buffer += L"-";
        }
    }
    const std::int32_t halfWidth = cg::render::MeasureDefaultText_44B260(buffer) / 2;
    const std::int32_t length = static_cast<std::int32_t>(buffer.size());
    for (std::int32_t index = 0; index < length; ++index) {
        YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 2, 0);
        if (action == nullptr) {
            break;
        }
        action->callback = UpdateBattleNumber_41F110;
        action->displayPriority = 0xCF;
        action->mapX1E0 = static_cast<std::int32_t>(std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index])));
        action->imageNo = 0;
        action->motionMagnitude54 = 18.0F;
        action->angle50 = x < 320 ? 270.0F : 90.0F;
        action->flags = static_cast<std::int32_t>(YobiActionFlag_Hidden);
        const std::int32_t character = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index]));
        std::int32_t mapNo;
        switch (character) {
        case '+': mapNo = kBattleNumberGraphics54D4E8[style][BattleNumberGraphicColumn_Plus]; break;
        case '-': mapNo = kBattleNumberGraphics54D4E8[style][BattleNumberGraphicColumn_Minus]; break;
        case ':': mapNo = kBattleNumberGraphics54D4E8[style][BattleNumberGraphicColumn_Colon]; break;
        default: mapNo = kBattleNumberGraphics54D4E8[style][BattleNumberGraphicColumn_DigitBase] + character - '0'; break;
        }
        static_cast<void>(resource::GetGraphicMapNoIndex(mapNo, &action->graphicIndex));
        std::int16_t graphicX;
        std::int16_t graphicY;
        static_cast<void>(resource::GetGraphicXY(
            static_cast<std::uint32_t>(action->graphicIndex), &graphicX, &graphicY));
        const double graphicOffsetX = graphicX;
        const std::int32_t advance = render::GetFontMetrics(fontType).sWidth * index;
        action->origin_x_offset40 = static_cast<float>(graphicOffsetX + x - halfWidth + advance);
        action->origin_y_offset44 = static_cast<float>(static_cast<double>(graphicY) + y);
        const std::int32_t projectedX = static_cast<std::int32_t>(action->origin_x_offset40);
        const float projectedY = action->origin_y_offset44;
        action->x = projectedX;
        const std::int32_t projectedYValue = static_cast<std::int32_t>(projectedY);
        const std::uint32_t initialFlags = action->flags;
        action->y = projectedYValue;
        const float nameX = action->nameOffsetAC;
        const std::uint32_t drawFlags = initialFlags | YobiActionFlag_FourPhaseUpdateGate;
        action->numberVelocityB8 = -8.0F;
        action->flags = drawFlags;
        action->field58 = 2 * index;
        action->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
        action->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
        const std::int32_t projectedNameX = static_cast<std::int32_t>(nameX);
        const float projectedNameY = action->nameOffsetB0;
        action->nameX = projectedNameX;
        action->nameY = static_cast<std::int32_t>(projectedNameY);
        action->pathCountOrDrawOrder_270 = 0;
    }
    return 0;
}

// 原函数：0x004206C0。
std::int32_t NewBattleTimeNumber_4206C0(
    YobiAction* const source, const std::int32_t x, const std::int32_t y, const std::int32_t seconds)
{
    const std::int32_t magnitude = seconds < 0 ? -seconds : seconds;
    const wchar_t* const prefix = seconds < 0 ? L"-" : L"";
    const auto buffer = std::format(L"{}{}:{:02}", prefix, magnitude / 60, magnitude % 60);
    const std::int32_t halfWidth = cg::render::MeasureDefaultText_44B260(buffer) / 2;
    const std::int32_t length = static_cast<std::int32_t>(buffer.size());
    for (std::int32_t index = 0; index < length; ++index) {
        const wchar_t character = buffer[index];
        std::int32_t mapNo = 0;
        if (character >= '0' && character <= '9') {
            mapNo = kBattleNumberGraphics54D4E8[0][BattleNumberGraphicColumn_DigitBase] + character - '0';
        }
        if (character == '-') {
            mapNo = kBattleNumberGraphics54D4E8[0][BattleNumberGraphicColumn_Minus];
        }
        if (character == ':') {
            mapNo = kBattleNumberGraphics54D4E8[0][BattleNumberGraphicColumn_Colon];
        }
        if (mapNo == 0) {
            continue;
        }
        YobiAction* const action = NewYobiAction_404AE0(*source->ownerSession, 2, 0);
        if (action == nullptr) {
            return 0;
        }
        action->callback = UpdateBattleNumber_41F110;
        action->displayPriority = 0xCF;
        action->mapX1E0 = static_cast<std::int32_t>(std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index])));
        action->imageNo = 0;
        action->motionMagnitude54 = 18.0F;
        action->angle50 = x < 320 ? 270.0F : 90.0F;
        action->flags = static_cast<std::int32_t>(YobiActionFlag_Hidden);
        static_cast<void>(resource::GetGraphicMapNoIndex(mapNo, &action->graphicIndex));
        std::int16_t graphicX;
        std::int16_t graphicY;
        static_cast<void>(resource::GetGraphicXY(
            static_cast<std::uint32_t>(action->graphicIndex), &graphicX, &graphicY));
        const std::int32_t graphicXValue = graphicX;
        const std::int32_t fontWidth = render::GetFontMetrics(2).sWidth;
        const double graphicOffsetX = graphicXValue;
        const std::int32_t advance = fontWidth * index;
        action->origin_x_offset40 = static_cast<float>(graphicOffsetX + x - halfWidth + advance);
        action->origin_y_offset44 = static_cast<float>(static_cast<double>(graphicY) + y);
        const std::int32_t screenX = static_cast<std::int32_t>(action->origin_x_offset40);
        const float screenY = action->origin_y_offset44;
        action->x = screenX;
        const std::int32_t screenYInteger = static_cast<std::int32_t>(screenY);
        const float nameX = action->nameOffsetAC;
        action->y = screenYInteger;
        action->flags |= YobiActionFlag_FourPhaseUpdateGate;
        action->numberVelocityB8 = -8.0F;
        action->field58 = 2 * index;
        action->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
        action->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
        const std::int32_t nameXInteger = static_cast<std::int32_t>(nameX);
        const float nameY = action->nameOffsetB0;
        action->nameX = nameXInteger;
        action->nameY = static_cast<std::int32_t>(nameY);
        action->pathCountOrDrawOrder_270 = 0;
    }
    return 0;
}

// 原函数：0x0041F450。
void UpdateBattleFloatingIcon_41F450(YobiAction* const action)
{
    const std::int32_t initialPhase = action->phase;
    if (initialPhase == BattleFloatingDisplayPhase_Moving) {
        const double speed = static_cast<double>(std::bit_cast<float>(action->verticalVelocityBits4C))
            + 0.8F;
        const bool reachedPeak = speed >= 7.2F;
        action->verticalVelocityBits4C = std::bit_cast<std::uint32_t>(static_cast<float>(speed));
        if (reachedPeak) {
            action->phase = BattleFloatingDisplayPhase_Holding;
        }
        // 原FST不弹出速度，Y使用未舍入和，而非刚存回的F32。
        action->origin_y_offset44 = static_cast<float>(speed + action->origin_y_offset44);
    } else if (initialPhase == BattleFloatingDisplayPhase_Holding) {
        const std::int32_t count = action->deltaX + 1;
        action->deltaX = count;
        if (count >= 60) {
            static_cast<void>(MarkDeath(action));
        }
    }
    const std::int32_t projectedX = static_cast<std::int32_t>(action->origin_x_offset40);
    const float projectedY = action->origin_y_offset44;
    action->x = projectedX;
    action->y = static_cast<std::int32_t>(projectedY);
}

// 原函数：0x0041F4C0。
YobiAction* NewBattleFloatingIcon_41F4C0(
    YobiAction* const parent, const std::int32_t imageNo,
    const std::int32_t offsetX, const std::int32_t offsetY)
{
    YobiAction* const action = NewYobiAction_404AE0(*parent->ownerSession, 2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleFloatingIcon_41F450;
    action->displayPriority = 0xCC;
    action->flags = flags | YobiActionFlag_FourPhaseUpdateGate;
    action->framePlayingRate = 100;
    action->origin_x_offset40 = static_cast<float>(static_cast<double>(offsetX) + parent->origin_x_offset40);
    const double y = static_cast<double>(offsetY) + parent->origin_y_offset44;
    action->verticalVelocityBits4C = 0xC1000000u;
    action->imageNo = imageNo;
    action->direction = YobiDirection_UpLeft;
    action->mixFlagsOverrideMask = render::GraphicMixFlag_OriginalPalette;
    action->origin_y_offset44 = static_cast<float>(y);
    action->mixFlagsOverrideValue = render::GraphicMixFlag_OriginalPalette;
    action->pathCountOrDrawOrder_270 = 0;
    static_cast<void>(PlayFrame_517DA0(action, AnimationPlayback_StopAtEnd));
    const std::int32_t projectedX = static_cast<std::int32_t>(action->origin_x_offset40);
    const float projectedY = action->origin_y_offset44;
    action->x = projectedX;
    action->y = static_cast<std::int32_t>(projectedY);
    return action;
}

} // namespace detail

} // namespace cg::yobi
