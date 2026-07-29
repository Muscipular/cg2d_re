#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::ClientIndex;

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
    int failures = Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }
    failures += Expect(error.fieldIndex.has_value() == hasFieldIndex, message);
    if (hasFieldIndex && error.fieldIndex.has_value()) {
        failures += Expect(*error.fieldIndex == fieldIndex, message);
    }
    return failures;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int TestSpecAndToPacket()
{
    const ClientIndex clientIndex(1, 62, 2, -1);
    const cg::packet::Packet packet = clientIndex.ToPacket();

    int failures = 0;
    failures += Expect(ClientIndex::Code == "PudO", "PudO code");
    failures += Expect(
        ClientIndex::Direction == cg::packet::PacketDirection::ServerToClient,
        "PudO direction");
    failures += Expect(ClientIndex::Fields.size() == 4U, "PudO fields");
    const std::array<std::string_view, 4U> names = {
        "objectIndex",
        "serverTime",
        "serverNumber",
        "adjustTime",
    };
    for (std::size_t index = 0U; index < names.size(); ++index) {
        failures += Expect(ClientIndex::Fields[index].name == names[index], "PudO field name");
        failures += Expect(
            ClientIndex::Fields[index].type == cg::packet::PacketFieldType::Int62,
            "PudO field type");
    }
    failures += Expect(ClientIndex::Spec.code == ClientIndex::Code, "PudO Spec code");
    failures += Expect(ClientIndex::Spec.direction == ClientIndex::Direction, "PudO Spec direction");
    failures += Expect(ClientIndex::Spec.fieldCount == 4U, "PudO Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("PudO"), "PudO packet code");
    failures += Expect(packet.direction == ClientIndex::Direction, "PudO packet direction");
    failures += Expect(packet.fields.size() == 4U, "PudO packet fields");
    failures += ExpectField(packet, 0U, "objectIndex", "1", "PudO objectIndex field");
    failures += ExpectField(packet, 1U, "serverTime", "62", "PudO serverTime field");
    failures += ExpectField(packet, 2U, "serverNumber", "2", "PudO serverNumber field");
    failures += ExpectField(packet, 3U, "adjustTime", "-1", "PudO adjustTime field");
    return failures;
}

int TestTypedParseAndCanonical()
{
    const cg::packet::PacketResult<ClientIndex> parsed =
        ClientIndex::Parse("PudO 1 10 2 -1 ");

    int failures = Expect(parsed.HasValue(), "PudO typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().objectIndex == 1, "PudO objectIndex value");
    failures += Expect(parsed.Value().serverTime == 62, "PudO serverTime value");
    failures += Expect(parsed.Value().serverNumber == 2, "PudO serverNumber value");
    failures += Expect(parsed.Value().adjustTime == -1, "PudO negative adjustTime");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), ClientIndex::Spec);
    failures += Expect(built.HasValue(), "PudO typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "PudO 1 10 2 -1 ", "PudO typed canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("PudO 1 10 2 -1", ClientIndex::Spec);

    int failures = Expect(parsed.HasValue(), "PudO generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), ClientIndex::Spec);
    failures += Expect(built.HasValue(), "PudO generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "PudO 1 10 2 -1 ", "PudO generic canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 4U>& tokens)
{
    return "PudO " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ' + tokens[3] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ClientIndex::Parse("bad 1 10 2 -1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "PudO wrong code");
    failures += ExpectError(
        ClientIndex::Parse("PudO 1 10 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PudO",
        true,
        3U,
        "PudO missing field");
    failures += ExpectError(
        ClientIndex::Parse("PudO 1 10 2 -1 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PudO",
        true,
        4U,
        "PudO extra field");

    const std::array<std::string, 4U> valid = {"1", "10", "2", "-1"};
    for (std::size_t index = 0U; index < valid.size(); ++index) {
        std::array<std::string, 4U> invalid = valid;
        invalid[index] = "_";
        failures += ExpectError(
            ClientIndex::Parse(BuildPlain(invalid)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PudO",
            true,
            index,
            "PudO invalid i62");

        std::array<std::string, 4U> overflow = valid;
        overflow[index] = "2lkCB2";
        failures += ExpectError(
            ClientIndex::Parse(BuildPlain(overflow)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PudO",
            true,
            index,
            "PudO i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestTypedParseAndCanonical()
        + TestGenericRoundtrip()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " client index packet test(s) failed\n";
        return 1;
    }
    return 0;
}
