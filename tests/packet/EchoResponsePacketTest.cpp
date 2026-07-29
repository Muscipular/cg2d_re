#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::EchoResponse;

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

int TestSpecAndToPacket()
{
    const EchoResponse response("nr");
    const cg::packet::Packet packet = response.ToPacket();

    int failures = 0;
    failures += Expect(EchoResponse::Code == "LOh", "LOh code");
    failures += Expect(
        EchoResponse::Direction == cg::packet::PacketDirection::ServerToClient,
        "LOh direction");
    failures += Expect(EchoResponse::Fields.size() == 1U, "LOh fields");
    failures += Expect(EchoResponse::Fields[0].name == "payload", "LOh field name");
    failures += Expect(
        EchoResponse::Fields[0].type == cg::packet::PacketFieldType::EscapedString,
        "LOh field type");
    failures += Expect(EchoResponse::Spec.code == EchoResponse::Code, "LOh Spec code");
    failures += Expect(EchoResponse::Spec.direction == EchoResponse::Direction, "LOh Spec direction");
    failures += Expect(EchoResponse::Spec.fieldCount == 1U, "LOh Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("LOh"), "LOh packet code");
    failures += Expect(packet.direction == EchoResponse::Direction, "LOh packet direction");
    failures += Expect(packet.fields.size() == 1U, "LOh packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "payload", "LOh packet field name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "LOh packet field type");
        failures += Expect(packet.fields[0].value == "nr", "LOh packet field value");
    }
    return failures;
}

int TestCanonicalAndGenericRoundtrip()
{
    const cg::packet::PacketResult<EchoResponse> typed =
        EchoResponse::Parse("LOh nr ");

    int failures = Expect(typed.HasValue(), "LOh typed parse");
    if (!typed.HasValue()) {
        return failures;
    }
    failures += Expect(typed.Value().payload == "nr", "LOh typed payload");

    const cg::packet::PacketResult<std::string> typedBuilt =
        cg::packet::BuildPacket(typed.Value().ToPacket(), EchoResponse::Spec);
    failures += Expect(typedBuilt.HasValue(), "LOh typed build");
    if (typedBuilt.HasValue()) {
        failures += Expect(typedBuilt.Value() == "LOh nr ", "LOh typed canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("LOh nr", EchoResponse::Spec);
    failures += Expect(generic.HasValue(), "LOh generic parse");
    if (generic.HasValue()) {
        const cg::packet::PacketResult<std::string> genericBuilt =
            cg::packet::BuildPacket(generic.Value(), EchoResponse::Spec);
        failures += Expect(genericBuilt.HasValue(), "LOh generic build");
        if (genericBuilt.HasValue()) {
            failures += Expect(genericBuilt.Value() == "LOh nr ", "LOh generic canonical");
        }
    }
    return failures;
}

int TestEmptyAndEscapedValues()
{
    const cg::packet::PacketResult<EchoResponse> empty = EchoResponse::Parse("LOh  ");

    int failures = Expect(empty.HasValue(), "LOh empty parse");
    if (empty.HasValue()) {
        failures += Expect(empty.Value().payload.empty(), "LOh empty payload");
        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(empty.Value().ToPacket(), EchoResponse::Spec);
        failures += Expect(built.HasValue(), "LOh empty build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == "LOh  ", "LOh empty canonical");
        }
    }

    const std::string wire = "LOh arbitrary\\Svalue\\\\tail ";
    const cg::packet::PacketResult<EchoResponse> escaped = EchoResponse::Parse(wire);
    failures += Expect(escaped.HasValue(), "LOh escaped parse");
    if (escaped.HasValue()) {
        failures += Expect(
            escaped.Value().payload == "arbitrary value\\tail",
            "LOh escaped payload");
        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(escaped.Value().ToPacket(), EchoResponse::Spec);
        failures += Expect(built.HasValue(), "LOh escaped build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == wire, "LOh escaped canonical");
        }
    }
    return failures;
}

int TestGbkRoundtrip()
{
    std::string wire = "LOh ";
    wire.push_back(static_cast<char>(0x81U));
    wire.push_back(static_cast<char>(0x7CU));
    wire.push_back(' ');

    const cg::packet::PacketResult<EchoResponse> parsed = EchoResponse::Parse(wire);
    int failures = Expect(parsed.HasValue(), "LOh GBK parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().payload.size() == 2U, "LOh GBK payload size");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), EchoResponse::Spec);
    failures += Expect(built.HasValue(), "LOh GBK build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == wire, "LOh GBK canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        EchoResponse::Parse("bad nr "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "LOh wrong code");
    failures += ExpectError(
        EchoResponse::Parse("LOh"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LOh",
        true,
        0U,
        "LOh missing field");
    failures += ExpectError(
        EchoResponse::Parse("LOh nr extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LOh",
        true,
        1U,
        "LOh extra field");

    std::string invalid = "LOh ";
    invalid.push_back(static_cast<char>(0x80U));
    invalid.push_back(' ');
    failures += ExpectError(
        EchoResponse::Parse(invalid),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LOh",
        true,
        0U,
        "LOh invalid es");

    std::string truncated = "LOh ";
    truncated.push_back(static_cast<char>(0x81U));
    truncated.push_back(' ');
    failures += ExpectError(
        EchoResponse::Parse(truncated),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LOh",
        true,
        0U,
        "LOh truncated GBK");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestCanonicalAndGenericRoundtrip()
        + TestEmptyAndEscapedValues()
        + TestGbkRoundtrip()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " echo response packet test(s) failed\n";
        return 1;
    }
    return 0;
}
