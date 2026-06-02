#include "packet/packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

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
        cg::packet::PacketCode(cg::packet::receive::DuelPoint::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildPlainHb(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "hB ";
    }

    std::string plain(cg::packet::receive::DuelPoint::Code);
    plain.push_back(' ');
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectDuelPointPacket(
    const cg::packet::Packet& packet,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::DuelPoint::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == data, message);
    return failures;
}

int ExpectDuelPointValues(
    const cg::packet::receive::DuelPoint& duelPoint,
    int dpAdd,
    int duelPointNow,
    const char* message)
{
    int failures = 0;
    failures += Expect(duelPoint.dpAdd == dpAdd, message);
    failures += Expect(duelPoint.duelPointNow == duelPointNow, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainHb("1|1C|"));

    int failures = Expect(static_cast<bool>(parsed), "parse hB outer");
    if (parsed.HasValue()) {
        failures += ExpectDuelPointPacket(parsed.Value(), "1|1C|", "parse hB outer packet");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    const cg::packet::receive::DuelPoint duelPoint(1, 100);

    int failures = Expect(duelPoint.BuildData() == "1|1C|", "hB BuildData 1 100");
    failures += ExpectDuelPointPacket(duelPoint.ToPacket(), "1|1C|", "hB ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(duelPoint.ToPacket());
    failures += Expect(static_cast<bool>(built), "build hB packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainHb("1|1C|"), "build hB plain");

    const cg::packet::PacketResult<cg::packet::receive::DuelPoint> parsed =
        cg::packet::receive::DuelPoint::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip hB");
    if (parsed.HasValue()) {
        failures += ExpectDuelPointValues(parsed.Value(), 1, 100, "roundtrip hB values");
    }
    return failures;
}

int TestZeroAndNegative()
{
    int failures = 0;
    const cg::packet::PacketResult<cg::packet::receive::DuelPoint> zero =
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("0|0|"));
    failures += Expect(static_cast<bool>(zero), "parse hB zero");
    if (zero.HasValue()) {
        failures += ExpectDuelPointValues(zero.Value(), 0, 0, "hB zero values");
    }

    const cg::packet::PacketResult<cg::packet::receive::DuelPoint> negative =
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("-1|1C|"));
    failures += Expect(static_cast<bool>(negative), "parse hB negative dpAdd");
    if (negative.HasValue()) {
        failures += ExpectDuelPointValues(negative.Value(), -1, 100, "hB negative values");
    }
    return failures;
}

int TestPayloadBase62()
{
    const cg::packet::PacketResult<cg::packet::receive::DuelPoint> parsed =
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("10|10|"));

    int failures = Expect(static_cast<bool>(parsed), "parse hB base62 not decimal");
    if (parsed.HasValue()) {
        failures += ExpectDuelPointValues(parsed.Value(), 62, 62, "hB 10 means base62 62");
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("1|1C")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hB",
        "hB missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("1|1C|0")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hB",
        "hB non-empty tail");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("1|1C|0|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hB",
        "hB too many parts");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("|1C|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hB",
        "hB empty dpAdd");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("1||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hB",
        "hB empty duelPointNow");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::Parse(BuildPlainHb("_|1C|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hB",
        "hB invalid i62");
    return failures;
}

int TestOuterErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("hB"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hB",
        "hB missing field");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::Parse("hB"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hB",
        "typed hB missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("hB 1|1C| extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hB",
        "hB extra field");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::Parse("Gn 1|1C| "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Gn",
        "typed hB wrong code");
    return failures;
}

int TestManualPacketErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::DuelPoint::FromPacket(PacketOf(cg::packet::PacketDirection::ClientToServer, {
            Field("data", cg::packet::PacketFieldType::EscapedString, "1|1C|"),
        })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hB",
        "hB manual wrong direction");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::FromPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hB",
        "hB manual missing field");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::FromPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("data", cg::packet::PacketFieldType::EscapedString, "1|1C|"),
            Field("extra", cg::packet::PacketFieldType::EscapedString, "0"),
        })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hB",
        "hB manual extra field");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::FromPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("payload", cg::packet::PacketFieldType::EscapedString, "1|1C|"),
        })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hB",
        "hB manual wrong field name");
    failures += ExpectError(
        cg::packet::receive::DuelPoint::FromPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("data", cg::packet::PacketFieldType::Int62, "1"),
        })),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "hB",
        "hB manual wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestBuildRoundtrip();
    failures += TestZeroAndNegative();
    failures += TestPayloadBase62();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    failures += TestManualPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
