#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ItemCharacterPetList::Spec);
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

std::string BuildPlainNfv(std::string_view payload)
{
    return BuildPlain(cg::packet::receive::ItemCharacterPetList::Code, payload);
}

int ExpectItemCharacterPetListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ItemCharacterPetList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectItemCharacterPetListPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view value,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectItemCharacterPetListPacket(packet.Value(), value, message);
    }
    return failures;
}

int ExpectBuildData(
    const cg::packet::receive::ItemCharacterPetList& itemCharacterPetList,
    std::string_view value,
    const char* message)
{
    const cg::packet::PacketResult<std::string> data = itemCharacterPetList.BuildData();
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
    const cg::packet::PacketResult<cg::packet::receive::ItemCharacterPetList> parsed =
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse nFV records in typed Parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 1U, "parse nFV records count");
    }
    if (parsed.HasValue() && parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "Alice", 35, 0, 120, 150, 40, 80, "parse nFV records");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<cg::packet::receive::ItemCharacterPetList> parsed =
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv(""));

    int failures = Expect(static_cast<bool>(parsed), "parse nFV empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "nFV empty data record count");
        failures += ExpectBuildData(parsed.Value(), "", "nFV empty data BuildData");
    }
    return failures;
}

int TestSingleRecord()
{
    const std::string payload = "Alice|35|0|120|150|40|80|";
    const cg::packet::PacketResult<cg::packet::receive::ItemCharacterPetList> parsed =
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse nFV single record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "nFV single record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "Alice", 35, 0, 120, 150, 40, 80, "nFV single record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "nFV single record BuildData");
    return failures;
}

int TestEmptyName()
{
    const std::string payload = "|35|0|120|150|40|80|";
    const cg::packet::PacketResult<cg::packet::receive::ItemCharacterPetList> parsed =
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse nFV empty name");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 1U, "nFV empty name record count");
    }
    if (parsed.HasValue() && parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "", 35, 0, 120, 150, 40, 80, "nFV empty name");
        failures += ExpectBuildData(parsed.Value(), payload, "nFV empty name BuildData");
    }
    return failures;
}

int TestSixRecordsRoundtrip()
{
    const std::string payload = BuildRecordsPayload(6);
    const cg::packet::receive::ItemCharacterPetList itemCharacterPetList(BuildRecords(6));

    const cg::packet::PacketResult<cg::packet::Packet> packet = itemCharacterPetList.ToPacket();
    int failures = ExpectItemCharacterPetListPacket(packet, payload, "nFV six records ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build nFV six records packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::ItemCharacterPetList> parsed =
        cg::packet::receive::ItemCharacterPetList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip nFV six records packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 6U, "roundtrip nFV six records count");
        failures += ExpectBuildData(parsed.Value(), payload, "roundtrip nFV six records payload");
    }
    return failures;
}

int TestRawNamePreserved()
{
    const std::string payload = "A\\zB\\cC\\yD\\nE|35|0|120|150|40|80|";
    const cg::packet::PacketResult<cg::packet::receive::ItemCharacterPetList> parsed =
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse nFV raw name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "nFV raw name record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            "A\\zB\\cC\\yD\\nE",
            35,
            0,
            120,
            150,
            40,
            80,
            "nFV raw name record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "nFV raw name BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectItemCharacterPetListPacket(packet, payload, "nFV raw name ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build nFV raw name packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainNfv(payload), "nFV raw name roundtrip");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse("nFV Alice extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nFV",
        "nFV outer field count");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv("Alice|35|0|120|150|40|80")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nFV",
        "nFV missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv("Alice|35|0|120|150|40|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nFV",
        "nFV six payload fields");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv("Alice|35|0|120|150|40|80|extra|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nFV",
        "nFV eight payload fields");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv(BuildRecordsPayload(7))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nFV",
        "nFV record count over 6");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv("Alice||0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nFV",
        "nFV empty decimal");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv("Alice|x|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nFV",
        "nFV non-decimal");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv("Alice|-1|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nFV",
        "nFV negative decimal");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlainNfv("Alice|2147483648|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nFV",
        "nFV decimal overflow");
    failures += ExpectError(
        cg::packet::receive::ItemCharacterPetList::Parse(BuildPlain("ND", "Alice|35|0|120|150|40|80|")),
        cg::packet::PacketErrorCode::InvalidCode,
        "ND",
        "nFV rejects wrong code");
    return failures;
}

int TestBuildRejectsInvalidRecords()
{
    int failures = 0;
    const cg::packet::receive::ItemCharacterPetList tooMany(BuildRecords(7));
    failures += ExpectError(
        tooMany.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "nFV",
        "nFV BuildData rejects seven records");
    failures += ExpectError(
        tooMany.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "nFV",
        "nFV ToPacket rejects seven records");

    const cg::packet::receive::ItemCharacterPetList negative({
        cg::packet::receive::CharacterPetRecord {"Alice", -1, 0, 120, 150, 40, 80},
    });
    failures += ExpectError(
        negative.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "nFV",
        "nFV BuildData rejects negative decimal");
    failures += ExpectError(
        negative.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "nFV",
        "nFV ToPacket rejects negative decimal");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParseRecordsInParse();
    failures += TestEmptyData();
    failures += TestSingleRecord();
    failures += TestEmptyName();
    failures += TestSixRecordsRoundtrip();
    failures += TestRawNamePreserved();
    failures += TestErrors();
    failures += TestBuildRejectsInvalidRecords();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
