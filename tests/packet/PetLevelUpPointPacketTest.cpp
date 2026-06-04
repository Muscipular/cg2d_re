#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::PetLevelUpPoint::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::PetLevelUpPoint::Spec);
}

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
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }

    if (hasFieldIndex) {
        failures += Expect(error.fieldIndex.has_value(), message);
        if (error.fieldIndex.has_value()) {
            failures += Expect(*error.fieldIndex == fieldIndex, message);
        }
    } else {
        failures += Expect(!error.fieldIndex.has_value(), message);
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
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectPetLevelUpPointPacket(const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::PetLevelUpPoint::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "petSlot", "2", message);
    failures += ExpectField(packet, 1U, "sqeno", "4", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::PetLevelUpPoint request(2, 4);

    int failures = ExpectPetLevelUpPointPacket(request.ToPacket(), "kjSK ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "kjSK BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "kjSK 2 4 ", "kjSK BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "kjSK Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "kjSK 2 4 ", "kjSK Encode value");
    }
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("kjSK 2 4 ");

    int failures = Expect(static_cast<bool>(parsed), "parse kjSK");
    if (parsed.HasValue()) {
        failures += ExpectPetLevelUpPointPacket(parsed.Value(), "parse kjSK packet");
    }
    return failures;
}

int TestInt62Values()
{
    const cg::packet::send::PetLevelUpPoint request(10, -1);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "kjSK Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "kjSK a -1 ", "kjSK i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "kjSK parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectField(parsed.Value(), 0U, "petSlot", "10", "kjSK parsed petSlot 10");
        failures += ExpectField(parsed.Value(), 1U, "sqeno", "-1", "kjSK parsed sqeno -1");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("kjSK 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kjSK",
        true,
        1U,
        "kjSK missing field");
    failures += ExpectError(
        ParsePacket("kjSK 2 4 6 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kjSK",
        true,
        2U,
        "kjSK extra field");
    failures += ExpectError(
        ParsePacket("kjSK _ 4 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kjSK",
        true,
        0U,
        "kjSK bad petSlot");
    failures += ExpectError(
        ParsePacket("kjSK 2 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kjSK",
        true,
        1U,
        "kjSK bad sqeno");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::PetLevelUpPoint(2, 4).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "kjSK",
        false,
        0U,
        "kjSK direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::PetLevelUpPoint(2, 4).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "kjSK",
        true,
        0U,
        "kjSK field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::PetLevelUpPoint(2, 4).ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "kjSK",
        true,
        1U,
        "kjSK field type mismatch");

    cg::packet::Packet emptyValue = cg::packet::send::PetLevelUpPoint(2, 4).ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        BuildPacket(emptyValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "kjSK",
        true,
        0U,
        "kjSK empty i62 value");

    cg::packet::Packet badValue = cg::packet::send::PetLevelUpPoint(2, 4).ToPacket();
    badValue.fields[1].value = "abc";
    failures += ExpectError(
        BuildPacket(badValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "kjSK",
        true,
        1U,
        "kjSK bad i62 value");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestParsePacket();
    failures += TestInt62Values();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
