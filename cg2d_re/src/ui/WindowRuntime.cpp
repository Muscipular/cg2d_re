#include "WindowRuntimeState.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../LoginSceneDependencies.hpp"
#include "../platform/WindowProcedureState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace cg::ui {
namespace detail {

std::array<WindowInstance*, kWindowCount> gWindows {};
std::array<WindowState, kWindowCount> gWindowStates {};
WindowInstance* gCurrentWindow = nullptr;
WindowState* gCurrentWindowState = nullptr;
std::uint8_t gValueD67BE0 = 0;
std::int32_t gDragItemIndex584DA4 = -1;
std::int32_t gDragImageNoD67D24 = 0;
std::int32_t gItemDragXCC4880 = 0;
std::int32_t gItemDragYCC4884 = 0;
std::int32_t gDraggingCA0B28 = 0;
std::int32_t gValue58589C = 1;
void* gFocusedElementDataD45F94 = nullptr;
std::int32_t gMouseValue57C110 = 0;
std::int32_t gDragSourceIndex57C114 = 0;
WindowDragCallback gDragCallback57C118 = nullptr;
std::int16_t gDragX57C11C = 0;
std::int16_t gDragY57C11E = 0;
std::int32_t gFocusWindowId57C120 = 0;
std::int32_t gFocusElementIndex57C124 = -1;
std::int32_t gMousePhase57C128 = 0;
std::int32_t gCurrentElementIndex57C12C = -1;
std::int32_t gCurrentWindowIdD67A1C = 0;
std::uint32_t gWindowHasFocusCA0AB4 = 0;
WindowInputPrototype gChatInputPrototype57E970 {
    {{&input::gInputContext626AA0}, WindowInputReferenceKind::Context},
    8, 5, 3, 9, 0, {}, 1, 76, 0, 0,
};
std::int32_t gChatWindowModeD66E04 = 0;
std::uint8_t gValue5F9754 = 0;
std::int32_t gChatWidthUnitsC9A930 = 0;
std::int32_t gChatWidthRequestC9A934 = 0;
std::int32_t gChatHeightUnitsC9A938 = 0;
std::int32_t gChatHeightRequestC9A93C = 0;
std::int32_t gChatScrollOffsetC9A940 = 0;
std::array<char, 2 * 20 * 0xC4C> gTradeItemsD460D0 {};
std::array<char, 2 * 5 * 0x5FC> gTradePetsC9C108 {};
std::array<char, 2 * 0xFBF8> gItemDetailStorageCC4D58 {};
static_assert(sizeof(gTradeItemsD460D0) == 0x1EBE0);
static_assert(sizeof(gTradePetsC9C108) == 0x3BD8);
static_assert(sizeof(gItemDetailStorageCC4D58) == 0x1F7F0);
static_assert(0x60 + 10 * 0x8C == 0x5D8);
std::int32_t gTradeValueCC4224 = 0;
std::int32_t gTradeValueCF4688 = 0;
std::int32_t gTradeValueD046D0 = 0;
std::int32_t gTradeValueD2E2B0 = 0;
std::uint8_t gTradeValueD67B80 = 0;

namespace {

// 交易记录仍是原字节区，窄数值通过复制写入，不对未对齐地址构造数值T*。
template <typename T>
void WriteTradeValue(char* const record, const std::size_t offset, const T value)
{
    static_assert(sizeof(T) == 2 || sizeof(T) == 4);
    std::memcpy(record + offset, &value, sizeof(value));
}

// 原54EFE8的71项S8类别，与窗口编号一一对应。
constexpr std::array<std::int8_t, kWindowCount> kWindowCategories {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 0, 1, 2, 2, 0, 1, 2, 4, 2, 2, 2, 2, 2,
    1, 1, 2, 4, 2, 2, 2, 1, 2, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 2, 2, 2,
};

constexpr std::array<std::int32_t, 52> kNumberDigitImages54F0D8 {
    243300, 243310, 243320, 243330, 243340, 243350, 243360,
    243370, 243380, 243390, 245500, 245500, 245500,
    243500, 243510, 243520, 243530, 243540, 243550, 243560,
    243570, 243580, 243590, 243500, 243500, 243500,
    243700, 243710, 243720, 243730, 243740, 243750, 243760,
    243770, 243780, 243790, 243700, 243700, 243700,
    245520, 245520, 245520, 245520, 245520, 245520, 245560,
    245580, 245520, 245520, 245540, 245540, 245540,
};

constexpr std::array<std::int32_t, 52> kNumberSymbolImages54F1A8 {
    243400, 243410, 243420, 243430, 243440, 243450, 243460,
    243470, 243480, 243490, 245510, 245510, 245510,
    243600, 243610, 243620, 243630, 243640, 243650, 243660,
    243670, 243680, 243690, 243600, 243600, 243600,
    243800, 243810, 243820, 243830, 243840, 243850, 243860,
    243870, 243880, 243890, 243800, 243800, 243800,
    245530, 245530, 245530, 245530, 245530, 245530, 245570,
    245580, 245530, 245530, 245550, 245550, 245550,
};

constexpr std::array<std::int32_t, 12> kNumberOffsets54F278 {
    2, 2, 5, 5, 4, 8, 5, 5, 8, 2, 3, 5,
};
constexpr std::array<std::int32_t, 4> kNumberSpacingDeductions54F2A8 {2, 2, 2, 1};
std::array<std::int32_t, 4> gNumberAdvancesD67AC4 {};

// 元素事件与阶段推进共用映射，按原edge/held/旧edge/pulse顺序采样与合并。
std::int32_t AppendMouseButtonEvents(std::int32_t events)
{
    const auto edge = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
    if ((edge & 1u) != 0) {
        events |= 0x02;
    }
    if ((edge & 2u) != 0) {
        events |= 0x04;
    }
    const auto held = static_cast<std::uint8_t>(platform::gLegacyMouseState.heldButtons);
    if ((held & 1u) != 0) {
        events |= 0x08;
    }
    if ((held & 2u) != 0) {
        events |= 0x10;
    }
    if ((edge & 0x10u) != 0) {
        events |= 0x100;
    }
    if ((edge & 0x20u) != 0) {
        events |= 0x200;
    }
    const auto pulse = static_cast<std::uint8_t>(platform::gLegacyMouseState.pulseButtons);
    if ((pulse & 1u) != 0) {
        events |= 0x400;
    }
    if ((pulse & 2u) != 0) {
        events |= 0x800;
    }
    return events;
}

// 原5FB7F4是最后一条输入上下文的尾槽，不是日志511的font。
std::int32_t& PreviousChatFont(const std::int32_t index)
{
    if (index == 0) {
        return input::gInputContexts5F9758[23].field158;
    }
    return world::detail::gRecords5FB7F8[index - 1].fontType;
}

// 两个原入口只在宽度来源上不同；测量IME及取宽度仍由各入口按序执行。
void ScrollInputAndDraw(input::InputContext* const context, const std::int32_t width)
{
    const std::int32_t caret = context->caretByteOffset;
    const std::int32_t offset = context->field158;
    context->field154 = width;
    if (caret < offset) {
        context->field158 = caret;
    } else {
        for (;;) {
            const std::int32_t currentOffset = context->field158;
            const std::int32_t length = context->caretByteOffset - currentOffset;
            if (length < 0) {
                break;
            }
            const std::int32_t textWidth = render::Font::measure(
                context->text + currentOffset, context->field13C, length);
            if (context->field154 >= textWidth) {
                break;
            }
            const std::uint8_t value = static_cast<std::uint8_t>(
                context->text[context->field158]);
            context->field158 += (value & 0x80u) != 0 ? 2 : 1;
        }
    }
    render::DrawInputText_4568A0(context);
}

} // namespace

std::int32_t Sub_4FB8F0()
{
    gDragItemIndex584DA4 = -1;
    gDragImageNoD67D24 = 0;
    gItemDragXCC4880 = 0;
    gItemDragYCC4884 = 0;
    gDraggingCA0B28 = 0;
    gValue58589C = 1;
    return 0;
}

std::int32_t Sub_4FB920(const std::int32_t index)
{
    const std::int32_t previous = gDragItemIndex584DA4;
    if (previous >= 0 && previous == index) {
        return Sub_4FB8F0();
    }
    return previous;
}

void Sub_4E54E0(const std::int32_t side, const std::int32_t index)
{
    if (index < 0 || index >= 20) {
        return;
    }
    char* const record = gTradeItemsD460D0.data() + (index + side * 20) * 0xC4C;
    std::memset(record, 0, 0xC4C);
    const world::detail::PlayerItemRecord& source = world::detail::gPlayerItemsFDA8A4[index + 8];
    if (source.used == 0) {
        return;
    }
    const std::int32_t imageNo = source.imageNo;
    const std::int32_t count = source.count;
    WriteTradeValue(record, 0, std::uint16_t{1});
    const std::uint16_t flags = source.flags;
    WriteTradeValue(record, 4, imageNo);
    const std::int32_t itemId = source.itemId;
    WriteTradeValue(record, 8, count);
    WriteTradeValue(record, 0x0C, itemId);
    WriteTradeValue(record, 0xC34,
        static_cast<std::int32_t>(std::bit_cast<std::int16_t>(flags)));
    if (std::strlen(source.memo.data()) != 0) {
        std::strcpy(record + 0xC38, source.memo.data());
    }
    std::strcpy(record + 0x10, source.name.data());
    if ((flags & 1u) == 0) {
        WriteTradeValue(record, 2, std::uint16_t{0xFFFF});
    }
}

void Sub_4E5600(const std::int32_t side)
{
    for (std::int32_t index = 0; index < 20; ++index) {
        Sub_4E54E0(side, index);
    }
}

void Sub_4E0100(const std::int32_t side, const std::int32_t index)
{
    if (index < 0 || index >= 20) {
        return;
    }
    const world::detail::PlayerItemRecord& source = world::detail::gPlayerItemsFDA8A4[index + 8];
    const bool used = source.used != 0;
    char* const record = gItemDetailStorageCC4D58.data() + side * 0xFBF8 + index * 0xC44;
    std::memset(record, 0, 0xC44);
    if (!used) {
        return;
    }
    const std::int32_t imageNo = source.imageNo;
    const std::int32_t count = source.count;
    WriteTradeValue(record, 0, std::uint16_t{1});
    WriteTradeValue(record, 4, imageNo);
    WriteTradeValue(record, 8, count);
    const std::uint16_t flags = source.flags;
    WriteTradeValue(record, 0xC40,
        static_cast<std::int32_t>(std::bit_cast<std::int16_t>(flags)));
    if (std::strlen(source.memo.data()) != 0) {
        std::strcpy(record + 0xC29, source.memo.data());
    }
    std::strcpy(record + 0x0C, source.name.data());
    for (std::int32_t line = 0; line < 32; ++line) {
        std::strcpy(record + 0x29 + line * 96, source.description[line].data());
    }
    WriteTradeValue(record, 0xC3C, source.pages);
    if ((flags & 0x60u) != 0) {
        WriteTradeValue(record, 2, std::uint16_t{0xFFFF});
    }
}

void Sub_4E52B0(const std::int32_t side)
{
    for (std::int32_t index = 0; index < 5; ++index) {
        world::detail::PetRuntimeRecord& source = world::detail::gPetRuntimeF633F8[index];
        if (source.used == 0) {
            continue;
        }
        char* const record = gTradePetsC9C108.data() + (index + side * 5) * 0x5FC;
        const bool hasUserName = source.name6BD[0] != '\0';
        WriteTradeValue(record, 0, std::uint16_t{1});
        std::strcpy(record + 0x5D8,
            hasUserName ? source.name6BD.data() : source.name6AC.data());
        const std::int32_t imageNo = source.imageNo6A0;
        const std::int32_t value08 = source.value08;
        std::strcpy(record + 0x5E9, source.name6AC.data());
        WriteTradeValue(record, 4, source.value04);
        WriteTradeValue(record, 8, imageNo);
        WriteTradeValue(record, 0x0C, value08);
        for (std::int32_t value = 0; value < 3; ++value) {
            WriteTradeValue(record, 0x10 + value * 4,
                platform::legacy_crypt_int::Read_401EC0(&source.values0C[value]));
        }
        const std::int32_t fourth =
            platform::legacy_crypt_int::Read_401EC0(&source.values0C[3]);
        std::int32_t first = source.values70[5];
        std::int32_t second = source.values70[6];
        WriteTradeValue(record, 0x1C, fourth);
        std::int32_t third = source.values70[7];
        WriteTradeValue(record, 0x38, first);
        first = source.values70[8];
        WriteTradeValue(record, 0x3C, second);
        second = source.values70[9];
        WriteTradeValue(record, 0x40, third);
        third = source.values70[0];
        WriteTradeValue(record, 0x44, first);
        first = source.values70[1];
        WriteTradeValue(record, 0x48, second);
        second = source.values70[2];
        WriteTradeValue(record, 0x20, third);
        third = source.values70[3];
        WriteTradeValue(record, 0x24, first);
        first = source.values70[4];
        WriteTradeValue(record, 0x28, second);
        second = source.values70[10];
        WriteTradeValue(record, 0x2C, third);
        third = source.value6A4;
        WriteTradeValue(record, 0x30, first);
        WriteTradeValue(record, 0x34, second);
        WriteTradeValue(record, 0x5C, third);
        for (std::int32_t value = 0; value < 4; ++value) {
            WriteTradeValue(record, 0x4C + value * 4, source.values70[21 + value]);
        }
        for (std::int32_t skill = 0; skill < 10; ++skill) {
            const std::int16_t sourceIndex =
                std::bit_cast<std::int16_t>(source.techniquePairs650[skill].index);
            std::memcpy(
                record + 0x60 + skill * 0x8C,
                &source.techniquesD8[sourceIndex], sizeof(world::detail::PetTechniqueRecord));
        }
    }
}

std::int32_t Sub_4E5710(const std::int32_t side)
{
    if (side == 0) {
        gTradeValueCF4688 = 0;
    } else {
        gTradeValueD046D0 = 0;
    }
    std::memset(gTradeItemsD460D0.data() + side * 0xF5F0, 0, 0xF5F0);
    std::memset(gTradePetsC9C108.data() + side * 0x1DEC, 0, 0x1DEC);
    return 0;
}

void Sub_4E5770(
    const std::int32_t index, const std::int32_t side, const char* const text)
{
    char* const record = gTradeItemsD460D0.data() + (index + side * 20) * 0xC4C;
    char* const rows = record + 0x2D;
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char* const row = rows + line * 96;
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            last = line;
        } else {
            row[0] = '\0';
        }
    }
    // 原回扫从最后成功行的前一行开始，不检查最后成功行本身。
    while (last > 0 && std::strlen(rows + (last - 1) * 96) == 0) {
        --last;
    }
    WriteTradeValue(record, 0xC30, last < 0 ? std::int32_t{0} : last / 8 + 1);
}

std::int32_t Sub_4E5830(const char* const text)
{
    char name[256];
    char memo[256];
    char description[768];
    std::int32_t field = 1;
    std::int32_t index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
    std::int32_t status =
        network::ExtractDelimitedField_52CED0(text, '|', field + 1, 255, name);
    while (status != 1) {
        char* const record = gTradeItemsD460D0.data() + index * 0xC4C;
        std::memset(record, 0, 0xC4C);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        WriteTradeValue(record, 0, std::uint16_t{1});
        if (std::strlen(name) <= 28) {
            std::strcpy(record + 0x10, name);
        }
        network::ExtractDelimitedField_52CED0(text, '|', field + 3, 767, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        Sub_4E5770(index, 0, description);
        network::ExtractDelimitedField_52CED0(text, '|', field + 13, 255, memo);
        input::UnescapeLegacyTextAndNotify_52D2F0(memo);
        if (std::strlen(memo) <= 16) {
            std::strcpy(record + 0xC38, memo);
        }
        WriteTradeValue(record, 4,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 4));
        WriteTradeValue(record, 0x0C,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 10));
        WriteTradeValue(record, 8,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 12));
        WriteTradeValue(record, 0xC34,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 9));
        field += 14;
        index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        status = network::ExtractDelimitedField_52CED0(text, '|', field + 1, 255, name);
    }
    return status;
}

std::int32_t Sub_4E5A40(const std::int32_t index, const char* const text)
{
    char* const record = gTradePetsC9C108.data() + index * 0x5FC;
    std::memset(record, 0, 0x5FC);
    char name[256];
    std::int32_t result =
        network::ExtractDelimitedField_52CED0(text, '|', 1, 255, name);
    if (result != 0) {
        return result;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    std::strcpy(record + 0x5D8, std::strlen(name) <= 16 ? name : "???");
    result = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    WriteTradeValue(record, 0x0C, result);
    if (result < 0) {
        return result;
    }
    WriteTradeValue(record, 0, std::uint16_t{1});
    WriteTradeValue(record, 4, static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(text, '|', 3)));
    WriteTradeValue(record, 0x14, network::ParseDelimitedInteger_52CFA0(text, '|', 4));
    for (std::int32_t field = 5; field <= 11; ++field) {
        WriteTradeValue(record, 0x1C + (field - 5) * 4,
            network::ParseDelimitedInteger_52CFA0(text, '|', field));
    }
    WriteTradeValue(record, 0x5C, network::ParseDelimitedInteger_52CFA0(text, '|', 12));
    network::ExtractDelimitedField_52CED0(text, '|', 13, 255, name);
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    std::strcpy(record + 0x5E9, std::strlen(name) <= 16 ? name : "???");
    WriteTradeValue(record, 8, network::ParseDelimitedInteger_52CFA0(text, '|', 14));
    for (std::int32_t field = 15; field <= 23; ++field) {
        result = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        WriteTradeValue(record, 0x38 + (field - 15) * 4, result);
    }
    return result;
}

std::int32_t Sub_4E5CD0(const std::int32_t index, const char* const text)
{
    if (index < 0 || index >= 5) {
        return index;
    }
    char* const record = gTradePetsC9C108.data() + index * 0x5FC;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return index;
    }
    char* skill = record + 0x60;
    std::memset(skill, 0, 0x8C);
    char name[256];
    for (std::int32_t field = 1; field <= 10; ++field) {
        const std::int32_t status =
            network::ExtractDelimitedField_52CED0(text, '|', field, 255, name);
        if (status == 1) {
            return status;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::size_t length = std::strlen(name);
        WriteTradeValue(skill, 0, std::uint16_t{1});
        if (length <= 24) {
            std::strcpy(skill + 6, name);
        }
        skill += 0x8C;
    }
    return 10;
}

std::int32_t Sub_503CB0()
{
    const std::int32_t decreased = platform::gValue73ECBC - 1;
    platform::gValue73ECBC = decreased;
    if (decreased < 0) {
        platform::gValue73ECBC = 2;
    }
    return decreased;
}

std::int32_t Sub_503CD0()
{
    const auto mode = static_cast<std::int16_t>(platform::gValue5F974C + 1);
    platform::gValue5F974C = mode;
    if (mode >= 7) {
        platform::gValue5F974C = 0;
    }
    static_cast<void>(login::detail::Sub_520590());
    return login::detail::Sub_520200();
}

std::int32_t Sub_4A6D80()
{
    platform::gValue554640 = 1;
    gChatScrollOffsetC9A940 = 0;
    world::detail::gCountC9A944 = 0;
    gChatWidthRequestC9A934 = 0;
    gChatHeightRequestC9A93C = 0;
    input::gInputContext626AA0.field140 = 0;
    return 0;
}

std::int32_t Sub_4A6DB0()
{
    gChatScrollOffsetC9A940 = 0;
    gChatWidthRequestC9A934 = 0;
    world::detail::gCountC9A944 = 0;
    gChatHeightRequestC9A93C = 0;
    return 0;
}

std::int32_t Sub_4A7700()
{
    world::detail::gCountC9A944 = 0;
    gChatScrollOffsetC9A940 = 0;
    return 0;
}

std::int32_t Sub_44B170()
{
    for (auto& record : world::detail::gRecords5FB7F8) {
        record.text[0] = '\0';
    }
    return Sub_4A7700();
}

void Sub_4A7710()
{
    gChatHeightRequestC9A93C = 1;
}

void Sub_4A7720()
{
    gChatHeightRequestC9A93C = -1;
}

void Sub_4A7730()
{
    gChatWidthRequestC9A934 = 1;
}

void Sub_4A7740()
{
    gChatWidthRequestC9A934 = -1;
}

std::int32_t Sub_4A82E0()
{
    const std::int32_t height = gChatHeightUnitsC9A938 + gChatHeightRequestC9A93C;
    gChatHeightUnitsC9A938 = height;
    if (height > 26) {
        gChatHeightUnitsC9A938 = 4;
    } else if (height < 4) {
        gChatHeightUnitsC9A938 = 26;
    }
    gChatHeightRequestC9A93C = 0;
    gChatWidthRequestC9A934 = 0;
    return 0;
}

void Sub_4A7590(input::InputContext* const context)
{
    const std::int32_t font = input::gCurrentInputData->field13C;
    static_cast<void>(login::detail::TextMeasure(platform::gImeComposition, font));
    const std::int32_t width = render::gFontMetrics[9].doubleWidth * gChatWidthUnitsC9A930;
    ScrollInputAndDraw(context, width);
}

void Sub_4A7640(input::InputContext* const context)
{
    const std::int32_t font = input::gCurrentInputData->field13C;
    static_cast<void>(login::detail::TextMeasure(platform::gImeComposition, font));
    ScrollInputAndDraw(context, 1024);
}

void Sub_4A7750(const std::int32_t x, const std::int32_t y)
{
    std::int32_t totalHeight = 0;
    const std::int32_t width = gChatWindowModeD66E04 == 1
        ? render::gFontMetrics[9].doubleWidth * 44
        : (gChatWidthUnitsC9A930 + 7) * render::gFontMetrics[9].doubleWidth;
    std::int32_t logIndex = world::detail::gWriteIndex626C08;
    logIndex -= gChatScrollOffsetC9A940;
    --logIndex;
    if (logIndex < 0) {
        logIndex = 511;
    }
    for (auto& record : world::detail::gRecords5FB7F8) {
        if (record.fontType > 14 || PreviousChatFont(logIndex) < 0) {
            record.fontType = 0;
        }
    }
    std::int32_t drawY = y - render::gFontMetrics[
        world::detail::gRecords5FB7F8[logIndex].fontType].doubleHeight;
    std::int32_t processedRecords = 0;
    char scratch[0xC70];
    char* const rows = scratch + 0x124;
    static_assert(0x124 + 10 * 0x121 + 2 == sizeof(scratch));
    for (;;) {
        auto& source = world::detail::gRecords5FB7F8[logIndex];
        if (source.text[0] == '\0') {
            return;
        }
        std::strcpy(scratch, source.text);
        const auto textLength = static_cast<std::int32_t>(std::strlen(scratch));
        std::int32_t* fontSlot = &source.fontType;
        std::int32_t lineCount = login::detail::TextMeasure(scratch, *fontSlot) / width;
        if (login::detail::TextMeasure(scratch, *fontSlot) % width != 0) {
            ++lineCount;
        }

        std::int32_t sourceOffset = 0;
        for (std::int32_t lineIndex = 0; lineIndex < lineCount; ++lineIndex) {
            std::int32_t column = 0;
            if (textLength > 0) {
                std::int32_t rowOffset = lineIndex * 0x121;
                for (;;) {
                    char* const outputByte = rows + rowOffset + column;
                    *outputByte = scratch[sourceOffset];
                    outputByte[1] = '\0';
                    outputByte[2] = '\0';
                    if (logIndex > 512) {
                        logIndex = world::detail::gWriteIndex626C08;
                        fontSlot = &world::detail::gRecords5FB7F8[logIndex].fontType;
                    }
                    char* const currentRow = rows + rowOffset;
                    const std::int32_t rowWidth =
                        login::detail::TextMeasure(currentRow, *fontSlot);
                    if (width < rowWidth) {
                        if (input::ClassifyLastAnsiUnit_44B190(currentRow) == 3) {
                            *outputByte = '\0';
                            --sourceOffset;
                        }
                        rowOffset += 0x121;
                        ++lineIndex;
                        column = -1;
                    }
                    ++sourceOffset;
                    if (sourceOffset > textLength) {
                        break;
                    }
                    ++column;
                    if (column >= textLength) {
                        break;
                    }
                }
            }
            if (sourceOffset > textLength) {
                break;
            }
        }

        if (lineCount > 0) {
            auto& record = world::detail::gRecords5FB7F8[logIndex];
            const char* row = rows + (lineCount - 1) * 0x121;
            for (std::int32_t line = 0; line < lineCount; ++line) {
                const std::int32_t font = record.fontType;
                totalHeight += render::gFontMetrics[font].doubleHeight;
                const std::int32_t heightLimit =
                    render::gFontMetrics[9].doubleHeight * gChatHeightUnitsC9A938;
                if (totalHeight > heightLimit) {
                    return;
                }
                if (*row != '\0') {
                    const std::int32_t color = record.color;
                    static_cast<void>(render::DrawText2_456670(
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                        3,
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(font)),
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(color)),
                        row, 0));
                }

                std::int32_t spacingFont;
                if (line + 1 == lineCount) {
                    const std::int32_t previous = PreviousChatFont(logIndex);
                    if (previous > 14 || previous < 0) {
                        PreviousChatFont(logIndex) = 0;
                    }
                    spacingFont = PreviousChatFont(logIndex);
                } else {
                    if (record.fontType > 14 || PreviousChatFont(logIndex) < 0) {
                        record.fontType = 0;
                    }
                    spacingFont = record.fontType;
                }
                row -= 0x121;
                drawY -= render::gFontMetrics[spacingFont].doubleHeight;
            }
        }

        --logIndex;
        ++processedRecords;
        if (logIndex < 0) {
            logIndex = 511;
        }
        if (processedRecords >= world::detail::gCountC9A944) {
            return;
        }
    }
}

void Sub_4771B0(
    const std::int32_t x, const std::int32_t y, const char* const text,
    const std::int8_t style, const std::int8_t color,
    const std::uint8_t flags, const std::uint8_t layer)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFFu;
    descriptor.bytes[9] = 0xFFu;
    descriptor.bytes[10] = 0xFFu;
    descriptor.bytes[11] = 0xFFu;
    descriptor.bytes[13] = 4;

    std::int32_t drawX;
    std::int32_t drawY;
    std::int32_t step;
    const bool reverse = (flags & 1u) != 0;
    if (reverse) {
        drawX = x - kNumberOffsets54F278[style * 3 + 1];
        drawY = kNumberOffsets54F278[style * 3 + 2];
        step = -gNumberAdvancesD67AC4[style];
        drawY += y;
    } else {
        step = gNumberAdvancesD67AC4[style];
        drawX = kNumberOffsets54F278[style * 3] + x;
        drawY = kNumberOffsets54F278[style * 3 + 2] + y;
    }

    const auto drawByte = [&](const char character) {
        std::int32_t imageNo;
        if (character >= '0' && character <= '9') {
            imageNo = kNumberDigitImages54F0D8[style * 13 + color] + character - '0';
        } else {
            std::int32_t symbol;
            switch (character) {
            case '+': symbol = 0; break;
            case '-': symbol = 1; break;
            case ':': symbol = 2; break;
            case '/': symbol = 3; break;
            default: return;
            }
            imageNo = kNumberSymbolImages54F1A8[style * 13 + color] + symbol;
        }
        static_cast<void>(login::detail::DrawGraphic(
            drawX, drawY, layer, imageNo, 0, &descriptor));
    };

    const char* cursor = text;
    if (reverse) {
        while (*cursor != '\0') {
            ++cursor;
        }
        while (cursor != text) {
            --cursor;
            drawByte(*cursor);
            drawX += step;
        }
    } else {
        while (*cursor != '\0') {
            drawByte(*cursor);
            drawX += step;
            ++cursor;
        }
    }
}

std::int32_t Sub_477460(const char* const text, const std::int8_t style)
{
    const auto length = static_cast<std::int32_t>(std::strlen(text));
    if (length <= 0) {
        return 0;
    }
    return gNumberAdvancesD67AC4[style] * length - kNumberSpacingDeductions54F2A8[style];
}

std::int32_t Sub_46FBC0(const std::int32_t)
{
    DrawCurrentWindow_4729B0();
    return 1;
}

std::int32_t Sub_49E750(const std::int32_t value)
{
    if (value == -1) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
        const char* const text = world::ReadText9DAD30();
        std::memcpy(data->text, text, std::strlen(text) + 1);
        const std::int32_t textWidth = login::detail::TextMeasure(text, 0);
        WindowInstance* const window = gCurrentWindow;
        window->elements[0].x = (window->width - textWidth) >> 1;
    } else {
        const std::uint32_t tick = platform::ReadTickCount32();
        if (world::detail::gValue62B5B0 + 2000u <= tick) {
            gCurrentWindow->state |= 0x8000u;
            world::detail::gValue62B5B0 = 0;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    return 1;
}

std::int32_t Sub_49BDB0(const std::int32_t)
{
    DrawCurrentWindow_4729B0();
    WindowInstance* const window = gCurrentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    login::detail::DrawNineGrid_49BE00(
        0x3BE3E, x, y, width, height, 0xDF, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    render::ClearTextQueueTail_456D50(3);
    return 1;
}

std::int32_t Sub_472F00(
    const std::int32_t left, const std::int32_t top,
    const std::int32_t width, const std::int32_t height)
{
    const std::int32_t x = platform::legacy_crypt_int::Subtract_401E60(
        &platform::gLegacyMouseState.mouseX, left);
    if (x < 0 || x >= width) {
        return 0;
    }
    const std::int32_t y = platform::legacy_crypt_int::Subtract_401E60(
        &platform::gLegacyMouseState.mouseY, top);
    return y >= 0 && y < height ? 1 : 0;
}

std::int32_t Sub_4763E0()
{
    gFocusElementIndex57C124 = -1;
    gMouseValue57C110 = 0;
    gFocusWindowId57C120 = 0;
    gDragCallback57C118 = nullptr;
    gMousePhase57C128 = 0;
    return 0;
}

std::int32_t Sub_472D20()
{
    const std::int32_t initialPhase = gMousePhase57C128;
    WindowInstance* window = gCurrentWindow;
    std::int32_t result = 0;
    if (initialPhase != 1 && (window->state & 2u) == 0) {
        const std::uint32_t state = window->state;
        if ((state & 0x800u) != 0) {
            if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.heldButtons) & 1u) != 0) {
                std::int32_t* const x = &window->x;
                const std::int32_t deltaX = platform::legacy_crypt_int::Subtract_401E60(
                    &platform::gLegacyMouseState.mouseX, window->dragMouseX1C);
                *x += deltaX;
                window = gCurrentWindow;
                std::int32_t* const y = &window->y;
                const std::int32_t deltaY = platform::legacy_crypt_int::Subtract_401E60(
                    &platform::gLegacyMouseState.mouseY, window->dragMouseY20);
                *y += deltaY;
                const std::int32_t mouseX = platform::legacy_crypt_int::Read_401EC0(
                    &platform::gLegacyMouseState.mouseX);
                gCurrentWindow->dragMouseX1C = mouseX;
                const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(
                    &platform::gLegacyMouseState.mouseY);
                gCurrentWindow->dragMouseY20 = mouseY;
            } else {
                window->state = state & ~std::uint32_t{0x800};
            }
            result = 2;
            window = gCurrentWindow;
        } else if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
            for (std::int32_t index = 0; index < window->secondaryCount; ++index) {
                const WindowDragRect& rect = window->dragRects40[index];
                const std::int32_t left = window->x + rect.x;
                const std::int32_t top = window->y + rect.y;
                const std::int32_t height = rect.height;
                const std::int32_t width = rect.width;
                if (gWindowHasFocusCA0AB4 == 0) {
                    if (Sub_472F00(left, top, width, height) != 0) {
                        const std::int32_t mouseX = platform::legacy_crypt_int::Read_401EC0(
                            &platform::gLegacyMouseState.mouseX);
                        gCurrentWindow->dragMouseX1C = mouseX;
                        const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(
                            &platform::gLegacyMouseState.mouseY);
                        gCurrentWindow->dragMouseY20 = mouseY;
                        gCurrentWindow->state |= 0x800u;
                        result |= 2;
                    }
                    window = gCurrentWindow;
                }
            }
        }
    }
    if ((window->state & 1u) == 0) {
        const std::int32_t phase = gMousePhase57C128;
        if (phase == 1 || phase == 4) {
            if (Sub_472F00(window->x, window->y, window->width, window->height) != 0) {
                result |= 1;
            }
        } else if (gWindowHasFocusCA0AB4 == 0) {
            if (Sub_472F00(window->x, window->y, window->width, window->height) != 0) {
                result |= 1;
                if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
                    gCurrentWindow->state |= 0x2000u;
                }
            }
        }
    }
    return result;
}

std::int32_t Sub_4730A0()
{
    std::int32_t count = 0;
    for (std::size_t index = 0; index < kWindowCount;) {
        WindowInstance* const window = gWindows[index];
        if (window == nullptr) {
            std::size_t source = index;
            while (source < kWindowCount && gWindows[source] == nullptr) {
                ++source;
            }
            if (source == kWindowCount) {
                break;
            }
            gWindows[index] = gWindows[source];
            gWindows[source] = nullptr;
            continue;
        }
        if ((window->state & 0x8000u) != 0) {
            FreeWindow_471270(window);
            for (std::size_t target = index; target + 1 < kWindowCount; ++target) {
                gWindows[target] = gWindows[target + 1];
            }
            gWindows[kWindowCount - 1] = nullptr;
            if (index + 1 != kWindowCount) {
                continue;
            }
        } else {
            if ((window->state & 0x2000u) != 0) {
                std::size_t target = index;
                while (target > 0) {
                    WindowInstance* const previous = gWindows[target - 1];
                    if (previous->layer < window->layer) {
                        break;
                    }
                    gWindows[target] = previous;
                    --target;
                }
                gWindows[target] = window;
                window->state &= ~std::uint32_t{0x2000};
            }
            ++count;
        }
        ++index;
    }
    return count;
}

void Sub_473E40()
{
    std::int32_t phase = gMousePhase57C128;
    if (phase == 1) {
        const WindowDragCallback callback = gDragCallback57C118;
        if (callback == nullptr) {
            gWindowHasFocusCA0AB4 = 1;
            *reinterpret_cast<unsigned char*>(&platform::gLegacyMouseState.valueD68DB4) = 5;
            return;
        }
        const std::int32_t events = AppendMouseButtonEvents(0);
        callback(0, events, gDragSourceIndex57C114);
        phase = gMousePhase57C128;
    } else if (phase == 2) {
        gMousePhase57C128 = 0;
        return;
    }
    switch (phase) {
    case 3:
        gMousePhase57C128 = 4;
        gWindowHasFocusCA0AB4 = 1;
        *reinterpret_cast<unsigned char*>(&platform::gLegacyMouseState.valueD68DB4) = 5;
        return;
    case 4: {
        const WindowDragCallback callback = gDragCallback57C118;
        if (callback != nullptr) {
            callback(2, 0, gDragSourceIndex57C114);
        }
        gMouseValue57C110 = 0;
        gDragSourceIndex57C114 = 0;
        gDragCallback57C118 = nullptr;
        gDragX57C11C = 0;
        gDragY57C11E = 0;
        gFocusWindowId57C120 = 0;
        gMousePhase57C128 = 0;
        gFocusElementIndex57C124 = -1;
        return;
    }
    case 6:
        gMousePhase57C128 = 0;
        return;
    case 1:
        gWindowHasFocusCA0AB4 = 1;
        *reinterpret_cast<unsigned char*>(&platform::gLegacyMouseState.valueD68DB4) = 5;
        break;
    default:
        break;
    }
}

void Sub_473560(const std::int32_t index)
{
    WindowInstance* const window = gCurrentWindow;
    WindowElement* const element = &window->elements[index];
    const std::int32_t left = window->x + element->x;
    const std::int32_t top = window->y + element->y;
    const std::int32_t width = element->width;
    const std::int32_t height = element->height;
    std::int32_t events = 0;
    std::int32_t phase = gMousePhase57C128;
    if (phase == 1 || phase == 4 || phase == 2 || phase == 6) {
        if (static_cast<std::uint8_t>(platform::gLegacyMouseState.valueD68DB4) < 0xD4u) {
            if (Sub_472F00(left, top, width, height) != 0) {
                phase = gMousePhase57C128;
                if (phase == 1 || phase == 4) {
                    events = 0x20;
                }
                if (gFocusWindowId57C120 == gCurrentWindowIdD67A1C
                    && gFocusElementIndex57C124 == gCurrentElementIndex57C12C) {
                    events = AppendMouseButtonEvents(events);
                }
            } else {
                phase = gMousePhase57C128;
            }
        }
    } else if (gWindowHasFocusCA0AB4 == 0) {
        if (Sub_472F00(left, top, width, height) != 0) {
            events = AppendMouseButtonEvents(1);
            WindowInstance* const currentWindow = gCurrentWindow;
            WindowElement* const currentElement = &currentWindow->elements[index];
            if (currentElement->type == 4) {
                auto* const data = static_cast<WindowMouseData*>(currentElement->data);
                data->deltaX = platform::legacy_crypt_int::Subtract_401E60(
                    &platform::gLegacyMouseState.mouseX,
                    currentWindow->x + currentElement->x);
                WindowInstance* const yWindow = gCurrentWindow;
                data->deltaY = platform::legacy_crypt_int::Subtract_401E60(
                    &platform::gLegacyMouseState.mouseY,
                    yWindow->y + yWindow->elements[index].y);
            }
        }
        phase = gMousePhase57C128;
    }
    if (phase == 4) {
        const std::int32_t dragX = gDragX57C11C - left;
        if (dragX >= 0 && dragX < width) {
            const std::int32_t dragY = gDragY57C11E - top;
            if (dragY >= 0 && dragY < height) {
                events |= 0x40;
            }
        }
    }
    if (element->visible == 1 && element->mouseCallback(index, events) != 0) {
        phase = gMousePhase57C128;
        auto* const eventByte = reinterpret_cast<unsigned char*>(
            &platform::gLegacyMouseState.valueD68DB4);
        *eventByte = 0xD4;
        gWindowHasFocusCA0AB4 = 1;
        if (phase == 4) {
            const WindowDragCallback callback = gDragCallback57C118;
            if (callback != nullptr) {
                callback(4, 0, gDragSourceIndex57C114);
            }
            static_cast<void>(Sub_4763E0());
        }
    }
}

std::int32_t Sub_4741A0(const std::int32_t index, const std::int32_t state)
{
    auto* const data = static_cast<WindowMouseData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((data->flags & 2u) == 0) {
        data->flags = 0;
    }
    if ((state & 8) != 0) {
        data->flags = 3;
        result = 1;
    }
    if ((data->flags & 2u) != 0) {
        WindowInstance* const window = gCurrentWindow;
        WindowElement* const previous = &window->elements[index - 1];
        const std::int32_t y = platform::legacy_crypt_int::Subtract_401E60(
            &platform::gLegacyMouseState.mouseY, window->y);
        previous->y = y - (previous->height >> 1);
    }
    WindowElement* element = &gCurrentWindow->elements[index];
    if (element[-1].y < element->y) {
        element[-1].y = element->y;
    }
    element = &gCurrentWindow->elements[index];
    const std::int32_t bottom = element->y + element->height;
    const std::int32_t previousHeight = element[-1].height;
    if (element[-1].y + previousHeight > bottom) {
        element[-1].y = bottom - previousHeight;
    }
    element = &gCurrentWindow->elements[index];
    data->deltaY = element[-1].y - element->y;
    if ((state & 8) == 0) {
        data->flags &= ~std::uint32_t{2};
    }
    return result;
}

std::uint8_t Sub_476570(
    WindowInputPrototype* const prototype, const std::int32_t x,
    const std::int32_t y, const std::uint32_t mode)
{
    if (prototype->input.kind == WindowInputReferenceKind::RejectedValue) {
        return 0;
    }
    input::InputContext* const context = prototype->input.context;
    if (context == nullptr) {
        return 0;
    }
    switch (mode) {
    case 0: {
        const std::int32_t encrypt = prototype->encrypt;
        const std::int32_t value224 = prototype->value224;
        const std::int32_t maxCharPerLine = prototype->maxCharPerLine;
        const std::int32_t maxLine = prototype->maxLine;
        const char* const text = prototype->text.data();
        const std::int32_t color = prototype->color;
        const std::int32_t fontType = prototype->fontType;
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::InitializeContext_449CF0(
            context, inputX, inputY, renderMode, fontType, color, text,
            maxLine, maxCharPerLine, value224, encrypt);
        return 1;
    }
    case 1: {
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t color = prototype->color;
        const std::int32_t maxCharPerLine = prototype->maxCharPerLine;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::Sub_45D720(context, inputX, inputY, maxCharPerLine, color, renderMode);
        [[fallthrough]];
    }
    case 2: {
        const std::int32_t inputX = prototype->x + x;
        prototype->input.context->field130 = inputX;
        const std::int32_t inputY = prototype->y + y;
        prototype->input.context->field134 = inputY;
        return 1;
    }
    case 3: {
        const std::int32_t fontType = prototype->fontType;
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t color = context->field140;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::Sub_45D760(context, inputX, inputY, color, renderMode, fontType);
        break;
    }
    default:
        break;
    }
    return 1;
}

void Sub_4740F0(char* const destination)
{
    std::int32_t count = 0;
    for (std::size_t position = 0; position < 10; ++position) {
        const auto& pair = world::detail::gSkillPairsF63370[position];
        if (pair.used != 0) {
            const std::int16_t index = std::bit_cast<std::int16_t>(pair.index);
            const auto& skill = world::detail::gSkillsF1BAFC[index];
            std::int16_t requiredSlots;
            std::memcpy(&requiredSlots, skill.requiredSlots19.data(), sizeof(requiredSlots));
            count += requiredSlots;
        }
    }
    char text[256];
    std::sprintf(text, "%2d/10", count);
    std::strcpy(destination, text);
}

std::int32_t Sub_4A7B00()
{
    const std::int32_t mode = gChatWindowModeD66E04;
    if (mode == 1) {
        WindowInstance* const window = gWindowStates[57].window;
        if (window == nullptr) {
            return 0;
        }
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(Sub_476570(&gChatInputPrototype57E970, x, y, 3));
        gFocusedElementDataD45F94 = gWindowStates[57].window->elements[0].data;
        input::SetContext_44B330(&input::gInputContext626AA0);
        return 1;
    }
    if (mode != 0) {
        return 0;
    }
    WindowInstance* const window = gWindowStates[35].window;
    if (window == nullptr) {
        return 0;
    }
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    if (Sub_476570(&gChatInputPrototype57E970, x, y, 3) == 0) {
        for (;;) {
            WindowInstance* const currentWindow = gWindowStates[35].window;
            gChatInputPrototype57E970.input.context = &input::gInputContext626AA0;
            gChatInputPrototype57E970.input.kind = WindowInputReferenceKind::Context;
            gChatInputPrototype57E970.x = 8;
            std::memcpy(
                &input::gInputContext626AA0, &world::detail::gInputContext626840,
                sizeof(input::InputContext));
            gChatInputPrototype57E970.y = 5;
            gChatInputPrototype57E970.renderMode = 3;
            gChatInputPrototype57E970.fontType = 9;
            gChatInputPrototype57E970.color = 0;
            gChatInputPrototype57E970.maxLine = 1;
            gChatInputPrototype57E970.maxCharPerLine = 76;
            gChatInputPrototype57E970.value224 = 0;
            gChatInputPrototype57E970.encrypt = 0;
            const std::int32_t currentY = currentWindow->y;
            const std::int32_t currentX = currentWindow->x;
            if (Sub_476570(&gChatInputPrototype57E970, currentX, currentY, 3) != 0) {
                break;
            }
        }
    }
    gFocusedElementDataD45F94 = gWindowStates[35].window->elements[0].data;
    input::SetContext_44B330(&input::gInputContext626AA0);
    return 1;
}

} // namespace detail

std::int32_t Sub_50E440(const std::uintptr_t)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCC4224 = 1;
    return gate;
}

std::int32_t Sub_50E470(const std::uintptr_t, const std::int32_t value)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCF4688 = value;
    return value;
}

std::int32_t Sub_50E4A0(const std::uintptr_t, const char* const text)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::Sub_4E5830(text);
}

std::int32_t Sub_50E4D0(
    const std::uintptr_t, const std::int32_t index, const char* const text)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::Sub_4E5A40(index, text);
}

std::int32_t Sub_50E500(
    const std::uintptr_t, const std::int32_t index, const char* const text)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::Sub_4E5CD0(index, text);
}

std::int32_t Sub_50E530(const std::uintptr_t)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCC4224 = 0;
    static_cast<void>(detail::Sub_4E5710(0));
    const std::int32_t previous = detail::gTradeValueD2E2B0;
    if (previous == 2) {
        detail::gTradeValueD2E2B0 = 1;
    }
    return previous;
}

void Sub_50E570(const std::uintptr_t, const std::int32_t value)
{
    if (world::detail::gValue9A6E0C != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    if (value == 1) {
        detail::gTradeValueCC4224 = 2;
    } else if (value == 0 || value == 2) {
        WindowInstance* const window = detail::gWindowStates[43].window;
        detail::gTradeValueD67B80 = 1;
        if (window != nullptr) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
}

void InitializeNumberSpacing_477170()
{
    using namespace detail;
    const std::int32_t first = kNumberSpacingDeductions54F2A8[0];
    const std::int32_t second = kNumberSpacingDeductions54F2A8[1];
    const std::int32_t third = kNumberSpacingDeductions54F2A8[2];
    gNumberAdvancesD67AC4[0] = first + 5;
    const std::int32_t fourth = kNumberSpacingDeductions54F2A8[3];
    gNumberAdvancesD67AC4[1] = second + 10;
    gNumberAdvancesD67AC4[2] = third + 11;
    gNumberAdvancesD67AC4[3] = fourth + 7;
}

std::uint8_t ReadValueD67BE0() noexcept
{
    return detail::gValueD67BE0;
}

std::uint8_t ReadValue5F9754() noexcept
{
    return detail::gValue5F9754;
}

std::int32_t ReadDragItemIndex_584DA4() noexcept
{
    return detail::gDragItemIndex584DA4;
}

WindowInstance* Sub_4CD3E0()
{
    WindowInstance* const window = detail::gWindowStates[68].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

WindowInstance* Sub_4CD400()
{
    WindowInstance* const window = detail::gWindowStates[67].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

void CancelWindowDrag_476410()
{
    using namespace detail;
    std::int32_t phase = gMousePhase57C128;
    if (phase == 0) {
        return;
    }
    const WindowDragCallback callback = gDragCallback57C118;
    if (callback != nullptr) {
        callback(phase == 1 ? 1 : 4, 0, gDragSourceIndex57C114);
        phase = gMousePhase57C128;
    }
    gMouseValue57C110 = 0;
    gDragSourceIndex57C114 = 0;
    gDragCallback57C118 = nullptr;
    gDragX57C11C = 0;
    gDragY57C11E = 0;
    gFocusWindowId57C120 = 0;
    gFocusElementIndex57C124 = -1;
    gMousePhase57C128 = phase == 1 ? 2 : 6;
}

void FreeWindow_471270(WindowInstance* const window)
{
    using namespace detail;
    window->release();
    gCurrentWindowState = &gWindowStates[window->id];
    gCurrentWindowState->x = window->x;
    gCurrentWindowState->y = window->y;
    gCurrentWindowState->flags &= ~std::uint32_t{0x80};
    gCurrentWindowState->flags |= 1u;
    gCurrentWindowState->window = nullptr;
    if (gFocusWindowId57C120 == window->id) {
        CancelWindowDrag_476410();
    }
    for (std::int32_t index = 0; index < window->elementCount; ++index) {
        WindowElement* const element = &window->elements[index];
        switch (element->type) {
        case 1:
        case 5:
            if (element->data == gFocusedElementDataD45F94) {
                static_cast<void>(Sub_4A7B00());
            }
            if (void* const data = window->elements[index].data; data != nullptr) {
                std::free(data);
                window->elements[index].data = nullptr;
            }
            break;
        case 2:
            if (element->data != nullptr) {
                const auto* const data = static_cast<const WindowAnimationData*>(element->data);
                login::detail::MarkDeath(data->action);
                std::free(window->elements[index].data);
                window->elements[index].data = nullptr;
            }
            break;
        case 3: {
            auto* const data = static_cast<WindowTextData*>(element->data);
            if (data->text != nullptr) {
                std::free(data->text);
                static_cast<WindowTextData*>(window->elements[index].data)->text = nullptr;
            }
            if (void* const current = window->elements[index].data; current != nullptr) {
                std::free(current);
                window->elements[index].data = nullptr;
            }
            break;
        }
        case 4:
        case 6:
        case 7:
            if (element->data != nullptr) {
                std::free(element->data);
                window->elements[index].data = nullptr;
            }
            break;
        default:
            break;
        }
    }
    if (window->elements != nullptr) {
        std::free(window->elements);
    }
    window->elements = nullptr;
    std::free(window);
}

void CloseWindowsByCategory_472240(const std::uint32_t mask)
{
    using namespace detail;
    for (std::size_t index = 0; index < gWindows.size(); ++index) {
        WindowInstance* const window = gWindows[index];
        if (window == nullptr) {
            continue;
        }
        const std::int32_t id = window->id;
        const auto category = static_cast<std::uint32_t>(
            static_cast<std::int32_t>(kWindowCategories[id]));
        if ((mask & category) != 0) {
            gCurrentWindowState = &gWindowStates[id];
            gCurrentWindowState->flags |= 2u;
            FreeWindow_471270(gWindows[index]);
            gWindows[index] = nullptr;
        }
    }
}

void DrawCurrentWindow_4729B0()
{
    using namespace detail;
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* window = gCurrentWindow;
    for (std::int32_t index = 0; index < window->elementCount; ++index) {
        WindowElement* const element = &window->elements[index];
        if (element->visible == 1) {
            const std::int32_t x = window->x + element->x;
            const std::int32_t y = window->y + element->y;
            switch (element->type) {
            case 1: {
                const auto* const data = static_cast<const WindowGraphicData*>(element->data);
                if (data->imageNo == 0) {
                    break;
                }
                std::array<std::uint16_t, 4> rectangle {};
                static_assert(sizeof(rectangle) == 8);
                std::uint8_t cropped = 0;
                if (data->offsetX != 0 || data->offsetY != 0
                    || data->width != 0 || data->height != 0) {
                    std::int32_t graphicIndex;
                    resource::GetGraphicMapNoIndex(data->imageNo, &graphicIndex);
                    std::int16_t graphicX;
                    std::int16_t graphicY;
                    resource::GetGraphicXY(
                        static_cast<std::uint32_t>(graphicIndex), &graphicX, &graphicY);
                    rectangle[0] = static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(data->offsetX) + graphicX);
                    rectangle[1] = static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(data->offsetY) + graphicY);
                    rectangle[2] = static_cast<std::uint16_t>(data->width);
                    rectangle[3] = static_cast<std::uint16_t>(data->height);
                    cropped = 1;
                }
                std::memcpy(descriptor.bytes.data(), rectangle.data(), sizeof(rectangle));
                std::memcpy(descriptor.bytes.data() + 8, &data->color, sizeof(data->color));
                descriptor.bytes[12] = cropped;
                descriptor.bytes[13] = 4;
                descriptor.bytes[14] = 0;
                descriptor.bytes[15] = 0;
                WindowInstance* const drawWindow = gCurrentWindow;
                const std::int32_t imageNo = data->imageNo;
                const std::int32_t drawY = drawWindow->y + element->y;
                const std::int32_t drawX = drawWindow->x + element->x;
                static_cast<void>(render::DrawGraphic3_520970(
                    drawX, drawY, 0xDF, imageNo, 0, 1, 0, &descriptor));
                break;
            }
            case 2: {
                const auto* const data = static_cast<const WindowAnimationData*>(element->data);
                const std::uint32_t stop = data->value04;
                login::YobiAction* const action = data->action;
                static_cast<void>(login::detail::Sub_517880(action, 100, stop != 0, 0));
                const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
                if (graphicIndex == 0) {
                    break;
                }
                const render::GraphicDescriptor16* const mode =
                    login::detail::ApplyWindowAnimationDrawMode(action);
                const std::int32_t offsetY = login::detail::Sub_5182B0(action, 1);
                WindowInstance* const verticalWindow = gCurrentWindow;
                const std::int32_t elementY = element->y;
                const std::int32_t drawY = offsetY + verticalWindow->y + elementY;
                const std::int32_t offsetX = login::detail::Sub_5182B0(action, 0);
                WindowInstance* const horizontalWindow = gCurrentWindow;
                const std::int32_t elementX = element->x;
                const std::int32_t drawX = offsetX + horizontalWindow->x + elementX;
                static_cast<void>(render::DrawGraphic2_521DD0(
                    drawX, drawY, 0xDF, graphicIndex, 0, 0, mode));
                break;
            }
            case 3: {
                const auto* const data = static_cast<const WindowTextData*>(element->data);
                const char* const text = data->text;
                if (text != nullptr) {
                    const std::int32_t drawY = y - element->height;
                    const auto color = std::bit_cast<std::int8_t>(
                        std::to_integer<std::uint8_t>(data->bytes00[0]));
                    const auto font = std::bit_cast<std::int8_t>(
                        std::to_integer<std::uint8_t>(data->bytes00[1]));
                    const std::int32_t drawX = x - element->width;
                    static_cast<void>(render::DrawText2_456670(
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                        3, font, color, text, 0));
                }
                break;
            }
            case 5: {
                const auto* const data = static_cast<const WindowInputData*>(element->data);
                if (data == gFocusedElementDataD45F94) {
                    if (input::gCurrentInputData == &input::gInputContext626AA0) {
                        if (input::gValueC9A96C == 1) {
                            Sub_4A7590(data->context);
                        }
                    } else if (data != nullptr) {
                        Sub_4A7640(data->context);
                    }
                }
                break;
            }
            case 6:
                Sub_4A7750(x + 5, y);
                break;
            case 7: {
                const auto* const data = static_cast<const WindowNumberData*>(element->data);
                if (data != nullptr) {
                    Sub_4771B0(
                        x, y, data->text, data->style,
                        std::bit_cast<std::int8_t>(data->color), data->flags, 0xDF);
                }
                break;
            }
            default:
                break;
            }
        }
        window = gCurrentWindow;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    world::detail::Sub_476680();
}

void ApplyObservationInputGate_440ADB()
{
    auto* const bytes = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if (detail::gValue5F9754 != 0) {
        const unsigned char previous = bytes[7];
        detail::gValue5F9754 = 0;
        bytes[7] = static_cast<unsigned char>(previous - 1);
    }
    for (std::size_t index = 0; index < 8; ++index) {
        bytes[index] = static_cast<unsigned char>(bytes[index] + 1);
    }
}

} // namespace cg::ui
