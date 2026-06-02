#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::PetTechData::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::PetTechData::Spec);
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
    return failures;
}

std::string BuildPlainObee(int petIndex, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPetIndex = cg::packet::EncodeInt62(petIndex);
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPetIndex.HasValue() || !encodedPayload.HasValue()) {
        return "OBee  ";
    }

    std::string plain = "OBee ";
    plain.append(encodedPetIndex.Value());
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectPetTechDataPacket(
    const cg::packet::Packet& packet,
    int petIndex,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::PetTechData::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "petIndex", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(petIndex), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == value, message);
    return failures;
}

int ExpectRecord(
    const cg::packet::receive::PetTechRecord& record,
    int skillSlot,
    int techId,
    std::string_view name,
    std::string_view description,
    int forcePoint,
    int techField,
    int target,
    int seqNo,
    const char* message)
{
    int failures = 0;
    failures += Expect(record.skillSlot == skillSlot, message);
    failures += Expect(record.techId == techId, message);
    failures += Expect(record.name == name, message);
    failures += Expect(record.description == description, message);
    failures += Expect(record.forcePoint == forcePoint, message);
    failures += Expect(record.techField == techField, message);
    failures += Expect(record.target == target, message);
    failures += Expect(record.seqNo == seqNo, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "0|100|Bite|desc|30|1|2|7|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("OBee 2 0|100|Bite|desc|30|1|2|7| ");

    int failures = Expect(static_cast<bool>(parsed), "parse OBee outer");
    if (parsed.HasValue()) {
        failures += ExpectPetTechDataPacket(parsed.Value(), 2, payload, "parse OBee outer packet");
    }
    return failures;
}

int TestSingleRecord()
{
    const std::string payload = "0|100|Bite|desc|30|1|2|7|";
    const cg::packet::PacketResult<cg::packet::receive::PetTechData> parsed =
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse OBee single record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 2, "parse OBee petIndex");
    failures += Expect(parsed.Value().records.size() == 1U, "parse OBee record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], 0, 100, "Bite", "desc", 30, 1, 2, 7, "parse OBee record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "build OBee payload");
    return failures;
}

int TestMultiRecordRoundtrip()
{
    std::vector<cg::packet::receive::PetTechRecord> records;
    records.push_back(cg::packet::receive::PetTechRecord {0, 100, "Bite", "desc", 30, 1, 2, 7});
    records.push_back(cg::packet::receive::PetTechRecord {1, 101, "Heal", "", 20, 3, 4, 8});
    const cg::packet::receive::PetTechData petTechData(5, std::move(records));
    const std::string payload = "0|100|Bite|desc|30|1|2|7|1|101|Heal||20|3|4|8|";

    int failures = ExpectPetTechDataPacket(petTechData.ToPacket(), 5, payload, "OBee multi ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(petTechData.ToPacket());
    failures += Expect(static_cast<bool>(built), "build OBee packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::PetTechData> parsed =
        cg::packet::receive::PetTechData::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip OBee packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().petIndex == 5, "roundtrip OBee petIndex");
        failures += Expect(parsed.Value().BuildData() == payload, "roundtrip OBee payload");
        failures += Expect(parsed.Value().records.size() == 2U, "roundtrip OBee record count");
    }
    return failures;
}

int TestRawTextPreserved()
{
    const std::string payload = "0|100|Name\\zPet|Desc\\nLine|30|1|2|7|";
    const cg::packet::PacketResult<cg::packet::receive::PetTechData> parsed =
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse OBee raw text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "parse OBee raw text record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            0,
            100,
            "Name\\zPet",
            "Desc\\nLine",
            30,
            1,
            2,
            7,
            "parse OBee raw text record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "build OBee raw text");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "OBee",
        "OBee empty data");
    failures += ExpectError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, "0|100|Bite|desc|30|1|2|7")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        "OBee missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, "0|100|Bite|desc|30|1|2|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OBee",
        "OBee payload field count");
    failures += ExpectError(
        cg::packet::receive::PetTechData::Parse("OBee _ 0|100|Bite|desc|30|1|2|7| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        "OBee bad petIndex i62");
    failures += ExpectError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, "0|100|Bite|desc|x|1|2|7|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        "OBee bad raw decimal");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestSingleRecord();
    failures += TestMultiRecordRoundtrip();
    failures += TestRawTextPreserved();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
