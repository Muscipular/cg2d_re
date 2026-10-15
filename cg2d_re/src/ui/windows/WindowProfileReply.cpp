#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x004CB9F0。
std::uint32_t WindowProfileReply_Refresh()
{
    const world::detail::ProfileRuntimeRecord& profile =
        world::detail::ReadReceivedProfile1074110();
    const std::int32_t image = character_select::detail::Sub_51BC50(profile.value10C);
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[13].data)->imageNo = image;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index + 14].data)->imageNo =
            profile.entries410[index].value04;
    }
    static_cast<WindowTextData*>(gCurrentWindow->elements[0].data)->text = cg::text::FromGbkWide(profile.text08.data());
    std::wstring number;
    cg::text::FormatTo(number, L"{}", profile.value108);
    static_cast<WindowTextData*>(gCurrentWindow->elements[1].data)->text = number;
    static_cast<WindowTextData*>(gCurrentWindow->elements[2].data)->text = cg::text::FromGbkWide(profile.text110.data());
    static_cast<WindowTextData*>(gCurrentWindow->elements[3].data)->text = cg::text::FromGbkWide(profile.text210.data());
    static_cast<WindowTextData*>(gCurrentWindow->elements[4].data)->text = cg::text::FromGbkWide(profile.text310.data());
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<WindowTextData*>(gCurrentWindow->elements[index + 5].data)->text = cg::text::FromGbkWide(profile.entries410[index].text08.data());
    }
    std::wstring line;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<void>(input::ExtractGbkTextLine(line, profile.text728.data(), index, 28));
        static_cast<WindowTextData*>(gCurrentWindow->elements[index + 8].data)->text = line;
    }
    static_cast<void>(input::ExtractGbkTextLine(line, profile.text728.data(), 3, 28));
    std::wstring& destination = static_cast<WindowTextData*>(gCurrentWindow->elements[11].data)->text;
    const auto units = static_cast<std::uint32_t>(line.size() + 1);
    destination = line;
    return units;
}

// 原函数：0x004CB9E0。
std::int32_t WindowProfileReply_Update(const std::int32_t)
{
    static_cast<void>(WindowProfileReply_Refresh());
    return 1;
}
} // namespace detail


// 原函数：0x005342E0。
WindowInstance* WindowProfileReply_Open()
{
    return WindowRuntime_Open(WindowId_ProfileReply);
}

} // namespace cg::ui
