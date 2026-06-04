#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ProfileMailReceipt::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ProfileMailReceipt::Spec);
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

std::string BuildPlainOw(std::string_view text)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(text);
    if (!encoded.HasValue()) {
        return "ow 1  3 ";
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
    std::string_view text,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileMailReceipt::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "profileIndex", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 1U, "dtext", cg::packet::PacketFieldType::EscapedString, text, message);
    failures += ExpectField(packet, 2U, "color", cg::packet::PacketFieldType::Int62, "3", message);
    return failures;
}

int ExpectProfileMailReceiptValue(
    const cg::packet::receive::ProfileMailReceipt& packet,
    int profileIndex,
    std::string_view text,
    int color,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.profileIndex == profileIndex, message);
    failures += Expect(packet.text == text, message);
    failures += Expect(packet.color == color, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainOw("2026-06-01|hello|"));

    int failures = Expect(static_cast<bool>(parsed), "parse ow outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailReceiptPacket(parsed.Value(), "2026-06-01|hello|", "parse ow outer packet");
    }
    return failures;
}

int TestTypedParse()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailReceipt> parsed =
        cg::packet::receive::ProfileMailReceipt::Parse("ow 1 2026-06-01|hello| 3 ");

    int failures = Expect(static_cast<bool>(parsed), "typed parse ow");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailReceiptValue(parsed.Value(), 1, "2026-06-01|hello|", 3, "typed ow value");
    }
    return failures;
}

int TestEscapedTextRoundtrip()
{
    const std::string text = "2026-06-01|hello with space|";
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailReceipt> parsed =
        cg::packet::receive::ProfileMailReceipt::Parse(BuildPlainOw(text));

    int failures = Expect(static_cast<bool>(parsed), "typed parse ow escaped text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectProfileMailReceiptValue(parsed.Value(), 1, text, 3, "typed ow escaped text value");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build ow escaped text");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainOw(text), "roundtrip ow escaped text");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    const cg::packet::receive::ProfileMailReceipt packetValue(1, "2026-06-01|hello|", 3);
    const cg::packet::Packet packet = packetValue.ToPacket();

    int failures = ExpectProfileMailReceiptPacket(packet, "2026-06-01|hello|", "ow ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build ow packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainOw("2026-06-01|hello|"), "build ow packet value");

    const cg::packet::PacketResult<cg::packet::receive::ProfileMailReceipt> parsed =
        cg::packet::receive::ProfileMailReceipt::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip ow packet");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailReceiptValue(parsed.Value(), 1, "2026-06-01|hello|", 3, "roundtrip ow value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ProfileMailReceipt::Parse("ow 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ow",
        "typed ow missing field");
    failures += ExpectError(
        cg::packet::receive::ProfileMailReceipt::Parse("ow 1 text 3 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ow",
        "typed ow extra field");
    failures += ExpectError(
        cg::packet::receive::ProfileMailReceipt::Parse("ow _ text 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ow",
        "typed ow bad profileIndex i62");
    failures += ExpectError(
        cg::packet::receive::ProfileMailReceipt::Parse("ow 1 text _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ow",
        "typed ow bad color i62");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParse();
    failures += TestEscapedTextRoundtrip();
    failures += TestBuildRoundtrip();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
