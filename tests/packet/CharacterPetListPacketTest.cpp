#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::CharacterPetList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::CharacterPetList::Spec);
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

std::string BuildPlain(std::string_view code, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        std::string plain(code);
        plain.append("  ");
        return plain;
    }

    std::string plain(code);
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildPlainNd(std::string_view payload)
{
    return BuildPlain(cg::packet::receive::CharacterPetList::Code, payload);
}

int ExpectCharacterPetListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::CharacterPetList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::SubPayload, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectCharacterPetListPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view value,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectCharacterPetListPacket(packet.Value(), value, message);
    }
    return failures;
}

int ExpectBuildData(
    const cg::packet::receive::CharacterPetList& characterPetList,
    std::string_view value,
    const char* message)
{
    const cg::packet::PacketResult<std::string> data = characterPetList.BuildData();
    int failures = Expect(static_cast<bool>(data), message);
    if (data.HasValue()) {
        failures += Expect(data.Value() == value, message);
    }
    return failures;
}

int ExpectRecord(
    const cg::packet::receive::CharacterPetRecord& record,
    std::string_view name,
    int level,
    int injury,
    int hp,
    int maxHp,
    int forcePoint,
    int maxForcePoint,
    const char* message)
{
    int failures = 0;
    failures += Expect(record.name == name, message);
    failures += Expect(record.level == level, message);
    failures += Expect(record.injury == injury, message);
    failures += Expect(record.hp == hp, message);
    failures += Expect(record.maxHp == maxHp, message);
    failures += Expect(record.forcePoint == forcePoint, message);
    failures += Expect(record.maxForcePoint == maxForcePoint, message);
    return failures;
}

std::string BuildRecordPayload(int index)
{
    std::string payload = "Name";
    payload.append(std::to_string(index));
    payload.push_back('|');
    payload.append(std::to_string(10 + index));
    payload.append("|0|");
    payload.append(std::to_string(100 + index));
    payload.push_back('|');
    payload.append(std::to_string(150 + index));
    payload.push_back('|');
    payload.append(std::to_string(30 + index));
    payload.push_back('|');
    payload.append(std::to_string(80 + index));
    payload.push_back('|');
    return payload;
}

std::string BuildRecordsPayload(int count)
{
    std::string payload;
    for (int index = 0; index < count; ++index) {
        payload.append(BuildRecordPayload(index));
    }
    return payload;
}

std::vector<cg::packet::receive::CharacterPetRecord> BuildRecords(int count)
{
    std::vector<cg::packet::receive::CharacterPetRecord> records;
    for (int index = 0; index < count; ++index) {
        records.push_back(cg::packet::receive::CharacterPetRecord {
            "Name" + std::to_string(index),
            10 + index,
            0,
            100 + index,
            150 + index,
            30 + index,
            80 + index,
        });
    }
    return records;
}

int TestParseRecordsInParse()
{
    const std::string payload = "Alice|35|0|120|150|40|80|";
    const cg::packet::PacketResult<cg::packet::Packet> outer = ParsePacket(BuildPlainNd(payload));
    const cg::packet::PacketResult<cg::packet::receive::CharacterPetList> parsed =
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd(payload));

    int failures = Expect(
        cg::packet::receive::CharacterPetList::Fields[0U].type ==
            cg::packet::PacketFieldType::SubPayload,
        "ND data spec SubPayload");
    failures += Expect(static_cast<bool>(outer), "parse ND outer");
    if (outer.HasValue() && outer.Value().fields.size() == 1U) {
        failures += Expect(
            outer.Value().fields[0U].type == cg::packet::PacketFieldType::EscapedString,
            "ND generic Parse runtime EscapedString");
    }
    failures += Expect(static_cast<bool>(parsed), "parse ND records in typed Parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 1U, "parse ND records count");
    }
    if (parsed.HasValue() && parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "Alice", 35, 0, 120, 150, 40, 80, "parse ND records");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterPetList> parsed =
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd(""));

    int failures = Expect(static_cast<bool>(parsed), "parse ND empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "ND empty data record count");
        failures += ExpectBuildData(parsed.Value(), "", "ND empty data BuildData");
        failures += ExpectCharacterPetListPacket(parsed.Value().ToPacket(), "", "ND empty data ToPacket");
    }
    return failures;
}

int TestSingleRecord()
{
    const std::string payload = "Alice|35|0|120|150|40|80|";
    const cg::packet::PacketResult<cg::packet::receive::CharacterPetList> parsed =
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ND single record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "ND single record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "Alice", 35, 0, 120, 150, 40, 80, "ND single record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "ND single record BuildData");
    return failures;
}

int TestSixRecordsRoundtrip()
{
    const std::string payload = BuildRecordsPayload(6);
    const cg::packet::receive::CharacterPetList characterPetList(BuildRecords(6));

    const cg::packet::PacketResult<cg::packet::Packet> packet = characterPetList.ToPacket();
    int failures = ExpectCharacterPetListPacket(packet, payload, "ND six records ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build ND six records packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::CharacterPetList> parsed =
        cg::packet::receive::CharacterPetList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip ND six records packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 6U, "roundtrip ND six records count");
        failures += ExpectBuildData(parsed.Value(), payload, "roundtrip ND six records payload");
    }
    return failures;
}

int TestSemanticNameRoundtrip()
{
    std::string name = "A|B,C\\D\nE";
    name.push_back(static_cast<char>(0x96));
    name.push_back(static_cast<char>(0x7C));
    std::string payload = "A\\zB\\cC\\yD\\nE";
    payload.push_back(static_cast<char>(0x96));
    payload.push_back(static_cast<char>(0x7C));
    payload.append("|35|0|120|150|40|80|");
    const cg::packet::PacketResult<cg::packet::receive::CharacterPetList> parsed =
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ND semantic GBK/legacy name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "ND semantic name record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            name,
            35,
            0,
            120,
            150,
            40,
            80,
            "ND semantic name record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "ND semantic name BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectCharacterPetListPacket(packet, payload, "ND semantic name ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build ND semantic name packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainNd(payload), "ND semantic name roundtrip");
    }
    return failures;
}

int TestSignedBoundaries()
{
    const std::string payload = "Bounds|-2147483648|2147483647|-1|0|-20|20|";
    const cg::packet::receive::CharacterPetList value({
        cg::packet::receive::CharacterPetRecord {
            "Bounds",
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            -1,
            0,
            -20,
            20,
        },
    });

    int failures = ExpectBuildData(value, payload, "ND signed boundaries BuildData");
    const cg::packet::PacketResult<cg::packet::Packet> packet = value.ToPacket();
    failures += ExpectCharacterPetListPacket(packet, payload, "ND signed boundaries ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build ND signed boundaries packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::CharacterPetList> parsed =
        cg::packet::receive::CharacterPetList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse ND signed boundaries");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 1U, "ND signed boundary record count");
    }
    if (parsed.HasValue() && parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0U],
            "Bounds",
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            -1,
            0,
            -20,
            20,
            "ND signed boundary record");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse("ND"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ND",
        0U,
        "ND outer missing data index");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse("ND data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ND",
        1U,
        "ND outer extra field index");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd("Alice|35|0|120|150|40|80")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ND",
        0U,
        "ND missing trailing delimiter");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd("Alice|35|0|120|150|40|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ND",
        0U,
        "ND payload field count");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd(BuildRecordsPayload(7))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ND",
        0U,
        "ND record count over 6");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd("Alice||0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ND",
        1U,
        "ND empty decimal");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd("Alice|35x|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ND",
        1U,
        "ND non-decimal");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd("Alice|2147483648|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ND",
        1U,
        "ND positive decimal overflow");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd("Alice|-2147483649|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ND",
        1U,
        "ND negative decimal overflow");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd("Name\\|35|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ND",
        0U,
        "ND first name legacy truncation index");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd(
            "Alice|35|0|120|150|40|80|Name\\|1|0|2|3|4|5|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ND",
        7U,
        "ND second name legacy truncation index");
    failures += ExpectFieldError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlainNd(
            "Alice|35|0|120|150|40|80|Pet|1|0|2|3|4|bad|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ND",
        13U,
        "ND second maxForcePoint field index");
    failures += ExpectError(
        cg::packet::receive::CharacterPetList::Parse(BuildPlain("ysVc", "Alice|35|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::InvalidCode,
        "ysVc",
        "ND rejects wrong code");
    return failures;
}

int TestBuildRejectsInvalidRecords()
{
    int failures = 0;
    const cg::packet::receive::CharacterPetList tooMany(BuildRecords(7));
    failures += ExpectFieldError(
        tooMany.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ND",
        0U,
        "ND BuildData rejects seven records");
    failures += ExpectFieldError(
        tooMany.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ND",
        0U,
        "ND ToPacket rejects seven records");

    std::string truncatedName(1U, static_cast<char>(0x96));
    const cg::packet::receive::CharacterPetList invalidName({
        cg::packet::receive::CharacterPetRecord {"Alice", 1, 0, 120, 150, 40, 80},
        cg::packet::receive::CharacterPetRecord {std::move(truncatedName), 2, 0, 120, 150, 40, 80},
    });
    failures += ExpectFieldError(
        invalidName.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ND",
        7U,
        "ND second name BuildData encoding failure index");
    failures += ExpectFieldError(
        invalidName.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ND",
        7U,
        "ND second name ToPacket encoding failure index");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParseRecordsInParse();
    failures += TestEmptyData();
    failures += TestSingleRecord();
    failures += TestSixRecordsRoundtrip();
    failures += TestSemanticNameRoundtrip();
    failures += TestSignedBoundaries();
    failures += TestErrors();
    failures += TestBuildRejectsInvalidRecords();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
