#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/BattleState.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049B630。
static std::int32_t WindowBattlePets_Update(std::int32_t unused); // 窗口28归还按钮显隐。

// 原函数：0x0049BA10。
static std::int32_t WindowBattlePets_ButtonReturn(std::int32_t index, std::int32_t state); // 窗口28宠物归还M|FF。

// 原函数：0x0049BB00。
static std::int32_t WindowBattlePets_SelectRow(std::int32_t index, std::int32_t state); // 窗口28三条宠物选择行。

// 原函数：0x0049B670。
static void WindowBattlePets_Refresh(
    std::int16_t x, std::int16_t y, std::int32_t petIndex,
    std::uint8_t textLayer, std::int32_t graphicLayer,
    const render::GraphicDescriptor16* descriptor); // 快照数值与宠物属性行，唯一caller弃返回。

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic56C798 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C798。

static WindowGraphicData gWindowGraphic56C7B0 {0x3BA74, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C7B0。

static WindowGraphicData gWindowGraphic56C7C8 {0x3BA75, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056C7C8。

static WindowGraphicData gWindowGraphic56C7E0 {0x3BA76, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C7E0。

static WindowElementPrototype gWindowElements56C800[8] { // 原全局：0x0056C800。
    {WindowElementType_Graphic, 302, 9, 11, 11, 1, {.graphic = &gWindowGraphic56C798}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 258, 173, 57, 17, 1, {.graphic = &gWindowGraphic56C7E0}, &WindowBattlePets_ButtonReturn},
    {WindowElementType_EventArea, 15, 28, 300, 48, 1, {.unusedValue = 0}, &WindowBattlePets_SelectRow},
    {WindowElementType_EventArea, 15, 76, 300, 48, 1, {.unusedValue = 0}, &WindowBattlePets_SelectRow},
    {WindowElementType_EventArea, 15, 124, 300, 48, 1, {.unusedValue = 0}, &WindowBattlePets_SelectRow},
    {WindowElementType_Graphic, 0, 0, 320, 353, 1, {.graphic = &gWindowGraphic56C7B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 25, 320, 353, 1, {.graphic = &gWindowGraphic56C7C8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 320, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口28；原全局：0x0054E810。
WindowPrototype gWindowPrototypeBattlePets {0, 4, 100, 100, 356, 223, 0x80010101u, 8, gWindowElements56C800, &WindowBattlePets_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 28: 54E810

// 窗口函数实现。

// 原函数：0x0049B630。
static std::int32_t WindowBattlePets_Update(const std::int32_t)
{
    gCurrentWindow->elements[1].visible = 0;
    if (world::FindBattlePetIndex_43C4B0() != -1) {
        gCurrentWindow->elements[1].visible = 1;
    }
    return 1;
}

// 原函数：0x0049BA10。
static std::int32_t WindowBattlePets_ButtonReturn(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"让这只宠物归还。";
    }
    if ((state & 2) != 0) {
        if (world::FindBattlePetIndex_43C4B0() != -1) {
            std::array<char, 768> buffer;
            std::sprintf(buffer.data(), "M|%X", 255u);
            static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
            gCurrentWindow->state |= 0x8000u;
            world::gBattlePlayerActionState5ABC74 = 1;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3BA76;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3BA78;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3BA77;
    }
    return consumed;
}

// 原函数：0x0049B670。
static void WindowBattlePets_Refresh(
    const std::int16_t x, const std::int16_t y, const std::int32_t petIndex,
    const std::uint8_t textLayer, const std::int32_t graphicLayer,
    const render::GraphicDescriptor16* const descriptor)
{
    std::int32_t battleImage = 0x3B58D;
    std::int32_t readyImage = 0x3B58A;
    const auto& pet = world::gPetRuntimeRecords[petIndex];
    if (pet.used == 1 && (pet.flags6AA == 1 || pet.flags6AA == 2)) {
        static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
        static_cast<void>(render::DrawText_456780(
            0, 0, textLayer, 8, 0, L"", 0, 0));
        const auto drawText = [&](const std::wstring_view text, const std::int32_t dx, const std::int32_t dy) {
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + dx)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + dy)),
                textLayer, 8, 0, text, 0, 0));
        };
        drawText(cg::text::FromGbkWide(pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data()), 10, 5);
        std::wstring buffer;
        cg::text::FormatTo(buffer, L"{:3}", pet.value08);
        drawText(buffer, 140, 5);
        const std::int32_t tribe = std::bit_cast<std::int16_t>(pet.value04);
        if (tribe >= 0 && tribe < 20) {
            drawText(gTribeNames57CB54[tribe], 130, 19);
        }
        const std::int32_t maxHp = gBattlePetVitalsCF4B78[petIndex].maxHp;
        const std::int32_t hp = gBattlePetVitalsCF4B78[petIndex].hp;
        cg::text::FormatTo(buffer, L"{:6}/{:6}", hp, maxHp);
        drawText(buffer, 35, 19);
        const std::int32_t maxFp = gBattlePetVitalsCF4B78[petIndex].maxFp;
        const std::int32_t fp = gBattlePetVitalsCF4B78[petIndex].fp;
        cg::text::FormatTo(buffer, L"{:6}/{:6}", fp, maxFp);
        drawText(buffer, 35, 32);
        for (std::int32_t row = 0; row < 4; ++row) {
            const std::int32_t segments = pet.values70[21 + row] / 10;
            const auto& images = gPetBarImages57E434[row];
            const std::int32_t barY = y + 6 + row * 10;
            if (segments > 1) {
                static_cast<void>(render::DrawGraphic3_520970(
                    x + 175, barY, static_cast<std::uint8_t>(graphicLayer), images[0], 0, 1, 0, descriptor));
                std::int32_t barX = x + 181;
                for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                    static_cast<void>(render::DrawGraphic3_520970(
                        barX, barY, static_cast<std::uint8_t>(graphicLayer), images[1], 0, 1, 0, descriptor));
                    barX += 6;
                }
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, static_cast<std::uint8_t>(graphicLayer), images[2], 0, 1, 0, descriptor));
            } else if (segments == 1) {
                static_cast<void>(render::DrawGraphic3_520970(
                    x + 175, barY, static_cast<std::uint8_t>(graphicLayer), images[3], 0, 1, 0, descriptor));
            }
        }
        const std::uint16_t flags = pet.flags6AA;
        if (flags == 2) {
            battleImage = 0x3B58C;
        }
        if (flags == 1) {
            readyImage = 0x3B589;
        }
    }
    static_cast<void>(render::DrawGraphic3_520970(
        x + 241, y + 6, static_cast<std::uint8_t>(graphicLayer), battleImage, 0, 1, 0, descriptor));
    static_cast<void>(render::DrawGraphic3_520970(
        x + 267, y + 25, static_cast<std::uint8_t>(graphicLayer), readyImage, 0, 1, 0, descriptor));
    static_cast<void>(render::DrawGraphic3_520970(
        x, y, static_cast<std::uint8_t>(graphicLayer), 0x3B5C4, 0, 1, 0, descriptor));
}

// 原函数：0x0049BB00。
static std::int32_t WindowBattlePets_SelectRow(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = window->elements + index;
    const auto x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(window->x)
            + static_cast<std::uint16_t>(element->x)));
    const auto y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(window->y)
            + static_cast<std::uint16_t>(element->y)));
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t selected = -1;
    std::int32_t visible = -1;
    for (const auto& pair : world::gPetPairsDE0B08) {
        const std::int32_t petIndex = std::bit_cast<std::int16_t>(pair.index);
        const auto& pet = world::gPetRuntimeRecords[petIndex];
        if (pet.used == 1 && (pet.flags6AA == 1 || pet.flags6AA == 2)) {
            ++visible;
        }
        if (visible == index - 2) {
            selected = petIndex;
            break;
        }
    }
    if (selected < 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    const auto& pet = world::gPetRuntimeRecords[selected];
    if (pet.used == 1) {
        std::uint16_t flags = pet.flags6AA;
        if (flags == 1 || flags == 2) {
            if ((state & 1) != 0) {
                WindowElement_DrawHoverFrame(window->elements + index, state, cg::render::gHoverFramePaletteIndex_D69768);
                flags = pet.flags6AA;
                if (flags != 2) {
                    const wchar_t* const hint = gBattlePetVitalsCF4B78[selected].hp > 0
                        ? L"召唤宠物。"
                        : L"这只宠物昏倒了。";
                    gHoverTextD68AE8 = hint;
                }
            }
            if ((state & 2) != 0) {
                if (gBattlePetVitalsCF4B78[selected].hp <= 0 || flags == 2) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    std::array<char, 768> buffer;
                    std::sprintf(buffer.data(), "M|%X", static_cast<std::uint32_t>(selected));
                    static_cast<void>(network::SendBattlePacket_5109B0(
                        platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
                    gCurrentWindow->state |= 0x8000u;
                    world::gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
                consumed = 1;
            }
        }
    }
    WindowBattlePets_Refresh(x, y, selected, 3, 223, &descriptor);
    return consumed;
}

} // namespace detail

} // namespace cg::ui
