#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

std::int32_t gProfileScrollCA0444 = 0; // 原全局：0x00CA0444。

// 原函数：0x004C9C70。
void WindowOwnProfileCategory_Refresh()
{
    const std::int32_t mode = gProfileCategory1085DA0;
    std::int32_t image;
    switch (mode) {
    case 0: image = 0x3C386; break;
    case 1: image = 0x3C387; break;
    case 2: image = 0x3C385; break;
    default: return;
    }
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[5].data)->imageNo = image;
    for (std::int32_t row = 0; row < 10; ++row) {
        const std::int32_t page = gProfileScrollCA0444;
        const auto& record = world::detail::ReadCategoryRecord105FA24(mode, page + row);
        const char* const source =
            record.used != 0 ? record.text0C.data() : input::LegacyText593850();
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[row + 9].data);
        const std::size_t length = std::strlen(source) + 1;
        data->text = cg::text::FromGbkWide(source);
    }
}

// 原函数：0x004C94B0。
// 关联原入口：0x004C94D6（共享尾已并入本函数正文）。
std::int32_t WindowOwnProfileCategory_Update(const std::int32_t value)
{
    if (value == -1) {
        gProfileScrollCA0444 = 0;
    }
    if (gWindowStates[WindowId_OwnProfile].window == nullptr) {
        gCurrentWindow->state |= 0x8000u;
    }
    WindowOwnProfileCategory_Refresh();
    return 1;
}

static constexpr wchar_t kProfileCategoryHint5884F4[] = L"設定販賣的目錄"; // 原常量参考：0x005884F4。

static constexpr wchar_t kProfileCategoryHint588504[] = L"設定購買的目錄"; // 原常量参考：0x00588504。

static constexpr wchar_t kProfileCategoryHint588514[] = L"設定描述的目錄"; // 原常量参考：0x00588514。

// 原函数：0x004C94F0。
std::int32_t WindowOwnProfileCategory_SelectCategory(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        switch (gProfileCategory1085DA0) {
        case 0: gHoverTextD68AE8 = kProfileCategoryHint5884F4; break;
        case 1: gHoverTextD68AE8 = kProfileCategoryHint588504; break;
        case 2: gHoverTextD68AE8 = kProfileCategoryHint588514; break;
        default: break;
        }
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB04;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB02;
    }

    const std::int32_t selectedMode = gProfileCategory1085DA0;
    if (selectedMode >= 0 && selectedMode <= 2) {
        const std::int32_t page = gProfileScrollCA0444;
        const std::int32_t selected =
            world::detail::BorrowCurrentProfile10738E8().entries410[selectedMode].value00;
        if (selected == page + index - 19) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB03;
        }
    }
    if ((state & 2) == 0) {
        return consumed;
    }

    const std::int32_t mode = gProfileCategory1085DA0;
    if (mode >= 0 && mode <= 2) {
        const std::int32_t page = gProfileScrollCA0444;
        const auto& record = world::detail::ReadCategoryRecord105FA24(mode, page + index - 19);
        if (record.used == 0) {
            return 1;
        }
        const std::int32_t value = record.value04;
        const std::int32_t image = record.value08;
        auto& entry = world::detail::BorrowCurrentProfile10738E8().entries410[mode];
        entry.value00 = value;
        entry.value04 = image;
    }
    static_cast<void>(WindowChat_RestoreInput());
    return 1;
}

// 原函数：0x004C96F0。
std::int32_t WindowOwnProfileCategory_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    const auto setImage = [index](const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = image;
    };
    switch (index) {
    case 3: {
        if ((state & 1) != 0) {
            setImage(0x3B594);
            consumed = 1;
        } else {
            setImage(0x3B592);
        }
        if ((state & 8) != 0) {
            setImage(0x3B593);
            consumed = 1;
        }
        if ((state & 0x400) == 0) {
            return consumed;
        }
        const std::int32_t page = gProfileScrollCA0444 - 1;
        gProfileScrollCA0444 = page;
        if (page < 0) {
            gProfileScrollCA0444 = 0;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t position = gProfileScrollCA0444;
            WindowInstance* window;
            std::int32_t count;
            if (mode == 2) {
                window = gCurrentWindow;
                count = world::detail::ReadCategoryCount105FA1C();
            } else {
                count = world::detail::ReadCategoryCount105FA18();
                window = gCurrentWindow;
            }
            static_cast<void>(WindowScroll_SetPosition(window->elements + 2, count - 10, position));
        }
        return 1;
    }
    case 4: {
        if ((state & 1) != 0) {
            setImage(0x3B597);
            consumed = 1;
        } else {
            setImage(0x3B595);
        }
        if ((state & 8) != 0) {
            setImage(0x3B596);
            consumed = 1;
        }
        if ((state & 0x400) == 0) {
            return consumed;
        }
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C()
                : world::detail::ReadCategoryCount105FA18();
            const std::int32_t page = gProfileScrollCA0444 + 1;
            gProfileScrollCA0444 = page;
            if (page <= count - 11) {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                gProfileScrollCA0444 = count > 10 ? count - 10 : 0;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = gProfileScrollCA0444;
            const std::int32_t currentCount = mode == 2
                ? world::detail::ReadCategoryCount105FA1C()
                : world::detail::ReadCategoryCount105FA18();
            WindowInstance* const window = gCurrentWindow;
            static_cast<void>(WindowScroll_SetPosition(window->elements + 2, currentCount - 10, position));
        }
        return 1;
    }
    case 6:
        setImage((state & 1) != 0 ? 0x3B558 : 0x3B556);
        if ((state & 2) != 0) {
            std::int32_t page = gProfileScrollCA0444 - 10;
            gProfileScrollCA0444 = page;
            if (page < 0) {
                page = 0;
                gProfileScrollCA0444 = page;
            }
            const std::int32_t mode = gProfileCategory1085DA0;
            if (mode >= 0 && mode <= 2) {
                const std::int32_t count = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C()
                    : world::detail::ReadCategoryCount105FA18();
                WindowInstance* const window = gCurrentWindow;
                static_cast<void>(WindowScroll_SetPosition(window->elements + 2, count - 10, page));
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(0x3B557);
        }
        return consumed;
    case 7:
        setImage((state & 1) != 0 ? 0x3B55B : 0x3B559);
        if ((state & 2) != 0) {
            const std::int32_t mode = gProfileCategory1085DA0;
            if (mode >= 0 && mode <= 2) {
                const std::int32_t count = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C()
                    : world::detail::ReadCategoryCount105FA18();
                const std::int32_t page = gProfileScrollCA0444 + 10;
                gProfileScrollCA0444 = page;
                if (page > count - 11) {
                    gProfileScrollCA0444 = count > 10 ? count - 10 : 0;
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                const std::int32_t position = gProfileScrollCA0444;
                const std::int32_t currentCount = mode == 2
                    ? world::detail::ReadCategoryCount105FA1C()
                    : world::detail::ReadCategoryCount105FA18();
                WindowInstance* const window = gCurrentWindow;
                static_cast<void>(WindowScroll_SetPosition(window->elements + 2, currentCount - 10, position));
            }
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(0x3B55A);
        }
        return consumed;
    default:
        return 0;
    }
}

// WSP01：旧X片段1992；恰1且mode0..2才按原分类count-10更新。
// 原函数：0x004C9B80。
std::int32_t WindowOwnProfileCategory_DragScroll(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t result = WindowElement_HandleVerticalScroll(index, state);
    if (result == 1) {
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C()
                : world::detail::ReadCategoryCount105FA18();
            WindowInstance* const window = gCurrentWindow;
            gProfileScrollCA0444 = WindowScroll_ReadPosition(window->elements + 2, count - 10);
        }
    }
    return result;
}

// 原函数：0x004C9BE0。
std::int32_t WindowOwnProfileCategory_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t mode = gProfileCategory1085DA0;
        if (mode >= 0 && mode <= 2) {
            const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
            const std::int32_t count = mode == 2
                ? world::detail::ReadCategoryCount105FA1C()
                : world::detail::ReadCategoryCount105FA18();
            const std::int32_t page = gProfileScrollCA0444;
            WindowInstance* const window = gCurrentWindow;
            gProfileScrollCA0444 = WindowScroll_ApplyWheel(window->elements + 2, page, count - 10, wheel);
        }
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
