#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {

constexpr const wchar_t* kEmoteHints5871B0[18] { // 原常量参考：0x005871B0。
    L"做「坐下」這個動作。",
    L"做「揮手」這個動作。",
    L"做「點頭」這個動作。",
    L"做「高興」這個動作。",
    L"做「憤怒」這個動作。",
    L"做「悲傷」這個動作。",
    L"做「投擲」這個動作。",
    L"做「出剪刀」這個動作。",
    L"做「出布」這個動作。",
    L"做「站立」這個動作。",
    L"做「走路」這個動作。",
    L"做「倒下」這個動作。",
    L"做「攻擊」這個動作。",
    L"做「防禦」這個動作。",
    L"做「受傷」這個動作。",
    L"做「魔法」這個動作。",
    L"做「移動中」這個動作。",
    L"做「出石頭」這個動作。",
};

constexpr std::int32_t kEmoteButtonActions[18] {
    0, 2, 4, 6, 8, 10, 12, 14, 16, 1, 3, 5, 7, 9, 11, 13, 15, 17,
};

void WindowEmote_ClearSelection()
{
    const std::uint32_t zero32 = 0;
    const std::uint16_t zero16 = 0;
    std::memcpy(gEmoteSelectionCF4FC4.data(), &zero32, sizeof(zero32));
    std::memcpy(gEmoteSelectionCF4FC4.data() + 4, &zero32, sizeof(zero32));
    std::memcpy(gEmoteSelectionCF4FC4.data() + 8, &zero32, sizeof(zero32));
    std::memcpy(gEmoteSelectionCF4FC4.data() + 12, &zero32, sizeof(zero32));
    std::memcpy(gEmoteSelectionCF4FC4.data() + 16, &zero16, sizeof(zero16));
}
} // namespace


// 原函数：0x00495F50。
std::int32_t WindowEmote_Update(const std::int32_t)
{
    if (WindowProfile_CheckDistance(1, 1) != 0) {
        static_cast<void>(WindowEmote_ResetSelection());
    }
    return 1;
}

// 原函数：0x00495F70。
std::int32_t WindowEmote_Select(const std::int32_t index)
{
    WindowEmote_ClearSelection();
    gEmoteSelectionCF4FC4[index] = 1;
    const std::int32_t y =
        world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x =
        world::detail::BorrowPlayerXStorage()->value();
    return WindowProfile_SaveOrigin(x, y);
}

// 原函数：0x00495FC0。
std::int32_t WindowEmote_ResetSelection()
{
    WindowEmote_ClearSelection();
    return 0;
}

// 原函数：0x00495FE0。
std::int32_t WindowEmote_ButtonEmote(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    std::int32_t consumed = 0;
    if (hover) {
        graphic->imageNo = 0x3B987 + index * 3;
        consumed = 1;
    } else {
        const bool selected = gEmoteSelectionCF4FC4[index] == 1;
        graphic->imageNo = (selected ? 0x3B985 : 0x3B986) + index * 3;
    }
    if (static_cast<std::uint32_t>(index) > 17u) {
        return consumed;
    }
    if (hover) {
        gHoverTextD68AE8 = kEmoteHints5871B0[index];
    }
    if ((state & 2) != 0) {
        world::detail::Sub_4FA8E0(kEmoteButtonActions[index]);
        static_cast<void>(WindowEmote_Select(index));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
