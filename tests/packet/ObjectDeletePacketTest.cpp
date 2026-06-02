#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ObjectDelete::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ObjectDelete::Spec);
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

std::string BuildPlainUsb(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "uSB ";
    }

    std::string plain = "uSB ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectObjectDeletePacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ObjectDelete::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectIndexes(
    const cg::packet::receive::ObjectDelete& objectDelete,
    const std::vector<int>& objectIndexes,
    const char* message)
{
    int failures = 0;
    failures += Expect(objectDelete.objectIndexes.size() == objectIndexes.size(), message);
    if (objectDelete.objectIndexes.size() != objectIndexes.size()) {
        return failures;
    }

    for (std::size_t index = 0; index < objectIndexes.size(); ++index) {
        failures += Expect(objectDelete.objectIndexes[index] == objectIndexes[index], message);
    }
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainUsb("1,2,a"));

    int failures = Expect(static_cast<bool>(parsed), "parse uSB outer");
    if (parsed.HasValue()) {
        failures += ExpectObjectDeletePacket(parsed.Value(), "1,2,a", "parse uSB outer packet");
    }
    return failures;
}

int TestTypedSingleIndex()
{
    const cg::packet::PacketResult<cg::packet::receive::ObjectDelete> parsed =
        cg::packet::receive::ObjectDelete::Parse(BuildPlainUsb("1"));

    int failures = Expect(static_cast<bool>(parsed), "parse uSB single index");
    if (parsed.HasValue()) {
        failures += ExpectIndexes(parsed.Value(), {1}, "parse uSB single value");
    }
    return failures;
}

int TestTypedMultiIndex()
{
    const cg::packet::PacketResult<cg::packet::receive::ObjectDelete> parsed =
        cg::packet::receive::ObjectDelete::Parse(BuildPlainUsb("1,2,a"));

    int failures = Expect(static_cast<bool>(parsed), "parse uSB multi index");
    if (parsed.HasValue()) {
        failures += ExpectIndexes(parsed.Value(), {1, 2, 10}, "parse uSB multi values");
    }
    return failures;
}

int TestBuildNoOuterObjectIndex()
{
    const cg::packet::receive::ObjectDelete objectDelete({1, 2, 10});
    const cg::packet::Packet packet = objectDelete.ToPacket();

    int failures = ExpectObjectDeletePacket(packet, "1,2,a", "uSB ToPacket data only");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build uSB packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "uSB 1,2,a ", "build uSB no outer objectIndex");
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse built uSB packet");
    if (parsed.HasValue()) {
        failures += ExpectObjectDeletePacket(parsed.Value(), "1,2,a", "built uSB packet data only");
    }
    return failures;
}

int TestEmptyList()
{
    int failures = 0;
    const cg::packet::PacketResult<cg::packet::receive::ObjectDelete> parsed =
        cg::packet::receive::ObjectDelete::Parse(BuildPlainUsb(""));
    failures += Expect(static_cast<bool>(parsed), "parse uSB empty list");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().objectIndexes.empty(), "parse uSB empty values");
    }

    const cg::packet::receive::ObjectDelete objectDelete({});
    const cg::packet::Packet packet = objectDelete.ToPacket();
    failures += ExpectObjectDeletePacket(packet, "", "uSB empty ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build uSB empty packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "uSB  ", "build uSB empty data");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("uSB"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uSB",
        "uSB missing data");
    failures += ExpectError(
        cg::packet::receive::ObjectDelete::Parse("uSB"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uSB",
        "typed uSB missing data");
    failures += ExpectError(
        cg::packet::receive::ObjectDelete::Parse(BuildPlainUsb("_")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uSB",
        "typed uSB bad object index");
    failures += ExpectError(
        cg::packet::receive::ObjectDelete::Parse(BuildPlainUsb("1,,2")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uSB",
        "typed uSB empty middle item");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedSingleIndex();
    failures += TestTypedMultiIndex();
    failures += TestBuildNoOuterObjectIndex();
    failures += TestEmptyList();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
