#include "packet/Send.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::CustomTitleInputRequest;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

std::string Bytes(std::initializer_list<unsigned int> bytes)
{
    std::string value;
    value.reserve(bytes.size());
    for (const unsigned int byte : bytes) {
        value.push_back(static_cast<char>(byte));
    }
    return value;
}

template <typename T>
int ExpectError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    bool hasFieldIndex,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }
    failures += Expect(error.fieldIndex.has_value() == hasFieldIndex, message);
    if (hasFieldIndex && error.fieldIndex.has_value()) {
        failures += Expect(*error.fieldIndex == fieldIndex, message);
    }
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view title,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("ACwB"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "title", message);
        failures += Expect(
            packet.fields[0].type ==
                cg::packet::PacketFieldType::EscapedString,
            message);
        failures += Expect(packet.fields[0].value == title, message);
    }
    return failures;
}

int TestSpecAndGbkEncode()
{
    const std::string title = Bytes({0xD3U, 0xC2U, 0xD5U, 0xDFU});
    const CustomTitleInputRequest request(title);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(CustomTitleInputRequest::Code == "ACwB", "ACwB code");
    failures += Expect(
        CustomTitleInputRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "ACwB direction");
    failures += Expect(
        CustomTitleInputRequest::Fields.size() == 1U,
        "ACwB fields");
    failures += Expect(
        CustomTitleInputRequest::Fields[0].name == "title",
        "ACwB field name");
    failures += Expect(
        CustomTitleInputRequest::Fields[0].type ==
            cg::packet::PacketFieldType::EscapedString,
        "ACwB field type");
    failures += Expect(
        CustomTitleInputRequest::Spec.code == CustomTitleInputRequest::Code,
        "ACwB Spec code");
    failures += Expect(
        CustomTitleInputRequest::Spec.direction ==
            CustomTitleInputRequest::Direction,
        "ACwB Spec direction");
    failures += Expect(
        CustomTitleInputRequest::Spec.fieldCount == 1U,
        "ACwB Spec field count");
    failures += Expect(request.title == title, "ACwB member");
    failures += ExpectPacket(packet, title, "ACwB packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "ACwB GBK Encode");
    if (encoded.HasValue()) {
        std::string expected = "ACwB ";
        expected.append(title);
        expected.push_back(' ');
        failures += Expect(encoded.Value() == expected, "ACwB GBK canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const std::string title = Bytes({0xD3U, 0xC2U, 0xD5U, 0xDFU});
    std::string wire = "ACwB ";
    wire.append(title);

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(wire, CustomTitleInputRequest::Spec);

    int failures = Expect(parsed.HasValue(), "ACwB generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), title, "ACwB generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            CustomTitleInputRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "ACwB generic build");
    if (rebuilt.HasValue()) {
        wire.push_back(' ');
        failures += Expect(
            rebuilt.Value() == wire,
            "ACwB generic canonical");
    }
    return failures;
}

int TestEscapedTitle()
{
    int failures = 0;

    const CustomTitleInputRequest empty("");
    const cg::packet::PacketResult<std::string> emptyEncoded = empty.Encode();
    failures += Expect(emptyEncoded.HasValue(), "ACwB empty Encode");
    if (emptyEncoded.HasValue()) {
        failures += Expect(
            emptyEncoded.Value() == "ACwB  ",
            "ACwB empty canonical");
    }

    const std::string freeText = "A B\nC\rD\\E";
    const CustomTitleInputRequest freeTextRequest(freeText);
    const cg::packet::PacketResult<std::string> freeTextEncoded =
        freeTextRequest.Encode();
    failures += Expect(freeTextEncoded.HasValue(), "ACwB free text Encode");
    if (freeTextEncoded.HasValue()) {
        failures += Expect(
            freeTextEncoded.Value() == "ACwB A\\SB\\nC\\rD\\\\E ",
            "ACwB free text canonical");
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(
                freeTextEncoded.Value(),
                CustomTitleInputRequest::Spec);
        failures += Expect(parsed.HasValue(), "ACwB free text parse");
        if (parsed.HasValue()) {
            failures += ExpectPacket(
                parsed.Value(),
                freeText,
                "ACwB free text roundtrip");
        }
    }

    const std::string gbkTrailPipe = Bytes({0x81U, 0x7CU});
    const CustomTitleInputRequest gbkRequest(gbkTrailPipe);
    const cg::packet::PacketResult<std::string> gbkEncoded =
        gbkRequest.Encode();
    failures += Expect(gbkEncoded.HasValue(), "ACwB GBK trail pipe Encode");
    if (gbkEncoded.HasValue()) {
        std::string expected = "ACwB ";
        expected.append(gbkTrailPipe);
        expected.push_back(' ');
        failures += Expect(
            gbkEncoded.Value() == expected,
            "ACwB GBK trail pipe canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad title ", CustomTitleInputRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ACwB wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("ACwB", CustomTitleInputRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ACwB",
        true,
        0U,
        "ACwB missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "ACwB title extra ",
            CustomTitleInputRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ACwB",
        true,
        1U,
        "ACwB extra field");
    std::string truncatedGbk = "ACwB ";
    truncatedGbk.push_back(static_cast<char>(0x81U));
    failures += ExpectError(
        cg::packet::ParsePacket(
            truncatedGbk,
            CustomTitleInputRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ACwB",
        true,
        0U,
        "ACwB truncated GBK");

    const std::string pollutedCode = "ACwB\x17" "title";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedCode, CustomTitleInputRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "ACwB polluted code");

    const std::string pollutedField = "ACwB title\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, CustomTitleInputRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ACwB",
        true,
        1U,
        "ACwB polluted field");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndGbkEncode()
        + TestGenericRoundtrip()
        + TestEscapedTitle()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " custom title input request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
