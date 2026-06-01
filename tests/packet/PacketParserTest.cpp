#include "packet/PacketParser.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace {

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

int ExpectError(
    const cg::packet::PacketResult<cg::packet::Packet>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    const char* message)
{
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (result.HasError()) {
        const cg::packet::PacketError& error = result.Error();
        failures += Expect(error.code == code, message);
        if (!packetCode.empty()) {
            failures += Expect(error.packetCode.has_value(), message);
            if (error.packetCode.has_value()) {
                failures += Expect(error.packetCode->View() == packetCode, message);
            }
        }
    }
    return failures;
}

int ExpectParsedField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int TestKnownPackets()
{
    int failures = 0;

    const cg::packet::PacketResult<cg::packet::Packet> login =
        cg::packet::ParsePacket("JFVf hash pass account ");
    failures += Expect(static_cast<bool>(login), "parse JFVf");
    if (login.HasValue()) {
        const cg::packet::Packet& packet = login.Value();
        failures += Expect(packet.code == cg::packet::PacketCode("JFVf"), "JFVf code");
        failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, "JFVf direction");
        failures += Expect(packet.source == cg::packet::PacketSourceTag::ExistingPrototype, "JFVf source");
        failures += Expect(packet.fields.size() == 3U, "JFVf field count");
        failures += ExpectParsedField(
            packet,
            0U,
            "clientHash",
            cg::packet::PacketFieldType::EscapedString,
            "hash",
            "JFVf clientHash");
        failures += ExpectParsedField(
            packet,
            1U,
            "password",
            cg::packet::PacketFieldType::EscapedString,
            "pass",
            "JFVf password");
        failures += ExpectParsedField(
            packet,
            2U,
            "account",
            cg::packet::PacketFieldType::EscapedString,
            "account",
            "JFVf account");
    }

    const cg::packet::PacketResult<cg::packet::Packet> checkCommand = cg::packet::ParsePacket("CCL a ");
    failures += Expect(static_cast<bool>(checkCommand), "parse CCL");
    if (checkCommand.HasValue()) {
        const cg::packet::Packet& packet = checkCommand.Value();
        failures += Expect(packet.code == cg::packet::PacketCode("CCL"), "CCL code");
        failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, "CCL direction");
        failures += Expect(packet.source == cg::packet::PacketSourceTag::Xlsx, "CCL source");
        failures += Expect(packet.fields.size() == 1U, "CCL field count");
        failures += ExpectParsedField(
            packet,
            0U,
            "time",
            cg::packet::PacketFieldType::Int62,
            "10",
            "CCL time");
    }

    const cg::packet::PacketResult<cg::packet::Packet> charList = cg::packet::ParsePacket("Gp");
    failures += Expect(static_cast<bool>(charList), "parse Gp");
    if (charList.HasValue()) {
        failures += Expect(charList.Value().code == cg::packet::PacketCode("Gp"), "Gp code");
        failures += Expect(charList.Value().fields.empty(), "Gp field count");
    }

    const cg::packet::PacketResult<cg::packet::Packet> charListTrailing = cg::packet::ParsePacket("Gp ");
    failures += Expect(static_cast<bool>(charListTrailing), "parse Gp trailing");
    if (charListTrailing.HasValue()) {
        failures += Expect(charListTrailing.Value().fields.empty(), "Gp trailing field count");
    }

    const cg::packet::PacketResult<cg::packet::Packet> busy = cg::packet::ParsePacket("whV 0 busy ");
    failures += Expect(static_cast<bool>(busy), "parse whV busy");
    if (busy.HasValue()) {
        const cg::packet::Packet& packet = busy.Value();
        failures += Expect(packet.code == cg::packet::PacketCode("whV"), "whV code");
        failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, "whV direction");
        failures += Expect(packet.fields.size() == 2U, "whV field count");
        failures += ExpectParsedField(
            packet,
            0U,
            "result",
            cg::packet::PacketFieldType::Int62,
            "0",
            "whV result");
        failures += ExpectParsedField(
            packet,
            1U,
            "data",
            cg::packet::PacketFieldType::EscapedString,
            "busy",
            "whV data");
    }

    return failures;
}

int TestEnvelopeBoundary()
{
    const cg::packet::PacketResult<std::string> envelope = cg::packet::EncodeEnvelope("whV 0 busy ");
    int failures = Expect(static_cast<bool>(envelope), "encode whV envelope");
    if (!envelope.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> plain = cg::packet::DecodeEnvelope(envelope.Value());
    failures += Expect(static_cast<bool>(plain), "decode whV envelope");
    if (!plain.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(plain.Value());
    failures += Expect(static_cast<bool>(parsed), "parse decoded whV");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().code == cg::packet::PacketCode("whV"), "decoded whV code");
        failures += Expect(parsed.Value().fields.size() == 2U, "decoded whV field count");
        failures += ExpectParsedField(
            parsed.Value(),
            1U,
            "data",
            cg::packet::PacketFieldType::EscapedString,
            "busy",
            "decoded whV data");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(cg::packet::ParsePacket(""), cg::packet::PacketErrorCode::EmptyInput, "", "empty input");
    failures += ExpectError(cg::packet::ParsePacket(" "), cg::packet::PacketErrorCode::InvalidCode, "", "empty code");
    failures += ExpectError(cg::packet::ParsePacket("BAD"), cg::packet::PacketErrorCode::UnknownCode, "BAD", "unknown code");

    const cg::packet::PacketResult<cg::packet::Packet> missingField = cg::packet::ParsePacket("whV 0");
    failures += ExpectError(
        missingField,
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "whV",
        "field count missing");
    if (missingField.HasError()) {
        failures += Expect(missingField.Error().fieldIndex == 1U, "missing field index");
    }

    const cg::packet::PacketResult<cg::packet::Packet> extraField = cg::packet::ParsePacket("Gp a");
    failures += ExpectError(
        extraField,
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gp",
        "field count extra");
    if (extraField.HasError()) {
        failures += Expect(extraField.Error().fieldIndex == 0U, "extra field index");
    }

    const cg::packet::PacketResult<cg::packet::Packet> intDecode = cg::packet::ParsePacket("CCL _");
    failures += ExpectError(intDecode, cg::packet::PacketErrorCode::DecodeFailed, "CCL", "i62 decode failed");
    if (intDecode.HasError()) {
        failures += Expect(intDecode.Error().fieldIndex == 0U, "i62 field index");
    }

    const cg::packet::PacketResult<cg::packet::Packet> emptyInt = cg::packet::ParsePacket("CCL ");
    failures += Expect(static_cast<bool>(emptyInt), "empty i62 token as zero");
    if (emptyInt.HasValue()) {
        failures += ExpectParsedField(
            emptyInt.Value(),
            0U,
            "time",
            cg::packet::PacketFieldType::Int62,
            "0",
            "empty i62 value");
    }

    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestKnownPackets();
    failures += TestEnvelopeBoundary();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
