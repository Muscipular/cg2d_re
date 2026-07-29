#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

using ProfileMailReceipt = cg::packet::receive::ProfileMailReceipt;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, ProfileMailReceipt::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, ProfileMailReceipt::Spec);
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
        failures += Expect(error.packetCode->View() == ProfileMailReceipt::Code, message);
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

std::string BuildPlainOw(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "ow ";
    }

    std::string plain = "ow 1 ";
    plain.append(encoded.Value());
    plain.append(" 3 ");
    return plain;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectProfileMailReceiptPacket(
    const cg::packet::Packet& packet,
    std::string_view dtext,
    cg::packet::PacketFieldType dtextType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(ProfileMailReceipt::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "profileIndex", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 1U, "dtext", dtextType, dtext, message);
    failures += ExpectField(packet, 2U, "color", cg::packet::PacketFieldType::Int62, "3", message);
    return failures;
}

int ExpectProfileMailReceiptValue(
    const ProfileMailReceipt& receipt,
    std::string_view sentTimeText,
    std::string_view message,
    const char* context)
{
    int failures = 0;
    failures += Expect(receipt.profileIndex == 1, context);
    failures += Expect(receipt.sentTimeText == sentTimeText, context);
    failures += Expect(receipt.message == message, context);
    failures += Expect(receipt.color == 3, context);
    return failures;
}

int TestGenericOuterRoundtrip()
{
    const std::string payload = "2026/ 7/29 12:34|hello|";
    const std::string plain = BuildPlainOw(payload);
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(plain);

    int failures = Expect(static_cast<bool>(parsed), "parse ow generic outer");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectProfileMailReceiptPacket(
        parsed.Value(),
        payload,
        cg::packet::PacketFieldType::EscapedString,
        "parse ow generic outer field");
    const cg::packet::PacketResult<std::string> rebuilt = BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(rebuilt), "build ow generic outer");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == plain, "roundtrip ow generic outer");
    }
    return failures;
}

int TestProductionTimeAndMessages()
{
    int failures = 0;
    const std::string sentTimeText = "2026/ 7/29 12:34";
    const cg::packet::PacketResult<ProfileMailReceipt> ordinary =
        ProfileMailReceipt::Parse(BuildPlainOw(sentTimeText + "|hello|"));
    failures += Expect(static_cast<bool>(ordinary), "parse ow production time");
    if (ordinary.HasValue()) {
        failures += ExpectProfileMailReceiptValue(
            ordinary.Value(),
            sentTimeText,
            "hello",
            "parse ow production time value");
    }

    const cg::packet::PacketResult<ProfileMailReceipt> empty =
        ProfileMailReceipt::Parse(BuildPlainOw("opaque time||"));
    failures += Expect(static_cast<bool>(empty), "parse ow empty message");
    if (empty.HasValue()) {
        failures += ExpectProfileMailReceiptValue(
            empty.Value(),
            "opaque time",
            "",
            "parse ow empty message value");
        const cg::packet::PacketResult<std::string> rebuilt = empty.Value().BuildData();
        failures += Expect(static_cast<bool>(rebuilt), "build ow empty message");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "opaque time||", "roundtrip ow empty message");
        }
    }
    return failures;
}

int TestLegacyEscapeCanonical()
{
    const std::string sentTimeText = "2026/ 7/29 12:34";
    const std::string legacy =
        sentTimeText + "|line\\ncomma\\cpipe\\zslash\\yunknown\\q|";
    const std::string semantic =
        "line\ncomma,pipe|slash\\unknownq";
    const std::string canonical =
        sentTimeText + "|line\\ncomma\\cpipe\\zslash\\yunknownq|";
    const cg::packet::PacketResult<ProfileMailReceipt> parsed =
        ProfileMailReceipt::Parse(BuildPlainOw(legacy));

    int failures = Expect(static_cast<bool>(parsed), "parse ow legacy escapes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectProfileMailReceiptValue(
        parsed.Value(),
        sentTimeText,
        semantic,
        "decode ow legacy n/c/z/y and unknown");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "build ow legacy canonical");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "canonicalize ow legacy escapes");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "ow legacy ToPacket");
    if (packet.HasValue()) {
        failures += ExpectProfileMailReceiptPacket(
            packet.Value(),
            canonical,
            cg::packet::PacketFieldType::SubPayload,
            "ow legacy ToPacket field");
        const cg::packet::PacketResult<std::string> wire = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(wire), "build ow legacy wire");
        if (wire.HasValue()) {
            failures += Expect(wire.Value() == BuildPlainOw(canonical), "canonical ow legacy wire");
        }
    }
    return failures;
}

int TestGbkTrailDelimiter()
{
    std::string message;
    message.push_back(static_cast<char>(0x81));
    message.push_back(static_cast<char>(0x7c));

    std::string payload = "time|";
    payload.append(message);
    payload.push_back('|');
    const cg::packet::PacketResult<ProfileMailReceipt> parsed =
        ProfileMailReceipt::Parse(BuildPlainOw(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ow GBK 81 7C");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectProfileMailReceiptValue(
        parsed.Value(),
        "time",
        message,
        "parse ow GBK message");
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build ow GBK 81 7C");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip ow GBK 81 7C");
    }
    return failures;
}

int TestPayloadShapeErrors()
{
    int failures = 0;
    failures += ExpectErrorAt(
        ProfileMailReceipt::Parse(BuildPlainOw("time|message")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        1U,
        "typed ow missing trailing segment");
    failures += ExpectErrorAt(
        ProfileMailReceipt::Parse(BuildPlainOw("time|message||")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        1U,
        "typed ow extra payload segment");
    failures += ExpectErrorAt(
        ProfileMailReceipt::Parse(BuildPlainOw("time|message|nonempty")),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "typed ow trailing segment not empty");
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ProfileMailReceipt::Parse("ow 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed ow missing outer field");
    failures += ExpectError(
        ProfileMailReceipt::Parse("ow 1 text 3 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed ow extra outer field");
    failures += ExpectErrorAt(
        ProfileMailReceipt::Parse("ow _ text 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "typed ow bad profileIndex i62");
    failures += ExpectErrorAt(
        ProfileMailReceipt::Parse("ow 1 text _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "typed ow bad color i62");
    failures += ExpectErrorAt(
        ProfileMailReceipt::Parse(BuildPlainOw("time|message\\|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "typed ow truncated legacy escape");

    std::string truncatedGbk = "ow 1 time|message|";
    truncatedGbk.push_back(static_cast<char>(0x81));
    truncatedGbk.append(" 3 ");
    failures += ExpectErrorAt(
        ProfileMailReceipt::Parse(truncatedGbk),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "typed ow truncated GBK");
    return failures;
}

int TestBuildAndToPacketErrors()
{
    std::string truncatedGbk(1U, static_cast<char>(0x81));
    const ProfileMailReceipt receipt(
        1,
        "opaque time",
        std::move(truncatedGbk),
        3);

    int failures = 0;
    failures += ExpectErrorAt(
        receipt.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "ow BuildData truncated GBK context");
    failures += ExpectErrorAt(
        receipt.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "ow ToPacket truncated GBK context");
    return failures;
}

int TestCanonicalToPacket()
{
    const ProfileMailReceipt receipt(
        1,
        "2026/ 7/29 12:34",
        "hello",
        3);
    const std::string payload = "2026/ 7/29 12:34|hello|";
    const cg::packet::PacketResult<cg::packet::Packet> packet = receipt.ToPacket();

    int failures = Expect(static_cast<bool>(packet), "ow canonical ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectProfileMailReceiptPacket(
        packet.Value(),
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "ow canonical ToPacket field");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build ow canonical packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainOw(payload), "build ow canonical wire");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestGenericOuterRoundtrip();
    failures += TestProductionTimeAndMessages();
    failures += TestLegacyEscapeCanonical();
    failures += TestGbkTrailDelimiter();
    failures += TestPayloadShapeErrors();
    failures += TestParseErrors();
    failures += TestBuildAndToPacketErrors();
    failures += TestCanonicalToPacket();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
