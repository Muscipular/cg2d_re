#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x004C9F20。
void WindowProfileCategory_Refresh()
{
    constexpr std::array<std::int32_t, 3> counts {32, 32, 15};
    constexpr std::array<std::int32_t, 3> textBases {0, 32, 64};
    for (std::int32_t group = 0; group < 3; ++group) {
        for (std::int32_t index = 1; index <= counts[group]; ++index) {
            const auto& record = world::detail::ReadCategoryRecord105FA24(group, index);
            const std::int32_t textIndex = textBases[group] + index - 1;
            if (record.used != 0) {
                gCurrentWindow->elements[textIndex].visible = 1;
                gCurrentWindow->elements[textIndex + 79].visible = 1;
                static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex].data)->text = cg::text::FromGbkWide(record.text0C.data());
                static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex].data)->bytes00[0] =
                    record.selected10C != 0 ? std::byte{0} : std::byte{7};
            } else {
                gCurrentWindow->elements[textIndex].visible = 0;
                gCurrentWindow->elements[textIndex + 79].visible = 0;
            }
        }
    }
}

// 原函数：0x004C9EE0。
// 关联原入口：0x004C9F08（共享尾已并入本函数正文）。
std::int32_t WindowProfileCategory_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowProfileCategory_Refresh();
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004CA300。
void WindowProfileCategory_Release()
{
    world::detail::gValue581800 = -1;
    static_cast<void>(WindowProfileView_RequestCategoryClose(2));
    static_cast<void>(WindowProfileView_RequestCategoryClose(3));
    static_cast<void>(WindowProfileView_RequestCategoryClose(4));
}

// 原函数：0x004CB4A0。
std::int32_t WindowProfileCategory_ResolveCategory(
    const std::int32_t index, std::int32_t* const group, std::int32_t* const value)
{
    if (index - 79 < 32) {
        *group = 0;
        *value = world::detail::ReadCategoryRecord105FA24(0, index - 78).value04;
    } else if (index - 79 < 64) {
        *group = 1;
        *value = world::detail::ReadCategoryRecord105FA24(1, index - 110).value04;
    } else {
        *group = 2;
        *value = world::detail::ReadCategoryRecord105FA24(2, index - 142).value04;
    }
    return 1;
}

// 原函数：0x004CA1A0。
std::int32_t WindowProfileCategory_SelectCategory(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"在此目錄中搜尋";
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 246042;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0;
    }
    if ((state & 2) == 0 || WindowProfileView_ReadSelection(index) == 0) {
        return consumed;
    }
    std::int32_t group;
    std::int32_t value;
    if (WindowProfileCategory_ResolveCategory(index, &group, &value) != 0) {
        std::array<std::wstring, 2> buffers;
        cg::text::FormatTo(buffers[0], L"{}|{}|", group, value);
        buffers[1] = input::EscapeTextAndNotify(buffers[0], 255);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, buffers[1]));
        gProfileSearchKeyword1075988.clear();
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
