#include "../../scene/login/LoginSavedState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowSettingsElementId : ElementId {
    ButtonClose = 1,
    ButtonLogoutPanel = 2,
    ButtonInputPanel = 3,
    ButtonAudioPanel = 4,
    ButtonDisplayPanel = 5,
    ButtonHotkeys = 6,
    Element7 = 7,
    Element8 = 8,
    ButtonLogout = 9,
    ButtonServerSelect = 10,
    ToggleItemStacking = 11,
    ToggleCursor = 12,
    CycleHotkeyMode = 13,
    Element14 = 14,
    ButtonMusicDecrease = 15,
    ButtonMusicIncrease = 16,
    Element17 = 17,
    ButtonSoundDecrease = 18,
    ButtonSoundIncrease = 19,
    ToggleChannels = 20,
    ToggleAudioFlag = 21,
    ToggleScanlines = 22,
    DrawRenderModeButton = 23,
    DrawRenderModeButton_24 = 24,
    DrawRenderModeButton_25 = 25,
    DrawRenderModeButton_26 = 26,
    DrawRenderModeButton_27 = 27,
    HandleSideArea = 28,
    HandleSideArea_29 = 29,
    HandleSideArea_30 = 30,
    HandleSideArea_31 = 31,
    HandleSideArea_32 = 32,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00493A70。
static void WindowSettings_RefreshLogoutPanel(std::int32_t offset); // 设置窗口登出侧栏裁剪与两按钮显隐。

// 原函数：0x00493AF0。
static void WindowSettings_RefreshInputPanel(std::int32_t offset); // 设置窗口输入侧栏裁剪与三按钮显隐。

// 原函数：0x00493B70。
static void WindowSettings_RefreshAudioPanel(std::int32_t offset); // 设置窗口音量侧栏，两同源数字分别测宽。

// 原函数：0x00493CB0。
static void WindowSettings_RefreshDisplayPanel(std::int32_t offset); // 设置窗口显示侧栏，读取当前模式S8。

// 原函数：0x00493D60。
static std::int32_t WindowSettings_Update(std::int32_t value); // -1仅隐藏侧栏、清活动索引和位移。

// 原函数：0x00493DB0。
// 关联原入口：0x00493DF9（共享尾已并入本函数正文）。
static std::int32_t WindowSettings_Draw(std::int32_t unusedValue); // 四侧栏减速展开与公共绘制尾。

// 原函数：0x00493E10。
static void WindowSettings_Release(); // 设置写回/持久化后播放关闭音效。

// 原函数：0x00493EE0。
static std::int32_t WindowSettings_ButtonLogoutPanel(std::int32_t index, std::int32_t state); // 登出侧栏开关。

// 原函数：0x00494000。
static std::int32_t WindowSettings_ButtonInputPanel(std::int32_t index, std::int32_t state); // 输入侧栏，展开后保留元素11载荷别名。

// 原函数：0x00494180。
static std::int32_t WindowSettings_ButtonAudioPanel(std::int32_t index, std::int32_t state); // 音量侧栏，生成两个type7同源文本。

// 原函数：0x00494370。
static std::int32_t WindowSettings_ButtonDisplayPanel(std::int32_t index, std::int32_t state); // 显示侧栏，展开后保留元素21载荷别名。

// 原函数：0x00493E50。
static std::int32_t WindowSettings_ButtonClose(std::int32_t index, std::int32_t state); // 设置窗关闭标记，无直接音效。

// 原函数：0x004944A0。
static std::int32_t WindowSettings_ButtonHotkeys(std::int32_t index, std::int32_t state); // 快捷键窗口25开关。

// 原函数：0x00494550。
static std::int32_t WindowSettings_ButtonLogout(std::int32_t index, std::int32_t state); // lO请求与场景10状态7。

// 原函数：0x00494620。
static std::int32_t WindowSettings_ButtonServerSelect(std::int32_t index, std::int32_t state); // 返回服务器选择，非场景10构造CryptInt零值。

// 原函数：0x00494720。
static std::int32_t WindowSettings_ToggleItemStacking(std::int32_t index, std::int32_t state); // 物品堆叠输入设置。

// 原函数：0x004947F0。
static std::int32_t WindowSettings_ToggleCursor(std::int32_t index, std::int32_t state); // 光标切换，非零模式逐帧预览。

// 原函数：0x004948F0。
static std::int32_t WindowSettings_CycleHotkeyMode(std::int32_t index, std::int32_t state); // 快捷键模式，同源六项图号表。

// 原函数：0x004949C0。
static std::int32_t WindowSettings_ButtonMusicDecrease(std::int32_t index, std::int32_t state); // 音乐减量，bit400触发。

// 原函数：0x00494AE0。
static std::int32_t WindowSettings_ButtonMusicIncrease(std::int32_t index, std::int32_t state); // 音乐增量。

// 原函数：0x00494C10。
static std::int32_t WindowSettings_ButtonSoundDecrease(std::int32_t index, std::int32_t state); // 音效减量，仍调用BGM音量入口。

// 原函数：0x00494D30。
static std::int32_t WindowSettings_ButtonSoundIncrease(std::int32_t index, std::int32_t state); // 音效增量。

// 原函数：0x00494E60。
static std::int32_t WindowSettings_ToggleChannels(std::int32_t index, std::int32_t state); // 声道设置，保留W32奇偶切换。

// 原函数：0x00494F30。
static std::int32_t WindowSettings_ToggleAudioFlag(std::int32_t index, std::int32_t state); // 554644切换，提示仍复用声道文案。

// 原函数：0x00495000。
static std::int32_t WindowSettings_ToggleScanlines(std::int32_t index, std::int32_t state); // 低U8切换后写W16扫描线设置。

// 原函数：0x004950D0。
static std::int32_t WindowSettings_DrawRenderModeButton(std::int32_t index, std::int32_t state); // 当前渲染模式提示/图号，恒返0。

// 原函数：0x00493D60。
static std::int32_t WindowSettings_Update(std::int32_t value);

// 原函数：0x00493DB0。
// 关联原入口：0x00493DF9（共享尾已并入本函数正文）。
static std::int32_t WindowSettings_Draw(std::int32_t value);

// 原函数：0x00493E10。
static void WindowSettings_Release();

// 控件载荷、窗口状态、组件表与原型。

static std::int8_t gSettingsPanelCC4234 = 0; // 原全局：0x00CC4234。

static std::int32_t gSettingsOffsetCA02DC = 0; // 原全局：0x00CA02DC。

static std::int32_t gSettingsStepD2D610 = 0; // 原全局：0x00D2D610。

// 原全局：0x00D3041C。 5689C0借用，音乐音量S32的%d文本。
static std::wstring gSettingsMusicTextD3041C {}; // 原全局：0x00D3041C。

// 原全局：0x00D2EA64。 5689B8借用，音效音量S32的%d文本。
static std::wstring gSettingsEffectTextD2EA64 {}; // 原全局：0x00D2EA64。

static constexpr std::array<WindowValueCallback, 4> kSettingsPanelCallbacks57DF38 { // 原常量参考：0x0057DF38。
    &WindowSettings_RefreshLogoutPanel, &WindowSettings_RefreshInputPanel, &WindowSettings_RefreshAudioPanel, &WindowSettings_RefreshDisplayPanel,
};

static constexpr std::array<std::int32_t, 6> kSettingsKeyImages54F460 { // 原常量参考：0x0054F460。
    0x3BE63, 0x3BE64, 0x3BE65, 0x3BE66, 0x3BE67, 0x3BE68,
};

static WindowGraphicData gWindowGraphic568720 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568720。

static WindowGraphicData gWindowGraphic568738 {0x3B8BE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568738。

static WindowGraphicData gWindowGraphic568750 {0x3B8C1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568750。

static WindowGraphicData gWindowGraphic568768 {0x3B8C4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568768。

static WindowGraphicData gWindowGraphic568780 {0x3B8C7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568780。

static WindowGraphicData gWindowGraphic568798 {0x3B8CA, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568798。

static WindowGraphicData gWindowGraphic5687B0 {0x3B8D0, 0, 0, 0, 79, 0xFFFFFFFFu}; // 原全局：0x005687B0。

static WindowGraphicData gWindowGraphic5687C8 {0x3B8DA, 0, 0, 0, 128, 0xFFFFFFFFu}; // 原全局：0x005687C8。

static WindowGraphicData gWindowGraphic5687E0 {0x3B8EE, 0, 0, 0, 203, 0xFFFFFFFFu}; // 原全局：0x005687E0。

static WindowGraphicData gWindowGraphic5687F8 {0x3B8F8, 0, 0, 0, 90, 0xFFFFFFFFu}; // 原全局：0x005687F8。

static WindowGraphicData gWindowGraphic568810 {0x3B8D1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568810。

static WindowGraphicData gWindowGraphic568828 {0x3B8D4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568828。

static WindowGraphicData gWindowGraphic568840 {0x3B8DB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568840。

static WindowGraphicData gWindowGraphic568858 {0x3B8E1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568858。

static WindowGraphicData gWindowGraphic568870 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568870。

static WindowGraphicData gWindowGraphic568888 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568888。

static WindowGraphicData gWindowGraphic5688A0 {0x3B8EF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005688A0。

static WindowGraphicData gWindowGraphic5688B8 {0x3B8F9, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005688B8。

static WindowGraphicData gWindowGraphic5688D0 {0x3B8BC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005688D0。

static WindowGraphicData gWindowGraphic5688E8 {0x3B8BD, 0, 0, 0, 0, 0x99FFFFFFu}; // 原全局：0x005688E8。

static WindowGraphicData gWindowGraphic568900 {0x3BE63, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568900。

static WindowGraphicData gWindowGraphic568918 {0x3BE78, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568918。

static WindowGraphicData gWindowGraphic568930 {0x3B5E2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568930。

static WindowGraphicData gWindowGraphic568948 {0x3B5E5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568948。

static WindowGraphicData gWindowGraphic568960 {0x3B5E8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568960。

static WindowGraphicData gWindowGraphic568978 {0x3B5EB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568978。

static WindowGraphicData gWindowGraphic568990 {0x3B5EE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568990。

static WindowNumberData gWindowNumber5689B8 {&gSettingsEffectTextD2EA64, 0, 3, 0, 0}; // 原全局：0x005689B8。

static WindowNumberData gWindowNumber5689C0 {&gSettingsMusicTextD3041C, 0, 3, 0, 0}; // 原全局：0x005689C0。

static WindowElementPrototype gWindowElements5689C8[32] { // 原全局：0x005689C8。
    {WindowElementType_Graphic, 186, 9, 11, 11, 1, {.graphic = &gWindowGraphic568720}, &WindowSettings_ButtonClose, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonClose)},
    {WindowElementType_Graphic, 69, 51, 70, 17, 1, {.graphic = &gWindowGraphic568738}, &WindowSettings_ButtonLogoutPanel, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonLogoutPanel)},
    {WindowElementType_Graphic, 69, 79, 70, 17, 1, {.graphic = &gWindowGraphic568750}, &WindowSettings_ButtonInputPanel, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonInputPanel)},
    {WindowElementType_Graphic, 69, 109, 70, 17, 1, {.graphic = &gWindowGraphic568768}, &WindowSettings_ButtonAudioPanel, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonAudioPanel)},
    {WindowElementType_Graphic, 69, 142, 70, 17, 1, {.graphic = &gWindowGraphic568780}, &WindowSettings_ButtonDisplayPanel, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonDisplayPanel)},
    {WindowElementType_Graphic, 69, 174, 70, 17, 1, {.graphic = &gWindowGraphic568798}, &WindowSettings_ButtonHotkeys, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonHotkeys)},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic5688D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSettingsElementId::Element7)},
    {WindowElementType_Graphic, 13, 26, 40, 11, 1, {.graphic = &gWindowGraphic5688E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSettingsElementId::Element8)},
    {WindowElementType_Graphic, -111, 30, 102, 18, 1, {.graphic = &gWindowGraphic568810}, &WindowSettings_ButtonLogout, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonLogout)},
    {WindowElementType_Graphic, -111, 58, 102, 18, 1, {.graphic = &gWindowGraphic568828}, &WindowSettings_ButtonServerSelect, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonServerSelect)},
    {WindowElementType_Graphic, -117, 40, 88, 18, 1, {.graphic = &gWindowGraphic568840}, &WindowSettings_ToggleItemStacking, -1, static_cast<ElementId>(WindowSettingsElementId::ToggleItemStacking)},
    {WindowElementType_Graphic, -117, 75, 88, 18, 1, {.graphic = &gWindowGraphic568858}, &WindowSettings_ToggleCursor, -1, static_cast<ElementId>(WindowSettingsElementId::ToggleCursor)},
    {WindowElementType_Graphic, -117, 110, 88, 18, 1, {.graphic = &gWindowGraphic568900}, &WindowSettings_CycleHotkeyMode, -1, static_cast<ElementId>(WindowSettingsElementId::CycleHotkeyMode)},
    {WindowElementType_Number, -72, 51, 0, 0, 1, {.number = &gWindowNumber5689C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSettingsElementId::Element14)},
    {WindowElementType_Graphic, -98, 47, 18, 18, 1, {.graphic = &gWindowGraphic568870}, &WindowSettings_ButtonMusicDecrease, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonMusicDecrease)},
    {WindowElementType_Graphic, -35, 47, 18, 18, 1, {.graphic = &gWindowGraphic568888}, &WindowSettings_ButtonMusicIncrease, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonMusicIncrease)},
    {WindowElementType_Number, -72, 95, 0, 0, 1, {.number = &gWindowNumber5689B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSettingsElementId::Element17)},
    {WindowElementType_Graphic, -98, 91, 18, 18, 1, {.graphic = &gWindowGraphic568870}, &WindowSettings_ButtonSoundDecrease, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonSoundDecrease)},
    {WindowElementType_Graphic, -35, 91, 18, 18, 1, {.graphic = &gWindowGraphic568888}, &WindowSettings_ButtonSoundIncrease, -1, static_cast<ElementId>(WindowSettingsElementId::ButtonSoundIncrease)},
    {WindowElementType_Graphic, -88, 141, 63, 18, 1, {.graphic = &gWindowGraphic5688A0}, &WindowSettings_ToggleChannels, -1, static_cast<ElementId>(WindowSettingsElementId::ToggleChannels)},
    {WindowElementType_Graphic, -88, 185, 63, 18, 1, {.graphic = &gWindowGraphic568918}, &WindowSettings_ToggleAudioFlag, -1, static_cast<ElementId>(WindowSettingsElementId::ToggleAudioFlag)},
    {WindowElementType_Graphic, -45, 25, 44, 18, 1, {.graphic = &gWindowGraphic5688B8}, &WindowSettings_ToggleScanlines, -1, static_cast<ElementId>(WindowSettingsElementId::ToggleScanlines)},
    {WindowElementType_Graphic, 0, 72, 22, 19, 0, {.graphic = &gWindowGraphic568930}, &WindowSettings_DrawRenderModeButton, -1, static_cast<ElementId>(WindowSettingsElementId::DrawRenderModeButton)},
    {WindowElementType_Graphic, -120, 72, 22, 19, 1, {.graphic = &gWindowGraphic568948}, &WindowSettings_DrawRenderModeButton, -1, static_cast<ElementId>(WindowSettingsElementId::DrawRenderModeButton_24)},
    {WindowElementType_Graphic, -89, 72, 22, 19, 1, {.graphic = &gWindowGraphic568960}, &WindowSettings_DrawRenderModeButton, -1, static_cast<ElementId>(WindowSettingsElementId::DrawRenderModeButton_25)},
    {WindowElementType_Graphic, -58, 72, 22, 19, 1, {.graphic = &gWindowGraphic568978}, &WindowSettings_DrawRenderModeButton, -1, static_cast<ElementId>(WindowSettingsElementId::DrawRenderModeButton_26)},
    {WindowElementType_Graphic, -27, 72, 22, 19, 1, {.graphic = &gWindowGraphic568990}, &WindowSettings_DrawRenderModeButton, -1, static_cast<ElementId>(WindowSettingsElementId::DrawRenderModeButton_27)},
    {WindowElementType_Graphic, -130, 10, 150, 79, 1, {.graphic = &gWindowGraphic5687B0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowSettingsElementId::HandleSideArea)},
    {WindowElementType_Graphic, -130, 10, 150, 128, 1, {.graphic = &gWindowGraphic5687C8}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowSettingsElementId::HandleSideArea_29)},
    {WindowElementType_Graphic, -130, 10, 150, 203, 1, {.graphic = &gWindowGraphic5687E0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowSettingsElementId::HandleSideArea_30)},
    {WindowElementType_Graphic, -130, 10, 150, 90, 1, {.graphic = &gWindowGraphic5687F8}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowSettingsElementId::HandleSideArea_31)},
    {WindowElementType_EventArea, 209, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowSettingsElementId::HandleSideArea_32)},
};

// 原型：窗口24；原全局：0x0054E660。
WindowPrototype gWindowPrototypeSettings {0, 4, 409, 100, 207, 222, 0x80010101u, 32, gWindowElements5689C8, &WindowSettings_Update, &WindowSettings_Draw, &WindowSettings_Release}; // 24: 54E660

// 窗口函数实现。

// 原函数：0x00493A70。
static void WindowSettings_RefreshLogoutPanel(const std::int32_t offset)
{
    WindowInstance* const window = gWindowStates[WindowId_Settings].window;
    if (auto* const handleSideAreaElement = window ? window->FindElementById(WindowSettingsElementId::HandleSideArea) : nullptr; handleSideAreaElement) handleSideAreaElement->x = offset + 16;
    auto* elementsWindow = window;
    auto* const handleSideAreaElement2 = elementsWindow ? elementsWindow->FindElementById(WindowSettingsElementId::HandleSideArea) : nullptr;
    if (handleSideAreaElement2) {
        const std::int32_t x = handleSideAreaElement2->x;
        auto* const handleSideAreaElement3 = elementsWindow ? elementsWindow->FindElementById(WindowSettingsElementId::HandleSideArea) : nullptr;
        if (handleSideAreaElement3) {
            auto* const graphic = handleSideAreaElement3->data.graphic;
            const std::int32_t width = 16 - x;
            graphic->width = width;
            auto* const handleSideAreaElement4 = window ? window->FindElementById(WindowSettingsElementId::HandleSideArea) : nullptr;
            if (handleSideAreaElement4) {
                const std::int32_t limit = handleSideAreaElement4->width;
                if (width > limit) {
                    graphic->width = limit;
                }
            }
        }
    }
    const std::int32_t visible = offset <= -146 ? 1 : 0;
    if (auto* const buttonLogoutElement = window ? window->FindElementById(WindowSettingsElementId::ButtonLogout) : nullptr; buttonLogoutElement) buttonLogoutElement->visible = visible;
    if (auto* const buttonServerSelectElement = window ? window->FindElementById(WindowSettingsElementId::ButtonServerSelect) : nullptr; buttonServerSelectElement) buttonServerSelectElement->visible = visible;
}

// 原函数：0x00493AF0。
static void WindowSettings_RefreshInputPanel(const std::int32_t offset)
{
    WindowInstance* const window = gWindowStates[WindowId_Settings].window;
    if (auto* const handleSideArea_29Element = window ? window->FindElementById(WindowSettingsElementId::HandleSideArea_29) : nullptr; handleSideArea_29Element) handleSideArea_29Element->x = offset + 16;
    auto* elementsWindow = window;
    auto* const handleSideArea_29Element2 = elementsWindow ? elementsWindow->FindElementById(WindowSettingsElementId::HandleSideArea_29) : nullptr;
    if (handleSideArea_29Element2) {
        const std::int32_t x = handleSideArea_29Element2->x;
        auto* const handleSideArea_29Element3 = elementsWindow ? elementsWindow->FindElementById(WindowSettingsElementId::HandleSideArea_29) : nullptr;
        if (handleSideArea_29Element3) {
            auto* const graphic = handleSideArea_29Element3->data.graphic;
            const std::int32_t width = 16 - x;
            graphic->width = width;
            auto* const handleSideArea_29Element4 = window ? window->FindElementById(WindowSettingsElementId::HandleSideArea_29) : nullptr;
            if (handleSideArea_29Element4) {
                const std::int32_t limit = handleSideArea_29Element4->width;
                if (width > limit) {
                    graphic->width = limit;
                }
            }
        }
    }
    const std::int32_t visible = offset <= -146 ? 1 : 0;
    if (auto* const toggleItemStackingElement = window ? window->FindElementById(WindowSettingsElementId::ToggleItemStacking) : nullptr; toggleItemStackingElement) toggleItemStackingElement->visible = visible;
    if (auto* const toggleCursorElement = window ? window->FindElementById(WindowSettingsElementId::ToggleCursor) : nullptr; toggleCursorElement) toggleCursorElement->visible = visible;
    if (auto* const cycleHotkeyModeElement = window ? window->FindElementById(WindowSettingsElementId::CycleHotkeyMode) : nullptr; cycleHotkeyModeElement) cycleHotkeyModeElement->visible = visible;
}

// 原函数：0x00493B70。
static void WindowSettings_RefreshAudioPanel(const std::int32_t offset)
{
    WindowInstance* const window = gWindowStates[WindowId_Settings].window;
    if (auto* const handleSideArea_30Element = window ? window->FindElementById(WindowSettingsElementId::HandleSideArea_30) : nullptr; handleSideArea_30Element) handleSideArea_30Element->x = offset + 16;
    auto* elementsWindow = window;
    auto* const handleSideArea_30Element2 = elementsWindow ? elementsWindow->FindElementById(WindowSettingsElementId::HandleSideArea_30) : nullptr;
    if (handleSideArea_30Element2) {
        const std::int32_t x = handleSideArea_30Element2->x;
        auto* const handleSideArea_30Element3 = elementsWindow ? elementsWindow->FindElementById(WindowSettingsElementId::HandleSideArea_30) : nullptr;
        if (handleSideArea_30Element3) {
            auto* const graphic = handleSideArea_30Element3->data.graphic;
            const std::int32_t width = 16 - x;
            graphic->width = width;
            auto* const handleSideArea_30Element4 = window ? window->FindElementById(WindowSettingsElementId::HandleSideArea_30) : nullptr;
            if (handleSideArea_30Element4) {
                const std::int32_t limit = handleSideArea_30Element4->width;
                if (width > limit) {
                    graphic->width = limit;
                }
            }
        }
    }
    if (offset > -146) {
        for (std::int32_t index = 13; index <= 20; ++index) {
            if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
        }
        return;
    }
    if (auto* const element14 = window ? window->FindElementById(WindowSettingsElementId::Element14) : nullptr; element14) element14->visible = 1;
    const std::int32_t musicWidth = WindowNumber_MeasureWidth(gSettingsMusicTextD3041C, 3);
    if (auto* const element142 = window ? window->FindElementById(WindowSettingsElementId::Element14) : nullptr; element142) element142->x = ((37 - musicWidth) >> 1) + offset + 69;
    if (auto* const buttonMusicDecreaseElement = window ? window->FindElementById(WindowSettingsElementId::ButtonMusicDecrease) : nullptr; buttonMusicDecreaseElement) buttonMusicDecreaseElement->visible = 1;
    if (auto* const buttonMusicIncreaseElement = window ? window->FindElementById(WindowSettingsElementId::ButtonMusicIncrease) : nullptr; buttonMusicIncreaseElement) buttonMusicIncreaseElement->visible = 1;
    if (auto* const element17 = window ? window->FindElementById(WindowSettingsElementId::Element17) : nullptr; element17) element17->visible = 1;
    const std::int32_t effectWidth = WindowNumber_MeasureWidth(gSettingsEffectTextD2EA64, 3);
    if (auto* const element172 = window ? window->FindElementById(WindowSettingsElementId::Element17) : nullptr; element172) element172->x = ((37 - effectWidth) >> 1) + offset + 69;
    if (auto* const buttonSoundDecreaseElement = window ? window->FindElementById(WindowSettingsElementId::ButtonSoundDecrease) : nullptr; buttonSoundDecreaseElement) buttonSoundDecreaseElement->visible = 1;
    if (auto* const buttonSoundIncreaseElement = window ? window->FindElementById(WindowSettingsElementId::ButtonSoundIncrease) : nullptr; buttonSoundIncreaseElement) buttonSoundIncreaseElement->visible = 1;
    if (auto* const toggleChannelsElement = window ? window->FindElementById(WindowSettingsElementId::ToggleChannels) : nullptr; toggleChannelsElement) toggleChannelsElement->visible = 1;
    if (auto* const toggleAudioFlagElement = window ? window->FindElementById(WindowSettingsElementId::ToggleAudioFlag) : nullptr; toggleAudioFlagElement) toggleAudioFlagElement->visible = 1;
}

// 原函数：0x00493CB0。
static void WindowSettings_RefreshDisplayPanel(const std::int32_t offset)
{
    WindowInstance* const window = gWindowStates[WindowId_Settings].window;
    if (auto* const handleSideArea_31Element = window ? window->FindElementById(WindowSettingsElementId::HandleSideArea_31) : nullptr; handleSideArea_31Element) handleSideArea_31Element->x = offset + 16;
    auto* elementsWindow = window;
    auto* const handleSideArea_31Element2 = elementsWindow ? elementsWindow->FindElementById(WindowSettingsElementId::HandleSideArea_31) : nullptr;
    if (handleSideArea_31Element2) {
        const std::int32_t x = handleSideArea_31Element2->x;
        auto* const handleSideArea_31Element3 = elementsWindow ? elementsWindow->FindElementById(WindowSettingsElementId::HandleSideArea_31) : nullptr;
        if (handleSideArea_31Element3) {
            auto* const graphic = handleSideArea_31Element3->data.graphic;
            const std::int32_t width = 16 - x;
            graphic->width = width;
            auto* const handleSideArea_31Element4 = window ? window->FindElementById(WindowSettingsElementId::HandleSideArea_31) : nullptr;
            if (handleSideArea_31Element4) {
                const std::int32_t limit = handleSideArea_31Element4->width;
                if (width > limit) {
                    graphic->width = limit;
                }
            }
        }
    }
    if (auto* const toggleScanlinesElement = window ? window->FindElementById(WindowSettingsElementId::ToggleScanlines) : nullptr; toggleScanlinesElement) toggleScanlinesElement->visible = 0;
    if (offset <= -146) {
        if (auto* const toggleScanlinesElement2 = window ? window->FindElementById(WindowSettingsElementId::ToggleScanlines) : nullptr; toggleScanlinesElement2) toggleScanlinesElement2->visible = 1;
        for (std::int32_t index = 22; index <= 26; ++index) {
            if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
        }
        const std::int8_t mode = std::bit_cast<std::int8_t>(platform::gSupportedRenderMode);
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(mode + 22)) : nullptr; indexedElement) indexedElement->visible = 1;
    }
}

// 原函数：0x00493D60。
static std::int32_t WindowSettings_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    if (value == -1) {
        for (std::int32_t index = 8; index <= 30; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        gSettingsPanelCC4234 = -1;
        gSettingsOffsetCA02DC = 0;
    }
    return 1;
}

// 原函数：0x00493DB0。
// 关联原入口：0x00493DF9（共享尾已并入本函数正文）。
static std::int32_t WindowSettings_Draw(const std::int32_t)
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
static void WindowSettings_Release()
{
    static_cast<void>(login::detail::CaptureAudioPreferences_520470());
    static_cast<void>(login::detail::CaptureClientPreferenceBytes_520590());
    const std::int32_t selected = login::gSelectedCharacterIndex;
    static_cast<void>(login::detail::CaptureCharacterDisplaySettings_520510(selected));
    static_cast<void>(login::detail::CapturePresetChatTexts_520640());
    static_cast<void>(login::detail::CaptureContactSortMode_5206F0());
    static_cast<void>(login::detail::SaveCurrentLoginRecord_520200());
    static_cast<void>(audio::PlaySe_52C450(57, 320));
}

// 原函数：0x00493EE0。
static std::int32_t WindowSettings_ButtonLogoutPanel(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if (graphic) graphic->imageNo = 0x3B8BE;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B8C0;
        gHoverTextD68AE8 = L"登出伺服器。" /* 原常量参考：0x00586DE8。 */;
    }
    if ((state & 2) != 0) {
        auto* const handleSideAreaElement = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea);
        if (handleSideAreaElement) {
            const std::int32_t visible = handleSideAreaElement->visible;
            for (std::int32_t element = 8; element <= 30; ++element) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
            }
            gSettingsPanelCC4234 = -1;
            gSettingsOffsetCA02DC = 0;
            if (visible == 0) {
                gSettingsPanelCC4234 = 0;
                gSettingsStepD2D610 = 17;
                if (auto* const buttonLogoutElement = currentWindow->FindElementById(WindowSettingsElementId::ButtonLogout); buttonLogoutElement) buttonLogoutElement->visible = 1;
                if (auto* const buttonServerSelectElement = currentWindow->FindElementById(WindowSettingsElementId::ButtonServerSelect); buttonServerSelectElement) buttonServerSelectElement->visible = 1;
                if (auto* const handleSideAreaElement2 = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea); handleSideAreaElement2) handleSideAreaElement2->visible = 1;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    }
    auto* const handleSideAreaElement = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea);
    if (handleSideAreaElement) {
        if (handleSideAreaElement->visible != 0) {
            if (graphic) graphic->imageNo = 0x3B8BF;
        }
    }
    return consumed;
}

// 原函数：0x00494000。
static std::int32_t WindowSettings_ButtonInputPanel(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if (graphic) graphic->imageNo = 0x3B8C1;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B8C3;
        gHoverTextD68AE8 = L"设定滑鼠与键盘的操作方式。" /* 原常量参考：0x00586DF8。 */;
    }
    if ((state & 2) != 0) {
        auto* const handleSideArea_29Element = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_29);
        if (handleSideArea_29Element) {
            const std::int32_t visible = handleSideArea_29Element->visible;
            for (std::int32_t element = 8; element <= 30; ++element) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
            }
            gSettingsPanelCC4234 = -1;
            gSettingsOffsetCA02DC = 0;
            if (visible == 0) {
                gSettingsPanelCC4234 = 1;
                gSettingsStepD2D610 = 17;
                if (auto* const toggleItemStackingElement = currentWindow->FindElementById(WindowSettingsElementId::ToggleItemStacking); toggleItemStackingElement) toggleItemStackingElement->visible = 1;
                if (auto* const toggleCursorElement = currentWindow->FindElementById(WindowSettingsElementId::ToggleCursor); toggleCursorElement) toggleCursorElement->visible = 1;
                if (auto* const cycleHotkeyModeElement = currentWindow->FindElementById(WindowSettingsElementId::CycleHotkeyMode); cycleHotkeyModeElement) cycleHotkeyModeElement->visible = 1;
                if (auto* const handleSideArea_29Element2 = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_29); handleSideArea_29Element2) handleSideArea_29Element2->visible = 1;
                auto* const toggleItemStackingElement2 = currentWindow->FindElementById(WindowSettingsElementId::ToggleItemStacking);
                if (toggleItemStackingElement2) {
                    auto* const inputGraphic = toggleItemStackingElement2->data.graphic;
                    inputGraphic->imageNo = 0x3B8DE;
                    if (platform::gInvertItemSplitModifier_D67AE8 != 0) {
                        inputGraphic->imageNo = 0x3B8DB;
                    }
                }
                // 原494121改写EBP；末尾选中图也写这个借用载荷。
                auto* const toggleCursorElement2 = currentWindow->FindElementById(WindowSettingsElementId::ToggleCursor);
                graphic = toggleCursorElement2 ? toggleCursorElement2->data.graphic : nullptr;
                if (graphic) graphic->imageNo = 0x3B8E4;
                if (platform::gNativeCursorEnabled_D67AE4 != 0) {
                    if (graphic) graphic->imageNo = 0x3B8E1;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    }
    auto* const handleSideArea_29Element = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_29);
    if (handleSideArea_29Element) {
        if (handleSideArea_29Element->visible != 0) {
            if (graphic) graphic->imageNo = 0x3B8C2;
        }
    }
    return consumed;
}

// 原函数：0x00494180。
static std::int32_t WindowSettings_ButtonAudioPanel(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if (graphic) graphic->imageNo = 0x3B8C4;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B8C6;
        gHoverTextD68AE8 = L"设定音乐音效。" /* 原常量参考：0x00586E14。 */;
    }
    if ((state & 2) != 0) {
        auto* const handleSideArea_30Element = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_30);
        if (handleSideArea_30Element) {
            const std::int32_t visible = handleSideArea_30Element->visible;
            for (std::int32_t element = 8; element <= 30; ++element) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
            }
            gSettingsPanelCC4234 = -1;
            gSettingsOffsetCA02DC = 0;
            if (visible == 0) {
                gSettingsPanelCC4234 = 2;
                gSettingsStepD2D610 = 17;
                if (auto* const toggleChannelsElement = currentWindow->FindElementById(WindowSettingsElementId::ToggleChannels); toggleChannelsElement) toggleChannelsElement->visible = 1;
                if (auto* const element14 = currentWindow->FindElementById(WindowSettingsElementId::Element14); element14) element14->visible = 1;
                if (auto* const buttonMusicDecreaseElement = currentWindow->FindElementById(WindowSettingsElementId::ButtonMusicDecrease); buttonMusicDecreaseElement) buttonMusicDecreaseElement->visible = 1;
                if (auto* const buttonMusicIncreaseElement = currentWindow->FindElementById(WindowSettingsElementId::ButtonMusicIncrease); buttonMusicIncreaseElement) buttonMusicIncreaseElement->visible = 1;
                if (auto* const element17 = currentWindow->FindElementById(WindowSettingsElementId::Element17); element17) element17->visible = 1;
                if (auto* const buttonSoundDecreaseElement = currentWindow->FindElementById(WindowSettingsElementId::ButtonSoundDecrease); buttonSoundDecreaseElement) buttonSoundDecreaseElement->visible = 1;
                if (auto* const buttonSoundIncreaseElement = currentWindow->FindElementById(WindowSettingsElementId::ButtonSoundIncrease); buttonSoundIncreaseElement) buttonSoundIncreaseElement->visible = 1;
                if (auto* const handleSideArea_30Element2 = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_30); handleSideArea_30Element2) handleSideArea_30Element2->visible = 1;
                cg::text::FormatTo(gSettingsMusicTextD3041C, L"{}", platform::gMusicVolume);
                const std::int32_t musicWidth = WindowNumber_MeasureWidth(gSettingsMusicTextD3041C, 3);
                auto* const element142 = currentWindow->FindElementById(WindowSettingsElementId::Element14);
                if (element142) {
                    element142->x =
                        ((37 - musicWidth) >> 1) + gSettingsOffsetCA02DC + 69;
                }
                cg::text::FormatTo(gSettingsEffectTextD2EA64, L"{}", platform::gEffectVolume);
                const std::int32_t effectWidth = WindowNumber_MeasureWidth(gSettingsEffectTextD2EA64, 3);
                auto* const element172 = currentWindow->FindElementById(WindowSettingsElementId::Element17);
                if (element172) {
                    element172->x =
                        ((37 - effectWidth) >> 1) + gSettingsOffsetCA02DC + 69;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    }
    auto* const handleSideArea_30Element = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_30);
    if (handleSideArea_30Element) {
        if (handleSideArea_30Element->visible != 0) {
            if (graphic) graphic->imageNo = 0x3B8C5;
        }
    }
    return consumed;
}

// 原函数：0x00494370。
static std::int32_t WindowSettings_ButtonDisplayPanel(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    auto* graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if (graphic) graphic->imageNo = 0x3B8C7;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B8C9;
        gHoverTextD68AE8 = L"扫描线同期设定" /* 原常量参考：0x00586E24。 */;
    }
    if ((state & 2) != 0) {
        auto* const handleSideArea_31Element = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_31);
        if (handleSideArea_31Element) {
            const std::int32_t visible = handleSideArea_31Element->visible;
            for (std::int32_t element = 8; element <= 30; ++element) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
            }
            gSettingsPanelCC4234 = -1;
            gSettingsOffsetCA02DC = 0;
            if (visible == 0) {
                gSettingsPanelCC4234 = 3;
                gSettingsStepD2D610 = 17;
                if (auto* const toggleScanlinesElement = currentWindow->FindElementById(WindowSettingsElementId::ToggleScanlines); toggleScanlinesElement) toggleScanlinesElement->visible = 1;
                if (auto* const handleSideArea_31Element2 = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_31); handleSideArea_31Element2) handleSideArea_31Element2->visible = 1;
                // 原49444C改写EBP，后续选中图仍写元素21。
                auto* const toggleScanlinesElement2 = currentWindow->FindElementById(WindowSettingsElementId::ToggleScanlines);
                graphic = toggleScanlinesElement2 ? toggleScanlinesElement2->data.graphic : nullptr;
                if (graphic) graphic->imageNo = 0x3B8F9;
                if (platform::gScanlineSyncMode != 0) {
                    if (graphic) graphic->imageNo = 0x3B8FC;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    }
    auto* const handleSideArea_31Element = currentWindow->FindElementById(WindowSettingsElementId::HandleSideArea_31);
    if (handleSideArea_31Element) {
        if (handleSideArea_31Element->visible != 0) {
            if (graphic) graphic->imageNo = 0x3B8C8;
        }
    }
    return consumed;
}

// 原函数：0x00493E50。
static std::int32_t WindowSettings_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
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
    return {};
}

// 原函数：0x004944A0。
static std::int32_t WindowSettings_ButtonHotkeys(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        graphic->imageNo = 0x3B8CA;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8CC;
            gHoverTextD68AE8 = L"开启热键一览表。" /* 原常量参考：0x00586E34。 */;
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
    return {};
}

// 原函数：0x00494550。
static std::int32_t WindowSettings_ButtonLogout(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"登出并回到城内登入点。" /* 原常量参考：0x00586E48。 */;
            window = currentWindow;
        }
        if ((state & 2) != 0) {
            window->state |= 0x2000u;
            static_cast<void>(network::RequestLogoutToTown_512CF0(platform::gConnectionSocket));
            if (world::gCurrentSceneType == 10) {
                world::gCurrentSceneState = 7;
            }
            static_cast<void>(audio::PlaySe_52C450(60, 320));
            consumed = 1;
            world::gLogoutToTownPending_D67E78 = 1;
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
    return {};
}

// 原函数：0x00494620。
static std::int32_t WindowSettings_ButtonServerSelect(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B8D6;
            gHoverTextD68AE8 = L"登出后回到伺服器选择画面。" /* 原常量参考：0x00586E60。 */;
            window = currentWindow;
        }
        if ((state & 2) != 0) {
            window->state |= 0x2000u;
            if (world::gCurrentSceneType == 10) {
                world::gCurrentSceneState = 9;
            } else {
                world::gSceneTransitionValue = std::int32_t{0};
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
    return {};
}

// 原函数：0x00494720。
static std::int32_t WindowSettings_ToggleItemStacking(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"变更物品堆叠视窗的开启方式。" /* 原常量参考：0x00586E7C。 */;
        }
        if ((state & 2) != 0) {
            window->state |= 0x2000u;
            platform::gInvertItemSplitModifier_D67AE8 = (platform::gInvertItemSplitModifier_D67AE8 - 1u) & 1u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if (platform::gInvertItemSplitModifier_D67AE8 != 0) {
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
    return {};
}

// 原函数：0x004947F0。
static std::int32_t WindowSettings_ToggleCursor(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"变更滑鼠游标种类。" /* 原常量参考：0x00586E9C。 */;
        }
        if ((state & 2) != 0) {
            window->state |= 0x2000u;
            platform::gNativeCursorEnabled_D67AE4 = platform::gNativeCursorEnabled_D67AE4 == 0 ? 1u : 0u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if (platform::gNativeCursorEnabled_D67AE4 != 0) {
            graphic->imageNo = 0x3B8E1;
            if ((state & 1) != 0) {
                graphic->imageNo = 0x3B8E3;
            }
            const std::int32_t y = (platform::gLegacyMouseState.mouseY + 16);
            const std::int32_t x = (platform::gLegacyMouseState.mouseX + 16);
            static_cast<void>(cg::render::DrawGraphic(x, y, 231, 22000, 0, nullptr));
        } else {
            graphic->imageNo = 0x3B8E4;
            if ((state & 1) != 0) {
                graphic->imageNo = 0x3B8E6;
            }
        }
        return consumed;
    }
    return {};
}

// 原函数：0x004948F0。
static std::int32_t WindowSettings_CycleHotkeyMode(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"变更快速键的型态。" /* 原常量参考：0x00586EB0。 */;
        }
        if ((state & 2) != 0) {
            window->state |= 0x2000u;
            const std::int32_t mode = gHotkeyLayoutMode_CF4AAC + 1;
            gHotkeyLayoutMode_CF4AAC = mode;
            if (mode == 2) {
                gHotkeyLayoutMode_CF4AAC = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        graphic->imageNo = kSettingsKeyImages54F460[3 * gHotkeyLayoutMode_CF4AAC];
        if ((state & 1) != 0) {
            graphic->imageNo = kSettingsKeyImages54F460[3 * gHotkeyLayoutMode_CF4AAC + 2];
        }
        return consumed;
    }
    return {};
}

// 原函数：0x004949C0。
static std::int32_t WindowSettings_ButtonMusicDecrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = platform::gMusicVolume <= 0
            ? L"无法再降低背景音乐的音量。" /* 原常量参考：0x00586EC4。 */ : L"降低背景音乐的音量。" /* 原常量参考：0x00586EE0。 */;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        window->state |= 0x2000u;
        const std::int32_t volume = platform::gMusicVolume;
        if (volume <= 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            platform::gMusicVolume = volume - 1;
            static_cast<void>(audio::ApplyBgmVolume_52C3D0());
            cg::text::FormatTo(gSettingsMusicTextD3041C, L"{}", platform::gMusicVolume);
            const std::int32_t width = WindowNumber_MeasureWidth(gSettingsMusicTextD3041C, 3);
            const std::int32_t x = ((37 - width) >> 1) + gSettingsOffsetCA02DC + 69;
            if (auto* const element14 = currentWindow->FindElementById(WindowSettingsElementId::Element14); element14) element14->x = x;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B558;
    }
    if ((state & 8) != 0) {
        if (graphic) graphic->imageNo = 0x3B557;
    }
    return consumed;
}

// 原函数：0x00494AE0。
static std::int32_t WindowSettings_ButtonMusicIncrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = platform::gMusicVolume >= 15
            ? L"无法再提高背景音乐的音量。" /* 原常量参考：0x00586EF8。 */ : L"提高背景音乐的音量。" /* 原常量参考：0x00586F14。 */;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        window->state |= 0x2000u;
        const std::int32_t volume = platform::gMusicVolume;
        if (volume >= 15) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            platform::gMusicVolume = volume + 1;
            static_cast<void>(audio::ApplyBgmVolume_52C3D0());
            cg::text::FormatTo(gSettingsMusicTextD3041C, L"{}", platform::gMusicVolume);
            const std::int32_t width = WindowNumber_MeasureWidth(gSettingsMusicTextD3041C, 3);
            const std::int32_t x = ((37 - width) >> 1) + gSettingsOffsetCA02DC + 69;
            if (auto* const element14 = currentWindow->FindElementById(WindowSettingsElementId::Element14); element14) element14->x = x;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B55B;
    }
    if ((state & 8) != 0) {
        if (graphic) graphic->imageNo = 0x3B55A;
    }
    return consumed;
}

// 原函数：0x00494C10。
static std::int32_t WindowSettings_ButtonSoundDecrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = platform::gEffectVolume <= 0
            ? L"无法再降低效果音量。" /* 原常量参考：0x00586F2C。 */ : L"降低效果音量。" /* 原常量参考：0x00586F44。 */;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        window->state |= 0x2000u;
        const std::int32_t volume = platform::gEffectVolume;
        if (volume <= 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            platform::gEffectVolume = volume - 1;
            static_cast<void>(audio::ApplyBgmVolume_52C3D0());
            cg::text::FormatTo(gSettingsEffectTextD2EA64, L"{}", platform::gEffectVolume);
            const std::int32_t width = WindowNumber_MeasureWidth(gSettingsEffectTextD2EA64, 3);
            const std::int32_t x = ((37 - width) >> 1) + gSettingsOffsetCA02DC + 69;
            if (auto* const element17 = currentWindow->FindElementById(WindowSettingsElementId::Element17); element17) element17->x = x;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B558;
    }
    if ((state & 8) != 0) {
        if (graphic) graphic->imageNo = 0x3B557;
    }
    return consumed;
}

// 原函数：0x00494D30。
static std::int32_t WindowSettings_ButtonSoundIncrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = platform::gEffectVolume >= 15
            ? L"无法再提高效果音量。" /* 原常量参考：0x00586F54。 */ : L"提高效果音量。" /* 原常量参考：0x00586F6C。 */;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        window->state |= 0x2000u;
        const std::int32_t volume = platform::gEffectVolume;
        if (volume >= 15) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            platform::gEffectVolume = volume + 1;
            static_cast<void>(audio::ApplyBgmVolume_52C3D0());
            cg::text::FormatTo(gSettingsEffectTextD2EA64, L"{}", platform::gEffectVolume);
            const std::int32_t width = WindowNumber_MeasureWidth(gSettingsEffectTextD2EA64, 3);
            const std::int32_t x = ((37 - width) >> 1) + gSettingsOffsetCA02DC + 69;
            if (auto* const element17 = currentWindow->FindElementById(WindowSettingsElementId::Element17); element17) element17->x = x;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (graphic) graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 0x3B55B;
    }
    if ((state & 8) != 0) {
        if (graphic) graphic->imageNo = 0x3B55A;
    }
    return consumed;
}

// 原函数：0x00494E60。
static std::int32_t WindowSettings_ToggleChannels(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"变更音响输出类型。" /* 原常量参考：0x00586F7C。 */;
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
    return {};
}

// 原函数：0x00494F30。
static std::int32_t WindowSettings_ToggleAudioFlag(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"变更音响输出类型。" /* 原常量参考：0x00586F7C。 */;
        }
        if ((state & 2) != 0) {
            window->state |= 0x2000u;
            platform::gChatNotificationSoundEnabled_554644 = static_cast<std::int32_t>(
                (static_cast<std::uint32_t>(platform::gChatNotificationSoundEnabled_554644) - 1u) & 1u);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if (platform::gChatNotificationSoundEnabled_554644 != 0) {
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
    return {};
}

// 原函数：0x00495000。
static std::int32_t WindowSettings_ToggleScanlines(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"扫描线同期设定" /* 原常量参考：0x00586E24。 */;
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
    return {};
}

// 原函数：0x004950D0。
static std::int32_t WindowSettings_DrawRenderModeButton(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"目前的描绘功能。" /* 原常量参考：0x00586F90。 */;
        }
        graphic->imageNo = 0x3B5A0 + 3 * index;
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
