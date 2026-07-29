#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::send::BattlePositionToggleRequest;

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

int TestSpecAndEncode()
{
    const BattlePositionToggleRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BattlePositionToggleRequest::Code == "ih", "ih code");
    failures += Expect(
        BattlePositionToggleRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "ih direction");
    failures += Expect(BattlePositionToggleRequest::Fields.empty(), "ih fields");
    failures += Expect(
        BattlePositionToggleRequest::Spec.code == BattlePositionToggleRequest::Code,
        "ih Spec code");
    failures += Expect(
        BattlePositionToggleRequest::Spec.direction == BattlePositionToggleRequest::Direction,
        "ih Spec direction");
    failures += Expect(BattlePositionToggleRequest::Spec.fieldCount == 0U, "ih Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("ih"), "ih packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "ih packet direction");
    failures += Expect(packet.fields.empty(), "ih packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "ih Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "ih ", "ih Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("ih", BattlePositionToggleRequest::Spec);

    int failures = Expect(parsed.HasValue(), "ih ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.empty(), "ih parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), BattlePositionToggleRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "ih BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "ih ", "ih canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", BattlePositionToggleRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ih wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("ih 1 ", BattlePositionToggleRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ih",
        true,
        0U,
        "ih extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " battle position toggle request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
