#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::receive::BattlePosition;

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

int TestSpecAndToPacket()
{
    const BattlePosition position(1);
    const cg::packet::Packet packet = position.ToPacket();

    int failures = 0;
    failures += Expect(BattlePosition::Code == "Ov", "Ov code");
    failures += Expect(
        BattlePosition::Direction == cg::packet::PacketDirection::ServerToClient,
        "Ov direction");
    failures += Expect(BattlePosition::Fields.size() == 1U, "Ov fields");
    failures += Expect(BattlePosition::Fields[0].name == "pos", "Ov field name");
    failures += Expect(
        BattlePosition::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "Ov field type");
    failures += Expect(BattlePosition::Spec.code == BattlePosition::Code, "Ov Spec code");
    failures += Expect(BattlePosition::Spec.direction == BattlePosition::Direction, "Ov Spec direction");
    failures += Expect(BattlePosition::Spec.fieldCount == 1U, "Ov Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("Ov"), "Ov packet code");
    failures += Expect(packet.direction == BattlePosition::Direction, "Ov packet direction");
    failures += Expect(packet.fields.size() == 1U, "Ov packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "pos", "Ov packet field name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            "Ov packet field type");
        failures += Expect(packet.fields[0].value == "1", "Ov packet field value");
    }
    return failures;
}

int TestPositionsAndCanonical()
{
    const cg::packet::PacketResult<BattlePosition> front = BattlePosition::Parse("Ov 0 ");
    int failures = Expect(front.HasValue(), "Ov front parse");
    if (front.HasValue()) {
        failures += Expect(front.Value().position == 0, "Ov front value");
    }

    const cg::packet::PacketResult<BattlePosition> back = BattlePosition::Parse("Ov 1 ");
    failures += Expect(back.HasValue(), "Ov back parse");
    if (!back.HasValue()) {
        return failures;
    }
    failures += Expect(back.Value().position == 1, "Ov back value");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(back.Value().ToPacket(), BattlePosition::Spec);
    failures += Expect(built.HasValue(), "Ov typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "Ov 1 ", "Ov typed canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("Ov 1", BattlePosition::Spec);

    int failures = Expect(parsed.HasValue(), "Ov generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), BattlePosition::Spec);
    failures += Expect(built.HasValue(), "Ov generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "Ov 1 ", "Ov generic canonical");
    }
    return failures;
}

int TestDomainErrors()
{
    int failures = 0;
    failures += ExpectError(
        BattlePosition::Parse("Ov -1 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ov",
        true,
        0U,
        "Ov negative position");
    failures += ExpectError(
        BattlePosition::Parse("Ov 2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ov",
        true,
        0U,
        "Ov position two");
    failures += ExpectError(
        BattlePosition::Parse("Ov 10000 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ov",
        true,
        0U,
        "Ov large position");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        BattlePosition::Parse("bad 1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Ov wrong code");
    failures += ExpectError(
        BattlePosition::Parse("Ov"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ov",
        true,
        0U,
        "Ov missing field");
    failures += ExpectError(
        BattlePosition::Parse("Ov 1 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ov",
        true,
        1U,
        "Ov extra field");
    failures += ExpectError(
        BattlePosition::Parse("Ov _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ov",
        true,
        0U,
        "Ov invalid i62");
    failures += ExpectError(
        BattlePosition::Parse("Ov 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ov",
        true,
        0U,
        "Ov i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestPositionsAndCanonical()
        + TestGenericRoundtrip()
        + TestDomainErrors()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " battle position packet test(s) failed\n";
        return 1;
    }
    return 0;
}
