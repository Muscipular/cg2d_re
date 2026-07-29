#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>
#include <type_traits>

namespace {

using cg::packet::send::BattleWatchStopRequest;

static_assert(std::is_empty_v<BattleWatchStopRequest>);

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
    const BattleWatchStopRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BattleWatchStopRequest::Code == "NgMy", "NgMy code");
    failures += Expect(
        BattleWatchStopRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "NgMy direction");
    failures += Expect(BattleWatchStopRequest::Dummy == 0, "NgMy fixed dummy");
    failures += Expect(BattleWatchStopRequest::Fields.size() == 1U, "NgMy fields");
    failures += Expect(BattleWatchStopRequest::Fields[0].name == "dummy", "NgMy field name");
    failures += Expect(
        BattleWatchStopRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "NgMy field type");
    failures += Expect(
        BattleWatchStopRequest::Spec.code == BattleWatchStopRequest::Code,
        "NgMy Spec code");
    failures += Expect(
        BattleWatchStopRequest::Spec.direction == BattleWatchStopRequest::Direction,
        "NgMy Spec direction");
    failures += Expect(
        BattleWatchStopRequest::Spec.fieldCount == 1U,
        "NgMy Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("NgMy"), "NgMy packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "NgMy packet direction");
    failures += ExpectDummyField(packet, "0", "NgMy packet dummy");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "NgMy Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "NgMy 0 ", "NgMy Encode fixed zero");
    }
    return failures;
}

int TestGenericCanonicalRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("NgMy 1 ", BattleWatchStopRequest::Spec);

    int failures = Expect(parsed.HasValue(), "NgMy generic accepts valid i62");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectDummyField(parsed.Value(), "1", "NgMy parsed generic dummy");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), BattleWatchStopRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "NgMy BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "NgMy 1 ", "NgMy generic canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 0 ", BattleWatchStopRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "NgMy wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("NgMy", BattleWatchStopRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NgMy",
        true,
        0U,
        "NgMy missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("NgMy 0 1 ", BattleWatchStopRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NgMy",
        true,
        1U,
        "NgMy extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("NgMy _ ", BattleWatchStopRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NgMy",
        true,
        0U,
        "NgMy invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("NgMy 2lkCB2 ", BattleWatchStopRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NgMy",
        true,
        0U,
        "NgMy i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecToPacketAndEncode() + TestGenericCanonicalRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " battle watch stop request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
