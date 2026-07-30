#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::DropPet;

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

int TestSpecAndEncode()
{
    const DropPet request(10, 20, 0);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(DropPet::Code == "FvWm", "FvWm code");
    failures += Expect(
        DropPet::Direction == cg::packet::PacketDirection::ClientToServer,
        "FvWm direction");
    failures += Expect(DropPet::Fields.size() == 3U, "FvWm fields");
    failures += Expect(DropPet::Fields[0].name == "x", "FvWm x name");
    failures += Expect(DropPet::Fields[1].name == "y", "FvWm y name");
    failures += Expect(DropPet::Fields[2].name == "petindex", "FvWm petindex name");
    for (const cg::packet::PacketFieldSpec& field : DropPet::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "FvWm field type");
    }
    failures += Expect(DropPet::Spec.code == DropPet::Code, "FvWm Spec code");
    failures += Expect(DropPet::Spec.direction == DropPet::Direction, "FvWm Spec direction");
    failures += Expect(DropPet::Spec.fieldCount == 3U, "FvWm Spec field count");
    failures += Expect(request.x == 10, "FvWm x member");
    failures += Expect(request.y == 20, "FvWm y member");
    failures += Expect(request.petindex == 0, "FvWm petindex member");
    failures += Expect(packet.code == cg::packet::PacketCode("FvWm"), "FvWm packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "FvWm packet direction");
    failures += Expect(packet.fields.size() == 3U, "FvWm packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "FvWm x field");
    failures += ExpectField(packet, 1U, "y", "20", "FvWm y field");
    failures += ExpectField(packet, 2U, "petindex", "0", "FvWm petindex field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "FvWm Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "FvWm a k 0 ", "FvWm Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("FvWm a k 0", DropPet::Spec);

    int failures = Expect(parsed.HasValue(), "FvWm generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "FvWm parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "FvWm parsed y");
    failures += ExpectField(parsed.Value(), 2U, "petindex", "0", "FvWm parsed petindex");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), DropPet::Spec);
    failures += Expect(rebuilt.HasValue(), "FvWm generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "FvWm a k 0 ", "FvWm generic canonical");
    }
    return failures;
}

int TestSignedAndUnrestrictedValues()
{
    int failures = 0;
    const cg::packet::PacketResult<std::string> signedValues =
        DropPet(-1, -2, -3).Encode();
    failures += Expect(signedValues.HasValue(), "FvWm signed values");
    if (signedValues.HasValue()) {
        failures += Expect(
            signedValues.Value() == "FvWm -1 -2 -3 ",
            "FvWm signed canonical");
    }

    const cg::packet::PacketResult<std::string> unrestrictedPetIndex =
        DropPet(0, 0, 5).Encode();
    failures += Expect(unrestrictedPetIndex.HasValue(), "FvWm unrestricted petindex");
    if (unrestrictedPetIndex.HasValue()) {
        failures += Expect(
            unrestrictedPetIndex.Value() == "FvWm 0 0 5 ",
            "FvWm unrestricted petindex canonical");
    }

    const DropPet extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min());
    const cg::packet::PacketResult<std::string> extremeEncoded = extremes.Encode();
    failures += Expect(extremeEncoded.HasValue(), "FvWm extreme values");
    if (extremeEncoded.HasValue()) {
        failures += Expect(
            extremeEncoded.Value() == "FvWm -2lkCB2 2lkCB1 -2lkCB2 ",
            "FvWm extreme canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 3U>& tokens)
{
    return "FvWm " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k 0 ", DropPet::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "FvWm wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("FvWm a k", DropPet::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "FvWm",
        true,
        2U,
        "FvWm missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("FvWm a k 0 1 ", DropPet::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "FvWm",
        true,
        3U,
        "FvWm extra field");

    const std::array<std::string, 3U> validTokens = {{"a", "k", "0"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 3U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), DropPet::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "FvWm",
            true,
            index,
            "FvWm invalid i62");

        std::array<std::string, 3U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), DropPet::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "FvWm",
            true,
            index,
            "FvWm i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndUnrestrictedValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " drop pet packet test(s) failed\n";
        return 1;
    }
    return 0;
}
