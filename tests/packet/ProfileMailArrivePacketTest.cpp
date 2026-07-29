#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

using ProfileMailArrive = cg::packet::receive::ProfileMailArrive;

constexpr std::string_view SamplePayload =
    "12|1000|35|Alice|Guild|Title|9|2026-06-01 12:00|Hello||3";

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, ProfileMailArrive::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, ProfileMailArrive::Spec);
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
        failures += Expect(error.packetCode->View() == ProfileMailArrive::Code, message);
    }
    return failures;
}

template <typename T>
int ExpectErrorAt(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, message);
    if (result.HasError()) {
        failures += Expect(result.Error().fieldIndex.has_value(), message);
        if (result.Error().fieldIndex.has_value()) {
            failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
        }
    }
    return failures;
}

std::string BuildPlainKyN(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "KyN ";
    }

    std::string plain = "KyN ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildRawKyN(std::string_view payload)
{
    std::string plain = "KyN ";
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    cg::packet::PacketFieldType type,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(ProfileMailArrive::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == type, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectSampleValue(const ProfileMailArrive& mail, const char* message)
{
    int failures = 0;
    failures += Expect(mail.profileIndex == 12, message);
    failures += Expect(mail.faceImageNumber == 1000, message);
    failures += Expect(mail.level == 35, message);
    failures += Expect(mail.name == "Alice", message);
    failures += Expect(mail.guildName == "Guild", message);
    failures += Expect(mail.ownTitle == "Title", message);
    failures += Expect(mail.senderProfileIndex == 9, message);
    failures += Expect(mail.sentTimeText == "2026-06-01 12:00", message);
    failures += Expect(mail.message == "Hello", message);
    failures += Expect(mail.color == 3, message);
    return failures;
}

int TestGenericOuterRoundtrip()
{
    const std::string plain = BuildPlainKyN(SamplePayload);
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(plain);

    int failures = Expect(static_cast<bool>(parsed), "parse KyN generic outer");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectPacket(
        parsed.Value(),
        SamplePayload,
        cg::packet::PacketFieldType::EscapedString,
        "parse KyN generic outer field");
    const cg::packet::PacketResult<std::string> rebuilt = BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(rebuilt), "build KyN generic outer");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == plain, "roundtrip KyN generic outer");
    }
    return failures;
}

int TestTypedSubPayloadRoundtrip()
{
    const cg::packet::PacketResult<ProfileMailArrive> parsed =
        ProfileMailArrive::Parse(BuildPlainKyN(SamplePayload));

    int failures = Expect(static_cast<bool>(parsed), "typed parse KyN");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectSampleValue(parsed.Value(), "typed KyN value");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "build KyN data");
    if (data.HasValue()) {
        failures += Expect(data.Value() == SamplePayload, "build KyN exact 11 segments without tail");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "KyN ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectPacket(
        packet.Value(),
        SamplePayload,
        cg::packet::PacketFieldType::SubPayload,
        "KyN typed SubPayload");
    const cg::packet::PacketResult<std::string> wire = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(wire), "build KyN wire");
    if (!wire.HasValue()) {
        return failures;
    }

    failures += Expect(wire.Value() == BuildPlainKyN(SamplePayload), "build KyN canonical wire");
    const cg::packet::PacketResult<ProfileMailArrive> roundtrip =
        ProfileMailArrive::Parse(wire.Value());
    failures += Expect(static_cast<bool>(roundtrip), "roundtrip KyN typed packet");
    if (roundtrip.HasValue()) {
        failures += ExpectSampleValue(roundtrip.Value(), "roundtrip KyN typed value");
    }
    return failures;
}

int TestFiveLegacyTextsCanonical()
{
    constexpr std::string_view legacy =
        "-12|-1000|-35|name\\n|guild\\c|title\\z|-9|time\\y|message\\q||-3";
    constexpr std::string_view canonical =
        "-12|-1000|-35|name\\n|guild\\c|title\\z|-9|time\\y|messageq||-3";
    const cg::packet::PacketResult<ProfileMailArrive> parsed =
        ProfileMailArrive::Parse(BuildPlainKyN(legacy));

    int failures = Expect(static_cast<bool>(parsed), "parse KyN five legacy texts");
    if (!parsed.HasValue()) {
        return failures;
    }

    const ProfileMailArrive& mail = parsed.Value();
    failures += Expect(mail.profileIndex == -12, "parse KyN signed profileIndex");
    failures += Expect(mail.faceImageNumber == -1000, "parse KyN signed faceImageNumber");
    failures += Expect(mail.level == -35, "parse KyN signed level");
    failures += Expect(mail.name == "name\n", "decode KyN name legacy n");
    failures += Expect(mail.guildName == "guild,", "decode KyN guild legacy c");
    failures += Expect(mail.ownTitle == "title|", "decode KyN title legacy z");
    failures += Expect(mail.senderProfileIndex == -9, "parse KyN signed senderProfileIndex");
    failures += Expect(mail.sentTimeText == "time\\", "decode KyN time legacy y");
    failures += Expect(mail.message == "messageq", "decode KyN message unknown legacy");
    failures += Expect(mail.color == -3, "parse KyN signed color");

    const cg::packet::PacketResult<std::string> data = mail.BuildData();
    failures += Expect(static_cast<bool>(data), "build KyN legacy canonical");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "canonicalize KyN five legacy texts");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = mail.ToPacket();
    failures += Expect(static_cast<bool>(packet), "KyN legacy ToPacket");
    if (packet.HasValue()) {
        failures += ExpectPacket(
            packet.Value(),
            canonical,
            cg::packet::PacketFieldType::SubPayload,
            "KyN legacy canonical SubPayload");
    }
    return failures;
}

int TestGbkTrailDelimiter()
{
    std::string gbk;
    gbk.push_back(static_cast<char>(0x81));
    gbk.push_back(static_cast<char>(0x7c));

    std::string payload = "12|1000|35|";
    payload.append(gbk);
    payload.append("|Guild|Title|9|time|message||3");
    const cg::packet::PacketResult<ProfileMailArrive> parsed =
        ProfileMailArrive::Parse(BuildPlainKyN(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse KyN GBK 81 7C");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().name == gbk, "decode KyN GBK name");
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build KyN GBK 81 7C");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip KyN GBK 81 7C");
    }
    return failures;
}

int TestPayloadShapeAndDecimalErrors()
{
    int failures = 0;
    failures += ExpectErrorAt(
        ProfileMailArrive::Parse(BuildPlainKyN("12|1000|35|Alice|Guild|Title|9|time|message|3")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "reject KyN 10 payload segments");
    failures += ExpectErrorAt(
        ProfileMailArrive::Parse(BuildPlainKyN("12|1000|35|Alice|Guild|Title|9|time|message|||3")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "reject KyN trailing payload segment");
    failures += ExpectErrorAt(
        ProfileMailArrive::Parse(BuildPlainKyN("12|1000|35|Alice|Guild|Title|9|time|message|x|3")),
        cg::packet::PacketErrorCode::DecodeFailed,
        9U,
        "reject KyN non-empty fixed segment");

    constexpr std::array<std::string_view, 5U> badDecimals = {{
        "x|1000|35|Alice|Guild|Title|9|time|message||3",
        "12|x|35|Alice|Guild|Title|9|time|message||3",
        "12|1000|x|Alice|Guild|Title|9|time|message||3",
        "12|1000|35|Alice|Guild|Title|x|time|message||3",
        "12|1000|35|Alice|Guild|Title|9|time|message||x",
    }};
    for (const std::string_view payload : badDecimals) {
        failures += ExpectError(
            ProfileMailArrive::Parse(BuildPlainKyN(payload)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "reject KyN invalid raw decimal");
    }
    return failures;
}

int TestTruncatedParseErrors()
{
    int failures = 0;
    failures += ExpectErrorAt(
        ProfileMailArrive::Parse(
            BuildPlainKyN("12|1000|35|name\\|Guild|Title|9|time|message||3")),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "reject KyN truncated legacy text");

    std::string truncatedPayload = "12|1000|35|Alice|Guild|Title|9|time|message||";
    truncatedPayload.push_back(static_cast<char>(0x81));
    failures += ExpectErrorAt(
        ProfileMailArrive::Parse(BuildRawKyN(truncatedPayload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "reject KyN truncated GBK");
    return failures;
}

int TestBuildAndToPacketErrors()
{
    std::string truncatedGbk(1U, static_cast<char>(0x81));
    const ProfileMailArrive mail(
        12,
        1000,
        35,
        std::move(truncatedGbk),
        "Guild",
        "Title",
        9,
        "time",
        "message",
        3);

    int failures = 0;
    failures += ExpectErrorAt(
        mail.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "KyN BuildData truncated GBK context");
    failures += ExpectErrorAt(
        mail.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "KyN ToPacket truncated GBK context");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestGenericOuterRoundtrip();
    failures += TestTypedSubPayloadRoundtrip();
    failures += TestFiveLegacyTextsCanonical();
    failures += TestGbkTrailDelimiter();
    failures += TestPayloadShapeAndDecimalErrors();
    failures += TestTruncatedParseErrors();
    failures += TestBuildAndToPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
