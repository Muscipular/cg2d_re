#include "packet/packet.hpp"

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::CharacterList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::CharacterList::Spec);
}

using CharacterList = cg::packet::receive::CharacterList;
using CharacterListRecord = cg::packet::receive::CharacterListRecord;

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

cg::packet::PacketField Field(
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::PacketField field;
    field.name = std::string(name);
    field.type = type;
    field.value = std::move(value);
    return field;
}

cg::packet::Packet PacketOf(
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(CharacterList::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(int result, std::string_view encodedPayload)
{
    const cg::packet::PacketResult<std::string> encodedResult = cg::packet::EncodeInt62(result);
    if (!encodedResult.HasValue()) {
        return "Daj ";
    }

    std::string plain("Daj ");
    plain.append(encodedResult.Value());
    plain.push_back(' ');
    plain.append(encodedPayload);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlain(int result, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return BuildRawPlain(result, "");
    }
    return BuildRawPlain(result, encodedPayload.Value());
}

std::string JoinDetailParts(const std::array<std::string, 19>& parts)
{
    std::string detail;
    for (std::size_t index = 0; index < parts.size(); ++index) {
        if (index != 0U) {
            detail.append("\\z");
        }
        detail.append(parts[index]);
    }
    return detail;
}

std::array<std::string, 19> DetailParts()
{
    return {{
        "1",
        "100",
        "20",
        "30",
        "40",
        "50",
        "60",
        "70",
        "1",
        "2",
        "3",
        "4",
        "5",
        "Alice",
        "900",
        "Warrior",
        "1",
        "200",
        "7",
    }};
}

CharacterListRecord MakeRecord(std::string nameRaw, int offset)
{
    CharacterListRecord record;
    record.nameRaw = std::move(nameRaw);
    record.dataPlaceNumber = 1 + offset;
    record.faceImageNumber = 100 + offset;
    record.level = 20 + offset;
    record.vital = 30 + offset;
    record.strength = 40 + offset;
    record.toughness = 50 + offset;
    record.quick = 60 + offset;
    record.magic = 70 + offset;
    record.earth = 1 + offset;
    record.water = 2 + offset;
    record.fire = 3 + offset;
    record.wind = 4 + offset;
    record.loginCount = 5 + offset;
    record.duplicateNameRaw = "Dup";
    record.registNumber = 900 + offset;
    record.jobNameRaw = "Job";
    record.renewal = 1 + offset;
    record.baseBaseImage = 200 + offset;
    record.titleId = 7 + offset;
    return record;
}

std::string PayloadFrom(const std::vector<CharacterListRecord>& records)
{
    const CharacterList list(records);
    const cg::packet::PacketResult<std::string> data = list.BuildData();
    return data.HasValue() ? data.Value() : std::string();
}

int ExpectCharacterListPacket(
    const cg::packet::Packet& packet,
    int result,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(CharacterList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "result", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(result), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == payload, message);
    return failures;
}

int ExpectRecord(const CharacterListRecord& actual, const CharacterListRecord& expected, const char* message)
{
    int failures = 0;
    failures += Expect(actual.nameRaw == expected.nameRaw, message);
    failures += Expect(actual.dataPlaceNumber == expected.dataPlaceNumber, message);
    failures += Expect(actual.faceImageNumber == expected.faceImageNumber, message);
    failures += Expect(actual.level == expected.level, message);
    failures += Expect(actual.vital == expected.vital, message);
    failures += Expect(actual.strength == expected.strength, message);
    failures += Expect(actual.toughness == expected.toughness, message);
    failures += Expect(actual.quick == expected.quick, message);
    failures += Expect(actual.magic == expected.magic, message);
    failures += Expect(actual.earth == expected.earth, message);
    failures += Expect(actual.water == expected.water, message);
    failures += Expect(actual.fire == expected.fire, message);
    failures += Expect(actual.wind == expected.wind, message);
    failures += Expect(actual.loginCount == expected.loginCount, message);
    failures += Expect(actual.duplicateNameRaw == expected.duplicateNameRaw, message);
    failures += Expect(actual.registNumber == expected.registNumber, message);
    failures += Expect(actual.jobNameRaw == expected.jobNameRaw, message);
    failures += Expect(actual.renewal == expected.renewal, message);
    failures += Expect(actual.baseBaseImage == expected.baseBaseImage, message);
    failures += Expect(actual.titleId == expected.titleId, message);
    return failures;
}

int TestParsePacketOuter()
{
    const CharacterListRecord record = MakeRecord("Alice", 0);
    const std::string payload = PayloadFrom({record});
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain(0, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Daj outer");
    if (parsed.HasValue()) {
        failures += ExpectCharacterListPacket(parsed.Value(), 0, payload, "parse Daj packet");
    }
    return failures;
}

int TestEmptyList()
{
    const cg::packet::PacketResult<CharacterList> parsed = CharacterList::Parse(BuildPlain(0, ""));

    int failures = Expect(static_cast<bool>(parsed), "parse Daj empty list");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().result == CharacterList::SUCCESSFUL, "Daj empty result");
    failures += Expect(parsed.Value().data.empty(), "Daj empty data");
    failures += Expect(parsed.Value().records.empty(), "Daj empty records");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "Daj empty BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value().empty(), "Daj empty BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Daj empty ToPacket");
    if (packet.HasValue()) {
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "Daj empty BuildPacket");
        if (built.HasValue()) {
            failures += Expect(built.Value() == "Daj 0  ", "Daj empty encoded");
        }
    }
    return failures;
}

int TestOneAndTwoRoundtrip()
{
    CharacterListRecord first = MakeRecord("Alice", 0);
    first.duplicateNameRaw = "Other";
    CharacterListRecord second = MakeRecord("Bob", 10);
    second.level = -1;
    second.renewal = -3;
    second.titleId = -7;
    second.jobNameRaw = "Mage";

    const std::vector<CharacterListRecord> records = {first, second};
    const CharacterList list(records);
    const std::string payload = PayloadFrom(records);

    int failures = Expect(!payload.empty(), "Daj two payload exists");
    const cg::packet::PacketResult<cg::packet::Packet> packet = list.ToPacket();
    failures += Expect(static_cast<bool>(packet), "Daj two ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectCharacterListPacket(packet.Value(), 0, payload, "Daj two packet");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "Daj two BuildPacket");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<CharacterList> parsed = CharacterList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "Daj two roundtrip parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 2U, "Daj two record count");
        if (parsed.Value().records.size() == 2U) {
            failures += ExpectRecord(parsed.Value().records[0], first, "Daj first record");
            failures += ExpectRecord(parsed.Value().records[1], second, "Daj second record");
            failures += Expect(
                parsed.Value().records[0].duplicateNameRaw != parsed.Value().records[0].nameRaw,
                "Daj duplicate name not checked");
        }
    }
    return failures;
}

int TestFailureResultsPreserveData()
{
    int failures = 0;
    for (const int result : {CharacterList::LOCKED, CharacterList::NOTYETAUTH, CharacterList::ETC}) {
        const std::string message = "bad|payload\\z2147483648";
        const cg::packet::PacketResult<CharacterList> parsed = CharacterList::Parse(BuildPlain(result, message));
        failures += Expect(static_cast<bool>(parsed), "Daj failure result parse");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().result == result, "Daj failure result value");
            failures += Expect(parsed.Value().data == message, "Daj failure data preserved");
            failures += Expect(parsed.Value().records.empty(), "Daj failure records empty");

            const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
            failures += Expect(static_cast<bool>(packet), "Daj failure ToPacket");
            if (packet.HasValue()) {
                failures += ExpectCharacterListPacket(packet.Value(), result, message, "Daj failure packet");
            }
        }
    }
    return failures;
}

int TestPacketLogSample()
{
    const std::string payload =
        "Muscipular|0\\z200648\\z120\\z137\\z217\\z0\\z80\\z0\\z0\\z100\\z0\\z0\\z608\\zMuscipular\\z92\\z幽影箭尊\\z2\\z106283\\z-1|"
        "测试|1\\z242200\\z4\\z10\\z15\\z0\\z5\\z0\\z0\\z0\\z0\\z100\\z11\\z测试\\z93\\z游民\\z2\\z106277\\z-1|";
    const cg::packet::PacketResult<CharacterList> parsed = CharacterList::Parse(BuildPlain(0, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Daj packet log sample");
    if (!parsed.HasValue()) {
        return failures;
    }

    CharacterListRecord first;
    first.nameRaw = "Muscipular";
    first.dataPlaceNumber = 0;
    first.faceImageNumber = 200648;
    first.level = 120;
    first.vital = 137;
    first.strength = 217;
    first.toughness = 0;
    first.quick = 80;
    first.magic = 0;
    first.earth = 0;
    first.water = 100;
    first.fire = 0;
    first.wind = 0;
    first.loginCount = 608;
    first.duplicateNameRaw = "Muscipular";
    first.registNumber = 92;
    first.jobNameRaw = "幽影箭尊";
    first.renewal = 2;
    first.baseBaseImage = 106283;
    first.titleId = -1;

    CharacterListRecord second;
    second.nameRaw = "测试";
    second.dataPlaceNumber = 1;
    second.faceImageNumber = 242200;
    second.level = 4;
    second.vital = 10;
    second.strength = 15;
    second.toughness = 0;
    second.quick = 5;
    second.magic = 0;
    second.earth = 0;
    second.water = 0;
    second.fire = 0;
    second.wind = 100;
    second.loginCount = 11;
    second.duplicateNameRaw = "测试";
    second.registNumber = 93;
    second.jobNameRaw = "游民";
    second.renewal = 2;
    second.baseBaseImage = 106277;
    second.titleId = -1;

    failures += Expect(parsed.Value().result == CharacterList::SUCCESSFUL, "Daj packet log result");
    failures += Expect(parsed.Value().records.size() == 2U, "Daj packet log record count");
    if (parsed.Value().records.size() == 2U) {
        failures += ExpectRecord(parsed.Value().records[0], first, "Daj packet log first record");
        failures += ExpectRecord(parsed.Value().records[1], second, "Daj packet log second record");
    }

    const cg::packet::PacketResult<std::string> built = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(built), "Daj packet log BuildData");
    if (built.HasValue()) {
        failures += Expect(built.Value() == payload, "Daj packet log BuildData value");
    }
    return failures;
}

int TestOuterErrors()
{
    int failures = 0;
    failures += ExpectError(
        CharacterList::Parse("whV 0 ok "),
        cg::packet::PacketErrorCode::InvalidCode,
        "whV",
        "Daj wrong code");
    failures += ExpectError(
        CharacterList::Parse("Daj 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Daj",
        "Daj missing data");
    failures += ExpectError(
        CharacterList::Parse("Daj 0 ok extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Daj",
        "Daj extra field");

    cg::packet::Packet wrongDirection = PacketOf(
        cg::packet::PacketDirection::ClientToServer,
        {Field("result", cg::packet::PacketFieldType::Int62, "0"),
         Field("data", cg::packet::PacketFieldType::EscapedString, "")});
    failures += ExpectError(
        CharacterList::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Daj",
        "Daj wrong direction");
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    const std::string detail = JoinDetailParts(DetailParts());
    failures += ExpectError(
        CharacterList::Parse(BuildPlain(0, std::string("Alice|") + detail)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Daj",
        "Daj missing trailing delimiter");
    failures += ExpectError(
        CharacterList::Parse(BuildPlain(0, std::string("Alice|") + detail + "|Bob|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Daj",
        "Daj odd top payload");
    failures += ExpectError(
        CharacterList::Parse(BuildPlain(0, "Alice|1\\z2|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Daj",
        "Daj detail count");

    std::array<std::string, 19> emptyNumber = DetailParts();
    emptyNumber[2] = "";
    failures += ExpectError(
        CharacterList::Parse(BuildPlain(0, std::string("Alice|") + JoinDetailParts(emptyNumber) + "|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Daj",
        "Daj empty decimal");

    std::array<std::string, 19> badNumber = DetailParts();
    badNumber[2] = "x";
    failures += ExpectError(
        CharacterList::Parse(BuildPlain(0, std::string("Alice|") + JoinDetailParts(badNumber) + "|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Daj",
        "Daj bad decimal");

    std::array<std::string, 19> overflowNumber = DetailParts();
    overflowNumber[2] = "2147483648";
    failures += ExpectError(
        CharacterList::Parse(BuildPlain(0, std::string("Alice|") + JoinDetailParts(overflowNumber) + "|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Daj",
        "Daj overflow decimal");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyList();
    failures += TestOneAndTwoRoundtrip();
    failures += TestFailureResultsPreserveData();
    failures += TestPacketLogSample();
    failures += TestOuterErrors();
    failures += TestPayloadErrors();

    if (failures != 0) {
        std::cerr << failures << " packet character list test(s) failed\n";
        return 1;
    }

    return 0;
}
