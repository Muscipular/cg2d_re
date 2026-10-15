#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

namespace {

// 原580870..580900：解析后的姓名与两行正文，GBK字节容量仅约束输入边界。
struct Window56TextStorage {
    std::wstring name;
    std::array<std::wstring, 2> messages;
};

} // namespace

// 回调与内部函数前置声明。

// 原函数：0x004C5C30。
static std::int32_t WindowAppearance_ButtonOk(std::int32_t index, std::int32_t state); // 图号原文本直接发送，不转义。

// 原函数：0x004C5D00。
static std::int32_t WindowAppearance_CycleClass(std::int32_t index, std::int32_t state); // class按版本上下界循环。

// 原函数：0x004C5DE0。
static std::int32_t WindowAppearance_CycleFifthDigit(std::int32_t index, std::int32_t state); // fifth分量0..4循环。

// 原函数：0x004C5EB0。
static std::int32_t WindowAppearance_CycleOnesDigit(std::int32_t index, std::int32_t state); // ones分量0..4循环。

// 原函数：0x004C5F80。
static std::int32_t WindowAppearance_SendDialogReply(
    std::int32_t button, std::int32_t row, const std::wstring* unusedTexts,
    std::uint8_t unusedInputCount); // 类型37回复，图号转义后发完整button。

// 原函数：0x004C5850。
static std::int32_t WindowAppearance_Update(std::int32_t value);

// 原函数：0x004C5B70。
static std::int32_t WindowAppearance_Draw(std::int32_t value);

// 原函数：0x004C5C00。
static void WindowAppearance_Release();

// 控件载荷、窗口状态、组件表与原型。

static Window56TextStorage gWindow56Texts580870 {L"NPCNAME", {L"MESSAGE", L"MESSAGE"}}; // 原全局：0x00580870。

static std::int32_t gWindow56DialogTypeCB1DE0 = 0; // 原全局：0x00CB1DE0。

static std::int32_t gWindow56RequestCF49C0 = 0; // 原全局：0x00CF49C0。 创建保留，确认请求实际读取共享对话参数。

static std::int32_t gWindow56TargetC8AF60 = 0; // 原全局：0x00C8AF60。

static std::int32_t gWindow56MinimumCB0F80 = 0; // 原全局：0x00CB0F80。

static std::int32_t gWindow56MaximumC8A958 = 0; // 原全局：0x00C8A958。

static std::int32_t gWindow56BaseCB0F8C = 0; // 原全局：0x00CB0F8C。

static std::int32_t gWindow56ClassCC466C = 0; // 原全局：0x00CC466C。

static std::int32_t gWindow56QuarterCC3E88 = 0; // 原全局：0x00CC3E88。

static std::int32_t gWindow56FifthCB0F88 = 0; // 原全局：0x00CB0F88。

static std::int32_t gWindow56OnesCF4680 = 0; // 原全局：0x00CF4680。

// 原D2F7A0..AC四个数值槽；D2F7B0只映射固定240高所能消费的13行。
static std::int32_t gImageDialogTypeD2F7A0 = 0; // 原全局：0x00D2F7A0。

static std::int32_t gImageDialogRequestD2F7A4 = 0; // 原全局：0x00D2F7A4。

static std::int32_t gImageDialogTargetD2F7A8 = 0; // 原全局：0x00D2F7A8。

static std::int32_t gImageDialogImageD2F7AC = 0; // 原全局：0x00D2F7AC。

static std::array<WindowDialogTextLine, 13> gImageDialogLinesD2F7B0 {}; // 原全局：0x00D2F7B0。

static WindowGraphicData gWindowGraphic576EF0 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576EF0。

static WindowGraphicData gWindowGraphic576F08 {30000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F08。

static WindowGraphicData gWindowGraphic576F20 {245042, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F20。

static WindowGraphicData gWindowGraphic576F38 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F38。

static WindowGraphicData gWindowGraphic576F50 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F50。

static WindowTextPrototype gWindowText576F68 {{}, L"請選擇臉部、眼睛及嘴巴。"}; // 原全局：0x00576F68。 // 原常量参考：0x00585D98。

static WindowTextPrototype gWindowText576F70 {{}, L"臉部"}; // 原全局：0x00576F70。 // 原常量参考：0x00585D90。

static WindowTextPrototype gWindowText576F78 {{}, L"眼睛"}; // 原全局：0x00576F78。 // 原常量参考：0x00585D88。

static WindowTextPrototype gWindowText576F80 {{}, L"嘴巴"}; // 原全局：0x00576F80。 // 原常量参考：0x00585D80。

static std::int32_t gWindowAnimationImage576F88 = 100000; // 原全局：0x00576F88。

// 原全局：0x00576F90。
// 原576F90的十五项，三个分量按钮复用源图形但实例各自分配。
static WindowElementPrototype gWindowElements576F90[15] {
    {WindowElementType_Graphic, 197, 206, 66, 17, 1, {.graphic = &gWindowGraphic576EF0}, &WindowAppearance_ButtonOk},
    {WindowElementType_Graphic, 48, 19, 0, 0, 1, {.graphic = &gWindowGraphic576F20}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 48, 19, 0, 0, 1, {.graphic = &gWindowGraphic576F08}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 144, 26, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleClass},
    {WindowElementType_Text, 194, 28, 0, 0, 1, {.text = &gWindowText576F70}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 260, 26, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleClass},
    {WindowElementType_Graphic, 144, 47, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleFifthDigit},
    {WindowElementType_Text, 194, 49, 0, 0, 1, {.text = &gWindowText576F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 260, 47, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleFifthDigit},
    {WindowElementType_Graphic, 144, 68, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleOnesDigit},
    {WindowElementType_Text, 194, 70, 0, 0, 1, {.text = &gWindowText576F80}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 260, 68, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleOnesDigit},
    {WindowElementType_Animation, 390, 120, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage576F88}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 147, 0, 0, 1, {.text = &gWindowText576F68}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 163, 0, 0, 1, {.text = &gWindowText576F68}, &WindowElement_NoOp},
};

// 原型：窗口56；原全局：0x0054ED20。
WindowPrototype gWindowPrototypeAppearance {0, 4, 96, 108, 448, 240, 0x80010101u, 15, gWindowElements576F90, &WindowAppearance_Update, &WindowAppearance_Draw, &WindowAppearance_Release}; // 56: 54ED20

// 窗口函数实现。

// 原函数：0x004C5790。
login::YobiAction* WindowAppearance_Open(
    const std::int32_t request, const std::int32_t target, const char* const text)
{
    gWindow56DialogTypeCB1DE0 = 36;
    gWindow56RequestCF49C0 = request;
    gWindow56TargetC8AF60 = target;
    gShopImageD2DAD0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char name[52];
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        text, '|', 2, 16, name));
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    gWindow56Texts580870.name = cg::text::FromGbkWide(name);
    std::array<char, 3072> buffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 1023, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 2; ++row) {
        char message[46];
        static_cast<void>(input::ExtractLegacyLine_503AB0(
            message, 46, buffer.data(), row, 45));
        gWindow56Texts580870.messages[row] = cg::text::FromGbkWide(message);
    }
    return WindowRuntime_CreateAnimation(WindowId_Appearance, 1, 0, nullptr);
}

// 原函数：0x004C5850。
static std::int32_t WindowAppearance_Update(const std::int32_t value)
{
    if (value != -1) {
        return 1;
    }
    auto* const animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[12].data);
    std::int32_t version = character_select::state::gGraphicVersionOverride58B6A0;
    if (version == 0xFF) {
        version = character_select::state::gPukVersion;
    }
    const std::int32_t converted = character_select::detail::Sub_51BFF0(
        character_select::detail::Sub_51BB80(world::detail::ReadPlayerValue17C()));
    gWindow56MinimumCB0F80 = 0;
    if (converted >= 241400) {
        gWindow56MinimumCB0F80 = 1;
    }
    gWindow56MaximumC8A958 = 0;
    if (version >= 5) {
        gWindow56MaximumC8A958 = 1;
    }
    if (version >= 7) {
        gWindow56MaximumC8A958 = 2;
    }
    if (converted >= 205000 && converted <= 205600) {
        gWindow56MinimumCB0F80 = 0;
        gWindow56MaximumC8A958 = 0;
    }
    if (converted >= 270000 && converted <= 270200) {
        gWindow56MinimumCB0F80 = 0;
        gWindow56MaximumC8A958 = 0;
    }
    static_cast<WindowTextData*>(gCurrentWindow->elements[13].data)->text = gWindow56Texts580870.messages[0];
    static_cast<WindowTextData*>(gCurrentWindow->elements[14].data)->text = gWindow56Texts580870.messages[1];
    for (std::int32_t index = 3; index <= 5; ++index) {
        gCurrentWindow->elements[index].visible = 0;
    }
    if (gWindow56MaximumC8A958 - gWindow56MinimumCB0F80 > 0) {
        for (std::int32_t index = 3; index <= 5; ++index) {
            gCurrentWindow->elements[index].visible = 1;
        }
    }
    for (std::int32_t index = 6; index <= 11; ++index) {
        gCurrentWindow->elements[index].visible = 1;
    }
    // 原门禁读取上次分类；本次玩家图号分解在可见性处理之后。
    const std::int32_t previousBase = gWindow56BaseCB0F8C;
    if (previousBase == 33100 || previousBase == 33101) {
        for (std::int32_t index = 3; index <= 11; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    }
    static_cast<void>(character_select::detail::Sub_4C56E0(
        world::detail::ReadPlayerValue17C(), &gWindow56BaseCB0F8C, &gWindow56ClassCC466C,
        &gWindow56QuarterCC3E88, &gWindow56FifthCB0F88, &gWindow56OnesCF4680));
    const std::int32_t ones = gWindow56OnesCF4680;
    const std::int32_t fifth = gWindow56FifthCB0F88;
    const std::int32_t quarter = gWindow56QuarterCC3E88;
    const std::int32_t imageClass = gWindow56ClassCC466C;
    const std::int32_t base = gWindow56BaseCB0F8C;
    const std::int32_t image = character_select::detail::Sub_4C5650(
        base, imageClass, quarter, fifth, ones);
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[2].data)->imageNo = image;
    login::YobiAction* const action = animation->action;
    login::detail::SetYobiActionField2A8(action, world::detail::ReadPlayerImageNo());
    login::detail::SetYobiActionField2B0(animation->action, 0);
    login::detail::SetYobiActionField2B8(animation->action, 5);
    return 1;
}

// 原函数：0x004C5B70。
static std::int32_t WindowAppearance_Draw(const std::int32_t unused)
{
    static_cast<void>(unused);
    if (WindowProfile_CheckDistance(2, 2) != 0 || world::detail::gValue581800 != gWindow56DialogTypeCB1DE0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    WindowRuntime_DrawCurrent();
    WindowInstance* const window = gCurrentWindow;
    login::detail::DrawNineGrid_49BE00(
        245300, static_cast<std::int16_t>(window->x), static_cast<std::int16_t>(window->y),
        static_cast<std::int16_t>(window->width), static_cast<std::int16_t>(window->height),
        223, -1, std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x004C5C00。
static void WindowAppearance_Release()
{
    if (world::detail::gValue581800 == gWindow56DialogTypeCB1DE0) {
        world::detail::gValue581800 = -1;
        gWindow56DialogTypeCB1DE0 = -1;
    }
}

// 原函数：0x004C5C30。
static std::int32_t WindowAppearance_ButtonOk(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const std::int32_t ones = gWindow56OnesCF4680;
        const std::int32_t fifth = gWindow56FifthCB0F88;
        const std::int32_t quarter = gWindow56QuarterCC3E88;
        const std::int32_t imageClass = gWindow56ClassCC466C;
        const std::int32_t base = gWindow56BaseCB0F8C;
        const std::int32_t image = character_select::detail::Sub_4C5650(
            base, imageClass, quarter, fifth, ones);
        std::array<char, 156> buffer;
        std::sprintf(buffer.data(), "%d", image);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t y =
            world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x =
            world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 1, cg::text::FromGbkWide(buffer.data())));
        gCurrentWindow->state |= 0x8000u;
        consumed = 1;
    }
    graphic->imageNo = 243048;
    if ((state & 1) != 0) {
        graphic->imageNo = 243050;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243049;
    }
    return consumed;
}

// 原函数：0x004C5D00。
static std::int32_t WindowAppearance_CycleClass(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        std::int32_t imageClass = gWindow56ClassCC466C + (index < 4 ? -1 : 1);
        const std::int32_t minimum = gWindow56MinimumCB0F80;
        gWindow56ClassCC466C = imageClass;
        if (imageClass < minimum) {
            imageClass = gWindow56MaximumC8A958;
            gWindow56ClassCC466C = imageClass;
        } else if (imageClass > gWindow56MaximumC8A958) {
            imageClass = minimum;
            gWindow56ClassCC466C = imageClass;
        }
        const std::int32_t ones = gWindow56OnesCF4680;
        const std::int32_t fifth = gWindow56FifthCB0F88;
        const std::int32_t quarter = gWindow56QuarterCC3E88;
        const std::int32_t base = gWindow56BaseCB0F8C;
        const std::int32_t image = character_select::detail::Sub_4C5650(
            base, imageClass, quarter, fifth, ones);
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[2].data)->imageNo = image;
        consumed = 1;
    }
    graphic->imageNo = index < 4 ? 243030 : 243033;
    if ((state & 1) != 0) {
        graphic->imageNo = index < 4 ? 243032 : 243035;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = index < 4 ? 243031 : 243034;
    }
    return consumed;
}

// 原函数：0x004C5DE0。
static std::int32_t WindowAppearance_CycleFifthDigit(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        std::int32_t fifth = gWindow56FifthCB0F88 + (index < 7 ? -1 : 1);
        gWindow56FifthCB0F88 = fifth;
        if (fifth < 0) {
            fifth = 4;
            gWindow56FifthCB0F88 = fifth;
        } else if (fifth >= 5) {
            fifth = 0;
            gWindow56FifthCB0F88 = fifth;
        }
        const std::int32_t ones = gWindow56OnesCF4680;
        const std::int32_t quarter = gWindow56QuarterCC3E88;
        const std::int32_t base = gWindow56BaseCB0F8C;
        const std::int32_t imageClass = gWindow56ClassCC466C;
        const std::int32_t image = character_select::detail::Sub_4C5650(
            base, imageClass, quarter, fifth, ones);
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[2].data)->imageNo = image;
        consumed = 1;
    }
    graphic->imageNo = index < 7 ? 243030 : 243033;
    if ((state & 1) != 0) {
        graphic->imageNo = index < 7 ? 243032 : 243035;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = index < 7 ? 243031 : 243034;
    }
    return consumed;
}

// 原函数：0x004C5EB0。
static std::int32_t WindowAppearance_CycleOnesDigit(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        std::int32_t ones = gWindow56OnesCF4680 + (index < 10 ? -1 : 1);
        gWindow56OnesCF4680 = ones;
        if (ones < 0) {
            ones = 4;
            gWindow56OnesCF4680 = ones;
        } else if (ones >= 5) {
            ones = 0;
            gWindow56OnesCF4680 = ones;
        }
        const std::int32_t quarter = gWindow56QuarterCC3E88;
        const std::int32_t imageClass = gWindow56ClassCC466C;
        const std::int32_t fifth = gWindow56FifthCB0F88;
        const std::int32_t base = gWindow56BaseCB0F8C;
        const std::int32_t image = character_select::detail::Sub_4C5650(
            base, imageClass, quarter, fifth, ones);
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[2].data)->imageNo = image;
        consumed = 1;
    }
    graphic->imageNo = index < 10 ? 243030 : 243033;
    if ((state & 1) != 0) {
        graphic->imageNo = index < 10 ? 243032 : 243035;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = index < 10 ? 243031 : 243034;
    }
    return consumed;
}

// 原函数：0x004C5F80。
static std::int32_t WindowAppearance_SendDialogReply(
    const std::int32_t button, const std::int32_t row, const std::wstring* const unusedTexts,
    const std::uint8_t unusedInputCount)
{
    static_cast<void>(unusedTexts);
    static_cast<void>(unusedInputCount);
    if (button == 0) {
        if (row < 0) {
            world::detail::gValue581800 = -1;
        }
        return 1;
    }
    std::wstring text;
    std::wstring escaped;
    cg::text::FormatTo(text, L"{}", gImageDialogImageD2F7AC);
    escaped = input::EscapeTextAndNotify(text, 255);
    const std::int32_t target = gImageDialogTargetD2F7A8;
    const std::int32_t request = gImageDialogRequestD2F7A4;
    const std::int32_t y =
        world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x =
        world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(network::Sub_512620(
        platform::gConnectionSocket, x, y, request, target, button, escaped));
    world::detail::gValue581800 = -1;
    static_cast<void>(audio::PlaySe_52C450(79, 320));
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 1;
}

// 原函数：0x004C6060。
login::YobiAction* WindowAppearance_OpenDialogPreview(
    const std::int32_t buttons, const std::int32_t request, const std::int32_t target,
    const char* const text)
{
    WindowDialog_Reset();
    gImageDialogTypeD2F7A0 = world::detail::gValue581800;
    gImageDialogRequestD2F7A4 = request;
    gImageDialogTargetD2F7A8 = target;
    gImageDialogImageD2F7AC = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    std::array<char, 3000> buffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 1000, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    const std::int32_t columns = login::detail::Sub_49C5C0(448);
    const std::int32_t count = login::detail::Sub_49C5E0(240, buttons, 16);
    gImageDialogLinesD2F7B0[0].text.clear();
    gImageDialogLinesD2F7B0[1].text.clear();
    gImageDialogLinesD2F7B0[2].text.clear();
    for (std::int32_t row = 3; row < count; ++row) {
        std::wstring& destination = gImageDialogLinesD2F7B0[row].text;
        std::wstring suffix;
        static_cast<void>(input::ExtractGbkTextLine(suffix, buffer.data(), row - 3, columns - 14));
        destination.assign(14, L' ');
        destination.append(suffix);
    }
    WindowDialog_AddImage(1, gImageDialogImageD2F7AC, 0, 86, 98);
    return WindowDialog_Open(
        96, 108, 448, 240, 245300, buttons, gImageDialogLinesD2F7B0.data(),
        count, 16, 0, 0, nullptr, columns, 2, 0, &WindowAppearance_SendDialogReply, nullptr, 1, 0);
}

} // namespace detail

} // namespace cg::ui
