#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

// 控件载荷、窗口状态、组件表与原型。

InputContext gInputContextCC2080 {}; // 原全局：0x00CC2080。

} // namespace cg::input

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004B5B70。
static std::int32_t WindowMessageBoard_HandleInput(std::int32_t index, std::int32_t state); // 输入焦点及非空文本提示。

// 原函数：0x004B5C30。
static std::int32_t WindowMessageBoard_HandleButtons(std::int32_t index, std::int32_t state); // 删除/写入/取消/翻页完整交互。

// 原函数：0x004B62A0。
static void WindowMessageBoard_RefreshRecords(); // 四条记录刷新元素及输入位置；唯一caller弃用原地址低字节返回。

// 原函数：0x004B6C20。
static std::int32_t WindowMessageBoard_Close(); // 清对话类型、关闭入口时实例并透传SE57结果。

// 原函数：0x004B5AD0。
static std::int32_t WindowMessageBoard_Update(std::int32_t value);

// 原函数：0x004B5B50。
static void WindowMessageBoard_Release();

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00580618。 借Input唯一CC2080，供更新与编辑交互。
static WindowInputPrototype gWindow50InputPrototype580618 { // 原全局：0x00580618。
    {{&input::gInputContextCC2080}, WindowInputReferenceKind::Context},
    33, 368, 3, 12, 0, {}, 2, 70, 16, 0,
};

// 原D30138..D303F8；4B6970整区清零，4DFD38从+9起消费，其他字段尚未冻结。
static std::array<std::byte, 0x2C0> gWindow50AlternateRecordsD30138 {}; // 原全局：0x00D30138。

static WindowGraphicData gWindowGraphic5734D8 {0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005734D8。

static WindowGraphicData gWindowGraphic5734F0 {0x3BB3C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005734F0。

static WindowGraphicData gWindowGraphic573508 {0x3BB3D, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00573508。

static WindowGraphicData gWindowGraphic573520 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573520。

static WindowGraphicData gWindowGraphic573538 {0x3B577, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573538。

static WindowGraphicData gWindowGraphic573550 {0x3BB3E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573550。

static WindowGraphicData gWindowGraphic573568 {0x3BB41, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573568。

static WindowGraphicData gWindowGraphic573580 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573580。

static WindowGraphicData gWindowGraphic573598 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573598。

static WindowTextPrototype gWindowText5735B0 { // 原全局：0x005735B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"留言板文字列",
};

static WindowTextPrototype gWindowText5735B8 { // 原全局：0x005735B8。
    {std::byte{0}, std::byte{10}, std::byte{0}, std::byte{0}}, L"名稱",
};

static WindowTextPrototype gWindowText5735C0 { // 原全局：0x005735C0。
    {std::byte{0}, std::byte{10}, std::byte{0}, std::byte{0}}, L"留言時間",
};

static WindowElementPrototype gWindowElements5735C8[36] { // 原全局：0x005735C8。
    {WindowElementType_Graphic, 593, 8, 12, 12, 1, {.graphic = &gWindowGraphic573520}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Input, 27, 362, 503, 42, 1, {.unusedValue = 0}, &WindowMessageBoard_HandleInput},
    {WindowElementType_Graphic, 22, 38, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 135, 46, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 330, 46, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 530, 46, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Text, 100, 73, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 89, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 105, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 22, 114, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 135, 122, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 330, 122, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 530, 122, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Text, 100, 150, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 166, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 182, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 22, 190, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 135, 198, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 330, 198, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 530, 198, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Text, 100, 226, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 242, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 258, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 22, 266, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 135, 274, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 330, 274, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 530, 274, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Text, 100, 302, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 318, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 334, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 538, 364, 49, 17, 1, {.graphic = &gWindowGraphic573550}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Graphic, 538, 386, 49, 17, 1, {.graphic = &gWindowGraphic573568}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Graphic, 576, 342, 17, 17, 1, {.graphic = &gWindowGraphic573580}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Graphic, 536, 342, 17, 17, 1, {.graphic = &gWindowGraphic573598}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5734F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 30, 0, 0, 1, {.graphic = &gWindowGraphic573508}, &WindowElement_NoOp},
};

// 原型：窗口50；原全局：0x0054EC30。
WindowPrototype gWindowPrototypeMessageBoard {0, 4, 5, 20, 634, 424, 0x80010101u, 36, gWindowElements5735C8, &WindowMessageBoard_Update, &WindowRuntime_DrawDefault, &WindowMessageBoard_Release}; // 50: 54EC30

// 窗口函数实现。

const std::byte* WindowMessageBoard_BorrowRecords() noexcept
{
    return gWindow50AlternateRecordsD30138.data();
}

// 原函数：0x004B5A90。
void WindowMessageBoard_ResetRecords()
{
    gLoginWindowRuntime.valueD3FAD0 = 0;
    for (WindowListRecordD3FAD4& record : gLoginWindowRuntime.recordsD3FAD4) {
        record.value00 = 0;
        record.value408 = 0;
        record.texts08[1][0] = '\0';
        record.texts08[2][0] = '\0';
        record.texts08[3][0] = '\0';
        record.texts08[0][0] = '\0';
        record.value04 = 0;
    }
}

// 原函数：0x004B5AD0。
static std::int32_t WindowMessageBoard_Update(const std::int32_t value)
{
    if (value == -1) {
        gWindow50InputPrototype580618.input.context->clearText();
        WindowInstance* const window = gCurrentWindow;
        static_cast<void>(WindowInput_ActivateElement(&gWindow50InputPrototype580618, window->x, window->y, 0));
        input::SetContext_44B330(gWindow50InputPrototype580618.input.context);
        gFocusedElementDataD45F94 = gCurrentWindow->elements[1].data;
        static_cast<WindowInputData*>(gCurrentWindow->elements[1].data)->context =
            gWindow50InputPrototype580618.input.context;
    }
    WindowMessageBoard_RefreshRecords();
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        static_cast<void>(WindowMessageBoard_Close());
    }
    return 1;
}

// 原函数：0x004B5B50。
static void WindowMessageBoard_Release()
{
    if (world::detail::gValue581800 == 21) {
        world::detail::gValue581800 = -1;
    }
}

// 原函数：0x004B5B70。
static std::int32_t WindowMessageBoard_HandleInput(const std::int32_t index, const std::int32_t state)
{
    if ((state & 1) != 0 && !gWindow50InputPrototype580618.input.context->empty()) {
        gHoverTextD68AE8 = L"寫下訊息。" /* 原常量参考：0x00587C84。 */;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    WindowInstance* const window = gCurrentWindow;
    if (gFocusedElementDataD45F94 != window->elements[index].data) {
        static_cast<void>(WindowInput_ActivateElement(&gWindow50InputPrototype580618, window->x, window->y, 0));
        input::SetContext_44B330(gWindow50InputPrototype580618.input.context);
        gFocusedElementDataD45F94 = gCurrentWindow->elements[index].data;
        static_cast<WindowInputData*>(gCurrentWindow->elements[index].data)->context =
            gWindow50InputPrototype580618.input.context;
    }
    return 1;
}

// 原函数：0x004B5C30。
static std::int32_t WindowMessageBoard_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::wstring escaped;
    std::wstring text;
    std::int32_t consumed = 0;
    switch (index) {
    case 0:
    case 31: {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        if ((state & 1) != 0) {
            graphic->imageNo = index == 0 ? 243002 : 244547;
            const wchar_t* const hint = index == 0
                ? L"關閉這個視窗。"
                : kShopHints57E6C4[2];
            gHoverTextD68AE8 = hint;
            consumed = 1;
        } else {
            graphic->imageNo = index == 0 ? 243001 : 244545;
        }
        if ((state & 2) != 0) {
            static_cast<void>(WindowMessageBoard_Close());
        }
        return consumed;
    }
    case 5:
    case 12:
    case 19:
    case 26: {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        if ((state & 1) != 0) {
            graphic->imageNo = 36818;
            gHoverTextD68AE8 = L"刪除此訊息。";
            consumed = 1;
        } else {
            graphic->imageNo = 36816;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        cg::text::FormatTo(text, L"{}", (index - 5) / 7);
        escaped = input::EscapeTextAndNotify(text, 255);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t y =
            world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x =
            world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 64, escaped));
        static_cast<void>(WindowMessageBoard_Close());
        return 1;
    }
    case 30: {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[30].data);
        if ((state & 1) != 0) {
            graphic->imageNo = 244544;
            gHoverTextD68AE8 = L"寫下訊息。" /* 原常量参考：0x00587C84。 */;
            consumed = 1;
        } else {
            graphic->imageNo = 244542;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        input::InputContext* const context = gWindow50InputPrototype580618.input.context;
        if (!context->empty()) {
            const std::wstring escapedText = input::EscapeTextAndNotify(input::ContextText(*context), 256);
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t y =
                world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t x =
                world::detail::BorrowPlayerXStorage()->value();
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 1, escapedText));
            gWindow50InputPrototype580618.input.context->clearText();
            static_cast<void>(WindowMessageBoard_Close());
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        return 1;
    }
    case 32:
    case 33: {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        const std::int32_t baseImage = index == 32 ? 243033 : 243030;
        if ((state & 1) != 0) {
            graphic->imageNo = baseImage + 2;
            const wchar_t* const hint = index == 32
                ? L"切換至下一頁視窗。"
                : L"回到上一頁視窗。";
            gHoverTextD68AE8 = hint;
            consumed = 1;
        } else {
            graphic->imageNo = baseImage;
        }
        if ((state & 2) != 0) {
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            escaped.clear();
            const std::int32_t y =
                world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t x =
                world::detail::BorrowPlayerXStorage()->value();
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, index == 32 ? 32 : 16, escaped));
            static_cast<void>(WindowMessageBoard_Close());
            consumed = 1;
        }
        if ((state & 8) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = baseImage + 1;
        }
        return consumed;
    }
    default:
        return 0;
    }
}

// 原函数：0x004B62A0。
static void WindowMessageBoard_RefreshRecords()
{
    std::wstring buffer;
    for (std::int32_t index = 0; index < 4; ++index) {
        const auto& record = gLoginWindowRuntime.recordsD3FAD4[index];
        const std::int32_t first = 2 + index * 7;
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[first].data);
        graphic->imageNo = record.value00;
        auto* const name = static_cast<WindowTextData*>(gCurrentWindow->elements[first + 1].data);
        name->text = cg::text::FromGbkWide(record.texts08[0].data());
        std::int32_t time = record.value04;
        if (time > 0) {
            const platform::LegacyTm53B8C6* const calendar = platform::LocalTime_53B8C6(&time);
            const auto* const bytes = reinterpret_cast<const std::byte*>(calendar);
            std::int32_t minute;
            std::memcpy(&minute, bytes + 4, sizeof(minute));
            std::int32_t hour;
            std::memcpy(&hour, bytes + 8, sizeof(hour));
            std::int32_t day;
            std::memcpy(&day, bytes + 12, sizeof(day));
            std::int32_t month;
            std::memcpy(&month, bytes + 16, sizeof(month));
            std::int32_t year;
            std::memcpy(&year, bytes + 20, sizeof(year));
            cg::text::FormatTo(buffer, L"{:4}/{:02}/{:02}/{:02}:{:02}", year + 1900, month + 1, day, hour, minute);
            auto* const date = static_cast<WindowTextData*>(gCurrentWindow->elements[first + 2].data);
            date->text = buffer;
        } else {
            gCurrentWindow->elements[first + 2].visible = 0;
        }
        for (std::int32_t line = 1; line < 4; ++line) {
            auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[first + 3 + line].data);
            text->text = cg::text::FromGbkWide(record.texts08[line].data());
        }
        gCurrentWindow->elements[first + 3].visible = record.value408 != 0 ? 1 : 0;
    }
    gCurrentWindow->elements[32].visible = (gLoginWindowRuntime.valueD3FAD0 & 0x20u) != 0 ? 1 : 0;
    gCurrentWindow->elements[33].visible = (gLoginWindowRuntime.valueD3FAD0 & 0x10u) != 0 ? 1 : 0;
    WindowInstance* const window = gCurrentWindow;
    if (gFocusedElementDataD45F94 == window->elements[1].data) {
        static_cast<void>(WindowInput_ActivateElement(&gWindow50InputPrototype580618, window->x, window->y, 2));
    }
}

// 原函数：0x004B6C20。
static std::int32_t WindowMessageBoard_Close()
{
    WindowInstance* const window = gCurrentWindow;
    world::detail::gValue581800 = -1;
    window->state |= 0x8000u;
    return audio::PlaySe_52C450(57, 320);
}

// 原函数：0x004B6970。
std::int32_t WindowMessageBoard_ReceiveRecords(const std::int32_t flags, char* const text)
{
    gLoginWindowRuntime.valueD3FAD0 = static_cast<std::uint32_t>(flags);
    input::UnescapeLegacyText_52D150(text);
    gWindow50AlternateRecordsD30138.fill(std::byte{0});
    char titleGbk[17]{};
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 1, 16, titleGbk));
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(titleGbk);
    std::array<char, 3072> buffer;
    char* const fieldText = buffer.data();
    char* const copiedText = buffer.data() + 768;
    std::int32_t field = 2;
    std::int32_t offset = 0;
    do {
        auto& record = gLoginWindowRuntime.recordsD3FAD4[offset / 0x40C];
        const std::int32_t imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record.value00 = character_select::detail::Sub_51BC50(imageNo);
        record.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record.value408 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field++, 767, fieldText));
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldText);
        std::strcpy(record.texts08[0].data(), std::strlen(fieldText) > 16 ? "???" : fieldText);
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field++, 767, fieldText));
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldText);
        std::strcpy(copiedText, fieldText);
        const auto length = static_cast<std::int32_t>(std::strlen(fieldText));
        if (std::strlen(fieldText) > 140) {
            std::strcpy(record.texts08[1].data(), input::LegacyText593850());
            std::strcpy(record.texts08[2].data(), input::LegacyText593850());
            std::strcpy(record.texts08[3].data(), input::LegacyText593850());
        } else {
            std::int32_t position = 0;
            for (std::int32_t line = 1; line < 4; ++line) {
                char* const destination = record.texts08[line].data();
                std::int32_t used = 0;
                for (;;) {
                    const char value = copiedText[position];
                    if (value == '\r') {
                        ++position;
                        break;
                    }
                    ++position;
                    destination[used] = value;
                    if (position > length) {
                        break;
                    }
                    if (++used >= 70) {
                        break;
                    }
                }
                destination[used] = '\0';
                if (input::ClassifyLastAnsiUnit_44B190(destination) == 3) {
                    --position;
                    destination[used - 1] = '\0';
                }
            }
        }
        offset += 0x40C;
    } while (offset < 0x1030);
    return offset;
}

} // namespace detail

} // namespace cg::ui
