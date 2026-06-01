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

int ExpectEnvelopeEncode(std::string_view payload, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::EncodeEnvelope(payload);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectEnvelopeDecode(std::string_view text, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::DecodeEnvelope(text);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectErrorCode(const cg::packet::PacketResult<std::string>& result,
                    cg::packet::PacketErrorCode code,
                    std::string_view messagePart,
                    const char* message)
{
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (result.HasError()) {
        failures += Expect(result.Error().code == code, message);
        failures += Expect(result.Error().message.find(messagePart) != std::string::npos, message);
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

int TestJEncodeRoundTrip()
{
    const std::string source = Bytes({0x00U, 0x80U, 0xffU, 'A', 0x00U, 'z'});
    const cg::packet::PacketResult<std::string> encoded = cg::packet::JEncode(source);
    int failures = Expect(static_cast<bool>(encoded), "jEncode bytes");
    if (!encoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> decoded = cg::packet::JDecode(encoded.Value());
    failures += Expect(static_cast<bool>(decoded), "jDecode bytes");
    if (decoded.HasValue()) {
        failures += Expect(decoded.Value() == source, "jEncode roundtrip bytes");
    }
    return failures;
}

int TestEnvelopeSamples()
{
    int failures = 0;
    failures += ExpectEnvelopeEncode("", "AgIH", "envelope empty sample");
    failures += ExpectEnvelopeEncode("A", "AgFDhg", "envelope A sample");
    failures += ExpectEnvelopeEncode("AB", "BEXJhw0", "envelope AB sample");
    failures += ExpectEnvelopeEncode("hello", "BrF-GsfUqgA", "envelope hello sample");
    failures += ExpectEnvelopeDecode("AgIH", "", "decode empty sample");
    failures += ExpectEnvelopeDecode("AgFDhg", "A", "decode A sample");
    failures += ExpectEnvelopeDecode("BEXJhw0", "AB", "decode AB sample");
    failures += ExpectEnvelopeDecode("BrF-GsfUqgA", "hello", "decode hello sample");
    return failures;
}

int TestEnvelopeBoundary()
{
    int failures = 0;

    const std::string payload98(98U, 'x');
    const cg::packet::PacketResult<std::string> encoded98 = cg::packet::EncodeEnvelope(payload98);
    failures += Expect(static_cast<bool>(encoded98), "envelope 98 encode");
    if (encoded98.HasValue()) {
        const cg::packet::PacketResult<std::string> decoded98 = cg::packet::DecodeEnvelope(encoded98.Value());
        failures += Expect(static_cast<bool>(decoded98), "envelope 98 decode");
        if (decoded98.HasValue()) {
            failures += Expect(decoded98.Value() == payload98, "envelope 98 roundtrip");
        }
    }

    const std::string payload99(99U, 'y');
    failures += ExpectErrorCode(
        cg::packet::EncodeEnvelope(payload99),
        cg::packet::PacketErrorCode::NotImplemented,
        "ringo",
        "envelope 99 default not implemented");

    const cg::packet::PacketResult<std::string> encodedNoCompress = cg::packet::EncodeEnvelope(payload99, true);
    failures += Expect(static_cast<bool>(encodedNoCompress), "envelope 99 noCompress encode");
    if (encodedNoCompress.HasValue()) {
        const cg::packet::PacketResult<std::string> decodedNoCompress =
            cg::packet::DecodeEnvelope(encodedNoCompress.Value());
        failures += Expect(static_cast<bool>(decodedNoCompress), "envelope 99 noCompress decode");
        if (decodedNoCompress.HasValue()) {
            failures += Expect(decodedNoCompress.Value() == payload99, "envelope 99 noCompress roundtrip");
        }
    }

    failures += ExpectEnvelopeDecode("BrF-GsfUqgA\n", "hello", "decode newline");
    failures += ExpectEnvelopeDecode("BrF-GsfUqgA\r\n", "hello", "decode crlf");
    return failures;
}

int TestEnvelopeCompressedDecode()
{
    const std::string oddFlagPlain = Bytes({0x03U, 0x00U});
    const cg::packet::PacketResult<std::string> jEncoded = cg::packet::JEncode(oddFlagPlain);
    int failures = Expect(static_cast<bool>(jEncoded), "odd flag jEncode");
    if (!jEncoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> encoded = cg::packet::Encode64(jEncoded.Value());
    failures += Expect(static_cast<bool>(encoded), "odd flag encode64");
    if (encoded.HasValue()) {
        failures += ExpectErrorCode(
            cg::packet::DecodeEnvelope(encoded.Value()),
            cg::packet::PacketErrorCode::NotImplemented,
            "ringo",
            "odd flag decode not implemented");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestEncode64();
    failures += TestDecode64();
    failures += TestJEncodeRoundTrip();
    failures += TestEnvelopeSamples();
    failures += TestEnvelopeBoundary();
    failures += TestEnvelopeCompressedDecode();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
