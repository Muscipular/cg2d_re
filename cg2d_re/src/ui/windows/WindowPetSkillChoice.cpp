#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

std::int32_t gWindow54DialogTypeCB2224 = 0; // 原全局：0x00CB2224。

std::int32_t gWindow54ImageCC18C4 = 0; // 原全局：0x00CC18C4。

std::int32_t gWindow54ValueCC2E68 = 0; // 原全局：0x00CC2E68。

std::int32_t gWindow54ValueCF4A3C = 0; // 原全局：0x00CF4A3C。

std::int32_t gWindow54ValueCB01E4 = 0; // 原全局：0x00CB01E4。

std::int32_t gWindow54ValueCB01E8 = 0; // 原全局：0x00CB01E8。

std::uint8_t gWindow54ModeC8AF64 = 0; // 原全局：0x00C8AF64。

std::int32_t gWindow54LifecycleD67434 = 0; // 原全局：0x00D67434。

std::array<WindowDialogTextLine, 15> gWindow54NamesC8AAA0 {}; // 原全局：0x00C8AAA0。

std::array<std::int32_t, 15> gWindow54ValuesCB01A4 {}; // 原全局：0x00CB01A4。

std::array<std::int32_t, 15> gWindow54ValuesCB09D8 {}; // 原全局：0x00CB09D8。

std::array<Window54SkillRecord, 16> gWindow54SkillsD2EAB8 {}; // 原全局：0x00D2EAB8。

std::int32_t gWindow54SkillScrollCC31F4 = 0; // 原全局：0x00CC31F4。

std::int32_t gWindow54MessageC9FCE0 = 0; // 原全局：0x00C9FCE0。

std::int32_t gWindow54HoverD2DAD8 = 0; // 原全局：0x00D2DAD8。

std::int32_t gWindow54SelectedD2DC04 = 0; // 原全局：0x00D2DC04。

std::wstring gWindow54AmountTextD45F78 {}; // 原全局：0x00D45F78。

std::array<std::wstring, 10> gWindow54ValueTextsCB035C {}; // 原全局：0x00CB035C。

std::array<std::wstring, 10> gWindow54ValueTextsC8B318 {}; // 原全局：0x00C8B318。

std::array<std::wstring, 5> gWindow54SkillValueTextsCF4978 {}; // 原全局：0x00CF4978。

std::array<std::wstring, 5> gWindow54SkillPriceTextsCF493C {}; // 原全局：0x00CF493C。

std::array<Window54PetOption, 5> gWindow54PetOptionsD67448 {}; // 原全局：0x00D67448。

std::array<std::int8_t, 5> gWindow54PetMapCF4AB0 {}; // 原全局：0x00CF4AB0。

std::int32_t gWindow54TribeCA0B2C = 0; // 原全局：0x00CA0B2C。

std::int32_t gWindow54PetChoiceC8AA90 = 0; // 原全局：0x00C8AA90。

std::int32_t gWindow54PetScrollCC31DC = 0; // 原全局：0x00CC31DC。

std::array<Window54TechniqueSlot, 10> gWindow54TechniqueSlotsD66E38 {}; // 原全局：0x00D66E38。

std::int32_t gWindow54ReplacementC8B08C = 0; // 原全局：0x00C8B08C。

std::array<std::int8_t, 15> gWindow54SkillMapCC4D04 {}; // 原全局：0x00CC4D04。

std::wstring gWindow54MoneyTextC9A9A4 {}; // 原全局：0x00C9A9A4。

static_assert(sizeof(gWindow54SkillsD2EAB8) == 0x600);

static_assert(sizeof(gWindow54PetOptionsD67448) == 0xC8);

static_assert(sizeof(gWindow54TechniqueSlotsD66E38) == 0x398);

// 原函数：0x004BD500。
void WindowPetSkillChoice_Release()
{
    if (gWindow54LifecycleD67434 != 2
        && gWindow54DialogTypeCB2224 == world::detail::gValue581800) {
        world::detail::gValue581800 = -1;
    }
    gWindow54LifecycleD67434 = 0;
}

// 原函数：0x004BD540。
login::YobiAction* WindowPetSkillChoice_OpenLearn(const char* const text)
{
    std::array<char, 1536> buffer;
    gWindow54DialogTypeCB2224 = world::detail::gValue581800;
    gWindow54ImageCC18C4 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 511, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[row + 1].text, buffer.data(), row, 21));
    }
    char headingGbkBuffer[3072]{};
    char* const heading = headingGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 4, 80, heading));
    input::UnescapeLegacyTextAndNotify_52D2F0(heading);
    gNpcDialogLinesCC24D4[9].text = cg::text::FromGbkWide(heading);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 5, 511, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 8; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[row + 10].text, buffer.data(), row, 32));
    }
    gWindow54ValueCC2E68 = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    gWindow54ValueCF4A3C = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    gWindow54ValueCB01E4 = 0;
    gWindow54ValueCB01E8 = 0;
    gWindow54ModeC8AF64 = 0;
    return WindowRuntime_CreateAnimation(WindowId_PetSkillChoice, 1, 0, nullptr);
}

// 原函数：0x004BD680。
login::YobiAction* WindowPetSkillChoice_OpenForget(const char* const text)
{
    std::array<char, 1536> buffer;
    gWindow54DialogTypeCB2224 = world::detail::gValue581800;
    gWindow54ImageCC18C4 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 511, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[row + 1].text, buffer.data(), row, 21));
    }
    for (std::int32_t row = 0; row < 15; ++row) {
        const std::int32_t field = 4 + row * 3;
        char nameGbkBuffer[3072]{};
    char* const name = nameGbkBuffer;
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field, 21, name));
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
    gNpcDialogLinesCC24D4[row + 10].text = cg::text::FromGbkWide(name);
        std::wstring& destination = gWindow54NamesC8AAA0[row].text;
        const std::size_t length = std::strlen(name) + 1;
        const std::size_t wholeWords = length & ~std::size_t{3};
        destination = cg::text::FromGbkWide(name);
        const char first = name[0];
        // 原分段字节复制已并入上面的UTF-16赋值。
        if (first != '\0') {
            gWindow54ValuesCB01A4[row] =
                network::ParseDelimitedInteger_52CFA0(text, '|', field + 1);
            gWindow54ValuesCB09D8[row] =
                network::ParseDelimitedInteger_52CFA0(text, '|', field + 2);
        } else {
            gWindow54ValuesCB01A4[row] = -1;
            gWindow54ValuesCB09D8[row] = -1;
        }
    }
    gWindow54ModeC8AF64 = 1;
    gWindow54ValueCB01E4 = 0;
    gWindow54ValueCB01E8 = 0;
    return WindowRuntime_CreateAnimation(WindowId_PetSkillChoice, 1, 0, nullptr);
}

// 两原创建仅模式值不同，保留原窗快照和生命周期标记的分支次序。
static login::YobiAction* WindowPetSkillChoice_CreateSkillList(
    const char* const text, const std::uint8_t mode)
{
    std::array<char, 768> buffer;
    gWindow54DialogTypeCB2224 = world::detail::gValue581800;
    gWindow54ImageCC18C4 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 255, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[1 + group * 4 + row].text, buffer.data(), row, 20));
        }
    }
    std::memset(gWindow54SkillsD2EAB8.data(), 0, sizeof(gWindow54SkillsD2EAB8));
    gWindow54ValueCB01E8 = 0;
    std::int32_t field = 6;
    for (Window54SkillRecord& record : gWindow54SkillsD2EAB8) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, buffer.data()) == 1) {
            break;
        }
        record.used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::strcpy(record.name.data(), std::strlen(buffer.data()) > 24 ? "???" : buffer.data());
        record.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record.price = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 255, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 2; ++row) {
            static_cast<void>(input::ExtractLegacyLine_503AB0(
                record.description[row].data(), 29, buffer.data(), row, 28));
        }
        ++gWindow54ValueCB01E8;
    }
    WindowInstance* const window = gWindowStates[WindowId_PetSkillChoice].window;
    gWindow54ValueCB01E4 = 0;
    gWindow54ModeC8AF64 = mode;
    if (window != nullptr) {
        if (gWindow54LifecycleD67434 != 0) {
            gWindow54LifecycleD67434 = 0;
            WindowPetSkillChoice_RefreshPetNames(window);
            return nullptr;
        }
        window->state |= 0x8000u;
        gWindow54LifecycleD67434 = 2;
    } else {
        gWindow54LifecycleD67434 = 0;
    }
    return WindowRuntime_CreateAnimation(WindowId_PetSkillChoice, 1, 0, nullptr);
}

// 原函数：0x004BD810。
login::YobiAction* WindowPetSkillChoice_OpenPetSkills(const char* const text)
{
    return WindowPetSkillChoice_CreateSkillList(text, 2);
}

// 原函数：0x004BDAD0。
login::YobiAction* WindowPetSkillChoice_OpenPetSkillsByTribe(const char* const text)
{
    return WindowPetSkillChoice_CreateSkillList(text, 12);
}

// 原函数：0x004BDD90。
void WindowPetSkillChoice_ReceivePetCandidates(const char* const text)
{
    if (gWindowStates[WindowId_PetSkillChoice].window == nullptr) {
        return;
    }
    std::array<char, 768> buffer;
    gWindow54DialogTypeCB2224 = world::detail::gValue581800;
    gWindow54ImageCC18C4 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 255, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 4; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[row + 1].text, buffer.data(), row, 20));
    }
    std::memset(gWindow54PetOptionsD67448.data(), 0, sizeof(gWindow54PetOptionsD67448));
    const std::int32_t threshold = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    std::int32_t field = 5;
    for (Window54PetOption& option : gWindow54PetOptionsD67448) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, buffer.data()) == 1) {
            break;
        }
        option.used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::strcpy(option.name.data(), std::strlen(buffer.data()) > 16 ? "???" : buffer.data());
        option.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.slots = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.tribe = -1;
        option.hintFlag = 1;
        if (threshold <= option.value04) {
            option.allowed = 1;
        }
    }
    WindowInstance* const window = gWindowStates[WindowId_PetSkillChoice].window;
    gWindow54TribeCA0B2C = -1;
    gWindow54ValueCB01E4 = 0;
    gWindow54ValueCB01E8 = 0;
    gWindow54ModeC8AF64 = 3;
    gWindow54LifecycleD67434 = 0;
    WindowPetSkillChoice_RefreshPetCandidates(window);
}

// 原函数：0x004BDF70。
void WindowPetSkillChoice_ReceiveTribeCandidates(const char* const text)
{
    if (gWindowStates[WindowId_PetSkillChoice].window == nullptr) {
        return;
    }
    std::array<char, 768> buffer;
    gWindow54DialogTypeCB2224 = world::detail::gValue581800;
    gWindow54ImageCC18C4 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 255, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[1 + group * 4 + row].text, buffer.data(), row, 20));
        }
    }
    std::memset(gWindow54PetOptionsD67448.data(), 0, sizeof(gWindow54PetOptionsD67448));
    const std::int32_t threshold = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    gWindow54TribeCA0B2C = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    std::int32_t field = 8;
    for (Window54PetOption& option : gWindow54PetOptionsD67448) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, buffer.data()) == 1) {
            break;
        }
        option.used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::strcpy(option.name.data(), std::strlen(buffer.data()) > 16 ? "???" : buffer.data());
        option.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.slots = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.tribe = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        option.hintFlag = static_cast<std::uint8_t>(
            network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        if (threshold <= option.value04) {
            option.allowed = 1;
        }
    }
    WindowInstance* const window = gWindowStates[WindowId_PetSkillChoice].window;
    gWindow54ValueCB01E4 = 0;
    gWindow54ValueCB01E8 = 0;
    gWindow54ModeC8AF64 = 13;
    gWindow54LifecycleD67434 = 0;
    WindowPetSkillChoice_RefreshPetCandidates(window);
}

// 原函数：0x004BE200。
void WindowPetSkillChoice_ReceiveSelectedPetSkills(const char* const text)
{
    if (gWindowStates[WindowId_PetSkillChoice].window == nullptr) {
        return;
    }
    std::array<char, 1536> buffer;
    gWindow54DialogTypeCB2224 = world::detail::gValue581800;
    gWindow54ImageCC18C4 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 511, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[row + 1].text, buffer.data(), row, 21));
    }
    std::memset(gWindow54TechniqueSlotsD66E38.data(), 0, sizeof(gWindow54TechniqueSlotsD66E38));
    std::int32_t field = 4;
    for (Window54TechniqueSlot& slot : gWindow54TechniqueSlotsD66E38) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 511, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (buffer[0] == '\0') {
            slot.used = 0;
            field += 2;
            continue;
        }
        slot.used = 1;
        std::strcpy(slot.name.data(), std::strlen(buffer.data()) > 16 ? "???" : buffer.data());
        slot.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 511, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 2; ++row) {
            static_cast<void>(input::ExtractLegacyLine_503AB0(
                slot.description[row].data(), 29, buffer.data(), row, 28));
        }
    }
    WindowInstance* const window = gWindowStates[WindowId_PetSkillChoice].window;
    gWindow54ValueCB01E4 = 0;
    gWindow54ValueCB01E8 = 0;
    gWindow54ModeC8AF64 = 4;
    gWindow54LifecycleD67434 = 0;
    WindowPetSkillChoice_RefreshSelectedPetSkills(window);
}

// 原函数：0x004BE3F0。
void WindowPetSkillChoice_RefreshLearn(WindowInstance* const window)
{
    static_cast<WindowGraphicData*>(window->elements[104].data)->imageNo = 244384;
    window->elements[2].visible = 1;
    window->elements[5].visible = 1;
    gWindow54ValueCB01E4 = 0;
    std::int32_t usedSlots = 0;
    std::int32_t count = 0;
    for (const world::detail::SkillPairRecord& pair : world::detail::gSkillPairsF63370) {
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            usedSlots += std::bit_cast<std::int8_t>(
                world::detail::gSkillsF1BAFC[index].requiredSlots19[0]);
            ++count;
        }
    }
    const std::int32_t totalSlots = world::detail::ReadPlayerValue16C();
    gWindow54ValueCB01E4 = count;
    cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[5].data)->text, L"{:2}/{:2}", usedSlots, totalSlots);
    std::int32_t freeSlots = world::detail::ReadPlayerValue16C() - usedSlots;
    if (freeSlots < 0) {
        freeSlots = 0;
    }
    const std::int32_t displayed = gWindow54ValueCB01E4 + freeSlots;
    gWindow54ValueCB01E4 = displayed;
    for (std::int32_t index = 9; index <= 16; ++index) {
        window->elements[index].visible = displayed > 10 ? 1 : 0;
    }
    window->elements[24].visible = 1;
    static_cast<WindowTextData*>(window->elements[24].data)->text = gNpcDialogLinesCC24D4[9].text;
    for (std::int32_t row = 0; row < 8; ++row) {
        window->elements[row + 25].visible = 1;
        static_cast<WindowTextData*>(window->elements[row + 25].data)->text = gNpcDialogLinesCC24D4[row + 10].text;
    }
    window->elements[33].visible = 1;
    cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[33].data)->text, L"所需的技能欄位數量：{:2}欄", gWindow54ValueCC2E68);
    window->elements[6].visible = 1;
    cg::text::FormatTo(gWindow54AmountTextD45F78, L"{:10}", gWindow54ValueCF4A3C);
    for (std::int32_t row = 0; row < 10; ++row) {
        window->elements[row + 69].mouseCallback = &WindowElement_NoOp;
    }
    const std::int32_t currentCount = gWindow54ValueCB01E4;
    const std::int32_t scroll = gWindow54PetScrollCC31DC;
    WindowPetSkillChoice_RefreshLearnRows(window, scroll, currentCount);
    for (std::int32_t index = 17; index <= 23; ++index) {
        window->elements[index].visible = 0;
    }
    for (std::int32_t index = 79; index <= 103; ++index) {
        window->elements[index].visible = 0;
    }
}

// 原函数：0x004BE600。
void WindowPetSkillChoice_RefreshForget(WindowInstance* const window)
{
    static_cast<WindowGraphicData*>(window->elements[104].data)->imageNo = 244384;
    window->elements[2].visible = 0;
    window->elements[5].visible = 1;
    for (std::int32_t row = 0; row < 15; ++row) {
        const world::detail::SkillPairRecord& pair = world::detail::gSkillPairsF63370[row];
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            std::int32_t empty = 0;
            for (std::int32_t skill = 0; skill < index; ++skill) {
                if (world::detail::gSkillsF1BAFC[skill].name[0] == '\0') {
                    ++empty;
                }
            }
            const auto lowIndex = static_cast<std::uint8_t>(pair.index);
            gWindow54SkillMapCC4D04[row] =
                std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(lowIndex - empty));
        } else {
            gWindow54SkillMapCC4D04[row] = -1;
        }
    }
    gWindow54ValueCB01E4 = 0;
    std::int32_t usedSlots = 0;
    std::int32_t count = 0;
    for (const world::detail::SkillPairRecord& pair : world::detail::gSkillPairsF63370) {
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            usedSlots += std::bit_cast<std::int8_t>(
                world::detail::gSkillsF1BAFC[index].requiredSlots19[0]);
            ++count;
        }
    }
    const std::int32_t totalSlots = world::detail::ReadPlayerValue16C();
    gWindow54ValueCB01E4 = count;
    cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[5].data)->text, L"{:2}/{:2}", usedSlots, totalSlots);
    std::int32_t freeSlots = world::detail::ReadPlayerValue16C() - usedSlots;
    if (freeSlots < 0) {
        freeSlots = 0;
    }
    const std::int32_t displayed = gWindow54ValueCB01E4 + freeSlots;
    gWindow54ValueCB01E4 = displayed;
    for (std::int32_t index = 9; index <= 16; ++index) {
        window->elements[index].visible = displayed > 10 ? 1 : 0;
    }
    window->elements[24].visible = 0;
    static_cast<WindowTextData*>(window->elements[25].data)->text = L"請選擇";
    static_cast<WindowTextData*>(window->elements[26].data)->text = L"想遺忘的技能";
    static_cast<WindowTextData*>(window->elements[27].data)->text = cg::text::FromGbkWide(input::LegacyText593850());
    static_cast<WindowTextData*>(window->elements[28].data)->text = L"＊＊＊＊＊＊　注意　＊＊＊＊＊＊";
    static_cast<WindowTextData*>(window->elements[29].data)->text = cg::text::FromGbkWide(input::LegacyText593850());
    static_cast<WindowTextData*>(window->elements[30].data)->text = L"　一旦遺忘技能後";
    static_cast<WindowTextData*>(window->elements[31].data)->text = L"　就不能再恢復原狀了。";
    static_cast<WindowTextData*>(window->elements[32].data)->text = L"　請特別注意。";
    for (std::int32_t index = 25; index <= 32; ++index) {
        window->elements[index].visible = 1;
    }
    window->elements[33].visible = 0;
    window->elements[6].visible = 1;
    cg::text::FormatTo(gWindow54AmountTextD45F78, L"{:10}", 0);
    for (std::int32_t row = 0; row < 10; ++row) {
        window->elements[row + 69].mouseCallback = &WindowPetSkillChoice_SubmitForget;
    }
    const std::int32_t currentCount = gWindow54ValueCB01E4;
    const std::int32_t scroll = gWindow54PetScrollCC31DC;
    WindowPetSkillChoice_RefreshForgetRows(window, scroll, currentCount);
    for (std::int32_t index = 17; index <= 23; ++index) {
        window->elements[index].visible = 0;
    }
    for (std::int32_t index = 79; index <= 103; ++index) {
        window->elements[index].visible = 0;
    }
}

// 原函数：0x004BE930。
void WindowPetSkillChoice_RefreshPetNames(WindowInstance* const window)
{
    static_cast<WindowGraphicData*>(window->elements[104].data)->imageNo = 244385;
    window->elements[2].visible = 0;
    window->elements[5].visible = 1;
    static_cast<WindowTextData*>(window->elements[5].data)->text = cg::text::FromGbkWide(input::LegacyText593850());
    for (std::int32_t index = 9; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    for (std::int32_t index = 24; index <= 32; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[33].visible = 0;
    window->elements[6].visible = 1;
    cg::text::FormatTo(gWindow54AmountTextD45F78, L"{:10}", 0);
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t petIndex =
            std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        window->elements[row + 69].visible = 1;
        window->elements[row + 69].mouseCallback = &WindowElement_NoOp;
        static_cast<WindowGraphicData*>(window->elements[row + 69].data)->imageNo = 243100;
        window->elements[row + 34].visible = 1;
        window->elements[row + 44].visible = 0;
        if (gWindow54ModeC8AF64 == 12) {
            window->elements[row + 44].visible = 1;
        }
        window->elements[row + 49].visible = 1;
        window->elements[row + 59].visible = 1;
        static_cast<WindowTextData*>(window->elements[row + 34].data)->text.clear();
        static_cast<WindowTextData*>(window->elements[row + 44].data)->text.clear();
        const world::detail::PetRuntimeRecord& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex];
        const bool used = pet.used != 0;
        gWindow54ValueTextsCB035C[row].clear();
        gWindow54ValueTextsC8B318[row].clear();
        if (used) {
            const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            static_cast<WindowTextData*>(window->elements[row + 34].data)->text = cg::text::FromGbkWide(name);
            const std::int32_t tribe = std::bit_cast<std::int16_t>(pet.value04);
            static_cast<WindowTextData*>(window->elements[row + 44].data)->text = gTribeNames57CB54[tribe];
        }
    }
    for (std::int32_t row = 5; row < 10; ++row) {
        window->elements[row + 69].visible = 0;
        window->elements[row + 34].visible = 0;
        window->elements[row + 49].visible = 0;
        window->elements[row + 59].visible = 0;
    }
    for (std::int32_t index = 17; index <= 23; ++index) {
        window->elements[index].visible = 1;
    }
    WindowPetSkillChoice_RefreshPetRows(window, gWindow54SkillScrollCC31F4);
}

// 原函数：0x004BEC20。
void WindowPetSkillChoice_RefreshPetCandidates(WindowInstance* const window)
{
    static_cast<WindowGraphicData*>(window->elements[104].data)->imageNo = 244385;
    window->elements[2].visible = 0;
    window->elements[5].visible = 1;
    static_cast<WindowTextData*>(window->elements[5].data)->text = cg::text::FromGbkWide(input::LegacyText593850());
    for (std::int32_t index = 9; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    for (std::int32_t index = 24; index <= 32; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[33].visible = 0;
    window->elements[6].visible = 1;
    cg::text::FormatTo(gWindow54AmountTextD45F78, L"{:10}", gWindow54SkillsD2EAB8[gWindow54SelectedD2DC04].price);
    for (std::int32_t row = 0; row < 5; ++row) {
        const world::detail::PetPairRecord& pair = world::detail::gPetPairsDE0B08[row];
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            std::int32_t empty = 0;
            for (std::int32_t pet = 0; pet < index; ++pet) {
                if ((*world::detail::BorrowPetRuntimeF633F8Storage())[pet].used == 0) {
                    ++empty;
                }
            }
            const auto lowIndex = static_cast<std::uint8_t>(pair.index);
            gWindow54PetMapCF4AB0[row] =
                std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(lowIndex - empty));
        } else {
            gWindow54PetMapCF4AB0[row] = -1;
        }
    }
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t selected = gWindow54PetMapCF4AB0[row];
        window->elements[row + 69].visible = 1;
        window->elements[row + 69].mouseCallback = &WindowPetSkillChoice_SelectPetCandidate;
        window->elements[row + 34].visible = 1;
        window->elements[row + 44].visible = 0;
        if (gWindow54ModeC8AF64 == 13) {
            window->elements[row + 44].visible = 1;
        }
        window->elements[row + 49].visible = 1;
        window->elements[row + 59].visible = 1;
        static_cast<WindowTextData*>(window->elements[row + 34].data)->text.clear();
        static_cast<WindowTextData*>(window->elements[row + 44].data)->text.clear();
        gWindow54ValueTextsCB035C[row].clear();
        gWindow54ValueTextsC8B318[row].clear();
        if (selected >= 0 && gWindow54PetOptionsD67448[selected].used != 0) {
            const Window54PetOption& option = gWindow54PetOptionsD67448[selected];
            static_cast<WindowTextData*>(window->elements[row + 34].data)->text = cg::text::FromGbkWide(option.name.data());
            const std::int32_t tribe = option.tribe;
            if (tribe >= 0) {
                static_cast<WindowTextData*>(window->elements[row + 44].data)->text = gTribeNames57CB54[tribe];
            }
        }
    }
    for (std::int32_t row = 5; row < 10; ++row) {
        window->elements[row + 69].visible = 0;
        window->elements[row + 34].visible = 0;
        window->elements[row + 49].visible = 0;
        window->elements[row + 59].visible = 0;
    }
    for (std::int32_t index = 17; index <= 23; ++index) {
        window->elements[index].visible = 1;
    }
    WindowPetSkillChoice_RefreshPetRows(window, gWindow54SkillScrollCC31F4);
}

// 原函数：0x004BEF30。
void WindowPetSkillChoice_RefreshSelectedPetSkills(WindowInstance* const window)
{
    static_cast<WindowGraphicData*>(window->elements[104].data)->imageNo = 244385;
    window->elements[2].visible = 0;
    window->elements[5].visible = 1;
    const std::int32_t capacity = gWindow54PetOptionsD67448[gWindow54PetChoiceC8AA90].slots;
    std::int32_t used = 0;
    for (std::int32_t index = 0; index < capacity; ++index) {
        if (gWindow54TechniqueSlotsD66E38[index].used != 0) {
            ++used;
        }
    }
    cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[5].data)->text, L"{:2}/{:2}", used, capacity);
    for (std::int32_t index = 9; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    for (std::int32_t index = 24; index <= 32; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[33].visible = 0;
    window->elements[6].visible = 1;
    cg::text::FormatTo(gWindow54AmountTextD45F78, L"{:10}", gWindow54SkillsD2EAB8[gWindow54SelectedD2DC04].price);
    for (std::int32_t row = 0; row < 5; ++row) {
        window->elements[row + 69].visible = 1;
        window->elements[row + 69].mouseCallback = &WindowPetSkillChoice_SelectPetCandidate;
        static_cast<WindowTextData*>(window->elements[row + 34].data)->text.clear();
        window->elements[row + 44].visible = 0;
        const bool usedOption = gWindow54PetOptionsD67448[row].used != 0;
        gWindow54ValueTextsCB035C[row].clear();
        gWindow54ValueTextsC8B318[row].clear();
        if (usedOption) {
            static_cast<WindowTextData*>(window->elements[row + 34].data)->text = cg::text::FromGbkWide(gWindow54PetOptionsD67448[row].name.data());
        }
    }
    for (std::int32_t row = 5; row < 10; ++row) {
        window->elements[row + 69].visible = 0;
        window->elements[row + 34].visible = 0;
        window->elements[row + 49].visible = 0;
        window->elements[row + 59].visible = 0;
    }
    for (std::int32_t index = 17; index <= 23; ++index) {
        window->elements[index].visible = 1;
    }
    WindowPetSkillChoice_RefreshPetRows(window, gWindow54SkillScrollCC31F4);
    WindowPetSkillChoice_RefreshSelectedSkillRows(window, gWindow54PetScrollCC31DC);
}

// 原函数：0x004BF160。
void WindowPetSkillChoice_RefreshLearnRows(
    WindowInstance* const window, const std::int32_t scroll, const std::int32_t count)
{
    for (std::int32_t row = 0; row < 10; ++row) {
        window->elements[row + 69].visible = 0;
        window->elements[row + 34].visible = 0;
        window->elements[row + 49].visible = 0;
        window->elements[row + 59].visible = 0;
        gWindow54ValueTextsCB035C[row].clear();
        gWindow54ValueTextsC8B318[row].clear();
        if (scroll + row < count) {
            window->elements[row + 69].visible = 1;
            const world::detail::SkillPairRecord& pair = world::detail::gSkillPairsF63370[scroll + row];
            if (pair.used != 0) {
                const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
                window->elements[row + 34].visible = 1;
                window->elements[row + 49].visible = 1;
                window->elements[row + 59].visible = 1;
                const world::detail::SkillRuntimeRecord& skill = world::detail::gSkillsF1BAFC[index];
                static_cast<WindowTextData*>(window->elements[row + 34].data)->text = cg::text::FromGbkWide(skill.name.data());
                cg::text::FormatTo(gWindow54ValueTextsCB035C[row], L"{:2}", skill.level);
                const std::int32_t requiredSlots = std::bit_cast<std::int8_t>(skill.requiredSlots19[0]);
                cg::text::FormatTo(gWindow54ValueTextsC8B318[row], L"{:2}", requiredSlots);
            }
        }
    }
    for (std::int32_t index = 44; index <= 48; ++index) {
        window->elements[index].visible = 0;
    }
}

// 原函数：0x004BF2E0。
void WindowPetSkillChoice_RefreshForgetRows(
    WindowInstance* const window, const std::int32_t scroll, const std::int32_t count)
{
    for (std::int32_t row = 0; row < 10; ++row) {
        window->elements[row + 69].visible = 0;
        window->elements[row + 34].visible = 0;
        window->elements[row + 49].visible = 0;
        window->elements[row + 59].visible = 0;
        gWindow54ValueTextsCB035C[row].clear();
        gWindow54ValueTextsC8B318[row].clear();
        if (scroll + row < count) {
            window->elements[row + 69].visible = 1;
            const std::int32_t selected = gWindow54SkillMapCC4D04[scroll + row];
            if (selected >= 0 && gWindow54ValuesCB01A4[selected] >= 0) {
                window->elements[row + 34].visible = 1;
                window->elements[row + 49].visible = 1;
                window->elements[row + 59].visible = 1;
                static_cast<WindowTextData*>(window->elements[row + 34].data)->text = gWindow54NamesC8AAA0[selected].text;
                cg::text::FormatTo(gWindow54ValueTextsCB035C[row], L"{:2}", gWindow54ValuesCB01A4[selected]);
                cg::text::FormatTo(gWindow54ValueTextsC8B318[row], L"{:2}", gWindow54ValuesCB09D8[selected]);
            }
        }
    }
    for (std::int32_t index = 44; index <= 48; ++index) {
        window->elements[index].visible = 0;
    }
}

// 原函数：0x004BF460。
void WindowPetSkillChoice_RefreshPetRows(WindowInstance* const window, const std::int32_t scroll)
{
    for (std::int32_t row = 0; row < 5; ++row) {
        const Window54SkillRecord& record = gWindow54SkillsD2EAB8[scroll + row];
        window->elements[row + 94].visible = 1;
        window->elements[row + 79].visible = 1;
        window->elements[row + 84].visible = 1;
        window->elements[row + 89].visible = 1;
        window->elements[row + 99].visible = 1;
        if (gWindow54ModeC8AF64 == 2) {
            window->elements[row + 99].mouseCallback = &WindowPetSkillChoice_SubmitPetSkill;
        }
        if (gWindow54ModeC8AF64 == 3) {
            window->elements[row + 99].mouseCallback = &WindowPetSkillChoice_DrawSelectedSkillFrame;
        }
        if (gWindow54ModeC8AF64 == 12) {
            window->elements[row + 99].mouseCallback = &WindowPetSkillChoice_SubmitPetSkill;
        }
        if (gWindow54ModeC8AF64 == 13) {
            window->elements[row + 99].mouseCallback = &WindowPetSkillChoice_DrawSelectedSkillFrame;
        }
        static_cast<WindowTextData*>(window->elements[row + 79].data)->text.clear();
        gWindow54SkillValueTextsCF4978[row].clear();
        gWindow54SkillPriceTextsCF493C[row].clear();
        // 原存在性检查不加scroll，显示内容仍取分页后的记录。
        if (gWindow54SkillsD2EAB8[row].name[0] != '\0') {
            static_cast<WindowTextData*>(window->elements[row + 79].data)->text = cg::text::FromGbkWide(record.name.data());
            cg::text::FormatTo(gWindow54SkillValueTextsCF4978[row], L"{:5}", record.value04);
            cg::text::FormatTo(gWindow54SkillPriceTextsCF493C[row], L"{:10}", record.price);
            const std::int32_t money = world::detail::ReadPlayerValueFC();
            const std::int32_t price = record.price;
            static_cast<WindowNumberData*>(window->elements[row + 89].data)->color =
                money >= price ? 0 : 6;
        }
    }
}

// 原函数：0x004BF620。
void WindowPetSkillChoice_RefreshSelectedSkillRows(WindowInstance* const window, const std::int32_t scroll)
{
    std::int32_t row = 0;
    if (gWindow54PetOptionsD67448[gWindow54PetChoiceC8AA90].slots - scroll > 0) {
        do {
            const Window54TechniqueSlot& slot = gWindow54TechniqueSlotsD66E38[scroll + row];
            window->elements[row + 69].visible = 1;
            window->elements[row + 34].visible = 1;
            window->elements[row + 59].visible = 1;
            window->elements[row + 69].mouseCallback = &WindowPetSkillChoice_SelectSkillSlot;
            static_cast<WindowTextData*>(window->elements[row + 34].data)->text.clear();
            const bool used = slot.used != 0;
            gWindow54ValueTextsCB035C[row].clear();
            gWindow54ValueTextsC8B318[row].clear();
            if (used) {
                static_cast<WindowTextData*>(window->elements[row + 34].data)->text = cg::text::FromGbkWide(slot.name.data());
                cg::text::FormatTo(gWindow54ValueTextsC8B318[row], L"{:3}", slot.value04);
            }
            ++row;
        } while (row < gWindow54PetOptionsD67448[gWindow54PetChoiceC8AA90].slots - scroll);
    }
    for (; row < 10; ++row) {
        window->elements[row + 69].visible = 0;
        window->elements[row + 34].visible = 0;
        window->elements[row + 59].visible = 0;
        window->elements[row + 69].mouseCallback = &WindowElement_NoOp;
    }
    for (std::int32_t index = 44; index <= 48; ++index) {
        window->elements[index].visible = 0;
    }
}

// 原函数：0x004BF7C0。
std::int32_t WindowPetSkillChoice_Update(const std::int32_t value)
{
    if (value == -1) {
        gCurrentWindow->elements[0].visible = 0;
        static_cast<WindowTextData*>(gCurrentWindow->elements[4].data)->text = gNpcDialogLinesCC24D4[0].text;
        auto* animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[8].data);
        login::detail::SetYobiActionField2A8(animation->action, gWindow54ImageCC18C4);
        animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[8].data);
        login::detail::SetYobiActionField2B8(animation->action, 5);
        gWindow54PetScrollCC31DC = 0;
        const auto mode = std::bit_cast<std::int8_t>(gWindow54ModeC8AF64);
        gWindow54SkillScrollCC31F4 = 0;
        switch (mode) {
        case 0:
            WindowPetSkillChoice_RefreshLearn(gCurrentWindow);
            break;
        case 1:
            WindowPetSkillChoice_RefreshForget(gCurrentWindow);
            break;
        case 2:
        case 12:
            WindowPetSkillChoice_RefreshPetNames(gCurrentWindow);
            break;
        default:
            break;
        }
        gWindow54MessageC9FCE0 = -1;
        gWindow54HoverD2DAD8 = -1;
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    const std::int32_t dialogType = world::detail::gValue581800;
    if (dialogType != -2 && gWindow54DialogTypeCB2224 != dialogType) {
        gCurrentWindow->state |= 0x8000u;
    }
    cg::text::FormatTo(gWindow54MoneyTextC9A9A4, L"{:10}", world::detail::ReadPlayerValueFC());
    if (gWindow54MessageC9FCE0 >= 0) {
        gWindow54MessageC9FCE0 = -1;
    }
    return 1;
}

// 原函数：0x004BF950。
std::int32_t WindowPetSkillChoice_Draw(const std::int32_t unused)
{
    static_cast<void>(unused);
    WindowInstance* window = gCurrentWindow;
    const auto mode = std::bit_cast<std::int8_t>(gWindow54ModeC8AF64);
    const std::int32_t x = window->x + 297;
    std::int32_t y = window->y + 52;
    switch (mode) {
    case 0:
    case 1: {
        if (gWindow54ValueCB01E4 - 10 <= 0) {
            window->elements[9].visible = 0;
            window = gCurrentWindow;
        }
        WindowElement* const elements = window->elements;
        if ((static_cast<const WindowMouseData*>(elements[10].data)->flags & 1u) != 0) {
            const std::int32_t position = WindowScroll_ReadPosition(elements + 10, gWindow54ValueCB01E4 - 10);
            const std::uint8_t currentMode = gWindow54ModeC8AF64;
            WindowInstance* const current = gCurrentWindow;
            gWindow54PetScrollCC31DC = position;
            const std::int32_t count = gWindow54ValueCB01E4;
            if (currentMode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(current, position, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(current, position, count);
            }
        }
        break;
    }
    case 2:
    case 3:
    case 4:
    case 12:
    case 13: {
        window->elements[9].visible = 1;
        if (gWindow54ValueCB01E4 - 10 <= 0) {
            gCurrentWindow->elements[9].visible = 0;
        }
        WindowElement* elements = gCurrentWindow->elements;
        if ((static_cast<const WindowMouseData*>(elements[10].data)->flags & 1u) != 0) {
            const std::int32_t position = WindowScroll_ReadPosition(elements + 10, gWindow54ValueCB01E4 - 10);
            WindowInstance* const current = gCurrentWindow;
            gWindow54PetScrollCC31DC = position;
            WindowPetSkillChoice_RefreshSelectedSkillRows(current, position);
        }
        gCurrentWindow->elements[17].visible = 1;
        if (gWindow54ValueCB01E8 - 5 <= 0) {
            gCurrentWindow->elements[17].visible = 0;
        }
        elements = gCurrentWindow->elements;
        if ((static_cast<const WindowMouseData*>(elements[18].data)->flags & 1u) != 0) {
            const std::int32_t position = WindowScroll_ReadPosition(elements + 18, gWindow54ValueCB01E8 - 5);
            gWindow54SkillScrollCC31F4 = position;
            WindowPetSkillChoice_RefreshPetRows(gCurrentWindow, position);
        }
        break;
    }
    default:
        break;
    }
    std::int32_t message = gWindow54MessageC9FCE0;
    gTooltipColorD67D30 = 0;
    if (message >= 0) {
        for (std::int32_t row = 0; row < 2; ++row) {
            if (row != 0) {
                message = gWindow54MessageC9FCE0;
            }
            static_cast<void>(WindowItem_SplitTooltipLines(
                x, y, 3, 8, 0, cg::text::FromGbkWide(gWindow54SkillsD2EAB8[message].description[row].data()), 0, 0));
            y += 10;
        }
    } else {
        const WindowDialogTextLine* line = gNpcDialogLinesCC24D4.data() + 1 + 4 * (-1 - message);
        for (std::int32_t row = 0; row < 4; ++row) {
            if (!line->text.empty()) {
                static_cast<void>(WindowItem_SplitTooltipLines(x, y, 3, 8, 0, line->text.data(), 0, 0));
            }
            ++line;
            y += 10;
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004BFBC0。
std::int32_t WindowPetSkillChoice_ButtonLearn(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        gHoverTextD68AE8 = L"學習此技能。";
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        if (gWindow54ModeC8AF64 == 0) {
            std::wstring escaped;
            escaped = input::EscapeTextAndNotify(L"1", 15);
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t y =
                world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t x =
                world::detail::BorrowPlayerXStorage()->value();
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 0, escaped));
            gCurrentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 243048;
    if (hover) {
        graphic->imageNo = 243050;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243049;
    }
    return consumed;
}

// 原函数：0x004BFCB0。
std::int32_t WindowPetSkillChoice_ButtonBack(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    auto mode = std::bit_cast<std::int8_t>(gWindow54ModeC8AF64);
    const bool hover = (state & 1) != 0;
    if (hover) {
        switch (mode) {
        case 0:
        case 1:
        case 2:
        case 12:
            gHoverTextD68AE8 = L"還是算了。";
            break;
        case 3:
        case 4:
        case 13:
            gHoverTextD68AE8 = L"回前一頁視窗";
            break;
        default:
            break;
        }
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        switch (mode) {
        case 0: {
            std::wstring escaped;
            escaped = input::EscapeTextAndNotify(L"2", 15);
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t y =
                world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t x =
                world::detail::BorrowPlayerXStorage()->value();
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 0, escaped));
            gCurrentWindow->state |= 0x8000u;
            break;
        }
        case 1: {
            std::wstring text;
            std::wstring escaped;
            cg::text::FormatTo(text, L"{}", 99);
            escaped = input::EscapeTextAndNotify(text, 15);
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t y =
                world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t x =
                world::detail::BorrowPlayerXStorage()->value();
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 0, escaped));
            gCurrentWindow->state |= 0x8000u;
            break;
        }
        case 2:
        case 12:
            window->state |= 0x8000u;
            break;
        case 3:
        case 4:
        case 13: {
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::wstring text = cg::text::FromGbkWide(input::LegacyText593850());
            const std::int32_t y =
                world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t x =
                world::detail::BorrowPlayerXStorage()->value();
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 2, text));
            world::detail::gValue581800 = -2;
            break;
        }
        default:
            break;
        }
        gWindow54LifecycleD67434 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        mode = std::bit_cast<std::int8_t>(gWindow54ModeC8AF64);
        consumed = 1;
    }
    switch (mode) {
    case 0:
    case 1:
    case 2:
    case 12:
        graphic->imageNo = 245320;
        if (hover) {
            graphic->imageNo = 245322;
        }
        if ((state & 8) != 0) {
            graphic->imageNo = 245321;
        }
        break;
    case 3:
    case 4:
    case 13:
        graphic->imageNo = 245332;
        if (hover) {
            graphic->imageNo = 245334;
        }
        if ((state & 8) != 0) {
            graphic->imageNo = 245333;
        }
        break;
    default:
        break;
    }
    return consumed;
}

// 原函数：0x004BFF60。
std::int32_t WindowPetSkillChoice_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        switch (std::bit_cast<std::int8_t>(gWindow54ModeC8AF64)) {
        case 0:
        case 1: {
            if (gWindow54PetScrollCC31DC > 0) {
                --gWindow54PetScrollCC31DC;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = gWindow54PetScrollCC31DC;
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t maximum = gWindow54ValueCB01E4 - 10;
            static_cast<void>(WindowScroll_SetPosition(window->elements + index - 1, maximum, position));
            const std::uint8_t mode = gWindow54ModeC8AF64;
            const std::int32_t count = gWindow54ValueCB01E4;
            WindowInstance* const current = gCurrentWindow;
            const std::int32_t scroll = gWindow54PetScrollCC31DC;
            if (mode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(current, scroll, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(current, scroll, count);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
            if (index <= 17) {
                if (gWindow54PetScrollCC31DC > 0) {
                    --gWindow54PetScrollCC31DC;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
                const std::int32_t position = gWindow54PetScrollCC31DC;
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t maximum = gWindow54ValueCB01E4 - 10;
                static_cast<void>(WindowScroll_SetPosition(window->elements + index - 1, maximum, position));
                const std::int32_t scroll = gWindow54PetScrollCC31DC;
                WindowPetSkillChoice_RefreshSelectedSkillRows(gCurrentWindow, scroll);
            } else {
                if (gWindow54SkillScrollCC31F4 > 0) {
                    --gWindow54SkillScrollCC31F4;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
                const std::int32_t position = gWindow54SkillScrollCC31F4;
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t maximum = gWindow54ValueCB01E8 - 5;
                static_cast<void>(WindowScroll_SetPosition(window->elements + index - 1, maximum, position));
                const std::int32_t scroll = gWindow54SkillScrollCC31F4;
                WindowPetSkillChoice_RefreshPetRows(gCurrentWindow, scroll);
            }
            break;
        default:
            break;
        }
        consumed = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return consumed;
}

// 原函数：0x004C0160。
std::int32_t WindowPetSkillChoice_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        switch (std::bit_cast<std::int8_t>(gWindow54ModeC8AF64)) {
        case 0:
        case 1: {
            const std::int32_t limit = gWindow54ValueCB01E4 - 10;
            if (gWindow54PetScrollCC31DC < limit) {
                ++gWindow54PetScrollCC31DC;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = gWindow54PetScrollCC31DC;
            const std::int32_t maximum = gWindow54ValueCB01E4 - 10;
            static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 2, maximum, position));
            const std::uint8_t mode = gWindow54ModeC8AF64;
            const std::int32_t scroll = gWindow54PetScrollCC31DC;
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t count = gWindow54ValueCB01E4;
            if (mode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(window, scroll, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(window, scroll, count);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
            if (index > 17) {
                const std::int32_t limit = gWindow54ValueCB01E8 - 5;
                if (gWindow54SkillScrollCC31F4 < limit) {
                    ++gWindow54SkillScrollCC31F4;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
                const std::int32_t position = gWindow54SkillScrollCC31F4;
                const std::int32_t maximum = gWindow54ValueCB01E8 - 5;
                static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 2, maximum, position));
                const std::int32_t scroll = gWindow54SkillScrollCC31F4;
                WindowPetSkillChoice_RefreshPetRows(gCurrentWindow, scroll);
                break;
            }
            [[fallthrough]];
        case 12:
        case 13: {
            const std::int32_t limit = gWindow54ValueCB01E4 - 10;
            if (gWindow54PetScrollCC31DC < limit) {
                ++gWindow54PetScrollCC31DC;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t position = gWindow54PetScrollCC31DC;
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t maximum = gWindow54ValueCB01E4 - 10;
            // 原4C0314为index-1；模式12/13也不按右侧元素改走另一分支。
            static_cast<void>(WindowScroll_SetPosition(window->elements + index - 1, maximum, position));
            const std::int32_t scroll = gWindow54PetScrollCC31DC;
            WindowPetSkillChoice_RefreshSelectedSkillRows(gCurrentWindow, scroll);
            break;
        }
        default:
            break;
        }
        consumed = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return consumed;
}

// 原函数：0x004C0380。
std::int32_t WindowPetSkillChoice_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        switch (std::bit_cast<std::int8_t>(gWindow54ModeC8AF64)) {
        case 0:
        case 1: {
            static_cast<void>(audio::PlaySe_52C450(gWindow54PetScrollCC31DC > 0 ? 51 : 55, 320));
            gWindow54PetScrollCC31DC -= 10;
            if (gWindow54PetScrollCC31DC < 0) {
                gWindow54PetScrollCC31DC = 0;
            }
            const std::int32_t position = gWindow54PetScrollCC31DC;
            const std::int32_t maximum = gWindow54ValueCB01E4 - 10;
            static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 3, maximum, position));
            const std::uint8_t mode = gWindow54ModeC8AF64;
            const std::int32_t count = gWindow54ValueCB01E4;
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t scroll = gWindow54PetScrollCC31DC;
            if (mode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(window, scroll, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(window, scroll, count);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
            if (index <= 17) {
                static_cast<void>(audio::PlaySe_52C450(gWindow54PetScrollCC31DC > 0 ? 51 : 55, 320));
                gWindow54PetScrollCC31DC -= 10;
                if (gWindow54PetScrollCC31DC < 0) {
                    gWindow54PetScrollCC31DC = 0;
                }
                const std::int32_t position = gWindow54PetScrollCC31DC;
                const std::int32_t maximum = gWindow54ValueCB01E4 - 10;
                static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 3, maximum, position));
                const std::int32_t scroll = gWindow54PetScrollCC31DC;
                WindowPetSkillChoice_RefreshSelectedSkillRows(gCurrentWindow, scroll);
            } else {
                static_cast<void>(audio::PlaySe_52C450(gWindow54SkillScrollCC31F4 > 0 ? 51 : 55, 320));
                gWindow54SkillScrollCC31F4 -= 5;
                if (gWindow54SkillScrollCC31F4 < 0) {
                    gWindow54SkillScrollCC31F4 = 0;
                }
                const std::int32_t position = gWindow54SkillScrollCC31F4;
                const std::int32_t maximum = gWindow54ValueCB01E8 - 5;
                static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 3, maximum, position));
                const std::int32_t scroll = gWindow54SkillScrollCC31F4;
                WindowPetSkillChoice_RefreshPetRows(gCurrentWindow, scroll);
            }
            break;
        default:
            break;
        }
        consumed = 1;
    }
    graphic->imageNo = 243030;
    if ((state & 1) != 0) {
        graphic->imageNo = 243032;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243031;
    }
    return consumed;
}

// 原函数：0x004C0590。
std::int32_t WindowPetSkillChoice_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        switch (std::bit_cast<std::int8_t>(gWindow54ModeC8AF64)) {
        case 0:
        case 1: {
            const std::int32_t limit = gWindow54ValueCB01E4 - 10;
            static_cast<void>(audio::PlaySe_52C450(
                limit > 0 && gWindow54PetScrollCC31DC < limit ? 51 : 55, 320));
            std::int32_t position = gWindow54PetScrollCC31DC;
            const std::int32_t maximum = gWindow54ValueCB01E4 - 10;
            position += 10;
            gWindow54PetScrollCC31DC = position;
            if (position > maximum) {
                position = maximum;
                gWindow54PetScrollCC31DC = position;
            }
            if (position < 0) {
                position = 0;
                gWindow54PetScrollCC31DC = position;
            }
            static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 4, maximum, position));
            const std::uint8_t mode = gWindow54ModeC8AF64;
            const std::int32_t scroll = gWindow54PetScrollCC31DC;
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t count = gWindow54ValueCB01E4;
            if (mode == 0) {
                WindowPetSkillChoice_RefreshLearnRows(window, scroll, count);
            } else {
                WindowPetSkillChoice_RefreshForgetRows(window, scroll, count);
            }
            break;
        }
        case 2:
        case 3:
        case 4:
        case 12:
        case 13:
            if (index <= 17) {
                const std::int32_t limit = gWindow54ValueCB01E4 - 10;
                static_cast<void>(audio::PlaySe_52C450(
                    limit > 0 && gWindow54PetScrollCC31DC < limit ? 51 : 55, 320));
                std::int32_t position = gWindow54PetScrollCC31DC;
                const std::int32_t maximum = gWindow54ValueCB01E4 - 10;
                position += 10;
                gWindow54PetScrollCC31DC = position;
                if (position > maximum) {
                    position = maximum;
                    gWindow54PetScrollCC31DC = position;
                }
                if (position < 0) {
                    position = 0;
                    gWindow54PetScrollCC31DC = position;
                }
                static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 4, maximum, position));
                const std::int32_t scroll = gWindow54PetScrollCC31DC;
                WindowPetSkillChoice_RefreshSelectedSkillRows(gCurrentWindow, scroll);
            } else {
                const std::int32_t limit = gWindow54ValueCB01E8 - 5;
                static_cast<void>(audio::PlaySe_52C450(
                    limit > 0 && gWindow54SkillScrollCC31F4 < limit ? 51 : 55, 320));
                std::int32_t position = gWindow54SkillScrollCC31F4;
                const std::int32_t maximum = gWindow54ValueCB01E8 - 5;
                position += 5;
                gWindow54SkillScrollCC31F4 = position;
                if (position > maximum) {
                    position = maximum;
                    gWindow54SkillScrollCC31F4 = position;
                }
                if (position < 0) {
                    position = 0;
                    gWindow54SkillScrollCC31F4 = position;
                }
                static_cast<void>(WindowScroll_SetPosition(gCurrentWindow->elements + index - 4, maximum, position));
                const std::int32_t scroll = gWindow54SkillScrollCC31F4;
                WindowPetSkillChoice_RefreshPetRows(gCurrentWindow, scroll);
            }
            break;
        default:
            break;
        }
        consumed = 1;
    }
    graphic->imageNo = 243033;
    if ((state & 1) != 0) {
        graphic->imageNo = 243035;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243034;
    }
    return consumed;
}

// 原函数：0x004C0800。
std::int32_t WindowPetSkillChoice_Scroll(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        switch (std::bit_cast<std::int8_t>(gWindow54ModeC8AF64)) {
        case 0:
        case 1: {
            const std::int32_t count = gWindow54ValueCB01E4;
            const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
            const std::int32_t position = gWindow54PetScrollCC31DC;
            gWindow54PetScrollCC31DC =
                WindowScroll_ApplyWheel(gCurrentWindow->elements + index - 5, position, count - 10, wheel);
            break;
        }
        case 2:
        case 3:
        case 4:
        case 12:
        case 13: {
            const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
            if (index <= 17) {
                const std::int32_t count = gWindow54ValueCB01E4;
                const std::int32_t position = gWindow54PetScrollCC31DC;
                const std::int32_t scroll =
                    WindowScroll_ApplyWheel(gCurrentWindow->elements + index - 5, position, count - 10, wheel);
                gWindow54PetScrollCC31DC = scroll;
                WindowPetSkillChoice_RefreshSelectedSkillRows(gCurrentWindow, scroll);
            } else {
                const std::int32_t count = gWindow54ValueCB01E8;
                const std::int32_t position = gWindow54SkillScrollCC31F4;
                const std::int32_t scroll =
                    WindowScroll_ApplyWheel(gCurrentWindow->elements + index - 5, position, count - 5, wheel);
                gWindow54SkillScrollCC31F4 = scroll;
                WindowPetSkillChoice_RefreshPetRows(gCurrentWindow, scroll);
            }
            break;
        }
        default:
            break;
        }
    }
    return 0;
}

// 原函数：0x004C0910。
std::int32_t WindowPetSkillChoice_SubmitForget(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t selected = gWindow54SkillMapCC4D04[gWindow54PetScrollCC31DC + index - 69];
    if (selected < 0 || gWindow54ValuesCB01A4[selected] < 0) {
        graphic->imageNo = 243104;
        return 0;
    }
    const bool hover = (state & 1) != 0;
    if (hover) {
        gHoverTextD68AE8 = L"遺忘此技能。";
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        std::wstring text;
        std::wstring escaped;
        cg::text::FormatTo(text, L"{}", selected + 1);
        escaped = input::EscapeTextAndNotify(text, 15);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t y =
            world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x =
            world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, escaped));
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 243103;
    if (hover) {
        graphic->imageNo = 243105;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243104;
    }
    return consumed;
}

// 原函数：0x004C0A50。
std::int32_t WindowPetSkillChoice_SelectPetCandidate(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t selected = gWindow54PetMapCF4AB0[gWindow54PetScrollCC31DC + index - 69];
    if (selected == 0) {
        gWindow54MessageC9FCE0 = -1;
    }
    // 原4C0A97：候选-1先读D67420的U16，别名为商店价格文本D673EC+34。
    std::uint16_t used;
    if (selected == -1) {
        used = gShopPriceTailD67420;
    } else {
        used = gWindow54PetOptionsD67448[selected].used;
    }
    if (used == 0) {
        graphic->imageNo = 243101;
        return 0;
    }
    const Window54PetOption& option = gWindow54PetOptionsD67448[selected];
    if (option.allowed == 0) {
        graphic->imageNo = 243101;
        return 0;
    }
    const bool hover = (state & 1) != 0;
    const std::int32_t requiredTribe = gWindow54TribeCA0B2C;
    if (requiredTribe >= 0 && option.tribe != requiredTribe) {
        if (hover) {
            gWindow54MessageC9FCE0 = -2;
        }
        graphic->imageNo = 243101;
        return 0;
    }
    // 此标记只改变悬停消息，原端仍允许下面的点击发送。
    if (option.hintFlag == 0 && hover) {
        gWindow54MessageC9FCE0 = -3;
    }
    if (hover) {
        gHoverTextD68AE8 = L"讓寵物學習此技能。";
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        gWindow54PetChoiceC8AA90 = selected;
        std::wstring text;
        std::wstring escaped;
        cg::text::FormatTo(text, L"{}", selected);
        escaped = input::EscapeTextAndNotify(text, 15);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t y =
            world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x =
            world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, escaped));
        world::detail::gValue581800 = -2;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 243100;
    if (hover) {
        graphic->imageNo = 243102;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243101;
    }
    return consumed;
}

// 原函数：0x004C0BF0。
std::int32_t WindowPetSkillChoice_SelectSkillSlot(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    const bool hover = (state & 1) != 0;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    const std::int32_t slot = gWindow54PetScrollCC31DC + index - 69;
    if (hover) {
        gHoverTextD68AE8 = L"把要學習的技能放在這一欄。";
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        gWindow54ReplacementC8B08C = slot;
        window->elements[0].visible = 1;
        gCurrentWindow->state |= 2u;
        static_cast<void>(audio::PlaySe_52C450(78, 320));
        consumed = 1;
    }
    graphic->imageNo = 243127;
    if (hover) {
        graphic->imageNo = 243129;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243128;
    }
    return consumed;
}

// 原函数：0x004C0CB0。
std::int32_t WindowPetSkillChoice_SubmitPetSkill(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t selected = gWindow54SkillScrollCC31F4 + index - 99;
    const Window54SkillRecord& record = gWindow54SkillsD2EAB8[selected];
    if (record.name[0] == '\0') {
        return 0;
    }
    if ((state & 1) != 0) {
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t hoverImage = login::detail::gValueD69768;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = window->x + element.x;
        const std::int32_t y = window->y + element.y;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
            static_cast<std::uint16_t>(x + element.width),
            static_cast<std::uint16_t>(y + element.height), 223, hoverImage, 0);
        if (gWindow54MessageC9FCE0 >= -1 || gWindow54HoverD2DAD8 != selected) {
            gWindow54MessageC9FCE0 = selected;
        }
        constexpr wchar_t hint[] =
            L"把此技能讓寵物學習。";
        gWindow54HoverD2DAD8 = selected;
        gHoverTextD68AE8 = hint;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    const std::int32_t money = world::detail::ReadPlayerValueFC();
    if (money < record.price) {
        gWindow54MessageC9FCE0 = -2;
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return 1;
    }
    std::int32_t petCount = 0;
    for (const world::detail::PetRuntimeRecord& pet : (*world::detail::BorrowPetRuntimeF633F8Storage())) {
        if (pet.used != 0) {
            ++petCount;
        }
    }
    if (petCount == 0) {
        gWindow54MessageC9FCE0 = -3;
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return 1;
    }
    gWindow54SelectedD2DC04 = selected;
    std::wstring text;
    std::wstring escaped;
    cg::text::FormatTo(text, L"{}", selected);
    escaped = input::EscapeTextAndNotify(text, 15);
    const std::int32_t target = world::detail::gDialogTargetD2DE6C;
    const std::int32_t request = world::detail::gDialogRequestCC21E0;
    const std::int32_t y =
        world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x =
        world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(network::Sub_512620(
        platform::gConnectionSocket, x, y, request, target, 0, escaped));
    world::detail::gValue581800 = -2;
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 1;
}

// 原函数：0x004C0EA0。
std::int32_t WindowPetSkillChoice_DrawSelectedSkillFrame(const std::int32_t index, const std::int32_t unusedState)
{
    static_cast<void>(unusedState);
    const std::int32_t selected = gWindow54SkillScrollCC31F4 + index - 99;
    if (gWindow54SkillsD2EAB8[selected].name[0] != '\0' && gWindow54SelectedD2DC04 == selected) {
        WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = window->x + element.x;
        const std::int32_t y = window->y + element.y;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
            static_cast<std::uint16_t>(x + element.width - 2),
            static_cast<std::uint16_t>(y + element.height - 2), 223, 254, 0);
    }
    return 0;
}

// 原函数：0x004C0F20。
std::int32_t WindowPetSkillChoice_HandleConfirmation(const std::int32_t index, const std::int32_t unusedState)
{
    static_cast<void>(unusedState);
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const bool acceptHover = login::detail::Sub_5049A0(240, 310, 306, 327, -1) != 0;
    const bool cancelHover = login::detail::Sub_5049A0(334, 310, 400, 327, -1) != 0;
    std::int32_t acceptImage = 245323;
    if (acceptHover) {
        acceptImage = 245325;
        const std::size_t length = gConfirmationAcceptHintCC3FCC.size() + 1;
        const auto buttons = static_cast<std::uint8_t>(platform::BorrowLegacyMouseStateStorage()->edgeButtons);
        gHoverTextD68AE8 = gConfirmationAcceptHintCC3FCC;
        if ((buttons & 1u) != 0) {
            gCurrentWindow->elements[index].visible = 0;
            std::wstring requestText;
            cg::text::FormatTo(requestText, L"{}", gWindow54ReplacementC8B08C);
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t y =
                world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t x =
                world::detail::BorrowPlayerXStorage()->value();
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 0, requestText));
            gCurrentWindow->state &= ~2u;
            acceptImage = 245324;
            gCurrentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    std::int32_t cancelImage = 245326;
    if (cancelHover) {
        cancelImage = 245328;
        const std::size_t length = gConfirmationCancelHintD45D70.size() + 1;
        const std::size_t prefix = length & ~std::size_t{3};
        gHoverTextD68AE8 = gConfirmationCancelHintD45D70;
        const auto buttons = static_cast<std::uint8_t>(platform::BorrowLegacyMouseStateStorage()->edgeButtons);
        static_cast<void>(0);
        if ((buttons & 1u) != 0) {
            cancelImage = 245327;
            gCurrentWindow->elements[index].visible = 0;
            gCurrentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    static_cast<void>(render::DrawGraphic3_520970(240, 310, 226, acceptImage, 0, 1, 0, &descriptor));
    static_cast<void>(render::DrawGraphic3_520970(334, 310, 226, cancelImage, 0, 1, 0, &descriptor));
    constexpr wchar_t learnFormat588344[] = L"學習『{}』";
    constexpr wchar_t petFormat588338[] = L"讓『{}』";
    constexpr wchar_t petMeasure588320[] =
        L"讓『1234567890123456』";
    constexpr wchar_t slotFormat58830C[] =
        L"使用『{}』的位置";
    constexpr wchar_t emptySlot5882F4[] =
        L"使用『空欄位』的位置";
    constexpr wchar_t learnSuffix5882E8[] = L"來學習。";
    constexpr wchar_t moneyFormat5882D4[] =
        L"所持金　 {:7}金幣";
    constexpr wchar_t priceFormat5882C0[] =
        L"購入金額 {:7}金幣";
    constexpr wchar_t question5882B0[] = L"這樣可以嗎？";
    std::wstring buffer;
    cg::text::FormatTo(buffer, learnFormat588344, cg::text::FromGbkWide(gWindow54SkillsD2EAB8[gWindow54SelectedD2DC04].name.data()).c_str());
    std::int32_t width = login::detail::TextMeasure(buffer, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(320 - width / 2), 130, 4, 0, 0, buffer, 0, 0));
    cg::text::FormatTo(buffer, petFormat588338, cg::text::FromGbkWide(gWindow54PetOptionsD67448[gWindow54PetChoiceC8AA90].name.data()).c_str());
    // 原第二行按固定16位示例名测宽，不按实际候选名测量。
    width = login::detail::TextMeasure(petMeasure588320, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(320 - width / 2), 154, 4, 0, 0, buffer, 0, 0));
    const Window54TechniqueSlot& slot = gWindow54TechniqueSlotsD66E38[gWindow54ReplacementC8B08C];
    if (slot.used != 0) {
        cg::text::FormatTo(buffer, slotFormat58830C, cg::text::FromGbkWide(slot.name.data()).c_str());
    } else {
        buffer = emptySlot5882F4;
    }
    width = login::detail::TextMeasure(buffer, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(320 - width / 2), 178, 4, 0, 0, buffer, 0, 0));
    width = login::detail::TextMeasure(learnSuffix5882E8, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(320 - width / 2), 198, 4, 0, 0, learnSuffix5882E8, 0, 0));
    cg::text::FormatTo(buffer, moneyFormat5882D4, world::detail::ReadPlayerValueFC());
    static_cast<void>(render::DrawText_456780(245, 230, 4, 0, 0, buffer, 0, 0));
    cg::text::FormatTo(buffer, priceFormat5882C0, gWindow54SkillsD2EAB8[gWindow54SelectedD2DC04].price);
    static_cast<void>(render::DrawText_456780(245, 250, 4, 0, 0, buffer, 0, 0));
    static_cast<void>(render::DrawText_456780(252, 282, 4, 0, 0, question5882B0, 0, 0));
    login::detail::DrawNineGrid_49BE00(
        245300, 160, 110, 320, 240, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

} // namespace detail

} // namespace cg::ui

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic574668 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574668。

WindowGraphicData gWindowGraphic574680 {244383, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574680。

WindowGraphicData gWindowGraphic574698 {244384, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574698。

WindowGraphicData gWindowGraphic5746B0 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746B0。

WindowGraphicData gWindowGraphic5746C8 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746C8。

WindowGraphicData gWindowGraphic5746E0 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746E0。

WindowGraphicData gWindowGraphic5746F8 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746F8。

WindowGraphicData gWindowGraphic574710 {245320, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574710。

WindowGraphicData gWindowGraphic574728 {244387, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574728。

WindowGraphicData gWindowGraphic574740 {243103, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574740。

WindowGraphicData gWindowGraphic574758 {244386, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574758。

WindowGraphicData gWindowGraphic574770 {243127, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574770。

WindowGraphicData gWindowGraphic574788 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574788。

WindowGraphicData gWindowGraphic5747A0 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005747A0。

WindowTextPrototype gWindowText5747C0 { // 原全局：0x005747C0。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

WindowTextPrototype gWindowText5747C8 { // 原全局：0x005747C8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

WindowTextPrototype gWindowText5747D0 { // 原全局：0x005747D0。
    {std::byte{0x8A}, std::byte{1}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

WindowTextPrototype gWindowText5747D8 { // 原全局：0x005747D8。
    {std::byte{0x8A}, std::byte{8}, std::byte{0}, std::byte{0}}, L"0000000000", // 原常量参考：0x00585DF4。
};

std::int32_t gWindowAnimationImage5747E0 = 100000; // 原全局：0x005747E0。

WindowNumberData gWindowNumber5747E8 {&gWindow54MoneyTextC9A9A4, 0, 3, 1, 0}; // 原全局：0x005747E8。

WindowNumberData gWindowNumber5747F0 {&gWindow54AmountTextD45F78, 0, 3, 1, 0}; // 原全局：0x005747F0。

WindowNumberData gWindowNumber5747F8 {&gWindow54SkillValueTextsCF4978[0], 0, 0, 1, 0}; // 原全局：0x005747F8。

WindowNumberData gWindowNumber574800 {&gWindow54SkillValueTextsCF4978[1], 0, 0, 1, 0}; // 原全局：0x00574800。

WindowNumberData gWindowNumber574808 {&gWindow54SkillValueTextsCF4978[2], 0, 0, 1, 0}; // 原全局：0x00574808。

WindowNumberData gWindowNumber574810 {&gWindow54SkillValueTextsCF4978[3], 0, 0, 1, 0}; // 原全局：0x00574810。

WindowNumberData gWindowNumber574818 {&gWindow54SkillValueTextsCF4978[4], 0, 0, 1, 0}; // 原全局：0x00574818。

WindowNumberData gWindowNumber574820 {&gWindow54SkillPriceTextsCF493C[0], 0, 3, 1, 0}; // 原全局：0x00574820。

WindowNumberData gWindowNumber574828 {&gWindow54SkillPriceTextsCF493C[1], 0, 3, 1, 0}; // 原全局：0x00574828。

WindowNumberData gWindowNumber574830 {&gWindow54SkillPriceTextsCF493C[2], 0, 3, 1, 0}; // 原全局：0x00574830。

WindowNumberData gWindowNumber574838 {&gWindow54SkillPriceTextsCF493C[3], 0, 3, 1, 0}; // 原全局：0x00574838。

WindowNumberData gWindowNumber574840 {&gWindow54SkillPriceTextsCF493C[4], 0, 3, 1, 0}; // 原全局：0x00574840。

WindowNumberData gWindowNumber574848 {&gWindow54ValueTextsCB035C[0], 0, 0, 1, 0}; // 原全局：0x00574848。

WindowNumberData gWindowNumber574850 {&gWindow54ValueTextsCB035C[1], 0, 0, 1, 0}; // 原全局：0x00574850。

WindowNumberData gWindowNumber574858 {&gWindow54ValueTextsCB035C[2], 0, 0, 1, 0}; // 原全局：0x00574858。

WindowNumberData gWindowNumber574860 {&gWindow54ValueTextsCB035C[3], 0, 0, 1, 0}; // 原全局：0x00574860。

WindowNumberData gWindowNumber574868 {&gWindow54ValueTextsCB035C[4], 0, 0, 1, 0}; // 原全局：0x00574868。

WindowNumberData gWindowNumber574870 {&gWindow54ValueTextsCB035C[5], 0, 0, 1, 0}; // 原全局：0x00574870。

WindowNumberData gWindowNumber574878 {&gWindow54ValueTextsCB035C[6], 0, 0, 1, 0}; // 原全局：0x00574878。

WindowNumberData gWindowNumber574880 {&gWindow54ValueTextsCB035C[7], 0, 0, 1, 0}; // 原全局：0x00574880。

WindowNumberData gWindowNumber574888 {&gWindow54ValueTextsCB035C[8], 0, 0, 1, 0}; // 原全局：0x00574888。

WindowNumberData gWindowNumber574890 {&gWindow54ValueTextsCB035C[9], 0, 0, 1, 0}; // 原全局：0x00574890。

WindowNumberData gWindowNumber574898 {&gWindow54ValueTextsC8B318[0], 0, 0, 1, 0}; // 原全局：0x00574898。

WindowNumberData gWindowNumber5748A0 {&gWindow54ValueTextsC8B318[1], 0, 0, 1, 0}; // 原全局：0x005748A0。

WindowNumberData gWindowNumber5748A8 {&gWindow54ValueTextsC8B318[2], 0, 0, 1, 0}; // 原全局：0x005748A8。

WindowNumberData gWindowNumber5748B0 {&gWindow54ValueTextsC8B318[3], 0, 0, 1, 0}; // 原全局：0x005748B0。

WindowNumberData gWindowNumber5748B8 {&gWindow54ValueTextsC8B318[4], 0, 0, 1, 0}; // 原全局：0x005748B8。

WindowNumberData gWindowNumber5748C0 {&gWindow54ValueTextsC8B318[5], 0, 0, 1, 0}; // 原全局：0x005748C0。

WindowNumberData gWindowNumber5748C8 {&gWindow54ValueTextsC8B318[6], 0, 0, 1, 0}; // 原全局：0x005748C8。

WindowNumberData gWindowNumber5748D0 {&gWindow54ValueTextsC8B318[7], 0, 0, 1, 0}; // 原全局：0x005748D0。

WindowNumberData gWindowNumber5748D8 {&gWindow54ValueTextsC8B318[8], 0, 0, 1, 0}; // 原全局：0x005748D8。

WindowNumberData gWindowNumber5748E0 {&gWindow54ValueTextsC8B318[9], 0, 0, 1, 0}; // 原全局：0x005748E0。

std::uint32_t gWindowMouseFlagsD67900 = 0; // 原全局：0x00D67900。

} // namespace

// 原型：窗口54；原全局：0x0054EC90。
WindowPrototype gWindowPrototypePetSkillChoice {0, 4, 10, 30, 549, 366, 0x80010101u, 107, gWindowElements5748E8, &WindowPetSkillChoice_Update, &WindowPetSkillChoice_Draw, &WindowPetSkillChoice_Release}; // 54: 54EC90

// 原5748E8的107项；选择回调由模式刷新发布，不替换初始4B0F10。
WindowElementPrototype gWindowElements5748E8[107] {
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowPetSkillChoice_HandleConfirmation},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic574668}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 203, 326, 66, 17, 1, {.graphic = &gWindowGraphic5746F8}, &WindowPetSkillChoice_ButtonLearn},
    {WindowElementType_Graphic, 467, 326, 66, 17, 1, {.graphic = &gWindowGraphic574710}, &WindowPetSkillChoice_ButtonBack},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText5747C0}, &WindowElement_NoOp},
    {WindowElementType_Text, 78, 52, 0, 14, 1, {.text = &gWindowText5747D8}, &WindowElement_NoOp},
    {WindowElementType_Number, 262, 37, 0, 14, 1, {.number = &gWindowNumber5747F0}, &WindowElement_NoOp},
    {WindowElementType_Number, 389, 30, 0, 14, 1, {.number = &gWindowNumber5747E8}, &WindowElement_NoOp},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5747E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 502, 146, 0, 14, 1, {.graphic = &gWindowGraphic5746B0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 502, 146, 11, 137, 1, {.mouseFlags = &gWindowMouseFlagsD67900}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 502, 136, 11, 11, 1, {.graphic = &gWindowGraphic5746C8}, &WindowPetSkillChoice_ButtonPreviousRow},
    {WindowElementType_Graphic, 502, 282, 11, 11, 1, {.graphic = &gWindowGraphic5746E0}, &WindowPetSkillChoice_ButtonNextRow},
    {WindowElementType_Graphic, 472, 300, 18, 18, 1, {.graphic = &gWindowGraphic574788}, &WindowPetSkillChoice_ButtonPreviousPage},
    {WindowElementType_Graphic, 498, 300, 18, 18, 1, {.graphic = &gWindowGraphic5747A0}, &WindowPetSkillChoice_ButtonNextPage},
    {WindowElementType_EventArea, 286, 27, 273, 280, 1, {.unusedValue = 0}, &WindowPetSkillChoice_Scroll},
    {WindowElementType_Graphic, 499, 132, 11, 11, 1, {.graphic = &gWindowGraphic574728}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 85, 0, 14, 1, {.graphic = &gWindowGraphic5746B0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 269, 85, 11, 244, 1, {.mouseFlags = &gWindowMouseFlagsD67900}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 269, 75, 11, 11, 1, {.graphic = &gWindowGraphic5746C8}, &WindowPetSkillChoice_ButtonPreviousRow},
    {WindowElementType_Graphic, 269, 327, 11, 11, 1, {.graphic = &gWindowGraphic5746E0}, &WindowPetSkillChoice_ButtonNextRow},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic574788}, &WindowPetSkillChoice_ButtonPreviousPage},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5747A0}, &WindowPetSkillChoice_ButtonNextPage},
    {WindowElementType_EventArea, 17, 28, 274, 325, 1, {.unusedValue = 0}, &WindowPetSkillChoice_Scroll},
    {WindowElementType_Text, 33, 87, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 127, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 147, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 167, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 187, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 207, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 227, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 247, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 267, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 287, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 135, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 151, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 167, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 183, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 199, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 215, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 231, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 247, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 263, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 279, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 135, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 151, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 167, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 183, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 199, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 136, 0, 0, 1, {.number = &gWindowNumber574848}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 152, 0, 0, 1, {.number = &gWindowNumber574850}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 168, 0, 0, 1, {.number = &gWindowNumber574858}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 184, 0, 0, 1, {.number = &gWindowNumber574860}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 200, 0, 0, 1, {.number = &gWindowNumber574868}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 216, 0, 0, 1, {.number = &gWindowNumber574870}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 232, 0, 0, 1, {.number = &gWindowNumber574878}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 248, 0, 0, 1, {.number = &gWindowNumber574880}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 264, 0, 0, 1, {.number = &gWindowNumber574888}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 280, 0, 0, 1, {.number = &gWindowNumber574890}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 136, 0, 0, 1, {.number = &gWindowNumber574898}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 152, 0, 0, 1, {.number = &gWindowNumber5748A0}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 168, 0, 0, 1, {.number = &gWindowNumber5748A8}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 184, 0, 0, 1, {.number = &gWindowNumber5748B0}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 200, 0, 0, 1, {.number = &gWindowNumber5748B8}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 216, 0, 0, 1, {.number = &gWindowNumber5748C0}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 232, 0, 0, 1, {.number = &gWindowNumber5748C8}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 248, 0, 0, 1, {.number = &gWindowNumber5748D0}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 264, 0, 0, 1, {.number = &gWindowNumber5748D8}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 280, 0, 0, 1, {.number = &gWindowNumber5748E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 133, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 149, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 165, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 181, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 197, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 213, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 229, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 245, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 261, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 277, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 82, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 137, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 192, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 247, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 302, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 83, 0, 14, 1, {.number = &gWindowNumber5747F8}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 138, 0, 14, 1, {.number = &gWindowNumber574800}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 193, 0, 14, 1, {.number = &gWindowNumber574808}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 248, 0, 14, 1, {.number = &gWindowNumber574810}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 303, 0, 14, 1, {.number = &gWindowNumber574818}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 108, 0, 14, 1, {.number = &gWindowNumber574820}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 163, 0, 14, 1, {.number = &gWindowNumber574828}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 218, 0, 14, 1, {.number = &gWindowNumber574830}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 273, 0, 14, 1, {.number = &gWindowNumber574838}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 328, 0, 14, 1, {.number = &gWindowNumber574840}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 80, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 135, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 190, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 245, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 300, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 70, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 125, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 180, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 235, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 290, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 17, 28, 274, 325, 1, {.graphic = &gWindowGraphic574698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic574680}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

} // namespace cg::ui::detail
