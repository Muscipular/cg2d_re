#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::send::PetSkillSwap::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
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

int ExpectPetSkillSwapPacket(const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::PetSkillSwap::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "petSlot", "1", message);
    failures += ExpectField(packet, 1U, "from", "2", message);
    failures += ExpectField(packet, 2U, "to", "3", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::PetSkillSwap request(1, 2, 3);

    int failures = ExpectPetSkillSwapPacket(request.ToPacket(), "HJQp ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "HJQp BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "HJQp 1 2 3 ", "HJQp BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "HJQp Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "HJQp 1 2 3 ", "HJQp Encode value");
    }
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("HJQp 1 2 3 ");

    int failures = Expect(static_cast<bool>(parsed), "parse HJQp");
    if (parsed.HasValue()) {
        failures += ExpectPetSkillSwapPacket(parsed.Value(), "parse HJQp packet");
    }
    return failures;
}

int TestInt62Values()
{
    const cg::packet::send::PetSkillSwap request(10, 61, 62);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "HJQp Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "HJQp a Z 10 ", "HJQp i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "HJQp parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectField(parsed.Value(), 0U, "petSlot", "10", "HJQp parsed petSlot 10");
        failures += ExpectField(parsed.Value(), 1U, "from", "61", "HJQp parsed from 61");
        failures += ExpectField(parsed.Value(), 2U, "to", "62", "HJQp parsed to 62");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("HJQp 1 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HJQp",
        true,
        2U,
        "HJQp missing field");
    failures += ExpectError(
        ParsePacket("HJQp 1 2 3 4 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HJQp",
        true,
        3U,
        "HJQp extra field");
    failures += ExpectError(
        ParsePacket("HJQp _ 2 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HJQp",
        true,
        0U,
        "HJQp bad petSlot");
    failures += ExpectError(
        ParsePacket("HJQp 1 _ 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HJQp",
        true,
        1U,
        "HJQp bad from");
    failures += ExpectError(
        ParsePacket("HJQp 1 2 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HJQp",
        true,
        2U,
        "HJQp bad to");
    failures += ExpectError(
        ParsePacket("HJQx 1 2 3 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "HJQx",
        false,
        0U,
        "HJQp wrong code");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::PetSkillSwap(1, 2, 3).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "HJQp",
        false,
        0U,
        "HJQp direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::PetSkillSwap(1, 2, 3).ToPacket();
    wrongName.fields[1].name = "wrong";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "HJQp",
        true,
        1U,
        "HJQp field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::PetSkillSwap(1, 2, 3).ToPacket();
    wrongType.fields[2].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "HJQp",
        true,
        2U,
        "HJQp field type mismatch");

    cg::packet::Packet emptyValue = cg::packet::send::PetSkillSwap(1, 2, 3).ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        BuildPacket(emptyValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "HJQp",
        true,
        0U,
        "HJQp empty i62 value");

    cg::packet::Packet badValue = cg::packet::send::PetSkillSwap(1, 2, 3).ToPacket();
    badValue.fields[2].value = "abc";
    failures += ExpectError(
        BuildPacket(badValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "HJQp",
        true,
        2U,
        "HJQp bad i62 value");
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
