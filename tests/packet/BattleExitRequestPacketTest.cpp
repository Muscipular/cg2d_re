#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>
#include <type_traits>

namespace {

using cg::packet::send::BattleExitRequest;

static_assert(std::is_empty_v<BattleExitRequest>);

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

int ExpectDummyField(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }
    failures += Expect(packet.fields[0].name == "dummy", message);
    failures += Expect(
        packet.fields[0].type == cg::packet::PacketFieldType::Int62,
        message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int TestSpecToPacketAndEncode()
{
    const BattleExitRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BattleExitRequest::Code == "eL", "eL code");
    failures += Expect(
        BattleExitRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "eL direction");
    failures += Expect(BattleExitRequest::Dummy == 0, "eL fixed dummy");
    failures += Expect(BattleExitRequest::Fields.size() == 1U, "eL fields");
    failures += Expect(BattleExitRequest::Fields[0].name == "dummy", "eL field name");
    failures += Expect(
        BattleExitRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "eL field type");
    failures += Expect(BattleExitRequest::Spec.code == BattleExitRequest::Code, "eL Spec code");
    failures += Expect(
        BattleExitRequest::Spec.direction == BattleExitRequest::Direction,
        "eL Spec direction");
    failures += Expect(BattleExitRequest::Spec.fieldCount == 1U, "eL Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("eL"), "eL packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "eL packet direction");
    failures += ExpectDummyField(packet, "0", "eL packet dummy");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "eL Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "eL 0 ", "eL Encode fixed zero");
    }
    return failures;
}

int TestGenericCanonicalRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("eL 10 ", BattleExitRequest::Spec);

    int failures = Expect(parsed.HasValue(), "eL generic accepts nonzero");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectDummyField(parsed.Value(), "62", "eL parsed nonzero dummy");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), BattleExitRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "eL BuildPacket");
    if (!rebuilt.HasValue()) {
        return failures;
    }
    failures += Expect(rebuilt.Value() == "eL 10 ", "eL canonical nonzero");

    const cg::packet::PacketResult<cg::packet::Packet> reparsed =
        cg::packet::ParsePacket(rebuilt.Value(), BattleExitRequest::Spec);
    failures += Expect(reparsed.HasValue(), "eL reparse canonical");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 0 ", BattleExitRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "eL wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("eL", BattleExitRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "eL",
        true,
        0U,
        "eL missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("eL 0 1 ", BattleExitRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "eL",
        true,
        1U,
        "eL extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("eL _ ", BattleExitRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "eL",
        true,
        0U,
        "eL invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("eL 2lkCB2 ", BattleExitRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "eL",
        true,
        0U,
        "eL i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecToPacketAndEncode() + TestGenericCanonicalRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " battle exit request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
