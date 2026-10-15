#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

std::int8_t gSettingsPanelCC4234 = 0; // 原全局：0x00CC4234。

std::int32_t gSettingsOffsetCA02DC = 0; // 原全局：0x00CA02DC。

std::int32_t gSettingsStepD2D610 = 0; // 原全局：0x00D2D610。

std::wstring gSettingsMusicTextD3041C {}; // 原全局：0x00D3041C。

std::wstring gSettingsEffectTextD2EA64 {}; // 原全局：0x00D2EA64。
namespace {


constexpr std::array<WindowValueCallback, 4> kSettingsPanelCallbacks57DF38 { // 原常量参考：0x0057DF38。
    &WindowSettings_RefreshLogoutPanel, &WindowSettings_RefreshInputPanel, &WindowSettings_RefreshAudioPanel, &WindowSettings_RefreshDisplayPanel,
};

constexpr wchar_t kSettingsLogout586DE8[] = L"登出伺服器。"; // 原常量参考：0x00586DE8。

constexpr wchar_t kSettingsInput586DF8[] = L"設定滑鼠與鍵盤的操作方式。"; // 原常量参考：0x00586DF8。

constexpr wchar_t kSettingsAudio586E14[] = L"設定音樂音效。"; // 原常量参考：0x00586E14。

constexpr wchar_t kSettingsDisplay586E24[] = L"掃描線同期設定"; // 原常量参考：0x00586E24。

constexpr std::array<std::int32_t, 6> kSettingsKeyImages54F460 { // 原常量参考：0x0054F460。
    0x3BE63, 0x3BE64, 0x3BE65, 0x3BE66, 0x3BE67, 0x3BE68,
};

constexpr wchar_t kSettingsHotkeys586E34[] = L"開啟熱鍵一覽表。"; // 原常量参考：0x00586E34。

constexpr wchar_t kSettingsLogoutTown586E48[] = L"登出並回到城內登入點。"; // 原常量参考：0x00586E48。

constexpr wchar_t kSettingsLogoutServer586E60[] = L"登出後回到伺服器選擇畫面。"; // 原常量参考：0x00586E60。

constexpr wchar_t kSettingsStackMode586E7C[] = L"變更物品堆疊視窗的開啟方式。"; // 原常量参考：0x00586E7C。

constexpr wchar_t kSettingsCursor586E9C[] = L"變更滑鼠游標種類。"; // 原常量参考：0x00586E9C。

constexpr wchar_t kSettingsKeyMode586EB0[] = L"變更快速鍵的型態。"; // 原常量参考：0x00586EB0。

constexpr wchar_t kSettingsMusicMin586EC4[] = L"無法再降低背景音樂的音量。"; // 原常量参考：0x00586EC4。

constexpr wchar_t kSettingsMusicDown586EE0[] = L"降低背景音樂的音量。"; // 原常量参考：0x00586EE0。

constexpr wchar_t kSettingsMusicMax586EF8[] = L"無法再提高背景音樂的音量。"; // 原常量参考：0x00586EF8。

constexpr wchar_t kSettingsMusicUp586F14[] = L"提高背景音樂的音量。"; // 原常量参考：0x00586F14。

constexpr wchar_t kSettingsEffectMin586F2C[] = L"無法再降低效果音量。"; // 原常量参考：0x00586F2C。

constexpr wchar_t kSettingsEffectDown586F44[] = L"降低效果音量。"; // 原常量参考：0x00586F44。

constexpr wchar_t kSettingsEffectMax586F54[] = L"無法再提高效果音量。"; // 原常量参考：0x00586F54。

constexpr wchar_t kSettingsEffectUp586F6C[] = L"提高效果音量。"; // 原常量参考：0x00586F6C。

constexpr wchar_t kSettingsOutput586F7C[] = L"變更音響輸出類型。"; // 原常量参考：0x00586F7C。

constexpr wchar_t kSettingsRenderMode586F90[] = L"目前的描繪功能。"; // 原常量参考：0x00586F90。
} // namespace


// 原函数：0x00493A70。
void WindowSettings_RefreshLogoutPanel(const std::int32_t offset)
{
    WindowInstance* const window = gWindowStates[WindowId_Settings].window;
    window->elements[27].x = offset + 16;
    WindowElement* const elements = window->elements;
    const std::int32_t x = elements[27].x;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[27].data);
    const std::int32_t width = 16 - x;
    graphic->width = width;
    const std::int32_t limit = window->elements[27].width;
    if (width > limit) {
        graphic->width = limit;
    }
    const std::int32_t visible = offset <= -146 ? 1 : 0;
    window->elements[8].visible = visible;
    window->elements[9].visible = visible;
}

// 原函数：0x00493AF0。
void WindowSettings_RefreshInputPanel(const std::int32_t offset)
{
    WindowInstance* const window = gWindowStates[WindowId_Settings].window;
    window->elements[28].x = offset + 16;
    WindowElement* const elements = window->elements;
    const std::int32_t x = elements[28].x;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[28].data);
    const std::int32_t width = 16 - x;
    graphic->width = width;
    const std::int32_t limit = window->elements[28].width;
    if (width > limit) {
        graphic->width = limit;
    }
    const std::int32_t visible = offset <= -146 ? 1 : 0;
    window->elements[10].visible = visible;
    window->elements[11].visible = visible;
    window->elements[12].visible = visible;
}

// 原函数：0x00493B70。
void WindowSettings_RefreshAudioPanel(const std::int32_t offset)
{
    WindowInstance* const window = gWindowStates[WindowId_Settings].window;
    window->elements[29].x = offset + 16;
    WindowElement* const elements = window->elements;
    const std::int32_t x = elements[29].x;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[29].data);
    const std::int32_t width = 16 - x;
    graphic->width = width;
    const std::int32_t limit = window->elements[29].width;
    if (width > limit) {
        graphic->width = limit;
    }
    if (offset > -146) {
        for (std::int32_t index = 13; index <= 20; ++index) {
            window->elements[index].visible = 0;
        }
        return;
    }
    window->elements[13].visible = 1;
    const std::int32_t musicWidth = WindowNumber_MeasureWidth(gSettingsMusicTextD3041C, 3);
    window->elements[13].x = ((37 - musicWidth) >> 1) + offset + 69;
    window->elements[14].visible = 1;
    window->elements[15].visible = 1;
    window->elements[16].visible = 1;
    const std::int32_t effectWidth = WindowNumber_MeasureWidth(gSettingsEffectTextD2EA64, 3);
    window->elements[16].x = ((37 - effectWidth) >> 1) + offset + 69;
    window->elements[17].visible = 1;
    window->elements[18].visible = 1;
    window->elements[19].visible = 1;
    window->elements[20].visible = 1;
}

// 原函数：0x00493CB0。
void WindowSettings_RefreshDisplayPanel(const std::int32_t offset)
{
    WindowInstance* const window = gWindowStates[WindowId_Settings].window;
    window->elements[30].x = offset + 16;
    WindowElement* const elements = window->elements;
    const std::int32_t x = elements[30].x;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[30].data);
    const std::int32_t width = 16 - x;
    graphic->width = width;
    const std::int32_t limit = window->elements[30].width;
    if (width > limit) {
        graphic->width = limit;
    }
    window->elements[21].visible = 0;
    if (offset <= -146) {
        window->elements[21].visible = 1;
        for (std::int32_t index = 22; index <= 26; ++index) {
            window->elements[index].visible = 0;
        }
        const std::int8_t mode = std::bit_cast<std::int8_t>(platform::gSupportedRenderMode);
        window->elements[mode + 22].visible = 1;
    }
}

// 原函数：0x00493D60。
std::int32_t WindowSettings_Update(const std::int32_t value)
{
    if (value == -1) {
        for (std::int32_t index = 8; index <= 30; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gSettingsPanelCC4234 = -1;
        gSettingsOffsetCA02DC = 0;
    }
    return 1;
}

// 原函数：0x00493DB0。
// 关联原入口：0x00493DF9（共享尾已并入本函数正文）。
std::int32_t WindowSettings_Draw(const std::int32_t)
{
    const std::int8_t panel = gSettingsPanelCC4234;
    if (panel >= 0) {
        std::int32_t offset = gSettingsOffsetCA02DC;
        if (offset > -146) {
            const std::int32_t step = gSettingsStepD2D610;
            offset -= step;
            gSettingsOffsetCA02DC = offset;
            gSettingsStepD2D610 = step - 1;
            if (offset < -146) {
                offset = -146;
                gSettingsOffsetCA02DC = offset;
            }
            kSettingsPanelCallbacks57DF38[panel](offset);
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x00493E10。
void WindowSettings_Release()
{
    static_cast<void>(login::detail::Sub_520470());
    static_cast<void>(login::detail::Sub_520590());
    const std::int32_t selected = login::gSelectedCharacterIndex;
    static_cast<void>(login::detail::Sub_520510(selected));
    static_cast<void>(login::detail::Sub_520640());
    static_cast<void>(login::detail::Sub_5206F0());
    static_cast<void>(login::detail::Sub_520200());
    static_cast<void>(audio::PlaySe_52C450(57, 320));
}

// 原函数：0x00493EE0。
std::int32_t WindowSettings_ButtonLogoutPanel(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    graphic->imageNo = 0x3B8BE;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B8C0;
        gHoverTextD68AE8 = kSettingsLogout586DE8;
    }
    if ((state & 2) != 0) {
        const std::int32_t visible = gCurrentWindow->elements[27].visible;
        for (std::int32_t element = 8; element <= 30; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gSettingsPanelCC4234 = -1;
        gSettingsOffsetCA02DC = 0;
        if (visible == 0) {
            gSettingsPanelCC4234 = 0;
            gSettingsStepD2D610 = 17;
            gCurrentWindow->elements[8].visible = 1;
            gCurrentWindow->elements[9].visible = 1;
            gCurrentWindow->elements[27].visible = 1;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (gCurrentWindow->elements[27].visible != 0) {
        graphic->imageNo = 0x3B8BF;
    }
    return consumed;
}

// 原函数：0x00494000。
std::int32_t WindowSettings_ButtonInputPanel(const std::int32_t index, const std::int32_t state)
{
    auto* graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    graphic->imageNo = 0x3B8C1;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B8C3;
        gHoverTextD68AE8 = kSettingsInput586DF8;
    }
    if ((state & 2) != 0) {
        const std::int32_t visible = gCurrentWindow->elements[28].visible;
        for (std::int32_t element = 8; element <= 30; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gSettingsPanelCC4234 = -1;
        gSettingsOffsetCA02DC = 0;
        if (visible == 0) {
            gSettingsPanelCC4234 = 1;
            gSettingsStepD2D610 = 17;
            gCurrentWindow->elements[10].visible = 1;
            gCurrentWindow->elements[11].visible = 1;
            gCurrentWindow->elements[12].visible = 1;
            gCurrentWindow->elements[28].visible = 1;
            auto* const inputGraphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[10].data);
            inputGraphic->imageNo = 0x3B8DE;
            if (platform::gValueD67AE8 != 0) {
                inputGraphic->imageNo = 0x3B8DB;
            }
            // 原494121改写EBP；末尾选中图也写这个借用载荷。
            graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[11].data);
            graphic->imageNo = 0x3B8E4;
            if (platform::gValueD67AE4 != 0) {
                graphic->imageNo = 0x3B8E1;
            }
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (gCurrentWindow->elements[28].visible != 0) {
        graphic->imageNo = 0x3B8C2;
    }
    return consumed;
}

// 原函数：0x00494180。
std::int32_t WindowSettings_ButtonAudioPanel(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    graphic->imageNo = 0x3B8C4;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B8C6;
        gHoverTextD68AE8 = kSettingsAudio586E14;
    }
    if ((state & 2) != 0) {
        const std::int32_t visible = gCurrentWindow->elements[29].visible;
        for (std::int32_t element = 8; element <= 30; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gSettingsPanelCC4234 = -1;
        gSettingsOffsetCA02DC = 0;
        if (visible == 0) {
            gSettingsPanelCC4234 = 2;
            gSettingsStepD2D610 = 17;
            gCurrentWindow->elements[19].visible = 1;
            gCurrentWindow->elements[13].visible = 1;
            gCurrentWindow->elements[14].visible = 1;
            gCurrentWindow->elements[15].visible = 1;
            gCurrentWindow->elements[16].visible = 1;
            gCurrentWindow->elements[17].visible = 1;
            gCurrentWindow->elements[18].visible = 1;
            gCurrentWindow->elements[29].visible = 1;
            cg::text::FormatTo(gSettingsMusicTextD3041C, L"{}", platform::gMusicVolume);
            const std::int32_t musicWidth = WindowNumber_MeasureWidth(gSettingsMusicTextD3041C, 3);
            gCurrentWindow->elements[13].x =
                ((37 - musicWidth) >> 1) + gSettingsOffsetCA02DC + 69;
            cg::text::FormatTo(gSettingsEffectTextD2EA64, L"{}", platform::gEffectVolume);
            const std::int32_t effectWidth = WindowNumber_MeasureWidth(gSettingsEffectTextD2EA64, 3);
            gCurrentWindow->elements[16].x =
                ((37 - effectWidth) >> 1) + gSettingsOffsetCA02DC + 69;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (gCurrentWindow->elements[29].visible != 0) {
        graphic->imageNo = 0x3B8C5;
    }
    return consumed;
}

// 原函数：0x00494370。
std::int32_t WindowSettings_ButtonDisplayPanel(const std::int32_t index, const std::int32_t state)
{
    auto* graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    graphic->imageNo = 0x3B8C7;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B8C9;
        gHoverTextD68AE8 = kSettingsDisplay586E24;
    }
    if ((state & 2) != 0) {
        const std::int32_t visible = gCurrentWindow->elements[30].visible;
        for (std::int32_t element = 8; element <= 30; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gSettingsPanelCC4234 = -1;
        gSettingsOffsetCA02DC = 0;
        if (visible == 0) {
            gSettingsPanelCC4234 = 3;
            gSettingsStepD2D610 = 17;
            gCurrentWindow->elements[21].visible = 1;
            gCurrentWindow->elements[30].visible = 1;
            // 原49444C改写EBP，后续选中图仍写元素21。
            graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[21].data);
            graphic->imageNo = 0x3B8F9;
            if (platform::gScanlineSyncMode != 0) {
                graphic->imageNo = 0x3B8FC;
            }
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (gCurrentWindow->elements[30].visible != 0) {
        graphic->imageNo = 0x3B8C8;
    }
    return consumed;
}

// 原函数：0x00493E50。
std::int32_t WindowSettings_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsClose55331C;
    }
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        consumed = 1;
    }
    graphic->imageNo = 0x3B538;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B53A;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B539;
    }
    return consumed;
}

// 原函数：0x004944A0。
std::int32_t WindowSettings_ButtonHotkeys(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    graphic->imageNo = 0x3B8CA;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B8CC;
        gHoverTextD68AE8 = kSettingsHotkeys586E34;
    }
    if ((state & 2) != 0) {
        WindowInstance* const hotkeys = gWindowStates[WindowId_Hotkeys].window;
        if (hotkeys != nullptr) {
            hotkeys->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        } else {
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Hotkeys, 1, 0, nullptr));
        }
        consumed = 1;
    }
    if (gWindowStates[WindowId_Hotkeys].window != nullptr) {
        graphic->imageNo = 0x3B8CB;
    }
    return consumed;
}

// 原函数：0x00494550。
std::int32_t WindowSettings_ButtonLogout(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsLogoutTown586E48;
        window = gCurrentWindow;
    }
    if ((state & 2) != 0) {
        window->state |= 0x2000u;
        static_cast<void>(network::Sub_512CF0(platform::gConnectionSocket));
        if (world::gCurrentSceneType == 10) {
            world::gCurrentSceneState = 7;
        }
        static_cast<void>(audio::PlaySe_52C450(60, 320));
        consumed = 1;
        world::gValueD67E78 = 1;
    }
    graphic->imageNo = 0x3B8D1;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B8D3;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B8D2;
    }
    return consumed;
}

// 原函数：0x00494620。
std::int32_t WindowSettings_ButtonServerSelect(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B8D6;
        gHoverTextD68AE8 = kSettingsLogoutServer586E60;
        window = gCurrentWindow;
    }
    if ((state & 2) != 0) {
        window->state |= 0x2000u;
        if (world::gCurrentSceneType == 10) {
            world::gCurrentSceneState = 9;
        } else {
            *world::detail::BorrowValue73ECD0Storage() = std::int32_t{0};
            static_cast<void>(world::SetPendingSceneType(7));
        }
        static_cast<void>(audio::PlaySe_52C450(60, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B8D4;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B8D6;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B8D5;
    }
    return consumed;
}

// 原函数：0x00494720。
std::int32_t WindowSettings_ToggleItemStacking(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsStackMode586E7C;
    }
    if ((state & 2) != 0) {
        window->state |= 0x2000u;
        platform::gValueD67AE8 = (platform::gValueD67AE8 - 1u) & 1u;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (platform::gValueD67AE8 != 0) {
        graphic->imageNo = 0x3B8DB;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8DD;
        }
    } else {
        graphic->imageNo = 0x3B8DE;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8E0;
        }
    }
    return consumed;
}

// 原函数：0x004947F0。
std::int32_t WindowSettings_ToggleCursor(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsCursor586E9C;
    }
    if ((state & 2) != 0) {
        window->state |= 0x2000u;
        platform::gValueD67AE4 = platform::gValueD67AE4 == 0 ? 1u : 0u;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (platform::gValueD67AE4 != 0) {
        graphic->imageNo = 0x3B8E1;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8E3;
        }
        const std::int32_t y = (platform::BorrowLegacyMouseStateStorage()->mouseY + 16);
        const std::int32_t x = (platform::BorrowLegacyMouseStateStorage()->mouseX + 16);
        static_cast<void>(login::detail::DrawGraphic(x, y, 231, 22000, 0, nullptr));
    } else {
        graphic->imageNo = 0x3B8E4;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8E6;
        }
    }
    return consumed;
}

// 原函数：0x004948F0。
std::int32_t WindowSettings_CycleHotkeyMode(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsKeyMode586EB0;
    }
    if ((state & 2) != 0) {
        window->state |= 0x2000u;
        const std::int32_t mode = gValueCF4AAC + 1;
        gValueCF4AAC = mode;
        if (mode == 2) {
            gValueCF4AAC = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = kSettingsKeyImages54F460[3 * gValueCF4AAC];
    if ((state & 1) != 0) {
        graphic->imageNo = kSettingsKeyImages54F460[3 * gValueCF4AAC + 2];
    }
    return consumed;
}

// 原函数：0x004949C0。
std::int32_t WindowSettings_ButtonMusicDecrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = platform::gMusicVolume <= 0
            ? kSettingsMusicMin586EC4 : kSettingsMusicDown586EE0;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        window->state |= 0x2000u;
        const std::int32_t volume = platform::gMusicVolume;
        if (volume <= 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            platform::gMusicVolume = volume - 1;
            static_cast<void>(audio::Sub_52C3D0());
            cg::text::FormatTo(gSettingsMusicTextD3041C, L"{}", platform::gMusicVolume);
            const std::int32_t width = WindowNumber_MeasureWidth(gSettingsMusicTextD3041C, 3);
            const std::int32_t x = ((37 - width) >> 1) + gSettingsOffsetCA02DC + 69;
            gCurrentWindow->elements[13].x = x;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557;
    }
    return consumed;
}

// 原函数：0x00494AE0。
std::int32_t WindowSettings_ButtonMusicIncrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = platform::gMusicVolume >= 15
            ? kSettingsMusicMax586EF8 : kSettingsMusicUp586F14;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        window->state |= 0x2000u;
        const std::int32_t volume = platform::gMusicVolume;
        if (volume >= 15) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            platform::gMusicVolume = volume + 1;
            static_cast<void>(audio::Sub_52C3D0());
            cg::text::FormatTo(gSettingsMusicTextD3041C, L"{}", platform::gMusicVolume);
            const std::int32_t width = WindowNumber_MeasureWidth(gSettingsMusicTextD3041C, 3);
            const std::int32_t x = ((37 - width) >> 1) + gSettingsOffsetCA02DC + 69;
            gCurrentWindow->elements[13].x = x;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B55B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B55A;
    }
    return consumed;
}

// 原函数：0x00494C10。
std::int32_t WindowSettings_ButtonSoundDecrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = platform::gEffectVolume <= 0
            ? kSettingsEffectMin586F2C : kSettingsEffectDown586F44;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        window->state |= 0x2000u;
        const std::int32_t volume = platform::gEffectVolume;
        if (volume <= 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            platform::gEffectVolume = volume - 1;
            static_cast<void>(audio::Sub_52C3D0());
            cg::text::FormatTo(gSettingsEffectTextD2EA64, L"{}", platform::gEffectVolume);
            const std::int32_t width = WindowNumber_MeasureWidth(gSettingsEffectTextD2EA64, 3);
            const std::int32_t x = ((37 - width) >> 1) + gSettingsOffsetCA02DC + 69;
            gCurrentWindow->elements[16].x = x;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557;
    }
    return consumed;
}

// 原函数：0x00494D30。
std::int32_t WindowSettings_ButtonSoundIncrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = platform::gEffectVolume >= 15
            ? kSettingsEffectMax586F54 : kSettingsEffectUp586F6C;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        window->state |= 0x2000u;
        const std::int32_t volume = platform::gEffectVolume;
        if (volume >= 15) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            platform::gEffectVolume = volume + 1;
            static_cast<void>(audio::Sub_52C3D0());
            cg::text::FormatTo(gSettingsEffectTextD2EA64, L"{}", platform::gEffectVolume);
            const std::int32_t width = WindowNumber_MeasureWidth(gSettingsEffectTextD2EA64, 3);
            const std::int32_t x = ((37 - width) >> 1) + gSettingsOffsetCA02DC + 69;
            gCurrentWindow->elements[16].x = x;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B55B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B55A;
    }
    return consumed;
}

// 原函数：0x00494E60。
std::int32_t WindowSettings_ToggleChannels(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsOutput586F7C;
    }
    if ((state & 2) != 0) {
        window->state |= 0x2000u;
        platform::gUseStereo = static_cast<std::int32_t>(
            (static_cast<std::uint32_t>(platform::gUseStereo) - 1u) & 1u);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (platform::gUseStereo != 0) {
        graphic->imageNo = 0x3B8EF;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8F1;
        }
    } else {
        graphic->imageNo = 0x3B8F2;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8F4;
        }
    }
    return consumed;
}

// 原函数：0x00494F30。
std::int32_t WindowSettings_ToggleAudioFlag(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsOutput586F7C;
    }
    if ((state & 2) != 0) {
        window->state |= 0x2000u;
        platform::gValue554644 = static_cast<std::int32_t>(
            (static_cast<std::uint32_t>(platform::gValue554644) - 1u) & 1u);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (platform::gValue554644 != 0) {
        graphic->imageNo = 0x3BE7B;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3BE7D;
        }
    } else {
        graphic->imageNo = 0x3BE78;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3BE7A;
        }
    }
    return consumed;
}

// 原函数：0x00495000。
std::int32_t WindowSettings_ToggleScanlines(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsDisplay586E24;
    }
    if ((state & 2) != 0) {
        window->state |= 0x2000u;
        platform::gScanlineSyncMode = static_cast<std::uint16_t>(
            (static_cast<std::uint8_t>(platform::gScanlineSyncMode) - 1) & 1);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (platform::gScanlineSyncMode != 0) {
        graphic->imageNo = 0x3B8FC;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8FE;
        }
    } else {
        graphic->imageNo = 0x3B8F9;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8FB;
        }
    }
    return consumed;
}

// 原函数：0x004950D0。
std::int32_t WindowSettings_DrawRenderModeButton(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kSettingsRenderMode586F90;
    }
    graphic->imageNo = 0x3B5A0 + 3 * index;
    return 0;
}

} // namespace detail

} // namespace cg::ui
