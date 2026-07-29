#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::send::TradePlayerListRequest;

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
    const TradePlayerListRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(TradePlayerListRequest::Code == "ixH", "ixH code");
    failures += Expect(
        TradePlayerListRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "ixH direction");
    failures += Expect(TradePlayerListRequest::Fields.empty(), "ixH fields");
    failures += Expect(
        TradePlayerListRequest::Spec.code == TradePlayerListRequest::Code,
        "ixH Spec code");
    failures += Expect(
        TradePlayerListRequest::Spec.direction == TradePlayerListRequest::Direction,
        "ixH Spec direction");
    failures += Expect(TradePlayerListRequest::Spec.fieldCount == 0U, "ixH Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("ixH"), "ixH packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "ixH packet direction");
    failures += Expect(packet.fields.empty(), "ixH packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "ixH Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "ixH ", "ixH Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("ixH", TradePlayerListRequest::Spec);

    int failures = Expect(parsed.HasValue(), "ixH ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.empty(), "ixH parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), TradePlayerListRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "ixH BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "ixH ", "ixH canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", TradePlayerListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ixH wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("ixH 1 ", TradePlayerListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ixH",
        true,
        0U,
        "ixH extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " trade player list request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
