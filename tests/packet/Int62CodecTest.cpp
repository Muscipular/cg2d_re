#include "packet/Utils.hpp"

#include <array>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

int ExpectEncode(int value, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::EncodeInt62(value);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectDecode(std::string_view text, int expected, const char* message)
{
    const cg::packet::PacketResult<int> result = cg::packet::DecodeInt62(text);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectDecodeError(std::string_view text, const char* message)
{
    const cg::packet::PacketResult<int> result = cg::packet::DecodeInt62(text);
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (result.HasError()) {
        failures += Expect(result.Error().code == cg::packet::PacketErrorCode::DecodeFailed, message);
    }
    return failures;
}

int TestEncode()
{
    int failures = 0;
    failures += ExpectEncode(0, "0", "encode zero");
    failures += ExpectEncode(9, "9", "encode 9");
    failures += ExpectEncode(10, "a", "encode 10");
    failures += ExpectEncode(35, "z", "encode 35");
    failures += ExpectEncode(36, "A", "encode 36");
    failures += ExpectEncode(61, "Z", "encode 61");
    failures += ExpectEncode(62, "10", "encode 62");
    failures += ExpectEncode(3843, "ZZ", "encode 3843");
    failures += ExpectEncode(3844, "100", "encode 3844");
    failures += ExpectEncode(-1, "-1", "encode -1");
    failures += ExpectEncode(-62, "-10", "encode -62");
    failures += ExpectEncode(std::numeric_limits<int>::max(), "2lkCB1", "encode int max");
    failures += ExpectEncode(std::numeric_limits<int>::min(), "-2lkCB2", "encode int min");
    return failures;
}

int TestDecode()
{
    int failures = 0;
    failures += ExpectDecode("", 0, "decode empty as zero");
    failures += ExpectDecode("-", 0, "decode minus as zero");
    failures += ExpectDecode("0", 0, "decode zero");
    failures += ExpectDecode("z", 35, "decode z");
    failures += ExpectDecode("A", 36, "decode A");
    failures += ExpectDecode("Z", 61, "decode Z");
    failures += ExpectDecode("10", 62, "decode 62");
    failures += ExpectDecode("-10", -62, "decode -62");
    failures += ExpectDecode("2lkCB1", std::numeric_limits<int>::max(), "decode int max");
    failures += ExpectDecode("-2lkCB2", std::numeric_limits<int>::min(), "decode int min");
    return failures;
}

int TestDecodeInvalid()
{
    int failures = 0;
    failures += ExpectDecodeError("_", "decode invalid underscore");
    failures += ExpectDecodeError("+1", "decode invalid plus");
    failures += ExpectDecodeError("1-", "decode invalid inner minus");
    failures += ExpectDecodeError("2lkCB2", "decode positive overflow");
    failures += ExpectDecodeError("-2lkCB3", "decode negative overflow");
    return failures;
}

int TestRoundTrip()
{
    constexpr std::array<int, 16> values = {
        0,
        1,
        9,
        10,
        35,
        36,
        61,
        62,
        63,
        3843,
        3844,
        -1,
        -61,
        -62,
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min(),
    };

    int failures = 0;
    for (const int value : values) {
        const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(value);
        failures += Expect(static_cast<bool>(encoded), "roundtrip encode");
        if (!encoded.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<int> decoded = cg::packet::DecodeInt62(encoded.Value());
        failures += Expect(static_cast<bool>(decoded), "roundtrip decode");
        if (decoded.HasValue()) {
            failures += Expect(decoded.Value() == value, "roundtrip value");
        }
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestEncode();
    failures += TestDecode();
    failures += TestDecodeInvalid();
    failures += TestRoundTrip();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
