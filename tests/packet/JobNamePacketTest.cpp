#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::JobName::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::JobName::Spec);
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
    bool hasFieldIndex,
    std::size_t fieldIndex,
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

    if (hasFieldIndex) {
        failures += Expect(error.fieldIndex.has_value(), message);
        if (error.fieldIndex.has_value()) {
            failures += Expect(*error.fieldIndex == fieldIndex, message);
        }
    } else {
        failures += Expect(!error.fieldIndex.has_value(), message);
    }
    return failures;
}

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string output;
    output.reserve(values.size());
    for (const unsigned int value : values) {
        output.push_back(static_cast<char>(value));
    }
    return output;
}

std::string SemanticName()
{
    std::string name = Bytes({0xC3, 0xFB, 0x96, 0x7C});
    name.append("A|B,C\\D\nE");
    return name;
}

std::string LegacyName()
{
    std::string name = Bytes({0xC3, 0xFB, 0x96, 0x7C});
    name.append("A\\zB\\cC\\yD\\nE");
    return name;
}

std::string BuildPlainJv(std::string_view legacyName)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(legacyName);
    if (!encoded.HasValue()) {
        return "jv 1  ";
    }

    std::string plain = "jv 1 ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectJobNamePacket(
    const cg::packet::Packet& packet,
    std::string_view jobAssort,
    std::string_view name,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::JobName::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "jobassort", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == jobAssort, message);
    failures += Expect(packet.fields[1].name == "name", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == name, message);
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("jv 1 Soldier ");

    int failures = Expect(static_cast<bool>(parsed), "parse jv");
    if (parsed.HasValue()) {
        failures += ExpectJobNamePacket(parsed.Value(), "1", "Soldier", "parse jv packet");
    }
    return failures;
}

int TestInt62Parse()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("jv a Soldier ");

    int failures = Expect(static_cast<bool>(parsed), "parse jv i62");
    if (parsed.HasValue()) {
        failures += ExpectJobNamePacket(parsed.Value(), "10", "Soldier", "parse jv i62 packet");
    }

    const cg::packet::PacketResult<cg::packet::receive::JobName> typed =
        cg::packet::receive::JobName::Parse("jv a Soldier ");
    failures += Expect(static_cast<bool>(typed), "typed parse jv i62");
    if (typed.HasValue()) {
        failures += Expect(typed.Value().jobAssort == 10, "typed jv i62 jobAssort");
        failures += Expect(typed.Value().name == "Soldier", "typed jv i62 name");
    }
    return failures;
}

int TestEscapedNameRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::JobName> parsed =
        cg::packet::receive::JobName::Parse("jv a Job\\SName ");

    int failures = Expect(static_cast<bool>(parsed), "typed parse jv escaped name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().jobAssort == 10, "typed jv escaped jobAssort");
    failures += Expect(parsed.Value().name == "Job Name", "typed jv escaped name");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(packet.HasValue(), "jv escaped ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build jv escaped name");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "jv a Job\\SName ", "roundtrip jv escaped name");
    }
    return failures;
}

int TestTypedParseAndBuild()
{
    const cg::packet::PacketResult<cg::packet::receive::JobName> parsed =
        cg::packet::receive::JobName::Parse("jv 1 Soldier ");

    int failures = Expect(static_cast<bool>(parsed), "typed parse jv");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().jobAssort == 1, "typed jv jobAssort");
    failures += Expect(parsed.Value().name == "Soldier", "typed jv name");
    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(packet.HasValue(), "typed jv ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectJobNamePacket(packet.Value(), "1", "Soldier", "typed jv ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build typed jv");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "jv 1 Soldier ", "build typed jv value");
    }
    return failures;
}

int TestLegacyNameRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::JobName> parsed =
        cg::packet::receive::JobName::Parse(BuildPlainJv(LegacyName()));

    int failures = Expect(parsed.HasValue(), "typed parse jv legacy name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().jobAssort == 1, "typed jv legacy jobAssort");
    failures += Expect(parsed.Value().name == SemanticName(), "typed jv legacy semantic name");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(packet.HasValue(), "jv legacy ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectJobNamePacket(packet.Value(), "1", LegacyName(), "jv legacy ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(built.HasValue(), "build jv legacy name");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainJv(LegacyName()), "jv legacy canonical plain");
    }
    return failures;
}

int TestEmptyName()
{
    const cg::packet::PacketResult<cg::packet::receive::JobName> parsed =
        cg::packet::receive::JobName::Parse("jv 1  ");

    int failures = Expect(parsed.HasValue(), "typed parse jv empty name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().name.empty(), "typed jv empty semantic name");
    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(packet.HasValue(), "jv empty ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(built.HasValue(), "build jv empty name");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "jv 1  ", "jv empty canonical plain");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("jv 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "jv",
        true,
        1U,
        "jv missing name");
    failures += ExpectError(
        cg::packet::receive::JobName::Parse("jv 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "jv",
        true,
        1U,
        "typed jv missing name");
    failures += ExpectError(
        ParsePacket("jv 1 Soldier extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "jv",
        true,
        2U,
        "jv extra field");
    failures += ExpectError(
        ParsePacket("jv _ Soldier "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "jv",
        true,
        0U,
        "jv bad i62");
    failures += ExpectError(
        cg::packet::receive::JobName::Parse("jv _ Soldier "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "jv",
        true,
        0U,
        "typed jv bad i62");
    failures += ExpectError(
        cg::packet::receive::JobName::Parse(BuildPlainJv("name\\")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "jv",
        true,
        1U,
        "typed jv truncated legacy escape");

    const cg::packet::receive::JobName invalidName(1, Bytes({0x96}));
    failures += ExpectError(
        invalidName.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "jv",
        true,
        1U,
        "typed jv truncated GBK name");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacket();
    failures += TestInt62Parse();
    failures += TestEscapedNameRoundtrip();
    failures += TestTypedParseAndBuild();
    failures += TestLegacyNameRoundtrip();
    failures += TestEmptyName();
    failures += TestParseErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
