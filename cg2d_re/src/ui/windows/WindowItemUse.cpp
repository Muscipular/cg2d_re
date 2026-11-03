#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"

namespace cg::ui {
namespace detail {

// 原CB10D8的52B值记录；实际六行存储由本文件私有持有。
struct ItemUseStatusRecord {
    std::wstring name;
    std::int32_t level; // 原成员偏移：+0x14。
    std::int32_t injury; // 原成员偏移：+0x18。
    std::int32_t hp; // 原成员偏移：+0x1C。
    std::int32_t maxHp;
    std::int32_t fp;
    std::int32_t maxFp; // 原成员偏移：+0x28。
    std::int32_t kind; // 原成员偏移：+0x2C。
    std::int32_t target; // 原成员偏移：+0x30。
};
// 原CB0EA8十条20B名单；与45的借用表及后续物品详情共用，不与交易名单混用。
struct ItemUseTargetRecord {
    std::int16_t kind;
    std::wstring name; // 原成员偏移：+0x02。
    std::byte byte13; // 原成员偏移：+0x13。
};

// 回调与内部函数前置声明。

// 原函数：0x0048AEA0。
static std::int32_t WindowItemUse_SelectTextTarget(std::int32_t index, const wchar_t* unusedText); // 选择发送/释放计数；-2仅允许关闭。

// 原函数：0x0048AF00。
static cg::yobi::YobiAction* WindowItemUse_OpenTextTargets(); // 名称/颜色借用及物品占用计数，创建失败按原条件回滚。

// 原函数：0x004E4860。
static cg::yobi::YobiAction* WindowItemUse_ReceiveTextTargets(const char* text); // LY完整目标名单生产，保留原Yobi返回。

// 原函数：0x0048B010。
static std::int32_t WindowItemUse_SelectAttributeTarget(std::int32_t index, const wchar_t* unusedText); // 用物品/请求回退/释放通知。

// 原函数：0x0048B0A0。
static cg::yobi::YobiAction* WindowItemUse_OpenAttributeTargets(); // 属性展示源及46创建成功后的引用计数。

// 原函数：0x004E4020。
static cg::yobi::YobiAction* WindowItemUse_ReceiveAttributeTargets(const char* text); // nFV完整属性值生产，不清旧表或替换宠物状态owner。

// 控件载荷、窗口状态、组件表与原型。

static std::array<ItemUseTargetRecord, 10> gItemUseTargetsCB0EA8 {}; // 原全局：0x00CB0EA8。

static std::int32_t gItemUseTargetCountCC40CC = 0; // 原全局：0x00CC40CC。

static std::int32_t gItemUsePageCC46DC = 0; // 原全局：0x00CC46DC。

static std::int32_t gItemUsePageCountCC3E70 = 0; // 原全局：0x00CC3E70。

static std::int32_t gItemUseTargetCB0A48 = 0; // 原全局：0x00CB0A48。

static std::array<const wchar_t*, 10> gItemUseNamesCB0A1C {}; // 原全局：0x00CB0A1C。

static std::array<std::uint8_t, 10> gItemUseColorsD2E91C {}; // 原全局：0x00D2E91C。

// 仅映射已证最多六行，不冻结相邻未消费容量；旧78借用同源记录。
static std::array<ItemUseStatusRecord, 6> gItemUseStatusRecordsCB10D8 {}; // 原全局：0x00CB10D8。

static std::int32_t gItemUseStatusCountCC4D44 = 0; // 原全局：0x00CC4D44。

static std::array<WindowStatusChoiceRecord, 6> gItemUseStatusViewsCB1278 {}; // 原全局：0x00CB1278。

// 窗口函数实现。

// 原函数：0x0048AEA0。
static std::int32_t WindowItemUse_SelectTextTarget(const std::int32_t index, const wchar_t*)
{
    if (index >= 0) {
        static_cast<void>(cg::network::GetGameConnection().SelectItemTextTarget(index));
        gItemUseTargetCB0A48 = index;
        return 0;
    }
    if (index == -1) {
        const std::int32_t selected = gSelectedItemCB1DF4;
        if (selected >= 0) {
            --reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data())[selected];
        }
        --gItemUseTargetReferences_D67B24;
        if (std::bit_cast<std::int8_t>(gItemUseTargetReferences_D67B24) <= 0) {
            gSelectedItemCB1DF4 = -1;
        }
    }
    return 1;
}

// 原函数：0x0048AF00。
static cg::yobi::YobiAction* WindowItemUse_OpenTextTargets()
{
    constexpr std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    for (std::int32_t index = 0; index < 10; ++index) {
        gItemUseNamesCB0A1C[index] = gItemUseTargetsCB0EA8[index].name.data();
        gItemUseColorsD2E91C[index] =
            static_cast<std::uint8_t>(colors[gItemUseTargetsCB0EA8[index].kind]);
    }
    constexpr std::array<std::int32_t, 3> windowIds {13, 45, 46};
    for (const std::int32_t id : windowIds) {
        WindowInstance* const window = WindowRuntime_States()[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    const std::int32_t selected = gSelectedItemCB0A44;
    gSelectedItemCB1DF4 = selected;
    ++reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data())[selected];
    ++gItemUseTargetReferences_D67B24;
    cg::yobi::YobiAction* const result = WindowTextTarget_Open(
        gItemUseNamesCB0A1C.data(), gItemUseColorsD2E91C.data(),
        gItemUseTargetCountCC40CC, WindowItemUse_SelectTextTarget);
    if (result == nullptr) {
        WindowInstance* const window = WindowRuntime_States()[WindowId_TextTarget].window;
        if (window == nullptr || (window->state & 0x8000u) != 0) {
            const std::int32_t previous = gSelectedItemCB1DF4;
            gSelectedItemCB1DF4 = -1;
            --reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data())[previous];
            --gItemUseTargetReferences_D67B24;
        }
    }
    return result;
}

// 原函数：0x004E4860。
static cg::yobi::YobiAction* WindowItemUse_ReceiveTextTargets(const char* const text)
{
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    gItemUseTargetsCB0EA8.fill({});
    const wchar_t* const ownName = world::GetPlayerDisplayText(1);
    const std::uint32_t ownIndex = world::gPlayerRuntimeSlots.objectIndex;
    gItemUseTargetsCB0EA8[0].name = ownName;
    std::int32_t count = 1;
    gItemUseTargetCountCC40CC = count;
    gItemUseTargetsCB0EA8[0].kind = 0;
    for (std::int16_t slot = 0; slot < 5; ++slot) {
        if (world::ReadPetMapFlags(slot) != 0
            && world::ReadPetMapObjectIndex(slot) != ownIndex) {
            cg::yobi::YobiAction* const action = world::ReadPetMapAction(slot);
            if (action != nullptr) {
                gItemUseTargetsCB0EA8[count].name = cg::yobi::GetYobiName(action);
            }
            const std::int32_t index = count++;
            gItemUseTargetCountCC40CC = count;
            gItemUseTargetsCB0EA8[index].kind = 1;
        }
    }
    char name[256];
    std::int32_t field = 1;
    std::int32_t appended = 0;
    while (count < 10) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, name) == 1) {
            count = gItemUseTargetCountCC40CC;
            break;
        }
        const std::int32_t flag = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::int32_t index = gItemUseTargetCountCC40CC;
        gItemUseTargetsCB0EA8[index].name = cg::text::FromGbkWide(name);
        gItemUseTargetsCB0EA8[index].kind = flag != 0 ? 3 : 2;
        count = index + 1;
        gItemUseTargetCountCC40CC = count;
        if (++appended >= 10) {
            break;
        }
    }
    gItemUsePageCC46DC = 0;
    gItemUsePageCountCC3E70 = (count + 4) / 5;
    return WindowItemUse_OpenTextTargets();
}

// 原函数：0x0048B010。
static std::int32_t WindowItemUse_SelectAttributeTarget(const std::int32_t index, const wchar_t*)
{
    if (index == -3) {
        static_cast<void>(cg::network::GetGameConnection().RequestItemTargetAttributes());
        return 0;
    }
    if (index >= 0) {
        const std::int32_t item = gSelectedItemCB0A44;
        const std::int32_t target = gItemUseStatusRecordsCB10D8[index].target;
        const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
        static_cast<void>(cg::network::GetGameConnection().SendItemUseRequest(x, y, item, target));
    }
    if (index == -1) {
        const std::int32_t selected = gSelectedItemCB1DF4;
        if (selected >= 0) {
            --reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data())[selected];
        }
        --gItemUseTargetReferences_D67B24;
        if (std::bit_cast<std::int8_t>(gItemUseTargetReferences_D67B24) <= 0) {
            gSelectedItemCB1DF4 = -1;
        }
    }
    return 1;
}

// 原函数：0x0048B0A0。
static cg::yobi::YobiAction* WindowItemUse_OpenAttributeTargets()
{
    const std::int32_t count = gItemUseStatusCountCC4D44;
    constexpr std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    for (std::int32_t index = 0; index < count; ++index) {
        const auto& source = gItemUseStatusRecordsCB10D8[index];
        auto& target = gItemUseStatusViewsCB1278[index];
        target.name = source.name.data();
        target.level = source.level;
        target.hp = source.hp;
        target.maxHp = source.maxHp;
        target.fp = source.fp;
        target.maxFp = source.maxFp;
        target.injury = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(source.injury));
        target.color = static_cast<std::uint8_t>(colors[source.kind]);
    }
    constexpr std::array<std::int32_t, 3> windowIds {13, 45, 46};
    for (const std::int32_t id : windowIds) {
        WindowInstance* const window = WindowRuntime_States()[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    cg::yobi::YobiAction* const result =
        WindowAttributeTarget_Open(gItemUseStatusViewsCB1278.data(), gItemUseStatusCountCC4D44, WindowItemUse_SelectAttributeTarget);
    bool retainReference = result != nullptr;
    if (!retainReference) {
        WindowInstance* const window = WindowRuntime_States()[WindowId_AttributeTarget].window;
        retainReference = window != nullptr && (window->state & 0x8000u) == 0;
    }
    if (retainReference) {
        ++reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data())[gSelectedItemCB1DF4];
        ++gItemUseTargetReferences_D67B24;
    }
    return result;
}

// 原函数：0x004E4020。
static cg::yobi::YobiAction* WindowItemUse_ReceiveAttributeTargets(const char* const text)
{
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    const std::int32_t selected = gItemUseTargetCB0A48;
    gItemUseStatusCountCC4D44 = 0;
    if (gItemUseTargetsCB0EA8[selected].kind == 0) {
        const wchar_t* const name = world::GetPlayerDisplayText(1);
        const std::int32_t level = world::gPlayerRuntimeSlots.level;
        gItemUseStatusRecordsCB10D8[0].name = name;
        const std::int32_t injury = world::gPlayerRuntimeSlots.valueF4;
        gItemUseStatusRecordsCB10D8[0].level = level;
        gItemUseStatusRecordsCB10D8[0].injury = injury;
        const std::int32_t hp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.hp);
        gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].hp = hp;
        const std::int32_t maxHp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.maxHp);
        gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].maxHp = maxHp;
        const std::int32_t fp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.fp);
        const std::int32_t own = gItemUseStatusCountCC4D44;
        gItemUseStatusRecordsCB10D8[own].fp = fp;
        const std::int32_t maxFp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.maxFp);
        gItemUseStatusRecordsCB10D8[own].maxFp = maxFp;
        std::int32_t count = gItemUseStatusCountCC4D44 + 1;
        gItemUseStatusRecordsCB10D8[own].kind = 0;
        gItemUseStatusRecordsCB10D8[own].target = 0;
        gItemUseStatusCountCC4D44 = count;
        std::int32_t target = 1;
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            auto& pet = world::gPetRuntimeRecords[slot];
            if (pet.used == 0 || gLoginWindowRuntime.valuesCC1A18[slot] == 3) {
                continue;
            }
            const wchar_t* const petName = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            gItemUseStatusRecordsCB10D8[count].name = petName;
            const std::int32_t petInjury = pet.values70[25];
            gItemUseStatusRecordsCB10D8[count].level = pet.value08;
            gItemUseStatusRecordsCB10D8[count].injury = petInjury;
            const std::int32_t petHp = static_cast<std::int32_t>(pet.values0C[0]);
            gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].hp = petHp;
            const std::int32_t petMaxHp = static_cast<std::int32_t>(pet.values0C[1]);
            gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].maxHp = petMaxHp;
            const std::int32_t petFp = static_cast<std::int32_t>(pet.values0C[2]);
            const std::int32_t output = gItemUseStatusCountCC4D44;
            gItemUseStatusRecordsCB10D8[output].fp = petFp;
            const std::int32_t petMaxFp = static_cast<std::int32_t>(pet.values0C[3]);
            gItemUseStatusRecordsCB10D8[output].maxFp = petMaxFp;
            count = gItemUseStatusCountCC4D44 + 1;
            gItemUseStatusRecordsCB10D8[output].kind = 4;
            gItemUseStatusRecordsCB10D8[output].target = target;
            gItemUseStatusCountCC4D44 = count;
            ++target;
        }
        return WindowItemUse_OpenAttributeTargets();
    }

    char name[128];
    std::int32_t field = 1;
    for (std::int32_t row = 0; row < 6; ++row) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 127, name) == 1) {
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].name = cg::text::FromGbkWide(name);
        std::int32_t value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].level = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].injury = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].hp = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gItemUseStatusRecordsCB10D8[gItemUseStatusCountCC4D44].maxHp = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        const std::int32_t output = gItemUseStatusCountCC4D44;
        gItemUseStatusRecordsCB10D8[output].fp = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        gItemUseStatusRecordsCB10D8[output].maxFp = value;
        switch (gItemUseTargetsCB0EA8[gItemUseTargetCB0A48].kind) {
        case 1: gItemUseStatusRecordsCB10D8[output].kind = row == 0 ? 1 : 5; break;
        case 2: gItemUseStatusRecordsCB10D8[output].kind = row == 0 ? 2 : 6; break;
        case 3: gItemUseStatusRecordsCB10D8[output].kind = 3; break;
        }
        const std::int32_t count = gItemUseStatusCountCC4D44 + 1;
        gItemUseStatusRecordsCB10D8[output].target = row;
        gItemUseStatusCountCC4D44 = count;
    }
    return WindowItemUse_OpenAttributeTargets();
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x0050E380。
void WindowItemUse_ReceiveTextTargetReply(cg::network::CgConnection&, const char* const text)
{
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    static_cast<void>(detail::WindowItemUse_ReceiveTextTargets(text));
}

// 原函数：0x0050E3A0。
void WindowItemUse_ReceiveAttributeTargetReply(cg::network::CgConnection&, const char* const text)
{
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    static_cast<void>(detail::WindowItemUse_ReceiveAttributeTargets(text));
}

} // namespace cg::ui
