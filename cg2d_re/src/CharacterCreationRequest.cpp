#include "cg/CharacterCreationRequest.hpp"

#include "CharacterSelectState.hpp"

#include "cg/LoginRequest.hpp"

#include <cstddef>
#include <cstring>

namespace {

constexpr std::size_t kRegistNumberOffset = 0x4C;
constexpr std::size_t kPreviewToggleOffset = 0x50;
constexpr char kSuccessfulResult[] = "successful";

static_assert(kRegistNumberOffset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(kPreviewToggleOffset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(sizeof(kSuccessfulResult) == 11);

} // namespace

namespace cg::login {

std::uint16_t gCharacterCreationReplyCode = 0;

void HandleCharacterCreationResponse_508EE0(
    const std::uintptr_t,
    const char* const result,
    const std::int32_t registNumber)
{
    if (gRequestStatus != 1) {
        return;
    }

    gRequestStatus = 2;
    std::size_t index = 0;
    for (;;) {
        const char expected = kSuccessfulResult[index];
        if (result[index] != expected) {
            return;
        }
        if (expected == '\0') {
            break;
        }
        ++index;
    }

    const std::int16_t selected = gSelectedCharacterIndex;
    gCharacterCreationReplyCode = 1;
    const std::int32_t previewToggle =
        character_select::state::gPreviewToggleState;
    CharacterRecordBytes* const record = gCharacterRecords.data() + selected;
    std::memcpy(
        record->data() + kRegistNumberOffset,
        &registNumber,
        sizeof(registNumber));
    std::memcpy(
        record->data() + kPreviewToggleOffset,
        &previewToggle,
        sizeof(previewToggle));
}

} // namespace cg::login
