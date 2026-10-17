#include "battle_message.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_scene.hpp"
#include "battle_skill.hpp"
#include "battle_skill_effect.hpp"
#include "battle_status.hpp"
#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>

namespace cg::yobi {

namespace detail {

// 原427470独占的同源W32；活动路径先从载荷恢复，失活消息先解析位置，不依赖初值。
std::int32_t gBattleMCasterPosition5AAA64 = 0; // 原全局：0x005AAA64。

std::uint32_t gBattleMResultFlags5A4A20 = 0; // 原全局：0x005A4A20。

std::int32_t gBattleMValue5ABA7C = 0; // 原全局：0x005ABA7C。

namespace {

std::int32_t gBattleMDefenderPosition5A4A24 = 0; // 原全局：0x005A4A24。

} // namespace

std::int32_t gBattleMTargetPosition5AAA40 = 0; // 原全局：0x005AAA40。

std::int32_t gBattleMSkill5AAA78 = 0; // 原全局：0x005AAA78。

std::int32_t gBattleMTechnique5A4A38 = 0; // 原全局：0x005A4A38。

namespace {

std::int32_t gBattleMValue59FA10 = 0; // 原全局：0x0059FA10。

} // namespace

std::int32_t gBattleMValue5A4A34 = 0; // 原全局：0x005A4A34。

std::int32_t gBattleMValue5ABA90 = 0; // 原全局：0x005ABA90。

std::uint32_t gBattleMFlags5ABA84 = 0; // 原全局：0x005ABA84。

std::int32_t gBattleMValue5A4A28 = 0; // 原全局：0x005A4A28。

std::int32_t gBattleMMessageValue59FA14 = 0; // 原全局：0x0059FA14。

namespace {

std::int32_t gBattleMStatusMode5ABA88 = 0; // 原全局：0x005ABA88。 PSN先写后读，不属于13项回存。

} // namespace

std::int32_t gBattleMSkillLevel5A4A2C = 0; // 原全局：0x005A4A2C。 SKL状态0先写后读，不属于13项快照。

std::int32_t gBattleMSkillLevelIndex5A3A18 = 0; // 原全局：0x005A3A18。 与88/8C独立的归一化等级索引。

namespace {

enum class BattleMTransitionHeader : std::int32_t {
    Escape = 1, Clear = 2, Support = 3, PetNotice = 15, PetLeave = 16, PetDown = 18,
};

// 439B30的SKL字段布局；同布局合并不改变技能集合、RID尾扫描或特殊字段。
enum class BattlePreviewSkillLayout {
    None, Attack, Mode73, Damage, ValuesRide, PairRide, ValuesOnly, PairOnly,
    SourceValue, NamedTargets, Mode98, OptionalPair, DirectValues, Team,
    DirectRecords, RideRecords, EffectRecords,
};

BattlePreviewSkillLayout GetBattlePreviewSkillLayout(const std::int32_t skill)
{
    switch (skill) {
    case 0: case 1: case 3: case 5: case 6: case 72:
    case 75: case 76: case 77: case 78: case 79: case 80: case 81:
    case 84: case 85: case 87: case 94: case 95: case 96: case 105: case 110: case 113:
    case 257: case 258: case 259: case 260: case 262: case 266:
    case 2007: case 2008: case 2016: case 2017: case 2021: case 2022:
    case 2096: case 2097: case 2098: case 2099: case 2100: case 2101:
    case 3019: case 3038: case 3100: case 3101: case 4002:
        return BattlePreviewSkillLayout::Attack;
    case 73:
        return BattlePreviewSkillLayout::Mode73;
    case 4: case 19: case 20: case 21: case 22: case 23: case 24: case 25:
    case 26: case 27: case 28: case 29: case 30: case 31: case 82: case 97:
    case 267: case 2005: case 2055: case 2056: case 2057:
    case 2068: case 2069: case 2070: case 2071: case 2072: case 2073:
    case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
    case 3029: case 3037: case 4003:
        return BattlePreviewSkillLayout::Damage;
    case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39:
    case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
    case 48: case 49: case 54: case 55: case 56: case 57: case 58: case 59: case 60:
    case 64: case 65: case 66: case 67: case 68: case 86:
    case 2006: case 2010: case 2011: case 2012: case 2013: case 2014: case 2015:
    case 2019: case 2027: case 2050: case 2051: case 2052:
    case 2058: case 2059: case 2060: case 2061: case 2062: case 2063:
    case 2064: case 2065: case 2066: case 2067:
    case 2089: case 2090: case 2091: case 2092: case 2093: case 2094: case 2095:
    case 3002: case 3003: case 3004: case 3005: case 3006: case 3007:
    case 3008: case 3009: case 3010: case 3011: case 3012: case 3013:
    case 3014: case 3015: case 3016:
        return BattlePreviewSkillLayout::ValuesRide;
    case 61: case 62: case 63: case 83: case 2028:
        return BattlePreviewSkillLayout::PairRide;
    case 99: case 100: case 101: case 102: case 103: case 104:
    case 107: case 109: case 112: case 114: case 117: case 3001:
        return BattlePreviewSkillLayout::ValuesOnly;
    case 108:
        return BattlePreviewSkillLayout::PairOnly;
    case 12: case 50: case 51: case 52: case 53: case 88:
        return BattlePreviewSkillLayout::SourceValue;
    case 93:
        return BattlePreviewSkillLayout::NamedTargets;
    case 98:
        return BattlePreviewSkillLayout::Mode98;
    case 263:
        return BattlePreviewSkillLayout::OptionalPair;
    case 400: case 401:
        return BattlePreviewSkillLayout::DirectValues;
    case 402:
        return BattlePreviewSkillLayout::Team;
    case 405:
        return BattlePreviewSkillLayout::DirectRecords;
    case 404: case 406:
        return BattlePreviewSkillLayout::RideRecords;
    case 106: case 111: case 2002:
        return BattlePreviewSkillLayout::EffectRecords;
    default:
        return BattlePreviewSkillLayout::None;
    }
}

} // namespace

// 原函数：0x00439070。
std::int32_t GetBattleMHeader_439070(char* const destination)
{
    using namespace world::detail;
    std::int32_t offset = gBattleMScanOffset5A8A3C;
    if (gBattleMText5AAA7C[offset] == '\0' || gBattleMText5AAA7C[offset + 1] == '\0') {
        return 1;
    }
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gBattleMText5AAA7C[offset]));
        if (value == 0) {
            gBattleMScanOffset5A8A3C = offset;
            return 1;
        }
        if (value >= 'A' && value <= 'Z') {
            break;
        }
        ++offset;
    }
    gBattleMScanOffset5A8A3C = offset;
    std::strncpy(destination, gBattleMText5AAA7C.data() + offset, static_cast<std::uint32_t>(3));
    destination[3] = '\0';
    gBattleMScanOffset5A8A3C += 4;
    return 0;
}

// 原函数：0x00438F70。
std::int32_t TakeBattleMInteger_438F70(YobiAction* const action)
{
    auto* const payload = action->payload.battleScene;
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const std::int32_t offset = payload->packetOffset64;
        const auto value = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(world::detail::gBattleMText5AAA7C[offset]));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            const std::int32_t next = offset + 1;
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            payload->packetOffset64 = next;
        } else if (hasDigit || value == '|') {
            ++payload->packetOffset64;
            break;
        } else {
            payload->packetOffset64 = offset + 1;
        }
    }
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x00438FE0。
void TakeBattleMString_438FE0(YobiAction* const action, char* const destination, const std::int32_t capacity)
{
    auto* const payload = action->payload.battleScene;
    std::int32_t written = 0;
    for (;;) {
        const std::int32_t offset = payload->packetOffset64;
        const char byte = world::detail::gBattleMText5AAA7C[offset];
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(byte));
        if (byte == '\0') {
            break;
        }
        if (byte == '|') {
            ++payload->packetOffset64;
            break;
        }
        if (written >= capacity - 1) {
            if (value < 0) {
                payload->packetOffset64 = offset + 1;
            }
            ++payload->packetOffset64;
            continue;
        }
        const bool hasTrail = value < 0;
        destination[written] = byte;
        if (hasTrail) {
            ++written;
            if (written >= capacity - 1) {
                // 原439034..43903C用同一局部游标连续发布两次。
                const std::int32_t next = payload->packetOffset64 + 1;
                payload->packetOffset64 = next;
                payload->packetOffset64 = next + 1;
                continue;
            }
            const std::int32_t next = payload->packetOffset64 + 1;
            payload->packetOffset64 = next;
            destination[written] = world::detail::gBattleMText5AAA7C[next];
        }
        const std::int32_t currentOffset = payload->packetOffset64;
        ++written;
        payload->packetOffset64 = currentOffset + 1;
    }
    destination[written] = '\0';
    input::UnescapeLegacyTextAndNotify_52D2F0(destination);
}

// 原函数：0x00439600。
std::int32_t TakeBattleMPreviewInteger_439600()
{
    using namespace world::detail;
    std::int32_t offset = gBattleMPreviewOffset5AAA60;
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gBattleMText5AAA7C[offset]));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            ++offset;
        } else if (hasDigit || value == '|') {
            gBattleMPreviewOffset5AAA60 = offset;
            ++offset;
            break;
        } else {
            ++offset;
        }
    }
    gBattleMPreviewOffset5AAA60 = offset;
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x00439670。
void TakeBattleMPreviewString_439670(char* const destination, const std::int32_t capacity)
{
    using namespace world::detail;
    std::int32_t written = 0;
    for (;;) {
        const char byte = gBattleMText5AAA7C[gBattleMPreviewOffset5AAA60];
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(byte));
        if (byte == '\0') {
            break;
        }
        if (byte == '|') {
            ++gBattleMPreviewOffset5AAA60;
            break;
        }
        if (written >= capacity - 1) {
            if (value < 0) {
                ++gBattleMPreviewOffset5AAA60;
            }
            ++gBattleMPreviewOffset5AAA60;
            continue;
        }
        const bool hasTrail = value < 0;
        destination[written] = byte;
        if (hasTrail) {
            const std::int32_t offset = gBattleMPreviewOffset5AAA60;
            ++written;
            if (written >= capacity - 1) {
                gBattleMPreviewOffset5AAA60 = offset + 1;
                ++gBattleMPreviewOffset5AAA60;
                continue;
            }
            // 原4396AA先读取第二字节，再发布游标；不同于显式游标版本。
            const char nextByte = gBattleMText5AAA7C[offset + 1];
            gBattleMPreviewOffset5AAA60 = offset + 1;
            destination[written] = nextByte;
        }
        const std::int32_t currentOffset = gBattleMPreviewOffset5AAA60;
        ++written;
        gBattleMPreviewOffset5AAA60 = currentOffset + 1;
    }
    destination[written] = '\0';
    input::UnescapeLegacyTextAndNotify_52D2F0(destination);
}

// 原函数：0x00439700。
std::int32_t GetBattleMPreviewHeader_439700(char* const destination)
{
    using namespace world::detail;
    std::int32_t offset = gBattleMPreviewOffset5AAA60;
    if (gBattleMText5AAA7C[offset] == '\0' || gBattleMText5AAA7C[offset + 1] == '\0') {
        return 1;
    }
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gBattleMText5AAA7C[offset]));
        if (value == 0) {
            gBattleMPreviewOffset5AAA60 = offset;
            return 1;
        }
        if (value >= 'A' && value <= 'Z') {
            break;
        }
        ++offset;
    }
    gBattleMPreviewOffset5AAA60 = offset;
    std::strncpy(destination, gBattleMText5AAA7C.data() + offset, static_cast<std::uint32_t>(3));
    destination[3] = '\0';
    gBattleMPreviewOffset5AAA60 += 4;
    return 0;
}

// 原函数：0x0043B940。
std::int32_t TakeBattleMIntegerAt_43B940(std::int32_t* const offset)
{
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const std::int32_t position = *offset;
        const auto value = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(world::detail::gBattleMText5AAA7C[position]));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            const std::int32_t next = position + 1;
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            *offset = next;
        } else if (hasDigit || value == '|') {
            ++*offset;
            break;
        } else {
            *offset = position + 1;
        }
    }
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x0043B9B0。
std::int32_t FindBattleMTarget_43B9B0(
    YobiAction* const scene, const std::int32_t position, std::int32_t skip)
{
    const auto* const payload = static_cast<const BattleScenePayload*>(scene->payload.battleScene);
    std::int32_t offset = payload->packetOffset64;
    while (skip > 0) {
        static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        --skip;
    }
    for (;;) {
        const std::int32_t first = GetBattlePosition_4237E0(TakeBattleMIntegerAt_43B940(&offset));
        if (first == 255) {
            return -1;
        }
        const std::int32_t second = GetBattlePosition_4237E0(TakeBattleMIntegerAt_43B940(&offset));
        const std::uint32_t flags = static_cast<std::uint32_t>(TakeBattleMIntegerAt_43B940(&offset));
        if ((flags & 0x1010u) == 0) {
            return -1;
        }
        if (first == position) {
            return second;
        }
        static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        if ((flags & 0x800000u) != 0) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
            for (std::int32_t index = 0; index < 2; ++index) {
                static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
                static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
                static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
            }
        }
    }
}

// 原函数：0x0043B8C0。
void TakeBattleMStringAt_43B8C0(
    std::int32_t* const offset, char* const destination, const std::int32_t capacity)
{
    std::int32_t written = 0;
    for (;;) {
        const std::int32_t position = *offset;
        const char byte = world::detail::gBattleMText5AAA7C[position];
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(byte));
        if (byte == '\0') {
            break;
        }
        if (byte == '|') {
            ++*offset;
            break;
        }
        if (written >= capacity - 1) {
            if (value < 0) {
                *offset = position + 1;
            }
            ++*offset;
            continue;
        }
        const bool hasTrail = value < 0;
        destination[written] = byte;
        if (hasTrail) {
            ++written;
            if (written >= capacity - 1) {
                const std::int32_t next = *offset + 1;
                *offset = next;
                *offset = next + 1;
                continue;
            }
            const std::int32_t next = *offset + 1;
            *offset = next;
            destination[written] = world::detail::gBattleMText5AAA7C[next];
        }
        const std::int32_t currentOffset = *offset;
        ++written;
        *offset = currentOffset + 1;
    }
    destination[written] = '\0';
    input::UnescapeLegacyTextAndNotify_52D2F0(destination);
}

// 原函数：0x00439770。
void ScanBattleMRideParticipants_439770(std::uint8_t* const participants)
{
    using world::detail::gBattleMPreviewOffset5AAA60;
    std::array<char, 4> header;
    std::array<char, 32> text;
    for (;;) {
        const std::int32_t savedOffset = gBattleMPreviewOffset5AAA60;
        static_cast<void>(GetBattleMPreviewHeader_439700(header.data()));
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind != 22) {
            gBattleMPreviewOffset5AAA60 = savedOffset;
            return;
        }
        std::int32_t offset = gBattleMPreviewOffset5AAA60;
        if (TakeBattleMIntegerAt_43B940(&offset) != 0) {
            gBattleMPreviewOffset5AAA60 = savedOffset;
            return;
        }
        const std::int32_t first = TakeBattleMIntegerAt_43B940(&offset);
        const std::int32_t second = TakeBattleMIntegerAt_43B940(&offset);
        static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        TakeBattleMStringAt_43B8C0(&offset, text.data(), 29);
        static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        for (std::int32_t i = 0; i < 2; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        }
        for (std::int32_t i = 0; i < 5; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        }
        const auto firstFlags = static_cast<std::uint8_t>(TakeBattleMIntegerAt_43B940(&offset));
        TakeBattleMStringAt_43B8C0(&offset, text.data(), 29);
        for (std::int32_t i = 0; i < 6; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        }
        const auto secondFlags = static_cast<std::uint8_t>(TakeBattleMIntegerAt_43B940(&offset));
        if ((firstFlags & 2u) == 0 && (secondFlags & 2u) == 0) {
            gBattleMPreviewOffset5AAA60 = savedOffset;
            return;
        }
        ++participants[first];
        ++participants[second];
        gBattleMPreviewOffset5AAA60 = offset;
    }
}

// 原函数：0x0043BF30。
void TakeBattleMAuxGraphics_43BF30(
    YobiAction* const action, const std::int32_t mode,
    std::int32_t* const firstImage, std::int32_t* const secondImage,
    std::int32_t* const firstImages, std::int32_t* const secondImages, std::int32_t* const values)
{
    if (mode == 1) {
        static_cast<void>(TakeBattleMPreviewInteger_439600());
        static_cast<void>(TakeBattleMPreviewInteger_439600());
        for (std::int32_t index = 0; index < 2; ++index) {
            static_cast<void>(TakeBattleMPreviewInteger_439600());
            static_cast<void>(TakeBattleMPreviewInteger_439600());
            static_cast<void>(TakeBattleMPreviewInteger_439600());
        }
    } else if (mode == 0) {
        *firstImage = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(action));
        *secondImage = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(action));
        for (std::int32_t index = 0; index < 2; ++index) {
            firstImages[index] = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(action));
            secondImages[index] = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(action));
            values[index] = TakeBattleMInteger_438F70(action);
        }
    }
}

// 原函数：0x00439920。
std::int32_t ScanBattleMAttackParticipants_439920(
    const std::int32_t skill, std::uint8_t* const participants)
{
    const std::int32_t first = GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
    if (first == 255) {
        return first;
    }
    ++participants[first];
    ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
    std::int32_t flags = TakeBattleMPreviewInteger_439600();
    static_cast<void>(TakeBattleMPreviewInteger_439600());
    std::int32_t currentSkill = skill;
    if (currentSkill == 81) {
        static_cast<void>(TakeBattleMPreviewInteger_439600());
    }
    if ((flags & 0x800000) != 0) {
        TakeBattleMAuxGraphics_43BF30(nullptr, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
    }
    for (;;) {
        std::int32_t extraPosition = -1;
        std::int32_t extraFlags = 0;
        if (currentSkill == 260 && (flags & 0x80000) != 0) {
            extraPosition = GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
            if (extraPosition != 255) {
                ++participants[extraPosition];
                extraFlags = TakeBattleMPreviewInteger_439600();
                static_cast<void>(TakeBattleMPreviewInteger_439600());
                if ((extraFlags & 0x800000) != 0) {
                    TakeBattleMAuxGraphics_43BF30(nullptr, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
                }
            }
        }
        if ((flags & 0x200) != 0) {
            ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
        }
        if (currentSkill == 260) {
            if ((flags & 0x80000) != 0 && (extraFlags & 0x200) != 0) {
                ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
            }
        } else if (currentSkill == 94) {
            static_cast<void>(TakeBattleMPreviewInteger_439600());
        } else if ((currentSkill == 72 || currentSkill == 2017) && (flags & 1) != 0) {
            extraPosition = GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
            ++participants[extraPosition];
            const std::int32_t extraAttackFlags = TakeBattleMPreviewInteger_439600();
            static_cast<void>(TakeBattleMPreviewInteger_439600());
            if ((extraAttackFlags & 0x800000) != 0) {
                TakeBattleMAuxGraphics_43BF30(nullptr, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
            }
            if ((extraAttackFlags & 0x200) != 0) {
                ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
            }
        }
        if (extraPosition == 255) {
            return first;
        }
        const std::int32_t next = GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
        if (next == 255) {
            return first;
        }
        ++participants[next];
        ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
        flags = TakeBattleMPreviewInteger_439600();
        static_cast<void>(TakeBattleMPreviewInteger_439600());
        if ((flags & 0x800000) != 0) {
            TakeBattleMAuxGraphics_43BF30(nullptr, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
        }
        currentSkill = -1;
    }
}

// 原函数：0x00439B30。
void ScanBattleMParticipants_439B30(
    const std::int32_t kind, std::uint8_t* const participants,
    std::int32_t* const source, std::int32_t* const flag)
{
    using namespace world::detail;
    std::int32_t position = -1;
    *flag = 0;
    // 原439B55..439B63：清零22槽参与计数。
    std::memset(participants, 0, 22);
    std::array<char, 32> text;
    std::array<char, 256> message;
    const auto skip = [](const std::int32_t count) {
        for (std::int32_t i = 0; i < count; ++i) {
            static_cast<void>(TakeBattleMPreviewInteger_439600());
        }
    };
    const auto readPosition = [] {
        return GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
    };
    const auto readSource = [&] {
        position = readPosition();
        ++participants[position];
    };
    const auto scanTargets = [&](const std::int32_t values, const bool auxiliaryGraphics) {
        for (;;) {
            const std::int32_t target = readPosition();
            if (target == 255) {
                return;
            }
            ++participants[target];
            if (values != 0) {
                const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
                skip(values - 1);
                if (auxiliaryGraphics && (targetFlags & 0x800000) != 0) {
                    TakeBattleMAuxGraphics_43BF30(nullptr, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
                }
            }
        }
    };
    switch (kind) {
    case 0: { // SKL
        const std::int32_t technique = TakeBattleMPreviewInteger_439600();
        skip(1);
        std::int32_t skill = TakeBattleMPreviewInteger_439600();
        skip(2);
        if (technique < 205000) {
            skill = technique / 100;
        }
        const BattlePreviewSkillLayout layout = GetBattlePreviewSkillLayout(skill);
        bool followRide = false;
        switch (layout) {
        case BattlePreviewSkillLayout::Attack:
        case BattlePreviewSkillLayout::Mode73: {
            const std::int32_t mode = TakeBattleMPreviewInteger_439600();
            if (mode == 0 || skill == 72 || skill == 2017 || skill == 113) {
                position = ScanBattleMAttackParticipants_439920(skill, participants);
            } else if (mode > 0 && mode <= 4) {
                readSource();
                for (;;) {
                    const std::int32_t target = readPosition();
                    if (target == 255) {
                        break;
                    }
                    std::uint8_t targetCount = participants[target];
                    ++targetCount;
                    const bool sourceTarget = layout == BattlePreviewSkillLayout::Attack
                        && target == position;
                    participants[target] = targetCount;
                    const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
                    skip(1);
                    if ((targetFlags & 0x800000) != 0) {
                        TakeBattleMAuxGraphics_43BF30(nullptr, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
                    }
                    if (sourceTarget && (targetFlags & 0x200) != 0) {
                        ++participants[readPosition()];
                    }
                }
            }
            followRide = true;
            break;
        }
        case BattlePreviewSkillLayout::Damage:
        case BattlePreviewSkillLayout::ValuesRide:
        case BattlePreviewSkillLayout::PairRide:
        case BattlePreviewSkillLayout::ValuesOnly:
        case BattlePreviewSkillLayout::PairOnly:
            skip(1);
            readSource();
            if (layout == BattlePreviewSkillLayout::Damage) {
                scanTargets(2, true);
            } else if (layout == BattlePreviewSkillLayout::PairRide
                || layout == BattlePreviewSkillLayout::PairOnly || skill == 68) {
                scanTargets(2, false);
            } else {
                scanTargets(1, false);
            }
            followRide = layout == BattlePreviewSkillLayout::Damage
                || layout == BattlePreviewSkillLayout::ValuesRide
                || layout == BattlePreviewSkillLayout::PairRide;
            break;
        case BattlePreviewSkillLayout::SourceValue:
            skip(1);
            readSource();
            skip(1);
            break;
        case BattlePreviewSkillLayout::NamedTargets:
            skip(1);
            readSource();
            for (;;) {
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                ++participants[target];
                TakeBattleMPreviewString_439670(text.data(), 29);
                skip(7);
            }
            break;
        case BattlePreviewSkillLayout::Mode98: {
            skip(1);
            const std::int32_t mode = TakeBattleMPreviewInteger_439600();
            readSource();
            followRide = true;
            switch (mode) {
            case 0: case 1: case 2: case 3: case 4: case 5:
            case 9: case 10: case 11: case 18:
                scanTargets(mode == 11 ? 2 : 1, false);
                break;
            case 6: case 8:
                scanTargets(0, false);
                break;
            case 7:
                for (;;) {
                    const std::int32_t target = readPosition();
                    if (target == 255) {
                        break;
                    }
                    ++participants[target];
                    ++participants[GetBattlePetPosition_405B40(target)];
                }
                break;
            case 12: case 13:
                position = readPosition();
                if (position == 255) {
                    followRide = false;
                    break;
                }
                ++participants[position];
                scanTargets(2, mode == 12);
                break;
            case 14:
                for (;;) {
                    skip(1);
                    position = TakeBattleMPreviewInteger_439600();
                    if (position == 255) {
                        break;
                    }
                    ++participants[position];
                    skip(1);
                    const std::int32_t target = readPosition();
                    if (target == 255) {
                        break;
                    }
                    ++participants[target];
                }
                break;
            case 15: case 16:
                scanTargets(2, false);
                break;
            case 17:
                scanTargets(1, false);
                break;
            default:
                break;
            }
            break;
        }
        case BattlePreviewSkillLayout::OptionalPair: {
            readSource();
            const std::int32_t target = readPosition();
            skip(2);
            if (target != -1) {
                ++participants[target];
            }
            break;
        }
        case BattlePreviewSkillLayout::DirectValues:
            readSource();
            scanTargets(1, false);
            break;
        case BattlePreviewSkillLayout::Team: {
            skip(1);
            readSource();
            const std::int32_t first = TakeBattleMPreviewInteger_439600() == 0 ? 0 : 10;
            for (std::int32_t index = first; index < first + 10; ++index) {
                ++participants[index];
            }
            break;
        }
        case BattlePreviewSkillLayout::DirectRecords:
            readSource();
            for (;;) {
                skip(1);
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                ++participants[target];
                skip(7);
            }
            break;
        case BattlePreviewSkillLayout::RideRecords:
            skip(1);
            readSource();
            for (;;) {
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                if (target == position) {
                    ++participants[readPosition()];
                    skip(1);
                    TakeBattleMPreviewString_439670(text.data(), 29);
                    skip(8);
                    TakeBattleMPreviewString_439670(text.data(), 29);
                    skip(7);
                } else {
                    ++participants[target];
                    const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
                    skip(1);
                    if ((targetFlags & 0x800000) != 0) {
                        TakeBattleMAuxGraphics_43BF30(nullptr, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
                    }
                }
            }
            followRide = true;
            break;
        case BattlePreviewSkillLayout::EffectRecords:
            skip(1);
            readSource();
            for (;;) {
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                ++participants[target];
                const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
                skip(1);
                auto& pair = gBattleEffectPairs5A8A40[target];
                const bool namedEffect = skill == 2002;
                pair[0] = 0;
                pair[1] = 0;
                if (namedEffect) {
                    pair[0] = TakeBattleMPreviewInteger_439600();
                    pair[1] = TakeBattleMPreviewInteger_439600();
                    TakeBattleMPreviewString_439670(text.data(), 29);
                    const std::int32_t value = TakeBattleMPreviewInteger_439600();
                    for (auto& record : gBattleNamedEffects5ABCE0) {
                        if (record.name[0] == '\0') {
                            std::strcpy(record.name.data(), text.data());
                            record.value = value;
                            break;
                        }
                        if (std::strcmp(text.data(), record.name.data()) == 0) {
                            record.value = value;
                            break;
                        }
                    }
                }
                if ((targetFlags & 0x800000) != 0) {
                    TakeBattleMAuxGraphics_43BF30(nullptr, 1, nullptr, nullptr, nullptr, nullptr, nullptr);
                }
            }
            followRide = true;
            break;
        case BattlePreviewSkillLayout::None:
            break;
        }
        if (followRide) {
            ScanBattleMRideParticipants_439770(participants);
        }
        *flag = 1;
        *source = position;
        return;
    }
    case 1: case 20: // ESC/BST
        readSource();
        skip(1);
        *flag = 1;
        break;
    case 2: case 3: // CLR/SUP
        for (;;) {
            position = readPosition();
            if (position == 255) {
                break;
            }
            ++participants[position];
        }
        *flag = 1;
        break;
    case 4: { // POS
        position = readPosition();
        const std::int32_t pet = GetBattlePetPosition_405B40(position);
        ++participants[position];
        ++participants[pet];
        *flag = 1;
        break;
    }
    case 5: // MON
        readSource();
        ++participants[GetBattlePetPosition_405B40(position)];
        if (TakeBattleMPreviewInteger_439600() != 0) {
            skip(1);
            TakeBattleMPreviewString_439670(text.data(), 29);
            skip(6);
        }
        skip(1);
        *flag = 1;
        break;
    case 6: // EQU
        readSource();
        skip(2);
        *flag = 1;
        break;
    case 7: { // ITM
        const std::int32_t mode = TakeBattleMPreviewInteger_439600();
        if (mode == 0) {
            readSource();
            for (;;) {
                skip(1);
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                ++participants[target];
            }
        } else if (mode == 6) {
            readSource();
            ++participants[readPosition()];
            skip(1);
        } else if (mode == 7) {
            readSource();
        }
        *flag = 1;
        break;
    }
    case 8: // CMB，输出循环终止位置FF，不是首参与者。
        ++participants[readPosition()];
        scanTargets(2, true);
        ScanBattleMRideParticipants_439770(participants);
        position = 255;
        *flag = 1;
        break;
    case 9: // PSN
        skip(2);
        readSource();
        skip(2);
        ScanBattleMRideParticipants_439770(participants);
        *flag = 1;
        break;
    case 10: case 12: // ANR/ACT
        readSource();
        *flag = 0;
        break;
    case 11: // PUE
        readSource();
        skip(2);
        *flag = 0;
        break;
    case 13: // CFP
        readSource();
        skip(1);
        *flag = 0;
        break;
    case 14: // CHT
        position = readPosition();
        if (static_cast<std::uint32_t>(position) <= 21u) {
            ++participants[position];
        }
        skip(3);
        TakeBattleMPreviewString_439670(message.data(), 256);
        *flag = 0;
        break;
    case 15: case 16: case 18: // PNO/PLV/PDN
        readSource();
        *flag = 1;
        break;
    case 17: // CBC
        readSource();
        skip(7);
        *flag = 0;
        break;
    case 19: { // REB
        readSource();
        const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
        skip(1);
        if ((targetFlags & 0x80000) != 0) {
            skip(1);
        }
        *flag = 1;
        break;
    }
    case 21: // GMN，首位置只参与计数，gate0仍输出-1。
        ++participants[readPosition()];
        if (TakeBattleMPreviewInteger_439600() != 0) {
            position = readPosition();
            skip(2);
            TakeBattleMPreviewString_439670(text.data(), 29);
            skip(7);
        }
        for (std::int32_t index = 0; index < 22; ++index) {
            ++participants[index];
        }
        *flag = 1;
        break;
    case 22: { // RID，此处零模式19整数，与439770试读20整数不同。
        const std::int32_t mode = TakeBattleMPreviewInteger_439600();
        position = readPosition();
        const std::int32_t target = readPosition();
        ++participants[position];
        ++participants[target];
        const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
        if (mode != 0) {
            if ((targetFlags & 0x80000) != 0) {
                TakeBattleMPreviewString_439670(text.data(), 29);
                skip(8);
            }
        } else {
            TakeBattleMPreviewString_439670(text.data(), 29);
            skip(8);
            TakeBattleMPreviewString_439670(text.data(), 29);
            skip(7);
        }
        *flag = 1;
        break;
    }
    case 23: // DSK
        skip(3);
        [[fallthrough]];
    case 24: // END
        for (std::int32_t index = 0; index < 22; ++index) {
            ++participants[index];
        }
        *flag = 0;
        break;
    default:
        break;
    }
    *source = position;
}

// 原函数：0x0043B290。
std::int32_t CheckBattleMParticipants_43B290(
    std::int32_t* const source, const std::int32_t previousSource,
    std::uint32_t* const mask, std::int32_t* const flag)
{
    using namespace world::detail;
    gBattleMPreviewOffset5AAA60 = gBattleMScanOffset5A8A3C;
    std::array<char, 4> header;
    std::array<std::uint8_t, 22> participants;
    if (GetBattleMPreviewHeader_439700(header.data()) == 1) {
        return 1;
    }
    std::int32_t kind = 0;
    while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
        && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
        ++kind;
    }
    if (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())) {
        ScanBattleMParticipants_439B30(static_cast<std::int32_t>(kind), participants.data(), source, flag);
        for (std::int32_t index = 0; index < 22; ++index) {
            const std::uint8_t count = participants[index];
            if (count == 0) {
                continue;
            }
            if (!(previousSource == *source && index == previousSource)
                && gBattlePreviewParticipants5AAA48[index] != 0) {
                return 0;
            }
            gBattlePreviewParticipants5AAA48[index] = count;
        }
    }
    // 原未知头路径不初始化局部计数；不补零或新增拒绝分支。
    for (std::int32_t index = 0; index < 22; ++index) {
        if (participants[index] != 0) {
            const std::uint32_t currentMask = *mask;
            const std::uint32_t bit = 1u << index;
            *mask = currentMask | bit;
        }
    }
    return 1;
}

// 原函数：0x0043B860。
std::int32_t PeekBattleMInteger_43B860(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleScenePayload*>(action->payload.battleScene);
    const char* cursor = world::detail::gBattleMText5AAA7C.data() + payload->packetOffset64;
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*cursor));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            ++cursor;
        } else if (hasDigit || value == '|') {
            break;
        } else {
            ++cursor;
        }
    }
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x00423960。
std::int32_t GetBattleMObjectHeader_423960(YobiAction* const action, char* const destination)
{
    using world::detail::gBattleMText5AAA7C;
    auto* const payload = action->payload.battleScene;
    const std::int32_t initialOffset = payload->packetOffset64;
    if (gBattleMText5AAA7C[initialOffset] == '\0' || gBattleMText5AAA7C[initialOffset + 1] == '\0') {
        return 1;
    }
    for (;;) {
        const std::int32_t offset = payload->packetOffset64;
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gBattleMText5AAA7C[offset]));
        if (value == 0) {
            return 1;
        }
        if (value >= 'A' && value <= 'Z') {
            break;
        }
        payload->packetOffset64 = offset + 1;
    }
    std::strncpy(destination, gBattleMText5AAA7C.data() + payload->packetOffset64, static_cast<std::uint32_t>(3));
    destination[3] = '\0';
    payload->packetOffset64 += 4;
    return 0;
}

namespace {

// 427750..42780B；两局部量在98之后、A4之前读取，不发布为另一组全局。
[[maybe_unused]] void RestoreBattleMExecutionState(
    const BattleScenePayload* const payload, std::uint32_t& flags9C, std::int32_t& valueA0)
{
    gBattleMCasterPosition5AAA64 = payload->casterPosition6C;
    gBattleMValue5ABA7C = payload->value70;
    gBattleMDefenderPosition5A4A24 = payload->defenderPosition74;
    gBattleMTargetPosition5AAA40 = payload->targetPosition78;
    gBattleMSkill5AAA78 = payload->skill7C;
    gBattleMTechnique5A4A38 = payload->technique80;
    gBattleMValue59FA10 = payload->value84;
    gBattleMValue5A4A34 = payload->value88;
    gBattleMValue5ABA90 = payload->value8C;
    gBattleMResultFlags5A4A20 = payload->resultFlags90;
    gBattleMFlags5ABA84 = payload->flags94;
    gBattleMValue5A4A28 = payload->value98;
    flags9C = payload->flags9C;
    valueA0 = payload->valueA0;
    gBattleMMessageValue59FA14 = payload->messageValueA4;
}

// 437A29..437ABA；即使消息已推进或对象已标死，活动路径仍回存13项，不写9C/A0。
[[maybe_unused]] void SaveBattleMExecutionState(BattleScenePayload* const payload)
{
    payload->casterPosition6C = gBattleMCasterPosition5AAA64;
    payload->value70 = gBattleMValue5ABA7C;
    payload->defenderPosition74 = gBattleMDefenderPosition5A4A24;
    payload->targetPosition78 = gBattleMTargetPosition5AAA40;
    payload->skill7C = gBattleMSkill5AAA78;
    payload->technique80 = gBattleMTechnique5A4A38;
    payload->value84 = gBattleMValue59FA10;
    payload->value88 = gBattleMValue5A4A34;
    payload->value8C = gBattleMValue5ABA90;
    payload->resultFlags90 = gBattleMResultFlags5A4A20;
    payload->flags94 = gBattleMFlags5ABA84;
    payload->value98 = gBattleMValue5A4A28;
    payload->messageValueA4 = gBattleMMessageValue59FA14;
}

// 427500..427732失活位路径；由427470入口的PrepareBattleMExecution调用，不单独绑定回调。
[[maybe_unused]] void DrainInactiveBattleMEvent(YobiAction* const scene, BattleScenePayload* const payload)
{
    using namespace world::detail;
    std::array<char, 256> initialMessage;
    std::array<char, 256> queuedMessage;
    std::array<char, 4> header;
    if (scene->phase == 14 && payload->packetSubState58 == 0) {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const std::int32_t color = TakeBattleMInteger_438F70(scene);
        const std::int32_t font = TakeBattleMInteger_438F70(scene);
        payload->messageValueA4 = TakeBattleMInteger_438F70(scene);
        TakeBattleMString_438FE0(scene, initialMessage.data(), 256);
        if (platform::gValue554644 != 0) {
            static_cast<void>(audio::PlaySe_52C450(74, 320));
        }
        Sub_44B050(cg::text::FromGbkWide(initialMessage.data()), color, font);
    }
    if (scene->phase == 20 && payload->packetSubState58 == 0) {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const std::int32_t seconds = TakeBattleMInteger_438F70(scene);
        if (gBattleMCasterPosition5AAA64 == gBattleCurrentPosition5ABCA4) {
            ConsumeBattlePlayerTimer(seconds);
        }
    }
    --payload->remainingMessages5C;
    while (payload->remainingMessages5C > 0 && GetBattleMObjectHeader_423960(scene, header.data()) == 0) {
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind == static_cast<std::int32_t>(kBattleMHeaders553E94.size())) {
            continue;
        }
        const std::int32_t remaining = payload->remainingMessages5C - 1;
        const bool isChat = kind == 14;
        payload->remainingMessages5C = remaining;
        if (isChat) {
            gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            const std::int32_t color = TakeBattleMInteger_438F70(scene);
            const std::int32_t font = TakeBattleMInteger_438F70(scene);
            payload->messageValueA4 = TakeBattleMInteger_438F70(scene);
            TakeBattleMString_438FE0(scene, queuedMessage.data(), 256);
            if (platform::gValue554644 != 0) {
                static_cast<void>(audio::PlaySe_52C450(74, 320));
            }
            Sub_44B050(cg::text::FromGbkWide(queuedMessage.data()), color, font);
        } else if (kind == 20) {
            gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            const std::int32_t seconds = TakeBattleMInteger_438F70(scene);
            if (gBattleMCasterPosition5AAA64 == gBattleCurrentPosition5ABCA4) {
                ConsumeBattlePlayerTimer(seconds);
            }
        }
    }
    FinishBattleEvent_43B6C0(scene);
}

// 4274DF..427500/427732..42780B；false时父入口直接返回，不执行活动路径的公共回存。
[[maybe_unused]] bool PrepareBattleMExecution(
    YobiAction* const scene, BattleScenePayload* const payload,
    std::uint32_t& flags9C, std::int32_t& valueA0)
{
    const std::uint32_t eventBit = 1u << payload->eventSlot60;
    if ((world::detail::gBattleActiveEventMask5AAA74 & eventBit) == 0) {
        DrainInactiveBattleMEvent(scene, payload);
        return false;
    }
    const std::int32_t delay = payload->groupBase68;
    if (delay >= 0) {
        const std::int32_t nextDelay = delay - 1;
        const bool groupDelayFinished = nextDelay == 0;
        payload->groupBase68 = nextDelay;
        if (groupDelayFinished) {
            --gBattlePendingGroupCount5ABB24;
        }
        return false;
    }
    RestoreBattleMExecutionState(payload, flags9C, valueA0);
    return true;
}

// 427470的六类完整消息段；入口载荷固定，返回后仍须由总入口执行437A29回存。
[[maybe_unused]] void ProcessBattleMTransitionHeader(
    YobiAction* const scene, BattleScenePayload* const payload, const BattleMTransitionHeader kind)
{
    using namespace world::detail;
    const std::int32_t subState = payload->packetSubState58;
    if (subState != 0) {
        if (subState == 1
            && CheckBattleTurnReady_4394A0(scene, kind == BattleMTransitionHeader::Clear ? 1 : 0) == 1) {
            if (kind == BattleMTransitionHeader::PetNotice || kind == BattleMTransitionHeader::PetLeave) {
                Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            }
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }

    switch (kind) {
    case BattleMTransitionHeader::Escape: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        gBattleMResultFlags5A4A20 = resultFlags;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[casterPosition], 68);
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22320;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 12);
        const std::uint32_t result = gBattleMResultFlags5A4A20;
        if (result == 0) {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 50);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 37);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
        } else if (result == 1 || result == 2) {
            if (result == 2) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 213);
            }
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1018 = 211;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 52);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 13);
            const std::int32_t currentPosition = gBattleMCasterPosition5AAA64;
            const bool playerPosition = gBattleCurrentPosition5ABCA4 == currentPosition;
            const std::int32_t castState = playerPosition ? 256 : 51;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[currentPosition], castState);
        }
        MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    }
    case BattleMTransitionHeader::Clear:
    case BattleMTransitionHeader::Support:
        for (;;) {
            const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            const bool endPosition = position == 255;
            gBattleMCasterPosition5AAA64 = position;
            if (endPosition) {
                break;
            }
            YobiAction* const actor = gBattleAllEntries59E980[position];
            if (actor == nullptr) {
                continue;
            }
            AppendBattleCast_4392A0(scene, actor, 68);
            if (kind == BattleMTransitionHeader::Clear) {
                gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->reactionImage1010 = 22320;
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 12);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 13);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
            } else {
                gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->reactionImage1010 = 22323;
                gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->value1018 = 206;
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
                YobiAction* const current = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
                if ((*reinterpret_cast<const unsigned char*>(
                        &current->payload.battleActor->flagsFDC) & 2u) == 0) {
                    AppendBattleCast_4392A0(scene, current, 57);
                    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
                }
            }
            MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        }
        break;
    case BattleMTransitionHeader::PetDown: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        actor->payload.battleActor->value1018 = 205;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 84);
        MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    }
    case BattleMTransitionHeader::PetNotice: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        actor->payload.battleActor->reactionImage1010 = 22334;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 65);
        MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    }
    case BattleMTransitionHeader::PetLeave: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        static_cast<void>(platform::SeedRandom_53B4BF(static_cast<std::uint32_t>(position)));
        const std::int32_t xIndex = platform::RandomRange_46FAF0(0, 19);
        // 原X先借角色、读坐标，再借载荷；Y先借角色及载荷，再读坐标。
        const std::int32_t xPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const xActor = gBattleAllEntries59E980[xPosition];
        const std::int32_t x = gBattleGraphicPoints553758[xIndex][0];
        xActor->payload.battleActor->x08 = x;
        const std::int32_t yIndex = platform::RandomRange_46FAF0(0, 19);
        const std::int32_t yPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const yActor = gBattleAllEntries59E980[yPosition];
        auto* const yPayload = yActor->payload.battleActor;
        const std::int32_t y = gBattleGraphicPoints553758[yIndex][1];
        yPayload->y0C = y;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 12);
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22324;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->value1018 = 211;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 13);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
        MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    }
    }
    ++payload->packetSubState58;
}

// 六类活动数据消息；由427470总入口分派，父入口负责恢复及回存，不单独绑定回调。
[[maybe_unused]] void ProcessBattleMDataHeader(
    YobiAction* const scene, BattleScenePayload* const payload, const BattleMDataHeader kind)
{
    using namespace world::detail;
    switch (kind) {
    case BattleMDataHeader::Action: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor != nullptr) {
            static_cast<void>(NewBattleAttachedEffect_41FBE0(actor, 2));
        }
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    case BattleMDataHeader::ConsumeFp: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (actorPresent) {
            const std::int32_t amount = TakeBattleMInteger_438F70(scene);
            (gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barValue1B0 -= static_cast<std::int32_t>(amount));
            if ((gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barValue1B0 < 0)) {
                gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barValue1B0 = std::int32_t{0};
            }
        }
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    case BattleMDataHeader::Chat: {
        const std::int32_t subState = payload->packetSubState58;
        if (subState == 0) {
            gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            const std::int32_t color = TakeBattleMInteger_438F70(scene);
            const std::int32_t font = TakeBattleMInteger_438F70(scene);
            const std::int32_t duration = TakeBattleMInteger_438F70(scene);
            payload->messageValueA4 = duration;
            gBattleMMessageValue59FA14 = duration;
            std::array<char, 256> message;
            TakeBattleMString_438FE0(scene, message.data(), 256);
            if (platform::gValue554644 != 0) {
                static_cast<void>(audio::PlaySe_52C450(74, 320));
            }
            Sub_44B050(cg::text::FromGbkWide(message.data()), color, font);
            const std::int32_t position = gBattleMCasterPosition5AAA64;
            if (position < 20) {
                YobiAction* const actor = gBattleAllEntries59E980[position];
                if (actor != nullptr) {
                    static_cast<void>(render::LoadGraphic_522570(actor->graphicIndex));
                    const std::int32_t currentPosition = gBattleMCasterPosition5AAA64;
                    const std::int32_t lifetime = gBattleMMessageValue59FA14;
                    YobiAction* const current = gBattleAllEntries59E980[currentPosition];
                    const std::int32_t offsetY = -render::ReadGraphicDxHeightFromField(current->graphicIndex);
                    static_cast<void>(NewBattleHitEffect_41F5B0(current, -30, offsetY, lifetime, 22335));
                }
            }
            ++payload->packetSubState58;
        } else if (subState == 1) {
            const std::int32_t remaining = payload->messageValueA4 - 1;
            payload->messageValueA4 = remaining;
            gBattleMMessageValue59FA14 = remaining;
            if (payload->messageValueA4 <= 0) {
                FinishBattleEvent_43B6C0(scene);
            }
        }
        return;
    }
    case BattleMDataHeader::ChangeBody: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::int32_t image = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->imageNo = image;
        {
            const std::int32_t value = TakeBattleMInteger_438F70(scene);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->readiness = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(scene);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->maxHp = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(scene);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barValue1B0 = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(scene);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barMaximum1C0 = static_cast<std::int32_t>(value);
        }
        const std::int32_t riderValue = TakeBattleMInteger_438F70(scene);
        for (std::int32_t index = 0; index < 2; ++index) {
            const std::int32_t gear = TakeBattleMInteger_438F70(scene);
            const std::int32_t firstImage = TakeBattleMInteger_438F70(scene);
            const std::int32_t secondImage = TakeBattleMInteger_438F70(scene);
            SetBattleGear_422C20(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], gear, index);
            SetBattleAuxGear_422C90(
                gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], firstImage, secondImage, index);
        }
        YobiAction* const current = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (current->payload.battleActor->rider13C8 != nullptr && riderValue != 0) {
            static_cast<void>(SetBattleRider_4222B0(current, current->imageNo, riderValue));
        }
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    case BattleMDataHeader::AppendSkill: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t technique = TakeBattleMInteger_438F70(scene);
        const std::uint32_t flags = gBattleMResultFlags5A4A20;
        const bool appendEight = flags == 0x400u;
        gBattleMTechnique5A4A38 = technique;
        if (appendEight) {
            AddBattleAppendEffect_43BE00(8, technique, gBattleMCasterPosition5AAA64);
        } else if (flags == 0x1000u) {
            AddBattleAppendEffect_43BE00(11, technique, gBattleMCasterPosition5AAA64);
        }
        if (CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }
    case BattleMDataHeader::End:
        if (CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            FinishBattleEvent_43B6C0(scene);
            world::gCurrentSceneState = 7;
        }
        return;
    }
}

// 六类装备/状态消息；固定载荷与当前槽分别取样，已由427470调用并回存，完整战斗另验。
[[maybe_unused]] void ProcessBattleMStatusHeader(
    YobiAction* const scene, BattleScenePayload* const payload, const BattleMStatusHeader kind)
{
    using namespace world::detail;
    const std::int32_t subState = payload->packetSubState58;
    if (subState != 0) {
        if ((kind == BattleMStatusHeader::QueuedValue || subState == 1)
            && CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }

    switch (kind) {
    case BattleMStatusHeader::Equipment: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::int32_t image = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image1014 = image;
        const std::int32_t baseImage = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->baseImage13AC = baseImage;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22321;
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->value1018 = 209;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 66);
        break;
    }
    case BattleMStatusHeader::StatusValue: {
        gBattleMStatusMode5ABA88 = TakeBattleMInteger_438F70(scene);
        gBattleMTechnique5A4A38 = TakeBattleMInteger_438F70(scene);
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t value = TakeBattleMInteger_438F70(scene);
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const skillActor = gBattleAllEntries59E980[casterPosition];
        const std::int32_t skillLevel = gBattleMTechnique5A4A38;
        skillActor->payload.battleActor->castSkillLevel1044 = skillLevel;
        switch (gBattleMStatusMode5ABA88) {
        case 0:
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1018 = 273;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 159);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 32);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1030 = value;
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1034 = 6;
            if ((*reinterpret_cast<const unsigned char*>(&gBattleMResultFlags5A4A20) & 0x20u) != 0) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 46);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 47);
            }
            break;
        case 1:
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 131);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 160);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1030 = value;
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1034 = 5;
            break;
        case 2:
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1018 = 274;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 160);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1030 = value;
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1034 = 3;
            break;
        case 3:
        case 4:
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1018 = 273;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
            if (gBattleMStatusMode5ABA88 == 4) {
                gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->image102C = 104216;
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 252);
            }
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 159);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1030 = value;
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1034 = 6;
            if ((*reinterpret_cast<const unsigned char*>(&gBattleMResultFlags5A4A20) & 0x20u) != 0) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 47);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 46);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 38);
            } else {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 36);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BattleMStatusHeader::Debuff: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        auto* const actorPayload = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor;
        const std::int32_t state = TakeBattleMInteger_438F70(scene);
        const std::int32_t type = TakeBattleMInteger_438F70(scene);
        YobiAction* const current = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (current == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        if (actorPayload->debuffState13CC != state || actorPayload->debuffType13D0 != type) {
            if (state == 0) {
                AppendBattleCast_4392A0(scene, current, 142);
                actorPayload->value13DC = -1;
                actorPayload->value13D8 = 0;
            } else if (state == 2 || state == 3) {
                AppendBattleCast_4392A0(scene, current, 142);
                actorPayload->value13DC = type;
                actorPayload->value13D8 = state;
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 141);
            }
        }
        break;
    }
    case BattleMStatusHeader::Power: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        auto* const actorPayload = actor->payload.battleActor;
        const std::int32_t mode = TakeBattleMInteger_438F70(scene) <= 0 ? 1 : 3;
        const std::int32_t type = TakeBattleMInteger_438F70(scene);
        if (actorPayload->value13D8 != mode || actorPayload->value13DC != type) {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 144);
            actorPayload->value13DC = type;
            actorPayload->value13D8 = mode;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 143);
        }
        break;
    }
    case BattleMStatusHeader::Rebirth: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        AppendBattleCast_4392A0(scene, actor, 40);
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t image = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
        if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
            gBattleMTargetPosition5AAA40 = TakeBattleMInteger_438F70(scene);
        }
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectPosition = gBattleMTargetPosition5AAA40;
        const BattleEntryEffectRecord effect {effectPosition, effectFlags, 0, image};
        AppendBattleEffect_439400(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], effect);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 163);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
        break;
    }
    case BattleMStatusHeader::QueuedValue: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(PeekBattleMInteger_43B860(scene));
        YobiAction* const preview = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (preview != nullptr
            && (preview->payload.battleActor->busyFD8 != 0 || preview->phase != 0)) {
            return;
        }
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::int32_t value = TakeBattleMInteger_438F70(scene);
        AppendBattleQueuedTarget_43B7F0(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], value);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 166);
        break;
    }
    }
    MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    if (kind == BattleMStatusHeader::StatusValue) {
        ParseBattleMRide_4239D0(scene);
    }
    ++payload->packetSubState58;
}

// 原427470的POS=4；固定旧槽地址，保留非对称附件重绑及空槽后的子状态递增。
[[maybe_unused]] void ProcessBattleMPositionHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    using namespace world::detail;
    const std::int32_t subState = payload->packetSubState58;
    if (subState != 0) {
        if (subState == 1 && CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            const std::int32_t otherPosition = GetBattlePetPosition_405B40(gBattleMCasterPosition5AAA64);
            Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            YobiAction* const other = gBattleAllEntries59E980[otherPosition];
            if (other != nullptr && other->characterAction != 10 && other->phase != 34) {
                Sub_406260(other);
            }
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }

    const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const bool unavailable = position == 255;
    gBattleMCasterPosition5AAA64 = position;
    if (unavailable) {
        return;
    }
    if (gBattleAllEntries59E980[position] == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        ++payload->packetSubState58;
        return;
    }
    const std::int32_t otherPosition = GetBattlePetPosition_405B40(position);
    const std::int32_t initialPosition = gBattleMCasterPosition5AAA64;
    const bool movePlayer = gBattleCurrentPosition5ABCA4 == initialPosition;
    YobiAction* const moving = gBattleAllEntries59E980[initialPosition];
    YobiAction** const oldSlot = &gBattleAllEntries59E980[initialPosition];
    const std::int32_t oldPosition = initialPosition;
    if (movePlayer) {
        gBattleCurrentPosition5ABCA4 = otherPosition;
    }
    {
        const std::int32_t pointIndex = gBattlePointIndices5536FC[otherPosition];
        auto* const movingPayload = moving->payload.battleActor;
        movingPayload->x00 = gBattleGraphicPoints553758[pointIndex][2];
    }
    {
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t pointIndex = gBattlePointIndices5536FC[otherPosition];
        YobiAction* const actor = gBattleAllEntries59E980[casterPosition];
        auto* const actorPayload = actor->payload.battleActor;
        actorPayload->y04 = gBattleGraphicPoints553758[pointIndex][3];
    }
    {
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t pointIndex = gBattlePointIndices5536FC[otherPosition];
        YobiAction* const actor = gBattleAllEntries59E980[casterPosition];
        auto* const actorPayload = actor->payload.battleActor;
        actorPayload->x08 = gBattleGraphicPoints553758[pointIndex][0];
    }
    {
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t pointIndex = gBattlePointIndices5536FC[otherPosition];
        YobiAction* const actor = gBattleAllEntries59E980[casterPosition];
        auto* const actorPayload = actor->payload.battleActor;
        actorPayload->y0C = gBattleGraphicPoints553758[pointIndex][1];
    }
    gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->battlePosition18 = otherPosition;
    YobiAction* const initialOther = gBattleAllEntries59E980[otherPosition];
    if (initialOther != nullptr) {
        auto* const initialOtherPayload = initialOther->payload.battleActor;
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        initialOtherPayload->battlePosition18 = casterPosition;
        YobiAction* const other = gBattleAllEntries59E980[otherPosition];
        if (other != nullptr && other->phase != 34) {
            {
                const std::int32_t sampledCasterPosition = gBattleMCasterPosition5AAA64;
                auto* const otherPayload = other->payload.battleActor;
                const std::int32_t pointIndex = gBattlePointIndices5536FC[sampledCasterPosition];
                otherPayload->x00 = gBattleGraphicPoints553758[pointIndex][2];
            }
            {
                const std::int32_t sampledCasterPosition = gBattleMCasterPosition5AAA64;
                YobiAction* const actor = gBattleAllEntries59E980[otherPosition];
                const std::int32_t pointIndex = gBattlePointIndices5536FC[sampledCasterPosition];
                auto* const actorPayload = actor->payload.battleActor;
                actorPayload->y04 = gBattleGraphicPoints553758[pointIndex][3];
            }
            {
                const std::int32_t sampledCasterPosition = gBattleMCasterPosition5AAA64;
                YobiAction* const actor = gBattleAllEntries59E980[otherPosition];
                const std::int32_t pointIndex = gBattlePointIndices5536FC[sampledCasterPosition];
                auto* const actorPayload = actor->payload.battleActor;
                actorPayload->x08 = gBattleGraphicPoints553758[pointIndex][0];
            }
            {
                const std::int32_t sampledCasterPosition = gBattleMCasterPosition5AAA64;
                YobiAction* const actor = gBattleAllEntries59E980[otherPosition];
                const std::int32_t pointIndex = gBattlePointIndices5536FC[sampledCasterPosition];
                auto* const actorPayload = actor->payload.battleActor;
                actorPayload->y0C = gBattleGraphicPoints553758[pointIndex][1];
            }
        }
    }
    const std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
    YobiAction* const replacement = gBattleAllEntries59E980[otherPosition];
    gBattleMCasterPosition5AAA64 = otherPosition;
    gBattleAllEntries59E980[sourcePosition] = replacement;
    gBattleAllEntries59E980[otherPosition] = moving;
    if (moving != nullptr) {
        YobiAction* const rider = moving->payload.battleActor->rider13C8;
        if (rider != nullptr) {
            rider->battleAttachmentParent10 = moving;
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->rider13C8->value1D4 = gBattleMCasterPosition5AAA64;
        }
    }
    YobiAction* const oldActor = *oldSlot;
    if (oldActor != nullptr) {
        YobiAction* const rider = oldActor->payload.battleActor->rider13C8;
        if (rider != nullptr) {
            rider->battleAttachmentParent10 = oldActor;
            (*oldSlot)->payload.battleActor->rider13C8->value1D4 = oldPosition;
        }
        // 原4286E8在旧槽非空门禁内重绑新槽两附件，后面仍重复同两次写入。
        for (std::size_t index = 0; index < 2; ++index) {
            YobiAction* const actor = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
            YobiAction* const gear = actor->payload.battleActor->auxiliaryGear13B8[index];
            if (gear != nullptr) {
                gear->battleAttachmentParent10 = actor;
                gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->auxiliaryGear13B8[index]->value1D4 = gBattleMCasterPosition5AAA64;
            }
        }
    }
    YobiAction* const newActor = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
    if (newActor != nullptr) {
        YobiAction* const gear = newActor->payload.battleActor->gear13B4;
        if (gear != nullptr) {
            gear->battleAttachmentParent10 = newActor;
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->gear13B4->value1D4 = gBattleMCasterPosition5AAA64;
        }
        for (std::size_t index = 0; index < 2; ++index) {
            YobiAction* const actor = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
            YobiAction* const auxiliary = actor->payload.battleActor->auxiliaryGear13B8[index];
            if (auxiliary != nullptr) {
                auxiliary->battleAttachmentParent10 = actor;
                gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->auxiliaryGear13B8[index]->value1D4 = gBattleMCasterPosition5AAA64;
            }
        }
    }
    YobiAction* const remaining = *oldSlot;
    if (remaining != nullptr) {
        YobiAction* const gear = remaining->payload.battleActor->gear13B4;
        if (gear != nullptr) {
            gear->battleAttachmentParent10 = remaining;
            (*oldSlot)->payload.battleActor->gear13B4->value1D4 = oldPosition;
        }
    }
    gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->value1018 = 210;
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 11);
    MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->value54 = 3.0F;
    gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->framePlayingRate = 100;
    YobiAction* const other = *oldSlot;
    if (other != nullptr && other->phase != 34) {
        AppendBattleCast_4392A0(scene, other, 11);
        MoveBattleCastState_439380(*oldSlot);
        (*oldSlot)->value54 = 3.0F;
        (*oldSlot)->framePlayingRate = 100;
    }
    ++payload->packetSubState58;
}

// 原427470的MON=5；子状态2沿恢复目标重试，成功解析期间固定目标对象及载荷。
[[maybe_unused]] void ProcessBattleMMonsterHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    using namespace world::detail;
    std::int32_t defenderPosition = gBattleMDefenderPosition5A4A24;
    const std::int32_t subState = payload->packetSubState58;
    if (subState == 1) {
        if (CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            YobiAction* const defender = gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24];
            if (defender == nullptr || defender->payload.battleActor->busyFD8 == 0) {
                Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
                if (gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24] != nullptr) {
                    const std::int32_t petPosition = GetBattlePetPosition_405B40(gBattleMCasterPosition5AAA64);
                    Sub_406260(gBattleAllEntries59E980[petPosition]);
                }
                FinishBattleEvent_43B6C0(scene);
            }
        }
        return;
    }
    if (subState == 0) {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        defenderPosition = GetBattlePetPosition_405B40(position);
        gBattleMDefenderPosition5A4A24 = defenderPosition;
    } else if (subState != 2) {
        return;
    }
    YobiAction* const pending = gBattleAllEntries59E980[defenderPosition];
    if (pending != nullptr && pending->payload.battleActor->busyFD8 == 1) {
        payload->packetSubState58 = 2;
        return;
    }
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 41);
    const std::int32_t targetCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const targetCaster = gBattleAllEntries59E980[targetCasterPosition];
    const std::int32_t targetPosition = gBattleMDefenderPosition5A4A24;
    targetCaster->payload.battleActor->targetPosition100C = targetPosition;
    gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
    const std::int32_t value = TakeBattleMInteger_438F70(scene);
    std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
    const bool playerEffect = gBattleCurrentPosition5ABCA4 == effectCasterPosition;
    if (playerEffect && (gBattleMResultFlags5A4A20 == 0 || value != -1)) {
        AppendBattleEffectList_4393A0(gBattleAllEntries59E980[effectCasterPosition], -2, 0, 0);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 223);
        effectCasterPosition = gBattleMCasterPosition5AAA64;
    }
    const std::int32_t effectDefenderPosition = gBattleMDefenderPosition5A4A24;
    YobiAction* const effectCaster = gBattleAllEntries59E980[effectCasterPosition];
    AppendBattleEffectList_4393A0(effectCaster, effectDefenderPosition, 0, 0);
    if (gBattleMResultFlags5A4A20 == 0) {
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22339;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 67);
        std::int32_t failedDefenderPosition = gBattleMDefenderPosition5A4A24;
        YobiAction* const defender = gBattleAllEntries59E980[failedDefenderPosition];
        if (defender == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        if ((*reinterpret_cast<const unsigned char*>(
                &defender->payload.battleActor->flagsFDC) & 2u) != 0) {
            ResetBattleCastList_4391E0(failedDefenderPosition);
            failedDefenderPosition = gBattleMDefenderPosition5A4A24;
        }
        gBattleAllEntries59E980[failedDefenderPosition]->payload.battleActor
            ->value1018 = 205;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], 80);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], 53);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], 51);
    } else {
        const std::int32_t monsterPosition = gBattleMDefenderPosition5A4A24;
        YobiAction** const defenderSlot = &gBattleAllEntries59E980[monsterPosition];
        if (*defenderSlot == nullptr) {
            *defenderSlot = NewBattleActor_438290(monsterPosition);
        }
        YobiAction* const defender = *defenderSlot;
        auto* const defenderPayload = defender->payload.battleActor;
        TakeBattleMString_438FE0(scene, defender->nameC4.data(), 29);
        static_cast<void>(input::UnescapeLegacyText_52D150(defender->nameC4.data()));
        defender->imageNo = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
        defender->field1D0 = TakeBattleMInteger_438F70(scene);
        {
            defender->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            defender->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            defender->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            defender->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        defenderPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        defenderPayload->stateFE0 = 0;
        defender->fields288[3] = 0;
        defender->fields288[2] = 0;
        defender->fields288[1] = 0;
        defender->fields288[0] = 0;
        if ((defenderPayload->flagsFDC & 0x01000000u) != 0) {
            defender->fields288[0] = 1;
        }
        if ((defenderPayload->flagsFDC & 0x02000000u) != 0) {
            defender->fields288[1] = 1;
        }
        if ((defenderPayload->flagsFDC & 0x04000000u) != 0) {
            defender->fields288[2] = 1;
        }
        if ((defenderPayload->flagsFDC & 0x08000000u) != 0) {
            defender->fields288[3] = 1;
        }
        defenderPayload->castPlayIndexFC4 = 0;
        defenderPayload->castListIndexFC8 = -1;
        defenderPayload->castListCA4[0] = 0;
        defenderPayload->castListCA4[1] = -1;
        defender->offset40 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[gBattleMDefenderPosition5A4A24]][2]);
        defender->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[gBattleMDefenderPosition5A4A24]][3]);
        defender->x = gBattleGraphicPoints553758[gBattlePointIndices5536FC[gBattleMDefenderPosition5A4A24]][2];
        defender->y = gBattleGraphicPoints553758[gBattlePointIndices5536FC[gBattleMDefenderPosition5A4A24]][3];
        defender->scaleX = 0.0F;
        defender->scaleY = 0.0F;
        defenderPayload->battlePosition18 = gBattleMDefenderPosition5A4A24;
        UpdateBattleAlbumFlag_4238F0(defender);
        defender->phase = 0;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22338;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 67);
        gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24]->payload.battleActor
            ->value1018 = 204;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], 80);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], 54);
    }
    AppendBattleEffectList_4393A0(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    payload->packetSubState58 = 1;
}

// 原427470的ITM=7；三模式保留不同的目标结束路径，返回后仍由总入口回存。
[[maybe_unused]] void ProcessBattleMItemHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    using namespace world::detail;
    const std::int32_t subState = payload->packetSubState58;
    if (subState != 0) {
        const std::int32_t mode = gBattleMValue59FA10;
        if (subState == 1 && (mode == 0 || mode == 6 || mode == 7)
            && CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }
    const std::int32_t mode = TakeBattleMInteger_438F70(scene);
    gBattleMValue59FA10 = mode;
    if (mode != 0 && mode != 6 && mode != 7) {
        return;
    }
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const actor = gBattleAllEntries59E980[casterPosition];
    if (actor == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    if (mode == 7) {
        actor->payload.battleActor->value1018 = 438;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 205);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 38);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 37);
        MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        ++payload->packetSubState58;
        return;
    }
    AppendBattleCast_4392A0(scene, actor, 25);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 138);
    if (mode == 6) {
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 73);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 0);
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            return;
        }
        if (gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::int32_t value = TakeBattleMInteger_438F70(scene);
        const std::int32_t valueTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMMessageValue59FA14 = value;
        gBattleAllEntries59E980[valueTargetPosition]->payload.battleActor
            ->value1030 = value;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, 0, 0);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 154);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 155);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 156);
        const std::int32_t reverseTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t reverseCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const reverseTarget = gBattleAllEntries59E980[reverseTargetPosition];
        AppendBattleEffectList_4393A0(reverseTarget, reverseCasterPosition, 0, 0);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 73);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 74);
        if (gBattleMMessageValue59FA14 == 1) {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->reactionImage1010 = 28880;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 57);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 57);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 57);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
        } else {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->reactionImage1010 = 22333;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 58);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 58);
        }
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
        MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        ++payload->packetSubState58;
        return;
    }

    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 71);
    const std::int32_t value = TakeBattleMInteger_438F70(scene);
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        if (gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::int32_t parsedMessageValue = TakeBattleMInteger_438F70(scene);
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMMessageValue59FA14 = parsedMessageValue;
        YobiAction* const effectCaster = gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, 0, 0);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        const bool secondKind = value == 7;
        YobiAction* const skillTarget = gBattleAllEntries59E980[skillTargetPosition];
        skillTarget->payload.battleActor->castSkillLevel1044 = 0;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], secondKind ? 150 : 149);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 160);
        const std::int32_t messageTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const messageTarget = gBattleAllEntries59E980[messageTargetPosition];
        const std::int32_t currentMessageValue = gBattleMMessageValue59FA14;
        messageTarget->payload.battleActor->value1030 = currentMessageValue;
        gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->value1034 = 5;
    }
    AppendBattleEffectList_4393A0(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原427470的CMB=8；总入口phase8调用，原局部图号跨本次循环保留，处理四个子状态。
[[maybe_unused]] void ProcessBattleMComboHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    using namespace world::detail;
    switch (payload->packetSubState58) {
    case 0:
        break;
    case 1:
        if (payload->value00 >= payload->entryCount04) {
            payload->value00 = 0;
            ++payload->packetSubState58;
        }
        return;
    case 2: {
        YobiAction* const actor = gBattleAllEntries59E980[payload->entryPositions08[payload->value00]];
        if (actor->phase == 0) {
            AdvanceBattleCastState_439340(actor);
        }
        const std::int32_t count = payload->entryCount04;
        if (payload->value00 >= count - 1) {
            if ((gBattleMResultFlags5A4A20 & 0x404u) == 0 && count >= 2) {
                const std::int32_t sequenceValue = gBattleMValue5ABA7C;
                const std::uint8_t sequenceCount =
                    *reinterpret_cast<const unsigned char*>(&payload->entryCount04);
                NewBattleSequence_420100(std::bit_cast<std::int8_t>(sequenceCount), sequenceValue);
            }
            ++payload->packetSubState58;
        }
        return;
    }
    case 3:
        if (CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            YobiAction* const target = gBattleAllEntries59E980[gBattleMTargetPosition5AAA40];
            if (target != nullptr && (*reinterpret_cast<const unsigned char*>(
                    &target->payload.battleActor->flagsFDC) & 2u) == 0) {
                Sub_406260(target);
            }
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    default:
        return;
    }

    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    std::int32_t amount; // 原a4仅在每个非空施法者解析时写入，不补空列表默认值。
    payload->value00 = 0;
    payload->entryCount04 = 0;
    gBattleMMessageValue59FA14 = 0;
    const std::int32_t initialTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMTargetPosition5AAA40 = initialTargetPosition;
    YobiAction* const initialTarget = gBattleAllEntries59E980[initialTargetPosition];
    if (initialTarget == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    AppendBattleCast_4392A0(scene, initialTarget, 0);
    for (;;) {
        const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool casterUnavailable = casterPosition == 255;
        gBattleMCasterPosition5AAA64 = casterPosition;
        if (casterUnavailable) {
            break;
        }
        if (gBattleAllEntries59E980[casterPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        amount = TakeBattleMInteger_438F70(scene);
        if ((gBattleMResultFlags5A4A20 & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->castSkill103C = -1;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        std::int32_t effectAmount = amount;
        if ((effectFlags & 0xE000u) == 0) {
            const std::int32_t accumulatedValue = gBattleMMessageValue59FA14 + amount;
            gBattleMMessageValue59FA14 = accumulatedValue;
            effectAmount = accumulatedValue;
        }
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, {effectTargetPosition, effectFlags, effectAmount, firstImage});
        const std::int32_t entryIndex = payload->entryCount04;
        const std::int32_t entryCasterPosition = gBattleMCasterPosition5AAA64;
        payload->entryPositions08[entryIndex] = entryCasterPosition;
        ++payload->entryCount04;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 7);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 9);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 72);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 0);
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxiliaryImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxiliaryTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t auxiliaryCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const auxiliaryCaster = gBattleAllEntries59E980[auxiliaryCasterPosition];
        AppendBattleAuxEffects_43BC10(auxiliaryCaster, auxiliaryTargetPosition, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, auxiliaryImage);
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 14);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0xE000u) == 0) {
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1038 = 1;
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & 0x40u) != 0) {
            if ((flags & 3u) != 0 && (flags & 0x2000u) != 0) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 34);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 127);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
                flags = gBattleMResultFlags5A4A20;
                if ((flags & 0x2000u) != 0) {
                    const std::int32_t playerCasterPosition = gBattleMCasterPosition5AAA64;
                    if (gBattleCurrentPosition5ABCA4 == playerCasterPosition) {
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[playerCasterPosition], 256);
                    } else {
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[playerCasterPosition], 52);
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
                    }
                    flags = gBattleMResultFlags5A4A20;
                }
            }
        } else if (amount != 0 && (flags & 3u) != 0) {
            if ((flags & 0x8000u) != 0) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 129);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
                flags = gBattleMResultFlags5A4A20;
            } else if ((flags & 0x4000u) != 0) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 128);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
                flags = gBattleMResultFlags5A4A20;
            } else if ((flags & 0x2000u) != 0) {
                const bool secondReaction = (flags & 2u) != 0;
                const std::int32_t reactionCasterPosition = gBattleMCasterPosition5AAA64;
                const std::int32_t reactionKind = secondReaction ? 165 : 27;
                YobiAction* const reactionCaster = gBattleAllEntries59E980[reactionCasterPosition];
                AppendBattleCast_4392A0(scene, reactionCaster, reactionKind);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 127);
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & 0x2020u) == 0x2020u) {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 11);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 46);
        } else {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 7);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 11);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
        }
        MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        gBattleMValue5ABA7C = gBattleMCasterPosition5AAA64;
    }

    gBattleAllEntries59E980[gBattleMValue5ABA7C]->payload.battleActor->value1038 = 0;
    std::uint32_t flags = gBattleMResultFlags5A4A20;
    if ((flags & 0xE000u) != 0) {
        YobiAction* const target = gBattleAllEntries59E980[gBattleMTargetPosition5AAA40];
        AppendBattleCastAt_4392F0(scene, target, -1,
            target->payload.battleActor->castListIndexFC8 - 1);
    } else {
        if ((flags & 0x40u) != 0) {
            if ((flags & 3u) != 0) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                if ((gBattleMResultFlags5A4A20 & 0x2000u) != 0) {
                    const std::int32_t playerCasterPosition = gBattleMCasterPosition5AAA64;
                    if (gBattleCurrentPosition5ABCA4 == playerCasterPosition) {
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[playerCasterPosition], 256);
                    } else {
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[playerCasterPosition], 52);
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
                    }
                } else {
                    const std::int32_t playerTargetPosition = gBattleMTargetPosition5AAA40;
                    if (gBattleCurrentPosition5ABCA4 == playerTargetPosition) {
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[playerTargetPosition], 256);
                    } else {
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[playerTargetPosition], 52);
                        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                    }
                }
                flags = gBattleMResultFlags5A4A20;
            }
        } else if (amount != 0 && (flags & 3u) != 0) {
            const std::int32_t reactionKind = (flags & 2u) != 0 ? 165 : 27;
            const std::int32_t reactionTargetPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const reactionTarget = gBattleAllEntries59E980[reactionTargetPosition];
            AppendBattleCast_4392A0(scene, reactionTarget, reactionKind);
            flags = gBattleMResultFlags5A4A20;
        }
        std::int32_t followupTargetPosition;
        if ((amount != 0 || (flags & 8u) != 0) && (flags & 0x4000u) == 0) {
            followupTargetPosition = gBattleMTargetPosition5AAA40;
            const std::int32_t lastCasterPosition = gBattleMValue5ABA7C;
            const bool samePosition = followupTargetPosition == lastCasterPosition;
            if ((flags & 0x2000u) != 0 ? samePosition : !samePosition) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[followupTargetPosition], 11);
                followupTargetPosition = gBattleMTargetPosition5AAA40;
                flags = gBattleMResultFlags5A4A20;
            }
        } else {
            followupTargetPosition = gBattleMTargetPosition5AAA40;
        }
        if ((flags & 0x20u) != 0) {
            const bool sameSide = (flags & 0x2000u) != 0;
            const std::int32_t lastCasterPosition = gBattleMValue5ABA7C;
            const bool samePosition = followupTargetPosition == lastCasterPosition;
            if (sameSide ? samePosition : !samePosition) {
                AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[followupTargetPosition], 46);
            }
        }
    }
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原427470的GMN=21；模式0仅退场，非零模式保留固定新对象与当前槽的区别。
[[maybe_unused]] void ProcessBattleMGmnHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    using namespace world::detail;
    const std::int32_t subState = payload->packetSubState58;
    if (subState != 0) {
        if (subState != 1) {
            return;
        }
        YobiAction* const first = gBattleAllEntries59E980[20];
        if (first != nullptr && first->payload.battleActor->busyFD8 == 1) {
            return;
        }
        YobiAction* const second = gBattleAllEntries59E980[21];
        if (second != nullptr && second->payload.battleActor->busyFD8 == 1) {
            return;
        }
        if (CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }

    const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t mode = TakeBattleMInteger_438F70(scene);
    if (mode == 0) {
        YobiAction** const slot = &gBattleAllEntries59E980[position];
        const std::int32_t palette = ((*slot)->mapX1E0).value();
        AppendBattleEffectList_4393A0(*slot, palette, 0, 40);
        AppendBattleCast_4392A0(scene, *slot, 174);
        AppendBattleCast_4392A0(scene, *slot, 176);
        AppendBattleCast_4392A0(scene, *slot, 172);
        AppendBattleCast_4392A0(scene, *slot, 168);
        AppendBattleCast_4392A0(scene, *slot, 51);
        const std::int32_t begin = position == 21 ? 10 : 0;
        const std::int32_t end = position == 21 ? 20 : 10;
        for (std::int32_t index = begin; index < end; ++index) {
            YobiAction** const memberSlot = &gBattleAllEntries59E980[index];
            YobiAction* const member = *memberSlot;
            if (member != nullptr) {
                AppendBattleCast_4392A0(scene, member, 171);
                AppendBattleCast_4392A0(scene, *memberSlot, 168);
                MoveBattleCastState_439380(*memberSlot);
            }
        }
        MoveBattleCastState_439380(*slot);
        ++payload->packetSubState58;
        return;
    }

    std::int8_t entrance = 0;
    if (mode == 1) {
        entrance = 2;
    } else if (mode == 2) {
        entrance = static_cast<std::int8_t>((platform::Rand16() & 1) + 1);
        static_cast<void>(NewBattleScreenEmitter_421700(150));
    }
    gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t value1018 = TakeBattleMInteger_438F70(scene);
    std::int32_t effectValue = TakeBattleMInteger_438F70(scene);
    if (effectValue == 0) {
        effectValue = -1;
    }
    YobiAction** const slot = &gBattleAllEntries59E980[position];
    YobiAction* const previous = *slot;
    if (previous != nullptr) {
        static_cast<void>(MarkDeath(previous));
    }
    YobiAction* const created = NewBattleActor_438290(position);
    *slot = created;
    auto* const actorPayload = created->payload.battleActor;
    TakeBattleMString_438FE0(scene, created->nameC4.data(), 29);
    created->imageNo = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
    created->field1D0 = TakeBattleMInteger_438F70(scene);
    {
        created->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
    }
    {
        created->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
    }
    {
        created->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
    }
    {
        created->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
    }
    actorPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
    actorPayload->stateFE0 = 0;
    actorPayload->castPlayIndexFC4 = 0;
    actorPayload->castListIndexFC8 = -1;
    actorPayload->castListCA4[0] = 0;
    actorPayload->castListCA4[1] = -1;
    actorPayload->effectRunningC9C = 0;
    actorPayload->effectCountCA0 = 0;
    actorPayload->effects1C[0].position = -1;
    actorPayload->effects1C[1].position = -1;
    created->offset40 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][2]);
    created->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][3]);
    created->x = gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][2];
    created->y = gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][3];
    actorPayload->battlePosition18 = position;
    created->phase = 0;
    created->rebirthValue1 = 4;
    created->rebirthValue2 = 4;
    {
        created->mapX1E0 = static_cast<std::int32_t>(render::gActivePalette);
    }
    static_cast<void>(PlayFrame_517DA0(*slot, 0));
    static_cast<void>(NewBattleText_4217F0(4, created->nameC4.data(), 0, 100));
    (*slot)->payload.battleActor->value1018 = value1018;
    AppendBattleCast_4392A0(scene, *slot, 80);
    AppendBattleEffect_439400(*slot, {13, 0, 40, effectValue});
    AppendBattleCast_4392A0(scene, *slot, 175);
    AppendBattleCast_4392A0(scene, *slot, 174);
    AppendBattleCast_4392A0(scene, *slot, 0);
    AppendBattleCast_4392A0(scene, *slot, 37);
    AppendBattleCast_4392A0(scene, *slot, 167);
    if (entrance == 0) {
        created->scaleX = 0.0F;
        created->scaleY = 0.0F;
        AppendBattleCast_4392A0(scene, *slot, 54);
    } else if (entrance == 1) {
        SetYobiActionField30F(*slot, 0);
        (*slot)->flags |= 2u;
        Sub_406260(*slot);
        AppendBattleCast_4392A0(scene, *slot, 171);
    } else if (entrance == 2) {
        {
            const std::int32_t pointIndex = gBattlePointIndices5536FC[position];
            YobiAction* const actor = *slot;
            actor->offset40 = static_cast<float>(gBattleGraphicPoints553758[pointIndex][0]);
        }
        {
            const std::int32_t pointIndex = gBattlePointIndices5536FC[position];
            YobiAction* const actor = *slot;
            actor->offset44 = static_cast<float>(gBattleGraphicPoints553758[pointIndex][1]);
        }
        {
            const std::int32_t pointIndex = gBattlePointIndices5536FC[position];
            YobiAction* const actor = *slot;
            auto* const currentPayload = actor->payload.battleActor;
            currentPayload->moveTargetXFE8 = gBattleGraphicPoints553758[pointIndex][2];
        }
        {
            const std::int32_t pointIndex = gBattlePointIndices5536FC[position];
            YobiAction* const actor = *slot;
            auto* const currentPayload = actor->payload.battleActor;
            currentPayload->moveTargetYFEC = gBattleGraphicPoints553758[pointIndex][3];
        }
        YobiAction* const xActor = *slot;
        xActor->x = static_cast<std::int32_t>(static_cast<std::int64_t>(xActor->offset40));
        YobiAction* const yActor = *slot;
        yActor->y = static_cast<std::int32_t>(static_cast<std::int64_t>(yActor->offset44));
        Sub_406260(*slot);
        AppendBattleCast_4392A0(scene, *slot, 6);
        AppendBattleCast_4392A0(scene, *slot, 10);
    }

    const std::int32_t begin = position == 21 ? 10 : 0;
    const std::int32_t end = position == 21 ? 20 : 10;
    std::int32_t linkedPosition = gBattleMCasterPosition5AAA64;
    if (position == 20 || position == 21) {
        for (std::int32_t index = begin; index < end; ++index) {
            if (gBattleAllEntries59E980[index] != nullptr) {
                linkedPosition = index;
                break;
            }
        }
    }
    for (std::int32_t index = begin; index < end; ++index) {
        YobiAction** const memberSlot = &gBattleAllEntries59E980[index];
        YobiAction* const member = *memberSlot;
        if (member == nullptr) {
            continue;
        }
        AppendBattleCast_4392A0(scene, member, 37);
        AppendBattleCast_4392A0(scene, *memberSlot, 167);
        YobiAction* const colored = *memberSlot;
        colored->color2 = std::bit_cast<std::int32_t>(colored->drawMode.color);
        YobiAction* const flagActor = *memberSlot;
        const bool linkedMember = index == linkedPosition;
        SetYobiActionField30F(flagActor, 255);
        if (linkedMember) {
            AppendBattleEffect_439400(*memberSlot,
                {position, static_cast<std::uint32_t>(static_cast<std::int32_t>(entrance)), 0, effectValue});
            AppendBattleCast_4392A0(scene, *memberSlot, 73);
            AppendBattleCast_4392A0(scene, *memberSlot, 74);
        }
        const bool casterMember = index == gBattleMCasterPosition5AAA64;
        YobiAction* const castMember = *memberSlot;
        AppendBattleCast_4392A0(scene, castMember, casterMember ? 173 : 172);
        if (index == linkedPosition) {
            AppendBattleCast_4392A0(scene, *memberSlot, 0);
        }
        MoveBattleCastState_439380(*memberSlot);
    }
    AppendBattleEffect_439400(*slot, {linkedPosition, 0, 0, 0});
    AppendBattleCast_4392A0(scene, *slot, 25);
    AppendBattleCast_4392A0(scene, *slot, 73);
    AppendBattleCast_4392A0(scene, *slot, 74);
    MoveBattleCastState_439380(*slot);
    gBattleMValue5ABA7C = gBattleMCasterPosition5AAA64;
    ++payload->packetSubState58;
}

// 原427470的RID=22；独立于4239D0预读路径，固定旧载荷并保留两名新角色各自坐标。
[[maybe_unused]] void ProcessBattleMRideHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    using namespace world::detail;
    const std::int32_t subState = payload->packetSubState58;
    if (subState != 0) {
        if (subState == 1 && CheckBattleTurnReady_4394A0(scene, 0) == 1) {
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }
    const std::int32_t mode = TakeBattleMInteger_438F70(scene);
    gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t secondPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    YobiAction* const existing = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
    if (existing == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        ++payload->packetSubState58;
        return;
    }
    auto* const existingPayload = existing->payload.battleActor;
    const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
    gBattleMResultFlags5A4A20 = resultFlags;
    if (mode != 0) {
        if ((resultFlags & 0x80000u) != 0) {
            const std::int32_t firstPosition = gBattleMCasterPosition5AAA64;
            if (gBattleAllEntries59E980[secondPosition] == nullptr) {
                FinishBattleEvent_43B6C0(scene);
                ++payload->packetSubState58;
                return;
            }
            YobiAction* const carrierAction = NewYobiAction_404AE0(2, sizeof(BattleRideCarrierPayload), YobiPayloadKind::BattleRideCarrier);
            auto* const carrier = carrierAction->payload.battleRideCarrier;
            TakeBattleMString_438FE0(scene, carrierAction->nameC4.data(), 29);
            carrierAction->imageNo = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
            carrier->gearImage0C = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
            carrierAction->field1D0 = TakeBattleMInteger_438F70(scene);
            {
                carrierAction->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                carrierAction->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                carrierAction->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                carrierAction->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            carrier->flags08 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
            carrierAction->offset40 = static_cast<float>(
                gBattleGraphicPoints553758[gBattlePointIndices5536FC[firstPosition]][2]);
            carrierAction->offset44 = static_cast<float>(
                gBattleGraphicPoints553758[gBattlePointIndices5536FC[firstPosition]][3]);
            existingPayload->effect1060 = carrierAction;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 224);
            const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
            const BattleEntryEffectRecord effectRecord {firstPosition, 1u, effectCasterPosition, secondPosition};
            AppendBattleEffect_439400(gBattleAllEntries59E980[effectCasterPosition], effectRecord);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
        } else {
            const std::int32_t firstPosition = gBattleMCasterPosition5AAA64;
            YobiAction* const effect = NewYobiAction_404AE0(2, 0);
            existingPayload->effect1060 = effect;
            effect->offset40 = static_cast<float>(
                gBattleGraphicPoints553758[gBattlePointIndices5536FC[firstPosition]][2]);
            const std::int32_t pointIndexY = gBattlePointIndices5536FC[firstPosition];
            YobiAction* const effectY = existingPayload->effect1060;
            effectY->offset44 = static_cast<float>(gBattleGraphicPoints553758[pointIndexY][3]);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 225);
            const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
            const BattleEntryEffectRecord effectRecord {firstPosition, 1u, effectCasterPosition, secondPosition};
            AppendBattleEffect_439400(gBattleAllEntries59E980[effectCasterPosition], effectRecord);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->reactionImage1010 = 22331;
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 58);
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 58);
            YobiAction* const second = gBattleAllEntries59E980[secondPosition];
            if (second != nullptr) {
                auto* const secondPayload = second->payload.battleActor;
                if ((*reinterpret_cast<const unsigned char*>(&secondPayload->flagsFDC) & 2u) == 0) {
                    secondPayload->reactionImage1010 = 22334;
                    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[secondPosition], 68);
                    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[secondPosition], 65);
                    MoveBattleCastState_439380(gBattleAllEntries59E980[secondPosition]);
                }
            }
        }
    } else {
        YobiAction* const carrierAction = NewYobiAction_404AE0(2, sizeof(BattleRideCarrierPayload), YobiPayloadKind::BattleRideCarrier);
        auto* const carrier = carrierAction->payload.battleRideCarrier;
        existingPayload->effect1060 = carrierAction;
        const std::int32_t firstPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const first = NewBattleActor_438290(firstPosition);
        auto* const firstPayload = first->payload.battleActor;
        TakeBattleMString_438FE0(scene, first->nameC4.data(), 29);
        first->imageNo = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
        carrier->gearImage0C = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
        first->field1D0 = TakeBattleMInteger_438F70(scene);
        {
            first->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            first->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            first->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            first->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        firstPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        firstPayload->stateFE0 = 0;
        first->fields288[3] = 0;
        first->fields288[2] = 0;
        first->fields288[1] = 0;
        first->fields288[0] = 0;
        if ((firstPayload->flagsFDC & 0x01000000u) != 0) {
            first->fields288[0] = 1;
        }
        if ((firstPayload->flagsFDC & 0x02000000u) != 0) {
            first->fields288[1] = 1;
        }
        if ((firstPayload->flagsFDC & 0x04000000u) != 0) {
            first->fields288[2] = 1;
        }
        if ((firstPayload->flagsFDC & 0x08000000u) != 0) {
            first->fields288[3] = 1;
        }
        firstPayload->castPlayIndexFC4 = 0;
        firstPayload->castListIndexFC8 = -1;
        firstPayload->castListCA4[0] = 0;
        firstPayload->castListCA4[1] = -1;
        first->offset40 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[firstPosition]][2]);
        first->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[firstPosition]][3]);
        first->x = gBattleGraphicPoints553758[gBattlePointIndices5536FC[firstPosition]][2];
        first->y = gBattleGraphicPoints553758[gBattlePointIndices5536FC[firstPosition]][3];
        first->scaleX = 1.0F;
        first->scaleY = 1.0F;
        firstPayload->battlePosition18 = firstPosition;
        UpdateBattleAlbumFlag_4238F0(first);
        first->phase = 0;
        first->flags |= 2u;
        first->callback = nullptr;
        carrier->first00 = first;
        std::strcpy(existingPayload->rider13C8->nameC4.data(), first->nameC4.data());
        existingPayload->rider13C8->field1D0 = first->field1D0;
        existingPayload->rider13C8->readiness = first->readiness;
        existingPayload->rider13C8->maxHp = first->maxHp;
        existingPayload->rider13C8->barValue1B0 = first->barValue1B0;
        existingPayload->rider13C8->barMaximum1C0 = first->barMaximum1C0;
        const std::int32_t casterPositionX = gBattleMCasterPosition5AAA64;
        YobiAction* const riderX = existingPayload->rider13C8;
        YobiAction* const sourceX = gBattleAllEntries59E980[casterPositionX];
        riderX->offset40 = static_cast<float>(static_cast<double>(sourceX->frameX) + sourceX->offset40);
        const std::int32_t casterPositionY = gBattleMCasterPosition5AAA64;
        YobiAction* const riderY = existingPayload->rider13C8;
        YobiAction* const sourceY = gBattleAllEntries59E980[casterPositionY];
        riderY->offset44 = sourceY->offset44;

        YobiAction* const second = NewBattleActor_438290(secondPosition);
        auto* const secondPayload = second->payload.battleActor;
        TakeBattleMString_438FE0(scene, second->nameC4.data(), 29);
        second->imageNo = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(scene));
        second->field1D0 = TakeBattleMInteger_438F70(scene);
        {
            second->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            second->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            second->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            second->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        secondPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        secondPayload->stateFE0 = 0;
        second->fields288[3] = 0;
        second->fields288[2] = 0;
        second->fields288[1] = 0;
        second->fields288[0] = 0;
        if ((secondPayload->flagsFDC & 0x01000000u) != 0) {
            second->fields288[0] = 1;
        }
        if ((secondPayload->flagsFDC & 0x02000000u) != 0) {
            second->fields288[1] = 1;
        }
        if ((secondPayload->flagsFDC & 0x04000000u) != 0) {
            second->fields288[2] = 1;
        }
        if ((secondPayload->flagsFDC & 0x08000000u) != 0) {
            second->fields288[3] = 1;
        }
        secondPayload->castPlayIndexFC4 = 0;
        secondPayload->castListIndexFC8 = -1;
        secondPayload->castListCA4[0] = 0;
        secondPayload->castListCA4[1] = -1;
        second->offset40 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[secondPosition]][2]);
        second->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[secondPosition]][3]);
        second->x = gBattleGraphicPoints553758[gBattlePointIndices5536FC[secondPosition]][2];
        second->y = gBattleGraphicPoints553758[gBattlePointIndices5536FC[secondPosition]][3];
        second->scaleX = 1.0F;
        second->scaleY = 1.0F;
        secondPayload->battlePosition18 = secondPosition;
        UpdateBattleAlbumFlag_4238F0(second);
        second->phase = 0;
        second->flags |= 2u;
        second->callback = nullptr;
        carrier->second04 = second;
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 224);
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const BattleEntryEffectRecord effectRecord {effectCasterPosition, 0u, firstPosition, secondPosition};
        AppendBattleEffect_439400(gBattleAllEntries59E980[effectCasterPosition], effectRecord);
        YobiAction* const firstEntry = carrier->first00;
        if ((*reinterpret_cast<const unsigned char*>(
                &firstEntry->payload.battleActor->flagsFDC) & 2u) != 0) {
            AppendBattleCast_4392A0(scene, firstEntry, 46);
        }
        YobiAction* const secondEntry = carrier->second04;
        if ((*reinterpret_cast<const unsigned char*>(
                &secondEntry->payload.battleActor->flagsFDC) & 2u) != 0) {
            AppendBattleCast_4392A0(scene, secondEntry, 46);
        }
    }
    MoveBattleCastState_439380(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

} // namespace

// 原函数：0x00427470。
void UpdateBattleEvent_427470(YobiAction* const scene)
{
    auto* const payload = scene->payload.battleScene;
    std::uint32_t flags9C;
    std::int32_t valueA0;
    if (!PrepareBattleMExecution(scene, payload, flags9C, valueA0)) {
        return;
    }
    switch (scene->phase) {
    case 0: {
        // E10／42A768：完成走437ABA出口，处理中仍走437A29公共回存。
        const auto result = ProcessLuaSkillPacket(scene);
        if (result == LuaSkillPacketResult::Finished) return;
        if (result == LuaSkillPacketResult::Native) {
            ProcessBattleMSkillHeader(scene, payload, flags9C, valueA0);
        }
        break;
    }
    case 1:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::Escape);
        break;
    case 2:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::Clear);
        break;
    case 3:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::Support);
        break;
    case 4:
        ProcessBattleMPositionHeader(scene, payload);
        break;
    case 5:
        ProcessBattleMMonsterHeader(scene, payload);
        break;
    case 6:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::Equipment);
        break;
    case 7:
        ProcessBattleMItemHeader(scene, payload);
        break;
    case 8:
        ProcessBattleMComboHeader(scene, payload);
        break;
    case 9:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::StatusValue);
        break;
    case 10:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::Debuff);
        break;
    case 11:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::Power);
        break;
    case 12:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::Action);
        break;
    case 13:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::ConsumeFp);
        break;
    case 14:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::Chat);
        break;
    case 15:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::PetNotice);
        break;
    case 16:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::PetLeave);
        break;
    case 17:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::ChangeBody);
        break;
    case 18:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::PetDown);
        break;
    case 19:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::Rebirth);
        break;
    case 20:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::QueuedValue);
        break;
    case 21:
        ProcessBattleMGmnHeader(scene, payload);
        break;
    case 22:
        ProcessBattleMRideHeader(scene, payload);
        break;
    case 23:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::AppendSkill);
        break;
    case 24:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::End);
        break;
    default:
        break;
    }
    SaveBattleMExecutionState(payload);
}

// 原函数：0x004239D0。
void ParseBattleMRide_4239D0(YobiAction* const action)
{
    using namespace world::detail;
    auto* const scene = action->payload.battleScene;
    std::array<char, 4> header;
    std::array<char, 32> previewName;
    for (;;) {
        const std::int32_t initialOffset = scene->packetOffset64;
        static_cast<void>(GetBattleMObjectHeader_423960(action, header.data()));
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind != 22) {
            scene->packetOffset64 = initialOffset;
            return;
        }
        std::int32_t previewOffset = scene->packetOffset64;
        if (TakeBattleMIntegerAt_43B940(&previewOffset) != 0) {
            scene->packetOffset64 = initialOffset;
            return;
        }
        for (std::int32_t i = 0; i < 3; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        }
        TakeBattleMStringAt_43B8C0(&previewOffset, previewName.data(), 29);
        // 原预读20个整数，实际消费19个；成功后仍发布预读游标。
        for (std::int32_t i = 0; i < 8; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        }
        const std::int32_t firstPreviewFlags = TakeBattleMIntegerAt_43B940(&previewOffset);
        TakeBattleMStringAt_43B8C0(&previewOffset, previewName.data(), 29);
        for (std::int32_t i = 0; i < 6; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        }
        const std::int32_t secondPreviewFlags = TakeBattleMIntegerAt_43B940(&previewOffset);
        if ((*reinterpret_cast<const unsigned char*>(&firstPreviewFlags) & 2u) == 0
            && (secondPreviewFlags & 2) == 0) {
            scene->packetOffset64 = initialOffset;
            return;
        }

        static_cast<void>(TakeBattleMInteger_438F70(action));
        const std::int32_t first = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(action));
        const std::int32_t second = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(action));
        YobiAction* const existing = gBattleAllEntries59E980[first];
        if (existing == nullptr) {
            FinishBattleEvent_43B6C0(action);
            ++scene->packetSubState58;
            return;
        }
        auto* const existingPayload = existing->payload.battleActor;
        static_cast<void>(TakeBattleMInteger_438F70(action));
        YobiAction* const firstActor = NewBattleActor_438290(first);
        auto* const firstPayload = firstActor->payload.battleActor;
        TakeBattleMString_438FE0(action, firstActor->nameC4.data(), 29);
        firstActor->imageNo = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(action));
        const std::int32_t gearImage = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(action));
        firstActor->field1D0 = TakeBattleMInteger_438F70(action);
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            firstActor->readiness = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            firstActor->maxHp = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            firstActor->barValue1B0 = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            firstActor->barMaximum1C0 = static_cast<std::int32_t>(value);
        }
        firstPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(action));
        firstPayload->stateFE0 = 0;
        firstActor->fields288[3] = 0;
        firstActor->fields288[2] = 0;
        firstActor->fields288[1] = 0;
        firstActor->fields288[0] = 0;
        if ((firstPayload->flagsFDC & 0x1000000u) != 0) {
            firstActor->fields288[0] = 1;
        }
        if ((firstPayload->flagsFDC & 0x2000000u) != 0) {
            firstActor->fields288[1] = 1;
        }
        if ((firstPayload->flagsFDC & 0x4000000u) != 0) {
            firstActor->fields288[2] = 1;
        }
        if ((firstPayload->flagsFDC & 0x8000000u) != 0) {
            firstActor->fields288[3] = 1;
        }
        firstPayload->castPlayIndexFC4 = 0;
        firstPayload->castListIndexFC8 = -1;
        firstPayload->castListCA4[0] = 0;
        firstPayload->castListCA4[1] = -1;
        firstActor->offset40 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[first]][2]);
        firstActor->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[first]][3]);
        firstActor->x = gBattleGraphicPoints553758[gBattlePointIndices5536FC[first]][2];
        firstActor->y = gBattleGraphicPoints553758[gBattlePointIndices5536FC[first]][3];
        action->scaleX = 1.0F;
        action->scaleY = 1.0F;
        firstPayload->battlePosition18 = first;
        UpdateBattleAlbumFlag_4238F0(firstActor);
        const std::uint32_t firstFlags = firstActor->flags | 2u;
        firstActor->phase = 0;
        firstActor->flags = firstFlags;
        firstActor->callback = nullptr;

        std::strcpy(existingPayload->rider13C8->nameC4.data(), firstActor->nameC4.data());
        existingPayload->rider13C8->field1D0 = firstActor->field1D0;
        existingPayload->rider13C8->readiness = firstActor->readiness;
        existingPayload->rider13C8->maxHp = firstActor->maxHp;
        existingPayload->rider13C8->barValue1B0 = firstActor->barValue1B0;
        existingPayload->rider13C8->barMaximum1C0 = firstActor->barMaximum1C0;
        YobiAction* const riderX = existingPayload->rider13C8;
        YobiAction** const originalSlot = &gBattleAllEntries59E980[first];
        YobiAction* const sourceX = *originalSlot;
        riderX->offset40 = static_cast<float>(static_cast<double>(sourceX->frameX) + sourceX->offset40);
        YobiAction* const sourceY = *originalSlot;
        YobiAction* const riderY = existingPayload->rider13C8;
        riderY->offset44 = sourceY->offset44;

        YobiAction* const secondActor = NewBattleActor_438290(second);
        auto* const secondPayload = secondActor->payload.battleActor;
        TakeBattleMString_438FE0(action, secondActor->nameC4.data(), 29);
        secondActor->imageNo = character_select::detail::Sub_51B740(TakeBattleMInteger_438F70(action));
        secondActor->field1D0 = TakeBattleMInteger_438F70(action);
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            secondActor->readiness = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            secondActor->maxHp = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            secondActor->barValue1B0 = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            secondActor->barMaximum1C0 = static_cast<std::int32_t>(value);
        }
        secondPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(action));
        secondPayload->stateFE0 = 0;
        secondActor->fields288[3] = 0;
        secondActor->fields288[2] = 0;
        secondActor->fields288[1] = 0;
        secondActor->fields288[0] = 0;
        if ((secondPayload->flagsFDC & 0x1000000u) != 0) {
            secondActor->fields288[0] = 1;
        }
        if ((secondPayload->flagsFDC & 0x2000000u) != 0) {
            secondActor->fields288[1] = 1;
        }
        if ((secondPayload->flagsFDC & 0x4000000u) != 0) {
            secondActor->fields288[2] = 1;
        }
        if ((secondPayload->flagsFDC & 0x8000000u) != 0) {
            secondActor->fields288[3] = 1;
        }
        secondPayload->castPlayIndexFC4 = 0;
        secondPayload->castListCA4[0] = 0;
        secondPayload->castListIndexFC8 = -1;
        secondPayload->castListCA4[1] = -1;
        // 第二对象仍从首位置取坐标；位置身份独立写入second。
        secondActor->offset40 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[first]][2]);
        secondActor->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[first]][3]);
        secondActor->x = gBattleGraphicPoints553758[gBattlePointIndices5536FC[first]][2];
        secondActor->y = gBattleGraphicPoints553758[gBattlePointIndices5536FC[first]][3];
        action->scaleX = 1.0F;
        action->scaleY = 1.0F;
        secondPayload->battlePosition18 = second;
        UpdateBattleAlbumFlag_4238F0(secondActor);
        const std::uint32_t secondFlags = secondActor->flags | 2u;
        secondActor->phase = 0;
        secondActor->flags = secondFlags;
        secondActor->callback = nullptr;

        YobiAction* const carrierAction = NewYobiAction_404AE0(2, sizeof(BattleRideCarrierPayload), YobiPayloadKind::BattleRideCarrier);
        existingPayload->effect1060 = carrierAction;
        auto* const carrier = carrierAction->payload.battleRideCarrier;
        carrier->first00 = firstActor;
        carrier->second04 = secondActor;
        carrier->gearImage0C = gearImage;
        AppendBattleCast_4392A0(action, *originalSlot, 226);
        AppendBattleEffect_439400(*originalSlot, {first, 0u, first, second});
        if ((*reinterpret_cast<const unsigned char*>(&firstPayload->flagsFDC) & 2u) != 0) {
            firstActor->value54 = 4.0F;
            AppendBattleCast_4392A0(action, firstActor, 11);
            AppendBattleCast_4392A0(action, firstActor, 40);
            AppendBattleCast_4392A0(action, firstActor, 46);
        } else {
            firstActor->value54 = 5.0F;
            AppendBattleCast_4392A0(action, firstActor, 7);
            AppendBattleCast_4392A0(action, firstActor, 11);
            AppendBattleCast_4392A0(action, firstActor, 10);
        }
        if ((*reinterpret_cast<const unsigned char*>(&secondPayload->flagsFDC) & 2u) != 0) {
            secondActor->value54 = 4.0F;
            AppendBattleCast_4392A0(action, secondActor, 11);
            AppendBattleCast_4392A0(action, secondActor, 40);
            AppendBattleCast_4392A0(action, secondActor, 46);
        } else {
            secondActor->value54 = 5.0F;
            AppendBattleCast_4392A0(action, secondActor, 7);
            AppendBattleCast_4392A0(action, secondActor, 11);
            AppendBattleCast_4392A0(action, secondActor, 10);
        }
        scene->packetOffset64 = previewOffset;
    }
}

// 原函数：0x00438450。
std::int32_t TakeBattleCInteger_438450()
{
    using namespace world::detail;
    std::int32_t offset = gBattleCOffset5ABA80;
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gBattleCText5A3A20[offset]));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            ++offset;
        } else if (hasDigit || value == '|') {
            gBattleCOffset5ABA80 = offset;
            ++offset;
            break;
        } else {
            ++offset;
        }
    }
    gBattleCOffset5ABA80 = offset;
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x004384C0。
void TakeBattleCString_4384C0(char* const destination, const std::int32_t capacity)
{
    using namespace world::detail;
    std::int32_t written = 0;
    for (;;) {
        const char byte = gBattleCText5A3A20[gBattleCOffset5ABA80];
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(byte));
        if (byte == '\0') {
            break;
        }
        if (byte == '|') {
            ++gBattleCOffset5ABA80;
            break;
        }
        if (written >= capacity - 1) {
            if (value < 0) {
                ++gBattleCOffset5ABA80;
            }
            ++gBattleCOffset5ABA80;
            continue;
        }
        destination[written] = byte;
        if (value < 0) {
            const std::int32_t offset = gBattleCOffset5ABA80;
            ++written;
            if (written >= capacity - 1) {
                gBattleCOffset5ABA80 = offset + 1;
                ++gBattleCOffset5ABA80;
                continue;
            }
            const char nextByte = gBattleCText5A3A20[offset + 1];
            gBattleCOffset5ABA80 = offset + 1;
            destination[written] = nextByte;
        }
        ++written;
        ++gBattleCOffset5ABA80;
    }
    destination[written] = '\0';
    input::UnescapeLegacyTextAndNotify_52D2F0(destination);
}

// 原函数：0x00438550。
void ParseBattleC_438550()
{
    using namespace world::detail;
    std::uint32_t present = 0;
    gBattleCOffset5ABA80 = 2;
    const auto fieldFlags = static_cast<std::uint32_t>(TakeBattleCInteger_438450());
    gBattleFieldFlags5ABB58 = fieldFlags;
    if ((fieldFlags & 0x20u) != 0) {
        return;
    }
    if ((fieldFlags & 1u) != 0) {
        static_cast<void>(NewBattleFieldEffect_41FFE0(110092));
    } else if ((fieldFlags & 2u) != 0) {
        static_cast<void>(NewBattleFieldEffect_41FFE0(110089));
    } else if ((fieldFlags & 4u) != 0) {
        static_cast<void>(NewBattleFieldEffect_41FFE0(110090));
    } else if ((fieldFlags & 8u) != 0) {
        static_cast<void>(NewBattleFieldEffect_41FFE0(110091));
    } else if ((fieldFlags & 0x10u) != 0) {
        static_cast<void>(NewBattleFieldEffect_41FFE0(110155));
    } else {
        static_cast<void>(MarkDeath(gBattleSceneAction5ABA94->payload.battleScene->fieldEffectB0));
        gBattleSceneAction5ABA94->payload.battleScene->fieldEffectB0 = nullptr;
    }

    while (gBattleCText5A3A20[gBattleCOffset5ABA80] != '\0') {
        std::int32_t position = TakeBattleCInteger_438450();
        if (position < 0 || position >= 22) {
            position = 0;
        }
        present |= 1u << position;
        YobiAction** const slot = &gBattleAllEntries59E980[position];
        if (*slot == nullptr) {
            *slot = NewBattleActor_438290(position);
            ResetBattleCastList_4391E0(position);
        }
        YobiAction* const actor = *slot;
        auto* const payload = actor->payload.battleActor;
        TakeBattleCString_4384C0(actor->nameC4.data(), 29);
        // 4384C0内部已反转义；原438673仍独立再次执行。
        input::UnescapeLegacyText_52D150(actor->nameC4.data());
        actor->imageNo = character_select::detail::Sub_51B740(TakeBattleCInteger_438450());
        payload->baseImage13AC = character_select::detail::Sub_51B740(TakeBattleCInteger_438450());
        actor->field1D0 = TakeBattleCInteger_438450();
        {
            const std::int32_t value = TakeBattleCInteger_438450();
            actor->readiness = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleCInteger_438450();
            actor->maxHp = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleCInteger_438450();
            actor->barValue1B0 = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleCInteger_438450();
            actor->barMaximum1C0 = static_cast<std::int32_t>(value);
        }
        const auto inputFlags = static_cast<std::uint32_t>(TakeBattleCInteger_438450());
        const std::int32_t ridingImage = character_select::detail::Sub_51B740(TakeBattleCInteger_438450());
        static_cast<void>(SetBattleRider_4222B0(actor, actor->imageNo, ridingImage));
        for (std::int32_t index = 0; index < 2; ++index) {
            const std::int32_t gearImage = character_select::detail::Sub_51B740(TakeBattleCInteger_438450());
            const std::int32_t auxiliaryImage = character_select::detail::Sub_51B740(TakeBattleCInteger_438450());
            const std::int32_t auxiliaryType = TakeBattleCInteger_438450();
            SetBattleGear_422C20(actor, gearImage, index);
            SetBattleAuxGear_422C90(actor, auxiliaryImage, auxiliaryType, index);
        }
        const std::int32_t imageNo = actor->imageNo;
        actor->rebirthValue1 = 0;
        if (imageNo >= 110350 && imageNo <= 110373) {
            actor->rebirthValue1 = 4;
            actor->rebirthValue2 = 4;
        }
        if ((inputFlags & 1u) != 0) {
            payload->flagsFDC &= ~1u;
            actor->offset40 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][0]);
            actor->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][1]);
        } else {
            const std::uint32_t flags = payload->flagsFDC | 1u;
            payload->flagsFDC = flags;
            if ((flags & 2u) == 0) {
                actor->offset40 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][2]);
                actor->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][3]);
                Sub_406260(actor);
                if ((inputFlags & 2u) != 0) {
                    Sub_406260(actor);
                    actor->characterAction = 10;
                    while (PlayFrame_517DA0(actor, 1) != 1) {
                    }
                    static_cast<void>(NewBattleCenteredEffect_41FA00(actor, 110169));
                }
            }
        }
        std::uint32_t deathFlags = payload->flagsFDC;
        std::uint32_t deathState = payload->stateFE0;
        if ((inputFlags & 2u) != 0) {
            deathFlags |= 2u;
            deathState |= 2u;
        } else {
            deathFlags &= ~2u;
            deathState &= ~2u;
        }
        payload->flagsFDC = deathFlags;
        std::uint32_t flags = payload->flagsFDC;
        payload->stateFE0 = deathState;
        flags = (inputFlags & 4u) != 0 ? flags | 4u : flags & ~4u;
        payload->flagsFDC = flags;
        flags = (inputFlags & 8u) != 0 ? flags | 8u : flags & ~8u;
        payload->flagsFDC = flags;
        if ((inputFlags & 0x10u) != 0) {
            static_cast<void>(NewBattleDebuffEffect_41F660(actor, 110075));
        } else {
            payload->flagsFDC &= ~0x10u;
        }
        if ((inputFlags & 0x20u) != 0) {
            static_cast<void>(NewBattleDebuffEffect_41F660(actor, 110076));
        } else {
            payload->flagsFDC &= ~0x20u;
        }
        if ((inputFlags & 0x40u) != 0) {
            static_cast<void>(NewBattleDebuffEffect_41F660(actor, 110077));
        } else {
            payload->flagsFDC &= ~0x40u;
        }
        if ((inputFlags & 0x80u) != 0) {
            static_cast<void>(NewBattleDebuffEffect_41F660(actor, 110078));
        } else {
            payload->flagsFDC &= ~0x80u;
        }
        if ((inputFlags & 0x100u) != 0) {
            static_cast<void>(NewBattleDebuffEffect_41F660(actor, 110079));
        } else {
            payload->flagsFDC &= ~0x100u;
        }
        if ((inputFlags & 0x200u) != 0) {
            static_cast<void>(NewBattleDebuffEffect_41F660(actor, 110080));
        } else {
            payload->flagsFDC &= ~0x200u;
        }
        const float scale = (inputFlags & 0x1000u) != 0 ? 1.3F : 1.0F;
        actor->scaleX = scale;
        actor->scaleY = scale;
        if ((inputFlags & 0x400u) != 0) {
            static_cast<void>(NewBattleAttachedEffect_41FBE0(actor, 1));
        } else {
            static_cast<void>(MarkDeath(actor->payload.battleActor->attachedEffect1058));
            actor->payload.battleActor->attachedEffect1058 = nullptr;
        }
        const std::uint32_t reverseFlags = payload->flagsFDC;
        if ((inputFlags & 0x800u) != 0) {
            payload->flagsFDC = reverseFlags | 0x800u;
            static_cast<void>(NewBattleReverseEffect_41F860(actor, 110152));
        } else {
            YobiAction* const effect = payload->effect105C;
            payload->flagsFDC = reverseFlags & ~0x800u;
            static_cast<void>(MarkDeath(effect));
            payload->effect105C = nullptr;
        }
        if ((inputFlags & 0x2000u) != 0) {
            payload->flagsFDC |= 0x2000u;
        }
        // 三对状态位沿原清除掩码，不能按输入位对称化。
        if ((inputFlags & 0x4000u) != 0) {
            static_cast<void>(NewBattleStatusEffect_41FE00(actor, 103018, 0));
        } else {
            payload->flagsFDC &= ~0x4000u;
        }
        if ((inputFlags & 0x8000u) != 0) {
            static_cast<void>(NewBattleStatusEffect_41FE00(actor, 103019, 0));
        } else {
            payload->flagsFDC &= ~0x4000u;
        }
        if ((inputFlags & 0x10000u) != 0) {
            static_cast<void>(NewBattleStatusEffect_41FE00(actor, 103020, 0));
        } else {
            payload->flagsFDC &= ~0x10000u;
        }
        if ((inputFlags & 0x20000u) != 0) {
            static_cast<void>(NewBattleStatusEffect_41FE00(actor, 103021, 0));
        } else {
            payload->flagsFDC &= ~0x10000u;
        }
        if ((inputFlags & 0x40000u) != 0) {
            static_cast<void>(NewBattleStatusEffect_41FE00(actor, 103022, 0));
        } else {
            payload->flagsFDC &= ~0x40000u;
        }
        if ((inputFlags & 0x80000u) != 0) {
            static_cast<void>(NewBattleStatusEffect_41FE00(actor, 103023, 0));
        } else {
            payload->flagsFDC &= ~0x40000u;
        }
        if ((inputFlags & 0x10000000u) != 0) {
            if (payload->rebirthEffectType1080 == 0) {
                if ((inputFlags & 0x1000000u) != 0) {
                    payload->rebirthEffectType1080 = 1;
                }
                if ((inputFlags & 0x2000000u) != 0) {
                    payload->rebirthEffectType1080 = 2;
                }
                if ((inputFlags & 0x4000000u) != 0) {
                    payload->rebirthEffectType1080 = 3;
                }
                if ((inputFlags & 0x8000000u) != 0) {
                    payload->rebirthEffectType1080 = 4;
                }
                const std::int8_t type = payload->rebirthEffectType1080;
                if (type >= 1 && type <= 4) {
                    {
                        const std::int32_t image = kRebirthPersistentImages4113EB[type - 1][0];
                        if (image != 0) {
                            YobiAction* const effect = NewBattleOffsetFollowEffect_419CF0(actor, 0, 2, image);
                            payload->rebirthEffect106C = effect;
                            effect->rebirthValue1 = 4;
                            payload->rebirthEffect106C->rebirthValue2 = 4;
                        }
                    }
                    {
                        const std::int32_t image = kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][1];
                        if (image != 0) {
                            YobiAction* const effect = NewBattleOffsetFollowEffect_419CF0(actor, 0, 1, image);
                            payload->rebirthAuxiliaryEffects1074[0] = effect;
                            effect->rebirthValue1 = 4;
                            payload->rebirthAuxiliaryEffects1074[0]->rebirthValue2 = 4;
                        }
                    }
                    {
                        const std::int32_t image = kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][2];
                        if (image != 0) {
                            YobiAction* const effect = NewBattleOffsetFollowEffect_419CF0(actor, 0, -1, image);
                            payload->rebirthAuxiliaryEffects1074[1] = effect;
                            effect->rebirthValue1 = 4;
                            payload->rebirthAuxiliaryEffects1074[1]->rebirthValue2 = 4;
                        }
                    }
                    {
                        const std::int32_t image = kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][3];
                        if (image != 0) {
                            YobiAction* const effect = NewBattleOffsetFollowEffect_419CF0(actor, 0, -2, image);
                            payload->rebirthAuxiliaryEffects1074[2] = effect;
                            effect->rebirthValue1 = 4;
                            payload->rebirthAuxiliaryEffects1074[2]->rebirthValue2 = 4;
                        }
                    }
                }
            }
            payload->flagsFDC |= 0x10000000u;
        } else {
            for (std::int32_t index = 0; index < 3; ++index) {
                YobiAction* const effect = payload->rebirthAuxiliaryEffects1074[index];
                if (effect != nullptr) {
                    static_cast<void>(MarkDeath(effect));
                    payload->rebirthAuxiliaryEffects1074[index] = nullptr;
                }
            }
            YobiAction* const effect = payload->rebirthEffect106C;
            if (effect != nullptr) {
                static_cast<void>(MarkDeath(effect));
                payload->rebirthEffect106C = nullptr;
            }
            const std::uint32_t rebirthFlags = payload->flagsFDC & ~0x10000000u;
            payload->rebirthEffectType1080 = 0;
            payload->flagsFDC = rebirthFlags;
        }
        actor->fields288[3] = 0;
        actor->fields288[2] = 0;
        actor->fields288[1] = 0;
        actor->fields288[0] = 0;
        if ((inputFlags & 0x1000000u) != 0) {
            actor->fields288[0] = 1;
        }
        if ((inputFlags & 0x2000000u) != 0) {
            actor->fields288[1] = 1;
        }
        if ((inputFlags & 0x4000000u) != 0) {
            actor->fields288[2] = 1;
        }
        if ((inputFlags & 0x8000000u) != 0) {
            actor->fields288[3] = 1;
        }
        UpdateBattleAlbumFlag_4238F0(actor);
        static_cast<void>(PlayFrame_517DA0(actor, 0));
    }

    for (std::int32_t index = 0; index < 22; ++index) {
        if ((present & (1u << index)) != 0) {
            continue;
        }
        YobiAction** const slot = &gBattleAllEntries59E980[index];
        YobiAction* const actor = *slot;
        if (actor == nullptr) {
            continue;
        }
        auto* const payload = actor->payload.battleActor;
        static_cast<void>(MarkDeath(payload->effect1054));
        static_cast<void>(MarkDeath(payload->attachedEffect1058));
        static_cast<void>(MarkDeath(payload->effect105C));
        static_cast<void>(MarkDeath(payload->effect1060));
        static_cast<void>(MarkDeath(payload->centeredEffect1064));
        static_cast<void>(MarkDeath(payload->effect1068));
        static_cast<void>(MarkDeath(payload->rebirthEffect106C));
        static_cast<void>(MarkDeath(payload->rebirthAuxiliaryEffects1074[0]));
        static_cast<void>(MarkDeath(payload->rebirthAuxiliaryEffects1074[1]));
        static_cast<void>(MarkDeath(payload->rebirthAuxiliaryEffects1074[2]));
        static_cast<void>(MarkDeath(payload->gear13B4));
        static_cast<void>(MarkDeath(payload->auxiliaryGear13B8[0]));
        static_cast<void>(MarkDeath(payload->auxiliaryGear13B8[1]));
        static_cast<void>(MarkDeath(payload->rider13C8));
        static_cast<void>(MarkDeath(*slot));
        *slot = nullptr;
    }
}

// BEMS01：固定入口payload、各次取包fresh场景指针；仅CHT/BST消费，见doc/cg2d_battle_exit_message_scan_spec.md。
// 原函数：0x0043BAA0。
std::int32_t Sub_43BAA0()
{
    using namespace world::detail;
    auto* const payload = gBattleSceneAction5ABA94->payload.battleScene;
    std::array<char, 4> header;
    std::array<char, 256> message;
    for (;;) {
        if (GetBattleMHeader_439070(header.data()) == 1) {
            return 1;
        }
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind != 14 && kind != 20) {
            continue;
        }
        payload->packetOffset64 = gBattleMScanOffset5A8A3C;
        if (kind == 14) { // CHT
            static_cast<void>(GetBattlePosition_4237E0(TakeBattleMInteger_438F70(gBattleSceneAction5ABA94)));
            const std::int32_t color = TakeBattleMInteger_438F70(gBattleSceneAction5ABA94);
            const std::int32_t font = TakeBattleMInteger_438F70(gBattleSceneAction5ABA94);
            payload->messageValueA4 = TakeBattleMInteger_438F70(gBattleSceneAction5ABA94);
            TakeBattleMString_438FE0(gBattleSceneAction5ABA94, message.data(), 256);
            if (platform::gValue554644 != 0) {
                static_cast<void>(audio::PlaySe_52C450(74, 320));
            }
            Sub_44B050(cg::text::FromGbkWide(message.data()), color, font);
        } else { // BST
            const std::int32_t position = GetBattlePosition_4237E0(
                TakeBattleMInteger_438F70(gBattleSceneAction5ABA94));
            const std::int32_t seconds = TakeBattleMInteger_438F70(gBattleSceneAction5ABA94);
            if (position == gBattleCurrentPosition5ABCA4) {
                ConsumeBattlePlayerTimer(seconds);
            }
        }
    }
}

} // namespace detail

} // namespace cg::yobi
