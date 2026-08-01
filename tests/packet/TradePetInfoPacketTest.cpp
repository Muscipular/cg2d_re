#include "packet/Packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::TradePetInfo::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::TradePetInfo::Spec);
}

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
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string LegacyDoubleByteText(unsigned char trail, std::string_view suffix)
{
    std::string text;
    text.push_back(static_cast<char>(0x81));
    text.push_back(static_cast<char>(trail));
    text.append(suffix);
    return text;
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
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(TradePetInfo::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "petPlace", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(petPlace), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == dataType, message);
    failures += Expect(packet.fields[1].value == payload, message);
    return failures;
}

int ExpectData(const TradePetInfoData& data, const char* message)
{
    int failures = 0;
    failures += Expect(data.freeNameRaw == "Free|Name", message);
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
    failures += Expect(data.nameRaw == "Pet\nName", message);
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
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain(2, payload));

    int failures = Expect(TradePetInfo::Spec.fieldCount == 2U, "RwK Spec exact2");
    failures += Expect(
        TradePetInfo::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "RwK Spec petPlace i62");
    failures += Expect(
        TradePetInfo::Fields[1].type == cg::packet::PacketFieldType::SubPayload,
        "RwK Spec data SubPayload");
    failures += Expect(static_cast<bool>(parsed), "parse RwK outer");
    if (parsed.HasValue()) {
        failures += ExpectTradePetInfoPacket(
            parsed.Value(),
            2,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse RwK packet");
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

    failures += ExpectTradePetInfoPacket(
        typedPacket.Value(),
        2,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "RwK ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(typedPacket.Value());
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

int TestLegacyDecodeAndRawBuild()
{
    const std::string raw = "A\\zB\\cC\\yD\\nE";
    std::vector<std::string> parts = BuildParts();
    parts[0] = raw;
    parts[12] = raw;
    const std::string payload = JoinParts(parts, true);

    const cg::packet::PacketResult<TradePetInfo> parsed = TradePetInfo::Parse(BuildPlain(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse RwK legacy escaped strings");
    if (!parsed.HasValue()) {
        return failures;
    }

    const std::string semantic = "A|B,C\\D\nE";
    failures += Expect(parsed.Value().data.freeNameRaw == semantic, "RwK decoded freeName");
    failures += Expect(parsed.Value().data.nameRaw == semantic, "RwK decoded name");
    failures += ExpectError(
        parsed.Value().BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "RwK",
        "RwK decoded literal pipe is not Server raw");

    TradePetInfoData rawData = BuildData();
    rawData.freeNameRaw = "A\\zB";
    rawData.nameRaw = "C\\nD";
    std::vector<std::string> rawParts = BuildParts();
    rawParts[0] = rawData.freeNameRaw;
    rawParts[12] = rawData.nameRaw;
    const std::string rawPayload = JoinParts(rawParts, true);

    const TradePetInfo rawPacket(2, std::move(rawData));
    const cg::packet::PacketResult<std::string> data = rawPacket.BuildData();
    failures += Expect(static_cast<bool>(data), "RwK Server raw BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == rawPayload, "RwK BuildData preserves raw legacy tokens");
    }

    const cg::packet::PacketResult<TradePetInfo> reparsed =
        TradePetInfo::Parse(BuildPlain(2, rawPayload));
    failures += Expect(static_cast<bool>(reparsed), "RwK reparses Server raw payload");
    if (reparsed.HasValue()) {
        failures += Expect(reparsed.Value().data.freeNameRaw == "A|B", "RwK raw freeName decodes once");
        failures += Expect(reparsed.Value().data.nameRaw == "C\nD", "RwK raw name decodes once");
    }
    return failures;
}

int TestGbkRawRoundtrip()
{
    const std::string freeName = LegacyDoubleByteText(0x7C, "Free");
    const std::string name = LegacyDoubleByteText(0x5C, "Pet");
    std::vector<std::string> parts = BuildParts();
    parts[0] = freeName;
    parts[12] = name;
    const std::string payload = JoinParts(parts, true);

    const cg::packet::PacketResult<TradePetInfo> parsed = TradePetInfo::Parse(BuildPlain(2, payload));
    int failures = Expect(static_cast<bool>(parsed), "parse RwK GBK trail bytes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().data.freeNameRaw == freeName, "RwK GBK trail 0x7C preserved");
    failures += Expect(parsed.Value().data.nameRaw == name, "RwK GBK trail 0x5C preserved");
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "RwK GBK raw rebuild");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "RwK GBK raw roundtrip");
    }
    return failures;
}

int TestEmptyTexts()
{
    std::vector<std::string> parts = BuildParts();
    parts[0].clear();
    parts[12].clear();
    const std::string payload = JoinParts(parts, true);
    const cg::packet::PacketResult<TradePetInfo> parsed = TradePetInfo::Parse(BuildPlain(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse RwK empty texts");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data.freeNameRaw.empty(), "RwK empty freeName");
        failures += Expect(parsed.Value().data.nameRaw.empty(), "RwK empty name");
        const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
        failures += Expect(rebuilt.HasValue(), "RwK empty texts rebuild");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == payload, "RwK empty texts raw roundtrip");
        }
    }
    return failures;
}

int TestSignedNumbers()
{
    std::vector<std::string> parts = BuildParts();
    parts[1] = "-2147483648";
    parts[22] = "2147483647";
    const cg::packet::PacketResult<TradePetInfo> parsed =
        TradePetInfo::Parse(BuildPlain(-3, JoinParts(parts, true)));

    int failures = Expect(static_cast<bool>(parsed), "parse RwK signed decimal boundaries");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().petPlace == -3, "RwK negative petPlace");
        failures += Expect(
            parsed.Value().data.level == std::numeric_limits<int>::min(),
            "RwK minimum level decimal");
        failures += Expect(
            parsed.Value().data.wind == std::numeric_limits<int>::max(),
            "RwK maximum wind decimal");
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

    TradePetInfoData truncatedFreeName = BuildData();
    truncatedFreeName.freeNameRaw.assign(1U, static_cast<char>(0x81));
    failures += ExpectError(
        TradePetInfo(2, std::move(truncatedFreeName)).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "RwK",
        "RwK build truncated freeName GBK");

    TradePetInfoData truncatedName = BuildData();
    truncatedName.nameRaw.assign(1U, static_cast<char>(0x81));
    failures += ExpectError(
        TradePetInfo(2, std::move(truncatedName)).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "RwK",
        "RwK build truncated name GBK");

    std::string truncatedPayload = BuildPayload();
    truncatedPayload.push_back(static_cast<char>(0x81));
    failures += ExpectError(
        TradePetInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("petPlace", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, std::move(truncatedPayload)),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RwK",
        "RwK parse truncated GBK");

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
                Field("data", cg::packet::PacketFieldType::Unknown, payload),
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
    failures += TestLegacyDecodeAndRawBuild();
    failures += TestGbkRawRoundtrip();
    failures += TestEmptyTexts();
    failures += TestSignedNumbers();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
