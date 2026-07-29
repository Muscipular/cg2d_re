#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::EchoRequest;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
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

int TestSpecToPacketAndEncode()
{
    const EchoRequest request("nr");
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(EchoRequest::Code == "Or", "Or code");
    failures += Expect(
        EchoRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "Or direction");
    failures += Expect(EchoRequest::Fields.size() == 1U, "Or fields");
    failures += Expect(EchoRequest::Fields[0].name == "payload", "Or field name");
    failures += Expect(
        EchoRequest::Fields[0].type == cg::packet::PacketFieldType::EscapedString,
        "Or field type");
    failures += Expect(EchoRequest::Spec.code == EchoRequest::Code, "Or Spec code");
    failures += Expect(EchoRequest::Spec.direction == EchoRequest::Direction, "Or Spec direction");
    failures += Expect(EchoRequest::Spec.fieldCount == 1U, "Or Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("Or"), "Or packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "Or packet direction");
    failures += Expect(packet.fields.size() == 1U, "Or packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "payload", "Or packet field name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "Or packet field type");
        failures += Expect(packet.fields[0].value == "nr", "Or packet field value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "Or Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "Or nr ", "Or Encode value");
    }

    const cg::packet::PacketResult<std::string> escaped = EchoRequest("n r").Encode();
    failures += Expect(escaped.HasValue(), "Or escaped Encode");
    if (escaped.HasValue()) {
        failures += Expect(escaped.Value() == "Or n\\Sr ", "Or escaped Encode value");
    }
    return failures;
}

int TestGenericCanonicalRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("Or n\\Sr ", EchoRequest::Spec);

    int failures = Expect(parsed.HasValue(), "Or ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.size() == 1U, "Or parsed fields");
    if (parsed.Value().fields.size() == 1U) {
        failures += Expect(parsed.Value().fields[0].name == "payload", "Or parsed name");
        failures += Expect(
            parsed.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "Or parsed type");
        failures += Expect(parsed.Value().fields[0].value == "n r", "Or parsed value");
    }

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), EchoRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "Or BuildPacket");
    if (!rebuilt.HasValue()) {
        return failures;
    }
    failures += Expect(rebuilt.Value() == "Or n\\Sr ", "Or canonical");

    const cg::packet::PacketResult<cg::packet::Packet> reparsed =
        cg::packet::ParsePacket(rebuilt.Value(), EchoRequest::Spec);
    failures += Expect(reparsed.HasValue(), "Or reparse canonical");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad nr ", EchoRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Or wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("Or", EchoRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Or",
        true,
        0U,
        "Or missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("Or nr extra ", EchoRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Or",
        true,
        1U,
        "Or extra field");

    std::string invalidEs = "Or ";
    invalidEs.push_back(static_cast<char>(0x80U));
    invalidEs.push_back(' ');
    failures += ExpectError(
        cg::packet::ParsePacket(invalidEs, EchoRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Or",
        true,
        0U,
        "Or invalid es");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecToPacketAndEncode() + TestGenericCanonicalRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " echo request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
