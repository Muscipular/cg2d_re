#include "battle_display.hpp"
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
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>
#include <string>

namespace cg::yobi {

namespace detail {

// 原4217F0的18H（24B）文本载荷；末4B用途未定，行表/行字节由421750释放。
struct BattleTextPayload {
    YobiAction* panel00; // 原成员偏移：+0x00。
    char** lines04; // 原成员偏移：+0x04。
    std::int32_t lineCount08; // 原成员偏移：+0x08。
    std::int32_t fontType0C; // 原成员偏移：+0x0C。
    std::int32_t remaining10; // 原成员偏移：+0x10。
    std::array<std::byte, 4> bytes14; // 原+14..+17H：仅显式保留存储，不猜新状态或地址类型。
};

static_assert(sizeof(BattleTextPayload) == sizeof(YobiAction*) + sizeof(char**) + 16);

std::int32_t gBattleActionSequenceIndex59F9DC = 0; // 原全局：0x0059F9DC。

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
        char** const lines = payload->lines04;
        YobiAction* const panel = payload->panel00;
        const char* const line = lines[index];
        const std::int32_t y = panel->y + 40;
        const std::int32_t x = panel->x + 40;
        // 原421793各行坐标相同，创建时的fontType只用于测量，不改为带行距绘制。
        static_cast<void>(render::DrawText3_456560(
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 2, 0, cg::text::FromGbkWide(line), 0));
    }
    const std::int32_t remaining = payload->remaining10 - 1;
    payload->remaining10 = remaining;
    if (remaining < 0) {
        for (std::int32_t index = 0; index < payload->lineCount08; ++index) {
            std::free(payload->lines04[index]);
        }
        std::free(payload->lines04);
        static_cast<void>(MarkDeath(payload->panel00));
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x004217F0。
YobiAction* NewBattleText_4217F0(
    const std::int8_t style, const char* const text,
    const std::int32_t fontType, const std::int32_t duration)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(BattleTextPayload), YobiPayloadKind::BattleText);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    auto* const payload = action->payload.battleText;
    action->callback = UpdateBattleText_421750;
    action->flags = flags | 0x40000u;
    payload->lineCount08 = 0;
    const char* current = text;
    payload->remaining10 = duration;
    payload->fontType0C = fontType;
    for (const char* newline = const_cast<char*>(std::strchr(current, '\n')); newline != nullptr;
         newline = const_cast<char*>(std::strchr(newline + 1, '\n'))) {
        ++payload->lineCount08;
    }
    ++payload->lineCount08;
    payload->lines04 = static_cast<char**>(
        std::calloc(static_cast<std::size_t>(payload->lineCount08), sizeof(char*)));
    std::int32_t longestLength = 0;
    std::int32_t longestIndex = -1;
    std::int32_t index = 0;
    if (current != nullptr) {
        do {
            const char* const newline = const_cast<char*>(std::strchr(current, '\n'));
            const std::int32_t length = newline != nullptr
                ? static_cast<std::int32_t>(newline - current)
                : static_cast<std::int32_t>(std::strlen(current));
            if (longestLength < length) {
                longestLength = length;
                longestIndex = index;
            }
            payload->lines04[index] = static_cast<char*>(
                std::calloc(static_cast<std::size_t>(length + 1), 1));
            std::memcpy(payload->lines04[index], current, static_cast<std::size_t>(length));
            payload->lines04[index][length] = '\0';
            if (newline == nullptr) {
                break;
            }
            current = newline + 1;
            ++index;
        } while (current != nullptr);
    }
    std::int32_t width = 80;
    if (longestIndex >= 0) {
        width = cg::render::TextMeasure(cg::text::FromGbkWide(payload->lines04[longestIndex]), payload->fontType0C) + 80;
    }
    const std::int32_t count = payload->lineCount08;
    std::int32_t height = 80;
    if (count > 0) {
        height = render::GetFontMetrics(payload->fontType0C).sHeight + 80;
    }
    if (count > 1) {
        height += count * (render::GetFontMetrics(payload->fontType0C).sHeight + 1);
    }
    payload->panel00 = CreateModalAction_5044E0((640 - width) >> 1, (480 - height) >> 1, width, height, style, 0);
    static_cast<void>(audio::PlaySe_52C450(56, 320));
    return action;
}

// 原函数：0x00420100。
void NewBattleSequence_420100(const std::int8_t value, const std::int32_t position)
{
    const std::int8_t limited = value > 10 ? 10 : value;
    const bool directOrder = position < 10;
    const std::int32_t baseX = directOrder ? 672 : -203;
    const float angle = directOrder ? 270.0F : 90.0F;
    const std::int32_t baseY = directOrder ? 384 : 96;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t slot = directOrder ? index : kBattleSequenceOrder553C1C[index];
        YobiAction* const action = NewYobiAction_404AE0(11, 0);
        if (action == nullptr) {
            return;
        }
        action->callback = UpdateBattleSequence_420300;
        action->displayPriority = 0xCD;
        action->mapX1E0 = static_cast<std::int32_t>(index);
        action->imageNo = 0;
        action->value54 = 18.0F;
        action->angle50 = angle;
        std::int32_t mapNo;
        if (slot == 0) {
            action->flags = 2;
            mapNo = 23138 + limited;
        } else if (slot == 8) {
            mapNo = 23162;
        } else if (slot == 9) {
            action->flags = 2;
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
        action->offset40 = static_cast<float>(static_cast<double>(offset) + graphicX + baseX);
        double y = static_cast<double>(graphicY) + baseY;
        if (slot == 9) {
            y -= 32.0;
        }
        action->offset44 = static_cast<float>(y);
        action->x = static_cast<std::int32_t>(action->offset40);
        action->y = static_cast<std::int32_t>(action->offset44);
        action->graphicIndex = graphic;
        action->rebirthValue1 = 4;
        action->rebirthValue2 = 4;
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
    const double xStep = xFactor * action->value54;
    action->offset40 = static_cast<float>(action->offset40 - xStep);
    const double yFactor = world::LookupSineDegrees_46F930(angle);
    const double yStep = yFactor * action->value54;
    const double y = yStep + action->offset44;
    action->offset44 = static_cast<float>(y);
    action->x = static_cast<std::int32_t>(action->offset40);
    // 原4205D2是FST；屏幕Y使用仍留在寄存器中的值，而非刚存回的F32。
    action->y = static_cast<std::int32_t>(y);
}

// 原函数：0x00420300。
void UpdateBattleSequence_420300(YobiAction* const action)
{
    switch (action->imageNo) {
    case 0: {
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
    case 1: {
        ++action->counter1DC;
        MoveBattleSequence_420590(action);
        const double speed = static_cast<double>(action->value54) - 0.7F;
        action->value54 = static_cast<float>(speed);
        // 原FST后比较未舍入值，FCOMP的无序也走阶段推进分支。
        if (!(speed > 0.0)) {
            ++action->imageNo;
            action->mapY1F0 = std::int32_t{0};
        }
        return;
    }
    case 2:
        ++action->counter1DC;
        static_cast<void>((++action->mapY1F0));
        if (action->flags != 0) {
            if ((action->mapY1F0 > 50)) {
                const std::int32_t next = action->imageNo + 1;
                action->flags = 0;
                action->imageNo = next;
                action->value54 = 10.0F;
                action->angle50 = 0.0F;
                if ((action->mapX1E0 == 9)) {
                    static_cast<void>(audio::PlaySe_52C450(64, 320));
                }
            }
        } else if (action->counter1DC > 240) {
            static_cast<void>(MarkDeath(action));
        }
        return;
    case 3: {
        ++action->counter1DC;
        MoveBattleSequence_420590(action);
        static_cast<void>((++action->mapY1F0));
        const double speed = action->value54 - 1.0;
        action->value54 = static_cast<float>(speed);
        if (!(speed > 0.0)) {
            const std::int32_t next = action->imageNo + 1;
            action->angle50 = 180.0F;
            action->imageNo = next;
        }
        return;
    }
    case 4: {
        ++action->counter1DC;
        static_cast<void>((++action->mapY1F0));
        const double speed = action->value54 + 1.0;
        action->value54 = static_cast<float>(speed);
        if (speed >= 10.0) {
            ++action->imageNo;
        }
        MoveBattleSequence_420590(action);
        return;
    }
    case 5:
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
    if (initialPhase == 0) {
        const std::int32_t oldDelay = action->field58;
        if (oldDelay > 0) {
            action->field58 = oldDelay - 1;
        } else {
            const double speed = static_cast<double>(action->numberVelocityB8)
                + 0.8F;
            const std::uint32_t flags = action->flags & ~2u;
            const bool reachedPeak = speed >= 7.2F;
            action->numberVelocityB8 = static_cast<float>(speed);
            action->flags = flags;
            if (reachedPeak) {
                action->phase = 1;
            }
            action->y += static_cast<std::int32_t>(speed);
        }
    } else if (initialPhase == 1) {
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
    const std::int32_t x, const std::int32_t y, const std::int32_t value,
    const std::int32_t style, const std::int32_t fontType, const std::uint32_t flags)
{
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", value);
    if (world::gBattleModeFlag5ABFCC == 0) {
        if ((flags & 0x40000000u) != 0) {
            buffer += L"+";
        }
        if ((flags & 0x80000000u) != 0) {
            buffer += L"-";
        }
    }
    const std::int32_t halfWidth = cg::render::MeasureDefaultText_44B260(buffer) / 2;
    const std::int32_t length = static_cast<std::int32_t>(buffer.size());
    for (std::int32_t index = 0; index < length; ++index) {
        YobiAction* const action = NewYobiAction_404AE0(2, 0);
        if (action == nullptr) {
            break;
        }
        action->callback = UpdateBattleNumber_41F110;
        action->displayPriority = 0xCF;
        action->mapX1E0 = static_cast<std::int32_t>(std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index])));
        action->imageNo = 0;
        action->value54 = 18.0F;
        action->angle50 = x < 320 ? 270.0F : 90.0F;
        action->flags = 2;
        const std::int32_t character = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index]));
        std::int32_t mapNo;
        switch (character) {
        case '+': mapNo = kBattleNumberGraphics54D4E8[style][1]; break;
        case '-': mapNo = kBattleNumberGraphics54D4E8[style][2]; break;
        case ':': mapNo = kBattleNumberGraphics54D4E8[style][3]; break;
        default: mapNo = kBattleNumberGraphics54D4E8[style][0] + character - '0'; break;
        }
        static_cast<void>(resource::GetGraphicMapNoIndex(mapNo, &action->graphicIndex));
        std::int16_t graphicX;
        std::int16_t graphicY;
        static_cast<void>(resource::GetGraphicXY(
            static_cast<std::uint32_t>(action->graphicIndex), &graphicX, &graphicY));
        const double graphicOffsetX = graphicX;
        const std::int32_t advance = render::GetFontMetrics(fontType).sWidth * index;
        action->offset40 = static_cast<float>(graphicOffsetX + x - halfWidth + advance);
        action->offset44 = static_cast<float>(static_cast<double>(graphicY) + y);
        const std::int32_t projectedX = static_cast<std::int32_t>(action->offset40);
        const float projectedY = action->offset44;
        action->x = projectedX;
        const std::int32_t projectedYValue = static_cast<std::int32_t>(projectedY);
        const std::uint32_t initialFlags = action->flags;
        action->y = projectedYValue;
        const float nameX = action->nameOffsetAC;
        const std::uint32_t drawFlags = initialFlags | 0x40000u;
        action->numberVelocityB8 = -8.0F;
        action->flags = drawFlags;
        action->field58 = 2 * index;
        action->rebirthValue1 = 4;
        action->rebirthValue2 = 4;
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
    const std::int32_t x, const std::int32_t y, const std::int32_t seconds)
{
    const std::int32_t magnitude = seconds < 0 ? -seconds : seconds;
    const char* const prefix = seconds < 0 ? "-" : "";
    char buffer[256];
    std::sprintf(buffer, "%s%d:%02d", prefix, magnitude / 60, magnitude % 60);
    const std::int32_t halfWidth = cg::render::MeasureDefaultText_44B260(cg::text::FromGbkWide(buffer)) / 2;
    const std::int32_t length = static_cast<std::int32_t>(std::strlen(buffer));
    for (std::int32_t index = 0; index < length; ++index) {
        const std::int8_t character = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index]));
        std::int32_t mapNo = 0;
        if (character >= '0' && character <= '9') {
            mapNo = kBattleNumberGraphics54D4E8[0][0] + character - '0';
        }
        if (character == '-') {
            mapNo = kBattleNumberGraphics54D4E8[0][2];
        }
        if (character == ':') {
            mapNo = kBattleNumberGraphics54D4E8[0][3];
        }
        if (mapNo == 0) {
            continue;
        }
        YobiAction* const action = NewYobiAction_404AE0(2, 0);
        if (action == nullptr) {
            return 0;
        }
        action->callback = UpdateBattleNumber_41F110;
        action->displayPriority = 0xCF;
        action->mapX1E0 = static_cast<std::int32_t>(std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index])));
        action->imageNo = 0;
        action->value54 = 18.0F;
        action->angle50 = x < 320 ? 270.0F : 90.0F;
        action->flags = 2;
        static_cast<void>(resource::GetGraphicMapNoIndex(mapNo, &action->graphicIndex));
        std::int16_t graphicX;
        std::int16_t graphicY;
        static_cast<void>(resource::GetGraphicXY(
            static_cast<std::uint32_t>(action->graphicIndex), &graphicX, &graphicY));
        const std::int32_t graphicXValue = graphicX;
        const std::int32_t fontWidth = render::GetFontMetrics(2).sWidth;
        const double graphicOffsetX = graphicXValue;
        const std::int32_t advance = fontWidth * index;
        action->offset40 = static_cast<float>(graphicOffsetX + x - halfWidth + advance);
        action->offset44 = static_cast<float>(static_cast<double>(graphicY) + y);
        const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
        const float screenY = action->offset44;
        action->x = screenX;
        const std::int32_t screenYInteger = static_cast<std::int32_t>(screenY);
        const float nameX = action->nameOffsetAC;
        action->y = screenYInteger;
        action->flags |= 0x40000u;
        action->numberVelocityB8 = -8.0F;
        action->field58 = 2 * index;
        action->rebirthValue1 = 4;
        action->rebirthValue2 = 4;
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
    if (initialPhase == 0) {
        const double speed = static_cast<double>(std::bit_cast<float>(action->value4CBits))
            + 0.8F;
        const bool reachedPeak = speed >= 7.2F;
        action->value4CBits = std::bit_cast<std::uint32_t>(static_cast<float>(speed));
        if (reachedPeak) {
            action->phase = 1;
        }
        // 原FST不弹出速度，Y使用未舍入和，而非刚存回的F32。
        action->offset44 = static_cast<float>(speed + action->offset44);
    } else if (initialPhase == 1) {
        const std::int32_t count = action->deltaX + 1;
        action->deltaX = count;
        if (count >= 60) {
            static_cast<void>(MarkDeath(action));
        }
    }
    const std::int32_t projectedX = static_cast<std::int32_t>(action->offset40);
    const float projectedY = action->offset44;
    action->x = projectedX;
    action->y = static_cast<std::int32_t>(projectedY);
}

// 原函数：0x0041F4C0。
YobiAction* NewBattleFloatingIcon_41F4C0(
    YobiAction* const parent, const std::int32_t imageNo,
    const std::int32_t offsetX, const std::int32_t offsetY)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleFloatingIcon_41F450;
    action->displayPriority = 0xCC;
    action->flags = flags | 0x40000u;
    action->framePlayingRate = 100;
    action->offset40 = static_cast<float>(static_cast<double>(offsetX) + parent->offset40);
    const double y = static_cast<double>(offsetY) + parent->offset44;
    action->value4CBits = 0xC1000000u;
    action->imageNo = imageNo;
    action->direction = 0;
    action->rebirthValue1 = 4;
    action->offset44 = static_cast<float>(y);
    action->rebirthValue2 = 4;
    action->pathCountOrDrawOrder_270 = 0;
    static_cast<void>(PlayFrame_517DA0(action, 1));
    const std::int32_t projectedX = static_cast<std::int32_t>(action->offset40);
    const float projectedY = action->offset44;
    action->x = projectedX;
    action->y = static_cast<std::int32_t>(projectedY);
    return action;
}

} // namespace detail

} // namespace cg::yobi
