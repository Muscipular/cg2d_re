#include "WorldUpdateDependencies.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../platform/ImeLifecycle.hpp"
#include "../platform/StartupNative.hpp"
#include "../platform/WindowProcedureState.hpp"

#include "cg/Input.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <imm.h>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib> // 原CRT接口：0x0054ABDD；原参数／返回合同由直接调用承接。

namespace cg::world::detail {

std::int32_t gValueD460C8 = 0; // 原全局：0x00D460C8。
// 原.data两说明坐标；本体按Y→X独立读取，保持S16和已加载初值。
std::int16_t gImeDescriptionX5595EC = 542; // 原全局：0x005595EC。
std::int16_t gImeDescriptionY5595F0 = 461; // 原全局：0x005595F0。

namespace {

inline constexpr std::array<std::uint8_t, 12> kLowercaseDescription {{
    0xAD, 0x5E, 0xBC, 0xC6, 0x2F, 0xA2,
    0xE9, 0xA2, 0xEA, 0xA2, 0xEB, 0x00,
}};
inline constexpr std::array<std::uint8_t, 12> kUppercaseDescription {{
    0xAD, 0x5E, 0xBC, 0xC6, 0x2F, 0xA2,
    0xCF, 0xA2, 0xD0, 0xA2, 0xD1, 0x00,
}};

static_assert(sizeof(DWORD) == sizeof(std::uint32_t));
static_assert(offsetof(CANDIDATELIST, dwCount) == 0x08);
static_assert(offsetof(CANDIDATELIST, dwSelection) == 0x0C);
static_assert(offsetof(CANDIDATELIST, dwPageStart) == 0x10);
static_assert(offsetof(CANDIDATELIST, dwPageSize) == 0x14);
static_assert(offsetof(CANDIDATELIST, dwOffset) == 0x18);

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return left + right;
}

std::int32_t SubW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return left - right;
}

std::int32_t MulW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return left * right;
}

std::int16_t LowS16(const std::int32_t value) noexcept
{
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        std::bit_cast<std::uint32_t>(value)));
}

std::uint16_t LowU16(const std::int32_t value) noexcept
{
    return static_cast<std::uint16_t>(std::bit_cast<std::uint32_t>(value));
}

std::int8_t LowS8(const std::int32_t value) noexcept
{
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(
        std::bit_cast<std::uint32_t>(value)));
}

std::uint32_t ReadField138Bits(
    const input::InputContext& context) noexcept
{
    std::uint32_t bits = 0;
    std::memcpy(&bits, context.field138, sizeof(bits));
    return bits;
}

const CANDIDATELIST* CurrentCandidateList() noexcept
{
    return static_cast<const CANDIDATELIST*>(platform::gImeCandidateList);
}

std::uint32_t ReadCandidateOffset(
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

const char* CandidateText(
    const CANDIDATELIST* const candidateList,
    const std::uint32_t pageIndex) noexcept
{
    const std::uint32_t candidateIndex =
        candidateList->dwPageStart + pageIndex;
    return reinterpret_cast<const char*>(candidateList)
        + ReadCandidateOffset(candidateList, candidateIndex);
}

char* FormatDecimalW32(
    const std::uint32_t valueBits,
    std::array<char, 12>& destination) noexcept
{
    return ::_itoa(
        std::bit_cast<std::int32_t>(valueBits), destination.data(), 10);
}

void DrawCandidateEntry(
    const std::uint32_t pageIndex,
    const std::int32_t targetStart,
    std::int32_t& units)
{
    const std::int32_t oldUnits = units;
    input::InputContext* const numberFontContext = input::gCurrentInputData;
    const std::int32_t numberFontType = numberFontContext->field13C;
    const std::int32_t numberHeight = render::gFontMetrics[
        static_cast<std::size_t>(numberFontType)].doubleHeight;
    std::array<char, 12> numberText;
    const char* const formattedNumber = FormatDecimalW32(pageIndex + 1U, numberText);

    input::InputContext* const numberPositionContext = input::gCurrentInputData;
    const std::int32_t numberY = numberPositionContext->y;
    const std::int32_t numberXBase = numberPositionContext->x;
    const std::int32_t numberX = AddW32(
        AddW32(AddW32(numberXBase, MulW32(oldUnits, numberHeight)), targetStart), 5);
    static_cast<void>(render::DrawText2_456670(
        LowS16(numberX), LowS16(AddW32(numberY, numberHeight)),
        4, LowS8(numberFontType), 0, formattedNumber, 0));

    input::InputContext* const bodyFontContext = input::gCurrentInputData;
    const std::int32_t bodyUnits = AddW32(oldUnits, 1);
    const std::int32_t bodyFontType = bodyFontContext->field13C;
    units = bodyUnits;
    const std::int32_t bodyHeight = render::gFontMetrics[
        static_cast<std::size_t>(bodyFontType)].doubleHeight;
    const CANDIDATELIST* const pageStartList = CurrentCandidateList();
    const std::uint32_t bodyCandidateIndex = pageStartList->dwPageStart + pageIndex;
    const CANDIDATELIST* const offsetList = CurrentCandidateList();
    const char* const bodyText = reinterpret_cast<const char*>(offsetList)
        + ReadCandidateOffset(offsetList, bodyCandidateIndex);
    input::InputContext* const bodyPositionContext = input::gCurrentInputData;
    const std::int32_t bodyY = bodyPositionContext->y;
    const std::int32_t bodyXBase = bodyPositionContext->x;
    const std::int32_t bodyX = AddW32(
        AddW32(bodyXBase, MulW32(bodyUnits, bodyHeight)), targetStart);
    static_cast<void>(render::DrawText2_456670(
        LowS16(bodyX), LowS16(AddW32(bodyY, bodyHeight)),
        4, LowS8(bodyFontType), 0, bodyText, 0));

    const CANDIDATELIST* const lengthList = CurrentCandidateList();
    const char* const measuredText = CandidateText(lengthList, pageIndex);
    const std::uint32_t lengthBits = static_cast<std::uint32_t>(std::strlen(measuredText));
    std::int32_t length = std::bit_cast<std::int32_t>(lengthBits);
    if (length < 2) {
        length = 2;
    }
    units = AddW32(bodyUnits, length / 2);
    input::InputContext* const measureContext = input::gCurrentInputData;
    const std::int32_t measureFontType = measureContext->field13C;
    const std::int32_t measuredWidth = login::detail::TextMeasure(measuredText, measureFontType);

    const CANDIDATELIST* const selectionList = CurrentCandidateList();
    const std::uint32_t selectionPageStart = selectionList->dwPageStart;
    const std::uint32_t selection = selectionList->dwSelection;
    const std::uint32_t selectedIndex = selectionPageStart + pageIndex;
    const bool isSelected = selection == selectedIndex;
    input::InputContext* const boxContext = input::gCurrentInputData;
    const std::int32_t boxFontType = boxContext->field13C;
    const std::int32_t boxY = boxContext->y;
    const std::int32_t boxX = boxContext->x;
    const std::int32_t boxHeight = render::gFontMetrics[
        static_cast<std::size_t>(boxFontType)].doubleHeight;
    const std::int32_t left = AddW32(
        AddW32(boxX, MulW32(oldUnits, boxHeight)), targetStart);
    const std::int32_t top = AddW32(boxY, boxHeight);
    const std::int32_t right = AddW32(
        AddW32(AddW32(boxX, MulW32(bodyUnits, boxHeight)), targetStart), measuredWidth);
    const std::int32_t bottom = AddW32(boxY, MulW32(boxHeight, 2));
    login::detail::DrawGraphicHover(
        LowU16(left), LowU16(top), LowU16(right), LowU16(bottom),
        isSelected ? std::uint8_t {0xD9} : std::uint8_t {0xD8},
        isSelected ? 0U : 252U, 1);
}

void DrawCandidates(
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

} // namespace

// 原 World::RenderInput@[0x4590E0,0x45957E)。
void RenderInput_4590E0()
{
    const char initialCompositionByte = platform::gImeComposition[0];
    std::size_t compositionOffset = 0;
    std::size_t attributeOffset = 0;
    std::int32_t scannedWidth = 0;
    std::int32_t targetStart = 0;
    bool foundTarget = false;

    platform::gImeState73EFF0 = static_cast<std::uint8_t>(
        static_cast<unsigned char>(initialCompositionByte));
    const std::int32_t isIme = platform::IsImeKeyboardLayout_459090();

    std::array<BYTE, 256> keyboardState;
    static_cast<void>(GetKeyboardState(keyboardState.data()));

    const std::int32_t descriptionGate = gValueD460C8;
    const std::uint8_t capsState = keyboardState[VK_CAPITAL];
    input::InputContext* context = input::gCurrentInputData;
    if (descriptionGate == 0 && context != nullptr) {
        if (isIme != 0) {
            const HKL keyboardLayout = GetKeyboardLayout(0);
            static_cast<void>(ImmGetDescriptionA(
                keyboardLayout,
                platform::gImeDescription,
                static_cast<UINT>(sizeof(platform::gImeDescription))));
            platform::gImeDescription[10] = '\0';
        } else {
            const auto& description = capsState != 0
                ? kUppercaseDescription
                : kLowercaseDescription;
            std::memcpy(
                platform::gImeDescription,
                description.data(),
                description.size());
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
    const std::uint32_t field138Bits = ReadField138Bits(*context);
    const std::uint8_t baseLayer = field138Bits == 0U ? 0xD2U : 0xE0U;
    const std::uint8_t compositionModeByte = context->field138[0];
    const std::int32_t compositionFontType = context->field13C;
    const std::int8_t compositionMode = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(compositionModeByte + 1U));
    const std::int32_t compositionY = context->y;
    const std::int32_t compositionX = context->x;
    static_cast<void>(render::DrawText2_456670(
        LowS16(compositionX), LowS16(compositionY), compositionMode,
        LowS8(compositionFontType), 0, platform::gImeComposition, 0));

    input::InputContext* const measureContext = input::gCurrentInputData;
    const std::int32_t compositionWidth = login::detail::TextMeasure(
        platform::gImeComposition, measureContext->field13C);
    input::InputContext* const boxContext = input::gCurrentInputData;
    const std::int32_t boxY = boxContext->y;
    const std::int32_t boxX = boxContext->x;
    const std::int32_t boxFontType = boxContext->field13C;
    const std::int32_t boxHeight = render::gFontMetrics[
        static_cast<std::size_t>(boxFontType)].doubleHeight;
    login::detail::DrawGraphicHover(
        LowU16(SubW32(boxX, 1)), LowU16(SubW32(boxY, 1)),
        LowU16(AddW32(boxX, compositionWidth)), LowU16(AddW32(boxY, boxHeight)),
        baseLayer, 1, 1);

    while (true) {
        const std::uint8_t attribute =
            platform::gImeAttributes[attributeOffset];
        if (attribute == 1U || attribute == 3U) {
            if (!foundTarget) {
                targetStart = scannedWidth;
                foundTarget = true;
            }
        }
        if (attribute == 2U || attribute == 0U) {
            if (foundTarget) {
                break;
            }
            if (attribute == 0U) {
                break;
            }
        }

        const std::uint8_t firstByte = static_cast<std::uint8_t>(
            static_cast<unsigned char>(
                platform::gImeComposition[compositionOffset]));
        const bool isDoubleByte = (firstByte & 0x80U) != 0U;
        if (isDoubleByte) {
            compositionOffset += 2U;
            attributeOffset += 2U;
            input::InputContext* const metricContext =
                input::gCurrentInputData;
            const std::int32_t metricFontType = metricContext->field13C;
            const render::FontMetrics& metrics = render::gFontMetrics[
                static_cast<std::size_t>(metricFontType)];
            scannedWidth = AddW32(scannedWidth, metrics.doubleWidth);
        } else {
            input::InputContext* const metricContext =
                input::gCurrentInputData;
            compositionOffset += 1U;
            const std::int32_t metricFontType = metricContext->field13C;
            attributeOffset += 1U;
            const render::FontMetrics& metrics = render::gFontMetrics[
                static_cast<std::size_t>(metricFontType)];
            scannedWidth = AddW32(scannedWidth, metrics.sWidth);
        }
    }

    if (foundTarget) {
        input::InputContext* const highlightContext = input::gCurrentInputData;
        const std::int32_t highlightY = highlightContext->y;
        const std::int32_t highlightX = highlightContext->x;
        const std::int32_t highlightFontType = highlightContext->field13C;
        const std::int32_t highlightHeight = render::gFontMetrics[
            static_cast<std::size_t>(highlightFontType)].doubleHeight;
        login::detail::DrawGraphicHover(
            LowU16(SubW32(AddW32(highlightX, targetStart), 1)),
            LowU16(SubW32(highlightY, 1)),
            LowU16(AddW32(highlightX, scannedWidth)),
            LowU16(AddW32(highlightY, highlightHeight)),
            static_cast<std::uint8_t>(baseLayer + 1U), 252, 1);
    }

    const CANDIDATELIST* const candidateList = CurrentCandidateList();
    if (candidateList != nullptr && candidateList->dwPageSize != 0U) {
        DrawCandidates(targetStart, candidateList);
    }
}

} // namespace cg::world::detail
