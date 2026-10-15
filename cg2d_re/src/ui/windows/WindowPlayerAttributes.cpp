#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原475E00：先按原顺序采样六值，连默认分支也计算门槛；区别于4767E0重读。
// 原函数：0x00475E00。
std::int32_t WindowPlayerAttributes_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::array<std::int32_t, 6> values;
    values[1] = world::detail::ReadPlayerValue78(1);
    values[2] = world::detail::ReadPlayerValue78(2);
    values[3] = world::detail::ReadPlayerValue78(3);
    values[4] = world::detail::ReadPlayerValue78(4);
    values[5] = world::detail::gValueD2F788;
    values[0] = world::detail::ReadPlayerValue78(0);
    std::int32_t total = 0;
    for (const std::int32_t value : values) {
        total += value;
    }
    const std::int32_t limit = total / 2;

    if (index >= 29 && index <= 33) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B53D;
            gHoverTextD68AE8 = L"分配點數至目前的能力數值。";
            consumed = 1;
        } else {
            graphic->imageNo = 0x3B53B;
        }
        if ((state & 2) != 0) {
            if (values[index - 29] < limit) {
                static_cast<void>(network::Sub_5123B0(platform::gConnectionSocket, index - 29));
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            }
            return 1;
        }
        return consumed;
    }

    std::int32_t idleImage;
    std::int32_t hoverImage;
    std::int32_t targetWindow;
    const wchar_t* hint;
    switch (index) {
    case 35:
        idleImage = 0x3BAB5;
        hoverImage = 0x3BAB7;
        targetWindow = 7;
        hint = L"切換成玩家狀態視窗。";
        break;
    case 37:
        idleImage = 0x3BAB8;
        hoverImage = 0x3BABA;
        targetWindow = 9;
        hint = L"切換成玩家稱號視窗。";
        break;
    case 38:
        if (character_select::state::gPukVersion < 8u) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
                0x3C111;
            return 0;
        }
        idleImage = 0x3C111;
        hoverImage = 0x3C112;
        targetWindow = 58;
        hint = L"切換成玩家簡介視窗";
        break;
    default:
        return 0;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = hoverImage;
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        graphic->imageNo = idleImage;
    }
    if ((state & 2) != 0 && gWindowStates[targetWindow].window == nullptr) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(WindowRuntime_Open(targetWindow));
        WindowInstance* const xSource = gCurrentWindow;
        WindowInstance* const xTarget = gWindowStates[targetWindow].window;
        xTarget->x = xSource->x;
        WindowInstance* const ySource = gCurrentWindow;
        WindowInstance* const yTarget = gWindowStates[targetWindow].window;
        yTarget->y = ySource->y;
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        consumed = 1;
    }
    return consumed;
}

// 原4767E0：逐项格式化后重取元素；点数门槛阶段独立读取，见window_8.md。
// 原函数：0x004767E0。
void WindowPlayerAttributes_Refresh()
{
    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
    const char* const name = world::detail::BorrowPlayerDisplayText(1);
    const std::size_t nameLength = std::strlen(name) + 1;
    nameData->text = cg::text::FromGbkWide(name);

    std::wstring buffer;
    const auto writeNumber = [&buffer](
        const std::int32_t index, const std::int32_t value, const std::wstring_view format) {
        cg::text::FormatTo(buffer, format, value);
        auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
        const std::size_t length = buffer.size() + 1;
        text->text = buffer;
    };
    writeNumber(1, world::detail::ReadPlayerLevel(), L"{:3}");
    writeNumber(2, world::detail::gValueD2F788, L"{:5}");
    for (std::int32_t index = 0; index < 5; ++index) {
        writeNumber(index + 3, world::detail::ReadPlayerValue78(index), L"{:5}");
    }
    for (std::int32_t index = 8; index < 11; ++index) {
        writeNumber(index, world::detail::ReadPlayerValue78(index), L"{:5}");
    }
    writeNumber(11, world::detail::ReadPlayerValueB0(0), L"{:5}");
    for (std::int32_t index = 11; index < 14; ++index) {
        writeNumber(index + 1, world::detail::ReadPlayerValue78(index), L"{:5}");
    }
    writeNumber(15, world::detail::ReadPlayerValueB0(1), L"{:5}");
    for (std::int32_t index = 0; index < 6; ++index) {
        writeNumber(index + 16, world::detail::ReadPlayerValueBC(index), L"{:4}");
    }
    for (std::int32_t index = 0; index < 4; ++index) {
        writeNumber(index + 22, world::detail::ReadPlayerValueD4(index), L"{:4}");
    }
    for (std::int32_t index = 5; index < 8; ++index) {
        writeNumber(index + 21, world::detail::ReadPlayerValue78(index), L"{:4}");
    }

    const std::int32_t remainingPoints = world::detail::gValueD2F788;
    if (remainingPoints > 0) {
        const std::int32_t quick = world::detail::ReadPlayerValue78(3);
        std::int32_t total = world::detail::ReadPlayerValue78(4);
        const std::int32_t toughness = world::detail::ReadPlayerValue78(2);
        total += quick;
        const std::int32_t strength = world::detail::ReadPlayerValue78(1);
        total += toughness;
        total += strength;
        const std::int32_t vitality = world::detail::ReadPlayerValue78(0);
        total += vitality;
        total += remainingPoints;
        const std::int32_t limit = total / 2;
        gCurrentWindow->elements[29].visible = vitality < limit ? 1 : 0;
        for (std::int32_t index = 1; index < 5; ++index) {
            const std::int32_t value = world::detail::ReadPlayerValue78(index);
            gCurrentWindow->elements[29 + index].visible = value < limit ? 1 : 0;
        }
    } else {
        for (std::int32_t index = 29; index < 34; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    }
}

// 原函数：0x00475DF0。
std::int32_t WindowPlayerAttributes_Update(const std::int32_t)
{
    WindowPlayerAttributes_Refresh();
    return 1;
}

// 原函数：0x00474360。
void WindowPlayerAttributes_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_PlayerAttributes].window;
    if (window != nullptr) {
        gWindowStates[WindowId_PlayerProfile].x = window->x;
        gWindowStates[WindowId_PlayerProfile].y = window->y;
        gWindowStates[WindowId_PlayerTitles].x = window->x;
        gWindowStates[WindowId_PlayerTitles].y = window->y;
        gWindowStates[WindowId_OwnProfile].x = window->x;
        gWindowStates[WindowId_OwnProfile].y = window->y;
    }
}

} // namespace detail

} // namespace cg::ui
