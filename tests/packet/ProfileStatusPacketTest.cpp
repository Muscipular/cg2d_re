#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ProfileStatus::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ProfileStatus::Spec);
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

std::string BuildPlainYo(std::string_view data)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(data);
    if (!encoded.HasValue()) {
        return "Yo  ";
    }

    std::string plain = "Yo ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectProfileStatusPacket(
    const cg::packet::Packet& packet,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileStatus::Code), message);
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

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket("Yo hello ");

    int failures = Expect(static_cast<bool>(parsed), "parse Yo outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileStatusPacket(parsed.Value(), "hello", "parse Yo outer packet");
    }
    return failures;
}

int TestRoundtripPreservesRawData()
{
    constexpr std::string_view data = "1|2|raw";
    const cg::packet::receive::ProfileStatus profileStatus {std::string(data)};
    const cg::packet::Packet packet = profileStatus.ToPacket();

    int failures = ExpectProfileStatusPacket(packet, data, "Yo ToPacket raw data");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build Yo packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainYo(data), "build Yo preserves raw data");

    const cg::packet::PacketResult<cg::packet::receive::ProfileStatus> parsed =
        cg::packet::receive::ProfileStatus::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip Yo packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data == data, "roundtrip Yo raw data");
    }
    return failures;
}

int TestMissingDataError()
{
    return ExpectError(
        cg::packet::receive::ProfileStatus::Parse("Yo"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yo",
        "Yo missing data");
}

int TestEscapedSpaceRoundtrip()
{
    constexpr std::string_view data = "hello world";
    const cg::packet::receive::ProfileStatus profileStatus {std::string(data)};
    const cg::packet::PacketResult<std::string> built = BuildPacket(profileStatus.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build Yo escaped space");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "Yo hello\\Sworld ", "build Yo escaped space token");

    const cg::packet::PacketResult<cg::packet::receive::ProfileStatus> parsed =
        cg::packet::receive::ProfileStatus::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip Yo escaped space");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data == data, "roundtrip Yo escaped space data");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestRoundtripPreservesRawData();
    failures += TestMissingDataError();
    failures += TestEscapedSpaceRoundtrip();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
