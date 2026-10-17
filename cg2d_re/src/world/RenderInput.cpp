#include "cg/render/GraphicPendingQueue.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"
#include "../platform/imeImpl.hpp"
#include "../platform/StartupNative.hpp"
#include "../platform/WindowProcedureState.hpp"

#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "WorldUpdateDependencies.hpp"

#include <imm.h>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib> // 原CRT接口：0x0054ABDD；原参数／返回合同由直接调用承接。

namespace cg::world {

std::int32_t gValueD460C8 = 0; // 原全局：0x00D460C8。

} // namespace cg::world

namespace cg::world::detail {
// 原.data两说明坐标；本体按Y→X独立读取，保持S16和已加载初值。
static std::int16_t gImeDescriptionX5595EC = 542; // 原全局：0x005595EC。
static std::int16_t gImeDescriptionY5595F0 = 461; // 原全局：0x005595F0。

static inline constexpr std::array<std::uint8_t, 12> kLowercaseDescription {{
    0xAD, 0x5E, 0xBC, 0xC6, 0x2F, 0xA2,
    0xE9, 0xA2, 0xEA, 0xA2, 0xEB, 0x00,
}};
static inline constexpr std::array<std::uint8_t, 12> kUppercaseDescription {{
    0xAD, 0x5E, 0xBC, 0xC6, 0x2F, 0xA2,
    0xCF, 0xA2, 0xD0, 0xA2, 0xD1, 0x00,
}};

static_assert(sizeof(DWORD) == sizeof(std::uint32_t));
static_assert(offsetof(CANDIDATELIST, dwCount) == 0x08);
static_assert(offsetof(CANDIDATELIST, dwSelection) == 0x0C);
static_assert(offsetof(CANDIDATELIST, dwPageStart) == 0x10);
static_assert(offsetof(CANDIDATELIST, dwPageSize) == 0x14);
static_assert(offsetof(CANDIDATELIST, dwOffset) == 0x18);

static const CANDIDATELIST* CurrentCandidateList() noexcept
{
    return static_cast<const CANDIDATELIST*>(platform::gImeCandidateList);
}

static std::uint32_t ReadCandidateOffset(
    const CANDIDATELIST* const candidateList,
    const std::uint32_t candidateIndex) noexcept
{
    std::uint32_t offset = 0;
    const auto* const offsetAddress =
        reinterpret_cast<const std::uint8_t*>(candidateList)
        + 0x18U
        + static_cast<std::size_t>(candidateIndex) * sizeof(std::uint32_t);
    std::memcpy(&offset, offsetAddress, sizeof(offset));
    return offset;
}

static std::wstring CandidateText(
    const CANDIDATELIST* const candidateList,
    const std::uint32_t pageIndex)
{
    const std::uint32_t candidateIndex = candidateList->dwPageStart + pageIndex;
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(candidateList) + ReadCandidateOffset(candidateList, candidateIndex);
    return std::wstring(reinterpret_cast<const wchar_t*>(bytes));
}

static void DrawCandidateEntry(
    const std::uint32_t pageIndex,
    const std::int32_t targetStart,
    std::int32_t& units)
{
    const std::int32_t oldUnits = units; // UTL01：本地累计值现在是实际像素，非旧GBK字节列。
    input::InputContext* const context = input::gCurrentInputData;
    const std::int32_t fontType = context->field13C;
    const std::int32_t height = render::gFontMetrics[static_cast<std::size_t>(fontType)].doubleHeight;
    const std::wstring number = std::to_wstring(std::bit_cast<std::int32_t>(pageIndex + 1U));
    const std::int32_t numberWidth = cg::render::TextMeasure(number, fontType);
    const std::int32_t left = (context->x + targetStart) + oldUnits;
    const std::int32_t top = context->y + height;
    static_cast<void>(render::DrawText2_456670(static_cast<std::int16_t>(left + 5), static_cast<std::int16_t>(top), 4, static_cast<std::int8_t>(fontType), 0, number, 0));
    const CANDIDATELIST* const candidateList = CurrentCandidateList();
    const std::wstring body = CandidateText(candidateList, pageIndex);
    const std::int32_t bodyWidth = cg::render::TextMeasure(body.c_str(), fontType);
    const std::int32_t bodyX = left + numberWidth + 10;
    static_cast<void>(render::DrawText2_456670(static_cast<std::int16_t>(bodyX), static_cast<std::int16_t>(top), 4, static_cast<std::int8_t>(fontType), 0, body.c_str(), 0));
    units = oldUnits + numberWidth + bodyWidth + 15;
    const CANDIDATELIST* const selectionList = CurrentCandidateList();
    const bool selected = selectionList->dwSelection == selectionList->dwPageStart + pageIndex;
    cg::render::DrawGraphicHover(static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top), static_cast<std::uint16_t>(bodyX + bodyWidth), static_cast<std::uint16_t>(top + height),
        selected ? std::uint8_t{0xD9} : std::uint8_t{0xD8}, selected ? 0U : 252U, 1);
}

static void DrawCandidates(
    const std::int32_t targetStart,
    const CANDIDATELIST* candidateList)
{
    std::int32_t units = 0;
    std::uint32_t pageIndex = 0;
    std::uint32_t pageSize;
    do {
        const std::uint32_t pageStart = candidateList->dwPageStart;
        const std::uint32_t count = candidateList->dwCount;
        const std::uint32_t candidateIndex = pageStart + pageIndex;
        if (candidateIndex < count) {
            DrawCandidateEntry(pageIndex, targetStart, units);
            // 原459556只在有效项绘框后重读；跳过项沿本轮表。
            candidateList = CurrentCandidateList();
        }
        pageSize = candidateList->dwPageSize;
        pageIndex += 1U;
    } while (pageIndex < pageSize);
}

// 原 World::RenderInput@[0x4590E0,0x45957E)。
void RenderInput_4590E0()
{
    const wchar_t initialCompositionUnit = platform::gImeComposition[0];
    std::size_t compositionOffset = 0;
    std::int32_t scannedWidth = 0;
    std::int32_t targetStart = 0;
    bool foundTarget = false;

    platform::gImeState73EFF0 = static_cast<std::uint8_t>(
        initialCompositionUnit != 0 ? 1 : 0);
    const std::int32_t isIme = platform::IsImeKeyboardLayout_459090();

    std::array<BYTE, 256> keyboardState;
    static_cast<void>(GetKeyboardState(keyboardState.data()));

    const std::int32_t descriptionGate = gValueD460C8;
    const std::uint8_t capsState = keyboardState[VK_CAPITAL];
    input::InputContext* context = input::gCurrentInputData;
    if (descriptionGate == 0 && context != nullptr) {
        if (isIme != 0) {
            const HKL keyboardLayout = GetKeyboardLayout(0);
            std::array<wchar_t, 100> description{};
            static_cast<void>(ImmGetDescriptionW(keyboardLayout, description.data(), static_cast<UINT>(description.size())));
            const std::wstring contents(description.data());
            std::size_t end = 0;
            const std::int32_t widthLimit = render::gFontMetrics[1].sWidth * 10;
            while (end < contents.size()) {
                const std::size_t next = text::Next(contents, end);
                if (render::Font::measure(contents, 1, static_cast<std::int32_t>(next)) > widthLimit) {
                    break;
                }
                end = next;
            }
            platform::gImeDescription.assign(contents, 0, end);
        } else {
            const auto& description = capsState != 0
                ? kUppercaseDescription
                : kLowercaseDescription;
            platform::gImeDescription = text::FromLegacyDisplayWide(std::string_view(
                reinterpret_cast<const char*>(description.data()), description.size() - 1));
        }

        const std::int16_t descriptionY = gImeDescriptionY5595F0;
        const std::int16_t descriptionX = gImeDescriptionX5595EC;
        static_cast<void>(render::DrawText_456780(
            descriptionX,
            descriptionY,
            3,
            1,
            8,
            platform::gImeDescription,
            0,
            0));
        context = input::gCurrentInputData;
    }

    if (isIme == 0
        || platform::gImeComposition[0] == '\0'
        || context == nullptr) {
        return;
    }
    const std::int32_t renderMode = context->field138;
    const std::uint8_t baseLayer = renderMode == 0 ? 0xD2U : 0xE0U;
    const std::uint8_t compositionModeByte = static_cast<std::uint8_t>(context->field138);
    const std::int32_t compositionFontType = context->field13C;
    const std::int8_t compositionMode = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(compositionModeByte + 1U));
    const std::int32_t compositionY = context->y;
    const std::int32_t compositionX = context->x;
    static_cast<void>(render::DrawText2_456670(
        static_cast<std::int16_t>(compositionX), static_cast<std::int16_t>(compositionY), compositionMode,
        static_cast<std::int8_t>(compositionFontType), 0, platform::gImeComposition, 0));

    input::InputContext* const measureContext = input::gCurrentInputData;
    const std::int32_t compositionWidth = cg::render::TextMeasure(
        platform::gImeComposition, measureContext->field13C);
    input::InputContext* const boxContext = input::gCurrentInputData;
    const std::int32_t boxY = boxContext->y;
    const std::int32_t boxX = boxContext->x;
    const std::int32_t boxFontType = boxContext->field13C;
    const std::int32_t boxHeight = render::gFontMetrics[
        static_cast<std::size_t>(boxFontType)].doubleHeight;
    cg::render::DrawGraphicHover(
        static_cast<std::uint16_t>(boxX - 1), static_cast<std::uint16_t>(boxY - 1),
        static_cast<std::uint16_t>(boxX + compositionWidth), static_cast<std::uint16_t>(boxY + boxHeight),
        baseLayer, 1, 1);

    const std::wstring_view composition(platform::gImeComposition);
    while (compositionOffset < composition.size()) {
        const std::uint8_t attribute = compositionOffset < platform::gImeAttributes.size() ? platform::gImeAttributes[compositionOffset] : 0;
        if (attribute == 1U || attribute == 3U) {
            if (!foundTarget) {
                targetStart = scannedWidth;
                foundTarget = true;
            }
        }
        if (attribute == 2U || attribute == 0U) {
            if (foundTarget || attribute == 0U) {
                break;
            }
        }
        compositionOffset = text::Next(composition, compositionOffset);
        scannedWidth = render::Font::measure(platform::gImeComposition, input::gCurrentInputData->field13C,
            static_cast<std::int32_t>(compositionOffset));
    }

    if (foundTarget) {
        input::InputContext* const highlightContext = input::gCurrentInputData;
        const std::int32_t highlightY = highlightContext->y;
        const std::int32_t highlightX = highlightContext->x;
        const std::int32_t highlightFontType = highlightContext->field13C;
        const std::int32_t highlightHeight = render::gFontMetrics[
            static_cast<std::size_t>(highlightFontType)].doubleHeight;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>((highlightX + targetStart) - 1),
            static_cast<std::uint16_t>(highlightY - 1),
            static_cast<std::uint16_t>(highlightX + scannedWidth),
            static_cast<std::uint16_t>(highlightY + highlightHeight),
            static_cast<std::uint8_t>(baseLayer + 1U), 252, 1);
    }

    const CANDIDATELIST* const candidateList = CurrentCandidateList();
    if (candidateList != nullptr && candidateList->dwPageSize != 0U) {
        DrawCandidates(targetStart, candidateList);
    }
}

} // namespace cg::world::detail
