#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::send::TradeSelectionCancelRequest;

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
    const TradeSelectionCancelRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(TradeSelectionCancelRequest::Code == "suI", "suI code");
    failures += Expect(
        TradeSelectionCancelRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "suI direction");
    failures += Expect(TradeSelectionCancelRequest::Fields.empty(), "suI fields");
    failures += Expect(
        TradeSelectionCancelRequest::Spec.code == TradeSelectionCancelRequest::Code,
        "suI Spec code");
    failures += Expect(
        TradeSelectionCancelRequest::Spec.direction == TradeSelectionCancelRequest::Direction,
        "suI Spec direction");
    failures += Expect(TradeSelectionCancelRequest::Spec.fieldCount == 0U, "suI Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("suI"), "suI packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "suI packet direction");
    failures += Expect(packet.fields.empty(), "suI packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "suI Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "suI ", "suI Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("suI", TradeSelectionCancelRequest::Spec);

    int failures = Expect(parsed.HasValue(), "suI ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.empty(), "suI parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), TradeSelectionCancelRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "suI BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "suI ", "suI canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", TradeSelectionCancelRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "suI wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("suI 1 ", TradeSelectionCancelRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "suI",
        true,
        0U,
        "suI extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " trade selection cancel request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
