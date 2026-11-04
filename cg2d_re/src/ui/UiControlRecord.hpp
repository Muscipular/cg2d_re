#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::session { class ClientSession; }

namespace cg::ui {

// LUR01：仅由登录／服务器／角色选择入口持有，退出时平移该场景新增的绘制记录。
class LegacyUiLayoutScope {
public:
    LegacyUiLayoutScope();
    ~LegacyUiLayoutScope();
    LegacyUiLayoutScope(const LegacyUiLayoutScope&) = delete;
    LegacyUiLayoutScope& operator=(const LegacyUiLayoutScope&) = delete;

    // 命中矩形使用与绘制相同的偏移；作用域外返回零。
    static std::int32_t GetOffsetX() noexcept { return offsetX_; }
    static std::int32_t GetOffsetY() noexcept { return offsetY_; }

private:
    std::uint16_t graphicStart_; // 本场景首条图形记录。
    std::uint32_t textStart_; // 本场景首条独立文本记录。
    inline static std::int32_t offsetX_ = 0;
    inline static std::int32_t offsetY_ = 0;
};

// 原四W32命中矩形；由角色创建命中与七W32控件记录共用。
struct UiControlBounds {
    std::int32_t left; // 原矩形成员偏移：+0x00。
    std::int32_t top; // 原矩形成员偏移：+0x04。
    std::int32_t width; // 原矩形成员偏移：+0x08。
    std::int32_t height; // 原矩形成员偏移：+0x0C。
};

static_assert(offsetof(UiControlBounds, left) == 0x00);
static_assert(offsetof(UiControlBounds, top) == 0x04);
static_assert(offsetof(UiControlBounds, width) == 0x08);
static_assert(offsetof(UiControlBounds, height) == 0x0C);
static_assert(sizeof(UiControlBounds) == 0x10);
static_assert(alignof(UiControlBounds) == alignof(std::int32_t));
static_assert(std::is_standard_layout_v<UiControlBounds>);
static_assert(std::is_trivially_copyable_v<UiControlBounds>);

// 登录／角色选择共用记录；普通实例直接读写，原始映像仅在场景边界按值转换。
struct UiControlRecord {
    UiControlBounds bounds; // 原成员范围：+0x00..+0x0F。
    std::int32_t normalImageNo; // 原成员偏移：+0x10。
    std::int32_t activeImageNo; // 原成员偏移：+0x14。
    std::int32_t clickedImageNo; // 原成员偏移：+0x18。
};

static_assert(offsetof(UiControlRecord, bounds) == 0x00);
static_assert(offsetof(UiControlRecord, normalImageNo) == 0x10);
static_assert(offsetof(UiControlRecord, activeImageNo) == 0x14);
static_assert(offsetof(UiControlRecord, clickedImageNo) == 0x18);
static_assert(sizeof(UiControlRecord) == 0x1C);
static_assert(alignof(UiControlRecord) == alignof(std::int32_t));
static_assert(std::is_standard_layout_v<UiControlRecord>);
static_assert(std::is_trivially_copyable_v<UiControlRecord>);

// 原函数：0x0045F100。 查询控件矩形命中的鼠标输入位图。
std::int32_t QueryUiControlInputState_45F100(session::ClientSession& session, const UiControlRecord* controlRecord);
// 原函数：0x0045EFD0。 按图像状态模式绘制底图及可选悬停边框。
void DrawUiControl_45EFD0(
    session::ClientSession& session,
    const UiControlRecord* controlRecord,
    std::uint8_t renderLayer,
    std::int32_t hoverFrameMode,
    std::int32_t hoverFrameImageNo,
    std::int32_t imageStateMode);

} // namespace cg::ui
