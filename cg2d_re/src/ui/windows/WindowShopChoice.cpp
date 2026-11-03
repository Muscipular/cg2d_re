#include "cg/network/CgConnection.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowShopChoiceElementId : ElementId {
    Element1 = 1,
    Element2 = 2,
    SelectRow = 3,
};

namespace detail {

using WindowChoiceCallback = std::int32_t (*)(std::int32_t index);

// 原C9A978..C9A998，所有文本/表及回调均借用；计数保存低U8、循环按S8解释。
struct ChoiceDialogRuntimeSlots {
    std::int32_t type;
    std::int32_t imageNo;
    std::wstring title;
    std::vector<std::wstring> lines;
    std::vector<std::wstring> choices;
    std::uint8_t lineCount;
    std::uint8_t choiceCount;
    std::vector<std::wstring> hints;
    WindowChoiceCallback callback;
    std::uint8_t closeEnabled;
};

// 回调与内部函数前置声明。

// 原函数：0x0049FCA0。
static cg::yobi::YobiAction* WindowShopChoice_Open(
    std::wstring_view title, std::int32_t imageNo, const std::wstring* const* lines, std::uint8_t lineCount,
    const wchar_t* const* choices, std::uint8_t choiceCount,
    const wchar_t* const* hints, WindowChoiceCallback callback); // 窗口52只借用文本及回调。

// 原函数：0x0049FFA0。
static std::int32_t WindowShopChoice_SelectRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x004A00C0。
static std::int32_t WindowShopChoice_SendBuySellChoice(std::int32_t index); // 商店回复仅索引0..1发送，其他返回原索引。

// 原函数：0x004A0280。
static std::int32_t WindowShopChoice_SendFirstCommandChoice(std::int32_t index); // CB03D4命令表的选择回复。

// 原函数：0x004A0420。
static std::int32_t WindowShopChoice_SendSecondCommandChoice(std::int32_t index); // CF47D8命令表的选择回复。

// 原函数：0x004A05C0。
static std::int32_t WindowShopChoice_SendExchangeChoice(std::int32_t index); // 索引0..1分别发送1/2。

// 原函数：0x004A0700。
static std::int32_t WindowShopChoice_SendGiveReceiveChoice(std::int32_t index); // 仅索引0发送原字节1。

// 原函数：0x0049FD20。
static std::int32_t WindowShopChoice_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0049FE40。
static std::int32_t WindowShopChoice_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0049FF70。
static void WindowShopChoice_Release(WindowInstance&);

// 控件载荷、窗口状态、组件表与原型。

static ChoiceDialogRuntimeSlots gChoiceDialog {};

static std::array<const std::wstring*, 5> gChoiceDialogLinesD672D8 {}; // 原全局：0x00D672D8。

static std::int32_t gShopImageC9A9A0 = 0; // 原全局：0x00C9A9A0。

static std::array<const wchar_t*, 3> gShopChoicesCB1DC8 {}; // 原全局：0x00CB1DC8。

static std::array<const wchar_t*, 3> gShopHintsD2F738 {}; // 原全局：0x00D2F738。

static std::array<std::int8_t, 3> gShopCommandsD64E10 {}; // 原全局：0x00D64E10。

static constexpr std::array<const wchar_t*, 3> kShopChoices57E6B8 { // 原常量参考：0x0057E6B8。
    L"买", L"卖", L"不用了",
};

static std::int32_t gChoiceImageCB0AE4 = 0; // 原全局：0x00CB0AE4。

static std::int32_t gChoiceImageCF4A18 = 0; // 原全局：0x00CF4A18。

static std::int32_t gChoiceImageCC4890 = 0; // 原全局：0x00CC4890。

static std::int32_t gChoiceImageC9A99C = 0; // 原全局：0x00C9A99C。

static std::int32_t gChoiceImageD2EA74 = 0; // 原全局：0x00D2EA74。

static std::array<const wchar_t*, 2> gChoicesD67438 {}; // 原全局：0x00D67438。

static std::array<const wchar_t*, 2> gChoiceHintsCC31FC {}; // 原全局：0x00CC31FC。

static std::array<std::int8_t, 2> gChoiceCommandsCB03D4 {}; // 原全局：0x00CB03D4。

static std::array<const wchar_t*, 2> gChoicesD2D818 {}; // 原全局：0x00D2D818。

static std::array<const wchar_t*, 2> gChoiceHintsCA0AAC {}; // 原全局：0x00CA0AAC。

static std::array<std::int8_t, 2> gChoiceCommandsCF47D8 {}; // 原全局：0x00CF47D8。

static constexpr std::array<const wchar_t*, 3> kSkillChoices57E6F0 { // 原常量参考：0x0057E6F0。
    L"想学习技能",
    L"想遗忘技能",
    kShopChoices57E6B8[2],
};

static constexpr std::array<const wchar_t*, 3> kSkillHints57E6FC { // 原常量参考：0x0057E6FC。
    L"向这位技能师范学习技能。",
    L"遗忘自己学习到的技能。",
    L"不学习技能。",
};

static constexpr std::array<const wchar_t*, 3> kExchangeChoices57E708 { // 原常量参考：0x0057E708。
    L"交易", kShopChoices57E6B8[2], nullptr,
};

static constexpr std::array<const wchar_t*, 3> kExchangeHints57E714 { // 原常量参考：0x0057E714。
    L"交换这家店的物品。",
    kShopHints57E6C4[2], nullptr,
};

static constexpr std::array<const wchar_t*, 2> kGiveReceiveLabels57E720 { // 原常量参考：0x0057E720。
    L"给予物品", L"接受物品",
};

static std::array<const wchar_t*, 2> gGiveReceiveChoices57E728 {nullptr, kShopChoices57E6B8[2]}; // 原全局：0x0057E728。

static constexpr std::array<const wchar_t*, 3> kGiveReceiveHints57E730 { // 原常量参考：0x0057E730。
    kExchangeHints57E714[0], kShopHints57E6C4[2], nullptr,
};

static_assert(std::is_same_v<decltype(&WindowShopChoice_SendBuySellChoice), WindowChoiceCallback>);

static_assert(std::is_same_v<decltype(&WindowShopChoice_SendFirstCommandChoice), WindowChoiceCallback>);

static_assert(std::is_same_v<decltype(&WindowShopChoice_SendSecondCommandChoice), WindowChoiceCallback>);

static_assert(std::is_same_v<decltype(&WindowShopChoice_SendExchangeChoice), WindowChoiceCallback>);

static_assert(std::is_same_v<decltype(&WindowShopChoice_SendGiveReceiveChoice), WindowChoiceCallback>);

static WindowTextPrototype gWindowText56D828 { // 原全局：0x0056D828。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"",
};

static std::int32_t gWindowAnimationImage56D830 = 100000; // 原全局：0x0056D830。

static WindowElementPrototype gWindowElements56D838[3] { // 原全局：0x0056D838。
    {WindowElementType_Text, 40, 10, 0, 0, 1, {.text = &gWindowText56D828}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopChoiceElementId::Element1)},
    {WindowElementType_Animation, 40, 135, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56D830}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShopChoiceElementId::Element2)},
    {WindowElementType_EventArea, 99, 142, 108, 0, 1, {.unusedValue = 0}, &WindowShopChoice_SelectRow, -1, static_cast<ElementId>(WindowShopChoiceElementId::SelectRow)},
};

// 原型：窗口52；原全局：0x0054E960。
WindowPrototype gWindowPrototypeShopChoice {2, 4, 209, 117, 222, 216, 0x80010101u, 3, gWindowElements56D838, &WindowShopChoice_Update, &WindowShopChoice_Draw, &WindowShopChoice_Release}; // 52: 54E960

// 窗口函数实现。

// 五个选择创建入口共用的原字段提取/反转义/五行发布序列。
static void WindowShopChoice_FillBody(
    const char* const text, const std::int32_t field, const std::int32_t capacity)
{
    std::array<char, 256> body;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field, capacity, body.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(body.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& line = gNpcDialogLinesCC24D4[row + 1].text;
        static_cast<void>(input::ExtractGbkTextLine(line, body.data(), row, 20));
        gChoiceDialogLinesD672D8[row] = &line;
    }
}

// 调用者传原连续32B临时区；不缓存跨回复的目标/请求/坐标。
static network::SendResult WindowShopChoice_SendReply(const std::wstring_view buffer)
{
    const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 15);
    const std::int32_t target = world::gDialogTargetD2DE6C;
    const std::int32_t request = world::gDialogRequestCC21E0;
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    return cg::network::GetGameConnection().SendDialogResponse(x, y, request, target, 0, escapedReply);
}

// 原函数：0x0049FCA0。
static cg::yobi::YobiAction* WindowShopChoice_Open(
    const std::wstring_view title, const std::int32_t imageNo,
    const std::wstring* const* const lines, const std::uint8_t lineCount,
    const wchar_t* const* const choices, const std::uint8_t choiceCount,
    const wchar_t* const* const hints, const WindowChoiceCallback callback)
{
    gChoiceDialog.type = world::gCurrentDialogType_581800;
    gChoiceDialog.title = title;
    gChoiceDialog.lines.clear();
    for (std::uint8_t row = 0; row < lineCount; ++row) { gChoiceDialog.lines.push_back(*lines[row]); }
    gChoiceDialog.imageNo = imageNo;
    gChoiceDialog.choiceCount = choiceCount;
    WindowInstance* const previous = WindowRuntime_States()[WindowId_ShopChoice].window;
    gChoiceDialog.lineCount = lineCount;
    gChoiceDialog.choices.clear();
    for (std::uint8_t row = 0; row < choiceCount; ++row) { gChoiceDialog.choices.push_back(choices[row]); }
    gChoiceDialog.callback = callback;
    gChoiceDialog.hints.clear();
    for (std::uint8_t row = 0; row < choiceCount; ++row) { gChoiceDialog.hints.push_back(hints[row]); }
    if (previous != nullptr) {
        previous->state |= 0x8000u;
        gChoiceDialog.closeEnabled = 0;
    }
    return WindowRuntime_CreateAnimation(WindowId_ShopChoice, 1, 0, nullptr);
}

// 原函数：0x0049FD20。
static std::int32_t WindowShopChoice_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        auto* const element1 = currentWindow->FindElementById(WindowShopChoiceElementId::Element1);
        if (element1) {
            auto* const title = element1->data.text;
            title->text = gChoiceDialog.title;
        }
        const std::int32_t measured = cg::render::TextMeasure(gChoiceDialog.title, 1);
        WindowInstance* const window = currentWindow;
        if (auto* const element12 = window->FindElementById(WindowShopChoiceElementId::Element1); element12) element12->x = ((window->width - measured) >> 1) + 6;
        auto* const element2 = currentWindow->FindElementById(WindowShopChoiceElementId::Element2);
        if (element2) {
            auto* animation = element2->data.animation;
            cg::yobi::SetYobiActionField2A8(animation->action, gChoiceDialog.imageNo);
            cg::yobi::SetYobiActionField2B8(animation->action, cg::yobi::YobiDirection_Down);
        }
        const std::uint8_t count = gChoiceDialog.choiceCount;
        auto* const selectRowElement = currentWindow->FindElementById(WindowShopChoiceElementId::SelectRow);
        if (selectRowElement) {
            selectRowElement->height =
                count != 0 ? 14 * std::bit_cast<std::int8_t>(count) - 2 : 0;
        }
        gChoiceDialog.closeEnabled = 1;
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        static_cast<void>(gChoiceDialog.callback(-1));
    }
    return 1;
}

// 原函数：0x0049FE40。
static std::int32_t WindowShopChoice_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowRuntime_DrawCurrent(callbackWindow);
    for (std::int32_t row = 0; row < std::bit_cast<std::int8_t>(gChoiceDialog.lineCount); ++row) {
        const std::wstring& text = gChoiceDialog.lines[row];
            WindowInstance* const window = currentWindow;
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(window->x + 87),
                static_cast<std::int16_t>(window->y + 40 + row * 13),
                3, 12, 0, text, 0, 0));
    }
    for (std::int32_t row = 0; row < std::bit_cast<std::int8_t>(gChoiceDialog.choiceCount); ++row) {
        const std::wstring& text = gChoiceDialog.choices[row];
            WindowInstance* const window = currentWindow;
            auto* const selectRowElement = window->FindElementById(WindowShopChoiceElementId::SelectRow);
            if (selectRowElement) {
                const auto& element = (*selectRowElement);
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(window->x + element.x),
                    static_cast<std::int16_t>(window->y + element.y + row * 14),
                    3, 12, 0, text, 0, 0));
            }
    }
    WindowInstance* const window = currentWindow;
    cg::yobi::DrawNineGrid_49BE00(245300, static_cast<std::int16_t>(window->x),
        static_cast<std::int16_t>(window->y), static_cast<std::int16_t>(window->width),
        static_cast<std::int16_t>(window->height), 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    if (world::gCurrentDialogType_581800 != gChoiceDialog.type) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x0049FF70。
static void WindowShopChoice_Release(WindowInstance&)
{
    if (gChoiceDialog.closeEnabled != 0 && world::gCurrentDialogType_581800 == gChoiceDialog.type) {
        world::gCurrentDialogType_581800 = -1;
    }
}

// 原函数：0x0049FFA0。
static std::int32_t WindowShopChoice_SelectRow(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_Hover) == 0) {
        return 0;
    }
    for (std::int32_t index = 0;
         index < std::bit_cast<std::int8_t>(gChoiceDialog.choiceCount); ++index) {
        WindowInstance* const window = currentWindow;
        auto* const selectRowElement = window->FindElementById(WindowShopChoiceElementId::SelectRow);
        if (selectRowElement) {
            const auto& element = (*selectRowElement);
            const auto x = static_cast<std::int16_t>(window->x + element.x);
            const auto y = static_cast<std::int16_t>(window->y + element.y + index * 14);
            if (cg::ui::HitTestMouseRect_5049A0(x, y, x + element.width, y + 12, -1) == 0) {
                continue;
            }
            auto* const selectRowElement2 = currentWindow->FindElementById(WindowShopChoiceElementId::SelectRow);
            if (selectRowElement2) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 2), static_cast<std::uint16_t>(y - 2),
                    static_cast<std::uint16_t>(x + selectRowElement2->width + 2),
                    static_cast<std::uint16_t>(y + 14), 223, cg::render::gHoverFramePaletteIndex_D69768, 0);
            }
        }
        gHoverTextD68AE8 = gChoiceDialog.hints[index];
        if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_LDown) != 0) {
            const WindowChoiceCallback callback = gChoiceDialog.callback;
            if (callback != nullptr) {
                static_cast<void>(callback(index));
            }
            static_cast<void>(audio::PlaySe_52C450(79, 320));
            currentWindow->state |= 0x8000u;
            break;
        }
    }
    return 1;
}

// 原函数：0x004A00C0。
static std::int32_t WindowShopChoice_SendBuySellChoice(const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", static_cast<std::int32_t>(gShopCommandsD64E10[index]));
    const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 15);
    const std::int32_t target = world::gDialogTargetD2DE6C;
    const std::int32_t request = world::gDialogRequestCC21E0;
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    return static_cast<std::int32_t>(cg::network::GetGameConnection().SendDialogResponse(
        x, y, request, target, 0, escapedReply));
}

// 原函数：0x004A0140。
cg::yobi::YobiAction* WindowShopChoice_OpenBuySell(const char* const text)
{
    gShopImageC9A9A0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    std::array<char, 256> body;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 255, body.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(body.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& line = gNpcDialogLinesCC24D4[row + 1].text;
        static_cast<void>(input::ExtractGbkTextLine(line, body.data(), row, 20));
        gChoiceDialogLinesD672D8[row] = &line;
    }
    const auto mask = static_cast<std::uint8_t>(network::ParseDelimitedInteger_52CFA0(text, '|', 4));
    std::uint8_t count = 0;
    if ((mask & 1u) != 0) {
        gShopChoicesCB1DC8[0] = kShopChoices57E6B8[0];
        gShopHintsD2F738[0] = kShopHints57E6C4[0];
        gShopCommandsD64E10[0] = 1;
        count = 1;
    }
    if ((mask & 2u) != 0) {
        gShopChoicesCB1DC8[count] = kShopChoices57E6B8[1];
        gShopHintsD2F738[count] = kShopHints57E6C4[1];
        gShopCommandsD64E10[count] = 2;
        ++count;
    }
    gShopChoicesCB1DC8[count] = kShopChoices57E6B8[2];
    const std::int32_t imageNo = gShopImageC9A9A0;
    gShopHintsD2F738[count] = kShopHints57E6C4[2];
    gShopCommandsD64E10[count] = 3;
    return WindowShopChoice_Open(cg::text::FromGbkWide(title), imageNo, gChoiceDialogLinesD672D8.data(), 5,
        gShopChoicesCB1DC8.data(), static_cast<std::uint8_t>(count + 1),
        gShopHintsD2F738.data(), &WindowShopChoice_SendBuySellChoice);
}

// 原函数：0x004A0280。
static std::int32_t WindowShopChoice_SendFirstCommandChoice(const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", static_cast<std::int32_t>(gChoiceCommandsCB03D4[index]));
    return static_cast<std::int32_t>(WindowShopChoice_SendReply(buffer));
}

// 原函数：0x004A0420。
static std::int32_t WindowShopChoice_SendSecondCommandChoice(const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", static_cast<std::int32_t>(gChoiceCommandsCF47D8[index]));
    return static_cast<std::int32_t>(WindowShopChoice_SendReply(buffer));
}

// 原函数：0x004A05C0。
static std::int32_t WindowShopChoice_SendExchangeChoice(const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", index + 1);
    return static_cast<std::int32_t>(WindowShopChoice_SendReply(buffer));
}

// 原函数：0x004A0700。
static std::int32_t WindowShopChoice_SendGiveReceiveChoice(const std::int32_t index)
{
    if (index != 0) {
        return index;
    }
    std::wstring buffer;
    buffer = L"1";
    return static_cast<std::int32_t>(WindowShopChoice_SendReply(buffer));
}

// 原函数：0x004A0300。
cg::yobi::YobiAction* WindowShopChoice_OpenFirstCommandList(const char* const text)
{
    gChoiceImageCB0AE4 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    WindowShopChoice_FillBody(text, 3, 255);
    std::uint8_t count = 0;
    if ((network::ParseDelimitedInteger_52CFA0(text, '|', 4) & 1) != 0) {
        gChoicesD67438[0] = kShopChoices57E6B8[0];
        gChoiceHintsCC31FC[0] = kShopHints57E6C4[0];
        gChoiceCommandsCB03D4[0] = 1;
        count = 1;
    }
    const std::int32_t imageNo = gChoiceImageCB0AE4;
    gChoicesD67438[count] = kShopChoices57E6B8[2];
    gChoiceHintsCC31FC[count] = kShopHints57E6C4[2];
    gChoiceCommandsCB03D4[count] = 3;
    return WindowShopChoice_Open(cg::text::FromGbkWide(title), imageNo, gChoiceDialogLinesD672D8.data(), 5,
        gChoicesD67438.data(), static_cast<std::uint8_t>(count + 1),
        gChoiceHintsCC31FC.data(), &WindowShopChoice_SendFirstCommandChoice);
}

// 原函数：0x004A04A0。
cg::yobi::YobiAction* WindowShopChoice_OpenSecondCommandList(const char* const text)
{
    gChoiceImageCF4A18 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    WindowShopChoice_FillBody(text, 3, 255);
    std::uint8_t count = 0;
    if ((network::ParseDelimitedInteger_52CFA0(text, '|', 4) & 1) != 0) {
        gChoicesD2D818[0] = kShopChoices57E6B8[0];
        gChoiceHintsCA0AAC[0] = kShopHints57E6C4[0];
        gChoiceCommandsCF47D8[0] = 1;
        count = 1;
    }
    const std::int32_t imageNo = gChoiceImageCF4A18;
    gChoicesD2D818[count] = kShopChoices57E6B8[2];
    gChoiceHintsCA0AAC[count] = kShopHints57E6C4[2];
    gChoiceCommandsCF47D8[count] = 3;
    return WindowShopChoice_Open(cg::text::FromGbkWide(title), imageNo, gChoiceDialogLinesD672D8.data(), 5,
        gChoicesD2D818.data(), static_cast<std::uint8_t>(count + 1),
        gChoiceHintsCA0AAC.data(), &WindowShopChoice_SendSecondCommandChoice);
}

// 原函数：0x004A0640。
cg::yobi::YobiAction* WindowShopChoice_OpenSkillChoice(const char* const text)
{
    gChoiceImageCC4890 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    WindowShopChoice_FillBody(text, 3, 255);
    return WindowShopChoice_Open(cg::text::FromGbkWide(title), gChoiceImageCC4890, gChoiceDialogLinesD672D8.data(), 5,
        kSkillChoices57E6F0.data(), 3, kSkillHints57E6FC.data(), &WindowShopChoice_SendExchangeChoice);
}

// 原函数：0x004A0790。
cg::yobi::YobiAction* WindowShopChoice_OpenExchangeChoice(const char* const text)
{
    gChoiceImageC9A99C = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    WindowShopChoice_FillBody(text, 3, 255);
    return WindowShopChoice_Open(cg::text::FromGbkWide(title), gChoiceImageC9A99C, gChoiceDialogLinesD672D8.data(), 5,
        kExchangeChoices57E708.data(), 2, kExchangeHints57E714.data(), &WindowShopChoice_SendGiveReceiveChoice);
}

// 原函数：0x004A0850。
cg::yobi::YobiAction* WindowShopChoice_OpenGiveReceiveChoice(const char* const text)
{
    gChoiceImageD2EA74 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    const std::int32_t receive = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    gGiveReceiveChoices57E728[0] = kGiveReceiveLabels57E720[0];
    if (receive != 0) {
        gGiveReceiveChoices57E728[0] = kGiveReceiveLabels57E720[1];
    }
    WindowShopChoice_FillBody(text, 4, 256);
    return WindowShopChoice_Open(cg::text::FromGbkWide(title), gChoiceImageD2EA74, gChoiceDialogLinesD672D8.data(), 5,
        gGiveReceiveChoices57E728.data(), 2, kGiveReceiveHints57E730.data(), &WindowShopChoice_SendExchangeChoice);
}

} // namespace detail

} // namespace cg::ui
