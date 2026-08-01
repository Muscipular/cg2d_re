#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
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

template <typename T>
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, packetCode, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
    }
    return failures;
}

template <typename T>
int ExpectErrorWithoutField(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    const char* message)
{
    int failures = ExpectError(result, code, packetCode, message);
    if (result.HasError()) {
        failures += Expect(!result.Error().fieldIndex.has_value(), message);
    }
    return failures;
}

std::string BuildOpaqueData()
{
    std::string data;
    data.push_back(static_cast<char>(0xC3));
    data.push_back(static_cast<char>(0xFB));
    data.append(" A|/\\zC\n");
    data.push_back(static_cast<char>(0x81));
    data.push_back(static_cast<char>(0x5C));
    data.push_back('D');
    return data;
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
    const std::string data = BuildOpaqueData();
    const cg::packet::receive::ProfileStatus profileStatus {data};
    const cg::packet::Packet packet = profileStatus.ToPacket();

    int failures = ExpectProfileStatusPacket(packet, data, "Yo ToPacket opaque data");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build Yo packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainYo(data), "build Yo preserves opaque outer es data");

    const cg::packet::PacketResult<cg::packet::receive::ProfileStatus> parsed =
        cg::packet::receive::ProfileStatus::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip Yo packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data == data, "roundtrip Yo opaque data");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::receive::ProfileStatus profileStatus {std::string()};
    const cg::packet::Packet packet = profileStatus.ToPacket();

    int failures = ExpectProfileStatusPacket(packet, "", "Yo ToPacket empty data");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build Yo empty data");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "Yo  ", "build Yo empty data canonical");
    const cg::packet::PacketResult<cg::packet::receive::ProfileStatus> parsed =
        cg::packet::receive::ProfileStatus::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse Yo empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data.empty(), "roundtrip Yo empty data");
    }
    return failures;
}

int TestMissingDataError()
{
    return ExpectFieldError(
        cg::packet::receive::ProfileStatus::Parse("Yo"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yo",
        0U,
        "Yo missing data");
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectErrorWithoutField(
        cg::packet::receive::ProfileStatus::Parse("OBee data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "OBee",
        "Yo wrong code without field index");
    failures += ExpectFieldError(
        cg::packet::receive::ProfileStatus::Parse("Yo data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yo",
        1U,
        "Yo extra outer field index");

    std::string truncatedPlain = "Yo ";
    truncatedPlain.push_back(static_cast<char>(0x81));
    truncatedPlain.push_back(' ');
    failures += ExpectFieldError(
        cg::packet::receive::ProfileStatus::Parse(truncatedPlain),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yo",
        0U,
        "Yo outer GBK truncated field index");

    cg::packet::receive::ProfileStatus truncatedValue {std::string(1U, static_cast<char>(0x81))};
    failures += ExpectFieldError(
        BuildPacket(truncatedValue.ToPacket()),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Yo",
        0U,
        "Yo BuildPacket GBK truncated field index");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestRoundtripPreservesRawData();
    failures += TestEmptyData();
    failures += TestMissingDataError();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
