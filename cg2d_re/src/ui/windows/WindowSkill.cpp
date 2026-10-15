#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

std::int32_t gSkillTargetCountCA0004 = 0; // 原全局：0x00CA0004。

std::int32_t gSkillTargetPageC8A974 = 0; // 原全局：0x00C8A974。

std::int32_t gSkillTargetPageCountCA0178 = 0; // 原全局：0x00CA0178。

std::int32_t gSkillSelectedTargetCC4B1C = 0; // 原全局：0x00CC4B1C。

std::int32_t gSkillWindowModeD672AC = 0; // 原全局：0x00D672AC。

std::int32_t gSkillWindowIndexD2EA60 = 0; // 原全局：0x00D2EA60。

static std::int32_t gSkillWindowScrollD2D648 = 0; // 原全局：0x00D2D648。

static std::int32_t gSkillWindowHoverD2F744 = 0; // 原全局：0x00D2F744。

static std::int32_t gSkillRecipeLevelC8B1B4 = 0; // 原全局：0x00C8B1B4。

static std::int32_t gSkillWindowMaximumCA000C = 0; // 原全局：0x00CA000C。

// 前15项为配对行号，末字节D3FACB单独参与479FA9计数门槛，见战斗列表SPEC。
static std::array<std::int8_t, 16> gBattleSkillRowsD3FABC {}; // 原全局：0x00D3FABC。

static SkillWindowSelectionCallback gSkillSelectionCallbackCC4230 = nullptr; // 原全局：0x00CC4230。

static WindowValueCallback gPetSkillSelectionCallbackCC4D14 = nullptr; // 原全局：0x00CC4D14。

std::array<std::array<char, 17>, 10> gSkillTargetNamesCB0550 {}; // 原全局：0x00CB0550。

std::array<std::int32_t, 10> gSkillTargetKindsD2F75C {}; // 原全局：0x00D2F75C。

std::array<const char*, 10> gSkillTargetNameViewsD45F98 {}; // 原全局：0x00D45F98。

std::array<std::uint8_t, 10> gSkillTargetColorsCB1DE4 {}; // 原全局：0x00CB1DE4。

std::uint8_t gSkillTargetPendingC8AF5F = 0; // 原全局：0x00C8AF5F。
namespace {

// ND六行属性使用各自原槽，不能与nFV值记录或ysVc名称表混用。
std::int32_t gSkillStatusCountC8AA94 = 0; // 原全局：0x00C8AA94。

std::array<std::array<char, 17>, 6> gSkillStatusNamesCC4670 {}; // 原全局：0x00CC4670。

std::array<std::int32_t, 6> gSkillStatusLevelsCC31C0 {}; // 原全局：0x00CC31C0。

std::array<std::int32_t, 6> gSkillStatusInjuriesD67600 {}; // 原全局：0x00D67600。

std::array<std::int32_t, 6> gSkillStatusHpCC318C {}; // 原全局：0x00CC318C。

std::array<std::int32_t, 6> gSkillStatusMaxHpD66C1C {}; // 原全局：0x00D66C1C。

std::array<std::int32_t, 6> gSkillStatusFpCC2348 {}; // 原全局：0x00CC2348。

std::array<std::int32_t, 6> gSkillStatusMaxFpD2EA80 {}; // 原全局：0x00D2EA80。

std::array<std::int32_t, 6> gSkillStatusKindsCA0A78 {}; // 原全局：0x00CA0A78。

std::array<std::int32_t, 6> gSkillStatusTargetsCF3FD0 {}; // 原全局：0x00CF3FD0。

std::array<WindowStatusChoiceRecord, 6> gSkillStatusViewsCC4BE8 {}; // 原全局：0x00CC4BE8。

constexpr std::array<std::array<std::int32_t, 2>, 8> kPetCrystalItemRanges {{
    {0x24B9, 0x24C2}, {0x24C3, 0x24CC}, {0x24CD, 0x24D6}, {0x24D7, 0x24E0},
    {0x24E1, 0x24E5}, {0x24E6, 0x24EA}, {0x24EB, 0x24EF}, {0x24F0, 0x24F4},
}};

// 原54F2C8起的15对等级闭区间，晶石等级1对应首项。
constexpr std::array<std::array<std::int32_t, 2>, 15> kPetCrystalLevels54F2C8 {{
    {1, 15}, {9, 22}, {16, 29}, {23, 36}, {30, 43},
    {37, 50}, {44, 57}, {51, 64}, {58, 71}, {65, 78},
    {72, 85}, {79, 92}, {86, 99}, {93, 106}, {100, 1000},
}};

constexpr std::array<std::int32_t, 15> kPetCrystalFp54F340 { // 原常量参考：0x0054F340。
    1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
};
} // namespace


LegacySkillStatusView WindowSkill_BorrowStatusView() noexcept
{
    return {&gSkillStatusCountC8AA94, gSkillStatusNamesCC4670.data(), gSkillStatusLevelsCC31C0.data(),
        gSkillStatusInjuriesD67600.data(), gSkillStatusHpCC318C.data(), gSkillStatusMaxHpD66C1C.data(),
        gSkillStatusFpCC2348.data(), gSkillStatusMaxFpD2EA80.data(), gSkillStatusKindsCA0A78.data(),
        gSkillStatusTargetsCF3FD0.data()};
}

std::int32_t WindowSkill_ReadRequestFlag() noexcept
{
    return std::bit_cast<std::int8_t>(gSkillRequestByteD64E13);
}

void WindowSkill_WriteRequestFlag(const std::uint8_t value) noexcept
{
    gSkillRequestByteD64E13 = value;
}

std::uint32_t WindowSkill_ReadExtraDelay() noexcept
{
    return gSkillExtraDelayCF46C4;
}

void WindowSkill_WriteExtraDelay(const std::uint32_t delay) noexcept
{
    gSkillExtraDelayCF46C4 = delay;
}

// 原函数：0x004797C0。
void WindowSkill_AdjustSpecialHoverCount(const std::int32_t first, const std::int32_t second)
{
    if (gValue5F9754 == 0) {
        return;
    }
    auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if (first == 7) {
        hover[7] = static_cast<unsigned char>(hover[7] + 1);
        hover[second] = static_cast<unsigned char>(hover[second] - 1);
    }
    if (second == 7) {
        hover[7] = static_cast<unsigned char>(hover[7] + 1);
        hover[first] = static_cast<unsigned char>(hover[first] - 1);
    }
}

// 原函数：0x00479820。
std::int32_t WindowSkill_FindSpecialSkill()
{
    for (const auto& pair : world::detail::gSkillPairsF63370) {
        if (pair.used != 0) {
            const auto index = std::bit_cast<std::int16_t>(pair.index);
            if (world::detail::gSkillsF1BAFC[index].skillId == 1009) {
                return 1;
            }
        }
    }
    return 0;
}

// 原函数：0x00479860。
std::int32_t WindowSkill_ReadCrystalLevel()
{
    const auto& item = world::detail::BorrowPlayerItemsFDA8A4()[7];
    if (item.used == 0) {
        return 0;
    }
    const std::int32_t id = item.itemId;
    for (std::int32_t index = 0; index < 8; ++index) {
        const auto& range = kPetCrystalItemRanges[index];
        if (id >= range[0] && id <= range[1]) {
            return id - range[0] + (index < 4 ? 1 : 11);
        }
    }
    return 0;
}

// 原函数：0x00479920。
std::int32_t WindowSkill_ReadCrystalElement()
{
    const auto& item = world::detail::BorrowPlayerItemsFDA8A4()[7];
    if (item.used == 0) {
        return 0;
    }
    const std::int32_t id = item.itemId;
    for (std::int32_t index = 0; index < 8; ++index) {
        const auto& range = kPetCrystalItemRanges[index];
        if (id >= range[0] && id <= range[1]) {
            return index % 4;
        }
    }
    return -1;
}

// 原函数：0x004799D0。
std::int32_t WindowSkill_ReadBattlePetImage()
{
    const std::int32_t index = world::detail::Sub_43C4B0();
    return index < 0 ? 0 : (*world::detail::BorrowPetRuntimeF633F8Storage())[index].imageNo6A0;
}

// 原函数：0x00479A00。
std::int32_t WindowSkill_FindPetTechnique(const std::int32_t petIndex)
{
    const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex];
    const std::int32_t count = pet.value6A4;
    for (std::int32_t index = 0; index < count; ++index) {
        const auto slot = std::bit_cast<std::int16_t>(pet.techniquePairs650[index].index);
        const auto& technique = pet.techniquesD8[slot];
        if (technique.name[0] != '\0') {
            const std::int32_t id = technique.techniqueId;
            if (id >= 40330 && id <= 40332) {
                return 1;
            }
        }
    }
    return 0;
}

// 原函数：0x00479A70。
std::int32_t WindowSkill_CheckPetLevel(const std::int32_t petIndex, const std::int32_t crystalLevel)
{
    const std::int32_t level = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].value08;
    const auto& range = kPetCrystalLevels54F2C8[crystalLevel - 1];
    if (level < range[0]) {
        return 0;
    }
    return level <= range[1] ? 1 : 0;
}

// 原函数：0x00479AB0。
std::int32_t WindowSkill_CheckPetFp()
{
    const std::int32_t index = world::detail::Sub_43C4B0();
    if (index < 0) {
        return 0;
    }
    const std::int32_t level = WindowSkill_ReadCrystalLevel();
    if (level < 1 || level > 15) {
        return 0;
    }
    const std::int32_t required = kPetCrystalFp54F340[level - 1];
    return ((*world::detail::BorrowPetRuntimeF633F8Storage())[index].values0C[2] < static_cast<std::int32_t>(required)) ? 0 : 1;
}

// 原函数：0x00479B00。
std::int32_t WindowSkill_CanUseSpecialSkill()
{
    if (WindowSkill_FindSpecialSkill() == 0) {
        return 0;
    }
    const std::int32_t level = WindowSkill_ReadCrystalLevel();
    if (level < 1) {
        return 0;
    }
    const std::int32_t index = world::detail::Sub_43C4B0();
    if (index < 0 || WindowSkill_FindPetTechnique(index) == 0) {
        return 0;
    }
    const std::int32_t attribute = WindowSkill_ReadCrystalElement();
    if ((*world::detail::BorrowPetRuntimeF633F8Storage())[index].values70[21 + attribute] < 50) {
        return 0;
    }
    return WindowSkill_CheckPetLevel(index, level) != 0 ? 1 : 0;
}

// 原函数：0x00486FE0。
std::int32_t WindowSkill_MatchesCrystalLevel()
{
    const std::int32_t crystalLevel = WindowSkill_ReadCrystalLevel();
    const std::int32_t skill = gUsingSkillIndexD2F32C;
    const std::int32_t level = gUsingSkillLevelCA0AC4;
    return world::detail::gSkillsF1BAFC[skill].techniques3C[level].value90 == crystalLevel ? 1 : 0;
}

// 原函数：0x004872E0。
std::int32_t WindowSkill_SelectAttributeTarget(const std::int32_t index, const char*)
{
    if (index == -1) {
        WindowRuntime_DecrementOperationCount();
        const auto count = static_cast<std::uint8_t>(gValueD67AFD - 1);
        const auto skillCount = static_cast<std::uint8_t>(gValueD67AFE - 1);
        gValueD67AFD = count;
        gValueD67AFE = skillCount;
        if (skillCount == 0 && world::detail::Sub_51BEC0() == 13) {
            static_cast<void>(world::detail::Sub_51BE50(0));
        }
        return 1;
    }
    if (index == -3) {
        const std::int32_t skill = gUsingSkillIndexD2F32C;
        static_cast<void>(network::Sub_511C30(platform::gConnectionSocket, skill));
        return 0;
    }
    if (index == -2) {
        return 1;
    }
    gSkillSendModeC8A970 = 0;
    if (WindowSkill_CanUseSpecialSkill() != 0 && WindowSkill_MatchesCrystalLevel() != 0 && WindowSkill_CheckPetFp() != 0) {
        gSkillSendModeC8A970 = 1;
        gSkillPetImageCB0A18 = WindowSkill_ReadBattlePetImage();
    }
    if (gSkillTargetKindCF4A10 <= 1) {
        gSkillSendModeC8A970 = 0;
    }
    char extra[256];
    extra[0] = '\0';
    std::strcpy(gSkillTargetNameD2F748.data(), gSkillStatusNamesCC4670[index].data());
    // 原509080只有ret；后续三分支均发送此空串，但不清局部其余字节。
    std::int32_t offset = 0;
    if (gValue5F9754 != 0) {
        offset = 100;
    } else if (gSkillSendModeC8A970 != 0) {
        offset = 200;
    }
    const std::int32_t target = gSkillStatusTargetsCF3FD0[index];
    const std::int32_t level = gUsingSkillLevelCA0AC4;
    const std::int32_t skill = gUsingSkillIndexD2F32C;
    const std::uintptr_t socket = platform::gConnectionSocket;
    static_cast<void>(network::Sub_511EF0(socket, skill + offset, level, target, cg::text::FromGbkWide(extra)));
    return 0;
}

// 原函数：0x004874B0。
login::YobiAction* WindowSkill_OpenAttributeTargets()
{
    const std::int32_t count = gSkillStatusCountC8AA94;
    constexpr std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    gSkillSendModeC8A970 = 0;
    for (std::int32_t index = 0; index < count; ++index) {
        auto& view = gSkillStatusViewsCC4BE8[index];
        const std::int32_t level = gSkillStatusLevelsCC31C0[index];
        view.name = gSkillStatusNamesCC4670[index].data();
        view.level = level;
        view.hp = gSkillStatusHpCC318C[index];
        view.maxHp = gSkillStatusMaxHpD66C1C[index];
        view.fp = gSkillStatusFpCC2348[index];
        view.maxFp = gSkillStatusMaxFpD2EA80[index];
        view.injury = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gSkillStatusInjuriesD67600[index]));
        view.color = static_cast<std::uint8_t>(colors[gSkillStatusKindsCA0A78[index]]);
    }
    WindowRuntime_IncrementOperationCount();
    const auto references = static_cast<std::uint8_t>(gValueD67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gValueD67AFE + 1);
    gValueD67AFD = references;
    gValueD67AFE = skillReferences;
    constexpr std::array<std::int32_t, 5> windowIds {11, 12, 45, 46, 13};
    for (const std::int32_t id : windowIds) {
        WindowInstance* const window = gWindowStates[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    return WindowAttributeTarget_Open(gSkillStatusViewsCC4BE8.data(), gSkillStatusCountC8AA94, WindowSkill_SelectAttributeTarget);
}

// 原函数：0x004EF750。
login::YobiAction* WindowSkill_ReceiveAttributeTargets(const char* const text)
{
    const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    const std::int32_t selected = gSkillSelectedTargetCC4B1C;
    const std::int32_t kind = gSkillTargetKindsD2F75C[selected];
    gSkillStatusCountC8AA94 = 0;
    if (kind == 0) {
        std::int32_t count = 0;
        if (gSkillTargetKindCF4A10 != 1) {
            const char* const name = world::detail::BorrowPlayerDisplayText(1);
            const std::size_t size = std::strlen(name) + 1;
            gSkillStatusLevelsCC31C0[0] = world::detail::ReadPlayerLevel();
            std::memcpy(gSkillStatusNamesCC4670[0].data(), name, size);
            gSkillStatusInjuriesD67600[0] = world::detail::ReadPlayerInjury();
            const std::int32_t hp = world::detail::ReadPlayerHpOnce();
            gSkillStatusHpCC318C[gSkillStatusCountC8AA94] = hp;
            const std::int32_t maxHp = world::detail::ReadPlayerMaxHpOnce();
            gSkillStatusMaxHpD66C1C[gSkillStatusCountC8AA94] = maxHp;
            const std::int32_t fp = world::detail::ReadPlayerFpOnce();
            const std::int32_t output = gSkillStatusCountC8AA94;
            gSkillStatusFpCC2348[output] = fp;
            const std::int32_t maxFp = world::detail::ReadPlayerMaxFpOnce();
            count = gSkillStatusCountC8AA94 + 1;
            gSkillStatusMaxFpD2EA80[output] = maxFp;
            gSkillStatusKindsCA0A78[output] = 0;
            gSkillStatusTargetsCF3FD0[output] = 0;
            gSkillStatusCountC8AA94 = count;
        }
        std::int32_t target = 1;
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[slot];
            if (pet.used == 0 || gLoginWindowRuntime.valuesCC1A18[slot] == 3) {
                continue;
            }
            const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            const std::size_t size = std::strlen(name) + 1;
            const std::size_t prefix = size & ~std::size_t{3};
            char* const destination = gSkillStatusNamesCC4670[count].data();
            std::memcpy(destination, name, prefix);
            gSkillStatusInjuriesD67600[count] = pet.values70[25];
            std::memcpy(destination + prefix, name + prefix, size - prefix);
            gSkillStatusLevelsCC31C0[count] = pet.value08;
            const std::int32_t hp = pet.values0C[0].value();
            gSkillStatusHpCC318C[gSkillStatusCountC8AA94] = hp;
            const std::int32_t maxHp = pet.values0C[1].value();
            gSkillStatusMaxHpD66C1C[gSkillStatusCountC8AA94] = maxHp;
            const std::int32_t fp = pet.values0C[2].value();
            const std::int32_t output = gSkillStatusCountC8AA94;
            gSkillStatusFpCC2348[output] = fp;
            const std::int32_t maxFp = pet.values0C[3].value();
            count = gSkillStatusCountC8AA94 + 1;
            gSkillStatusMaxFpD2EA80[output] = maxFp;
            gSkillStatusTargetsCF3FD0[output] = target++;
            gSkillStatusKindsCA0A78[output] = 4;
            gSkillStatusCountC8AA94 = count;
        }
        return WindowSkill_OpenAttributeTargets();
    }

    char name[128];
    std::int32_t field = 1;
    std::int32_t target = 0;
    for (std::int32_t row = 0; row < 6; ++row) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 127, name) == 1) {
            break;
        }
        if (gSkillTargetKindCF4A10 == 1 && row == 0) {
            field += 6;
            ++target;
            continue;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        std::strcpy(gSkillStatusNamesCC4670[gSkillStatusCountC8AA94].data(), name);
        std::int32_t value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusLevelsCC31C0[gSkillStatusCountC8AA94] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusInjuriesD67600[gSkillStatusCountC8AA94] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusHpCC318C[gSkillStatusCountC8AA94] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusMaxHpD66C1C[gSkillStatusCountC8AA94] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        const std::int32_t output = gSkillStatusCountC8AA94;
        gSkillStatusFpCC2348[output] = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gSkillStatusMaxFpD2EA80[output] = value;
        switch (gSkillTargetKindsD2F75C[gSkillSelectedTargetCC4B1C]) {
        case 1: gSkillStatusKindsCA0A78[output] = row == 0 ? 1 : 5; break;
        case 2: gSkillStatusKindsCA0A78[output] = row == 0 ? 2 : 6; break;
        case 3: gSkillStatusKindsCA0A78[output] = 3; break;
        }
        const std::int32_t count = gSkillStatusCountC8AA94 + 1;
        gSkillStatusTargetsCF3FD0[output] = target++;
        gSkillStatusCountC8AA94 = count;
    }
    return WindowSkill_OpenAttributeTargets();
}

// 原函数：0x004EF710。
std::int32_t WindowSkill_ClassifyAttributeSkill(const std::int32_t skillId)
{
    switch (skillId) {
    case 221: return 0;
    case 222: return 1;
    case 223: return 2;
    case 224: return 3;
    default: return -1;
    }
}
namespace {


constexpr std::array<std::int32_t, 18> kSkillListImages54F37C { // 原常量参考：0x0054F37C。
    0x3B59F, 0x3B5A0, 0x3B5A1, 0x3B5A5, 0x3B5A6, 0x3B5A7,
    0x3B5A5, 0x3B5A6, 0x3B5A7, 0x3B5A8, 0x3B5A9, 0x3B5AA,
    0x3B5A2, 0x3B5A3, 0x3B5A4, 0x3B59F, 0x3B5A0, 0x3B5A1,
};

constexpr std::array<std::int32_t, 3> kPetSkillImages54F454 {0x3B5B7, 0x3B5B8, 0x3B5B9}; // 原常量参考：0x0054F454。

constexpr std::array<std::int32_t, 18> kSkillTechniqueImages54F3C4 { // 原常量参考：0x0054F3C4。
    0x3B5AB, 0x3B5AC, 0x3B5AD, 0x3B5B1, 0x3B5B2, 0x3B5B3,
    0x3B5B1, 0x3B5B2, 0x3B5B3, 0x3B5B4, 0x3B5B5, 0x3B5B6,
    0x3B5AE, 0x3B5AF, 0x3B5B0, 0x3B5AB, 0x3B5AC, 0x3B5AD,
};

constexpr std::array<std::int32_t, 18> kSkillRecipeImages54F40C { // 原常量参考：0x0054F40C。
    0x3B5B7, 0x3B5B8, 0x3B5B9, 0x3B5BD, 0x3B5BE, 0x3B5BF,
    0x3B5BD, 0x3B5BE, 0x3B5BF, 0x3B5C0, 0x3B5C1, 0x3B5C2,
    0x3B5BA, 0x3B5BB, 0x3B5BC, 0x3B5B7, 0x3B5B8, 0x3B5B9,
};

constexpr wchar_t kSkillUsable586518[] = // 原常量参考：0x00586518。
    L"使用此一等級的技能。";

constexpr wchar_t kSkillUnusable5864FC[] = // 原常量参考：0x005864FC。
    L"無法使用此一等級的技能。";

constexpr wchar_t kSkillInventoryFull5864E4[] = L"物品欄已滿"; // 原常量参考：0x005864E4。

constexpr wchar_t kSkillRecipeHint586530[] = // 原常量参考：0x00586530。
    L"製作此配方內容的物品。";

constexpr wchar_t kSkillListHint5864C8[] = // 原常量参考：0x005864C8。
    L"打開這個技能的技能視窗。";

constexpr wchar_t kBattleSkillInventoryFull586548[] = // 原常量参考：0x00586548。
    L"由於持有物品滿了，無法使用。";

constexpr wchar_t kBattleSkillNoFp586568[] = // 原常量参考：0x00586568。
    L"由於魔力不足，無法使用此技能。";

constexpr wchar_t kBattleSkillUnusable586588[] = // 原常量参考：0x00586588。
    L"由於能力不足，無法使用此技能。";

constexpr wchar_t kSkillBackToTechniques586470[] = // 原常量参考：0x00586470。
    L"切換回技能等級視窗。";

constexpr wchar_t kSkillBackToList586488[] = // 原常量参考：0x00586488。
    L"切換回技能一覽視窗。";

constexpr wchar_t kPetSkillSkip5865A8[] = L"略過此回合。"; // 原常量参考：0x005865A8。
} // namespace


// 原函数：0x0047B6C0。
void WindowSkill_DrawSkillRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    const std::int32_t pairIndex, const std::int32_t enabled,
    const std::int32_t textLayer, const std::int32_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (world::detail::gSkillPairsF63370[pairIndex].used == 0) {
        return;
    }
    const auto skillIndex = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pairIndex].index);
    const auto* const skill = &world::detail::gSkillsF1BAFC[skillIndex];
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const std::int8_t nameColor = enabled != 0 ? 0 : 7;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, cg::text::FromGbkWide(skill->name.data()).c_str(), 0, 0));
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:2}", skill->level);
    WindowNumber_Draw(x + 155, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    const std::int32_t requiredSlots = std::bit_cast<std::int8_t>(skill->requiredSlots19[0]);
    cg::text::FormatTo(buffer, L"{:2}", requiredSlots);
    WindowNumber_Draw(x + 193, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    const std::int32_t kind = skill->canRebirth;
    const std::int32_t imageNo = kSkillListImages54F37C[kind * 3 + visualState];
    static_cast<void>(login::detail::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), imageNo, 0, &descriptor));
}

// 原函数：0x00479700。
void WindowSkill_HandleDrag(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    if (message != 0) {
        return;
    }
    if ((platform::BorrowLegacyMouseStateStorage()->edgeButtons & 2u) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = static_cast<std::int16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseX.value());
        const auto y = static_cast<std::int16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseY.value());
        static_cast<void>(WindowDrag_StartAtPosition(4, sourceIndex, x, y, WindowSkill_HandleDrag));
    }
    if ((platform::BorrowLegacyMouseStateStorage()->edgeButtons & 1u) != 0) {
        WindowDrag_Cancel();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t y =
        (platform::BorrowLegacyMouseStateStorage()->mouseY - 8);
    const std::int32_t x =
        (platform::BorrowLegacyMouseStateStorage()->mouseX - 103);
    WindowSkill_DrawSkillRow(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y),
        2, sourceIndex, 1, 5, 230, 0x80FFFFFFu);
}

// 原函数：0x004797A0。
std::int32_t WindowSkill_StartDrag(const std::int32_t sourceIndex)
{
    return WindowDrag_Start(4, sourceIndex, WindowSkill_HandleDrag);
}

// 原函数：0x0047B820。
std::int32_t WindowSkill_HandleSkillRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t pairIndex = gSkillWindowScrollD2D648 + index - 17;
    if (world::detail::gSkillPairsF63370[pairIndex].used == 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    std::uint32_t events = static_cast<std::uint32_t>(state);
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 4) {
        events &= ~0x60u;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source != pairIndex && world::detail::gValueDA3B20 == 0) {
            const std::int32_t targetSkill =
                std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pairIndex].index);
            const std::int32_t sourceSkill =
                std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[source].index);
            static_cast<void>(network::Sub_512470(platform::gConnectionSocket, sourceSkill, targetSkill));
            world::detail::gValueDA3B20 = 1;
        }
        WindowDrag_Reset();
    }
    const bool clicked = (events & 2u) != 0;
    if (clicked) {
        if ((events & 0x21u) != 0) {
            WindowSkill_ShowTechniques(gCurrentWindow, pairIndex);
            WindowRuntime_IncrementOperationCount();
            static_cast<void>(WindowDrag_ResetMouse());
            consumed = 1;
        }
    } else if ((events & 1u) != 0 && (events & 4u) != 0 && gValueD67AFD == 0) {
        static_cast<void>(WindowSkill_StartDrag(pairIndex));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((events & 1u) != 0) {
        gHoverTextD68AE8 = kSkillListHint5864C8;
    }
    if ((events & 0x21u) != 0) {
        visualState = 2;
        gSkillWindowHoverD2F744 = pairIndex;
    }
    if (clicked) {
        visualState = 1;
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = &window->elements[index];
    const auto y = static_cast<std::int16_t>(element->y + window->y);
    const auto x = static_cast<std::int16_t>(element->x + window->x);
    WindowSkill_DrawSkillRow(x, y, visualState, pairIndex, 1, 3, 223, 0xFFFFFFFFu);
    return consumed;
}

// 原函数：0x00479B80。
void WindowSkill_ShowSkillList(WindowInstance* const window, const std::int32_t skillIndex)
{
    std::int32_t row = 0;
    while (row < 15
        && std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[row].index) != skillIndex) {
        ++row;
    }
    if (row >= 15) {
        row = 0;
    }
    std::int32_t position = row - 9;
    gSkillWindowModeD672AC = 0;
    gSkillWindowScrollD2D648 = position;
    if (position < 0) {
        position = 0;
        gSkillWindowScrollD2D648 = position;
    }
    if (skillIndex < 0) {
        position = 0;
        gSkillWindowScrollD2D648 = position;
    }
    const std::int32_t oldMaximum = gSkillWindowMaximumCA000C;
    static_cast<void>(WindowScroll_SetPosition(&window->elements[11], oldMaximum, position));
    for (std::int32_t index = 1; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[1].visible = 1;
    window->elements[3].visible = 1;
    std::int32_t count = 0;
    for (std::int32_t index = 0; index < 15; ++index) {
        if (world::detail::gSkillPairsF63370[index].used != 0) {
            ++count;
        }
    }
    gSkillWindowMaximumCA000C = count - 10;
    if (gSkillWindowMaximumCA000C < 0) {
        gSkillWindowMaximumCA000C = 0;
    }
    for (std::int32_t index = 10; index <= 16; ++index) {
        window->elements[index].visible = 1;
    }
    static_cast<WindowGraphicData*>(window->elements[27].data)->imageNo = 0x3BA24;
    for (std::int32_t index = 17; index <= 26; ++index) {
        window->elements[index].mouseCallback = WindowSkill_HandleSkillRow;
    }
}

// 原函数：0x0047B9B0。
void WindowSkill_HandleTechniqueRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    std::int32_t technique, const std::int32_t textLayer,
    const std::int32_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const bool battle = technique >= 100;
    if (battle) {
        technique -= 100;
    }
    const auto* record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    if (record->techniques3C[technique].name[0] == '\0') {
        return;
    }
    std::int8_t nameColor = 0;
    if (record->techniques3C[technique].value8C == 0) {
        nameColor = 7;
    } else if (battle) {
        const std::int32_t cost = record->techniques3C[technique].value7C * record->fpReduce / 100;
        const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
        login::YobiAction* const action = world::detail::gBattleEntries59E980[position];
        if (login::detail::IsYobiActionFpLessThan(action, cost)) {
            nameColor = 6;
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const char* const name =
        world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].techniques3C[technique].name.data();
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, cg::text::FromGbkWide(name).c_str(), 0, 0));
    std::wstring buffer;
    const std::int32_t level =
        world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].techniques3C[technique].value90;
    cg::text::FormatTo(buffer, L"{:2}", level);
    WindowNumber_Draw(x + 155, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    const std::int32_t kind = record->canRebirth;
    if (kind != 1 && kind != 2) {
        const std::int32_t cost = record->techniques3C[technique].value7C * record->fpReduce / 100;
        cg::text::FormatTo(buffer, L"{:3}", cost);
        WindowNumber_Draw(x + 186, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    } else {
        static_cast<void>(render::DrawText_456780(
            static_cast<std::int16_t>(x + 189), static_cast<std::int16_t>(y + 3),
            static_cast<std::uint8_t>(textLayer), 12, nameColor, kWindow12Unknown554590, 0, 0));
    }
    const std::int32_t currentKind = world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].canRebirth;
    const std::int32_t imageNo = kSkillTechniqueImages54F3C4[currentKind * 3 + visualState];
    static_cast<void>(login::detail::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), imageNo, 0, &descriptor));
}

// 原函数：0x0047C240。
void WindowSkill_HandleRecipeRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    const std::int32_t recipe, const std::int32_t textLayer,
    const std::int32_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].recipes8E8[recipe].itemId < 0) {
        return;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const auto* const row = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].recipes8E8[recipe];
    const std::int8_t nameColor = row->value0C != 0 ? 0 : 7;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, cg::text::FromGbkWide(row->name.data()).c_str(), 0, 0));
    const auto* const record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    const std::int32_t cost = record->recipes8E8[recipe].value04 * record->fpReduce / 100;
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", cost);
    WindowNumber_Draw(x + 186, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    const std::int32_t kind = world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].canRebirth;
    const std::int32_t imageNo = kSkillRecipeImages54F40C[kind * 3 + visualState];
    static_cast<void>(login::detail::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), imageNo, 0, &descriptor));
}

// 原函数：0x00479C80。
void WindowSkill_ShowTechniques(WindowInstance* const window, const std::int32_t pairIndex)
{
    gSkillWindowModeD672AC = 1;
    gSkillWindowScrollD2D648 = 0;
    if (pairIndex >= 0) {
        gSkillWindowIndexD2EA60 = std::bit_cast<std::int16_t>(
            world::detail::gSkillPairsF63370[pairIndex].index);
    }
    const std::int32_t oldMaximum = gSkillWindowMaximumCA000C;
    static_cast<void>(WindowScroll_SetPosition(&window->elements[11], oldMaximum, 0));
    for (std::int32_t index = 1; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[2].visible = 1;
    window->elements[4].visible = 1;
    window->elements[5].visible = 1;
    window->elements[9].visible = 1;
    window->elements[6].visible = 0;
    const auto* const techniques =
        world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].techniques3C.data();
    std::int32_t count = 0;
    while (count < 15 && techniques[count].name[0] != '\0') {
        ++count;
    }
    gSkillWindowMaximumCA000C = count - 10;
    if (gSkillWindowMaximumCA000C < 0) {
        gSkillWindowMaximumCA000C = 0;
    }
    for (std::int32_t index = 10; index <= 16; ++index) {
        window->elements[index].visible = 1;
    }
    window->elements[14].visible = 0;
    window->elements[15].visible = 0;
    static_cast<WindowGraphicData*>(window->elements[27].data)->imageNo = 0x3BA26;
    for (std::int32_t index = 17; index <= 26; ++index) {
        window->elements[index].mouseCallback = WindowSkill_SelectSkillType;
    }
}

// 原函数：0x0047B0E0。
void WindowSkill_Release()
{
    const std::int32_t mode = gSkillWindowModeD672AC;
    if (mode == 1 || mode == 2) {
        WindowRuntime_DecrementOperationCount();
        if (gSkillWindowModeD672AC == 2) {
            WindowRuntime_DecrementGatherOperationCount();
            --gValueD67AFD;
        }
    }
}

// 原函数：0x00479EA0。
void WindowSkill_ShowBattleSkills(WindowInstance* const window, const std::int32_t skillIndex)
{
    std::int32_t row = 0;
    while (row < 15
        && std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[row].index) != skillIndex) {
        ++row;
    }
    if (row >= 15) {
        row = 0;
    }
    gSkillWindowModeD672AC = 3;
    gSkillWindowScrollD2D648 = row - 9;
    if (gSkillWindowScrollD2D648 < 0) {
        gSkillWindowScrollD2D648 = 0;
    }
    if (skillIndex < 0) {
        gSkillWindowScrollD2D648 = 0;
    }
    for (std::int32_t index = 1; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[1].visible = 1;
    window->elements[3].visible = 1;
    for (std::int32_t index = 10; index <= 16; ++index) {
        window->elements[index].visible = 1;
    }
    static_cast<WindowGraphicData*>(window->elements[27].data)->imageNo = 0x3BA24;
    std::int32_t end = 0;
    for (std::int32_t index = 0; index < 15; ++index) {
        if (world::detail::gSkillPairsF63370[index].used != 0) {
            const auto skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[index].index);
            const std::int32_t kind = world::detail::gSkillsF1BAFC[skill].canRebirth;
            if (kind == 0 || kind == 5) {
                gBattleSkillRowsD3FABC[end++] = static_cast<std::int8_t>(index);
            }
        }
    }
    while (end < 15) {
        gBattleSkillRowsD3FABC[end++] = -1;
    }
    // 原479FA9固定读取末尾字节；479FBE允许配对-1别名读取末条技能记录的U16。
    const std::int8_t countGate = gBattleSkillRowsD3FABC[end];
    std::int32_t count = 0;
    for (std::int32_t index = 0; index < 15; ++index) {
        if (countGate >= 0 && world::detail::ReadSkillPairUsed(gBattleSkillRowsD3FABC[index]) != 0) {
            ++count;
        }
    }
    gSkillWindowMaximumCA000C = count - 10;
    if (gSkillWindowMaximumCA000C < 0) {
        gSkillWindowMaximumCA000C = 0;
    }
    for (std::int32_t index = 17; index <= 26; ++index) {
        window->elements[index].mouseCallback = WindowSkill_HandleBattleSkillRow;
    }
}

// 原函数：0x0047A000。
void WindowSkill_ShowBattleTechniques(
    WindowInstance* const window, const std::int32_t index, const std::uint8_t directSkill)
{
    gSkillWindowModeD672AC = 4;
    gSkillWindowScrollD2D648 = 0;
    if (directSkill != 0) {
        gSkillWindowIndexD2EA60 = index;
    } else if (index >= 0) {
        gSkillWindowIndexD2EA60 = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[index].index);
    }
    const std::int32_t oldMaximum = gSkillWindowMaximumCA000C;
    static_cast<void>(WindowScroll_SetPosition(&window->elements[11], oldMaximum, 0));
    for (std::int32_t element = 1; element <= 16; ++element) {
        window->elements[element].visible = 0;
    }
    window->elements[2].visible = 1;
    window->elements[4].visible = 1;
    window->elements[5].visible = 1;
    window->elements[9].visible = 1;
    std::int32_t selectedSkill = gSkillWindowIndexD2EA60;
    if (world::detail::gSkillsF1BAFC[selectedSkill].canRebirth != 0) {
        window->elements[6].visible = 1;
        selectedSkill = gSkillWindowIndexD2EA60;
    }
    const auto* const techniques =
        world::detail::gSkillsF1BAFC[selectedSkill].techniques3C.data();
    std::int32_t count = 0;
    while (count < 15 && techniques[count].name[0] != '\0') {
        ++count;
    }
    gSkillWindowMaximumCA000C = count - 10;
    if (gSkillWindowMaximumCA000C < 0) {
        gSkillWindowMaximumCA000C = 0;
    }
    for (std::int32_t element = 10; element <= 16; ++element) {
        window->elements[element].visible = 1;
    }
    window->elements[14].visible = 0;
    window->elements[15].visible = 0;
    static_cast<WindowGraphicData*>(window->elements[27].data)->imageNo = 0x3BA26;
    for (std::int32_t element = 17; element <= 26; ++element) {
        window->elements[element].mouseCallback = WindowSkill_SelectBattleTechnique;
    }
}

// 原函数：0x0047C570。
std::int32_t WindowSkill_HandleBattleSkillRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t pairIndex = gBattleSkillRowsD3FABC[gSkillWindowScrollD2D648 + index - 17];
    if (pairIndex < 0 || world::detail::gSkillPairsF63370[pairIndex].used == 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    const bool clicked = (state & 2) != 0;
    if (clicked) {
        const std::int32_t skillIndex =
            std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pairIndex].index);
        const std::uint32_t flags = world::detail::gBattleSkillFlags5ABC04;
        const std::uint32_t mask = 1u << (static_cast<std::uint32_t>(skillIndex) & 31u);
        if ((flags & mask) != 0) {
            WindowSkill_ShowBattleTechniques(gCurrentWindow, pairIndex, 0);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        } else {
            if (world::detail::gSkillsF1BAFC[skillIndex].skillId == 72
                && world::detail::Sub_518C60() == 0) {
                world::detail::Sub_44B050(kBattleSkillInventoryFull586548, 4, 3);
            }
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    const std::uint32_t flags = world::detail::gBattleSkillFlags5ABC04;
    if ((state & 1) != 0) {
        const auto skillIndex = *reinterpret_cast<const std::uint8_t*>(
            &world::detail::gSkillPairsF63370[pairIndex].index);
        visualState = 2;
        gSkillWindowHoverD2F744 = pairIndex;
        if ((flags & (1u << (skillIndex & 31u))) != 0) {
            gHoverTextD68AE8 = kSkillListHint5864C8;
        }
    }
    if (clicked) {
        visualState = 1;
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = &window->elements[index];
    const auto skillIndex = *reinterpret_cast<const std::uint8_t*>(
        &world::detail::gSkillPairsF63370[pairIndex].index);
    const std::int32_t enabled = (flags & (1u << (skillIndex & 31u))) != 0 ? 1 : 0;
    // 原47C6DA..47C6E6依次读取元素Y/X、窗口Y/X低W16；共享坐标仍为W32。
    std::uint16_t elementY;
    std::memcpy(&elementY, &element->y, sizeof(elementY));
    std::uint16_t elementX;
    std::memcpy(&elementX, &element->x, sizeof(elementX));
    std::uint16_t windowY;
    std::memcpy(&windowY, &window->y, sizeof(windowY));
    const auto y = static_cast<std::int16_t>(
        std::bit_cast<std::int16_t>(elementY) + std::bit_cast<std::int16_t>(windowY));
    std::uint16_t windowX;
    std::memcpy(&windowX, &window->x, sizeof(windowX));
    const auto x = static_cast<std::int16_t>(
        std::bit_cast<std::int16_t>(elementX) + std::bit_cast<std::int16_t>(windowX));
    WindowSkill_DrawSkillRow(x, y, visualState, pairIndex, enabled, 3, 223, 0xFFFFFFFFu);
    return consumed;
}

// 原函数：0x0047C700。
std::int32_t WindowSkill_SelectBattleTechnique(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t technique = gSkillWindowScrollD2D648 + index - 17;
    const auto* record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    if (record->techniques3C[technique].name[0] == '\0') {
        return 0;
    }
    const std::int32_t techniqueCost = record->techniques3C[technique].value7C;
    const std::int32_t fpReduce = record->fpReduce;
    const std::int32_t cost = techniqueCost * fpReduce / 100;
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    bool allowed = record->techniques3C[technique].value8C != 0;
    if (allowed) {
        const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
        allowed = login::detail::IsYobiActionFpGreaterEqual(world::detail::gBattleEntries59E980[position], cost);
    }
    const bool clicked = (state & 2) != 0;
    if (clicked) {
        if (allowed) {
            const SkillWindowSelectionCallback callback = gSkillSelectionCallbackCC4230;
            if (callback != nullptr) {
                callback(gSkillWindowIndexD2EA60, technique);
            }
            gCurrentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if ((*reinterpret_cast<const std::uint8_t*>(&state) & 1u) != 0) {
        record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        visualState = 2;
        gSkillWindowHoverD2F744 = technique;
        const wchar_t* hint = kBattleSkillUnusable586588;
        if (record->techniques3C[technique].value8C != 0) {
            const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
            hint = login::detail::IsYobiActionFpLessThan(world::detail::gBattleEntries59E980[position], cost)
                ? kBattleSkillNoFp586568 : kSkillUsable586518;
        }
        gHoverTextD68AE8 = hint;
    }
    if (clicked && world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].techniques3C[technique].value8C != 0) {
        const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
        if (login::detail::IsYobiActionFpGreaterEqual(world::detail::gBattleEntries59E980[position], cost)) {
            visualState = 1;
        }
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = &window->elements[index];
    // 原47C91F..47C92B依次读取元素Y/X、窗口Y/X低W16；共享坐标仍为W32。
    std::uint16_t elementY;
    std::memcpy(&elementY, &element->y, sizeof(elementY));
    std::uint16_t elementX;
    std::memcpy(&elementX, &element->x, sizeof(elementX));
    std::uint16_t windowY;
    std::memcpy(&windowY, &window->y, sizeof(windowY));
    const auto y = static_cast<std::int16_t>(
        std::bit_cast<std::int16_t>(elementY) + std::bit_cast<std::int16_t>(windowY));
    std::uint16_t windowX;
    std::memcpy(&windowX, &window->x, sizeof(windowX));
    const auto x = static_cast<std::int16_t>(
        std::bit_cast<std::int16_t>(elementX) + std::bit_cast<std::int16_t>(windowX));
    WindowSkill_HandleTechniqueRow(x, y, visualState, technique + 100, 3, 223, 0xFFFFFFFFu);
    return consumed;
}

// 原函数：0x0047A490。
login::YobiAction* WindowSkill_Open()
{
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    const bool hasWindow = window != nullptr;
    gSkillSelectionCallbackCC4230 = nullptr;
    gSkillWindowModeD672AC = 0;
    if (hasWindow) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047A4C0。
login::YobiAction* WindowSkill_OpenBattleSkills(const SkillWindowSelectionCallback callback)
{
    gSkillWindowModeD672AC = 3;
    gSkillSelectionCallbackCC4230 = callback;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047A500。
login::YobiAction* WindowSkill_OpenBattleTechniques(
    const std::int32_t skillIndex, const SkillWindowSelectionCallback callback)
{
    gSkillSelectionCallbackCC4230 = callback;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    gSkillWindowModeD672AC = 4;
    gSkillWindowIndexD2EA60 = skillIndex;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047CB00。
void WindowSkill_DrawPetSkillRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    const std::int32_t pairIndex, const std::int32_t textLayer,
    const std::int32_t graphicLayer, const std::uint32_t color)
{
    const std::int32_t petIndex = world::detail::Sub_43C4B0();
    const std::int32_t pairPetIndex = world::detail::Sub_43C4B0();
    const auto technique = std::bit_cast<std::int16_t>(
        (*world::detail::BorrowPetRuntimeF633F8Storage())[pairPetIndex].techniquePairs650[pairIndex].index);
    render::GraphicDescriptor16 descriptor {};
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const auto* const record = &(*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].techniquesD8[technique];
    if (record->name[0] == '\0') {
        return;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const std::int32_t position = world::detail::GetBattlePetPosition_405B40(
        world::detail::gBattleCurrentPosition5ABCA4);
    login::YobiAction* const action = world::detail::gBattleEntries59E980[position];
    const std::int32_t cost = record->fp;
    std::int8_t nameColor = login::detail::IsCostGreaterThanYobiActionFp(cost, action) ? 6 : 0;
    const std::uint32_t flags = world::detail::gBattlePetSkillFlags5ABB78;
    if ((flags & (1u << (static_cast<std::uint32_t>(pairIndex) & 31u))) == 0) {
        nameColor = 7;
    }
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, cg::text::FromGbkWide(record->name.data()).c_str(), 0, 0));
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", record->fp);
    WindowNumber_Draw(x + 186, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    static_cast<void>(login::detail::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), kPetSkillImages54F454[visualState], 0, &descriptor));
}

// 原函数：0x0047CE70。
void WindowSkill_DrawRidingSkillRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t visualState,
    const std::int32_t pairIndex, const std::int32_t textLayer,
    const std::int32_t graphicLayer, const std::uint32_t color)
{
    const auto petIndex = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
    render::GraphicDescriptor16 descriptor {};
    descriptor.bytes[13] = 4;
    const auto technique = std::bit_cast<std::int16_t>(
        (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].techniquePairs650[pairIndex].index);
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    const auto* const record = &(*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].techniquesD8[technique];
    if (record->name[0] == '\0') {
        return;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(static_cast<std::int8_t>(textLayer)));
    static_cast<void>(render::DrawText_456780(
        0, 0, static_cast<std::uint8_t>(textLayer), 12, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
    const std::int32_t cost = record->fp;
    login::YobiAction* const action = world::detail::gBattleEntries59E980[position];
    std::int8_t nameColor = login::detail::IsCostGreaterThanYobiActionFp(cost, action) ? 6 : 0;
    const std::uint32_t flags = world::detail::gBattlePetSkillFlags5ABB78;
    if ((flags & (1u << (static_cast<std::uint32_t>(pairIndex) & 31u))) == 0) {
        nameColor = 7;
    }
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(x + 7), static_cast<std::int16_t>(y + 2),
        static_cast<std::uint8_t>(textLayer), 12, nameColor, cg::text::FromGbkWide(record->name.data()).c_str(), 0, 0));
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", record->fp);
    WindowNumber_Draw(x + 186, y + 3, buffer, 0, 0, 0, static_cast<std::uint8_t>(graphicLayer));
    static_cast<void>(login::detail::DrawGraphic(
        x + 105, y + 8, static_cast<std::uint8_t>(graphicLayer), kPetSkillImages54F454[visualState], 0, &descriptor));
}

// 原函数：0x0047CC80。
std::int32_t WindowSkill_SelectPetSkill(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t pairIndex = index - 17;
    std::int32_t pairPet = world::detail::Sub_43C4B0();
    auto technique = std::bit_cast<std::int16_t>(
        (*world::detail::BorrowPetRuntimeF633F8Storage())[pairPet].techniquePairs650[pairIndex].index);
    const std::int32_t namePet = world::detail::Sub_43C4B0();
    if ((*world::detail::BorrowPetRuntimeF633F8Storage())[namePet].techniquesD8[technique].name[0] == '\0') {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    const std::uint32_t flags = world::detail::gBattlePetSkillFlags5ABB78;
    if ((flags & (1u << (static_cast<std::uint32_t>(pairIndex) & 31u))) != 0) {
        pairPet = world::detail::Sub_43C4B0();
        technique = std::bit_cast<std::int16_t>(
            (*world::detail::BorrowPetRuntimeF633F8Storage())[pairPet].techniquePairs650[pairIndex].index);
        const std::int32_t costPet = world::detail::Sub_43C4B0();
        const std::int32_t playerPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t cost = (*world::detail::BorrowPetRuntimeF633F8Storage())[costPet].techniquesD8[technique].fp;
        const std::int32_t position = world::detail::GetBattlePetPosition_405B40(playerPosition);
        if (login::detail::IsYobiActionFpGreaterEqual(world::detail::gBattleEntries59E980[position], cost)
            && (reinterpret_cast<const std::uint8_t*>(&state)[0] & 2u) != 0) {
            if (gPetSkillSelectionCallbackCC4D14 != nullptr) {
                const std::int32_t pet = world::detail::Sub_43C4B0();
                const auto selected = std::bit_cast<std::int16_t>(
                    (*world::detail::BorrowPetRuntimeF633F8Storage())[pet].techniquePairs650[pairIndex].index);
                gPetSkillSelectionCallbackCC4D14(selected);
            }
            gCurrentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    } else if ((reinterpret_cast<const std::uint8_t*>(&state)[0] & 2u) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    const std::uint8_t visualFlags = reinterpret_cast<const std::uint8_t*>(&state)[0];
    if ((visualFlags & 1u) != 0) {
        visualState = 2;
        gSkillWindowHoverD2F744 = pairIndex;
        gHoverTextD68AE8 = kSkillUsable586518;
    }
    if ((visualFlags & 2u) != 0) {
        visualState = 1;
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = &window->elements[index];
    // 原坐标按element Y/X、window Y/X取低W16，避免宽指针别名。
    std::uint16_t elementYBits;
    std::uint16_t elementXBits;
    std::uint16_t windowYBits;
    std::uint16_t windowXBits;
    std::memcpy(&elementYBits, &element->y, sizeof(elementYBits));
    std::memcpy(&elementXBits, &element->x, sizeof(elementXBits));
    std::memcpy(&windowYBits, &window->y, sizeof(windowYBits));
    std::memcpy(&windowXBits, &window->x, sizeof(windowXBits));
    const auto y = static_cast<std::int16_t>(
        std::bit_cast<std::int16_t>(elementYBits) + std::bit_cast<std::int16_t>(windowYBits));
    const auto x = static_cast<std::int16_t>(
        std::bit_cast<std::int16_t>(elementXBits) + std::bit_cast<std::int16_t>(windowXBits));
    WindowSkill_DrawPetSkillRow(x, y, visualState, pairIndex, 3, 223, 0xFFFFFFFFu);
    return consumed;
}

// 原函数：0x0047CFE0。
std::int32_t WindowSkill_SelectRidingSkill(const std::int32_t index, const std::int32_t state)
{
    const auto petIndex = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
    const std::int32_t pairIndex = index - 17;
    const auto technique = std::bit_cast<std::int16_t>(
        (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].techniquePairs650[pairIndex].index);
    const auto* const record = &(*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].techniquesD8[technique];
    if (record->name[0] == '\0') {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    if ((world::detail::gBattlePetSkillFlags5ABB78 & (1u << (static_cast<std::uint32_t>(pairIndex) & 31u))) != 0) {
        const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t cost = record->fp;
        if (login::detail::IsYobiActionFpGreaterEqual(world::detail::gBattleEntries59E980[position], cost)
            && (reinterpret_cast<const std::uint8_t*>(&state)[0] & 2u) != 0) {
            const WindowValueCallback callback = gPetSkillSelectionCallbackCC4D14;
            if (callback != nullptr) {
                const auto pet = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
                const auto selected = std::bit_cast<std::int16_t>(
                    (*world::detail::BorrowPetRuntimeF633F8Storage())[pet].techniquePairs650[pairIndex].index);
                callback(selected);
            }
            gCurrentWindow->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    } else if ((reinterpret_cast<const std::uint8_t*>(&state)[0] & 2u) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    const std::uint8_t visualFlags = reinterpret_cast<const std::uint8_t*>(&state)[0];
    if ((visualFlags & 1u) != 0) {
        visualState = 2;
        gSkillWindowHoverD2F744 = pairIndex;
        gHoverTextD68AE8 = kSkillUsable586518;
    }
    if ((visualFlags & 2u) != 0) {
        visualState = 1;
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = &window->elements[index];
    // 原坐标按element Y/X、window Y/X取低W16，避免宽指针别名。
    std::uint16_t elementYBits;
    std::uint16_t elementXBits;
    std::uint16_t windowYBits;
    std::uint16_t windowXBits;
    std::memcpy(&elementYBits, &element->y, sizeof(elementYBits));
    std::memcpy(&elementXBits, &element->x, sizeof(elementXBits));
    std::memcpy(&windowYBits, &window->y, sizeof(windowYBits));
    std::memcpy(&windowXBits, &window->x, sizeof(windowXBits));
    const auto y = static_cast<std::int16_t>(
        std::bit_cast<std::int16_t>(elementYBits) + std::bit_cast<std::int16_t>(windowYBits));
    const auto x = static_cast<std::int16_t>(
        std::bit_cast<std::int16_t>(elementXBits) + std::bit_cast<std::int16_t>(windowXBits));
    WindowSkill_DrawRidingSkillRow(x, y, visualState, pairIndex, 3, 223, 0xFFFFFFFFu);
    return consumed;
}

// 原函数：0x0047A1A0。
void WindowSkill_ShowPetSkills(WindowInstance* const window)
{
    gSkillWindowModeD672AC = 5;
    gSkillWindowScrollD2D648 = 0;
    for (std::int32_t index = 1; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[2].visible = 1;
    gSkillWindowMaximumCA000C = 0;
    for (std::int32_t index = 10; index <= 16; ++index) {
        window->elements[index].visible = 1;
    }
    const std::int32_t petIndex = world::detail::Sub_43C4B0();
    if (petIndex < 0) {
        return;
    }
    const auto* const pet = &(*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex];
    const char* name = pet->name6BD.data();
    if (name[0] == '\0') {
        name = pet->name6AC.data();
    }
    static_cast<WindowTextData*>(window->elements[2].data)->text = cg::text::FromGbkWide(name);
    std::int32_t index = 0;
    std::int32_t count = pet->value6A4;
    while (index < count) {
        const auto technique = std::bit_cast<std::int16_t>(pet->techniquePairs650[index].index);
        const auto* const record = &pet->techniquesD8[technique];
        if (record->name[0] != '\0') {
            const std::int32_t position = world::detail::GetBattlePetPosition_405B40(
                world::detail::gBattleCurrentPosition5ABCA4);
            login::YobiAction* const action = world::detail::gBattleEntries59E980[position];
            const std::int32_t cost = record->fp;
            static_cast<void>(login::detail::IsCostGreaterThanYobiActionFp(cost, action));
        }
        count = pet->value6A4;
        ++index;
    }
    window->elements[8].visible = 1;
    gSkillWindowMaximumCA000C = pet->value6A4 - 10;
    window->elements[7].visible = 1;
    static_cast<WindowGraphicData*>(window->elements[27].data)->imageNo = 0x3BA28;
    for (std::int32_t elementIndex = 17; elementIndex <= 26; ++elementIndex) {
        window->elements[elementIndex].mouseCallback = WindowSkill_SelectPetSkill;
    }
}

// 原函数：0x0047A320。
void WindowSkill_ShowRidingSkills(WindowInstance* const window)
{
    gSkillWindowModeD672AC = 6;
    gSkillWindowScrollD2D648 = 0;
    for (std::int32_t index = 1; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[2].visible = 1;
    gSkillWindowMaximumCA000C = 0;
    for (std::int32_t index = 10; index <= 16; ++index) {
        window->elements[index].visible = 1;
    }
    const auto petIndex = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
    if (petIndex < 0) {
        return;
    }
    const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
    const WindowElement* const elements = window->elements;
    login::YobiAction* const displayAction = world::detail::gBattleEntries59E980[position];
    auto* const textData = static_cast<WindowTextData*>(elements[2].data);
    const char* const name = login::detail::BorrowYobiNameC4(displayAction);
    textData->text = cg::text::FromGbkWide(name);
    const auto* const pet = &(*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex];
    std::int32_t index = 0;
    std::int32_t count = pet->value6A4;
    while (index < count) {
        const auto technique = std::bit_cast<std::int16_t>(pet->techniquePairs650[index].index);
        const auto* const record = &pet->techniquesD8[technique];
        if (record->name[0] != '\0') {
            const std::int32_t currentPosition = world::detail::gBattleCurrentPosition5ABCA4;
            login::YobiAction* const action = world::detail::gBattleEntries59E980[currentPosition];
            const std::int32_t cost = record->fp;
            static_cast<void>(login::detail::IsCostGreaterThanYobiActionFp(cost, action));
        }
        count = pet->value6A4;
        ++index;
    }
    window->elements[8].visible = 1;
    gSkillWindowMaximumCA000C = pet->value6A4 - 10;
    window->elements[7].visible = 1;
    static_cast<WindowGraphicData*>(window->elements[27].data)->imageNo = 0x3BA28;
    for (std::int32_t elementIndex = 17; elementIndex <= 26; ++elementIndex) {
        window->elements[elementIndex].mouseCallback = WindowSkill_SelectRidingSkill;
    }
}

// 原函数：0x0047A540。
login::YobiAction* WindowSkill_OpenPetSkills(const WindowValueCallback callback)
{
    gSkillWindowModeD672AC = 5;
    gPetSkillSelectionCallbackCC4D14 = callback;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047A580。
login::YobiAction* WindowSkill_OpenRidingSkills(const WindowValueCallback callback)
{
    gSkillWindowModeD672AC = 6;
    gPetSkillSelectionCallbackCC4D14 = callback;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    }
    return WindowRuntime_CreateAnimation(WindowId_Skill, 1, 0, nullptr);
}

// 原函数：0x0047B110。
std::int32_t WindowSkill_ButtonBack(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = &window->elements[index];
    const bool hovered = (state & 1) != 0;
    auto* const graphic = static_cast<WindowGraphicData*>(element->data);
    std::int32_t consumed = 0;
    if (hovered) {
        const std::int32_t mode = gSkillWindowModeD672AC;
        if (mode == 1 || mode == 4) {
            gHoverTextD68AE8 = kSkillBackToList586488;
        } else if (mode == 2) {
            gHoverTextD68AE8 = kSkillBackToTechniques586470;
        }
    }
    if ((*reinterpret_cast<const std::uint8_t*>(&state) & 2u) != 0) {
        switch (gSkillWindowModeD672AC) {
        case 1:
            WindowSkill_ShowSkillList(window, gSkillWindowIndexD2EA60);
            WindowRuntime_DecrementOperationCount();
            break;
        case 2:
            WindowSkill_ShowTechniques(window, -1);
            WindowRuntime_DecrementGatherOperationCount();
            --gValueD67AFD;
            break;
        case 4:
            WindowSkill_ShowBattleSkills(window, gSkillWindowIndexD2EA60);
            break;
        default:
            break;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B571;
    if (hovered) {
        graphic->imageNo = 0x3B573;
    }
    if ((*reinterpret_cast<const std::uint8_t*>(&state) & 8u) != 0) {
        graphic->imageNo = 0x3B572;
    }
    return consumed;
}

// 原函数：0x0047A5C0。
std::int32_t WindowSkill_Back()
{
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    if (window == nullptr) {
        return 0;
    }
    switch (gSkillWindowModeD672AC) {
    case 0:
    case 3:
    case 5:
    case 6:
        window->state |= 0x8000u;
        return 1;
    case 4:
        WindowSkill_ShowBattleSkills(window, gSkillWindowIndexD2EA60);
        return 0;
    default:
        return 0;
    }
}

// TUC01：初始元素→单字节动作标志→图形；持有 hover，Y/X发送后按原序发布转生/占用。
// 原函数：0x0047B210。
std::int32_t WindowSkill_ButtonUse(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const initialWindow = gCurrentWindow;
    WindowElement* const elements = initialWindow->elements;
    const std::uint8_t actionFlags = world::detail::ReadPlayerActionFlagsByte();
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    if ((actionFlags & 0x80u) != 0
        || world::detail::Sub_440B50() != 0
        || world::detail::ReadPlayerValue08() == 0
        || world::detail::ReadPlayerTimerRemaining0C() <= 0
        || world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].canRebirth == 0) {
        gCurrentWindow->elements[index].visible = 0;
        return 0;
    }
    const std::uint8_t rebirth = gValue5F9754;
    const bool hover = (state & 1) != 0;
    if (hover) {
        gHoverTextD68AE8 = rebirth != 0
            ? kSkillDisableRebirth5864B4 : kSkillEnableRebirth5864A0;
    }
    std::int32_t consumed = 0;
    if (gValueD67AFC != 0) {
        if ((state & 2) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((state & 2) != 0) {
        const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(network::Sub_511DD0(platform::gConnectionSocket, x, y, rebirth != 0 ? 62 : 61));
        auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
        const std::uint8_t previous = occupancy[7];
        gValue5F9754 = rebirth != 0 ? 0 : 1;
        occupancy[7] = static_cast<std::uint8_t>(previous + (rebirth != 0 ? -1 : 1));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B57A;
    if (hover) {
        graphic->imageNo = 0x3B57C;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B57B;
    }
    if (gValueD67AFC != 0) {
        graphic->imageNo = 0x3B57B;
    }
    return consumed;
}

// 原函数：0x0047C950。
std::int32_t WindowSkill_ButtonPetSkillBack(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t mode = gSkillWindowModeD672AC;
    std::int32_t petIndex;
    std::int32_t position; // 原仅mode5/6写入；其他模式未初始化路径不模拟，见SPEC。
    if (mode == 5) {
        petIndex = world::detail::Sub_43C4B0();
        position = world::detail::GetBattlePetPosition_405B40(world::detail::gBattleCurrentPosition5ABCA4);
    } else if (mode == 6) {
        position = world::detail::gBattleCurrentPosition5ABCA4;
        petIndex = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
    } else {
        petIndex = state;
    }
    const auto* const pet = &(*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex];
    std::int32_t count = 0;
    std::int32_t unavailable = 0;
    std::int32_t row = 0;
    std::int32_t capacity = pet->value6A4;
    while (row < capacity) {
        const auto technique = std::bit_cast<std::int16_t>(pet->techniquePairs650[row].index);
        const auto* const record = &pet->techniquesD8[technique];
        if (record->name[0] != '\0') {
            const std::int32_t cost = record->fp;
            login::YobiAction* const action = world::detail::gBattleEntries59E980[position];
            if (login::detail::IsCostGreaterThanYobiActionFp(cost, action)
                || (world::detail::gBattlePetSkillFlags5ABB78 & (1u << (static_cast<std::uint32_t>(row) & 31u))) == 0) {
                ++unavailable;
            }
            ++count;
        }
        capacity = pet->value6A4;
        ++row;
    }
    if (count > unavailable) {
        gCurrentWindow->elements[8].visible = 0;
        return 0;
    }
    const bool hovered = (state & 1) != 0;
    if (hovered) {
        gHoverTextD68AE8 = kPetSkillSkip5865A8;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const WindowValueCallback callback = gPetSkillSelectionCallbackCC4D14;
        if (callback != nullptr) {
            callback(-1);
        }
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3BE7E;
    if (hovered) {
        graphic->imageNo = 0x3BE80;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3BE7F;
    }
    return consumed;
}

// 原函数：0x0047A620。
std::int32_t WindowSkill_Update(const std::int32_t value)
{
    if (value == -1) {
        const std::int32_t mode = gSkillWindowModeD672AC;
        gSkillWindowScrollD2D648 = 0;
        switch (mode) {
        case 0:
            WindowSkill_ShowSkillList(gCurrentWindow, -1);
            break;
        case 3:
            WindowSkill_ShowBattleSkills(gCurrentWindow, -1);
            break;
        case 4: {
            const std::int32_t skillIndex = gSkillWindowIndexD2EA60;
            WindowSkill_ShowBattleTechniques(gCurrentWindow, skillIndex, 1);
            break;
        }
        case 5:
            WindowSkill_ShowPetSkills(gCurrentWindow);
            break;
        case 6:
            WindowSkill_ShowRidingSkills(gCurrentWindow);
            break;
        default:
            break;
        }
    }
    const std::int32_t mode = gSkillWindowModeD672AC;
    gSkillWindowHoverD2F744 = -1;
    if ((mode == 1 || mode == 2 || mode == 4)
        && world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].name[0] == '\0') {
        if (mode == 2) {
            WindowRuntime_DecrementGatherOperationCount();
            --gValueD67AFD;
        }
        if (mode == 1 || mode == 2) {
            WindowRuntime_DecrementOperationCount();
        }
        WindowSkill_ShowSkillList(gCurrentWindow, 0);
    }
    gCurrentWindow->elements[6].visible = 0;
    if (gSkillWindowModeD672AC == 1) {
        gCurrentWindow->elements[6].visible = 1;
    }
    gCurrentWindow->elements[8].visible = 0;
    if (gSkillWindowModeD672AC == 5) {
        gCurrentWindow->elements[8].visible = 1;
    }
    if (gSkillWindowModeD672AC == 6) {
        gCurrentWindow->elements[8].visible = 1;
    }
    return 1;
}

// 原函数：0x0047A7A0。
std::int32_t WindowSkill_Draw(const std::int32_t)
{
    WindowElement* const elements = gCurrentWindow->elements;
    const auto* const mouse = static_cast<const WindowMouseData*>(elements[11].data);
    elements[10].visible = 0;
    std::int32_t mode = gSkillWindowModeD672AC;
    if (mode >= 0 && mode <= 5 && gSkillWindowMaximumCA000C > 0) {
        gCurrentWindow->elements[10].visible = 1;
        mode = gSkillWindowModeD672AC;
    }
    std::int32_t position;
    if ((mouse->flags & 1u) != 0) {
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        position = WindowScroll_ReadPosition(&window->elements[11], maximum);
        mode = gSkillWindowModeD672AC;
        gSkillWindowScrollD2D648 = position;
    } else {
        position = gSkillWindowScrollD2D648;
    }
    std::wstring buffer;
    const auto drawDescription = [&](const char* text, const std::int32_t line, const std::int32_t columns) {
        if (input::ExtractGbkTextLine(buffer, text, line, columns) != 0) {
            const WindowInstance* const window = gCurrentWindow;
            const auto y = static_cast<std::int16_t>(window->y + 208 + 12 * line);
            const auto x = static_cast<std::int16_t>(window->x + 23);
            static_cast<void>(render::DrawText_456780(x, y, 3, 12, 0, buffer, 0, 0));
        }
    };
    switch (mode) {
    case 0:
    case 3: {
        if (mode == 0) {
            std::int32_t count = 0;
            for (std::int32_t index = 0; index < 15; ++index) {
                if (world::detail::gSkillPairsF63370[index].used != 0) {
                    ++count;
                }
            }
            gSkillWindowMaximumCA000C = count - 10;
            if (gSkillWindowMaximumCA000C < 0) {
                gSkillWindowMaximumCA000C = 0;
            }
            if (position > gSkillWindowMaximumCA000C) {
                gSkillWindowScrollD2D648 = gSkillWindowMaximumCA000C;
            }
        }
        static_cast<WindowTextData*>(gCurrentWindow->elements[1].data)->text = cg::text::FromGbkWide(world::detail::BorrowPlayerJobName());
        std::int32_t usedSlots = 0;
        for (std::int32_t index = 0; index < 15; ++index) {
            if (world::detail::gSkillPairsF63370[index].used != 0) {
                const auto skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[index].index);
                const auto* const record = &world::detail::gSkillsF1BAFC[skill];
                if (mode == 3) {
                    const std::int32_t kind = record->canRebirth;
                    if (kind != 0 && kind != 5) {
                        continue;
                    }
                }
                usedSlots += std::bit_cast<std::int8_t>(record->requiredSlots19[0]);
            }
        }
        const std::int32_t totalSlots = world::detail::ReadPlayerValue16C();
        cg::text::FormatTo(buffer, L"{:2}/{:2}", usedSlots, totalSlots);
        static_cast<WindowTextData*>(gCurrentWindow->elements[3].data)->text = buffer;
        break;
    }
    case 1:
    case 4: {
        const auto* record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        if (mode == 1) {
            std::int32_t count = 0;
            while (count < 15 && record->techniques3C[count].name[0] != '\0') {
                ++count;
            }
            gSkillWindowMaximumCA000C = count - 10;
            if (gSkillWindowMaximumCA000C < 0) {
                gSkillWindowMaximumCA000C = 0;
            }
            if (position > gSkillWindowMaximumCA000C) {
                gSkillWindowScrollD2D648 = gSkillWindowMaximumCA000C;
            }
        }
        static_cast<WindowTextData*>(gCurrentWindow->elements[2].data)->text = cg::text::FromGbkWide(record->name.data());
        record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        if (record->level < record->maxLevel) {
            const std::int32_t next = record->nextExperience;
            const std::int32_t experience = record->experience;
            cg::text::FormatTo(buffer, L"{:4}/{:4}", experience, next);
        } else {
            cg::text::FormatTo(buffer, L"{:4}/ -- ", record->experience);
        }
        static_cast<WindowTextData*>(gCurrentWindow->elements[5].data)->text = buffer;
        record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        const std::int32_t maximum = record->maxLevel;
        const std::int32_t level = record->level;
        cg::text::FormatTo(buffer, L"{:2}/{:2}", level, maximum);
        static_cast<WindowTextData*>(gCurrentWindow->elements[9].data)->text = buffer;
        if (gSkillWindowHoverD2F744 >= 0) {
            for (std::int32_t line = 0; line < 4; ++line) {
                const std::int32_t hover = gSkillWindowHoverD2F744;
                const std::int32_t skill = gSkillWindowIndexD2EA60;
                drawDescription(world::detail::gSkillsF1BAFC[skill].techniques3C[hover].description.data(), line, 26);
            }
        }
        break;
    }
    case 2: {
        const auto* const record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        std::int32_t count = 0;
        while (record->recipes8E8[count].itemId >= 0) {
            ++count;
        }
        gSkillWindowMaximumCA000C = count - 10;
        if (gSkillWindowMaximumCA000C < 0) {
            gSkillWindowMaximumCA000C = 0;
        }
        if (position > gSkillWindowMaximumCA000C) {
            gSkillWindowScrollD2D648 = gSkillWindowMaximumCA000C;
        }
        const std::int32_t levelIndex = gSkillRecipeLevelC8B1B4;
        static_cast<WindowTextData*>(gCurrentWindow->elements[2].data)->text = cg::text::FromGbkWide(record->techniques3C[levelIndex].name.data());
        const std::int32_t level = world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].level;
        cg::text::FormatTo(buffer, L"{:5}", level);
        static_cast<WindowTextData*>(gCurrentWindow->elements[9].data)->text = buffer;
        for (std::int32_t line = 0; line < 2; ++line) {
            const std::int32_t skill = gSkillWindowIndexD2EA60;
            const std::int32_t hover = gSkillWindowHoverD2F744;
            // 原47ABC0无悬停门禁；hover=-1读取技能记录+7E5，保留跨字段别名。
            const std::ptrdiff_t descriptionOffset =
                static_cast<std::ptrdiff_t>(offsetof(world::detail::SkillRuntimeRecord, recipes8E8))
                + static_cast<std::ptrdiff_t>(hover)
                    * static_cast<std::ptrdiff_t>(sizeof(world::detail::SkillRecipeRecord))
                + static_cast<std::ptrdiff_t>(offsetof(world::detail::SkillRecipeRecord, description));
            const char* const description = reinterpret_cast<const char*>(&world::detail::gSkillsF1BAFC[skill])
                + descriptionOffset;
            drawDescription(description, line, 28);
        }
        break;
    }
    case 5:
    case 6: {
        std::int32_t maximumPosition = world::detail::gBattleCurrentPosition5ABCA4;
        if (mode == 5) {
            maximumPosition = world::detail::GetBattlePetPosition_405B40(maximumPosition);
        }
        const std::int32_t maximum =
            login::detail::ReadYobiActionMaxFpOnce(world::detail::gBattleEntries59E980[maximumPosition]);
        std::int32_t currentPosition = world::detail::gBattleCurrentPosition5ABCA4;
        if (mode == 5) {
            currentPosition = world::detail::GetBattlePetPosition_405B40(currentPosition);
        }
        const std::int32_t current =
            login::detail::ReadYobiActionFpOnce(world::detail::gBattleEntries59E980[currentPosition]);
        cg::text::FormatTo(static_cast<WindowTextData*>(gCurrentWindow->elements[7].data)->text, L"{:4}/{:4}", current, maximum);
        if (gSkillWindowHoverD2F744 >= 0) {
            for (std::int32_t line = 0; line < 4; ++line) {
                std::int32_t pet;
                std::int32_t hover;
                if (mode == 5) {
                    pet = world::detail::Sub_43C4B0();
                    hover = gSkillWindowHoverD2F744;
                } else {
                    hover = gSkillWindowHoverD2F744;
                    pet = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
                }
                drawDescription((*world::detail::BorrowPetRuntimeF633F8Storage())[pet].techniquesD8[hover].description.data(), line, 26);
            }
        }
        break;
    }
    default:
        break;
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x0047B3D0。
std::int32_t WindowSkill_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        if (gSkillWindowScrollD2D648 > 0) {
            --gSkillWindowScrollD2D648;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gSkillWindowScrollD2D648;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        static_cast<void>(WindowScroll_SetPosition(&window->elements[11], maximum, position));
        consumed = 1;
    }
    graphic->imageNo = 0x3B592;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B594;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B593;
    }
    return consumed;
}

// 原函数：0x0047B470。
std::int32_t WindowSkill_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t position = gSkillWindowScrollD2D648;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        if (position < maximum) {
            gSkillWindowScrollD2D648 = position + 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t current = gSkillWindowScrollD2D648;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t currentMaximum = gSkillWindowMaximumCA000C;
        static_cast<void>(WindowScroll_SetPosition(&window->elements[11], currentMaximum, current));
        consumed = 1;
    }
    graphic->imageNo = 0x3B595;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B597;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B596;
    }
    return consumed;
}

// 原函数：0x0047B520。
std::int32_t WindowSkill_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(gSkillWindowScrollD2D648 > 0 ? 51 : 55, 320));
        gSkillWindowScrollD2D648 -= 10;
        if (gSkillWindowScrollD2D648 < 0) {
            gSkillWindowScrollD2D648 = 0;
        }
        const std::int32_t position = gSkillWindowScrollD2D648;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        static_cast<void>(WindowScroll_SetPosition(&window->elements[11], maximum, position));
        consumed = 1;
    }
    graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557;
    }
    return consumed;
}

// 原函数：0x0047B5D0。
std::int32_t WindowSkill_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const std::int32_t initialMaximum = gSkillWindowMaximumCA000C;
        const bool canAdvance = initialMaximum > 0 && gSkillWindowScrollD2D648 < initialMaximum;
        static_cast<void>(audio::PlaySe_52C450(canAdvance ? 51 : 55, 320));
        std::int32_t position = gSkillWindowScrollD2D648;
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        position += 10;
        gSkillWindowScrollD2D648 = position;
        if (position > maximum) {
            position = maximum;
            gSkillWindowScrollD2D648 = position;
        }
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[11], maximum, position));
        consumed = 1;
    }
    graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B55B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B55A;
    }
    return consumed;
}

// 原函数：0x0047B680。
std::int32_t WindowSkill_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        const std::int32_t maximum = gSkillWindowMaximumCA000C;
        const std::int32_t position = gSkillWindowScrollD2D648;
        gSkillWindowScrollD2D648 =
            WindowScroll_ApplyWheel(&gCurrentWindow->elements[11], position, maximum, wheel);
    }
    return 0;
}

// 原函数：0x00479DA0。
void WindowSkill_ShowRecipes(WindowInstance* const window, const std::int32_t level)
{
    if (level >= 0) {
        gSkillRecipeLevelC8B1B4 = level;
    }
    const std::int32_t oldMaximum = gSkillWindowMaximumCA000C;
    gSkillWindowModeD672AC = 2;
    gSkillWindowScrollD2D648 = 0;
    static_cast<void>(WindowScroll_SetPosition(&window->elements[11], oldMaximum, 0));
    for (std::int32_t index = 1; index <= 16; ++index) {
        window->elements[index].visible = 0;
    }
    window->elements[2].visible = 1;
    window->elements[4].visible = 1;
    window->elements[9].visible = 1;
    const auto* const recipes = world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60].recipes8E8.data();
    std::int32_t count = 0;
    while (recipes[count].itemId >= 0) {
        ++count;
    }
    gSkillWindowMaximumCA000C = count - 10;
    for (std::int32_t index = 10; index <= 16; ++index) {
        window->elements[index].visible = 1;
    }
    static_cast<WindowGraphicData*>(window->elements[27].data)->imageNo = 0x3BA25;
    for (std::int32_t index = 17; index <= 26; ++index) {
        window->elements[index].mouseCallback = WindowSkill_SelectRecipe;
    }
}

// 原函数：0x0047C3A0。
std::int32_t WindowSkill_SelectRecipe(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t recipe = gSkillWindowScrollD2D648 + index - 17;
    const auto* const record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    const std::uint8_t rebirthMode = gValue5F9754;
    if ((record->rebirthState4C48 != 0) != (rebirthMode != 0)
        || record->recipes8E8[recipe].itemId < 0) {
        return 0;
    }
    std::int8_t visualState = 0;
    const bool clicked = (state & 2) != 0;
    if (clicked) {
        bool allowed = (world::detail::ReadPlayerActionFlags() & 0x80u) == 0
            && world::detail::Sub_440B50() == 0;
        if (allowed) {
            const std::int32_t skill = gSkillWindowIndexD2EA60;
            allowed = world::detail::gSkillsF1BAFC[skill].recipes8E8[recipe].value0C != 0;
            if (allowed) {
                bool create;
                if (gWindowStates[WindowId_Craft].window != nullptr) {
                    create = gCraftSkillIndexCC2368 != skill
                        || gCraftRequestValueD30400 != gSkillRecipeLevelC8B1B4
                        || gCraftRecipeIndexD2D638 != recipe;
                } else {
                    create = gWindowActionCA0174 == nullptr;
                }
                if (create) {
                    gWindowActionCA0174 = WindowCraft_OpenRecipe(skill, recipe, gSkillRecipeLevelC8B1B4);
                }
            }
        }
        static_cast<void>(audio::PlaySe_52C450(allowed ? 51 : 55, 320));
    }
    if ((state & 1) != 0) {
        visualState = 2;
        gSkillWindowHoverD2F744 = recipe;
        gHoverTextD68AE8 = kSkillRecipeHint586530;
    }
    if (clicked) {
        visualState = 1;
    }
    if (gWindowStates[WindowId_Craft].window != nullptr
        && gCraftSkillIndexCC2368 == gSkillWindowIndexD2EA60
        && gCraftRequestValueD30400 == gSkillRecipeLevelC8B1B4
        && gCraftRecipeIndexD2D638 == recipe) {
        visualState = 1;
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = &window->elements[index];
    const auto y = static_cast<std::int16_t>(element->y + window->y);
    const auto x = static_cast<std::int16_t>(element->x + window->x);
    WindowSkill_HandleRecipeRow(x, y, visualState, recipe, 3, 223, 0xFFFFFFFFu);
    return 0;
}

// 原函数：0x0047BC00。
std::int32_t WindowSkill_SelectSkillType(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t technique = gSkillWindowScrollD2D648 + index - 17;
    const auto* record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
    if (record->techniques3C[technique].name[0] == '\0') {
        return 0;
    }
    std::int32_t consumed = 0;
    std::int8_t visualState = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = record->techniques3C[technique].value8C != 0 ? kSkillUsable586518 : kSkillUnusable5864FC;
        if (record->canRebirth >= 3 && record->canRebirth <= 4) {
            const std::int32_t cost = record->techniques3C[technique].value7C * record->fpReduce / 100;
            if (world::detail::IsPlayerFpLessThan(cost)) {
                gHoverTextD68AE8 = kWindow12NoFp5864F0;
            }
        }
        visualState = 2;
        gSkillWindowHoverD2F744 = technique;
    }
    if ((state & 2) != 0) {
        record = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
        if (record->techniques3C[technique].value8C == 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            // 可用技术所有分支最终还播放SE51，包括动作门禁与部分FP失败。
            const auto activate = [&] {
                const auto actionBlocked = [] {
                    return (world::detail::ReadPlayerActionFlags() & 0x80u) != 0
                        || world::detail::Sub_440B50() != 0;
                };
                const auto insufficientFp = [&] {
                    const auto* const current = &world::detail::gSkillsF1BAFC[gSkillWindowIndexD2EA60];
                    const std::int32_t cost =
                        current->techniques3C[technique].value7C * current->fpReduce / 100;
                    return world::detail::IsPlayerFpLessThan(cost);
                };
                const auto fail = [] {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                };
                switch (record->canRebirth) {
                case 4:
                    if (actionBlocked()) {
                        fail();
                        return;
                    }
                    if (insufficientFp()) {
                        world::detail::Sub_44B050(kWindow12NoFp5864F0, 4, 3);
                        fail();
                        return;
                    }
                    if (world::detail::Sub_518C60() <= 0) {
                        world::detail::Sub_44B050(kSkillInventoryFull5864E4, 4, 3);
                        fail();
                        return;
                    }
                    if (gWindowStates[WindowId_Gather].window != nullptr) {
                        const std::int32_t current = std::bit_cast<std::int8_t>(gWindow12SkillIndexCA0470);
                        const std::int32_t skill = gSkillWindowIndexD2EA60;
                        if (current != skill || gUsingSkillLevelCA0AC4 != technique) {
                            gWindowActionD2E928 = WindowGather_Open(skill, technique);
                        }
                    } else if (gWindowActionD2E928 == nullptr) {
                        gWindowActionD2E928 = WindowGather_Open(gSkillWindowIndexD2EA60, technique);
                    }
                    return;
                case 3: {
                    if (actionBlocked()) {
                        fail();
                        return;
                    }
                    if (insufficientFp()) {
                        return;
                    }
                    const std::int32_t skill = gSkillWindowIndexD2EA60;
                    const std::uintptr_t socket = platform::gConnectionSocket;
                    gUsingSkillIndexD2F32C = skill;
                    gUsingSkillLevelCA0AC4 = technique;
                    static_cast<void>(network::Sub_511C30(socket, skill));
                    return;
                }
                case 2:
                    if (actionBlocked()) {
                        fail();
                        return;
                    }
                    if (gWindowStates[WindowId_Craft].window != nullptr) {
                        const std::int32_t skill = gSkillWindowIndexD2EA60;
                        if (gCraftSkillIndexCC2368 != skill || gCraftRequestValueD30400 != technique) {
                            gWindowActionCA0174 = WindowCraft_OpenItemWork(skill, technique);
                        }
                    } else if (gWindowActionCA0174 == nullptr) {
                        gWindowActionCA0174 = WindowCraft_OpenItemWork(gSkillWindowIndexD2EA60, technique);
                    }
                    return;
                case 1: {
                    const std::int32_t skillId = record->skillId;
                    if (skillId == 255 || skillId == 606 || skillId == 2025 || skillId == 2026) {
                        if (actionBlocked()) {
                            fail();
                            return;
                        }
                        const std::int32_t skill = gSkillWindowIndexD2EA60;
                        if (skillId == 255 || skillId == 606) {
                            static_cast<void>(WindowCraft_OpenDecoration(skill, technique, skillId == 606 ? 1 : 0));
                        } else if (skillId == 2025) {
                            static_cast<void>(WindowCraft_OpenRefining(skill, technique));
                        } else {
                            static_cast<void>(WindowCraft_OpenMaterials(skill, technique));
                        }
                        return;
                    }
                    WindowRuntime_IncrementGatherOperationCount();
                    const std::uint8_t previous = gValueD67AFD;
                    const std::int32_t skill = gSkillWindowIndexD2EA60;
                    gValueD67AFD = static_cast<std::uint8_t>(previous + 1);
                    const std::int32_t rebirth = world::detail::gSkillsF1BAFC[skill].rebirthState4C48;
                    const bool enabled = gValue5F9754 != 0;
                    if ((rebirth != 0) != enabled) {
                        static_cast<void>(network::Sub_510F50(
                            platform::gConnectionSocket, skill + (enabled ? 100 : 0)));
                    }
                    WindowSkill_ShowRecipes(gCurrentWindow, technique);
                    return;
                }
                case 5: {
                    if (actionBlocked()) {
                        fail();
                        return;
                    }
                    if (login::detail::ReadMapActorAttachment44(world::detail::ReadPlayerAction()) == nullptr
                        && insufficientFp()) {
                        world::detail::Sub_44B050(kWindow12NoFp5864F0, 4, 3);
                        fail();
                        return;
                    }
                    const std::uint8_t rebirth = gValue5F9754;
                    const std::int32_t skill = gSkillWindowIndexD2EA60;
                    const std::uintptr_t socket = platform::gConnectionSocket;
                    static_cast<void>(network::Sub_511EF0(
                        socket, skill + (rebirth != 0 ? 100 : 0), technique, -1, cg::text::FromGbkWide(input::LegacyText593850())));
                    return;
                }
                default:
                    return;
                }
            };
            activate();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
        visualState = 1;
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement* const element = &window->elements[index];
    const auto y = static_cast<std::int16_t>(element->y + window->y);
    const auto x = static_cast<std::int16_t>(element->x + window->x);
    WindowSkill_HandleTechniqueRow(x, y, visualState, technique, 3, 223, 0xFFFFFFFFu);
    return consumed;
}

// 原函数：0x0047A140。
void WindowSkill_SyncStates()
{
    if (gWindowStates[WindowId_Skill].window == nullptr || gSkillWindowModeD672AC != 2) {
        return;
    }
    const std::int32_t index = gSkillWindowIndexD2EA60;
    const std::int32_t rebirth = world::detail::gSkillsF1BAFC[index].rebirthState4C48;
    const bool enabled = gValue5F9754 != 0;
    if ((rebirth != 0) != enabled) {
        static_cast<void>(network::Sub_510F50(
            platform::gConnectionSocket, index + (enabled ? 100 : 0)));
    }
}

// 原函数：0x00487020。
std::int32_t WindowSkill_SelectTextTarget(const std::int32_t index, const char* const text)
{
    if (index == -1) {
        WindowRuntime_DecrementOperationCount();
        const auto count = static_cast<std::uint8_t>(gValueD67AFD - 1);
        const auto skillCount = static_cast<std::uint8_t>(gValueD67AFE - 1);
        gValueD67AFD = count;
        gValueD67AFE = skillCount;
        if (skillCount == 0 && world::detail::Sub_51BEC0() == 13) {
            static_cast<void>(world::detail::Sub_51BE50(0));
        }
        return 1;
    }
    if (index == -2) {
        return 1;
    }
    if (index >= 0 && gSkillTargetPendingC8AF5F == 0) {
        if (gSkillTargetKindCF4A10 != 0 && gSkillTargetKindsD2F75C[index] != 3) {
            static_cast<void>(network::Sub_511C80(platform::gConnectionSocket, index));
            gSkillSelectedTargetCC4B1C = index;
            gSkillTargetPendingC8AF5F = 1;
            return 0;
        }
        gSkillSendModeC8A970 = 0;
        char extra[256];
        extra[0] = '\0';
        std::strcpy(gSkillTargetNameD2F748.data(), text);
        // 原509080只有ret；保留调用前的文本和状态写入，不伪造文本生产。
        std::int32_t skill;
        std::int32_t level;
        if (gValue5F9754 != 0) {
            skill = gUsingSkillIndexD2F32C;
            level = gUsingSkillLevelCA0AC4;
            skill += 100;
        } else if (gSkillSendModeC8A970 != 0) {
            skill = gUsingSkillIndexD2F32C;
            level = gUsingSkillLevelCA0AC4;
            skill += 200;
        } else {
            level = gUsingSkillLevelCA0AC4;
            skill = gUsingSkillIndexD2F32C;
        }
        static_cast<void>(network::Sub_511EF0(
            platform::gConnectionSocket, skill, level, index, cg::text::FromGbkWide(extra)));
    }
    gSkillTargetPendingC8AF5F = 1;
    return 0;
}

// 原函数：0x004871B0。
login::YobiAction* WindowSkill_OpenTextTargets()
{
    constexpr std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    gSkillSendModeC8A970 = 0;
    for (std::int32_t index = 0; index < 10; ++index) {
        const std::int32_t kind = gSkillTargetKindsD2F75C[index];
        gSkillTargetNameViewsD45F98[index] = gSkillTargetNamesCB0550[index].data();
        gSkillTargetColorsCB1DE4[index] = static_cast<std::uint8_t>(colors[kind]);
    }
    const std::int32_t skillIndex = gUsingSkillIndexD2F32C;
    ++gValueD67AFE;
    const std::int32_t kind = WindowSkill_ClassifyAttributeSkill(world::detail::gSkillsF1BAFC[skillIndex].skillId);
    if (kind >= 2 && kind <= 3) {
        if (world::detail::Sub_51BEC0() != 13) {
            static_cast<void>(world::detail::Sub_51BE50(13));
        }
    } else {
        static_cast<void>(world::detail::Sub_51BE50(0));
    }
    WindowRuntime_IncrementOperationCount();
    ++gValueD67AFD;
    constexpr std::array<std::int32_t, 5> windowIds {11, 12, 45, 46, 13};
    for (const std::int32_t id : windowIds) {
        WindowInstance* const window = gWindowStates[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    const std::int32_t count = gSkillTargetCountCA0004;
    gSkillTargetPendingC8AF5F = 0;
    return WindowTextTarget_Open(
        gSkillTargetNameViewsD45F98.data(), gSkillTargetColorsCB1DE4.data(), count, WindowSkill_SelectTextTarget);
}

// 原函数：0x004F0200。
void WindowSkill_ReceiveTextTargets(const char* const text)
{
    const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    const std::int32_t kind = WindowSkill_ClassifyAttributeSkill(world::detail::gSkillsF1BAFC[gUsingSkillIndexD2F32C].skillId);
    gSkillTargetKindCF4A10 = kind;
    gSkillTargetCountCA0004 = 0;
    gSkillTargetPageC8A974 = 0;
    gSkillTargetPageCountCA0178 = 1;
    for (auto& name : gSkillTargetNamesCB0550) {
        name[0] = '\0';
    }
    std::int32_t count = 0;
    if (kind != 0) {
        const char* const name = world::detail::BorrowPlayerDisplayText(1);
        const std::size_t size = std::strlen(name) + 1;
        count = 1;
        gSkillTargetKindsD2F75C[0] = 0;
        gSkillTargetCountCA0004 = count;
        std::memcpy(gSkillTargetNamesCB0550[0].data(), name, size);
    }
    for (std::int16_t slot = 0; slot < 5; ++slot) {
        if (world::detail::ReadPetMapFlags(slot) == 0) {
            continue;
        }
        const std::uint32_t ownIndex = world::ReadPlayerObjectIndex_FDA880();
        if (world::detail::ReadPetMapObjectIndex(slot) == ownIndex) {
            continue;
        }
        login::YobiAction* const action = world::detail::ReadPetMapAction(slot);
        if (action != nullptr) {
            std::strcpy(gSkillTargetNamesCB0550[count].data(), login::detail::BorrowYobiNameC4(action));
        }
        gSkillTargetKindsD2F75C[count] = 1;
        gSkillTargetCountCA0004 = ++count;
    }
    char name[256];
    std::int32_t field = 1;
    std::int32_t appended = 0;
    while (count < 10) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, name) == 1) {
            count = gSkillTargetCountCA0004;
            break;
        }
        const std::int32_t flag = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        if (gSkillTargetKindCF4A10 == 0 && flag == 1) {
            count = gSkillTargetCountCA0004;
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::int32_t index = gSkillTargetCountCA0004;
        std::strcpy(gSkillTargetNamesCB0550[index].data(), name);
        gSkillTargetKindsD2F75C[index] = flag != 0 ? 3 : 2;
        count = index + 1;
        gSkillTargetCountCA0004 = count;
        if (++appended >= 10) {
            break;
        }
    }
    gSkillTargetPageCountCA0178 = (count + 4) / 5;
    if (count > 0) {
        static_cast<void>(WindowSkill_OpenTextTargets());
    } else if (gSkillTargetKindCF4A10 == 0) {
        constexpr wchar_t message[] =
            L"沒有可變裝的角色！";
        world::detail::Sub_44B050(message, 4, 3);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        static_cast<void>(audio::Sub_5042D0(19));
    }
}
} // namespace detail


// 原函数：0x0050E340。
void WindowSkill_ReceiveTextTargetReply(const std::uintptr_t, const char* const text)
{
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    detail::WindowSkill_ReceiveTextTargets(text);
}

// 原函数：0x0050E360。
void WindowSkill_ReceiveAttributeTargetReply(const std::uintptr_t, const char* const text)
{
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    static_cast<void>(detail::WindowSkill_ReceiveAttributeTargets(text));
}

} // namespace cg::ui

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic561A40 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A40。

WindowGraphicData gWindowGraphic561A58 {0x3BA24, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A58。

WindowGraphicData gWindowGraphic561A70 {0x3BA27, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00561A70。

WindowGraphicData gWindowGraphic561A88 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A88。

WindowGraphicData gWindowGraphic561AA0 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AA0。

WindowGraphicData gWindowGraphic561AB8 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AB8。

WindowGraphicData gWindowGraphic561AD0 {0x3B571, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AD0。

WindowGraphicData gWindowGraphic561AE8 {0x3B57A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AE8。

WindowGraphicData gWindowGraphic561B00 {0x3B57A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B00。

WindowGraphicData gWindowGraphic561B18 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B18。

WindowGraphicData gWindowGraphic561B30 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B30。

WindowTextPrototype gWindowText561B48 { // 原全局：0x00561B48。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"無職", // 原常量参考：0x005860A4。
};

WindowTextPrototype gWindowText561B50 { // 原全局：0x00561B50。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"10/10", // 原常量参考：0x0058609C。
};

WindowTextPrototype gWindowText561B58 { // 原全局：0x00561B58。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"經驗值", // 原常量参考：0x005861E0。
};

WindowTextPrototype gWindowText561B60 { // 原全局：0x00561B60。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};

WindowTextPrototype gWindowText561B68 { // 原全局：0x00561B68。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586094。
};

WindowTextPrototype gWindowText561B70 { // 原全局：0x00561B70。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};

std::uint32_t gWindowMouseFlagsD67780 = 0; // 原全局：0x00D67780。

} // namespace

// 原型：窗口10；原全局：0x0054E420。
WindowPrototype gWindowPrototypeSkill {0, 4, 25, 122, 241, 258, 0x80010101u, 30, gWindowElements561B78, &WindowSkill_Update, &WindowSkill_Draw, &WindowSkill_Release}; // 10: 54E420

WindowElementPrototype gWindowElements561B78[30] { // 原全局：0x00561B78。
    {WindowElementType_Graphic, 222, 9, 11, 11, 1, {.graphic = &gWindowGraphic561A40}, &WindowRuntime_ButtonClose},
    {WindowElementType_Text, 82, 12, 0, 0, 1, {.text = &gWindowText561B48}, &WindowElement_NoOp},
    {WindowElementType_Text, 87, 12, 0, 0, 1, {.text = &gWindowText561B68}, &WindowElement_NoOp},
    {WindowElementType_Text, 66, 32, 0, 0, 1, {.text = &gWindowText561B50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 28, 50, 15, 1, {.graphic = &gWindowGraphic561AD0}, &WindowSkill_ButtonBack},
    {WindowElementType_Text, 103, 31, 0, 0, 1, {.text = &gWindowText561B58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 184, 28, 50, 15, 1, {.graphic = &gWindowGraphic561AE8}, &WindowSkill_ButtonUse},
    {WindowElementType_Text, 95, 31, 0, 0, 1, {.text = &gWindowText561B70}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 28, 49, 17, 1, {.graphic = &gWindowGraphic561B00}, &WindowSkill_ButtonPetSkillBack},
    {WindowElementType_Text, 190, 13, 0, 0, 1, {.text = &gWindowText561B60}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 223, 57, 0, 14, 1, {.graphic = &gWindowGraphic561A88}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 223, 57, 11, 140, 1, {.mouseFlags = &gWindowMouseFlagsD67780}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 223, 47, 11, 11, 1, {.graphic = &gWindowGraphic561AA0}, &WindowSkill_ButtonPreviousRow},
    {WindowElementType_Graphic, 223, 196, 11, 11, 1, {.graphic = &gWindowGraphic561AB8}, &WindowSkill_ButtonNextRow},
    {WindowElementType_Graphic, 195, 28, 18, 18, 1, {.graphic = &gWindowGraphic561B18}, &WindowSkill_ButtonPreviousPage},
    {WindowElementType_Graphic, 215, 28, 18, 18, 1, {.graphic = &gWindowGraphic561B30}, &WindowSkill_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 241, 258, 1, {.unusedValue = 0}, &WindowSkill_Scroll},
    {WindowElementType_EventArea, 14, 46, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 62, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 78, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 94, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 110, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 126, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 142, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 158, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 174, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 190, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561A58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic561A70}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 243, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

} // namespace cg::ui::detail
