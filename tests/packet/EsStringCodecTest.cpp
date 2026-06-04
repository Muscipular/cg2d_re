#include "packet/Utils.hpp"

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

int ExpectEncode(std::string_view text, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::EncodeEscapedString(text);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectDecode(std::string_view text, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::DecodeEscapedString(text);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectEncodeError(std::string_view text, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::EncodeEscapedString(text);
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (result.HasError()) {
        failures += Expect(result.Error().code == cg::packet::PacketErrorCode::EncodeFailed, message);
    }
    return failures;
}

int ExpectDecodeError(std::string_view text, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::DecodeEscapedString(text);
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
    failures += ExpectEncode("a b", "a\\Sb", "encode space");
    failures += ExpectEncode("line\nret\rslash\\", "line\\nret\\rslash\\\\", "encode controls");
    failures += ExpectEncode(Bytes({0x17U}), Bytes({0x17U}), "encode token separator unchanged");

    const std::string source = Bytes({0x81U, ' ', 0x82U, '\\', ' ', '\\'});
    std::string expected = Bytes({0x81U, ' ', 0x82U, '\\'});
    expected += "\\S\\\\";
    failures += ExpectEncode(source, expected, "encode legacy double-byte pairs");
    failures += ExpectEncodeError(Bytes({0x81U}), "encode truncated double-byte pair");
    return failures;
}

int TestDecode()
{
    int failures = 0;
    failures += ExpectDecode("a\\Sb", "a b", "decode space");
    failures += ExpectDecode("line\\nret\\rslash\\\\", "line\nret\rslash\\", "decode controls");
    failures += ExpectDecode(Bytes({0x17U}), Bytes({0x17U}), "decode token separator unchanged");
    failures += ExpectDecode("\\", "\\", "decode trailing slash");
    failures += ExpectDecode("\\xY", "\\Y", "decode unknown escape");

    const std::string encoded = Bytes({0x81U, '\\'});
    std::string mixed = encoded;
    mixed += "\\S";
    std::string expected = encoded;
    expected.push_back(' ');
    failures += ExpectDecode(mixed, expected, "decode legacy double-byte pair");
    failures += ExpectDecodeError(Bytes({0x81U}), "decode truncated double-byte pair");
    return failures;
}

int TestRoundTrip()
{
    std::string source = "hello world\nret\rslash\\";
    source.push_back('\x17');
    source += Bytes({0x81U, ' ', 0x82U, '\\'});

    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(source);
    int failures = Expect(static_cast<bool>(encoded), "roundtrip encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> decoded = cg::packet::DecodeEscapedString(encoded.Value());
    failures += Expect(static_cast<bool>(decoded), "roundtrip decode");
    if (decoded.HasValue()) {
        failures += Expect(decoded.Value() == source, "roundtrip value");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestEncode();
    failures += TestDecode();
    failures += TestRoundTrip();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
