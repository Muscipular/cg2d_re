#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::send::PetSkillUseRequest::Spec;
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
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectPetSkillUseRequestPacket(const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::PetSkillUseRequest::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 4U, message);
    if (packet.fields.size() != 4U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "havepetindex", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 1U, "havepetskill", cg::packet::PacketFieldType::Int62, "2", message);
    failures += ExpectField(packet, 2U, "toindex", cg::packet::PacketFieldType::Int62, "3", message);
    failures += ExpectField(packet, 3U, "data", cg::packet::PacketFieldType::EscapedString, "option", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::PetSkillUseRequest request(1, 2, 3, "option");

    int failures = ExpectPetSkillUseRequestPacket(request.ToPacket(), "Flvp ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "Flvp BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "Flvp 1 2 3 option ", "Flvp BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "Flvp Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "Flvp 1 2 3 option ", "Flvp Encode value");
    }
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("Flvp 1 2 3 option ");

    int failures = Expect(static_cast<bool>(parsed), "parse Flvp");
    if (parsed.HasValue()) {
        failures += ExpectPetSkillUseRequestPacket(parsed.Value(), "parse Flvp packet");
    }

    const cg::packet::PacketResult<cg::packet::Packet> emptyData =
        ParsePacket("Flvp 1 2 3  ");
    failures += Expect(static_cast<bool>(emptyData), "parse Flvp empty data");
    if (emptyData.HasValue()) {
        failures += ExpectField(
            emptyData.Value(),
            3U,
            "data",
            cg::packet::PacketFieldType::EscapedString,
            "",
            "Flvp parsed empty data");
    }
    return failures;
}

int TestInt62Values()
{
    const cg::packet::send::PetSkillUseRequest request(10, -1, 6, "");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "Flvp Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "Flvp a -1 6  ", "Flvp i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "Flvp parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            0U,
            "havepetindex",
            cg::packet::PacketFieldType::Int62,
            "10",
            "Flvp parsed havepetindex 10");
        failures += ExpectField(
            parsed.Value(),
            1U,
            "havepetskill",
            cg::packet::PacketFieldType::Int62,
            "-1",
            "Flvp parsed havepetskill -1");
        failures += ExpectField(
            parsed.Value(),
            2U,
            "toindex",
            cg::packet::PacketFieldType::Int62,
            "6",
            "Flvp parsed toindex 6");
    }
    return failures;
}

int TestEscapedDataRoundtrip()
{
    const cg::packet::send::PetSkillUseRequest request(1, 2, 3, "a b\nc\\d");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "Flvp Encode escaped data");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "Flvp 1 2 3 a\\Sb\\nc\\\\d ", "Flvp escaped value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "Flvp parse escaped data");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            3U,
            "data",
            cg::packet::PacketFieldType::EscapedString,
            request.data,
            "Flvp roundtrip data");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("Flvp 1 2 3"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Flvp",
        true,
        3U,
        "Flvp missing data");
    failures += ExpectError(
        ParsePacket("Flvp 1 2 3 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Flvp",
        true,
        4U,
        "Flvp extra field");
    failures += ExpectError(
        ParsePacket("Flvp _ 2 3 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Flvp",
        true,
        0U,
        "Flvp bad havepetindex");
    failures += ExpectError(
        ParsePacket("Flvp 1 _ 3 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Flvp",
        true,
        1U,
        "Flvp bad havepetskill");
    failures += ExpectError(
        ParsePacket("Flvp 1 2 _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Flvp",
        true,
        2U,
        "Flvp bad toindex");

    std::string truncatedData = "Flvp 1 2 3 ";
    truncatedData.push_back(static_cast<char>(0x80U));
    truncatedData.push_back(' ');
    failures += ExpectError(
        ParsePacket(truncatedData),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Flvp",
        true,
        3U,
        "Flvp truncated data");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestParsePacket();
    failures += TestInt62Values();
    failures += TestEscapedDataRoundtrip();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
