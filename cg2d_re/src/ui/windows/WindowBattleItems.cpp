#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/BattleState.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 4C6EC0发布的战斗背包/装备交互。
// 原函数：0x00499930。
static std::int32_t WindowBattleItems_TogglePreview(std::int32_t unusedIndex, std::int32_t state);

// 原函数：0x00499950。
static std::int32_t WindowBattleItems_CycleEquipment(std::int32_t unusedIndex, std::int32_t state);

// 原函数：0x00499970。
static std::int32_t WindowBattleItems_RotatePreview(std::int32_t unusedIndex, std::int32_t state);

// 原函数：0x00499B00。
static std::int32_t WindowBattleItems_RefreshMoney(std::int32_t unusedIndex, std::int32_t unusedState);

// 原函数：0x00499B20。
static std::int32_t WindowBattleItems_RefreshExtraValue(std::int32_t unusedIndex, std::int32_t unusedState);

// 原函数：0x00499B40。
static std::int32_t WindowBattleItems_TabEquipment(std::int32_t unusedIndex, std::int32_t state); // 战斗页签忽略内部返回。

// 原函数：0x00499B60。
static std::int32_t WindowBattleItems_TabPet(std::int32_t index, std::int32_t state); // SAP01：战斗宠物页签，成功忽略4C7380结果返回S32零，无效图处理保持。

// 原函数：0x0049B610。
static std::int32_t WindowBattleItems_ShowExtraAttributes(std::int32_t unusedIndex, std::int32_t state); // D2F338附加属性页包装。

// 原函数：0x00499990。
static std::int32_t WindowBattleItems_Update(std::int32_t value);

// 原函数：0x004999D0。
static std::int32_t WindowBattleItems_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowItemSelectorState gItemSelectorD2F338 {}; // 原全局：0x00D2F338。

// 原全局：0x00CF49B0。 窗口27每帧清-1，499C30发布提示槽。
static std::int32_t gBattleItemHoverCF49B0 = 0; // 原全局：0x00CF49B0。

static std::int32_t gBattleItemPageCB01E0 = 0; // 原全局：0x00CB01E0。 窗口27物品提示页，同源于两战斗交互。

static std::int32_t gBattleBackpackHover57E430 = -2; // 原全局：0x0057E430。

// 原全局：0x00CF469C。 499B00格式化，type7原型借用。
static std::wstring gBattleMoneyTextCF469C {}; // 原全局：0x00CF469C。

// 原全局：0x00C8A960。 499B20格式化，独立于普通库存文本。
static std::wstring gBattleValueTextC8A960 {}; // 原全局：0x00C8A960。

static WindowGraphicData gWindowGraphic56C348 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C348。

static WindowGraphicData gWindowGraphic56C360 {0x3BA42, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C360。

static WindowGraphicData gWindowGraphic56C378 {0x3BA46, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C378。

static WindowGraphicData gWindowGraphic56C390 {0x17F63, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C390。

static WindowGraphicData gWindowGraphic56C3A8 {0x17F53, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C3A8。

static WindowGraphicData gWindowGraphic56C3C0 {0x17F5C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C3C0。

static WindowGraphicData gWindowGraphic56C3D8 {0x17F5F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C3D8。

static WindowGraphicData gWindowGraphic56C3F0 {0x3BA49, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C3F0。

static std::int32_t gWindowAnimationImage56C418 = 100000; // 原全局：0x0056C418。

static WindowNumberData gWindowNumber56C420 {&gBattleMoneyTextCF469C, 10, 3, 1, 0}; // 原全局：0x0056C420。

static WindowNumberData gWindowNumber56C428 {&gBattleValueTextC8A960, 10, 3, 1, 0}; // 原全局：0x0056C428。

static WindowElementPrototype gWindowElements56C430[22] { // 原全局：0x0056C430。
    {WindowElementType_Graphic, 258, 8, 11, 11, 1, {.graphic = &gWindowGraphic56C348}, &WindowRuntime_ButtonClose},
    {WindowElementType_Number, 198, 31, 0, 0, 1, {.number = &gWindowNumber56C420}, &WindowBattleItems_RefreshMoney},
    {WindowElementType_Number, 99, 31, 0, 0, 1, {.number = &gWindowNumber56C428}, &WindowBattleItems_RefreshExtraValue},
    {WindowElementType_Graphic, 8, 0, 276, 253, 1, {.graphic = &gWindowGraphic56C360}, &WindowBattleItems_HandleInventory},
    {WindowElementType_Animation, -81, 163, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56C418}, &WindowElement_NoOp},
    {WindowElementType_Text, -81, 28, 0, 0, 0, {.text = &gWindowText562C70}, &WindowElement_NoOp},
    {WindowElementType_Text, -81, 48, 0, 0, 0, {.text = &gWindowText562C78}, &WindowElement_NoOp},
    {WindowElementType_EventArea, -114, 188, 33, 17, 0, {.unusedValue = 0}, &WindowBattleItems_TogglePreview},
    {WindowElementType_EventArea, -81, 188, 33, 17, 0, {.unusedValue = 0}, &WindowBattleItems_CycleEquipment},
    {WindowElementType_EventArea, -121, 73, 78, 113, 0, {.unusedValue = 0}, &WindowBattleItems_RotatePreview},
    {WindowElementType_Graphic, -170, 73, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 73, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -170, 123, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 123, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -170, 23, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 23, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 2, 17, 19, 46, 1, {.graphic = &gWindowGraphic56C378}, &WindowBattleItems_TabEquipment},
    {WindowElementType_Graphic, 2, 62, 19, 46, 1, {.graphic = &gWindowGraphic56C3A8}, &WindowBattleItems_TabPet},
    {WindowElementType_Graphic, 2, 107, 19, 46, 1, {.graphic = &gWindowGraphic56C3C0}, &WindowBattleItems_TabPet},
    {WindowElementType_Graphic, 2, 152, 19, 46, 1, {.graphic = &gWindowGraphic56C3D8}, &WindowBattleItems_TabPet},
    {WindowElementType_Graphic, -197, 17, 212, 210, 1, {.graphic = &gWindowGraphic56C390}, &WindowBattleItems_HandleEquipment},
    {WindowElementType_EventArea, 320, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口27；原全局：0x0054E7E0。
WindowPrototype gWindowPrototypeBattleItems {0, 4, 100, 100, 276, 253, 0x80010101u, 22, gWindowElements56C430, &WindowBattleItems_Update, &WindowBattleItems_Draw, &WindowRuntime_ReleaseDefault}; // 27: 54E7E0

// 窗口函数实现。

// 原函数：0x00499930。
static std::int32_t WindowBattleItems_TogglePreview(const std::int32_t, const std::int32_t state)
{
    return WindowItemSelector_TogglePreview(&gItemSelectorD2F338, state) ? 1 : 0;
}

// 原函数：0x00499950。
static std::int32_t WindowBattleItems_CycleEquipment(const std::int32_t, const std::int32_t state)
{
    return WindowItemSelector_CycleEquipment(&gItemSelectorD2F338, state) ? 1 : 0;
}

// 原函数：0x00499970。
static std::int32_t WindowBattleItems_RotatePreview(const std::int32_t, const std::int32_t state)
{
    static_cast<void>(WindowItemSelector_RotatePreview(&gItemSelectorD2F338, state));
    return 0;
}

// 原函数：0x00499B00。
static std::int32_t WindowBattleItems_RefreshMoney(const std::int32_t, const std::int32_t)
{
    cg::text::FormatTo(gBattleMoneyTextCF469C, L"{:10}", world::gPlayerRuntimeSlots.valueFC);
    return 0;
}

// 原函数：0x00499B20。
static std::int32_t WindowBattleItems_RefreshExtraValue(const std::int32_t, const std::int32_t)
{
    cg::text::FormatTo(gBattleValueTextC8A960, L"{:10}", world::gPlayerRuntimeSlots.value15BCC);
    return 0;
}

// 原函数：0x00499B40。
static std::int32_t WindowBattleItems_TabEquipment(const std::int32_t, const std::int32_t state)
{
    static_cast<void>(WindowItemSelector_HandleTab(&gItemSelectorD2F338, 0, state));
    return 0;
}

// SAP01：原71条；五对与S8模式保持，成功忽略按钮结果恒0，无效图/声效按原，运行待。
// 原函数：0x00499B60。
static std::int32_t WindowBattleItems_TabPet(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t ordinal = index - 17;
    std::int32_t remaining = ordinal;
    bool found = false;
    for (std::int32_t position = 0; position < 5; ++position) {
        const auto& pair = world::gPetPairsDE0B08[position];
        if (pair.used == 0) {
            continue;
        }
        const std::int32_t pet = std::bit_cast<std::int16_t>(pair.index);
        const std::uint16_t flags = world::gPetRuntimeRecords[pet].flags6AA;
        if (flags == 2 || flags == 1) {
            --remaining;
            if (remaining < 0) {
                found = true;
                break;
            }
        }
    }
    if (found) {
        if (ordinal >= 0) {
            static_cast<void>(WindowItemSelector_HandleTab(&gItemSelectorD2F338, ordinal + 1, state));
            return 0;
        }
    } else if (std::bit_cast<std::int8_t>(gBattleItemActorSelectionMode_57E42C) == ordinal + 1) {
        --gBattleItemActorSelectionMode_57E42C;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        consumed = 1;
    }
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 244306;
    return consumed;
}

// 原函数：0x00499990。
static std::int32_t WindowBattleItems_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowItemSelector_BindElements(&gItemSelectorD2F338, 1);
        gItemSelectorD2F338.initialized3E8 = 0;
        gBattleItemPageCB01E0 = 0;
    }
    gBattleItemHoverCF49B0 = -1;
    WindowItemSelector_Update(&gItemSelectorD2F338);
    return 1;
}

// 原函数：0x004999D0。
static std::int32_t WindowBattleItems_Draw(const std::int32_t)
{
    const std::int32_t hovered = gBattleItemHoverCF49B0;
    if (hovered >= 0) {
        static_cast<void>(WindowItem_DrawEquipmentTooltip(hovered, gBattleItemPageCB01E0));
    } else {
        const std::int32_t kind = WindowDrag_ReadKind();
        if (kind == 2) {
            if (WindowDrag_ReadFocusWindowId() == 27 && (platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
                static_cast<void>(WindowDrag_ResetMouse());
            }
        } else if (kind == 0 && (platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            gCurrentWindow->state |= 0x8000u;
        }
    }
    const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
    WindowInstance* const window = gCurrentWindow;
    cg::yobi::YobiAction* const source = world::gBattleEntries59E980[position];
    auto* const data = static_cast<WindowAnimationData*>(window->elements[4].data);
    const std::int32_t imageNo = cg::yobi::GetYobiActionField2A8(source);
    cg::yobi::SetYobiActionField2A8(data->action, imageNo);
    cg::yobi::SetYobiActionField2B8(
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[4].data)->action, 5);
    cg::yobi::SetYobiActionField309(
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[4].data)->action, 0);
    WindowElement* const elements = gCurrentWindow->elements;
    if (elements[4].visible != 0) {
        const std::int32_t currentImage = cg::yobi::GetYobiActionField2A8(
            world::gBattleEntries59E980[world::gBattleCurrentPosition5ABCA4]);
        if (currentImage >= 110350 && currentImage <= 110373) {
            cg::yobi::SetYobiActionField309(
                static_cast<WindowAnimationData*>(elements[4].data)->action, 4);
            cg::yobi::SetYobiActionField310(
                static_cast<WindowAnimationData*>(gCurrentWindow->elements[4].data)->action, 4);
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// BIP01：原1175条；六BYTE/五grid/区域取序明确，原未加8容量索引/技能经验成本和目标发布保持，全生产消费及运行待。
// 原函数：0x00499C30。
std::int32_t WindowBattleItems_HandleInventory(const std::int32_t index, const std::int32_t state)
{
    constexpr std::int32_t kBombType = 51;
    constexpr std::int32_t kMedicineType = 43;
    static constexpr std::int32_t levelLimits[] {
        9, 19, 29, 39, 49, 59, 69, 79, 89, 99, 109, 119, 129, 139, 149,
    };
    static constexpr wchar_t noSpace[] = L"物品栏位不足！";
    static constexpr wchar_t unidentified[] =
        L"无法装备未鉴定物品！";
    static constexpr wchar_t cannotEquip[] =
        L"无法装备此物品。";
    static constexpr wchar_t canUse[] = L"可使用此物品。";
    static constexpr wchar_t cannotUse[] =
        L"无法使用此物品。";
    std::int32_t skillIndex = -1;
    std::int32_t usableLevel = -1;
    for (std::int32_t position = 0; position < world::kSkillSlotCount; ++position) {
        const auto& skill = world::gSkillsF1BAFC[position];
        if (skill.skillId != 2002) {
            continue;
        }
        skillIndex = position;
        for (std::int32_t technique = 0; technique < world::kTechniqueSlotCount; ++technique) {
            if (skill.techniques3C[technique].value8C <= 0) {
                break;
            }
            usableLevel = technique + 1;
        }
        break;
    }
    const std::int32_t playerLevel = world::gPlayerRuntimeSlots.level;
    std::int32_t levelLimit = 1;
    while (levelLimit < 15 && playerLevel > levelLimits[levelLimit - 1]) {
        ++levelLimit;
    }
    if (usableLevel > levelLimit) {
        usableLevel = levelLimit;
    }
    // 原+24+94H*等级：等级0为experience，等级1起为前一技术的FP字段。
    const auto readBombCost = [skillIndex](const std::int32_t level) {
        if (skillIndex < 0 || skillIndex >= world::kSkillSlotCount || level < 0 || level > world::kTechniqueSlotCount) return 0;
        const auto& skill = world::gSkillsF1BAFC[skillIndex];
        return level == 0 ? skill.experience : skill.techniques3C[level - 1].value7C;
    };
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    descriptor.color = color;
    dimmed.color = dimmedColor;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    auto& items = world::gPlayerRuntimeSlots.items;
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    std::uint8_t events = static_cast<std::uint8_t>(state);
    std::int32_t consumed = 0;
    std::wstring countText;
    if (std::bit_cast<std::int8_t>(gBattleItemActorSelectionMode_57E42C) > 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t elementX = element.x;
            const std::int32_t windowX = window->x;
            const std::int32_t left = elementX + 50 * (position % 5) + windowX + 15;
            const std::int32_t elementY = element.y;
            const std::int32_t windowY = window->y;
            const std::int32_t top = elementY + 50 * (position / 5) + windowY + 48;
            const auto& item = items[position + 8];
            if ((events & 0x21u) != 0) {
                if (occupancy[position + 8] != 0) {
                    continue;
                }
                if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0
                    && item.used != 0) {
                    const std::int32_t type = item.itemType;
                    gHoverTextD68AE8 = ((type >= 0 && type <= 7) || (type >= 70 && type <= 79))
                            ? cannotEquip : cannotUse;
                    if ((events & 1u) != 0) {
                        cg::render::DrawGraphicHover(
                            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                            223, cg::render::gHoverFramePaletteIndex_D69768, 0);
                    }
                }
                consumed = 1;
            }
            if (item.used == 0) {
                continue;
            }
            static_cast<void>(cg::render::DrawGraphic(left + 129, top + 118, 223, 22738, 0, nullptr));
            if (occupancy[position + 8] != 0) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                    223, 249, 0);
                static_cast<void>(cg::render::DrawGraphic(
                    left + 24, top + 24, 223, item.imageNo, 0, &dimmed));
            } else {
                static_cast<void>(cg::render::DrawGraphic(
                    left + 24, top + 24, 223, item.imageNo, 0, &descriptor));
            }
            if (item.count > 0) {
                cg::text::FormatTo(countText, L"{:3}", item.count);
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(left + 21), static_cast<std::int16_t>(top + 31),
                    3, 1, 1, countText, 0, 0));
            }
        }
        return consumed;
    }
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 2) {
        events &= 0x9Fu;
        consumed = 1;
    }
    std::array<char, 768> buffer;
    if ((events & 0x40u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source == 2 || source == 3) {
            std::int32_t weapons = 0;
            if (items[2].used == 1 && items[2].itemType != 7) {
                weapons = 1;
            }
            if (items[3].used == 1 && items[3].itemType != 7) {
                ++weapons;
            }
            if (world::CountFreeInventorySlots_518C60() < weapons) {
                world::AppendChatMessage_44B050(noSpace, 4, 3);
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if (weapons == 0) {
                world::AppendChatMessage_44B050(L"没有装备武器！", 4, 3);
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                gCurrentWindow->state |= 0x8000u;
                world::gBattleSelectionModes5ABC7C[5] = 0;
                std::sprintf(buffer.data(), "Q|%X|", 255u);
                static_cast<void>(network::SendBattlePacket_5109B0(
                    platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
                world::gBattlePlayerActionState5ABC74 = 1;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            }
        }
        WindowDrag_Reset();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t elementX = element.x;
            const std::int32_t windowX = window->x;
            const std::int32_t left = elementX + 50 * (position % 5) + windowX + 15;
            const std::int32_t elementY = element.y;
            const std::int32_t windowY = window->y;
            const std::int32_t top = elementY + 50 * (position / 5) + windowY + 48;
            if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
        if (hovered >= 0 && items[hovered + 8].used != 0) {
            const auto& item = items[hovered + 8];
            const std::int32_t type = item.itemType;
            const wchar_t* hint = cannotUse;
            if (type < 23) {
                hint = ((type >= 0 && type <= 7) || (type >= 70 && type <= 79))
                    && item.hasDescription != 0 ? L"可装备此物品。" : cannotEquip;
            } else if ((*reinterpret_cast<const unsigned char*>(&item.battleUseFlag) & 1u) != 0) {
                if (item.hasDescription != 0) {
                    if (type == kMedicineType) {
                        if ((world::gBattlePlayerFlags5ABCAC & 0x40000u) == 0) {
                            hint = canUse;
                        }
                    } else if (type == kBombType) {
                        const std::int32_t level = item.level;
                        world::gBattleBombLevelD67B2C = level;
                        const std::int32_t cost = readBombCost(level);
                        if (usableLevel >= level && items[2].used == 0 && items[3].used == 0
                            && !(world::gPlayerRuntimeSlots.fp < cost)
                            && world::gBattleModeFlag5ABFCC != 1) {
                            hint = canUse;
                        }
                    } else {
                        hint = canUse;
                    }
                }
            } else {
                gHoverTextD68AE8 = canUse;
            }
            gHoverTextD68AE8 = hint;
        }
    }
    if (gBattleBackpackHover57E430 != hovered) {
        gBattleItemPageCB01E0 = 0;
    }
    gBattleBackpackHover57E430 = hovered;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (WindowDrag_ReadFocusWindowId() == 27 || WindowDrag_ReadFocusWindowId() == 0) {
            const auto& item = items[hovered + 8];
            bool useItem = false;
            if (item.used != 1) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if ((*reinterpret_cast<const unsigned char*>(&item.battleUseFlag) & 1u) != 0 && item.itemType == kBombType) {
                const std::int32_t cost = readBombCost(items[hovered + 8].level);
                if (usableLevel < item.level || items[2].used != 0 || items[3].used != 0
                    || (world::gPlayerRuntimeSlots.fp < cost)
                    || world::gBattleModeFlag5ABFCC == 1) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    useItem = true;
                }
            } else {
                const std::int32_t type = item.itemType;
                if ((type >= 0 && type <= 7) || (type >= 70 && type <= 79)) {
                    std::int32_t occupiedHands = items[2].used == 1;
                    if (items[3].used == 1) {
                        ++occupiedHands;
                    }
                    // 原49A510以未加8的hover索引读取battleUseFlag。
                    if (occupiedHands == 2 && world::CountFreeInventorySlots_518C60() == 0
                        && (*reinterpret_cast<const unsigned char*>(&items[hovered].battleUseFlag) & 8u) != 0) {
                        world::AppendChatMessage_44B050(noSpace, 4, 3);
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    } else if (item.hasDescription == 0) {
                        world::AppendChatMessage_44B050(unidentified, 4, 3);
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    } else {
                        gCurrentWindow->state |= 0x8000u;
                        world::gBattleSelectionModes5ABC7C[5] = 0;
                        std::sprintf(buffer.data(), "Q|%X|", static_cast<std::uint32_t>(hovered + 8));
                        static_cast<void>(network::SendBattlePacket_5109B0(
                            platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
                        world::gBattlePlayerActionState5ABC74 = 1;
                        static_cast<void>(audio::PlaySe_52C450(77, 320));
                    }
                } else if ((*reinterpret_cast<const unsigned char*>(&item.battleUseFlag) & 1u) == 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (item.hasDescription == 0) {
                    world::AppendChatMessage_44B050(unidentified, 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (item.itemType == kMedicineType
                    && (world::gBattlePlayerFlags5ABCAC & 0x40000u) != 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    useItem = true;
                }
            }
            if (useItem) {
                gCurrentWindow->state |= 0x8000u;
                const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
                const auto target = static_cast<std::uint32_t>(
                    static_cast<std::int32_t>(std::bit_cast<std::int16_t>(item.target)));
                if (cg::yobi::CheckBattleCommand_43C560(position, target) != 0) {
                    world::gBattleSelectedItem5ABC48 = hovered + 8;
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                    world::gBattleItemTargetPending5ABCC4 = 1;
                    gBattleItemTargetFlagCC4D40 = 1;
                } else {
                    const std::int32_t currentPosition = world::gBattleCurrentPosition5ABCA4;
                    world::gBattleSelectedItem5ABC48 = hovered + 8;
                    std::sprintf(buffer.data(), "I|%X|%X",
                        static_cast<std::uint32_t>(hovered + 8),
                        static_cast<std::uint32_t>(currentPosition));
                    static_cast<void>(network::SendBattlePacket_5109B0(
                        platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
                    cg::yobi::FlushBattleEntries_43C060();
                    world::gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                }
            }
            static_cast<void>(WindowDrag_ResetMouse());
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            const auto& item = items[hovered + 8];
            bool canDrag;
            if (item.itemType == kBombType) {
                const std::int32_t cost = readBombCost(items[hovered + 8].level);
                canDrag = usableLevel >= item.level && items[2].used == 0 && items[3].used == 0
                    && !(world::gPlayerRuntimeSlots.fp < cost)
                    && world::gBattleModeFlag5ABFCC != 1;
            } else {
                canDrag = item.used != 0;
            }
            if (canDrag) {
                const std::int32_t type = item.itemType;
                if (((*reinterpret_cast<const unsigned char*>(&item.battleUseFlag) & 1u) != 0
                        || (type >= 0 && type <= 7) || (type >= 70 && type <= 79))
                    && (item.itemType != kMedicineType
                        || (world::gBattlePlayerFlags5ABCAC & 0x40000u) == 0)) {
                    static_cast<void>(WindowItemDrag_Start(hovered + 8, 0));
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            const auto& item = items[hovered + 8];
            if (item.used != 0) {
                const std::int32_t pages = item.pages;
                const std::int32_t page = gBattleItemPageCB01E0 + 1;
                gBattleItemPageCB01E0 = page;
                if (page >= pages) {
                    gBattleItemPageCB01E0 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 8) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t elementX = element.x;
            const std::int32_t windowX = window->x;
            const std::int32_t left = elementX + 50 * ((source - 8) % 5) + windowX + 15;
            const std::int32_t elementY = element.y;
            const std::int32_t windowY = window->y;
            const std::int32_t top = elementY + 50 * ((source - 8) / 5) + windowY + 48;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t elementX = element.x;
        const std::int32_t windowX = window->x;
        const std::int32_t left = elementX + 50 * (hovered % 5) + windowX + 15;
        const std::int32_t elementY = element.y;
        const std::int32_t windowY = window->y;
        const std::int32_t top = elementY + 50 * (hovered / 5) + windowY + 48;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        if (items[hovered + 8].used != 0) {
            gBattleItemHoverCF49B0 = hovered + 8;
        }
    }
    if (WindowDrag_ReadKind() != 0 && (events & 0x20u) != 0 && WindowDrag_ReadSourceIndex() < 8) {
        const WindowInstance* const window = gCurrentWindow;
        const std::uint32_t hoverColor = cg::render::gHoverFramePaletteIndex_D69768;
        const WindowElement* const elements = window->elements;
        const std::int32_t windowY = window->y;
        const std::int32_t windowX = window->x;
        const WindowElement& element = elements[index];
        const std::int32_t elementX = element.x;
        const std::int32_t height = element.height;
        const std::int32_t elementY = element.y;
        const std::int32_t width = element.width;
        const std::int32_t right = width + windowX + elementX;
        const std::int32_t bottom = height + windowY + elementY;
        const std::int32_t top = windowY + elementY;
        const std::int32_t left = windowX + elementX;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(right), static_cast<std::uint16_t>(bottom),
            223, hoverColor, 0);
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, L"", 0, 0));
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = items[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t elementX = element.x;
        const std::int32_t windowX = window->x;
        const std::int32_t x = elementX + 50 * (position % 5) + windowX + 39;
        const std::int32_t elementY = element.y;
        const std::int32_t windowY = window->y;
        const std::int32_t y = elementY + 50 * (position / 5) + windowY + 73;
        if (item.itemType == kBombType) {
            const std::int32_t level = item.level;
            const std::int32_t cost = readBombCost(level);
            if (usableLevel < level || items[2].used != 0 || items[3].used != 0
                || (world::gPlayerRuntimeSlots.fp < cost)
                || world::gBattleModeFlag5ABFCC == 1) {
                static_cast<void>(cg::render::DrawGraphic(
                    x + 105, y + 93, 223, 22738, 0, nullptr));
            }
        }
        if ((*reinterpret_cast<const unsigned char*>(&item.battleUseFlag) & 1u) == 0) {
            const std::int32_t type = item.itemType;
            if (!((type >= 0 && type <= 7) || (type >= 70 && type <= 79))) {
                static_cast<void>(cg::render::DrawGraphic(
                    x + 105, y + 93, 223, 22738, 0, nullptr));
            }
        }
        if (item.hasDescription == 0 || (item.itemType == kMedicineType
                && (world::gBattlePlayerFlags5ABCAC & 0x40000u) != 0)) {
            static_cast<void>(cg::render::DrawGraphic(
                x + 105, y + 93, 223, 22738, 0, nullptr));
        }
        if (occupancy[position + 8] != 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        }
        if (item.count > 0) {
            cg::text::FormatTo(countText, L"{:3}", item.count);
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(x - 3), static_cast<std::int16_t>(y + 7),
                3, 1, 1, countText, 0, 0));
        }
    }
    return consumed;
}

// 原函数：0x0049B610。
static std::int32_t WindowBattleItems_ShowExtraAttributes(const std::int32_t, const std::int32_t state)
{
    return WindowItemSelector_HandlePlayerAttributes(&gItemSelectorD2F338, state) ? 1 : 0;
}

// BEI01：原755条，三BYTE及区域描边取序明确；宠物禁用/玩家装备/原两Q命令保持，全生产消费及运行待。
// 原函数：0x0049AC40。
std::int32_t WindowBattleItems_HandleEquipment(const std::int32_t index, const std::int32_t state)
{
    static constexpr wchar_t noSpace[] = L"物品栏位不足！";
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    descriptor.color = color;
    dimmed.color = dimmedColor;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    world::PlayerItemRecord* items;
    std::int32_t count;
    if (!WindowItemSelector_GetItemList_4C72C0(&gItemSelectorD2F338, &items, &count)) {
        return 0;
    }
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if (std::bit_cast<std::int8_t>(gBattleItemActorSelectionMode_57E42C) > 0) {
        const std::int32_t pet = WindowItemSelector_ReadSelectedPet(&gItemSelectorD2F338);
        const std::int32_t slots = world::gPetRuntimeRecords[pet].values6D8[0];
        for (std::int32_t position = 0; position < slots; ++position) {
            const auto& item = items[position];
            const std::int32_t left = WindowItemSelector_ReadScreenX(&gItemSelectorD2F338, index, position, -24);
            const std::int32_t top = WindowItemSelector_ReadScreenY(&gItemSelectorD2F338, index, position, -24);
            if (item.used == 0) {
                continue;
            }
            if ((events & 0x21u) != 0) {
                if (occupancy[position] != 0) {
                    continue;
                }
                if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    gHoverTextD68AE8 = L"无法在战斗中更换装备。";
                    if ((events & 1u) != 0) {
                        cg::render::DrawGraphicHover(
                            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                            223, cg::render::gHoverFramePaletteIndex_D69768, 0);
                    }
                }
                consumed = 1;
            }
            static_cast<void>(cg::render::DrawGraphic(left + 129, top + 117, 223, 22738, 0, nullptr));
            if (occupancy[position] != 0) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                    223, 249, 0);
                static_cast<void>(cg::render::DrawGraphic(
                    left + 24, top + 24, 223, item.imageNo, 0, &dimmed));
            } else {
                static_cast<void>(cg::render::DrawGraphic(
                    left + 24, top + 24, 223, item.imageNo, 0, &descriptor));
            }
        }
        return consumed;
    }
    if ((events & 0x60u) != 0 && (WindowDrag_ReadKind() != 2 || WindowDrag_ReadSourceIndex() >= 100)) {
        events &= 0x9Fu;
        consumed = 1;
    }
    std::array<char, 768> buffer;
    if ((events & 0x40u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= count && world::gPlayerRuntimeSlots.items[source].used == 1) {
            const std::int32_t type = world::gPlayerRuntimeSlots.items[source].itemType;
            if ((type >= 0 && type <= 7) || (type >= 70 && type <= 79)) {
                std::int32_t occupiedHands = world::gPlayerRuntimeSlots.items[2].used == 1;
                if (world::gPlayerRuntimeSlots.items[3].used == 1) {
                    ++occupiedHands;
                }
                if (occupiedHands == 2 && world::CountFreeInventorySlots_518C60() == 0
                    && (*reinterpret_cast<const unsigned char*>(
                        &world::gPlayerRuntimeSlots.items[source].battleUseFlag) & 8u) != 0) {
                    world::AppendChatMessage_44B050(noSpace, 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (world::gPlayerRuntimeSlots.items[source].hasDescription == 0) {
                    world::AppendChatMessage_44B050(L"无法装备未鉴定物品！", 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    gCurrentWindow->state |= 0x8000u;
                    world::gBattleSelectionModes5ABC7C[5] = 0;
                    std::sprintf(buffer.data(), "Q|%X|", static_cast<std::uint32_t>(source));
                    static_cast<void>(network::SendBattlePacket_5109B0(
                        platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
                    world::gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                }
            }
        }
        WindowDrag_Reset();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < count; ++position) {
            if (occupancy[position] != 0) {
                continue;
            }
            const std::int32_t left = WindowItemSelector_ReadScreenX(&gItemSelectorD2F338, index, position, -24);
            const std::int32_t top = WindowItemSelector_ReadScreenY(&gItemSelectorD2F338, index, position, -24);
            if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
        consumed = 1;
    }
    auto* const item = items + hovered;
    if ((events & 2u) != 0) {
        gCurrentWindow->state |= 0x2000u;
    }
    if ((events & 2u) != 0 && hovered == -1) {
        if (world::gReplyRecordF7C950.used != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            WindowInstance* const window = gCurrentWindow;
            gItemSelectorD2F338.value3FC = 1;
            window->elements[20].mouseCallback = WindowBattleItems_ShowExtraAttributes;
            return consumed;
        }
    } else if (hovered >= 0) {
        const wchar_t* hint;
        if (item->used != 0 && item->itemType < 23) {
            const std::int32_t type = item->itemType;
            hint = ((type >= 0 && type <= 7) || (type >= 70 && type <= 79))
                && item->hasDescription != 0
                ? L"可装备此物品。"
                : L"无法装备此物品。";
        } else {
            hint = (*reinterpret_cast<const unsigned char*>(&item->battleUseFlag) & 1u) != 0 && item->hasDescription != 0
                ? L"可使用此物品。"
                : L"无法使用此物品。";
        }
        gHoverTextD68AE8 = hint;
    }
    if ((platform::gLegacyMouseState.edgeButtons & 0x10u) != 0 && hovered >= 0 && item->used != 0) {
        if (WindowDrag_ReadFocusWindowId() == 14 || WindowDrag_ReadFocusWindowId() == 0) {
            if (hovered == 2 || hovered == 3) {
                std::int32_t weapons = 0;
                if (items[2].used == 1 && items[2].itemType != 7) {
                    weapons = 1;
                }
                if (items[3].used == 1 && items[3].itemType != 7) {
                    ++weapons;
                }
                if (world::CountFreeInventorySlots_518C60() < weapons) {
                    world::AppendChatMessage_44B050(noSpace, 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (weapons == 0) {
                    world::AppendChatMessage_44B050(L"没有装备武器！", 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    gCurrentWindow->state |= 0x8000u;
                    world::gBattleSelectionModes5ABC7C[5] = 0;
                    std::sprintf(buffer.data(), "Q|%X|", 255u);
                    static_cast<void>(network::SendBattlePacket_5109B0(
                        platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
                    world::gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
            }
            static_cast<void>(WindowDrag_ResetMouse());
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if ((hovered == 2 || hovered == 3) && item->used != 0 && item->itemType != 7) {
                static_cast<void>(WindowItemDrag_Start(hovered, 0));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (item->used != 0) {
                ++gBattleItemPageCB01E0;
                if (gBattleItemPageCB01E0 >= item->pages) {
                    gBattleItemPageCB01E0 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source < count) {
            const std::int32_t left = WindowItemSelector_ReadScreenX(&gItemSelectorD2F338, index, source, -24);
            const std::int32_t top = WindowItemSelector_ReadScreenY(&gItemSelectorD2F338, index, source, -24);
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        const std::int32_t left = WindowItemSelector_ReadScreenX(&gItemSelectorD2F338, index, hovered, -24);
        const std::int32_t top = WindowItemSelector_ReadScreenY(&gItemSelectorD2F338, index, hovered, -24);
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, cg::render::gHoverFramePaletteIndex_D69768, 0);
    }
    if (WindowDrag_ReadKind() != 0 && (events & 0x20u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= count) {
            const std::int32_t type = items[source].itemType;
            if ((type >= 0 && type <= 7) || (type >= 70 && type <= 79)) {
                const WindowInstance* const window = gCurrentWindow;
                const std::uint32_t hoverColor = cg::render::gHoverFramePaletteIndex_D69768;
                const WindowElement* const elements = window->elements;
                const std::int32_t windowY = window->y;
                const std::int32_t windowX = window->x;
                const WindowElement& element = elements[index];
                const std::int32_t elementX = element.x;
                const std::int32_t height = element.height;
                const std::int32_t elementY = element.y;
                const std::int32_t width = element.width;
                const std::int32_t right = width + windowX + elementX;
                const std::int32_t bottom = height + windowY + elementY;
                const std::int32_t top = windowY + elementY;
                const std::int32_t left = windowX + elementX;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(right), static_cast<std::uint16_t>(bottom),
                    223, hoverColor, 0);
            }
        }
    }
    for (std::int32_t position = 0; position < count; ++position) {
        const auto& record = items[position];
        const std::int32_t x = WindowItemSelector_ReadScreenX(&gItemSelectorD2F338, index, position, 0);
        const std::int32_t y = WindowItemSelector_ReadScreenY(&gItemSelectorD2F338, index, position, 0);
        if (record.used == 0) {
            continue;
        }
        if ((position != 2 && position != 3) || record.itemType == 7) {
            static_cast<void>(cg::render::DrawGraphic(x + 105, y + 93, 223, 22738, 0, nullptr));
        }
        if (record.hasDescription == 0) {
            static_cast<void>(cg::render::DrawGraphic(x + 105, y + 93, 223, 22738, 0, nullptr));
        }
        if ((*reinterpret_cast<const unsigned char*>(&record.flags) & 0x80u) == 0) {
            static_cast<void>(cg::render::DrawGraphic(x + 105, y + 93, 223, 22739, 0, &descriptor));
        }
        if (occupancy[position] != 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, record.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, record.imageNo, 0, &descriptor));
        }
    }
    return consumed;
}

} // namespace detail

// 窗口函数实现。

// 原499900固定D2F338；原内嵌+78预览沿已有唯一静态对象绑定。
// 原函数：0x00499900。
static detail::WindowItemSelectorState* WindowBattleItems_ConstructSelector()
{
    return detail::WindowItemSelector_ConstructPreview(&detail::gItemSelectorD2F338, 1);
}

// 原函数：0x004998F0；LCI01启动拥有者替代CRT构造／退出登记。
SecondItemSelectorStartup::SecondItemSelectorStartup()
{
    WindowBattleItems_ConstructSelector();
}

} // namespace cg::ui
