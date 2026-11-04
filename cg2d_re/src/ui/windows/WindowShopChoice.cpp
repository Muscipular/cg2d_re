#include "cg/session/ClientSession.hpp"
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

// 回调与内部函数前置声明。

// 原函数：0x0049FCA0。
static cg::yobi::YobiAction* WindowShopChoice_Open(
    session::ClientSession& session,
    std::wstring_view title, std::int32_t imageNo, const std::wstring* const* lines, std::uint8_t lineCount,
    const wchar_t* const* choices, std::uint8_t choiceCount,
    const wchar_t* const* hints, WindowChoiceCallback callback); // 窗口52只借用文本及回调。

// 原函数：0x0049FFA0。
static std::int32_t WindowShopChoice_SelectRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state);

// 原函数：0x004A00C0。
static std::int32_t WindowShopChoice_SendBuySellChoice(session::ClientSession& session, std::int32_t index); // 商店回复仅索引0..1发送，其他返回原索引。

// 原函数：0x004A0280。
static std::int32_t WindowShopChoice_SendFirstCommandChoice(session::ClientSession& session, std::int32_t index); // CB03D4命令表的选择回复。

// 原函数：0x004A0420。
static std::int32_t WindowShopChoice_SendSecondCommandChoice(session::ClientSession& session, std::int32_t index); // CF47D8命令表的选择回复。

// 原函数：0x004A05C0。
static std::int32_t WindowShopChoice_SendExchangeChoice(session::ClientSession& session, std::int32_t index); // 索引0..1分别发送1/2。

// 原函数：0x004A0700。
static std::int32_t WindowShopChoice_SendGiveReceiveChoice(session::ClientSession& session, std::int32_t index); // 仅索引0发送原字节1。

// 原函数：0x0049FD20。
static std::int32_t WindowShopChoice_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0049FE40。
static std::int32_t WindowShopChoice_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0049FF70。
static void WindowShopChoice_Release(WindowInstance&);

// 控件载荷、窗口状态、组件表与原型。

static constexpr std::array<const wchar_t*, 3> kShopChoices57E6B8 { // 原常量参考：0x0057E6B8。
    L"买", L"卖", L"不用了",
};

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
    session::ClientSession& session,
    const char* const text, const std::int32_t field, const std::int32_t capacity)
{
    auto& choice = session.UiFrame().native.shopChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    std::array<char, 256> body;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field, capacity, body.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(body.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& line = npcLines[row + 1].text;
        static_cast<void>(input::ExtractGbkTextLine(line, body.data(), row, 20));
        choice.lines[row] = &line;
    }
}

// 调用者传原连续32B临时区；不缓存跨回复的目标/请求/坐标。
static network::SendResult WindowShopChoice_SendReply(session::ClientSession& session, const std::wstring_view buffer)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 15);
    const std::int32_t target = npcDialog.target;
    const std::int32_t request = npcDialog.request;
    const std::int32_t y =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    return session.Connection().SendDialogResponse(x, y, request, target, 0, escapedReply);
}

// 原函数：0x0049FCA0。
static cg::yobi::YobiAction* WindowShopChoice_Open(
    session::ClientSession& session,
    const std::wstring_view title, const std::int32_t imageNo,
    const std::wstring* const* const lines, const std::uint8_t lineCount,
    const wchar_t* const* const choices, const std::uint8_t choiceCount,
    const wchar_t* const* const hints, const WindowChoiceCallback callback)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& choice = session.UiFrame().native.shopChoice;
    choice.dialog.type = npcDialog.type;
    choice.dialog.title = title;
    choice.dialog.lines.clear();
    for (std::uint8_t row = 0; row < lineCount; ++row) { choice.dialog.lines.push_back(*lines[row]); }
    choice.dialog.imageNo = imageNo;
    choice.dialog.choiceCount = choiceCount;
    WindowInstance* const previous = WindowRuntime_States(session)[WindowId_ShopChoice].window;
    choice.dialog.lineCount = lineCount;
    choice.dialog.choices.clear();
    for (std::uint8_t row = 0; row < choiceCount; ++row) { choice.dialog.choices.push_back(choices[row]); }
    choice.dialog.callback = callback;
    choice.dialog.hints.clear();
    for (std::uint8_t row = 0; row < choiceCount; ++row) { choice.dialog.hints.push_back(hints[row]); }
    if (previous != nullptr) {
        previous->state |= 0x8000u;
        choice.dialog.closeEnabled = 0;
    }
    return WindowRuntime_CreateAnimation(session, WindowId_ShopChoice, 1, 0, nullptr);
}

// 原函数：0x0049FD20。
static std::int32_t WindowShopChoice_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.shopChoice;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        auto* const element1 = currentWindow->FindElementById(WindowShopChoiceElementId::Element1);
        if (element1) {
            auto* const title = element1->data.text;
            title->text = choice.dialog.title;
        }
        const std::int32_t measured = cg::render::TextMeasure(choice.dialog.title, 1);
        WindowInstance* const window = currentWindow;
        if (auto* const element12 = window->FindElementById(WindowShopChoiceElementId::Element1); element12) element12->x = ((window->width - measured) >> 1) + 6;
        auto* const element2 = currentWindow->FindElementById(WindowShopChoiceElementId::Element2);
        if (element2) {
            auto* animation = element2->data.animation;
            cg::yobi::SetYobiActionField2A8(animation->action, choice.dialog.imageNo);
            cg::yobi::SetYobiActionField2B8(animation->action, cg::yobi::YobiDirection_Down);
        }
        const std::uint8_t count = choice.dialog.choiceCount;
        auto* const selectRowElement = currentWindow->FindElementById(WindowShopChoiceElementId::SelectRow);
        if (selectRowElement) {
            selectRowElement->height =
                count != 0 ? 14 * std::bit_cast<std::int8_t>(count) - 2 : 0;
        }
        choice.dialog.closeEnabled = 1;
    }
    if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 2, 2) != 0) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
        static_cast<void>(choice.dialog.callback(*callbackWindow.ownerSession, -1));
    }
    return 1;
}

// 原函数：0x0049FE40。
static std::int32_t WindowShopChoice_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    auto& npcDialog = callbackWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = callbackWindow.ownerSession->UiFrame().native.shopChoice;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowRuntime_DrawCurrent(callbackWindow);
    for (std::int32_t row = 0; row < std::bit_cast<std::int8_t>(choice.dialog.lineCount); ++row) {
        const std::wstring& text = choice.dialog.lines[row];
            WindowInstance* const window = currentWindow;
            static_cast<void>(render::DrawText_456780(*callbackWindow.ownerSession,
                static_cast<std::int16_t>(window->x + 87),
                static_cast<std::int16_t>(window->y + 40 + row * 13),
                3, 12, 0, text, 0, 0));
    }
    for (std::int32_t row = 0; row < std::bit_cast<std::int8_t>(choice.dialog.choiceCount); ++row) {
        const std::wstring& text = choice.dialog.choices[row];
            WindowInstance* const window = currentWindow;
            auto* const selectRowElement = window->FindElementById(WindowShopChoiceElementId::SelectRow);
            if (selectRowElement) {
                const auto& element = (*selectRowElement);
                static_cast<void>(render::DrawText_456780(*callbackWindow.ownerSession,
                    static_cast<std::int16_t>(window->x + element.x),
                    static_cast<std::int16_t>(window->y + element.y + row * 14),
                    3, 12, 0, text, 0, 0));
            }
    }
    WindowInstance* const window = currentWindow;
    cg::yobi::DrawNineGrid_49BE00(*callbackWindow.ownerSession, 245300, static_cast<std::int16_t>(window->x),
        static_cast<std::int16_t>(window->y), static_cast<std::int16_t>(window->width),
        static_cast<std::int16_t>(window->height), 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    if (npcDialog.type != choice.dialog.type) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(*callbackWindow.ownerSession, 3));
    return 1;
}

// 原函数：0x0049FF70。
static void WindowShopChoice_Release(WindowInstance& releasedWindow)
{
    auto& npcDialog = releasedWindow.ownerSession->WorldScene().npcDialog;
    auto& choice = releasedWindow.ownerSession->UiFrame().native.shopChoice;
    if (choice.dialog.closeEnabled != 0 && npcDialog.type == choice.dialog.type) {
        npcDialog.type = -1;
    }
}

// 原函数：0x0049FFA0。
static std::int32_t WindowShopChoice_SelectRow(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& choice = callbackWindow.ownerSession->UiFrame().native.shopChoice;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_Hover) == 0) {
        return 0;
    }
    for (std::int32_t index = 0;
         index < std::bit_cast<std::int8_t>(choice.dialog.choiceCount); ++index) {
        WindowInstance* const window = currentWindow;
        auto* const selectRowElement = window->FindElementById(WindowShopChoiceElementId::SelectRow);
        if (selectRowElement) {
            const auto& element = (*selectRowElement);
            const auto x = static_cast<std::int16_t>(window->x + element.x);
            const auto y = static_cast<std::int16_t>(window->y + element.y + index * 14);
            if (cg::ui::HitTestMouseRect_5049A0(*callbackWindow.ownerSession, x, y, x + element.width, y + 12, -1) == 0) {
                continue;
            }
            auto* const selectRowElement2 = currentWindow->FindElementById(WindowShopChoiceElementId::SelectRow);
            if (selectRowElement2) {
                cg::render::DrawGraphicHover(*callbackWindow.ownerSession,
                    static_cast<std::uint16_t>(x - 2), static_cast<std::uint16_t>(y - 2),
                    static_cast<std::uint16_t>(x + selectRowElement2->width + 2),
                    static_cast<std::uint16_t>(y + 14), 223, callbackWindow.ownerSession->UiFrame().hoverFramePaletteIndex, 0);
            }
        }
        uiFrame.hoverText = choice.dialog.hints[index];
        if ((platform::ReadSessionMouseEdgeButtons(*callbackWindow.ownerSession) & platform::LegacyMouseEdge_LDown) != 0) {
            const WindowChoiceCallback callback = choice.dialog.callback;
            if (callback != nullptr) {
                static_cast<void>(callback(*callbackWindow.ownerSession, index));
            }
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 79, 320));
            currentWindow->state |= 0x8000u;
            break;
        }
    }
    return 1;
}

// 原函数：0x004A00C0。
static std::int32_t WindowShopChoice_SendBuySellChoice(session::ClientSession& session, const std::int32_t index)
{
    auto& npcDialog = session.WorldScene().npcDialog;
    auto& choice = session.UiFrame().native.shopChoice;
    if (index < 0 || index > 1) {
        return index;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", static_cast<std::int32_t>(choice.shopCommands[index]));
    const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 15);
    const std::int32_t target = npcDialog.target;
    const std::int32_t request = npcDialog.request;
    const std::int32_t y =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    return static_cast<std::int32_t>(session.Connection().SendDialogResponse(
        x, y, request, target, 0, escapedReply));
}

// 原函数：0x004A0140。
cg::yobi::YobiAction* WindowShopChoice_OpenBuySell(session::ClientSession& session, const char* const text)
{
    auto& choice = session.UiFrame().native.shopChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    choice.shopImage = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    std::array<char, 256> body;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 255, body.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(body.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        std::wstring& line = npcLines[row + 1].text;
        static_cast<void>(input::ExtractGbkTextLine(line, body.data(), row, 20));
        choice.lines[row] = &line;
    }
    const auto mask = static_cast<std::uint8_t>(network::ParseDelimitedInteger_52CFA0(text, '|', 4));
    std::uint8_t count = 0;
    if ((mask & 1u) != 0) {
        choice.shopChoices[0] = kShopChoices57E6B8[0];
        choice.shopHints[0] = kShopHints57E6C4[0];
        choice.shopCommands[0] = 1;
        count = 1;
    }
    if ((mask & 2u) != 0) {
        choice.shopChoices[count] = kShopChoices57E6B8[1];
        choice.shopHints[count] = kShopHints57E6C4[1];
        choice.shopCommands[count] = 2;
        ++count;
    }
    choice.shopChoices[count] = kShopChoices57E6B8[2];
    const std::int32_t imageNo = choice.shopImage;
    choice.shopHints[count] = kShopHints57E6C4[2];
    choice.shopCommands[count] = 3;
    return WindowShopChoice_Open(session, cg::text::FromGbkWide(title), imageNo, choice.lines.data(), 5,
        choice.shopChoices.data(), static_cast<std::uint8_t>(count + 1),
        choice.shopHints.data(), &WindowShopChoice_SendBuySellChoice);
}

// 原函数：0x004A0280。
static std::int32_t WindowShopChoice_SendFirstCommandChoice(session::ClientSession& session, const std::int32_t index)
{
    auto& choice = session.UiFrame().native.shopChoice;
    if (index < 0 || index > 1) {
        return index;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", static_cast<std::int32_t>(choice.firstCommands[index]));
    return static_cast<std::int32_t>(WindowShopChoice_SendReply(session, buffer));
}

// 原函数：0x004A0420。
static std::int32_t WindowShopChoice_SendSecondCommandChoice(session::ClientSession& session, const std::int32_t index)
{
    auto& choice = session.UiFrame().native.shopChoice;
    if (index < 0 || index > 1) {
        return index;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", static_cast<std::int32_t>(choice.secondCommands[index]));
    return static_cast<std::int32_t>(WindowShopChoice_SendReply(session, buffer));
}

// 原函数：0x004A05C0。
static std::int32_t WindowShopChoice_SendExchangeChoice(session::ClientSession& session, const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", index + 1);
    return static_cast<std::int32_t>(WindowShopChoice_SendReply(session, buffer));
}

// 原函数：0x004A0700。
static std::int32_t WindowShopChoice_SendGiveReceiveChoice(session::ClientSession& session, const std::int32_t index)
{
    if (index != 0) {
        return index;
    }
    std::wstring buffer;
    buffer = L"1";
    return static_cast<std::int32_t>(WindowShopChoice_SendReply(session, buffer));
}

// 原函数：0x004A0300。
cg::yobi::YobiAction* WindowShopChoice_OpenFirstCommandList(session::ClientSession& session, const char* const text)
{
    auto& choice = session.UiFrame().native.shopChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    choice.firstCommandImage = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    WindowShopChoice_FillBody(session, text, 3, 255);
    std::uint8_t count = 0;
    if ((network::ParseDelimitedInteger_52CFA0(text, '|', 4) & 1) != 0) {
        choice.firstChoices[0] = kShopChoices57E6B8[0];
        choice.firstHints[0] = kShopHints57E6C4[0];
        choice.firstCommands[0] = 1;
        count = 1;
    }
    const std::int32_t imageNo = choice.firstCommandImage;
    choice.firstChoices[count] = kShopChoices57E6B8[2];
    choice.firstHints[count] = kShopHints57E6C4[2];
    choice.firstCommands[count] = 3;
    return WindowShopChoice_Open(session, cg::text::FromGbkWide(title), imageNo, choice.lines.data(), 5,
        choice.firstChoices.data(), static_cast<std::uint8_t>(count + 1),
        choice.firstHints.data(), &WindowShopChoice_SendFirstCommandChoice);
}

// 原函数：0x004A04A0。
cg::yobi::YobiAction* WindowShopChoice_OpenSecondCommandList(session::ClientSession& session, const char* const text)
{
    auto& choice = session.UiFrame().native.shopChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    choice.secondCommandImage = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    WindowShopChoice_FillBody(session, text, 3, 255);
    std::uint8_t count = 0;
    if ((network::ParseDelimitedInteger_52CFA0(text, '|', 4) & 1) != 0) {
        choice.secondChoices[0] = kShopChoices57E6B8[0];
        choice.secondHints[0] = kShopHints57E6C4[0];
        choice.secondCommands[0] = 1;
        count = 1;
    }
    const std::int32_t imageNo = choice.secondCommandImage;
    choice.secondChoices[count] = kShopChoices57E6B8[2];
    choice.secondHints[count] = kShopHints57E6C4[2];
    choice.secondCommands[count] = 3;
    return WindowShopChoice_Open(session, cg::text::FromGbkWide(title), imageNo, choice.lines.data(), 5,
        choice.secondChoices.data(), static_cast<std::uint8_t>(count + 1),
        choice.secondHints.data(), &WindowShopChoice_SendSecondCommandChoice);
}

// 原函数：0x004A0640。
cg::yobi::YobiAction* WindowShopChoice_OpenSkillChoice(session::ClientSession& session, const char* const text)
{
    auto& choice = session.UiFrame().native.shopChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    choice.skillImage = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    WindowShopChoice_FillBody(session, text, 3, 255);
    return WindowShopChoice_Open(session, cg::text::FromGbkWide(title), choice.skillImage, choice.lines.data(), 5,
        kSkillChoices57E6F0.data(), 3, kSkillHints57E6FC.data(), &WindowShopChoice_SendExchangeChoice);
}

// 原函数：0x004A0790。
cg::yobi::YobiAction* WindowShopChoice_OpenExchangeChoice(session::ClientSession& session, const char* const text)
{
    auto& choice = session.UiFrame().native.shopChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    choice.exchangeImage = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    WindowShopChoice_FillBody(session, text, 3, 255);
    return WindowShopChoice_Open(session, cg::text::FromGbkWide(title), choice.exchangeImage, choice.lines.data(), 5,
        kExchangeChoices57E708.data(), 2, kExchangeHints57E714.data(), &WindowShopChoice_SendGiveReceiveChoice);
}

// 原函数：0x004A0850。
cg::yobi::YobiAction* WindowShopChoice_OpenGiveReceiveChoice(session::ClientSession& session, const char* const text)
{
    auto& choice = session.UiFrame().native.shopChoice;
    auto& npcLines = session.UiFrame().native.npcDialogLines;
    choice.giveReceiveImage = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    npcLines[0].text = cg::text::FromGbkWide(title);
    const std::int32_t receive = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    choice.giveReceiveChoices[0] = kGiveReceiveLabels57E720[0];
    if (receive != 0) {
        choice.giveReceiveChoices[0] = kGiveReceiveLabels57E720[1];
    }
    WindowShopChoice_FillBody(session, text, 4, 256);
    return WindowShopChoice_Open(session, cg::text::FromGbkWide(title), choice.giveReceiveImage, choice.lines.data(), 5,
        choice.giveReceiveChoices.data(), 2, kGiveReceiveHints57E730.data(), &WindowShopChoice_SendExchangeChoice);
}

} // namespace detail

ShopChoiceWindowRuntimeState::ShopChoiceWindowRuntimeState() noexcept
    : giveReceiveChoices {nullptr, detail::kShopChoices57E6B8[2]}
{
}

} // namespace cg::ui
