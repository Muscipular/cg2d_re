#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {


constexpr std::array<std::int32_t, 2> kAlbumMarkImages57DF24 {0x3BB0F, 0x3BB0E}; // 原常量参考：0x0057DF24。

constexpr std::array<std::int32_t, 3> kAlbumMarkImages57DF2C {0, 0x3BB10, 0x3BB11}; // 原常量参考：0x0057DF2C。

constexpr wchar_t kAlbumPreviousTen586DC8[] = L"快速切換至後10頁視窗。"; // 原常量参考：0x00586DC8。

constexpr wchar_t kAlbumPrevious586DB4[] = L"切換至後一頁視窗。"; // 原常量参考：0x00586DB4。

constexpr wchar_t kAlbumNext586DA0[] = L"切換至前一頁視窗。"; // 原常量参考：0x00586DA0。

constexpr wchar_t kAlbumNextTen586D88[] = L"快速切換至前10頁視窗。"; // 原常量参考：0x00586D88。
} // namespace


// 原评级参数是S32，不是反编译的载荷指针；五个caller均不消费混合返回。
// 原函数：0x004939A0。
void WindowAlbumDetail_DrawRating(const std::int32_t rating, const std::int32_t base)
{
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        gCurrentWindow->elements[base + slot].visible = 0;
    }
    const std::int32_t count = rating + 1;
    if (count < 1 || count > 10 || count <= 0) {
        return;
    }
    for (std::int32_t part = 0; part < count; ++part) {
        if (part % 2 == 0) {
            const std::int32_t element = base + part / 2;
            gCurrentWindow->elements[element].visible = 1;
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[element].data)->imageNo = 0x3BB0C;
        }
        if (part == count - 1 && count % 2 == 1) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[base + part / 2].data)->imageNo = 0x3BB0D;
        }
    }
}

// 唯一caller492410不消费原混合返回；保留各消费点独立的角色/记录取样。
// 原函数：0x00492F70。
void WindowAlbumDetail_Refresh()
{
    const auto selectedRecord = []() {
        const std::byte* const records = world::detail::BorrowSelectedAlbumRecords();
        const std::int32_t index = gLoginWindowRuntime.valueCB2200;
        return records + index * 0x80;
    };
    const auto readValue = [](const std::byte* const record, const std::size_t offset) {
        std::int32_t value;
        std::memcpy(&value, record + offset, sizeof(value));
        return value;
    };
    const auto copyBeforeScan = [](const std::int32_t index, const std::wstring_view source) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
        data->text = source;
    };
    const auto copyAfterScan = [](const std::int32_t index, const std::wstring_view source) {
        WindowElement* const elements = gCurrentWindow->elements;
        auto* const data = static_cast<WindowTextData*>(elements[index].data);
        data->text = source;
    };
    std::wstring buffer;
    if (selectedRecord()[0] != std::byte{0}) {
        constexpr std::int32_t shown[] {2, 1, 3, 4, 5, 7, 8};
        for (const std::int32_t element : shown) {
            gCurrentWindow->elements[element].visible = 1;
        }
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            gCurrentWindow->elements[53 + slot].visible = 1;
            gCurrentWindow->elements[58 + slot].visible = 1;
            gCurrentWindow->elements[63 + slot].visible = 1;
            gCurrentWindow->elements[68 + slot].visible = 1;
            gCurrentWindow->elements[73 + slot].visible = 1;
        }
        for (std::int32_t slot = 0; slot < 10; ++slot) {
            gCurrentWindow->elements[13 + slot].visible = 1;
            gCurrentWindow->elements[23 + slot].visible = 1;
            gCurrentWindow->elements[33 + slot].visible = 1;
            gCurrentWindow->elements[43 + slot].visible = 1;
        }
        const std::int16_t nameCharacter = login::gSelectedCharacterIndex;
        const std::int32_t nameIndex = gLoginWindowRuntime.valueCB2200;
        const char* const name = world::detail::ClearAlbumNewFlagAndBorrowName(nameCharacter, nameIndex);
        copyBeforeScan(2, cg::text::FromGbkWide(name));

        const std::int32_t image = readValue(selectedRecord(), 8);
        const std::int32_t mapped = character_select::detail::Sub_51B740(image);
        auto* const imageData = static_cast<WindowAnimationData*>(gCurrentWindow->elements[1].data);
        login::detail::SetYobiActionField2A8(imageData->action, mapped);
        auto* const directionData = static_cast<WindowAnimationData*>(gCurrentWindow->elements[1].data);
        const std::int32_t direction = gLoginWindowRuntime.valueCF4A00;
        login::detail::SetYobiActionField2B8(directionData->action, direction);

        buffer = std::format(L"{:3}", gLoginWindowRuntime.valueCB2200 + 1);
        copyAfterScan(3, buffer);
        buffer = std::format(L"{:2}", readValue(selectedRecord(), 0x30));
        copyAfterScan(4, buffer);
        const std::byte* const tribeRecord = selectedRecord();
        WindowInstance* const tribeWindow = gCurrentWindow;
        const auto tribe = std::bit_cast<std::int8_t>(tribeRecord[3]);
        WindowElement* const tribeElements = tribeWindow->elements;
        const wchar_t* const tribeName = gTribeNames57CB54[tribe];
        auto* const tribeData = static_cast<WindowTextData*>(tribeElements[5].data);
        tribeData->text = tribeName;
        buffer = std::format(L"{:3}/{:3}", gLoginWindowRuntime.valueCB2200 + 1, 960);
        copyBeforeScan(6, buffer);

        const std::byte* const firstMarkRecord = selectedRecord();
        WindowInstance* const firstMarkWindow = gCurrentWindow;
        const auto firstMark = std::bit_cast<std::int8_t>(firstMarkRecord[1]);
        auto* const firstMarkData = static_cast<WindowGraphicData*>(firstMarkWindow->elements[7].data);
        firstMarkData->imageNo = kAlbumMarkImages57DF24[firstMark];
        WindowInstance* const secondMarkWindow = gCurrentWindow;
        const std::byte* const secondMarkRecord = selectedRecord();
        const auto secondMark = std::bit_cast<std::int8_t>(secondMarkRecord[2]);
        auto* const secondMarkData = static_cast<WindowGraphicData*>(secondMarkWindow->elements[8].data);
        secondMarkData->imageNo = kAlbumMarkImages57DF2C[secondMark];

        const std::int32_t firstRatingIndex = gLoginWindowRuntime.valueCB2200;
        const std::byte* const firstRatingRecords = world::detail::BorrowSelectedAlbumRecords();
        WindowAlbumDetail_DrawRating(readValue(firstRatingRecords + firstRatingIndex * 0x80, 0x0C), 53);
        for (std::int32_t rating = 1; rating < 5; ++rating) {
            WindowAlbumDetail_DrawRating(readValue(selectedRecord(), 0x0C + rating * 4), 53 + rating * 5);
        }
        const auto showSegment = [](const std::int32_t element, const std::int32_t graphic) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[element].data)->imageNo = graphic;
            gCurrentWindow->elements[element].visible = 1;
        };
        for (std::int32_t attribute = 0; attribute < 4; ++attribute) {
            const std::int32_t base = 13 + attribute * 10;
            for (std::int32_t slot = 0; slot < 10; ++slot) {
                gCurrentWindow->elements[base + slot].visible = 0;
            }
            const std::int32_t segments = readValue(selectedRecord(), 0x20 + attribute * 4);
            const auto& graphics = gPetBarImages57E434[attribute];
            if (segments == 1) {
                showSegment(base, graphics[3]);
            } else {
                for (std::int32_t segment = 0; segment < segments; ++segment) {
                    showSegment(base + segment, graphics[1]);
                    if (segment == 0) {
                        showSegment(base, graphics[0]);
                    }
                    if (segment == segments - 1) {
                        showSegment(base + segment, graphics[2]);
                    }
                }
            }
        }
    } else {
        constexpr std::int32_t hidden[] {2, 1, 4, 5, 7, 8};
        for (const std::int32_t element : hidden) {
            gCurrentWindow->elements[element].visible = 0;
        }
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            gCurrentWindow->elements[53 + slot].visible = 0;
            gCurrentWindow->elements[58 + slot].visible = 0;
            gCurrentWindow->elements[63 + slot].visible = 0;
            gCurrentWindow->elements[68 + slot].visible = 0;
            gCurrentWindow->elements[73 + slot].visible = 0;
        }
        for (std::int32_t slot = 0; slot < 10; ++slot) {
            gCurrentWindow->elements[13 + slot].visible = 0;
            gCurrentWindow->elements[23 + slot].visible = 0;
            gCurrentWindow->elements[33 + slot].visible = 0;
            gCurrentWindow->elements[43 + slot].visible = 0;
        }
        buffer = std::format(L"{:3}", gLoginWindowRuntime.valueCB2200 + 1);
        copyBeforeScan(3, buffer);
        buffer = std::format(L"{:3}/{:3}", gLoginWindowRuntime.valueCB2200 + 1, 960);
        copyBeforeScan(6, buffer);
    }
}

// 原函数：0x00492410。
std::int32_t WindowAlbumDetail_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valueCF4A00 = 5;
    }
    WindowAlbumDetail_Refresh();
    return 1;
}

// 原函数：0x00492880。
std::int32_t WindowAlbumDetail_HandleNavigation(const std::int32_t index, const std::int32_t state)
{
    std::int32_t delta;
    std::int32_t idle;
    const wchar_t* hint;
    switch (index) {
    case 9:
        delta = -10;
        idle = 0x3B55C;
        hint = kAlbumPreviousTen586DC8;
        break;
    case 10:
        delta = -1;
        idle = 0x3BE5D;
        hint = kAlbumPrevious586DB4;
        break;
    case 11:
        delta = 1;
        idle = 0x3BE5A;
        hint = kAlbumNext586DA0;
        break;
    case 12:
        delta = 10;
        idle = 0x3B55F;
        hint = kAlbumNextTen586D88;
        break;
    case 80:
        if ((state & 0x400) != 0) {
            const std::int32_t direction = gLoginWindowRuntime.valueCF4A00 + 1;
            gLoginWindowRuntime.valueCF4A00 = direction;
            if (direction > 7) {
                gLoginWindowRuntime.valueCF4A00 = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & 0x800) != 0) {
            const std::int32_t direction = gLoginWindowRuntime.valueCF4A00 - 1;
            gLoginWindowRuntime.valueCF4A00 = direction;
            if (direction < 0) {
                gLoginWindowRuntime.valueCF4A00 = 7;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        return 0;
    default:
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = hint;
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = idle + 2;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = idle;
    }
    if ((state & 0x400) != 0) {
        const std::int32_t selected = gLoginWindowRuntime.valueCB2200 + delta;
        gLoginWindowRuntime.valueCB2200 = selected;
        if (delta < 0) {
            if (selected < 0) {
                gLoginWindowRuntime.valueCB2200 = 0;
            }
        } else if (selected > 959) {
            gLoginWindowRuntime.valueCB2200 = 959;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = idle + 1;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic5679B0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679B0。

WindowGraphicData gWindowGraphic5679C8 {0x3BB0A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679C8。

WindowGraphicData gWindowGraphic5679E0 {0x3BB0B, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005679E0。

WindowGraphicData gWindowGraphic5679F8 {0x3BB0B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679F8。

WindowGraphicData gWindowGraphic567A10 {0x3BB0C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A10。

WindowGraphicData gWindowGraphic567A40 {0x3B55C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A40。

WindowGraphicData gWindowGraphic567A58 {0x3BE5D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A58。

WindowGraphicData gWindowGraphic567A70 {0x3BE5A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A70。

WindowGraphicData gWindowGraphic567A88 {0x3B55F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A88。

WindowGraphicData gWindowGraphic567AA0 {0x3BAC7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567AA0。

WindowTextPrototype gWindowText567AB8 { // 原全局：0x00567AB8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"寵物名稱", // 原常量参考：0x00586048。
};

WindowTextPrototype gWindowText567AC0 { // 原全局：0x00567AC0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"000", // 原常量参考：0x00585FEC。
};

WindowTextPrototype gWindowText567AC8 { // 原全局：0x00567AC8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"00", // 原常量参考：0x00585FE8。
};

WindowTextPrototype gWindowText567AD0 { // 原全局：0x00567AD0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"種類", // 原常量参考：0x00586018。
};

WindowTextPrototype gWindowText567AD8 { // 原全局：0x00567AD8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"255/255", // 原常量参考：0x00585FE0。
};

std::uint32_t gWindowAnimationImage567AE0 = 100000; // 原全局：0x00567AE0。

std::uint32_t gWindowMouseFlagsD677D0 = 0; // 原全局：0x00D677D0。

} // namespace

// 原型：窗口20；原全局：0x0054E630。
WindowPrototype gWindowPrototypeAlbumDetail {0, 4, 357, 90, 256, 289, 0x80010101u, 82, gWindowElements567AE8, &WindowAlbumDetail_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 20: 54E630

WindowElementPrototype gWindowElements567AE8[82] { // 原全局：0x00567AE8。
    {WindowElementType_Graphic, 235, 8, 11, 11, 1, {.graphic = &gWindowGraphic5679B0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Animation, 186, 135, 0, 0, 1, {.animationImageNo = reinterpret_cast<std::int32_t*>(&gWindowAnimationImage567AE0)}, &WindowElement_NoOp},
    {WindowElementType_Text, 60, 28, 0, 0, 1, {.text = &gWindowText567AB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 214, 28, 0, 0, 1, {.text = &gWindowText567AC0}, &WindowElement_NoOp},
    {WindowElementType_Text, 95, 47, 0, 0, 1, {.text = &gWindowText567AC8}, &WindowElement_NoOp},
    {WindowElementType_Text, 63, 63, 0, 0, 1, {.text = &gWindowText567AD0}, &WindowElement_NoOp},
    {WindowElementType_Text, 48, 262, 0, 0, 1, {.text = &gWindowText567AD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 18, 175, 0, 0, 1, {.graphic = &gWindowGraphic5679F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 54, 173, 0, 0, 1, {.graphic = &gWindowGraphic5679F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 18, 261, 13, 11, 1, {.graphic = &gWindowGraphic567A40}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_Graphic, 38, 261, 10, 10, 1, {.graphic = &gWindowGraphic567A58}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_Graphic, 90, 261, 10, 10, 1, {.graphic = &gWindowGraphic567A70}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_Graphic, 102, 261, 13, 11, 1, {.graphic = &gWindowGraphic567A88}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_Graphic, 31, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 43, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 49, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 55, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 61, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 67, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 79, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 85, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 43, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 49, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 55, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 61, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 67, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 79, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 85, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 43, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 49, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 55, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 61, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 67, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 79, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 85, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 43, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 49, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 55, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 61, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 67, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 79, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 85, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 11, 11, 1, {.graphic = &gWindowGraphic5679C8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 13, 26, 11, 11, 1, {.graphic = &gWindowGraphic5679E0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 131, 59, 114, 110, 1, {.mouseFlags = &gWindowMouseFlagsD677D0}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_EventArea, 256, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

} // namespace cg::ui::detail
