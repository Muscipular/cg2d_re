#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {

const WindowStatusChoiceRecord* gStatusChoiceRecordsCB03D0 = nullptr; // 原全局：0x00CB03D0。

std::int32_t gStatusChoiceCountCC2364 = 0; // 原全局：0x00CC2364。

WindowTextChoiceCallback gStatusChoiceCallbackCF47D4 = nullptr; // 原全局：0x00CF47D4。

std::int32_t gStatusChoiceOffsetD671D4 = 0; // 原全局：0x00D671D4。

std::uint8_t gStatusChoiceEnabledC9A970 = 0; // 原全局：0x00C9A970。
} // namespace


// 原函数：0x0049F760。
login::YobiAction* WindowAttributeTarget_Open(
    const WindowStatusChoiceRecord* const records, const std::int32_t count,
    const WindowTextChoiceCallback callback)
{
    gStatusChoiceRecordsCB03D0 = records;
    gStatusChoiceCountCC2364 = count;
    gStatusChoiceCallbackCF47D4 = callback;
    return WindowRuntime_CreateAnimation(WindowId_AttributeTarget, 1, 0, nullptr);
}

// 原函数：0x0049F790。
void WindowAttributeTarget_Release()
{
    const WindowTextChoiceCallback callback = gStatusChoiceCallbackCF47D4;
    if (callback != nullptr) {
        static_cast<void>(callback(-1, nullptr));
    }
}

// 原函数：0x0049F7B0。
std::int32_t WindowAttributeTarget_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowAttributeTarget_Refresh(gStatusChoiceOffsetD671D4);
        gStatusChoiceEnabledC9A970 = 1;
    }
    return 1;
}

// 原函数：0x0049F7E0。
void WindowAttributeTarget_Refresh(const std::int32_t offset)
{
    std::int32_t row = 0;
    for (; row < 6 && offset + row < gStatusChoiceCountCC2364; ++row) {
        gCurrentWindow->elements[row + 4].visible = 0;
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 10].data)->text.clear();
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text.clear();
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 28].data)->text.clear();
        const char* const name = gStatusChoiceRecordsCB03D0[offset + row].name;
        if (name != nullptr) {
            cg::text::FormatTo(static_cast<WindowTextData*>(gCurrentWindow->elements[row + 10].data)->text, L"           {}", cg::text::FromGbkWide(name).c_str());
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 10].data)->bytes00[0] =
                static_cast<std::byte>(gStatusChoiceRecordsCB03D0[offset + row].color);
            gCurrentWindow->elements[row + 4].visible = 1;
            const std::int32_t imageIndex = WindowItem_HealthImageLevel(gStatusChoiceRecordsCB03D0[offset + row].injury);
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[row + 4].data)->imageNo =
                gHealthImages57E6A4[imageIndex];
            const std::int32_t level = gStatusChoiceRecordsCB03D0[offset + row].level;
            cg::text::FormatTo(static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text, L"{:10}", level);
            const WindowStatusChoiceRecord* const record = gStatusChoiceRecordsCB03D0 + offset + row;
            const std::int32_t maxFp = record->maxFp;
            const std::int32_t fp = record->fp;
            const std::int32_t maxHp = record->maxHp;
            const std::int32_t hp = record->hp;
            cg::text::FormatTo(static_cast<WindowTextData*>(gCurrentWindow->elements[row + 28].data)->text, L"                            {:6}/{:6} {:6}/{:6}", hp, maxHp, fp, maxFp);
        }
    }
    for (; row < 6; ++row) {
        gCurrentWindow->elements[row + 4].visible = 0;
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 10].data)->text = cg::text::FromGbkWide(input::LegacyText593850());
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text = cg::text::FromGbkWide(input::LegacyText593850());
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 28].data)->text = cg::text::FromGbkWide(input::LegacyText593850());
    }
}

// 原函数：0x0049FA40。
std::int32_t WindowAttributeTarget_ButtonCancel(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"關閉此視窗";
    }
    if ((state & 2) != 0) {
        if (gStatusChoiceEnabledC9A970 != 0) {
            const WindowTextChoiceCallback callback = gStatusChoiceCallbackCF47D4;
            if (callback != nullptr) {
                if (callback(-2, nullptr) != 0) {
                    gCurrentWindow->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
                gStatusChoiceEnabledC9A970 = 0;
            }
        }
        consumed = 1;
    }
    graphic->imageNo = 245320;
    if ((state & 1) != 0) {
        graphic->imageNo = 245322;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 245321;
    }
    return consumed;
}

// 原函数：0x0049FB00。
std::int32_t WindowAttributeTarget_ButtonBack(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"回到前一頁視窗。";
    }
    if ((state & 2) != 0) {
        if (gStatusChoiceEnabledC9A970 != 0) {
            const WindowTextChoiceCallback callback = gStatusChoiceCallbackCF47D4;
            if (callback != nullptr) {
                if (callback(-3, nullptr) != 0) {
                    gCurrentWindow->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
                gStatusChoiceEnabledC9A970 = 0;
            }
        }
        consumed = 1;
    }
    graphic->imageNo = 245332;
    if ((state & 1) != 0) {
        graphic->imageNo = 245334;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 245333;
    }
    return consumed;
}

// 原函数：0x0049FBC0。
std::int32_t WindowAttributeTarget_SelectRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t selected = gStatusChoiceOffsetD671D4 + index - 16;
    if (index - 16 >= gStatusChoiceCountCC2364) {
        return 0;
    }
    if ((state & 1) != 0) {
        WindowElement_DrawHoverFrame(gCurrentWindow->elements + index, state, login::detail::gValueD69768);
        gHoverTextD68AE8 = L"選擇此人物。";
    }
    if ((state & 2) != 0) {
        if (gStatusChoiceEnabledC9A970 != 0) {
            const WindowTextChoiceCallback callback = gStatusChoiceCallbackCF47D4;
            if (callback != nullptr) {
                if (callback(selected, gStatusChoiceRecordsCB03D0[selected].name) != 0) {
                    gCurrentWindow->state |= 0x8000u;
                }
                gStatusChoiceEnabledC9A970 = 0;
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            }
        }
        return 1;
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
