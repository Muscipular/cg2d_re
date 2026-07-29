#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::DuelRequest;

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

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "ik " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestSpecAndEncode()
{
    const DuelRequest request(62, 124);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(DuelRequest::Code == "ik", "ik code");
    failures += Expect(
        DuelRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "ik direction");
    failures += Expect(DuelRequest::Fields.size() == 2U, "ik fields");
    failures += Expect(DuelRequest::Fields[0].name == "x", "ik x name");
    failures += Expect(DuelRequest::Fields[1].name == "y", "ik y name");
    failures += Expect(
        DuelRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "ik x type");
    failures += Expect(
        DuelRequest::Fields[1].type == cg::packet::PacketFieldType::Int62,
        "ik y type");
    failures += Expect(DuelRequest::Spec.code == DuelRequest::Code, "ik Spec code");
    failures += Expect(
        DuelRequest::Spec.direction == DuelRequest::Direction,
        "ik Spec direction");
    failures += Expect(DuelRequest::Spec.fieldCount == 2U, "ik Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("ik"), "ik packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "ik packet direction");
    failures += Expect(packet.fields.size() == 2U, "ik packet fields");
    failures += ExpectField(packet, 0U, "x", "62", "ik x field");
    failures += ExpectField(packet, 1U, "y", "124", "ik y field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "ik Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "ik 10 20 ", "ik Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("ik 10 20 ", DuelRequest::Spec);

    int failures = Expect(parsed.HasValue(), "ik ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "62", "ik parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "124", "ik parsed y");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), DuelRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "ik BuildPacket");
    if (!rebuilt.HasValue()) {
        return failures;
    }
    failures += Expect(rebuilt.Value() == "ik 10 20 ", "ik canonical");

    const cg::packet::PacketResult<cg::packet::Packet> reparsed =
        cg::packet::ParsePacket(rebuilt.Value(), DuelRequest::Spec);
    failures += Expect(reparsed.HasValue(), "ik reparse canonical");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 10 20 ", DuelRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ik wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("ik 10", DuelRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ik",
        true,
        1U,
        "ik missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("ik 10 20 30 ", DuelRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ik",
        true,
        2U,
        "ik extra field");

    const std::array<std::string, 2U> validTokens = {"10", "20"};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), DuelRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ik",
            true,
            index,
            "ik invalid i62");

        std::array<std::string, 2U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), DuelRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ik",
            true,
            index,
            "ik i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " duel request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
