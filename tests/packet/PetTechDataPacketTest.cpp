#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

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

template <typename T>
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, packetCode, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
    }
    return failures;
}

std::string EncodeLegacy(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeLegacyPayloadText(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string WithGbkTrailPipe(std::string_view prefix)
{
    std::string value(prefix);
    value.push_back(static_cast<char>(0x96));
    value.push_back(static_cast<char>(0x7C));
    return value;
}

void AppendRecordData(
    std::string& data,
    const cg::packet::receive::PetTechRecord& record)
{
    data.append(std::to_string(record.skillSlot));
    data.push_back('|');
    data.append(std::to_string(record.techId));
    data.push_back('|');
    data.append(EncodeLegacy(record.name));
    data.push_back('|');
    data.append(EncodeLegacy(record.description));
    data.push_back('|');
    data.append(std::to_string(record.forcePoint));
    data.push_back('|');
    data.append(std::to_string(record.techField));
    data.push_back('|');
    data.append(std::to_string(record.target));
    data.push_back('|');
    data.append(std::to_string(record.seqNo));
    data.push_back('|');
}

std::string BuildRecordsData(
    const std::vector<cg::packet::receive::PetTechRecord>& records)
{
    std::string data;
    for (const cg::packet::receive::PetTechRecord& record : records) {
        AppendRecordData(data, record);
    }
    return data;
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
    cg::packet::PacketFieldType dataType,
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
    failures += Expect(packet.fields[1].type == dataType, message);
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

    int failures = Expect(
        cg::packet::receive::PetTechData::Fields[1U].type ==
            cg::packet::PacketFieldType::SubPayload,
        "OBee data spec SubPayload");
    failures += Expect(static_cast<bool>(parsed), "parse OBee outer");
    if (parsed.HasValue()) {
        failures += ExpectPetTechDataPacket(
            parsed.Value(),
            2,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse OBee outer packet");
    }
    return failures;
}

int TestSingleRecord()
{
    const std::string name = WithGbkTrailPipe("Name|Pet,Line\\Tail\n");
    const std::string description = WithGbkTrailPipe("Desc,Pet|Line\\Tail\n");
    const std::vector<cg::packet::receive::PetTechRecord> expectedRecords = {
        {0, 100, name, description, -30, 1, 2, 7},
    };
    const std::string payload = BuildRecordsData(expectedRecords);
    const cg::packet::PacketResult<cg::packet::receive::PetTechData> parsed =
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse OBee single record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 2, "parse OBee petIndex");
    failures += Expect(parsed.Value().records.size() == 1U, "parse OBee record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            0,
            100,
            name,
            description,
            -30,
            1,
            2,
            7,
            "parse OBee semantic GBK/legacy record");
    }
    const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(builtData), "build OBee single payload");
    if (builtData.HasValue()) {
        failures += Expect(builtData.Value() == payload, "build OBee single payload value");
        failures += Expect(builtData.Value().back() == '|', "build OBee fixed trailing delimiter");
    }
    return failures;
}

int TestMultiRecordRoundtrip()
{
    std::vector<cg::packet::receive::PetTechRecord> records;
    records.reserve(10U);
    for (int index = 0; index < 10; ++index) {
        records.push_back(cg::packet::receive::PetTechRecord {
            index,
            100 + index,
            index == 9 ? WithGbkTrailPipe("Name9") : "Name" + std::to_string(index),
            index == 0 ? "Desc|0,Line\\Tail\n" : "Desc" + std::to_string(index),
            -30 + index,
            1 + index,
            2 + index,
            7 + index,
        });
    }
    const std::string payload = BuildRecordsData(records);
    const cg::packet::receive::PetTechData petTechData(-5, std::move(records));

    const cg::packet::PacketResult<cg::packet::Packet> packet = petTechData.ToPacket();
    int failures = Expect(static_cast<bool>(packet), "OBee ten-record ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectPetTechDataPacket(
        packet.Value(),
        -5,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "OBee ten-record ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build OBee packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::PetTechData> parsed =
        cg::packet::receive::PetTechData::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip OBee packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().petIndex == -5, "roundtrip OBee signed petIndex");
        failures += Expect(parsed.Value().records.size() == 10U, "roundtrip OBee ten-record upper bound");
        const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(builtData), "roundtrip OBee BuildData");
        if (builtData.HasValue()) {
            failures += Expect(builtData.Value() == payload, "roundtrip OBee payload");
        }
    }
    return failures;
}

int TestKnownLegacyText()
{
    const std::string payload =
        "0|100|Name\\zPet\\cLine\\yTail\\n|Desc\\nLine\\zTail\\cEnd\\y|30|1|2|7|";
    const cg::packet::PacketResult<cg::packet::receive::PetTechData> parsed =
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse OBee known legacy text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "parse OBee known legacy record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            0,
            100,
            "Name|Pet,Line\\Tail\n",
            "Desc\nLine|Tail,End\\",
            30,
            1,
            2,
            7,
            "parse OBee known legacy semantic record");
    }
    const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(builtData), "build OBee known legacy text");
    if (builtData.HasValue()) {
        failures += Expect(builtData.Value() == payload, "build OBee known legacy wire text");
    }
    return failures;
}

int TestTwoRecordTextOffsets()
{
    const std::string secondName = WithGbkTrailPipe("Second|Name");
    const std::string secondDescription = "Second,Description\\Line\n";
    const std::vector<cg::packet::receive::PetTechRecord> records = {
        {0, 100, "First", "Description", 30, 1, 2, 7},
        {1, 101, secondName, secondDescription, 20, 3, 4, 8},
    };
    const std::string payload = BuildRecordsData(records);
    const cg::packet::PacketResult<cg::packet::receive::PetTechData> parsed =
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(0, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse OBee two records");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "OBee two-record count");
    if (parsed.Value().records.size() == 2U) {
        failures += ExpectRecord(
            parsed.Value().records[1],
            1,
            101,
            secondName,
            secondDescription,
            20,
            3,
            4,
            8,
            "OBee second semantic record");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "OBee",
        1U,
        "OBee empty data");
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, "0|100|Bite|desc|30|1|2|7")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        1U,
        "OBee missing trailing delimiter");
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, "0|100|Bite|desc|30|1|2|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OBee",
        1U,
        "OBee payload field count");
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse("OBee _ 0|100|Bite|desc|30|1|2|7| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        0U,
        "OBee bad petIndex i62");
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, "0|100|Bite|desc|x|1|2|7|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        4U,
        "OBee bad raw decimal");

    failures += ExpectError(
        cg::packet::receive::PetTechData::Parse("JgO 0 data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "JgO",
        "OBee wrong code");
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse("OBee 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OBee",
        1U,
        "OBee outer missing data index");
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse("OBee 0 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OBee",
        2U,
        "OBee outer extra field index");

    const std::vector<std::string> badDecimalPayloads = {
        "x|100|Name|Desc|30|1|2|7|",
        "0|x|Name|Desc|30|1|2|7|",
        "0|100|Name|Desc|x|1|2|7|",
        "0|100|Name|Desc|30|x|2|7|",
        "0|100|Name|Desc|30|1|x|7|",
        "0|100|Name|Desc|30|1|2|x|",
    };
    const std::vector<std::size_t> badDecimalIndexes = {0U, 1U, 4U, 5U, 6U, 7U};
    for (std::size_t index = 0U; index < badDecimalPayloads.size(); ++index) {
        failures += ExpectFieldError(
            cg::packet::receive::PetTechData::Parse(
                BuildPlainObee(2, badDecimalPayloads[index])),
            cg::packet::PacketErrorCode::DecodeFailed,
            "OBee",
            badDecimalIndexes[index],
            "OBee raw decimal local field index");
    }

    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse(
            BuildPlainObee(2, "0|100|Name\\|Desc|30|1|2|7|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        2U,
        "OBee name legacy truncated index");
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(
            2,
            "0|100|Name|Desc|30|1|2|7|1|101|Name|Desc\\|20|3|4|8|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        11U,
        "OBee second description legacy truncated index");
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(
            2,
            "0|100|Name|Desc|30|1|2|7|1|101|Name|Desc|20|3|4|x|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OBee",
        15U,
        "OBee second seqNo local field index");

    std::vector<cg::packet::receive::PetTechRecord> elevenRecords(
        11U,
        cg::packet::receive::PetTechRecord {0, 100, "Name", "Desc", 30, 1, 2, 7});
    const std::string elevenData = BuildRecordsData(elevenRecords);
    failures += ExpectFieldError(
        cg::packet::receive::PetTechData::Parse(BuildPlainObee(2, elevenData)),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OBee",
        1U,
        "OBee parse eleven records rejected");
    const cg::packet::receive::PetTechData elevenValue(2, std::move(elevenRecords));
    failures += ExpectFieldError(
        elevenValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "OBee",
        1U,
        "OBee build eleven records rejected");

    const cg::packet::receive::PetTechData emptyValue(
        2,
        std::vector<cg::packet::receive::PetTechRecord> {});
    failures += ExpectFieldError(
        emptyValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "OBee",
        1U,
        "OBee build empty records rejected");
    failures += ExpectFieldError(
        emptyValue.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "OBee",
        1U,
        "OBee ToPacket empty records propagation");

    cg::packet::receive::PetTechRecord badNameRecord {0, 100, "", "Desc", 30, 1, 2, 7};
    badNameRecord.name.assign(1U, static_cast<char>(0x96));
    const cg::packet::receive::PetTechData badNameValue(
        2,
        std::vector<cg::packet::receive::PetTechRecord> {std::move(badNameRecord)});
    failures += ExpectFieldError(
        badNameValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "OBee",
        2U,
        "OBee build name truncated GBK");
    failures += ExpectFieldError(
        badNameValue.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "OBee",
        2U,
        "OBee ToPacket name encoding propagation");

    cg::packet::receive::PetTechRecord badDescriptionRecord {1, 101, "Name", "", 20, 3, 4, 8};
    badDescriptionRecord.description.assign(1U, static_cast<char>(0x96));
    const cg::packet::receive::PetTechData badDescriptionValue(
        2,
        std::vector<cg::packet::receive::PetTechRecord> {
            {0, 100, "Name", "Desc", 30, 1, 2, 7},
            std::move(badDescriptionRecord),
        });
    failures += ExpectFieldError(
        badDescriptionValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "OBee",
        11U,
        "OBee build second description truncated GBK");
    failures += ExpectFieldError(
        badDescriptionValue.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "OBee",
        11U,
        "OBee ToPacket description encoding propagation");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestSingleRecord();
    failures += TestMultiRecordRoundtrip();
    failures += TestKnownLegacyText();
    failures += TestTwoRecordTextOffsets();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
