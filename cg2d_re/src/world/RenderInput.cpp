#include "WorldUpdateDependencies.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../platform/ImeLifecycle.hpp"
#include "../platform/StartupNative.hpp"
#include "../platform/WindowProcedureState.hpp"

#include "cg/Input.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <imm.h>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::world::detail {

std::int32_t gValueD460C8 = 0;

namespace {

inline constexpr std::size_t kImeBufferCapacity = 256;
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
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(right));
}

std::int32_t SubW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        - std::bit_cast<std::uint32_t>(right));
}

std::int32_t MulW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        * std::bit_cast<std::uint32_t>(right));
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

bool IsValidFontType(const std::int32_t fontType) noexcept
{
    return fontType >= 0
        && fontType < static_cast<std::int32_t>(render::kFontTypeCount);
}

std::uint32_t ReadField138Bits(
    const input::InputContext& context) noexcept
{
    std::uint32_t bits = 0;
    std::memcpy(&bits, context.field138, sizeof(bits));
    return bits;
}

bool HasSafeCompositionText() noexcept
{
    std::size_t offset = 0;
    while (offset < kImeBufferCapacity) {
        const std::uint8_t firstByte = static_cast<std::uint8_t>(
            static_cast<unsigned char>(platform::gImeComposition[offset]));
        if (firstByte == 0) {
            return true;
        }
        if ((firstByte & 0x80U) != 0U) {
            if (offset + 1U >= kImeBufferCapacity) {
                return false;
            }
            offset += 2U;
        } else {
            ++offset;
        }
    }
    return false;
}

bool CanScanCompositionAttributes() noexcept
{
    std::size_t compositionOffset = 0;
    std::size_t attributeOffset = 0;
    bool foundTarget = false;

    while (compositionOffset < kImeBufferCapacity
           && attributeOffset < kImeBufferCapacity) {
        const std::uint8_t attribute =
            platform::gImeAttributes[attributeOffset];
        if (attribute == 1U || attribute == 3U) {
            foundTarget = true;
        }
        if (attribute == 0U || (attribute == 2U && foundTarget)) {
            return true;
        }

        const std::uint8_t firstByte = static_cast<std::uint8_t>(
            static_cast<unsigned char>(
                platform::gImeComposition[compositionOffset]));
        const std::size_t step = (firstByte & 0x80U) != 0U ? 2U : 1U;
        if (step == 2U && compositionOffset + 1U >= kImeBufferCapacity) {
            return false;
        }
        compositionOffset += step;
        attributeOffset += step;
    }
    return false;
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
    if (candidateList == nullptr) {
        return nullptr;
    }

    const std::uint32_t candidateIndex =
        candidateList->dwPageStart + pageIndex;
    if (candidateIndex >= candidateList->dwCount) {
        return nullptr;
    }
    return reinterpret_cast<const char*>(candidateList)
        + ReadCandidateOffset(candidateList, candidateIndex);
}

void FormatDecimalW32(
    const std::uint32_t valueBits,
    std::array<char, 12>& destination) noexcept
{
    std::size_t outputLength = 0;
    std::uint32_t magnitude = valueBits;
    if (std::bit_cast<std::int32_t>(valueBits) < 0) {
        destination[outputLength++] = '-';
        magnitude = 0U - valueBits;
    }

    std::array<char, 10> reversed {};
    std::size_t digitCount = 0;
    do {
        reversed[digitCount++] = static_cast<char>(
            '0' + static_cast<char>(magnitude % 10U));
        magnitude /= 10U;
    } while (magnitude != 0U);

    while (digitCount != 0U) {
        destination[outputLength++] = reversed[--digitCount];
    }
    destination[outputLength] = '\0';
}

void DrawCandidateEntry(
    const std::uint32_t pageIndex,
    const std::int32_t targetStart,
    std::int32_t& units)
{
    const std::int32_t oldUnits = units;

    input::InputContext* const numberFontContext =
        input::gCurrentInputData;
    std::int32_t numberFontType = 0;
    std::int32_t numberHeight = 0;
    bool canDrawNumber = false;
    if (numberFontContext != nullptr) {
        numberFontType = numberFontContext->field13C;
        if (IsValidFontType(numberFontType)) {
            numberHeight = render::gFontMetrics[
                static_cast<std::size_t>(numberFontType)].doubleHeight;
            canDrawNumber = true;
        }
    }

    std::array<char, 12> numberText {};
    FormatDecimalW32(pageIndex + 1U, numberText);

    input::InputContext* const numberPositionContext =
        input::gCurrentInputData;
    if (canDrawNumber && numberPositionContext != nullptr) {
        const std::int32_t numberY = numberPositionContext->y;
        const std::int32_t numberXBase = numberPositionContext->x;
        const std::int32_t numberX = AddW32(
            AddW32(
                AddW32(
                    numberXBase,
                    MulW32(oldUnits, numberHeight)),
                targetStart),
            5);
        const std::int32_t drawY = AddW32(
            numberY,
            numberHeight);
        static_cast<void>(render::DrawText2_456670(
            LowS16(numberX),
            LowS16(drawY),
            4,
            LowS8(numberFontType),
            0,
            numberText.data(),
            0));
    }

    const std::int32_t bodyUnits = AddW32(oldUnits, 1);
    input::InputContext* const bodyFontContext = input::gCurrentInputData;
    std::int32_t bodyFontType = 0;
    std::int32_t bodyHeight = 0;
    bool canDrawBody = false;
    if (bodyFontContext != nullptr) {
        bodyFontType = bodyFontContext->field13C;
        if (IsValidFontType(bodyFontType)) {
            bodyHeight = render::gFontMetrics[
                static_cast<std::size_t>(bodyFontType)].doubleHeight;
            canDrawBody = true;
        }
    }

    const CANDIDATELIST* const pageStartList = CurrentCandidateList();
    if (pageStartList == nullptr) {
        return;
    }
    const std::uint32_t bodyCandidateIndex =
        pageStartList->dwPageStart + pageIndex;
    const CANDIDATELIST* const offsetList = CurrentCandidateList();
    const char* bodyText = nullptr;
    if (offsetList != nullptr
        && bodyCandidateIndex < offsetList->dwCount) {
        bodyText = reinterpret_cast<const char*>(offsetList)
            + ReadCandidateOffset(offsetList, bodyCandidateIndex);
    }

    input::InputContext* const bodyPositionContext =
        input::gCurrentInputData;
    if (canDrawBody
        && bodyText != nullptr
        && bodyPositionContext != nullptr) {
        const std::int32_t bodyY = bodyPositionContext->y;
        const std::int32_t bodyXBase = bodyPositionContext->x;
        const std::int32_t bodyX = AddW32(
            AddW32(
                bodyXBase,
                MulW32(bodyUnits, bodyHeight)),
            targetStart);
        const std::int32_t drawY = AddW32(
            bodyY,
            bodyHeight);
        static_cast<void>(render::DrawText2_456670(
            LowS16(bodyX),
            LowS16(drawY),
            4,
            LowS8(bodyFontType),
            0,
            bodyText,
            0));
    }

    const CANDIDATELIST* const lengthList = CurrentCandidateList();
    const char* const measuredText = CandidateText(lengthList, pageIndex);
    if (measuredText == nullptr) {
        return;
    }

    const std::uint32_t lengthBits = static_cast<std::uint32_t>(
        std::strlen(measuredText));
    std::int32_t length = std::bit_cast<std::int32_t>(lengthBits);
    if (length < 2) {
        length = 2;
    }
    units = AddW32(bodyUnits, length / 2);

    input::InputContext* const measureContext = input::gCurrentInputData;
    std::int32_t measuredWidth = 0;
    bool hasMeasuredWidth = false;
    if (measureContext != nullptr) {
        const std::int32_t measureFontType = measureContext->field13C;
        measuredWidth = login::detail::TextMeasure(
            measuredText,
            measureFontType);
        hasMeasuredWidth = true;
    }

    const CANDIDATELIST* const selectionList = CurrentCandidateList();
    if (selectionList == nullptr) {
        return;
    }
    const std::uint32_t selectedIndex =
        selectionList->dwPageStart + pageIndex;
    if (selectedIndex >= selectionList->dwCount) {
        return;
    }
    const bool isSelected = selectionList->dwSelection == selectedIndex;

    input::InputContext* const boxContext = input::gCurrentInputData;
    if (!hasMeasuredWidth || boxContext == nullptr) {
        return;
    }
    const std::int32_t boxFontType = boxContext->field13C;
    const std::int32_t boxY = boxContext->y;
    const std::int32_t boxX = boxContext->x;
    if (!IsValidFontType(boxFontType)) {
        return;
    }
    const std::int32_t boxHeight = render::gFontMetrics[
        static_cast<std::size_t>(boxFontType)].doubleHeight;
    const std::int32_t left = AddW32(
        AddW32(boxX, MulW32(oldUnits, boxHeight)),
        targetStart);
    const std::int32_t top = AddW32(boxY, boxHeight);
    const std::int32_t right = AddW32(
        AddW32(
            AddW32(
                boxX,
                MulW32(bodyUnits, boxHeight)),
            targetStart),
        measuredWidth);
    const std::int32_t bottom = AddW32(
        boxY,
        MulW32(boxHeight, 2));
    login::detail::DrawGraphicHover(
        LowU16(left),
        LowU16(top),
        LowU16(right),
        LowU16(bottom),
        isSelected ? std::uint8_t {0xD9} : std::uint8_t {0xD8},
        isSelected ? 0U : 252U,
        1);
}

void DrawCandidates(
    const std::int32_t targetStart,
    const CANDIDATELIST* candidateList)
{
    std::int32_t units = 0;
    std::uint32_t pageIndex = 0;
    do {
        const std::uint32_t candidateIndex =
            candidateList->dwPageStart + pageIndex;
        if (candidateIndex < candidateList->dwCount) {
            DrawCandidateEntry(pageIndex, targetStart, units);
        }

        pageIndex += 1U;
        candidateList = CurrentCandidateList();
    } while (candidateList != nullptr
             && pageIndex < candidateList->dwPageSize);
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

    std::array<BYTE, 256> keyboardState {};
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

        static_cast<void>(render::DrawText_456780(
            542,
            461,
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
    if (HasSafeCompositionText()) {
        const std::uint8_t compositionModeByte = context->field138[0];
        const std::int32_t compositionFontType = context->field13C;
        const std::int8_t compositionMode = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(compositionModeByte + 1U));
        const std::int32_t compositionY = context->y;
        const std::int32_t compositionX = context->x;
        static_cast<void>(render::DrawText2_456670(
            LowS16(compositionX),
            LowS16(compositionY),
            compositionMode,
            LowS8(compositionFontType),
            0,
            platform::gImeComposition,
            0));

        input::InputContext* const measureContext =
            input::gCurrentInputData;
        std::int32_t compositionWidth = 0;
        bool hasCompositionWidth = false;
        if (measureContext != nullptr) {
            compositionWidth = login::detail::TextMeasure(
                platform::gImeComposition,
                measureContext->field13C);
            hasCompositionWidth = true;
        }

        input::InputContext* const boxContext = input::gCurrentInputData;
        if (hasCompositionWidth && boxContext != nullptr) {
            const std::int32_t boxY = boxContext->y;
            const std::int32_t boxX = boxContext->x;
            const std::int32_t boxFontType = boxContext->field13C;
            if (IsValidFontType(boxFontType)) {
                const std::int32_t boxHeight = render::gFontMetrics[
                    static_cast<std::size_t>(boxFontType)].doubleHeight;
                login::detail::DrawGraphicHover(
                    LowU16(SubW32(boxX, 1)),
                    LowU16(SubW32(boxY, 1)),
                    LowU16(AddW32(boxX, compositionWidth)),
                    LowU16(AddW32(boxY, boxHeight)),
                    baseLayer,
                    1,
                    1);
            }
        }
    }

    if (!CanScanCompositionAttributes()) {
        return;
    }

    bool attributeScanValid = true;
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
            if (metricContext == nullptr) {
                attributeScanValid = false;
                break;
            }
            const std::int32_t metricFontType = metricContext->field13C;
            if (!IsValidFontType(metricFontType)) {
                attributeScanValid = false;
                break;
            }
            const render::FontMetrics& metrics = render::gFontMetrics[
                static_cast<std::size_t>(metricFontType)];
            scannedWidth = AddW32(scannedWidth, metrics.doubleWidth);
        } else {
            input::InputContext* const metricContext =
                input::gCurrentInputData;
            compositionOffset += 1U;
            if (metricContext == nullptr) {
                attributeScanValid = false;
                break;
            }
            const std::int32_t metricFontType = metricContext->field13C;
            attributeOffset += 1U;
            if (!IsValidFontType(metricFontType)) {
                attributeScanValid = false;
                break;
            }
            const render::FontMetrics& metrics = render::gFontMetrics[
                static_cast<std::size_t>(metricFontType)];
            scannedWidth = AddW32(scannedWidth, metrics.sWidth);
        }
    }

    if (!attributeScanValid && !foundTarget) {
        return;
    }

    if (foundTarget && attributeScanValid) {
        input::InputContext* const highlightContext =
            input::gCurrentInputData;
        if (highlightContext != nullptr) {
            const std::int32_t highlightY = highlightContext->y;
            const std::int32_t highlightX = highlightContext->x;
            const std::int32_t highlightFontType =
                highlightContext->field13C;
            if (IsValidFontType(highlightFontType)) {
                const std::int32_t highlightHeight = render::gFontMetrics[
                    static_cast<std::size_t>(
                        highlightFontType)].doubleHeight;
                login::detail::DrawGraphicHover(
                    LowU16(SubW32(
                        AddW32(highlightX, targetStart),
                        1)),
                    LowU16(SubW32(highlightY, 1)),
                    LowU16(AddW32(highlightX, scannedWidth)),
                    LowU16(AddW32(highlightY, highlightHeight)),
                    static_cast<std::uint8_t>(baseLayer + 1U),
                    252,
                    1);
            }
        }
    }

    const CANDIDATELIST* const candidateList = CurrentCandidateList();
    if (candidateList != nullptr && candidateList->dwPageSize != 0U) {
        DrawCandidates(targetStart, candidateList);
    }
}

} // namespace cg::world::detail
