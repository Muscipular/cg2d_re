#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00495FC0。
static std::int32_t WindowEmote_ResetSelection(); // 按四W32/一W16清唯一18B选择区，返回0。

// 原函数：0x00495FE0。
static std::int32_t WindowEmote_ButtonEmote(std::int32_t index, std::int32_t state); // 情绪窗18按钮，请求被拒仍更新选择并发声。

// 原函数：0x00495F50。
static std::int32_t WindowEmote_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static constexpr const wchar_t* kEmoteHints5871B0[18] { // 原常量参考：0x005871B0。
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

static constexpr std::int32_t kEmoteButtonActions[18] {
    0, 2, 4, 6, 8, 10, 12, 14, 16, 1, 3, 5, 7, 9, 11, 13, 15, 17,
};

static WindowGraphicData gWindowGraphic56B0D8 {0x3B985, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B0D8。

static WindowGraphicData gWindowGraphic56B0F0 {0x3B988, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B0F0。

static WindowGraphicData gWindowGraphic56B108 {0x3B98B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B108。

static WindowGraphicData gWindowGraphic56B120 {0x3B98E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B120。

static WindowGraphicData gWindowGraphic56B138 {0x3B991, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B138。

static WindowGraphicData gWindowGraphic56B150 {0x3B994, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B150。

static WindowGraphicData gWindowGraphic56B168 {0x3B997, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B168。

static WindowGraphicData gWindowGraphic56B180 {0x3B99A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B180。

static WindowGraphicData gWindowGraphic56B198 {0x3B99D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B198。

static WindowGraphicData gWindowGraphic56B1B0 {0x3B9A0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1B0。

static WindowGraphicData gWindowGraphic56B1C8 {0x3B9A3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1C8。

static WindowGraphicData gWindowGraphic56B1E0 {0x3B9A6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1E0。

static WindowGraphicData gWindowGraphic56B1F8 {0x3B9A9, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1F8。

static WindowGraphicData gWindowGraphic56B210 {0x3B9AC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B210。

static WindowGraphicData gWindowGraphic56B228 {0x3B9AF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B228。

static WindowGraphicData gWindowGraphic56B240 {0x3B9B2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B240。

static WindowGraphicData gWindowGraphic56B258 {0x3B9B5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B258。

static WindowGraphicData gWindowGraphic56B270 {0x3B9B8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B270。

static WindowGraphicData gWindowGraphic56B288 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B288。

static WindowGraphicData gWindowGraphic56B2A0 {0x3B984, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B2A0。

static WindowElementPrototype gWindowElements56B2B8[20] { // 原全局：0x0056B2B8。
    {WindowElementType_Graphic, 5, 25, 23, 23, 1, {.graphic = &gWindowGraphic56B0D8}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 50, 23, 23, 1, {.graphic = &gWindowGraphic56B0F0}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 75, 23, 23, 1, {.graphic = &gWindowGraphic56B108}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 100, 23, 23, 1, {.graphic = &gWindowGraphic56B120}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 125, 23, 23, 1, {.graphic = &gWindowGraphic56B138}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 150, 23, 23, 1, {.graphic = &gWindowGraphic56B150}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 175, 23, 23, 1, {.graphic = &gWindowGraphic56B168}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 200, 23, 23, 1, {.graphic = &gWindowGraphic56B180}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 225, 23, 23, 1, {.graphic = &gWindowGraphic56B198}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 25, 23, 23, 1, {.graphic = &gWindowGraphic56B1B0}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 50, 23, 23, 1, {.graphic = &gWindowGraphic56B1C8}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 75, 23, 23, 1, {.graphic = &gWindowGraphic56B1E0}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 100, 23, 23, 1, {.graphic = &gWindowGraphic56B1F8}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 125, 23, 23, 1, {.graphic = &gWindowGraphic56B210}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 150, 23, 23, 1, {.graphic = &gWindowGraphic56B228}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 175, 23, 23, 1, {.graphic = &gWindowGraphic56B240}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 200, 23, 23, 1, {.graphic = &gWindowGraphic56B258}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 225, 23, 23, 1, {.graphic = &gWindowGraphic56B270}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 45, 7, 11, 11, 1, {.graphic = &gWindowGraphic56B288}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B2A0}, &WindowElement_NoOp},
};

// 原型：窗口34；原全局：0x0054E6F0。
WindowPrototype gWindowPrototypeEmote {0, 4, 575, 97, 61, 260, 0x80010101u, 20, gWindowElements56B2B8, &WindowEmote_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 34: 54E6F0

// 窗口函数实现。

static void WindowEmote_ClearSelection()
{
    const std::uint32_t zero32 = 0;
    const std::uint16_t zero16 = 0;
    std::memcpy(gEmoteSelectionCF4FC4.data(), &zero32, sizeof(zero32));
    std::memcpy(gEmoteSelectionCF4FC4.data() + 4, &zero32, sizeof(zero32));
    std::memcpy(gEmoteSelectionCF4FC4.data() + 8, &zero32, sizeof(zero32));
    std::memcpy(gEmoteSelectionCF4FC4.data() + 12, &zero32, sizeof(zero32));
    std::memcpy(gEmoteSelectionCF4FC4.data() + 16, &zero16, sizeof(zero16));
}

// 原函数：0x00495F50。
static std::int32_t WindowEmote_Update(const std::int32_t)
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
        world::detail::gPlayerY.value();
    const std::int32_t x =
        world::detail::gPlayerX.value();
    return WindowProfile_SaveOrigin(x, y);
}

// 原函数：0x00495FC0。
static std::int32_t WindowEmote_ResetSelection()
{
    WindowEmote_ClearSelection();
    return 0;
}

// 原函数：0x00495FE0。
static std::int32_t WindowEmote_ButtonEmote(const std::int32_t index, const std::int32_t state)
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
