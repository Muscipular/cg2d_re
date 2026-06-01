#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
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

int ExpectJobNamePacket(
    const cg::packet::Packet& packet,
    std::string_view jobAssort,
    std::string_view name,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::JobName::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "jobassort", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields[0].value == jobAssort, message);
    failures += Expect(packet.fields[1].name == "name", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields[1].value == name, message);
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("jv 1 Soldier ");

    int failures = Expect(static_cast<bool>(parsed), "parse jv");
    if (parsed.HasValue()) {
        failures += ExpectJobNamePacket(parsed.Value(), "1", "Soldier", "parse jv packet");
    }
    return failures;
}

int TestInt62Parse()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("jv a Soldier ");

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

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
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
    const cg::packet::Packet packet = parsed.Value().ToPacket();
    failures += ExpectJobNamePacket(packet, "1", "Soldier", "typed jv ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build typed jv");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "jv 1 Soldier ", "build typed jv value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("jv 1"),
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
        cg::packet::ParsePacket("jv 1 Soldier extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "jv",
        true,
        2U,
        "jv extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("jv _ Soldier "),
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
    failures += TestParseErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
