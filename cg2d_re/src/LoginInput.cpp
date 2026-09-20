#include "cg/LoginInput.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/Palette.hpp"

#include <cstddef>

namespace cg::login {

input::InputContext gInputContext9A5960 {};
input::InputContext gInputContext9A5678 {};

std::int16_t gInputContext9A5960X = 0;
std::int16_t gInputContext9A5960Y = 0;
std::int16_t gInputContext9A5678X = 0;
std::int16_t gInputContext9A5678Y = 0;
std::int32_t gValue9A6E20 = 0;

input::InputContext* gInputContextFocusTable[3] {
    &gInputContext9A5960,
    &gInputContext9A5678,
    &gInputContext9A5960,
};

namespace {

inline constexpr std::size_t kStoredInputValueSize = 51;

char gValueFF494F[kStoredInputValueSize] {};
char gValueFF4982[kStoredInputValueSize] {};

char* ReadStoredInputValue(
    char* const output,
    const char (&storedValue)[kStoredInputValueSize])
{
    for (std::size_t index = 0; index < kStoredInputValueSize; ++index) {
        const char value = storedValue[index];
        output[index] = value;
        if (value == '\0') {
            return output;
        }
    }
    return output;
}

} // namespace

char* ReadInputValueFF494F_5203E0(char* const output)
{
    if (output == nullptr) {
        return nullptr;
    }
    return ReadStoredInputValue(output, gValueFF494F);
}

char* ReadInputValueFF4982_520440(char* const output)
{
    if (output == nullptr) {
        return nullptr;
    }
    return ReadStoredInputValue(output, gValueFF4982);
}

void InitializeInputContexts_45C7C0()
{
    char firstInitialText[256] {};
    char secondInitialText[256] {};

    const std::int32_t capability =
        resource::ProbePaletteCapability_45C970();
    const bool useHighLayout = capability >= 1;
    render::gPaletteCapability = capability;

    if (useHighLayout) {
        gInputContext9A5960X = 114;
        gInputContext9A5960Y = 341;
        gInputContext9A5960.text[0] = '\0';
        gInputContext9A5960.totalByteLength = 0;
        gInputContext9A5960.caretByteOffset = 0;
        ReadInputValueFF494F_5203E0(firstInitialText);
        input::InitializeContext_449CF0(
            &gInputContext9A5960,
            gInputContext9A5960X,
            gInputContext9A5960Y,
            0,
            0,
            3,
            firstInitialText,
            1,
            20,
            0,
            1);
        gInputContext9A5960.encrypt = 1;

        gInputContext9A5678X = 114;
        gInputContext9A5678Y = 309;
        gInputContext9A5678.text[0] = '\0';
        gInputContext9A5678.totalByteLength = 0;
        gInputContext9A5678.caretByteOffset = 0;
        ReadInputValueFF4982_520440(secondInitialText);
        input::InitializeContext_449CF0(
            &gInputContext9A5678,
            gInputContext9A5678X,
            gInputContext9A5678Y,
            0,
            0,
            3,
            secondInitialText,
            1,
            20,
            0,
            0);
    } else {
        gInputContext9A5960X = 335;
        gInputContext9A5960Y = 300;
        gInputContext9A5960.text[0] = '\0';
        gInputContext9A5960.totalByteLength = 0;
        gInputContext9A5960.caretByteOffset = 0;
        ReadInputValueFF494F_5203E0(firstInitialText);
        input::InitializeContext_449CF0(
            &gInputContext9A5960,
            gInputContext9A5960X,
            gInputContext9A5960Y,
            0,
            0,
            3,
            firstInitialText,
            1,
            20,
            0,
            0);
        gInputContext9A5960.encrypt = 1;

        gInputContext9A5678X = 335;
        gInputContext9A5678Y = 260;
        gInputContext9A5678.text[0] = '\0';
        gInputContext9A5678.totalByteLength = 0;
        gInputContext9A5678.caretByteOffset = 0;
        ReadInputValueFF4982_520440(secondInitialText);
        input::InitializeContext_449CF0(
            &gInputContext9A5678,
            gInputContext9A5678X,
            gInputContext9A5678Y,
            0,
            0,
            3,
            secondInitialText,
            1,
            23,
            0,
            0);
    }

    input::InputContext* const initialContext =
        gInputContextFocusTable[1];
    gValue9A6E20 = 1;
    input::SetContext_44B330(initialContext);
}

} // namespace cg::login
