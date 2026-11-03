#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "../Yobi/map_actor.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "../Yobi/base.hpp"
#include "../Yobi/map_transition.hpp"
#include "../Yobi/map_render.hpp"
#include "WorldObjectState.hpp"
#include "../platform/MapEffectMath.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "PlayerState.hpp"
#include "WorldMapState.hpp"
#include "WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>

namespace cg::world::detail {

// YGOS01：完整S32动画图号区间判定；仅445350方向门禁调用，不新增公头接口。
// 原函数：0x005183B0。
std::int32_t IsAnimationImageNumber_5183B0(const std::int32_t imageNo)
{
    return imageNo >= 100000 && imageNo < 200000 ? 1 : 0;
}

// 原函数：0x00444410。
void ApplyWorldActorActionCommand_444410(
    cg::yobi::YobiAction* const action, const std::int32_t x, const std::int32_t y,
    const std::int32_t direction, const std::int32_t characterAction,
    const std::int32_t option1, const std::int32_t option2,
    const std::int32_t option3, const std::int32_t option4)
{
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.mapActor;
    const std::int32_t index = payload->index;
    switch (characterAction) {
    case 0:
        action->pathCountOrDrawOrder_270 = 0;
        [[fallthrough]];
    case 31:
        cg::yobi::SetMapActorAnimationState_43EFC0(action, 0, 1);
        action->direction = direction;
        return;
    case 50: {
        auto& record = gWorldObjects[index];
        record.value34 = direction;
        record.value38 = x;
        record.value3C = y;
        if ((action->mapX200 == static_cast<std::int32_t>(x))
            && (action->mapY210 == static_cast<std::int32_t>(y))) {
            return;
        }
        const auto distance = [](const std::int32_t target,
                                 platform::LegacyCryptInt* const coordinate) {
            const std::int32_t sign =
                (static_cast<std::int32_t>(target) - (*coordinate));
            const std::int32_t value =
                (static_cast<std::int32_t>(target) - (*coordinate));
            return sign < 0 ? -value : value;
        };
        const std::int32_t firstX = distance(x, &action->mapX200);
        const std::int32_t firstY = distance(y, &action->mapY210);
        const bool yLarger = firstX - firstY < 0;
        const std::int32_t secondX = distance(x, &action->mapX200);
        const std::int32_t secondY = distance(y, &action->mapY210);
        const std::int32_t difference = yLarger ? secondY - secondX : secondX - secondY;
        if (difference > 1
            || (reinterpret_cast<const unsigned char*>(&record.flags42)[1] & 2u) != 0) {
            return;
        }
        [[fallthrough]];
    }
    case 1: {
        const std::uint16_t flags = gWorldObjects[index].flags42;
        if ((flags & 0x300u) == 0x300u) {
            if (action->pathCountOrDrawOrder_270 < 10) {
                cg::yobi::AppendInterpolatedMapActorPath_441FC0(action, x, y);
            } else {
                ResetMapFollowersFromPath(*action->ownerSession, action);
                cg::yobi::AppendMapActorPathPoint_441F60(action, x, y);
                gWorldObjects[index].value34 = -1;
            }
        } else {
            cg::yobi::AppendMapActorPathPoint_441F60(action, x, y);
        }
        if (characterAction == 1) {
            action->walkSpeed = option1;
        }
        cg::yobi::SetMapActorAnimationState_43EFC0(action, 1, 0);
        return;
    }
    case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
    case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
    case 18: case 19:
        cg::yobi::SetMapActorAnimationState_43EFC0(action, characterAction, 1);
        if (characterAction == 18 || characterAction == 19) {
            action->lastCharacterAction = -1;
        }
        action->direction = direction;
        return;
    case 30:
        cg::yobi::SetMapActorAnimationState_43EFC0(action, 1, 1);
        action->direction = direction;
        return;
    case 40:
        if (option1 >= 0) {
            Sub_442D60(action, option1);
            cg::yobi::SetMapActorAnimationState_43EFC0(action, 0, 1);
        } else {
            Sub_442D90(action);
        }
        cg::yobi::SnapMapActorToPosition_442AB0(action, x, y);
        action->direction = direction;
        return;
    case 41:
        if (option1 == 1) {
            MarkWorldActorAsPartyLeader_442BD0(action);
        } else {
            ClearWorldActorLeaderMarker_442C00(action);
        }
        action->direction = direction;
        return;
    case 42:
        if (option1 >= 0) {
            Sub_442CA0(action, option1);
        } else {
            Sub_442CD0(action);
        }
        cg::yobi::SnapMapActorToPosition_442AB0(action, x, y);
        action->direction = direction;
        return;
    case 44:
        if (option1 >= 0) {
            Sub_442D00(action, option1);
        } else {
            Sub_442D30(action);
        }
        cg::yobi::SnapMapActorToPosition_442AB0(action, x, y);
        action->direction = direction;
        return;
    case 45:
        if (option1 == 1) {
            ShowWorldActorPersistentStatusIcon_43D970(action);
        } else {
            HideWorldActorPersistentStatusIcon_43D9A0(action);
        }
        action->direction = direction;
        return;
    case 46:
        if (option1 != 0) {
            SetWorldActorStatusIcon_43D9D0(action, option1);
        } else {
            ClearWorldActorStatusIcon_43DA10(action);
        }
        action->direction = direction;
        return;
    case 47:
        ShowWorldActorTimedStatusIcon_43DA40(action);
        action->direction = direction;
        return;
    case 48:
        if (option1 == 1) {
            ShowActorPetPersistentStatusIcon_43DA70(action);
        } else {
            HideActorPetPersistentStatusIcon_43DAA0(action);
        }
        action->direction = direction;
        return;
    case 49:
        ShowActorPetTimedStatusIcon_43DAD0(action);
        action->direction = direction;
        return;
    case 51:
        cg::yobi::SnapMapActorToPosition_442AB0(action, x, y);
        action->pathCountOrDrawOrder_270 = 0;
        cg::yobi::SetMapActorAnimationState_43EFC0(action, 0, 1);
        action->direction = direction;
        gWorldObjects[index].value34 = -1;
        return;
    case 60: {
        action->imageNo = character_select::ResolveCharacterImageForVersion(option1);
        const std::int32_t recordImage = character_select::ResolveCharacterImageForVersion(option1);
        gWorldObjects[payload->index].imageNo = static_cast<std::uint32_t>(recordImage);
        if (option2 > 0) {
            if (payload->mainGear30 == nullptr) {
                const std::int32_t image = character_select::ResolveCharacterImageForVersion(option2);
                payload->mainGear30 = cg::yobi::NewYobiGear_4410F0(action, image);
            }
            if (payload->mainGear30 != nullptr) {
                const std::int32_t image = character_select::ResolveCharacterImageForVersion(option2);
                payload->mainGear30->imageNo = image;
            }
        } else {
            cg::yobi::YobiAction* const gear = payload->mainGear30;
            if (gear != nullptr) {
                cg::yobi::MarkDeath(gear);
                payload->mainGear30 = nullptr;
            }
        }
        if (option3 > 0) {
            std::int32_t image = character_select::ResolveCharacterImageForVersion(option3);
            if (payload->firstAuxiliaryGear34 == nullptr) {
                const std::int32_t petY =
                    static_cast<std::int32_t>(action->mapY1F0);
                const std::int32_t petX =
                    static_cast<std::int32_t>(action->mapX1E0);
                payload->firstAuxiliaryGear34 = cg::yobi::NewYobiGearPet_441130(action, image, petX, petY);
            }
            cg::yobi::YobiAction* const firstPet = payload->firstAuxiliaryGear34;
            if (firstPet != nullptr) {
                firstPet->imageNo = image;
            }
            if (option4 == 6) {
                cg::yobi::YobiAction* const previousSecondPet = payload->secondAuxiliaryGear38;
                ++image;
                if (previousSecondPet == nullptr) {
                    const std::int32_t petY =
                        static_cast<std::int32_t>(action->mapY1F0);
                    const std::int32_t petX =
                        static_cast<std::int32_t>(action->mapX1E0);
                    payload->secondAuxiliaryGear38 = cg::yobi::NewYobiGearPet_441130(action, image, petX, petY);
                }
                cg::yobi::YobiAction* const secondPet = payload->secondAuxiliaryGear38;
                if (secondPet != nullptr) {
                    secondPet->imageNo = image;
                }
            }
        } else {
            cg::yobi::YobiAction* const firstPet = payload->firstAuxiliaryGear34;
            if (firstPet != nullptr) {
                cg::yobi::MarkDeath(firstPet);
                payload->firstAuxiliaryGear34 = nullptr;
            }
            cg::yobi::YobiAction* const secondPet = payload->secondAuxiliaryGear38;
            if (secondPet != nullptr) {
                cg::yobi::MarkDeath(secondPet);
                payload->secondAuxiliaryGear38 = nullptr;
            }
        }
        payload->firstAuxiliaryGearKind3C = option4;
        return;
    }
    case 63:
        SetWorldActorStatusIcon_43D9D0(action, option1);
        action->direction = direction;
        return;
    case 64:
        ClearWorldActorStatusIcon_43DA10(action);
        action->direction = direction;
        return;
    case 70:
        if (option1 >= 0) {
            if (payload->rideActor44 == nullptr) {
                cg::yobi::YobiAction* const ride = cg::yobi::NewYobiAction_404AE0(*action->ownerSession, 2, 0);
                payload->rideActor44 = ride;
                ride->callback = nullptr;
                payload->rideActor44->characterAction = 0;
                payload->rideActor44->displayPriority = 10;
                payload->rideActor44->flags = 0x34u;
                payload->rideActor44->flags |= 0x200u;
            }
            const std::int32_t image = character_select::ResolveCharacterImageForVersion(option1);
            payload->rideActor44->imageNo = image;
            const std::int32_t original =
                std::bit_cast<std::int32_t>(gWorldObjects[payload->index].imageNo);
            action->imageNo = ResolveRidingCharacterImage_51C0D0(original);
            action->phase = cg::yobi::MapActorPhase_Mounting;
        } else {
            action->phase = cg::yobi::MapActorPhase_Dismounting;
        }
        {
            cg::yobi::YobiAction* const ride = payload->rideActor44;
            const bool resetRide = option1 >= 0 || ride != nullptr;
            payload->transitionProgress48 = 0;
            if (resetRide) {
                ride->characterAction = 0;
                if (action->characterAction == 1) {
                    payload->rideActor44->characterAction = 1;
                }
            }
        }
        return;
    case 80:
        action->phase = cg::yobi::MapActorPhase_Spiral;
        action->imageNo = character_select::ResolveCharacterImageForVersion(option1);
        payload->transitionProgress48 = 0;
        return;
    case 200:
        if (option1 != 0) {
            reinterpret_cast<unsigned char*>(&gWorldObjects[payload->index].flags120)[1] |= 0x10u;
        } else {
            gWorldObjects[payload->index].flags120 &= 0xEFFFu;
        }
        return;
    default:
        return;
    }
}

// WBR01：批量补建空action；未知类别仍保创建，仅跳过名字/flags尾，原主体保持。
// 原函数：0x00446640。
void RecreateMissingWorldObjectActions_446640(session::ClientSession& session)
{
    for (std::size_t index = 0; index < kWorldObjectCapacity; ++index) {
        auto& record = gWorldObjects[index];
        if (record.used == 0 || record.action != nullptr) {
            continue;
        }
        const std::int32_t direction = record.direction;
        const std::int32_t mapY =
            static_cast<std::int32_t>(record.coordinate1C);
        const std::int32_t mapX =
            static_cast<std::int32_t>(record.coordinate0C);
        const auto imageNo = std::bit_cast<std::int32_t>(record.imageNo);
        cg::yobi::YobiAction* action =
            cg::yobi::CreateMapActorAction_441DB0(session, imageNo, mapX, mapY, direction);
        record.action = action;
        if (action == nullptr) {
            continue;
        }
        record.used = 2;
        record.action->payload.mapActor->index =
            static_cast<std::int32_t>(index);
        const std::uint16_t category = record.category;
        if (category == 1 || category == 8) {
            action = record.action;
            const std::int32_t level = record.level2C;
            action->field1D0 = level;
            record.action->nameC4 = record.name54;
        } else if (category == 2 || category == 4) {
            const std::size_t limit = category == 2 ? 28u : 16u;
            if (cg::text::ToGbk(record.nameC9).size() <= limit) {
                record.action->nameC4 = record.nameC9;
            } else {
                record.action->nameC4 = L"???";
            }
        } else {
            continue;
        }
        action = record.action;
        const std::uint32_t flags = record.flags114;
        action->flags |= flags;
    }
}

// MOP01：完整对象生产/更新；两坐标三值先取、四256B文本槽原复用关系沿SPEC，原失败/字段宽度保持。
// 原函数：0x00445350。
void UpsertWorldActor_445350(
    session::ClientSession& session, const std::int32_t objectIndex, const std::int32_t imageNo,
    const std::int32_t mapX, const std::int32_t mapY,
    const std::int32_t direction, const char* const name,
    const char* const titles, const char* const extraText,
    const std::int32_t level, const std::int32_t value40,
    const std::int32_t flag1, const std::int32_t flag2,
    const std::int32_t type)
{
    std::int32_t index = FindWorldObjectSlot_444FA0(objectIndex);
    std::int32_t selectedDirection = direction;
    if (index < 0) {
        index = ReserveWorldObjectSlot_446130();
        if (index < 0) {
            return;
        }
        if (type > 3 && gMapResourceMode_9DAD68 == 0 && IsAnimationImageNumber_5183B0(imageNo) == 0) {
            selectedDirection = 0;
        }
        cg::yobi::YobiAction* const action =
            cg::yobi::CreateMapActorAction_441DB0(session, imageNo, mapX, mapY, selectedDirection);
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        record.action = action;
        if (action == nullptr) {
            return;
        }
        record.used = 2;
        record.action->payload.mapActor->index = index;
        record.category = static_cast<std::uint16_t>(MapWorldObjectTypeToCategory_446210(type));
        record.objectIndex = objectIndex;
        record.value34 = -1;
    }

    auto& record = gWorldObjects[static_cast<std::size_t>(index)];
    record.imageNo = static_cast<std::uint32_t>(imageNo);
    record.value40 = static_cast<std::uint16_t>(value40);
    record.level2C = level;
    record.coordinate0C = static_cast<std::int32_t>(mapX);
    record.coordinate1C = static_cast<std::int32_t>(mapY);
    record.direction = selectedDirection;
    if (flag1 != 0) {
        *reinterpret_cast<unsigned char*>(&record.flags42) |= 0x40u;
    }
    if (flag2 != 0) {
        *reinterpret_cast<unsigned char*>(&record.flags42) |= 0x80u;
    }
    if (std::strlen(name) <= 16) {
        record.name54 = cg::text::FromGbkWide(name);
    }

    char text100[256]; // 原var_100：action第4段/type3第1段。
    char text200[256]; // 原var_200：记录/action第2段共用。
    char text300[256]; // 原var_300：记录第1段/action第3段共用。
    char text400[256]; // 原var_400：记录第4段/type3第1段、action普通第1段共用。
    if (type != 3) {
        network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text300);
        input::UnescapeLegacyTextAndNotify_52D2F0(text300);
        if (std::strlen(text300) <= 16) {
            record.title65 = cg::text::FromGbkWide(text300);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, text200);
        input::UnescapeLegacyTextAndNotify_52D2F0(text200);
        if (std::strlen(text200) <= 32) {
            record.text76 = cg::text::FromGbkWide(text200);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 4, 255, text400);
        input::UnescapeLegacyTextAndNotify_52D2F0(text400);
        if (std::strlen(text400) <= 16) {
            record.text128 = cg::text::FromGbkWide(text400);
        }
        if (std::strlen(extraText) <= 16) {
            record.textB8 = cg::text::FromGbkWide(extraText);
        }
    } else {
        network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text400);
        input::UnescapeLegacyTextAndNotify_52D2F0(text400);
        if (std::strlen(text400) <= 16) {
            record.title65 = cg::text::FromGbkWide(text400);
        }
        record.textB8.clear();
    }
    const std::uint32_t flags = MapWorldObjectTypeToDrawFlags_4461E0(type);
    record.flags114 = flags;
    const auto levelBits = static_cast<std::uint32_t>(level);
    if ((flags & 0x1000u) != 0
        && (levelBits & 0x100u) != 0 && (levelBits & 0x40u) != 0) {
        record.flags114 = flags | 0x400000u;
    }
    record.value118 = 1;
    cg::yobi::YobiAction* action = record.action;
    if (action == nullptr) {
        return;
    }
    action->imageNo = imageNo;
    record.action->field1D0 = level;
    action = record.action;
    const std::uint32_t actionFlags = record.flags114;
    action->flags |= actionFlags;
    if (std::strlen(name) <= 16) {
        record.action->nameC4 = cg::text::FromGbkWide(name);
    }

    // 原action阶段独立重新解析，不能复用记录阶段的文本或补写记录第3段。
    if (type != 3) {
        network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text400);
        input::UnescapeLegacyTextAndNotify_52D2F0(text400);
        if (std::strlen(text400) <= 16) {
            record.action->titleE1 = cg::text::FromGbkWide(text400);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, text200);
        input::UnescapeLegacyTextAndNotify_52D2F0(text200);
        if (std::strlen(text200) <= 32) {
            record.action->text102 = cg::text::FromGbkWide(text200);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, text300);
        input::UnescapeLegacyTextAndNotify_52D2F0(text300);
        if (std::strlen(text300) <= 32) {
            record.action->text123 = cg::text::FromGbkWide(text300);
        }
        network::ExtractDelimitedField_52CED0(titles, '|', 4, 255, text100);
        input::UnescapeLegacyTextAndNotify_52D2F0(text100);
        if (std::strlen(text100) <= 16) {
            record.action->text31C = cg::text::FromGbkWide(text100);
        }
        if (std::strlen(extraText) <= 16) {
            record.action->text144 = cg::text::FromGbkWide(extraText);
        }
    } else {
        network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text100);
        input::UnescapeLegacyTextAndNotify_52D2F0(text100);
        if (std::strlen(text100) <= 16) {
            record.action->titleE1 = cg::text::FromGbkWide(text100);
        }
    }
    record.action->hoverTextColor1D8 = value40;
}

// WGQ01：X→fresh Y→类别/必需flags120，返对象编号或-1；保比较读取/RNG与动态上界。
// 原函数：0x00445030。
std::int32_t FindWorldObjectAtWithActionFlags_445030(
    const std::int32_t x, const std::int32_t y,
    const std::uint16_t category, const std::uint16_t flags)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return -1;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            cg::yobi::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0
                && (record.flags120 & flags) != 0) {
                return record.objectIndex;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return -1;
}

// WGQ01：X→fresh Y→类别，返完整0/1，无额外flags门禁；原主体保持。
// 原函数：0x004450D0。
std::int32_t HasWorldObjectAt_4450D0(
    const std::int32_t x, const std::int32_t y, const std::uint16_t category)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return 0;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            cg::yobi::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0) {
                return 1;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return 0;
}

// WGQ01：X→fresh Y→类别，返对象编号或-1，无标志门禁；保动态上界。
// 原函数：0x00445160。
std::int32_t FindWorldObjectAt_445160(
    const std::int32_t x, const std::int32_t y, const std::uint16_t category)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return -1;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            cg::yobi::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0) {
                return record.objectIndex;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return -1;
}

// WGQ01：包含掩码0分别跳过相应W16字段，原X/fresh Y/类别后再检查，返完整0/1。
// 原函数：0x004451F0。
std::int32_t HasWorldObjectAtWithFlags_4451F0(
    const std::int32_t x, const std::int32_t y, const std::uint16_t category,
    const std::uint16_t flags42Mask, const std::uint16_t flags120Mask)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return 0;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            cg::yobi::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0
                && (flags42Mask == 0 || (record.flags42 & flags42Mask) != 0)
                && (flags120Mask == 0 || (record.flags120 & flags120Mask) != 0)) {
                return 1;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return 0;
}

// WGQ01：排除掩码0仍沿原W16字段读取，42失败不取120，返完整0/1；主体保持。
// 原函数：0x004452A0。
std::int32_t HasWorldObjectAtWithoutFlags_4452A0(
    const std::int32_t x, const std::int32_t y, const std::uint16_t category,
    const std::uint16_t excluded42, const std::uint16_t excluded120)
{
    std::int32_t upperBound = gWorldObjectUpperBound;
    if (upperBound <= 0) {
        return 0;
    }
    std::int32_t index = 0;
    do {
        auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0) {
            cg::yobi::YobiAction* const action = record.action;
            if (action != nullptr
                && (action->mapX1E0 == static_cast<std::int32_t>(x))
                && (record.action->mapY1F0 == static_cast<std::int32_t>(y))
                && (record.category & category) != 0
                && (record.flags42 & excluded42) == 0
                && (record.flags120 & excluded120) == 0) {
                return 1;
            }
        }
        upperBound = gWorldObjectUpperBound;
        ++index;
    } while (index < upperBound);
    return 0;
}

// 原函数：0x0046B4B0。
void TurnPlayerAndSendDirection_46B4B0(session::ClientSession& session, const std::int32_t direction)
{
    if (direction < 0 || direction > 7) {
        return;
    }
    cg::yobi::YobiAction* const action = session.WorldScene().player->action;
    if (action == nullptr) {
        return;
    }
    const float movementX = std::bit_cast<float>(gPlayerMovementStepXBits_9EE8FC);
    if (movementX < 0.0F || movementX > 0.0F) {
        return;
    }
    const float movementY = std::bit_cast<float>(gPlayerMovementStepYBits_9EE900);
    if (movementY < 0.0F || movementY > 0.0F
        || gPlayerMovementAndFacingBlocked_62B5AA != 0 || gMapMovementInputBlocked_C82220 != 0
        || action->direction == direction) {
        return;
    }
    const std::uint64_t now = platform::ReadTickCount64();
    if (session.WorldScene().lastExplicitFacingRequestTick + 500u >= now) {
        return;
    }
    static_cast<void>(SetPlayerFacingDirection_51A040(session, direction));
    const std::uint32_t encoded = EncodeMovementDirectionByte_46BD20(direction, 1);
    const char text[2] {
        std::bit_cast<char>(static_cast<std::uint8_t>(encoded)), '\0',
    };
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    static_cast<void>(SendCurrentPlayerMovementPath_50C8D0(session, x, y, text));
    session.WorldScene().lastExplicitFacingRequestTick = platform::ReadTickCount64();
}

// 原函数：0x0046A190。
std::uint32_t UpdatePartyFollowMovement_46A190(session::ClientSession& session)
{
    for (std::size_t position = 0; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        const std::uint32_t playerIndex = session.WorldScene().player->objectIndex;
        cg::yobi::YobiAction* const action = ReadPetMapAction(index);
        const bool isPlayer = ReadPetMapObjectIndex(index) == playerIndex;
        if (isPlayer) {
            const double targetX = static_cast<std::int32_t>(gPlayerSnapshotX) * 64.0;
            const std::uint32_t worldXBits = gWorldCurrentXBits;
            const float worldX = std::bit_cast<float>(worldXBits);
            if (targetX < worldX || targetX > worldX) {
                continue;
            }
            const double targetY = static_cast<std::int32_t>(gPlayerSnapshotY) * 64.0;
            const std::uint32_t worldYBits = gWorldCurrentYBits;
            const float worldY = std::bit_cast<float>(worldYBits);
            if (targetY < worldY || targetY > worldY
                || std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) <= 0) {
                continue;
            }
        } else {
            const double targetX = static_cast<std::int32_t>(action->mapX200) * 64.0;
            const float worldX = action->worldX;
            if (targetX < worldX || targetX > worldX) {
                continue;
            }
            const double targetY = static_cast<std::int32_t>(action->mapY210) * 64.0;
            const float worldY = action->worldY;
            if (targetY < worldY || targetY > worldY
                || std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) <= 0) {
                continue;
            }
            if (position == 0) {
                gPartyCatchUpRateBits_9DAD78 = 0x3F800000u;
                const std::int16_t count =
                    std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270);
                if (count > 5) {
                    gPartyCatchUpRateBits_9DAD78 = 0x40000000u;
                } else if (count >= 4) {
                    gPartyCatchUpRateBits_9DAD78 = 0x3FCCCCCDu;
                } else if (count >= 2) {
                    gPartyCatchUpRateBits_9DAD78 = 0x3F99999Au;
                }
            }
        }

        cg::yobi::YobiAction* const first = ReadPetMapAction(ReadPetFollowIndex(0));
        const std::int32_t nextY = action->pathY248[0];
        const std::int32_t speed = first->walkSpeed;
        const std::int32_t nextX = action->pathX220[0];
        if (isPlayer) {
            SetPlayerMovementTarget_46A410(session, nextX, nextY, speed);
        } else {
            cg::yobi::StartPartyMemberMovement_4422F0(action, nextX, nextY, speed);
        }
        cg::yobi::PopMapActorPathPoint_442460(action);
        // 两路末项前视均在消费前丢弃，不扩展原五槽顺序表。
        if (position + 1 >= 5) {
            continue;
        }
        const std::int16_t nextIndex = ReadPetFollowIndex(position + 1);
        if (nextIndex < 0) {
            continue;
        }
        cg::yobi::YobiAction* const next = ReadPetMapAction(nextIndex);
        const std::int32_t y =
            static_cast<std::int32_t>(action->mapY1F0);
        const std::int32_t x =
            static_cast<std::int32_t>(action->mapX1E0);
        cg::yobi::AppendMapActorPathPoint_441F60(next, x, y);
    }

    for (std::size_t position = 0; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        const std::uint32_t playerIndex = session.WorldScene().player->objectIndex;
        if (ReadPetMapObjectIndex(index) == playerIndex) {
            AdvancePlayerWorldMovement_46A700(session);
        } else {
            cg::yobi::StepPartyMemberMovement_4428F0(ReadPetMapAction(index));
        }
    }
    static_cast<void>(UpdatePlayerMapWindow_468200());
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    gWorldProjectionReferenceXBits = worldXBits;
    gWorldProjectionReferenceYBits = worldYBits;
    return worldXBits;
}

// 原函数：0x0046B1B0。
void MovePlayerOneStep_46B1B0(
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y, const std::int32_t speed)
{
    cg::yobi::YobiAction* const player = session.WorldScene().player->action;
    auto* const payload = player == nullptr
        ? nullptr : player->payload.mapActor;
    const float rate = static_cast<float>(
        static_cast<double>(speed) * 0.01F);
    if (payload != nullptr && payload->attachedActor2C != nullptr) {
        const std::int32_t previousY =
            static_cast<std::int32_t>(player->mapY1F0);
        const std::int32_t previousX =
            static_cast<std::int32_t>(session.WorldScene().player->action->mapX1E0);
        static_cast<void>(cg::yobi::StartMapActorMovement_442110(
            payload->attachedActor2C, previousX, previousY, speed, rate));
    }

    gPlayerSnapshotX = static_cast<std::int32_t>(x);
    gPlayerSnapshotY = static_cast<std::int32_t>(y);

    const std::int32_t pixelX =
        (gPlayerSnapshotX * 64);
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const float differenceX = static_cast<float>(
        static_cast<double>(pixelX) - std::bit_cast<float>(worldXBits));
    const std::int32_t pixelY =
        (gPlayerSnapshotY * 64);
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    const platform::detail::MapMoveDifference difference {
        differenceX,
        static_cast<double>(pixelY) - std::bit_cast<float>(worldYBits),
    };
    platform::detail::MapMoveDirection direction;
    platform::detail::Cg2dMapMoveDirection(&difference, &direction);
    gPlayerMovementStepXBits_9EE8FC = std::bit_cast<std::uint32_t>(
        static_cast<float>(direction.unroundedX * rate * 4.0F));
    gPlayerMovementStepYBits_9EE900 = std::bit_cast<std::uint32_t>(
        static_cast<float>(static_cast<double>(direction.y) * rate * 4.0F));
    if (direction.unroundedX < 0.0 || direction.unroundedX > 0.0
        || direction.y < 0.0F || direction.y > 0.0F) {
        const auto value = static_cast<std::int32_t>(
            GetEightWayMapDirection_46CDF0(direction.x, direction.y));
        static_cast<void>(SetPlayerFacingDirection_51A040(session, value));
        static_cast<void>(MarkPlayerMovementActive_51A640(session));
    }
    SynchronizePlayerMapActionPosition_519EE0(session);
}

// 原函数：0x0046A5F0。
void AdvanceFollowersBehindPlayerLeader_46A5F0(session::ClientSession& session)
{
    if (gPartyActive_FF0D36 == 0 || (ReadPlayerFlags18A(session) & 0x100u) == 0) {
        return;
    }
    for (std::size_t position = 1; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        cg::yobi::YobiAction* const action = ReadPetMapAction(index);
        const double targetX = static_cast<std::int32_t>(action->mapX200) * 64.0;
        const float worldX = action->worldX;
        if (targetX < worldX || targetX > worldX) {
            continue;
        }
        const double targetY = static_cast<std::int32_t>(action->mapY210) * 64.0;
        const float worldY = action->worldY;
        if (targetY < worldY || targetY > worldY
            || std::bit_cast<std::int16_t>(action->pathCountOrDrawOrder_270) <= 0) {
            continue;
        }

        const std::int32_t speed = session.WorldScene().player->walkSpeed;
        const std::int32_t nextY = action->pathY248[0];
        const std::int32_t nextX = action->pathX220[0];
        static_cast<void>(
            cg::yobi::StartMapActorMovement_442110(action, nextX, nextY, speed, 0.0F));
        cg::yobi::PopMapActorPathPoint_442460(action);
        // 原末位置的9FB942前视值无论正负都被丢弃；不虚构第六槽。
        if (position + 1 >= 5) {
            continue;
        }
        const std::int16_t nextIndex = ReadPetFollowIndex(position + 1);
        if (nextIndex < 0) {
            continue;
        }
        cg::yobi::YobiAction* const nextAction = ReadPetMapAction(nextIndex);
        const std::int32_t y =
            static_cast<std::int32_t>(action->mapY1F0);
        const std::int32_t x =
            static_cast<std::int32_t>(action->mapX1E0);
        cg::yobi::AppendMapActorPathPoint_441F60(nextAction, x, y);
    }
}

// 原函数：0x0051A640。
cg::yobi::YobiAction* MarkPlayerMovementActive_51A640(session::ClientSession& session)
{
    cg::yobi::YobiAction* const action = session.WorldScene().player->action;
    if (action != nullptr) {
        action->movementActive_272 = 1;
    }
    return action;
}

// 原函数：0x0051A660。
cg::yobi::YobiAction* ClearPlayerMovementActive_51A660(session::ClientSession& session)
{
    cg::yobi::YobiAction* const action = session.WorldScene().player->action;
    if (action != nullptr) {
        action->movementActive_272 = 0;
    }
    return action;
}

// 原函数：0x0051A680。
std::int32_t ReadPlayerMovementActive_51A680(session::ClientSession& session)
{
    cg::yobi::YobiAction* const action = session.WorldScene().player->action;
    return action == nullptr ? 0 : std::bit_cast<std::int16_t>(action->movementActive_272);
}

} // namespace cg::world::detail

namespace cg::world {

// 原函数：0x0050AEF0。
void HandlePartyActorVitalsReply_50AEF0(cg::network::CgConnection&, const char* const payload)
{
    char text[2048];
    for (std::int32_t field = 3; field < 28; field += 5) {
        network::ExtractDelimitedField_52CED0(payload, '|', field - 2, 2047, text);
        if (std::strlen(text) == 0) {
            return;
        }
        const std::int32_t objectIndex = network::Base62Decode_52D050(text);
        network::ExtractDelimitedField_52CED0(payload, '|', field - 1, 2047, text);
        const std::int32_t hp = std::atoi(text);
        network::ExtractDelimitedField_52CED0(payload, '|', field, 2047, text);
        const std::int32_t maxHp = std::atoi(text);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 2047, text);
        const std::int32_t fp = std::atoi(text);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 2, 2047, text);
        const std::int32_t maxFp = std::atoi(text);
        cg::yobi::YobiAction* const action = FindObjectYobi_4462B0(objectIndex);
        if (action == nullptr) {
            continue;
        }

        // 四个目标的发布顺序不同，保留每次构造/复制/析构，不统一同类operator=。
        action->readiness = static_cast<std::int32_t>(hp);

        action->maxHp = static_cast<std::int32_t>(maxHp);

        action->barValue1B0 = static_cast<std::int32_t>(fp);

        action->barMaximum1C0 = static_cast<std::int32_t>(maxFp);
    }
}

// WLR01：仅已占用/空action恢复，方向→第二/第一坐标→图号，创建后按原借用/字节复制/flags发布。
// 原函数：0x004467D0。
void RecreateWorldObjectAction_4467D0(
    cg::yobi::YobiAction* const source, const std::int32_t objectIndex)
{
    if (objectIndex < 0) {
        return;
    }
    const std::int32_t index = detail::FindWorldObjectSlot_444FA0(objectIndex);
    if (index < 0) {
        return;
    }
    auto& record = detail::gWorldObjects[static_cast<std::size_t>(index)];
    if (record.used == 0 || record.action != nullptr) {
        return;
    }
    const std::int32_t direction = record.direction;
    const std::int32_t mapY =
        static_cast<std::int32_t>(record.coordinate1C);
    const std::int32_t mapX =
        static_cast<std::int32_t>(record.coordinate0C);
    const auto imageNo = std::bit_cast<std::int32_t>(record.imageNo);
    cg::yobi::YobiAction* action =
        cg::yobi::CreateMapActorAction_441DB0(*source->ownerSession, imageNo, mapX, mapY, direction);
    record.action = action;
    if (action == nullptr) {
        return;
    }
    record.used = 2;
    record.action->payload.mapActor->index = index;
    action = record.action;
    const std::int32_t level = record.level2C;
    action->field1D0 = level;
    record.action->nameC4 = record.name54;
    record.action->titleE1 = record.title65;
    action = record.action;
    const std::uint32_t flags = record.flags114;
    action->flags |= flags;
}

// 原函数：0x00519EE0。
void SynchronizePlayerMapActionPosition_519EE0(session::ClientSession& session)
{
    if (session.WorldScene().player->action == nullptr) {
        return;
    }
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    const std::int32_t blocked = IsMapCellBlocked_469810(x, y);
    cg::yobi::YobiAction* action = session.WorldScene().player->action;
    if (blocked != 0) {
        gPlayerX = action->mapX1E0;
        gPlayerY = action->mapY1F0;
    }

    const std::uint32_t worldXBits = gWorldCurrentXBits;
    action->worldX = std::bit_cast<float>(worldXBits);
    action = session.WorldScene().player->action;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    action->worldY = std::bit_cast<float>(worldYBits);
    action = session.WorldScene().player->action;
    action->mapX1E0 = gPlayerX;
    action = session.WorldScene().player->action;
    action->mapY1F0 = gPlayerY;
    action = session.WorldScene().player->action;
    action->movementStepXBits_27C = detail::gPlayerMovementStepXBits_9EE8FC;
    const std::uint32_t movementY = detail::gPlayerMovementStepYBits_9EE900;
    action = session.WorldScene().player->action;
    action->movementStepYBits_280 = movementY;
    action = session.WorldScene().player->action;
    action->mapX200 = gPlayerSnapshotX;
    action = session.WorldScene().player->action;
    action->mapY210 = gPlayerSnapshotY;
}

} // namespace cg::world
