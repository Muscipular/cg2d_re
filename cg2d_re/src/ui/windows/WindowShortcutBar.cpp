#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {

constexpr wchar_t kQuickDuel556C80[] = L"要求進行對戰。"; // 原常量参考：0x00556C80。

constexpr wchar_t kQuickWatch556C90[] = L"要求進行觀戰。"; // 原常量参考：0x00556C90。

constexpr wchar_t kQuickTeam556CA0[] = L"要求加入冒險隊伍。"; // 原常量参考：0x00556CA0。

constexpr wchar_t kQuickCard556CB4[] = L"要求交換名片。"; // 原常量参考：0x00556CB4。

constexpr wchar_t kQuickTrade556CD8[] = L"要求進行交易。"; // 原常量参考：0x00556CD8。

constexpr wchar_t kQuickEmote556CE8[] = L"打開情緒動作視窗。"; // 原常量参考：0x00556CE8。

constexpr wchar_t kQuickMap58625C[] = L"打開小地圖視窗。"; // 原常量参考：0x0058625C。

constexpr wchar_t kQuickGuild586284[] = L"邀請對方加入自己的家族。"; // 原常量参考：0x00586284。

constexpr wchar_t kQuickStall5862A0[] = L"進行擺攤"; // 原常量参考：0x005862A0。
} // namespace


// 原函数：0x0046FBA0。
std::int32_t WindowShortcutBar_Update(const std::int32_t)
{
    if (world::gCurrentSceneType == 10) {
        gCurrentWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x0046FBD0。
std::int32_t WindowShortcutBar_ButtonCollapse(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(WindowId_CollapsedShortcutBar));
    gValueCA0440 = 1;
    return 1;
}

// 原函数：0x0046FC10。
std::int32_t WindowShortcutBar_ButtonExpand(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(WindowId_ShortcutBar));
    gValueCA0440 = 0;
    return 1;
}

// 原函数：0x0046FC60。
std::int32_t WindowShortcutBar_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    std::int32_t consumed = hover ? 1 : 0;
    if (index == 8) {
        if (hover) {
            gHoverTextD68AE8 = kQuickStall5862A0;
            graphic->imageNo = 0x18224;
        } else {
            graphic->imageNo = 0x18225;
        }
        if ((state & 8) != 0) {
            graphic->imageNo = 0x18223;
            consumed = 1;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        if (world::gCurrentSceneType != 10) {
            const std::uint32_t now = platform::ReadTickCount32();
            if (gWindowActionTick62B5AC + 500u < now) {
                WindowInstance* const window = gWindowStates[WindowId_OwnStall].window;
                if (window == nullptr) {
                    static_cast<void>(WindowStallBrowse_RequestClose());
                    static_cast<void>(audio::PlaySe_52C450(56, 320));
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_OwnStall, 1, 0, nullptr));
                } else {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
                gWindowActionTick62B5AC = platform::ReadTickCount32();
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        }
        return 1;
    }

    graphic->imageNo = 0x3B90E + index * 3 + (hover ? 2 : 0);
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B90F + index * 3;
    }
    if (static_cast<std::uint32_t>(index) > 7u) {
        return consumed;
    }
    if (index < 5) {
        const wchar_t* const hints[5] {
            kQuickDuel556C80, kQuickWatch556C90, kQuickTeam556CA0,
            kQuickCard556CB4, kQuickTrade556CD8,
        };
        void (*const requests[5])() {
            &world::detail::Sub_470050, &world::detail::Sub_470150,
            &world::detail::Sub_470270, &world::detail::Sub_4703C0,
            &world::detail::Sub_4704A0,
        };
        if (hover) {
            gHoverTextD68AE8 = hints[index];
        }
        if ((state & 2) != 0) {
            requests[index]();
            return 1;
        }
        return consumed;
    }
    if (index == 5) {
        if (character_select::state::gPukVersion < 7u) {
            if (hover) {
                graphic->imageNo = 0x3B91D;
                gHoverTextD68AE8 = kStatusGuildUnavailable586270;
            }
            return consumed;
        }
        if (hover) {
            gHoverTextD68AE8 = kQuickGuild586284;
        }
        if ((state & 2) != 0) {
            world::detail::Sub_470670();
            return 1;
        }
        return consumed;
    }
    if (index == 6) {
        if (hover) {
            gHoverTextD68AE8 = kQuickEmote556CE8;
        }
        return WindowStatusBar_ButtonEmote(6, state);
    }
    if (hover) {
        gHoverTextD68AE8 = kQuickMap58625C;
    }
    return WindowStatusBar_ButtonMiniMap(7, state);
}

} // namespace detail

} // namespace cg::ui
