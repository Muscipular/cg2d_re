#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x004CB430。
std::int32_t WindowProfileView_ReadSelection(const std::int32_t index)
{
    if (index - 79 < 32) {
        return world::detail::ReadCategoryRecord105FA24(0, index - 78).selected10C != 0 ? 1 : 0;
    }
    if (index - 79 < 64) {
        return world::detail::ReadCategoryRecord105FA24(1, index - 110).selected10C != 0 ? 1 : 0;
    }
    return world::detail::ReadCategoryRecord105FA24(2, index - 142).selected10C != 0 ? 1 : 0;
}

// 原函数：0x004CAFA0。
std::uint32_t WindowProfileView_Refresh()
{
    const world::detail::ProfileRuntimeRecord& profile =
        world::detail::ReadViewedProfile105ECE0();
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[14].data)->imageNo =
        profile.value10C;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index + 15].data)->imageNo =
            profile.entries410[index].value04;
    }
    static_cast<WindowTextData*>(gCurrentWindow->elements[1].data)->text = cg::text::FromGbkWide(profile.text08.data());
    std::wstring number;
    cg::text::FormatTo(number, L"{:3}", profile.value108);
    static_cast<WindowTextData*>(gCurrentWindow->elements[2].data)->text = number;
    static_cast<WindowTextData*>(gCurrentWindow->elements[3].data)->text = cg::text::FromGbkWide(profile.text110.data());
    static_cast<WindowTextData*>(gCurrentWindow->elements[4].data)->text = cg::text::FromGbkWide(profile.text210.data());
    static_cast<WindowTextData*>(gCurrentWindow->elements[5].data)->text = cg::text::FromGbkWide(profile.text310.data());
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<WindowTextData*>(gCurrentWindow->elements[index + 6].data)->text = cg::text::FromGbkWide(profile.entries410[index].text08.data());
    }
    std::wstring line;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<void>(input::ExtractGbkTextLine(line, profile.text728.data(), index, 28));
        static_cast<WindowTextData*>(gCurrentWindow->elements[index + 9].data)->text = line;
    }
    static_cast<void>(input::ExtractGbkTextLine(line, profile.text728.data(), 3, 28));
    std::wstring& destination = static_cast<WindowTextData*>(gCurrentWindow->elements[12].data)->text;
    const auto units = static_cast<std::uint32_t>(line.size() + 1);
    destination = line;
    return units;
}

// 原函数：0x004CAF60。
// 关联原入口：0x004CAF88（共享尾已并入本函数正文）。
std::int32_t WindowProfileView_Update(const std::int32_t)
{
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    static_cast<void>(WindowProfileView_Refresh());
    return 1;
}

// 原函数：0x004CB9B0。
WindowInstance* WindowProfileView_RequestCategoryClose(const std::int32_t index)
{
    WindowInstance* const window = gWindowStates[59 + index].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

// 原函数：0x004CB530。
void WindowProfileView_Release()
{
    world::detail::gValue581800 = -1;
    static_cast<void>(WindowProfileView_RequestCategoryClose(4));
}

// 原函数：0x004CB2F0。
std::int32_t WindowProfileView_ButtonMail(const std::int32_t index, const std::int32_t state)
{
    if (index != 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[0].data)->imageNo = 244253;
        std::wstring hint;
        cg::text::FormatTo(hint, kMiniMailCostHint588588, world::detail::ReadCategoryValue105FA20());
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[0].data)->imageNo = 244251;
    }
    if ((state & 2) != 0) {
        const std::int32_t money = world::detail::ReadPlayerValueFC();
        if (money >= world::detail::ReadCategoryValue105FA20()) {
            const auto& profile = world::detail::ReadViewedProfile105ECE0();
            gMiniMailCompose105F508.target = profile.value00;
            std::strcpy(gMiniMailCompose105F508.name.data(), profile.text08.data());
            std::strcpy(gMiniMailCompose105F508.text104.data(), input::LegacyText593850());
            static_cast<void>(WindowRuntime_Open(WindowId_MiniMail));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        return 1;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
