#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::send::ItemFrontCharListRequest;

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
    const ItemFrontCharListRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ItemFrontCharListRequest::Code == "as", "as code");
    failures += Expect(
        ItemFrontCharListRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "as direction");
    failures += Expect(ItemFrontCharListRequest::Fields.empty(), "as fields");
    failures += Expect(
        ItemFrontCharListRequest::Spec.code == ItemFrontCharListRequest::Code,
        "as Spec code");
    failures += Expect(
        ItemFrontCharListRequest::Spec.direction == ItemFrontCharListRequest::Direction,
        "as Spec direction");
    failures += Expect(ItemFrontCharListRequest::Spec.fieldCount == 0U, "as Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("as"), "as packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "as packet direction");
    failures += Expect(packet.fields.empty(), "as packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "as Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "as ", "as Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("as", ItemFrontCharListRequest::Spec);

    int failures = Expect(parsed.HasValue(), "as ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.empty(), "as parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), ItemFrontCharListRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "as BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "as ", "as canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", ItemFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "as wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("as 1 ", ItemFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "as",
        true,
        0U,
        "as extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " item front character list request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
