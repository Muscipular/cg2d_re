#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::CharacterLoginRequest;

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

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

std::string BuildPlain(const std::array<std::string, 3U>& tokens)
{
    return "emRV " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ';
}

int TestSpecAndEncode()
{
    const CharacterLoginRequest request(62, -1, 36);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(CharacterLoginRequest::Code == "emRV", "emRV code");
    failures += Expect(
        CharacterLoginRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "emRV direction");
    failures += Expect(CharacterLoginRequest::Fields.size() == 3U, "emRV fields");
    failures += Expect(CharacterLoginRequest::Fields[0].name == "registnumber", "emRV registnumber name");
    failures += Expect(CharacterLoginRequest::Fields[1].name == "renewal", "emRV renewal name");
    failures += Expect(CharacterLoginRequest::Fields[2].name == "face", "emRV face name");
    for (const cg::packet::PacketFieldSpec& field : CharacterLoginRequest::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "emRV field type");
    }
    failures += Expect(CharacterLoginRequest::Spec.code == CharacterLoginRequest::Code, "emRV Spec code");
    failures += Expect(
        CharacterLoginRequest::Spec.direction == CharacterLoginRequest::Direction,
        "emRV Spec direction");
    failures += Expect(CharacterLoginRequest::Spec.fieldCount == 3U, "emRV Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("emRV"), "emRV packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "emRV packet direction");
    failures += Expect(packet.fields.size() == 3U, "emRV packet fields");
    failures += ExpectField(packet, 0U, "registnumber", "62", "emRV registnumber field");
    failures += ExpectField(packet, 1U, "renewal", "-1", "emRV renewal field");
    failures += ExpectField(packet, 2U, "face", "36", "emRV face field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "emRV Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "emRV 10 -1 A ", "emRV Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("emRV 10 -1 A ", CharacterLoginRequest::Spec);

    int failures = Expect(parsed.HasValue(), "emRV ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "registnumber", "62", "emRV parsed registnumber");
    failures += ExpectField(parsed.Value(), 1U, "renewal", "-1", "emRV parsed renewal");
    failures += ExpectField(parsed.Value(), 2U, "face", "36", "emRV parsed face");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), CharacterLoginRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "emRV BuildPacket");
    if (!rebuilt.HasValue()) {
        return failures;
    }
    failures += Expect(rebuilt.Value() == "emRV 10 -1 A ", "emRV canonical");

    const cg::packet::PacketResult<cg::packet::Packet> reparsed =
        cg::packet::ParsePacket(rebuilt.Value(), CharacterLoginRequest::Spec);
    failures += Expect(reparsed.HasValue(), "emRV reparse canonical");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 10 -1 A ", CharacterLoginRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "emRV wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("emRV 10 -1", CharacterLoginRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "emRV",
        true,
        2U,
        "emRV missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("emRV 10 -1 A 0 ", CharacterLoginRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "emRV",
        true,
        3U,
        "emRV extra field");

    const std::array<std::string, 3U> validTokens = {"10", "-1", "A"};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 3U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), CharacterLoginRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "emRV",
            true,
            index,
            "emRV invalid i62");

        std::array<std::string, 3U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), CharacterLoginRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "emRV",
            true,
            index,
            "emRV i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " character login request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
