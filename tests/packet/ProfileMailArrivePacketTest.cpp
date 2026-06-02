#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view SamplePayload = "12|1000|35|Alice|Guild|Title|9|2026-06-01 12:00|Hello||3";

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

std::string BuildPlainKyN(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "KyN  ";
    }

    std::string plain = "KyN ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectProfileMailArrivePacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileMailArrive::Code), message);
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

int ExpectSampleValue(const cg::packet::receive::ProfileMailArrive& packet, const char* message)
{
    int failures = 0;
    failures += Expect(packet.profileIndex == 12, message);
    failures += Expect(packet.faceImageNumber == 1000, message);
    failures += Expect(packet.level == 35, message);
    failures += Expect(packet.name == "Alice", message);
    failures += Expect(packet.guildName == "Guild", message);
    failures += Expect(packet.ownTitle == "Title", message);
    failures += Expect(packet.senderProfileIndex == 9, message);
    failures += Expect(packet.sentTimeText == "2026-06-01 12:00", message);
    failures += Expect(packet.message == "Hello", message);
    failures += Expect(packet.color == 3, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainKyN(SamplePayload));

    int failures = Expect(static_cast<bool>(parsed), "parse KyN outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailArrivePacket(parsed.Value(), SamplePayload, "parse KyN outer packet");
    }
    return failures;
}

int TestTypedParse()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailArrive> parsed =
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN(SamplePayload));

    int failures = Expect(static_cast<bool>(parsed), "typed parse KyN");
    if (parsed.HasValue()) {
        failures += ExpectSampleValue(parsed.Value(), "typed KyN value");
        failures += Expect(parsed.Value().BuildData() == SamplePayload, "typed KyN preserve empty segment");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    const cg::packet::receive::ProfileMailArrive packetValue(
        12,
        1000,
        35,
        "Alice",
        "Guild",
        "Title",
        9,
        "2026-06-01 12:00",
        "Hello",
        3);
    const cg::packet::Packet packet = packetValue.ToPacket();

    int failures = ExpectProfileMailArrivePacket(packet, SamplePayload, "KyN ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build KyN packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainKyN(SamplePayload), "build KyN packet value");

    const cg::packet::PacketResult<cg::packet::receive::ProfileMailArrive> parsed =
        cg::packet::receive::ProfileMailArrive::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip KyN packet");
    if (parsed.HasValue()) {
        failures += ExpectSampleValue(parsed.Value(), "roundtrip KyN value");
    }
    return failures;
}

int TestEmptyGuildAndTitle()
{
    constexpr std::string_view payload = "12|1000|35|Alice|||9|time|message||3";
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailArrive> parsed =
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN(payload));

    int failures = Expect(static_cast<bool>(parsed), "typed parse KyN empty guild/title");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().guildName.empty(), "typed KyN empty guildName");
        failures += Expect(parsed.Value().ownTitle.empty(), "typed KyN empty ownTitle");
        failures += Expect(parsed.Value().BuildData() == payload, "typed KyN empty guild/title roundtrip");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse("KyN"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KyN",
        "typed KyN missing data");
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN("12|1000|35|Alice|Guild|Title|9|time|message|3")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KyN",
        "typed KyN too few payload fields");
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN("12|1000|35|Alice|Guild|Title|9|time|message|||3")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KyN",
        "typed KyN too many payload fields");
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN("12|1000|35|Alice|Guild|Title|9|time|message|x|3")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KyN",
        "typed KyN missing fixed empty field");
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN("x|1000|35|Alice|Guild|Title|9|time|message||3")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KyN",
        "typed KyN bad profileIndex decimal");
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN("12|x|35|Alice|Guild|Title|9|time|message||3")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KyN",
        "typed KyN bad faceImageNumber decimal");
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN("12|1000|x|Alice|Guild|Title|9|time|message||3")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KyN",
        "typed KyN bad level decimal");
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN("12|1000|35|Alice|Guild|Title|x|time|message||3")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KyN",
        "typed KyN bad senderProfileIndex decimal");
    failures += ExpectError(
        cg::packet::receive::ProfileMailArrive::Parse(BuildPlainKyN("12|1000|35|Alice|Guild|Title|9|time|message||x")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KyN",
        "typed KyN bad color decimal");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParse();
    failures += TestBuildRoundtrip();
    failures += TestEmptyGuildAndTitle();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
