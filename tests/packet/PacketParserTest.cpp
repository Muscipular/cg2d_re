#include "packet/PacketParser.hpp"
#include "packet/Receive.hpp"
#include "packet/Send.hpp"

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
        cg::packet::ParsePacket("JFVf hash pass account ", cg::packet::send::ClientLogin::Spec);
    failures += Expect(static_cast<bool>(login), "parse JFVf");
    if (login.HasValue()) {
        const cg::packet::Packet& packet = login.Value();
        failures += Expect(packet.code == cg::packet::PacketCode("JFVf"), "JFVf code");
        failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, "JFVf direction");
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

    const cg::packet::PacketResult<cg::packet::Packet> loginSample =
        cg::packet::ParsePacket("JFVf aassdd u01 b42e99e808dc ", cg::packet::send::ClientLogin::Spec);
    failures += Expect(static_cast<bool>(loginSample), "parse JFVf packet log sample");
    if (loginSample.HasValue()) {
        failures += ExpectParsedField(
            loginSample.Value(),
            0U,
            "clientHash",
            cg::packet::PacketFieldType::EscapedString,
            "aassdd",
            "JFVf packet log clientHash");
        failures += ExpectParsedField(
            loginSample.Value(),
            1U,
            "password",
            cg::packet::PacketFieldType::EscapedString,
            "u01",
            "JFVf packet log password");
        failures += ExpectParsedField(
            loginSample.Value(),
            2U,
            "account",
            cg::packet::PacketFieldType::EscapedString,
            "b42e99e808dc",
            "JFVf packet log account");
    }

    const cg::packet::PacketResult<cg::packet::Packet> checkCommand =
        cg::packet::ParsePacket("CCL a ", cg::packet::send::CheckCommand::Spec);
    failures += Expect(static_cast<bool>(checkCommand), "parse CCL");
    if (checkCommand.HasValue()) {
        const cg::packet::Packet& packet = checkCommand.Value();
        failures += Expect(packet.code == cg::packet::PacketCode("CCL"), "CCL code");
        failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, "CCL direction");
        failures += Expect(packet.fields.size() == 1U, "CCL field count");
        failures += ExpectParsedField(
            packet,
            0U,
            "time",
            cg::packet::PacketFieldType::Int62,
            "10",
            "CCL time");
    }

    const cg::packet::PacketResult<cg::packet::Packet> checkCommandSample =
        cg::packet::ParsePacket("CCL 4lF ", cg::packet::send::CheckCommand::Spec);
    failures += Expect(static_cast<bool>(checkCommandSample), "parse CCL packet log sample");
    if (checkCommandSample.HasValue()) {
        failures += ExpectParsedField(
            checkCommandSample.Value(),
            0U,
            "time",
            cg::packet::PacketFieldType::Int62,
            "16719",
            "CCL packet log time");
    }

    const cg::packet::PacketResult<cg::packet::Packet> charList =
        cg::packet::ParsePacket("Gp", cg::packet::send::CharacterListRequest::Spec);
    failures += Expect(static_cast<bool>(charList), "parse Gp");
    if (charList.HasValue()) {
        failures += Expect(charList.Value().code == cg::packet::PacketCode("Gp"), "Gp code");
        failures += Expect(charList.Value().fields.empty(), "Gp field count");
    }

    const cg::packet::PacketResult<cg::packet::Packet> charListTrailing =
        cg::packet::ParsePacket("Gp ", cg::packet::send::CharacterListRequest::Spec);
    failures += Expect(static_cast<bool>(charListTrailing), "parse Gp trailing");
    if (charListTrailing.HasValue()) {
        failures += Expect(charListTrailing.Value().fields.empty(), "Gp trailing field count");
    }

    const cg::packet::PacketResult<cg::packet::Packet> busy =
        cg::packet::ParsePacket("whV 0 busy ", cg::packet::receive::ClientLoginResult::Spec);
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

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("", cg::packet::receive::ClientLoginResult::Spec),
        cg::packet::PacketErrorCode::EmptyInput,
        "",
        "empty input");
    failures += ExpectError(
        cg::packet::ParsePacket(" ", cg::packet::receive::ClientLoginResult::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "",
        "empty code");
    failures += ExpectError(
        cg::packet::ParsePacket("BAD", cg::packet::receive::ClientLoginResult::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "BAD",
        "wrong spec code");

    const cg::packet::PacketResult<cg::packet::Packet> missingField =
        cg::packet::ParsePacket("whV 0", cg::packet::receive::ClientLoginResult::Spec);
    failures += ExpectError(
        missingField,
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "whV",
        "field count missing");
    if (missingField.HasError()) {
        failures += Expect(missingField.Error().fieldIndex == 1U, "missing field index");
    }

    const cg::packet::PacketResult<cg::packet::Packet> extraField =
        cg::packet::ParsePacket("Gp a", cg::packet::send::CharacterListRequest::Spec);
    failures += ExpectError(
        extraField,
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gp",
        "field count extra");
    if (extraField.HasError()) {
        failures += Expect(extraField.Error().fieldIndex == 0U, "extra field index");
    }

    const cg::packet::PacketResult<cg::packet::Packet> intDecode =
        cg::packet::ParsePacket("CCL _", cg::packet::send::CheckCommand::Spec);
    failures += ExpectError(intDecode, cg::packet::PacketErrorCode::DecodeFailed, "CCL", "i62 decode failed");
    if (intDecode.HasError()) {
        failures += Expect(intDecode.Error().fieldIndex == 0U, "i62 field index");
    }

    const cg::packet::PacketResult<cg::packet::Packet> emptyInt =
        cg::packet::ParsePacket("CCL ", cg::packet::send::CheckCommand::Spec);
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
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
