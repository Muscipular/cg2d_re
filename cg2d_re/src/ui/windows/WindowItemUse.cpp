#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0048AEA0。
static std::int32_t WindowItemUse_SelectTextTarget(session::ClientSession& session, std::int32_t index, const wchar_t* unusedText); // 选择发送/释放计数；-2仅允许关闭。

// 原函数：0x0048AF00。
static cg::yobi::YobiAction* WindowItemUse_OpenTextTargets(session::ClientSession& session); // 名称/颜色借用及物品占用计数，创建失败按原条件回滚。

// 原函数：0x004E4860。
static cg::yobi::YobiAction* WindowItemUse_ReceiveTextTargets(session::ClientSession& session, const char* text); // LY完整目标名单生产，保留原Yobi返回。

// 原函数：0x0048B010。
static std::int32_t WindowItemUse_SelectAttributeTarget(session::ClientSession& session, std::int32_t index, const wchar_t* unusedText); // 用物品/请求回退/释放通知。

// 原函数：0x0048B0A0。
static cg::yobi::YobiAction* WindowItemUse_OpenAttributeTargets(session::ClientSession& session); // 属性展示源及46创建成功后的引用计数。

// 原函数：0x004E4020。
static cg::yobi::YobiAction* WindowItemUse_ReceiveAttributeTargets(session::ClientSession& session, const char* text); // nFV完整属性值生产，不清旧表或替换宠物状态owner。

// 控件载荷、窗口状态、组件表与原型。

// 窗口函数实现。

// 原函数：0x0048AEA0。
static std::int32_t WindowItemUse_SelectTextTarget(session::ClientSession& session, const std::int32_t index, const wchar_t*)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& targets = session.UiFrame().native.itemTargets;
    if (index >= 0) {
        static_cast<void>(session.Connection().SelectItemTextTarget(index));
        targets.selected = index;
        return 0;
    }
    if (index == -1) {
        const std::int32_t selected = inventory.selectedItem;
        if (selected >= 0) {
            --reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data())[selected];
        }
        --targets.references;
        if (std::bit_cast<std::int8_t>(targets.references) <= 0) {
            inventory.selectedItem = -1;
        }
    }
    return 1;
}

// 原函数：0x0048AF00。
static cg::yobi::YobiAction* WindowItemUse_OpenTextTargets(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& targets = session.UiFrame().native.itemTargets;
    constexpr std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    for (std::int32_t index = 0; index < 10; ++index) {
        targets.names[index] = targets.records[index].name.data();
        targets.colors[index] =
            static_cast<std::uint8_t>(colors[targets.records[index].kind]);
    }
    constexpr std::array<std::int32_t, 3> windowIds {13, 45, 46};
    for (const std::int32_t id : windowIds) {
        WindowInstance* const window = WindowRuntime_States(session)[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    const std::int32_t selected = inventory.currentItem;
    inventory.selectedItem = selected;
    ++reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data())[selected];
    ++targets.references;
    cg::yobi::YobiAction* const result = WindowTextTarget_Open(
        session, targets.names.data(), targets.colors.data(),
        targets.count, WindowItemUse_SelectTextTarget);
    if (result == nullptr) {
        WindowInstance* const window = WindowRuntime_States(session)[WindowId_TextTarget].window;
        if (window == nullptr || (window->state & 0x8000u) != 0) {
            const std::int32_t previous = inventory.selectedItem;
            inventory.selectedItem = -1;
            --reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data())[previous];
            --targets.references;
        }
    }
    return result;
}

// 原函数：0x004E4860。
static cg::yobi::YobiAction* WindowItemUse_ReceiveTextTargets(session::ClientSession& session, const char* const text)
{
    auto& targets = session.UiFrame().native.itemTargets;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(session, x, y));
    targets.records.fill({});
    const wchar_t* const ownName = world::GetPlayerDisplayText(1);
    const std::uint32_t ownIndex = world::gPlayerRuntimeSlots.objectIndex;
    targets.records[0].name = ownName;
    std::int32_t count = 1;
    targets.count = count;
    targets.records[0].kind = 0;
    for (std::int16_t slot = 0; slot < 5; ++slot) {
        if (world::ReadPetMapFlags(slot) != 0
            && world::ReadPetMapObjectIndex(slot) != ownIndex) {
            cg::yobi::YobiAction* const action = world::ReadPetMapAction(slot);
            if (action != nullptr) {
                targets.records[count].name = cg::yobi::GetYobiName(action);
            }
            const std::int32_t index = count++;
            targets.count = count;
            targets.records[index].kind = 1;
        }
    }
    char name[256];
    std::int32_t field = 1;
    std::int32_t appended = 0;
    while (count < 10) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 255, name) == 1) {
            count = targets.count;
            break;
        }
        const std::int32_t flag = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::int32_t index = targets.count;
        targets.records[index].name = cg::text::FromGbkWide(name);
        targets.records[index].kind = flag != 0 ? 3 : 2;
        count = index + 1;
        targets.count = count;
        if (++appended >= 10) {
            break;
        }
    }
    targets.page = 0;
    targets.pageCount = (count + 4) / 5;
    return WindowItemUse_OpenTextTargets(session);
}

// 原函数：0x0048B010。
static std::int32_t WindowItemUse_SelectAttributeTarget(session::ClientSession& session, const std::int32_t index, const wchar_t*)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& targets = session.UiFrame().native.itemTargets;
    if (index == -3) {
        static_cast<void>(session.Connection().RequestItemTargetAttributes());
        return 0;
    }
    if (index >= 0) {
        const std::int32_t item = inventory.currentItem;
        const std::int32_t target = targets.attributes[index].target;
        const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
        static_cast<void>(session.Connection().SendItemUseRequest(x, y, item, target));
    }
    if (index == -1) {
        const std::int32_t selected = inventory.selectedItem;
        if (selected >= 0) {
            --reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data())[selected];
        }
        --targets.references;
        if (std::bit_cast<std::int8_t>(targets.references) <= 0) {
            inventory.selectedItem = -1;
        }
    }
    return 1;
}

// 原函数：0x0048B0A0。
static cg::yobi::YobiAction* WindowItemUse_OpenAttributeTargets(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& targets = session.UiFrame().native.itemTargets;
    const std::int32_t count = targets.attributeCount;
    constexpr std::array<std::int32_t, 7> colors {5, 1, 0, 0, 0, 0, 0};
    for (std::int32_t index = 0; index < count; ++index) {
        const auto& source = targets.attributes[index];
        auto& target = targets.attributeViews[index];
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
        WindowInstance* const window = WindowRuntime_States(session)[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    cg::yobi::YobiAction* const result =
        WindowAttributeTarget_Open(session, targets.attributeViews.data(), targets.attributeCount, WindowItemUse_SelectAttributeTarget);
    bool retainReference = result != nullptr;
    if (!retainReference) {
        WindowInstance* const window = WindowRuntime_States(session)[WindowId_AttributeTarget].window;
        retainReference = window != nullptr && (window->state & 0x8000u) == 0;
    }
    if (retainReference) {
        ++reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data())[inventory.selectedItem];
        ++targets.references;
    }
    return result;
}

// 原函数：0x004E4020。
static cg::yobi::YobiAction* WindowItemUse_ReceiveAttributeTargets(session::ClientSession& session, const char* const text)
{
    auto& business = session.UiFrame().native.business;
    auto& targets = session.UiFrame().native.itemTargets;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(session, x, y));
    const std::int32_t selected = targets.selected;
    targets.attributeCount = 0;
    if (targets.records[selected].kind == 0) {
        const wchar_t* const name = world::GetPlayerDisplayText(1);
        const std::int32_t level = world::gPlayerRuntimeSlots.level;
        targets.attributes[0].name = name;
        const std::int32_t injury = world::gPlayerRuntimeSlots.valueF4;
        targets.attributes[0].level = level;
        targets.attributes[0].injury = injury;
        const std::int32_t hp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.hp);
        targets.attributes[targets.attributeCount].hp = hp;
        const std::int32_t maxHp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.maxHp);
        targets.attributes[targets.attributeCount].maxHp = maxHp;
        const std::int32_t fp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.fp);
        const std::int32_t own = targets.attributeCount;
        targets.attributes[own].fp = fp;
        const std::int32_t maxFp = static_cast<std::int32_t>(world::gPlayerRuntimeSlots.maxFp);
        targets.attributes[own].maxFp = maxFp;
        std::int32_t count = targets.attributeCount + 1;
        targets.attributes[own].kind = 0;
        targets.attributes[own].target = 0;
        targets.attributeCount = count;
        std::int32_t target = 1;
        for (std::int32_t slot = 0; slot < 5; ++slot) {
            auto& pet = world::gPetRuntimeRecords[slot];
            if (pet.used == 0 || business.valuesCC1A18[slot] == 3) {
                continue;
            }
            const wchar_t* const petName = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            targets.attributes[count].name = petName;
            const std::int32_t petInjury = pet.values70[25];
            targets.attributes[count].level = pet.value08;
            targets.attributes[count].injury = petInjury;
            const std::int32_t petHp = static_cast<std::int32_t>(pet.values0C[0]);
            targets.attributes[targets.attributeCount].hp = petHp;
            const std::int32_t petMaxHp = static_cast<std::int32_t>(pet.values0C[1]);
            targets.attributes[targets.attributeCount].maxHp = petMaxHp;
            const std::int32_t petFp = static_cast<std::int32_t>(pet.values0C[2]);
            const std::int32_t output = targets.attributeCount;
            targets.attributes[output].fp = petFp;
            const std::int32_t petMaxFp = static_cast<std::int32_t>(pet.values0C[3]);
            targets.attributes[output].maxFp = petMaxFp;
            count = targets.attributeCount + 1;
            targets.attributes[output].kind = 4;
            targets.attributes[output].target = target;
            targets.attributeCount = count;
            ++target;
        }
        return WindowItemUse_OpenAttributeTargets(session);
    }

    char name[128];
    std::int32_t field = 1;
    for (std::int32_t row = 0; row < 6; ++row) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 127, name) == 1) {
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        targets.attributes[targets.attributeCount].name = cg::text::FromGbkWide(name);
        std::int32_t value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        targets.attributes[targets.attributeCount].level = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        targets.attributes[targets.attributeCount].injury = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        targets.attributes[targets.attributeCount].hp = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        targets.attributes[targets.attributeCount].maxHp = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        const std::int32_t output = targets.attributeCount;
        targets.attributes[output].fp = value;
        value = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        targets.attributes[output].maxFp = value;
        switch (targets.records[targets.selected].kind) {
        case 1: targets.attributes[output].kind = row == 0 ? 1 : 5; break;
        case 2: targets.attributes[output].kind = row == 0 ? 2 : 6; break;
        case 3: targets.attributes[output].kind = 3; break;
        }
        const std::int32_t count = targets.attributeCount + 1;
        targets.attributes[output].target = row;
        targets.attributeCount = count;
    }
    return WindowItemUse_OpenAttributeTargets(session);
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x0050E380。
void WindowItemUse_ReceiveTextTargetReply(cg::network::CgConnection& connection, const char* const text)
{
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    static_cast<void>(detail::WindowItemUse_ReceiveTextTargets(connection.Owner(), text));
}

// 原函数：0x0050E3A0。
void WindowItemUse_ReceiveAttributeTargetReply(cg::network::CgConnection& connection, const char* const text)
{
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    static_cast<void>(detail::WindowItemUse_ReceiveAttributeTargets(connection.Owner(), text));
}

} // namespace cg::ui
