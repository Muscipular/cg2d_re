#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {

constexpr wchar_t kWindow47Delimiter5531C4[] = L"\n"; // 原常量参考：0x005531C4。

constexpr wchar_t kWindow47Space55C9BC[] = L" "; // 原常量参考：0x0055C9BC。

std::int32_t WindowConfirmation_HandleButton(
    const std::int32_t index, const std::int32_t state, const std::int32_t idleImage,
    WindowElementCallback* const callbackSlot, const std::int32_t sound, const bool requiresControl)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = idleImage;
    if ((state & 1) != 0) {
        graphic->imageNo = idleImage + 2;
    }
    if ((state & 2) == 0 || (requiresControl && (platform::gKeyStates[17] & 2) == 0)) {
        return 0;
    }
    static_cast<void>(WindowConfirmation_RequestClose());
    const WindowElementCallback callback = *callbackSlot;
    if (callback != nullptr) {
        static_cast<void>(callback(index, state));
    }
    static_cast<void>(audio::PlaySe_52C450(sound, 320));
    return 1;
}
} // namespace


// 原函数：0x004AE5F0。
std::int32_t WindowConfirmation_Update(const std::int32_t value)
{
    if (value == -1) {
        static_cast<void>(WindowConfirmation_RefreshButtons());
        static_cast<void>(WindowConfirmation_RefreshText());
        const WindowCallback callback = gWindow47Runtime.initializeCC4250;
        if (callback != nullptr) {
            static_cast<void>(callback(-1));
        }
    }
    return 1;
}

// 原函数：0x004AE620。
std::int32_t WindowConfirmation_ButtonOk(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE4B, &gWindow47Runtime.actionsCC4254[0], 51, true);
}

// 原函数：0x004AE690。
std::int32_t WindowConfirmation_ButtonCancel(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE4E, &gWindow47Runtime.actionsCC4254[1], 55, false);
}

// 原函数：0x004AE700。
std::int32_t WindowConfirmation_ButtonActionTwo(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3B568, &gWindow47Runtime.actionsCC4254[2], 51, false);
}

// 原函数：0x004AE770。
std::int32_t WindowConfirmation_ButtonActionThree(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE48, &gWindow47Runtime.actionsCC4254[3], 55, false);
}

// 原函数：0x004AE7E0。
std::int32_t WindowConfirmation_ButtonActionFour(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE54, &gWindow47Runtime.actionsCC4254[4], 51, false);
}

// 原函数：0x004AE850。
std::int32_t WindowConfirmation_ButtonActionFive(const std::int32_t index, const std::int32_t state)
{
    return WindowConfirmation_HandleButton(index, state, 0x3BE51, &gWindow47Runtime.actionsCC4254[5], 51, false);
}

// 原函数：0x004AE8C0。
WindowInstance* WindowConfirmation_RequestClose()
{
    WindowInstance* const window = gWindowStates[WindowId_Confirmation].window;
    window->state |= 0x8000u;
    return window;
}

// 原函数：0x004AE8D0。
wchar_t* WindowConfirmation_RefreshText()
{
    for (std::int32_t index = 0; index < 11; ++index) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
        data->text = cg::text::FromGbkWide(input::LegacyText593850());
    }
    std::wstring text = gWindow47Runtime.textCC426C;
    std::wstring line;
    wchar_t* context = nullptr;
    wchar_t* token = std::wcstok(text.data(), kWindow47Delimiter5531C4, &context);
    std::int32_t row = 0;
    while (token != nullptr) {
        static_cast<void>(WindowConfirmation_PadText(line, token));
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[row].data);
        data->text = line;
        token = std::wcstok(nullptr, kWindow47Delimiter5531C4, &context);
        ++row;
    }
    return token;
}

// 原函数：0x004AE9C0。
std::int32_t WindowConfirmation_RefreshButtons()
{
    std::int32_t mask = gWindow47Runtime.visibleMaskCC4240;
    for (std::int32_t index = 11; index < 17; ++index) {
        gCurrentWindow->elements[index].visible = 0;
        if ((mask & 1) != 0) {
            gCurrentWindow->elements[index].visible = 1;
        }
        mask >>= 1;
    }
    return 216;
}

// 原函数：0x004AEA10。
std::int32_t WindowConfirmation_PadText(std::wstring& destination, const std::wstring_view text)
{
    std::int32_t length = 0; // 原GBK排版单位：ASCII一列，其余完整字符两列。
    for (std::size_t offset = 0; offset < text.size(); offset = cg::text::Next(text, offset)) {
        length += text[offset] < 0x80 ? 1 : 2;
    }
    const std::int32_t widthUnits = gWindow47Runtime.widthUnitsCC4244;
    std::int32_t spaces = ((widthUnits * 74 - length * 12 - 20) / 2) / 12;
    destination = cg::text::FromGbkWide(input::LegacyText593850());
    while (spaces > 0) {
        destination += kWindow47Space55C9BC;
        --spaces;
    }
    destination += text;
    return 0;
}

} // namespace detail

} // namespace cg::ui
