#include "packet/packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using TradePetInfo = cg::packet::receive::TradePetInfo;
using TradePetInfoData = cg::packet::receive::TradePetInfoData;

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
        cg::packet::PacketCode(TradePetInfo::Code),
        direction,
        cg::packet::PacketSourceTag::ServerC);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::vector<std::string> BuildParts()
{
    return {
        "Free\\zName",
        "12",
        "3",
        "100",
        "30",
        "1",
        "2",
        "3",
        "4",
        "5",
        "80",
        "8",
        "Pet\\nName",
        "1001",
        "50",
        "40",
        "30",
        "20",
        "10",
        "1",
        "2",
        "3",
        "4",
    };
}

std::string JoinParts(const std::vector<std::string>& parts, bool trailingDelimiter)
{
    std::string payload;
    for (std::size_t index = 0; index < parts.size(); ++index) {
        if (index != 0U) {
            payload.push_back('|');
        }
        payload.append(parts[index]);
    }
    if (trailingDelimiter) {
        payload.push_back('|');
    }
    return payload;
}

std::string BuildPayload()
{
    return JoinParts(BuildParts(), true);
}

std::string BuildRawPlain(int petPlace, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPetPlace = cg::packet::EncodeInt62(petPlace);
    if (!encodedPetPlace.HasValue()) {
        return "RwK ";
    }

    std::string plain("RwK ");
    plain.append(encodedPetPlace.Value());
    plain.push_back(' ');
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlain(int petPlace, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return BuildRawPlain(petPlace, "");
    }
    return BuildRawPlain(petPlace, encoded.Value());
}

TradePetInfoData BuildData()
{
    return TradePetInfoData {
        "Free\\zName",
        12,
        3,
        100,
        30,
        1,
        2,
        3,
        4,
        5,
        80,
        8,
        "Pet\\nName",
        1001,
        50,
        40,
        30,
        20,
        10,
        1,
        2,
        3,
        4,
    };
}

int ExpectTradePetInfoPacket(
    const cg::packet::Packet& packet,
    int petPlace,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(TradePetInfo::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "petPlace", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields[0].value == std::to_string(petPlace), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields[1].value == payload, message);
    return failures;
}

int ExpectData(const TradePetInfoData& data, const char* message)
{
    int failures = 0;
    failures += Expect(data.freeNameRaw == "Free\\zName", message);
    failures += Expect(data.level == 12, message);
    failures += Expect(data.tribe == 3, message);
    failures += Expect(data.maxHp == 100, message);
    failures += Expect(data.maxForcePoint == 30, message);
    failures += Expect(data.vital == 1, message);
    failures += Expect(data.strength == 2, message);
    failures += Expect(data.tough == 3, message);
    failures += Expect(data.quick == 4, message);
    failures += Expect(data.magic == 5, message);
    failures += Expect(data.loyalty == 80, message);
    failures += Expect(data.skillSlot == 8, message);
    failures += Expect(data.nameRaw == "Pet\\nName", message);
    failures += Expect(data.imageId == 1001, message);
    failures += Expect(data.attack == 50, message);
    failures += Expect(data.defense == 40, message);
    failures += Expect(data.agility == 30, message);
    failures += Expect(data.magicPower == 20, message);
    failures += Expect(data.recovery == 10, message);
    failures += Expect(data.earth == 1, message);
    failures += Expect(data.water == 2, message);
    failures += Expect(data.fire == 3, message);
    failures += Expect(data.wind == 4, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = BuildPayload();
    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(BuildPlain(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse RwK outer");
    if (parsed.HasValue()) {
        failures += ExpectTradePetInfoPacket(parsed.Value(), 2, payload, "parse RwK packet");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    const std::string payload = BuildPayload();
    const TradePetInfo packet(2, BuildData());

    const cg::packet::PacketResult<std::string> data = packet.BuildData();
    int failures = Expect(static_cast<bool>(data), "RwK BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "RwK BuildData tail");
    }

    const cg::packet::PacketResult<cg::packet::Packet> typedPacket = packet.ToPacket();
    failures += Expect(static_cast<bool>(typedPacket), "RwK ToPacket");
    if (!typedPacket.HasValue()) {
        return failures;
    }

    failures += ExpectTradePetInfoPacket(typedPacket.Value(), 2, payload, "RwK ToPacket fields");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(typedPacket.Value());
    failures += Expect(static_cast<bool>(built), "build RwK packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<TradePetInfo> parsed = TradePetInfo::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip RwK");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().petPlace == 2, "roundtrip RwK petPlace");
        failures += ExpectData(parsed.Value().data, "roundtrip RwK data");
    }
    return failures;
}

int TestRawEscapesPreserved()
{
    const std::string raw = "A\\zB\\cC\\yD\\nE";
    std::vector<std::string> parts = BuildParts();
    parts[0] = raw;
    parts[12] = raw;
    const std::string payload = JoinParts(parts, true);

    const cg::packet::PacketResult<TradePetInfo> parsed = TradePetInfo::Parse(BuildRawPlain(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse RwK raw escaped strings");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().data.freeNameRaw == raw, "RwK raw freeName");
    failures += Expect(parsed.Value().data.nameRaw == raw, "RwK raw name");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "RwK raw BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "RwK raw payload");
    }
    return failures;
}

int TestNegativeNumbers()
{
    std::vector<std::string> parts = BuildParts();
    parts[1] = "-1";
    parts[22] = "-4";
    const cg::packet::PacketResult<TradePetInfo> parsed =
        TradePetInfo::Parse(BuildPlain(-3, JoinParts(parts, true)));

    int failures = Expect(static_cast<bool>(parsed), "parse RwK negative numbers");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().petPlace == -3, "RwK negative petPlace");
        failures += Expect(parsed.Value().data.level == -1, "RwK negative level");
        failures += Expect(parsed.Value().data.wind == -4, "RwK negative wind");
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;

    failures += ExpectError(
        TradePetInfo::Parse(BuildPlain(2, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "RwK",
        "RwK empty payload");

    std::vector<std::string> parts = BuildParts();
    failures += ExpectError(
        TradePetInfo::Parse(BuildPlain(2, JoinParts(parts, false))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "RwK",
        "RwK missing tail");

    parts = BuildParts();
    parts.push_back("extra");
    failures += ExpectError(
        TradePetInfo::Parse(BuildPlain(2, JoinParts(parts, true))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "RwK",
        "RwK extra field");

    failures += ExpectError(
        TradePetInfo::Parse(BuildPlain(2, BuildPayload() + "|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "RwK",
        "RwK extra trailing delimiter");

    parts = BuildParts();
    parts[1].clear();
    failures += ExpectError(
        TradePetInfo::Parse(BuildPlain(2, JoinParts(parts, true))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RwK",
        "RwK empty decimal");

    parts = BuildParts();
    parts[2] = "abc";
    failures += ExpectError(
        TradePetInfo::Parse(BuildPlain(2, JoinParts(parts, true))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RwK",
        "RwK invalid decimal");

    parts = BuildParts();
    parts[3] = "2147483648";
    failures += ExpectError(
        TradePetInfo::Parse(BuildPlain(2, JoinParts(parts, true))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RwK",
        "RwK decimal overflow");

    failures += ExpectError(
        TradePetInfo(2, TradePetInfoData {"A|B"}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "RwK",
        "RwK build literal delimiter");

    return failures;
}

int TestOuterErrors()
{
    int failures = 0;
    const std::string payload = BuildPayload();

    failures += ExpectError(
        TradePetInfo::Parse("ms Alice a "),
        cg::packet::PacketErrorCode::InvalidCode,
        "ms",
        "RwK wrong code");

    failures += ExpectError(
        TradePetInfo::Parse("RwK a"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "RwK",
        "RwK missing data");

    failures += ExpectError(
        TradePetInfo::Parse("RwK a data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "RwK",
        "RwK extra outer token");

    failures += ExpectError(
        TradePetInfo::Parse("RwK _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RwK",
        "RwK bad petPlace i62");

    failures += ExpectError(
        TradePetInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("petPlace", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, payload),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RwK",
        "RwK manual wrong direction");

    failures += ExpectError(
        TradePetInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("place", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, payload),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RwK",
        "RwK manual wrong field name");

    failures += ExpectError(
        TradePetInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("petPlace", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::RawString, payload),
            })),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "RwK",
        "RwK manual wrong field type");

    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestBuildRoundtrip();
    failures += TestRawEscapesPreserved();
    failures += TestNegativeNumbers();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
