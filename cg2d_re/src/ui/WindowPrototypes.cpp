#include "cg/session/ClientSession.hpp"
#include "WindowPrototypeDependencies.hpp"
#include "cg/text/Encoding.hpp"
#include <string>
#include "cg/input/Input.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include <array>
#include <cstdlib>

namespace cg::ui {

// R02：原640×480窗口矩形；在任何原型坐标被适配前一次性取样。
struct WindowLayoutBounds {
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
};

static const auto& GetOriginalWindowBounds()
{
    static const auto bounds = [] {
        std::array<WindowLayoutBounds, detail::kWindowCount> original {};
        for (std::size_t index = 0; index < original.size(); ++index) {
            const auto& prototype = *detail::gWindowPrototypes57C130[index];
            original[index] = {prototype.defaultX, prototype.defaultY, prototype.width, prototype.height};
            if (index == WindowId_Chat) {
                // 聊天原型宽640是占位值；共享宽度尚未初始化，按原默认12单位取267。
                original[index].width = 12 * 12 + 123;
            }
        }
        return original;
    }();
    return bounds;
}

static bool IsWindowNearOriginalCenter(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height)
{
    const std::int32_t doubledOffsetX = 2 * x + width - 640;
    const std::int32_t doubledOffsetY = 2 * y + height - 480;
    return doubledOffsetX >= -100 && doubledOffsetX <= 100
        && doubledOffsetY >= -100 && doubledOffsetY <= 100;
}

std::int32_t AdaptWindowAxisPosition(
    const std::int32_t position, const std::int32_t size,
    const std::int32_t baseSize, const std::int32_t targetSize)
{
    if (targetSize == 0 || targetSize == baseSize) {
        return position;
    }
    if (std::abs(position) > std::abs(baseSize - position - size)) {
        return position + targetSize - baseSize;
    }
    return position;
}

void AdaptWindowPosition(
    std::int32_t& x, std::int32_t& y, const std::int32_t width, const std::int32_t height)
{
    if (IsWindowNearOriginalCenter(x, y, width, height)) {
        x = (platform::gWindowWidth - width) / 2;
        y = (platform::gWindowHeight - height) / 2;
    } else {
        x = AdaptWindowAxisPosition(x, width, 640, platform::gWindowWidth);
        y = AdaptWindowAxisPosition(y, height, 480, platform::gWindowHeight);
    }
}

void AdaptWindowPosition(
    std::int16_t& x, std::int16_t& y, const std::int32_t width, const std::int32_t height)
{
    std::int32_t positionX = x;
    std::int32_t positionY = y;
    AdaptWindowPosition(positionX, positionY, width, height);
    x = static_cast<std::int16_t>(positionX);
    y = static_cast<std::int16_t>(positionY);
}

bool CenterPopupWindowPosition(const std::int32_t id,
    const std::int32_t width, const std::int32_t height,
    std::int32_t& x, std::int32_t& y)
{
    const auto& original = GetOriginalWindowBounds()[id];
    if (!IsWindowNearOriginalCenter(original.x, original.y, original.width, original.height)) {
        return false;
    }
    const std::int32_t adaptedX = (platform::gWindowWidth - width) / 2;
    const std::int32_t adaptedY = (platform::gWindowHeight - height) / 2;
    // 以适配后的位置比较当前位置；任一轴超出容差时保留整个窗口的位置。
    const std::int64_t offsetX = static_cast<std::int64_t>(x) - adaptedX;
    const std::int64_t offsetY = static_cast<std::int64_t>(y) - adaptedY;
    if (offsetX < -10 || offsetX > 10 || offsetY < -10 || offsetY > 10) {
        return false;
    }
    x = adaptedX;
    y = adaptedY;
    return true;
}

void InitializeWindowLayout()
{
    const auto& original = GetOriginalWindowBounds();
    for (std::size_t index = 0; index < detail::kWindowCount; ++index) {
        auto& prototype = *detail::gWindowPrototypes57C130[index];
        prototype.defaultX = original[index].x;
        prototype.defaultY = original[index].y;
        AdaptWindowPosition(prototype.defaultX, prototype.defaultY, original[index].width, original[index].height);
        detail::WindowRuntime_States()[index].x = prototype.defaultX;
        detail::WindowRuntime_States()[index].y = prototype.defaultY;
    }
}

namespace detail {

WindowPrototype& WindowRuntime_DialogPrototype(session::ClientSession& session)
{
    auto& native = session.UiFrame().native;
    if (!native.dialogPrototypeInitialized) {
        native.dialogPrototype = gWindowPrototypeDialog;
        native.dialogPrototypeInitialized = true;
    }
    return native.dialogPrototype;
}

const WindowPrototype& WindowRuntime_Prototype(session::ClientSession& session, const std::size_t id)
{
    const WindowPrototype* const prototype = gWindowPrototypes57C130[id];
    if (prototype == &gWindowPrototypeDialog) {
        return WindowRuntime_DialogPrototype(session);
    }
    return *prototype;
}

// 控件载荷、窗口状态、组件表与原型。

WindowGraphicData gWindowGraphic570160 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570160。

WindowGraphicData gWindowGraphic570190 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570190。

WindowTextPrototype gWindowText5701F0 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"测试人员"}; // 原全局：0x005701F0。 // 原常量参考：0x00585EA4。

WindowGraphicData gWindowGraphic565608 {0x3BAEC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565608。

WindowGraphicData gWindowGraphic565620 {0x3BAED, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00565620。

WindowGraphicData gWindowGraphic565638 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565638。

WindowGraphicData gWindowGraphic565650 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565650。

WindowGraphicData gWindowGraphic5656B0 {0x3BAF5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656B0。

WindowGraphicData gWindowGraphic5656C8 {0x3B584, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656C8。

WindowGraphicData gWindowGraphic5656E0 {0x3BAF2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656E0。

WindowGraphicData gWindowGraphic565878 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565878。

WindowGraphicData gWindowGraphic565890 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565890。

WindowGraphicData gWindowGraphic5658C0 {0x3BAF7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658C0。

WindowTextPrototype gWindowText565950 { // 原全局：0x00565950。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名称", // 原常量参考：0x00586230。
};

WindowTextPrototype gWindowText565988 { // 原全局：0x00565988。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"1/5", // 原常量参考：0x0058600C。
};

WindowTextPrototype gWindowText562C70 { // 原全局：0x00562C70。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

WindowTextPrototype gWindowText562C78 { // 原全局：0x00562C78。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

WindowGraphicData gWindowGraphicD678E0 {}; // 原全局：0x00D678E0。

static_assert(sizeof(WindowGraphicData) == 0x24);

static_assert(offsetof(WindowGraphicData, height) == 0x10);

// 各窗口唯一原型对象；71槽索引仍由WindowPrototypes.cpp统一绑定。
// 原全局：0x0054EFB8。
// 原型：窗口0；原全局：0x0054EFB8。
static WindowPrototype gWindowPrototypeNone {0, 0, 0, 0, 0, 0, 0x00000000u, 0, nullptr, nullptr, nullptr, nullptr}; // 0: 54EFB8

// 71个原生指针；原36/37同源，不因窗口编号不同复制对象。
std::array<WindowPrototype*, kWindowCount> gWindowPrototypes57C130 { // 原全局：0x0057C130。
    &gWindowPrototypeNone, // 0
    &gWindowPrototypeStatusBar,
    &gWindowPrototypeCompactStatusBar,
    &gWindowPrototypeCollapsedStatusBar,
    &gWindowPrototypeShortcutBar,
    &gWindowPrototypeCollapsedShortcutBar,
    &gWindowPrototypeBottomBar,
    &gWindowPrototypePlayerProfile,
    &gWindowPrototypePlayerAttributes,
    &gWindowPrototypePlayerTitles,
    &gWindowPrototypeSkill, // 10
    &gWindowPrototypeCraft,
    &gWindowPrototypeGather,
    &gWindowPrototypeSkillResult,
    &gWindowPrototypeInventory,
    &gWindowPrototypePetList,
    &gWindowPrototypePetDetail,
    &gWindowPrototypePetAttributes,
    &gWindowPrototypePetSkills,
    &gWindowPrototypeAlbumList,
    &gWindowPrototypeAlbumDetail, // 20
    &gWindowPrototypeContacts,
    &gWindowPrototypeMailCompose,
    &gWindowPrototypePetMailCompose,
    &gWindowPrototypeSettings,
    &gWindowPrototypeHotkeys,
    &gWindowPrototypeBattleCommand,
    &gWindowPrototypeBattleItems,
    &gWindowPrototypeBattlePets,
    &gWindowPrototypeSpectator,
    &gWindowPrototypeBattleResult, // 30
    &gWindowPrototypeDpResult,
    &gWindowPrototypeBattleStateNotice,
    &gWindowPrototypeMiniMap,
    &gWindowPrototypeEmote,
    &gWindowPrototypeChat,
    &gWindowPrototypeDialog,
    &gWindowPrototypeDialog, // 37
    &gWindowPrototypeItemStorage,
    &gWindowPrototypeTimerNotice,
    &gWindowPrototypeMailRead, // 40
    &gWindowPrototypeBank,
    &gWindowPrototypeQuantity,
    &gWindowPrototypeTrade,
    &gWindowPrototypeTradeTarget,
    &gWindowPrototypeTextTarget,
    &gWindowPrototypeAttributeTarget,
    &gWindowPrototypeConfirmation,
    &gWindowPrototypeGuildDetails,
    &gWindowPrototypeGroupSelect,
    &gWindowPrototypeMessageBoard, // 50
    &gWindowPrototypeTextList,
    &gWindowPrototypeShopChoice,
    &gWindowPrototypeShop,
    &gWindowPrototypePetSkillChoice,
    &gWindowPrototypeGuildRank,
    &gWindowPrototypeAppearance,
    &gWindowPrototypeCompactChat,
    &gWindowPrototypeOwnProfile,
    &gWindowPrototypeOwnProfileCategory,
    &gWindowPrototypeProfileCategory, // 60
    &gWindowPrototypeProfileSearch,
    &gWindowPrototypeProfileView,
    &gWindowPrototypeMiniMail,
    &gWindowPrototypeProfileReply,
    &gWindowPrototypeBlacklist,
    &gWindowPrototypeAdventureLog,
    &gWindowPrototypeOwnStall,
    &gWindowPrototypeStallBrowse,
    &gWindowPrototypeCrystalShop,
    &gWindowPrototypeCrystalExchangeShop, // 70
    &gWindowPrototypeBigMap, // 71
    &gWindowPrototypePetEquipment, // 72
};

} // namespace detail

} // namespace cg::ui
