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
        cg::packet::ParsePacket(
            "JFVf hash account password 001122334455 ",
            cg::packet::send::ClientLogin::Spec);
    failures += Expect(static_cast<bool>(login), "parse JFVf");
    if (login.HasValue()) {
        const cg::packet::Packet& packet = login.Value();
        failures += Expect(packet.code == cg::packet::PacketCode("JFVf"), "JFVf code");
        failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, "JFVf direction");
        failures += Expect(packet.fields.size() == 4U, "JFVf field count");
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
            "account",
            cg::packet::PacketFieldType::EscapedString,
            "account",
            "JFVf account");
        failures += ExpectParsedField(
            packet,
            2U,
            "password",
            cg::packet::PacketFieldType::EscapedString,
            "password",
            "JFVf password");
        failures += ExpectParsedField(
            packet,
            3U,
            "macAddress",
            cg::packet::PacketFieldType::EscapedString,
            "001122334455",
            "JFVf macAddress");
    }

    const cg::packet::PacketResult<cg::packet::Packet> loginSample =
        cg::packet::ParsePacket(
            "JFVf aassdd u01 b42e99e808dc 001122334455 ",
            cg::packet::send::ClientLogin::Spec);
    failures += Expect(static_cast<bool>(loginSample), "parse JFVf cg2d-order sample");
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
            "account",
            cg::packet::PacketFieldType::EscapedString,
            "u01",
            "JFVf cg2d-order account");
        failures += ExpectParsedField(
            loginSample.Value(),
            2U,
            "password",
            cg::packet::PacketFieldType::EscapedString,
            "b42e99e808dc",
            "JFVf cg2d-order password");
        failures += ExpectParsedField(
            loginSample.Value(),
            3U,
            "macAddress",
            cg::packet::PacketFieldType::EscapedString,
            "001122334455",
            "JFVf cg2d-order macAddress");
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

int TestRepeatedTail()
{
    int failures = 0;

    const cg::packet::PacketResult<cg::packet::Packet> zero =
        cg::packet::ParsePacket("kza 0", cg::packet::send::AlbumInfoRequest::Spec);
    failures += Expect(static_cast<bool>(zero), "parse kza zero");
    if (zero.HasValue()) {
        failures += Expect(zero.Value().fields.size() == 1U, "kza zero field count");
        failures += ExpectParsedField(
            zero.Value(),
            0U,
            "count",
            cg::packet::PacketFieldType::Int62,
            "0",
            "kza zero count");
    }

    const cg::packet::PacketResult<cg::packet::Packet> zeroTrailing =
        cg::packet::ParsePacket("kza 0 ", cg::packet::send::AlbumInfoRequest::Spec);
    failures += Expect(static_cast<bool>(zeroTrailing), "parse kza zero trailing");

    const cg::packet::PacketResult<cg::packet::Packet> emptyCount =
        cg::packet::ParsePacket("kza ", cg::packet::send::AlbumInfoRequest::Spec);
    failures += Expect(static_cast<bool>(emptyCount), "parse kza empty count as zero");
    if (emptyCount.HasValue()) {
        failures += ExpectParsedField(
            emptyCount.Value(),
            0U,
            "count",
            cg::packet::PacketFieldType::Int62,
            "0",
            "kza empty count value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> multiple =
        cg::packet::ParsePacket("kza 2 a k ", cg::packet::send::AlbumInfoRequest::Spec);
    failures += Expect(static_cast<bool>(multiple), "parse kza repeated tail");
    if (multiple.HasValue()) {
        failures += Expect(multiple.Value().fields.size() == 3U, "kza repeated field count");
        failures += ExpectParsedField(
            multiple.Value(),
            0U,
            "count",
            cg::packet::PacketFieldType::Int62,
            "2",
            "kza repeated count");
        failures += ExpectParsedField(
            multiple.Value(),
            1U,
            "albumid",
            cg::packet::PacketFieldType::Int62,
            "10",
            "kza first albumid");
        failures += ExpectParsedField(
            multiple.Value(),
            2U,
            "albumid",
            cg::packet::PacketFieldType::Int62,
            "20",
            "kza second albumid");
    }

    const cg::packet::PacketResult<cg::packet::Packet> single =
        cg::packet::ParsePacket("kza 1 a ", cg::packet::send::AlbumInfoRequest::Spec);
    failures += Expect(static_cast<bool>(single), "parse kza single repeated tail");
    if (single.HasValue()) {
        failures += Expect(single.Value().fields.size() == 2U, "kza single field count");
        failures += ExpectParsedField(
            single.Value(),
            1U,
            "albumid",
            cg::packet::PacketFieldType::Int62,
            "10",
            "kza single albumid");
    }

    const auto expectFieldError = [&](std::string_view text,
                                      cg::packet::PacketErrorCode code,
                                      std::size_t fieldIndex,
                                      const char* message) {
        const cg::packet::PacketResult<cg::packet::Packet> result =
            cg::packet::ParsePacket(text, cg::packet::send::AlbumInfoRequest::Spec);
        int localFailures = ExpectError(result, code, "kza", message);
        if (result.HasError()) {
            localFailures += Expect(
                result.Error().fieldIndex == fieldIndex,
                message);
        }
        return localFailures;
    };

    failures += expectFieldError(
        "kza 2 a",
        cg::packet::PacketErrorCode::FieldCountMismatch,
        2U,
        "kza repeated tail missing");
    failures += expectFieldError(
        "kza 2 a ",
        cg::packet::PacketErrorCode::FieldCountMismatch,
        2U,
        "kza repeated tail missing with delimiter");
    failures += expectFieldError(
        "kza 1 a k",
        cg::packet::PacketErrorCode::FieldCountMismatch,
        2U,
        "kza repeated tail extra");
    failures += expectFieldError(
        "kza -1",
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "kza negative count");
    failures += expectFieldError(
        "kza _",
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "kza invalid count");
    failures += expectFieldError(
        "kza 2lkCB2",
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "kza overflow count");
    failures += expectFieldError(
        "kza 1 _",
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "kza invalid albumid");
    failures += expectFieldError(
        "kza 1 2lkCB2",
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "kza overflow albumid");

    constexpr cg::packet::PacketSpec invalidSpec {
        "bad",
        cg::packet::PacketDirection::ClientToServer,
        nullptr,
        0U,
        {true, 0U},
    };
    const cg::packet::PacketResult<cg::packet::Packet> invalidShape =
        cg::packet::ParsePacket("bad", invalidSpec);
    failures += ExpectError(
        invalidShape,
        cg::packet::PacketErrorCode::InvalidFieldType,
        "bad",
        "invalid repeated tail spec");
    if (invalidShape.HasError()) {
        failures += Expect(invalidShape.Error().fieldIndex == 0U, "invalid spec field index");
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

    const cg::packet::PacketResult<cg::packet::Packet> clientLoginMissingMac =
        cg::packet::ParsePacket(
            "JFVf hash account password",
            cg::packet::send::ClientLogin::Spec);
    failures += ExpectError(
        clientLoginMissingMac,
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JFVf",
        "JFVf missing macAddress");
    if (clientLoginMissingMac.HasError()) {
        failures += Expect(
            clientLoginMissingMac.Error().fieldIndex == 3U,
            "JFVf missing macAddress field index");
    }

    const cg::packet::PacketResult<cg::packet::Packet> clientLoginExtraField =
        cg::packet::ParsePacket(
            "JFVf hash account password 001122334455 extra ",
            cg::packet::send::ClientLogin::Spec);
    failures += ExpectError(
        clientLoginExtraField,
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JFVf",
        "JFVf extra field");
    if (clientLoginExtraField.HasError()) {
        failures += Expect(
            clientLoginExtraField.Error().fieldIndex == 4U,
            "JFVf extra field index");
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
    failures += TestRepeatedTail();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
