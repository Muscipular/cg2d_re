#include "packet/utils.hpp"

#include <initializer_list>
#include <iostream>
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

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string text;
    text.reserve(values.size());
    for (const unsigned int value : values) {
        text.push_back(static_cast<char>(value));
    }
    return text;
}

int ExpectEncode64(std::string_view bytes, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::Encode64(bytes);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectDecode64(std::string_view text, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::Decode64(text);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectDecode64Error(std::string_view text, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::Decode64(text);
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (result.HasError()) {
        failures += Expect(result.Error().code == cg::packet::PacketErrorCode::DecodeFailed, message);
    }
    return failures;
}

int TestEncode64()
{
    int failures = 0;
    failures += ExpectEncode64("", "", "encode64 empty");
    failures += ExpectEncode64(Bytes({0x00U}), "AA", "encode64 one byte");
    failures += ExpectEncode64(Bytes({0x00U, 0x00U}), "AAA", "encode64 two bytes");
    failures += ExpectEncode64(Bytes({0x00U, 0x00U, 0x00U}), "AAAA", "encode64 three bytes");
    failures += ExpectEncode64(Bytes({0xffU, 0xffU, 0xffU}), "----", "encode64 0xff triplet");
    return failures;
}

int TestDecode64()
{
    int failures = 0;
    failures += ExpectDecode64("", "", "decode64 empty");
    failures += ExpectDecode64("A", "", "decode64 single tail ignored");
    failures += ExpectDecode64("AA", Bytes({0x00U}), "decode64 one byte");
    failures += ExpectDecode64("AAA", Bytes({0x00U, 0x00U}), "decode64 two bytes");
    failures += ExpectDecode64("AAAA", Bytes({0x00U, 0x00U, 0x00U}), "decode64 three bytes");
    failures += ExpectDecode64("----", Bytes({0xffU, 0xffU, 0xffU}), "decode64 0xff triplet");
    failures += ExpectDecode64Error("_", "decode64 invalid char");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestEncode64();
    failures += TestDecode64();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
