#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x0048C150。
std::int32_t WindowPetList_ResetSelection()
{
    gLoginWindowRuntime.valueD673D0 = 5;
    gLoginWindowRuntime.selectedPetCC3E5C = 0;
    for (std::int32_t& value : gLoginWindowRuntime.valuesCC1A18) {
        value = 0;
    }
    gLoginWindowRuntime.valuesCC4CE8[0] = 0;
    gLoginWindowRuntime.valuesCC4CE8[2] = 0;
    gLoginWindowRuntime.valuesCC4CE8[3] = 0;
    gLoginWindowRuntime.valuesCC4CE8[4] = 0;
    gLoginWindowRuntime.valuesCC4CE8[1] = 0;
    gLoginWindowRuntime.valuesD2EAA0[0] = 0;
    gLoginWindowRuntime.valuesD2EAA0[1] = -1;
    gLoginWindowRuntime.valuesD2EAA0[2] = -1;
    return -1;
}
namespace {


// 原511AE0三处调用均先逆序读取五状态，最后读取socket。
std::int32_t WindowPetList_SendStates(const std::array<std::int32_t, 5>& values)
{
    const std::int32_t fifth = values[4];
    const std::int32_t fourth = values[3];
    const std::int32_t third = values[2];
    const std::int32_t second = values[1];
    const std::int32_t first = values[0];
    const std::uintptr_t socket = platform::gConnectionSocket;
    return network::Sub_511AE0(socket, first, second, third, fourth, fifth);
}

constexpr wchar_t kPetPlacedHint586C28[] = L"無法將此寵物放在地上。"; // 原常量参考：0x00586C28。

constexpr wchar_t kPetBattleHint586C40[] = L"使這隻寵物處於戰鬥狀態。"; // 原常量参考：0x00586C40。

constexpr wchar_t kPetBattlingHint586C5C[] = L"這隻寵物正處於戰鬥狀態。"; // 原常量参考：0x00586C5C。

constexpr wchar_t kPetReadyHint586C78[] = L"使這隻寵物處於待命狀態。"; // 原常量参考：0x00586C78。

constexpr wchar_t kPetReadiedHint586C94[] = L"這隻寵物正處於待命狀態。"; // 原常量参考：0x00586C94。

constexpr wchar_t kPetFollowHint586CD8[] = L"將此寵物帶在身後散步。"; // 原常量参考：0x00586CD8。

constexpr wchar_t kPetFollowingHint586CF0[] = L"無法將此寵物帶在身後散步。"; // 原常量参考：0x00586CF0。

constexpr wchar_t kPetDetailsHint586D0C[] = L"打開此寵物的能力視窗。"; // 原常量参考：0x00586D0C。

constexpr wchar_t kPetLowLoyalty586CB0[] = // 原常量参考：0x00586CB0。
    L"寵物忠誠度需達到60以上才可以帶它散步喔!";
} // namespace


// 原函数：0x0048D4E0。
std::int32_t WindowPetList_SubmitStates()
{
    std::array<std::int32_t, 5> values;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        const std::int32_t follow = (*world::detail::BorrowPetRuntimeF633F8Storage())[slot].follow6D4;
        const std::int32_t state = gLoginWindowRuntime.valuesCC1A18[slot];
        values[slot] = state;
        if (follow != 0) {
            values[slot] = state | 0x10;
        }
    }
    return WindowPetList_SendStates(values);
}

// 原函数：0x0048C1E0。
std::int32_t WindowPetList_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    auto& states = gLoginWindowRuntime.valuesCC1A18;
    const auto petIndex = [](const std::int32_t row) {
        return std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
    };
    const auto setImage = [index](const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = image;
    };

    if (index >= 253 && index <= 257) {
        const std::int32_t row = index - 253;
        std::uint32_t events = static_cast<std::uint32_t>(state);
        if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 3) {
            consumed = 1;
            events &= ~0x60u;
        }
        if ((events & 0x40u) != 0) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            const std::int16_t target = petIndex(row);
            WindowItem_RequestPetTransfer(15, index, source, target);
            WindowDrag_Reset();
        }
        if ((events & 1u) != 0) {
            gHoverTextD68AE8 = kPetDetailsHint586D0C;
            consumed = 1;
        }
        if ((events & 2u) != 0) {
            const std::int16_t selected = petIndex(row);
            const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[selected];
            if (pet.used != 0) {
                WindowInstance* const detailsWindow = gWindowStates[WindowId_PetDetail].window;
                gLoginWindowRuntime.selectedPetCC3E5C = row;
                gLoginWindowRuntime.valueD673D0 = 5;
                if (detailsWindow != nullptr || gWindowStates[WindowId_PetAttributes].window != nullptr
                    || gWindowStates[WindowId_PetSkills].window != nullptr) {
                    const char* const name =
                        pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    const std::size_t length = std::strlen(name) + 1;
                    const std::size_t bulk = length & ~std::size_t{3};
                    gPetNameInputPrototype57DCF8.text = cg::text::FromGbkWide(name);
                    WindowInstance* const window = gCurrentWindow;
                    const std::int32_t y = window->y;
                    const std::int32_t x = window->x;
                    static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
                } else {
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetDetail, 1, 0, nullptr));
                }
            }
            static_cast<void>(WindowDrag_ResetMouse());
            consumed = 1;
        } else if ((events & 1u) != 0 && (events & 4u) != 0) {
            const std::int16_t selected = petIndex(row);
            if ((*world::detail::BorrowPetRuntimeF633F8Storage())[selected].used != 0) {
                static_cast<void>(WindowPetDrag_Start(selected, 0));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
        gLoginWindowRuntime.valuesD2EAA0[0] = 0;
        gLoginWindowRuntime.valuesD2EAA0[1] = -1;
        if (WindowDrag_ReadKind() == 3) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            gLoginWindowRuntime.valuesD2EAA0[0] = 1;
            gLoginWindowRuntime.valuesD2EAA0[1] = WindowItem_FindSortedPetIndex(source);
        }
        return consumed;
    }

    if (index >= 226 && index <= 230) {
        const std::int32_t row = index - 226;
        if (character_select::state::gPukVersion < 7u) {
            gCurrentWindow->elements[index].visible = 0;
            return 0;
        }
        if (WindowPet_HasSkill25600(petIndex(row)) != 1) {
            return 0;
        }
        if ((state & 1) != 0) {
            setImage(0x3B588);
            const std::int32_t follow = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex(row)].follow6D4;
            gHoverTextD68AE8 = follow != 0 ? kPetFollowingHint586CF0 : kPetFollowHint586CD8;
            consumed = 1;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        const std::int16_t selected = petIndex(row);
        if ((*world::detail::BorrowPetRuntimeF633F8Storage())[selected].follow6D4 != 0) {
            static_cast<void>(WindowPetList_SendStates(states));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            return consumed;
        }
        std::int32_t followCount = 0;
        for (std::int32_t position = 0; position < 5; ++position) {
            if ((*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex(position)].follow6D4 != 0) {
                ++followCount;
            }
        }
        if (followCount > 0) {
            return consumed;
        }
        if ((*world::detail::BorrowPetRuntimeF633F8Storage())[selected].values70[10] < 60) {
            world::detail::Sub_44B050(kPetLowLoyalty586CB0, 4, 3);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            return consumed;
        }
        states[selected] |= 0x10;
        static_cast<void>(WindowPetList_SendStates(states));
        states[petIndex(row)] &= ~0x10;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }

    if (index >= 231 && index <= 235) {
        const std::int32_t row = index - 231;
        if ((state & 1) != 0) {
            setImage(0x3B58B);
            gHoverTextD68AE8 = states[petIndex(row)] == 1 ? kPetReadiedHint586C94 : kPetReadyHint586C78;
            consumed = 1;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        const std::int16_t selected = petIndex(row);
        const std::int32_t previous = states[selected];
        if (previous == 3) {
            return consumed;
        }
        if (previous == 1) {
            states[selected] = 0;
            static_cast<void>(WindowPetList_SubmitStates());
            return consumed;
        }
        if (previous == 2) {
            WindowCraft_ResetItemSelection();
        }
        std::int32_t count = 0;
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            const std::int32_t value = states[slot];
            if (value == 1) {
                ++count;
            }
            if (value == 2) {
                ++count;
            }
        }
        const std::int16_t target = petIndex(row);
        if (count >= 3 && states[target] != 2) {
            const std::int32_t firstReady = gLoginWindowRuntime.valuesCC4CE8[2];
            states[firstReady] &= ~1;
        }
        states[target] = 1;
        static_cast<void>(WindowPetList_SubmitStates());
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }

    if (index >= 236 && index <= 240) {
        const std::int32_t row = index - 236;
        if ((state & 1) != 0) {
            setImage(0x3B58E);
            gHoverTextD68AE8 = states[petIndex(row)] == 2 ? kPetBattlingHint586C5C : kPetBattleHint586C40;
            consumed = 1;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        const std::int16_t selected = petIndex(row);
        const std::int32_t previous = states[selected];
        if (previous == 3) {
            return consumed;
        }
        if (previous == 2) {
            states[selected] = 0;
        } else {
            const std::int32_t oldBattle = gLoginWindowRuntime.valuesCC4CE8[0];
            if (oldBattle != -1) {
                states[oldBattle] &= ~2;
            }
            states[selected] = 2;
        }
        static_cast<void>(WindowPetList_SubmitStates());
        WindowCraft_ResetItemSelection();
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }

    if (index >= 241 && index <= 245) {
        const std::int32_t row = index - 241;
        if ((state & 1) != 0) {
            setImage(0x3B591);
            gHoverTextD68AE8 = states[petIndex(row)] == 3 ? kPetPlacedHint586C28 : kPetPlaceHint586C14;
            consumed = 1;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        if (states[petIndex(row)] == 2) {
            WindowCraft_ResetItemSelection();
        }
        const std::int16_t selected = petIndex(row);
        if (states[selected] == 3) {
            states[selected] = 0;
            static_cast<void>(WindowPetList_SubmitStates());
        } else if ((world::detail::ReadPlayerActionFlags() & 0x80u) == 0
            && world::detail::Sub_440B50() == 0) {
            states[petIndex(row)] = 3;
            static_cast<void>(WindowPetList_SubmitStates());
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return consumed;
    }
    return 0;
}

// 原函数：0x0048D460。
void WindowPetList_RefreshStates()
{
    gLoginWindowRuntime.valuesCC4CE8[0] = -1;
    gLoginWindowRuntime.valuesCC4CE8[2] = -1;
    gLoginWindowRuntime.valuesCC4CE8[3] = -1;
    gLoginWindowRuntime.valuesCC4CE8[4] = -1;
    gLoginWindowRuntime.valuesCC4CE8[1] = -1;
    gLoginWindowRuntime.countCC4CFC = 0;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[slot];
        const std::int32_t state = std::bit_cast<std::int16_t>(pet.flags6AA);
        gLoginWindowRuntime.valuesCC1A18[slot] = state;
        if (state == 2) {
            gLoginWindowRuntime.valuesCC4CE8[0] = slot;
        } else if (state == 1) {
            const std::int32_t count = gLoginWindowRuntime.countCC4CFC;
            gLoginWindowRuntime.valuesCC4CE8[count + 2] = slot;
            gLoginWindowRuntime.countCC4CFC = gLoginWindowRuntime.countCC4CFC + 1;
        }
        if (pet.follow6D4 != 0) {
            gLoginWindowRuntime.valuesCC4CE8[1] = slot;
        }
    }
}
namespace {


// 原48C920四段同构属性条：先逐格隐藏，再取S16宠物索引，图号/显隐分别重取窗口。
void WindowPetList_RefreshAttributeBar(
    const std::int32_t row, const world::detail::PetPairRecord* const pair,
    const std::int32_t attribute, const std::int32_t singleImage,
    const std::int32_t middleImage, const std::int32_t firstImage,
    const std::int32_t lastImage)
{
    const std::int32_t base = 26 + row * 40 + attribute * 10;
    for (std::int32_t segment = 0; segment < 10; ++segment) {
        gCurrentWindow->elements[base + segment].visible = 0;
    }
    const std::int16_t petIndex = std::bit_cast<std::int16_t>(pair->index);
    const std::int32_t segments =
        (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].values70[21 + attribute] / 10;
    const auto show = [](const std::int32_t element, const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[element].data)->imageNo = image;
        gCurrentWindow->elements[element].visible = 1;
    };
    if (segments == 1) {
        show(base, singleImage);
    } else {
        for (std::int32_t segment = 0; segment < segments; ++segment) {
            show(base + segment, middleImage);
            if (segment == 0) {
                show(base, firstImage);
            }
            if (segment == segments - 1) {
                show(base + segment, lastImage);
            }
        }
    }
}
} // namespace


// 原函数：0x0048C920。
void WindowPetList_Refresh()
{
    WindowPetList_RefreshStates();
    std::wstring buffer;
    const auto copyNumber = [&buffer](const std::int32_t element) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[element].data);
        const std::size_t length = buffer.size() + 1;
        data->text = buffer;
    };
    const auto setImage = [](const std::int32_t element, const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[element].data)->imageNo = image;
    };
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto* const pair = &world::detail::gPetPairsDE0B08[row];
        // 只固定索引对地址，技能查询和各次CryptInt读取之间重新取编号。
        const auto petIndex = [pair]() {
            return std::bit_cast<std::int16_t>(pair->index);
        };
        const std::int16_t initialIndex = petIndex();
        WindowInstance* const initialWindow = gCurrentWindow;
        const std::int32_t textBase = 1 + row * 5;
        if ((*world::detail::BorrowPetRuntimeF633F8Storage())[initialIndex].used == 0) {
            initialWindow->elements[textBase].visible = 0;
            for (std::int32_t column = 1; column < 5; ++column) {
                gCurrentWindow->elements[textBase + column].visible = 0;
            }
            for (std::int32_t attribute = 0; attribute < 4; ++attribute) {
                for (std::int32_t segment = 0; segment < 10; ++segment) {
                    gCurrentWindow->elements[26 + row * 40 + attribute * 10 + segment].visible = 0;
                }
            }
            gCurrentWindow->elements[231 + row].visible = 0;
            gCurrentWindow->elements[236 + row].visible = 0;
            gCurrentWindow->elements[226 + row].visible = 0;
            gCurrentWindow->elements[241 + row].visible = 0;
            continue;
        }

        initialWindow->elements[textBase].visible = 1;
        for (std::int32_t column = 1; column < 5; ++column) {
            gCurrentWindow->elements[textBase + column].visible = 1;
        }
        gCurrentWindow->elements[226 + row].visible = 1;
        gCurrentWindow->elements[231 + row].visible = 1;
        gCurrentWindow->elements[236 + row].visible = 1;
        if (WindowPet_HasSkill25600(petIndex()) == 1) {
            gCurrentWindow->elements[226 + row].visible = 1;
        }
        const std::int16_t loyaltyIndex = petIndex();
        const bool canSelect = (*world::detail::BorrowPetRuntimeF633F8Storage())[loyaltyIndex].values70[10] == 100
            || gLoginWindowRuntime.valuesCC1A18[loyaltyIndex] == 3;
        gCurrentWindow->elements[241 + row].visible = canSelect ? 1 : 0;

        const auto& namePet = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex()];
        const char* const name =
            namePet.name6BD[0] != '\0' ? namePet.name6BD.data() : namePet.name6AC.data();
        WindowElement* const nameElements = gCurrentWindow->elements;
        const std::size_t nameLength = std::strlen(name) + 1;
        auto* const nameData = static_cast<WindowTextData*>(nameElements[textBase].data);
        nameData->text = cg::text::FromGbkWide(name);

        const std::int16_t colorIndex = petIndex();
        WindowInstance* const colorWindow = gCurrentWindow;
        const std::int32_t colorState = gLoginWindowRuntime.valuesCC1A18[colorIndex];
        static_cast<WindowTextData*>(colorWindow->elements[textBase].data)->bytes00[0] =
            colorState == 3 ? std::byte{7} : std::byte{0};
        cg::text::FormatTo(buffer, L"{:3}", (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex()].value08);
        copyNumber(textBase + 1);
        const std::int32_t maxHp = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex()].values0C[1].value();
        const std::int32_t hp = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex()].values0C[0].value();
        cg::text::FormatTo(buffer, L"{:6}/{:6}", hp, maxHp);
        copyNumber(textBase + 2);
        const std::int32_t maxFp = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex()].values0C[3].value();
        const std::int32_t fp = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex()].values0C[2].value();
        cg::text::FormatTo(buffer, L"{:6}/{:6}", fp, maxFp);
        copyNumber(textBase + 3);

        const std::int16_t tribeIndex = petIndex();
        WindowInstance* const tribeWindow = gCurrentWindow;
        const std::int16_t tribe =
            std::bit_cast<std::int16_t>((*world::detail::BorrowPetRuntimeF633F8Storage())[tribeIndex].value04);
        WindowElement* const tribeElements = tribeWindow->elements;
        const wchar_t* const tribeName = gTribeNames57CB54[tribe];
        auto* const tribeData = static_cast<WindowTextData*>(tribeElements[textBase + 4].data);
        tribeData->text = tribeName;

        const std::int32_t state = gLoginWindowRuntime.valuesCC1A18[petIndex()];
        if (state >= 0 && state <= 3) {
            setImage(231 + row, state == 1 ? 0x3B589 : 0x3B58A);
            setImage(236 + row, state == 2 ? 0x3B58C : 0x3B58D);
            setImage(241 + row, state == 3 ? 0x3B58F : 0x3B590);
        }
        if (WindowPet_HasSkill25600(petIndex()) == 1) {
            const std::int16_t followIndex = petIndex();
            WindowInstance* const followWindow = gCurrentWindow;
            const std::int32_t follow = (*world::detail::BorrowPetRuntimeF633F8Storage())[followIndex].follow6D4;
            static_cast<WindowGraphicData*>(followWindow->elements[226 + row].data)->imageNo =
                follow != 0 ? 0x3B586 : 0x3B587;
        } else {
            gCurrentWindow->elements[226 + row].visible = 0;
        }
        WindowPetList_RefreshAttributeBar(row, pair, 0, 0x3B5D2, 0x3B5D0, 0x3B5CF, 0x3B5D1);
        WindowPetList_RefreshAttributeBar(row, pair, 1, 0x3B5D7, 0x3B5D5, 0x3B5D4, 0x3B5D6);
        WindowPetList_RefreshAttributeBar(row, pair, 2, 0x3B5DC, 0x3B5DA, 0x3B5D9, 0x3B5DB);
        WindowPetList_RefreshAttributeBar(row, pair, 3, 0x3B5E1, 0x3B5DF, 0x3B5DE, 0x3B5E0);
    }
    for (std::int32_t row = 0; row < 5; ++row) {
        const bool selected = gLoginWindowRuntime.valuesD2EAA0[0] == 1
            && gLoginWindowRuntime.valuesD2EAA0[1] == row;
        setImage(246 + row, selected ? 0x3BAE4 : 0x3B5C4);
    }
}

// 原函数：0x0048C1B0。
// 关联原入口：0x0048C1CF（共享尾已并入本函数正文）。
std::int32_t WindowPetList_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valuesD2EAA0[0] = 0;
        gLoginWindowRuntime.valuesD2EAA0[1] = -1;
        gLoginWindowRuntime.valuesD2EAA0[2] = -1;
    }
    WindowPetList_Refresh();
    return 1;
}

} // namespace detail

} // namespace cg::ui
