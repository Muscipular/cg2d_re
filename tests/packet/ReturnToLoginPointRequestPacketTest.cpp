#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::send::ReturnToLoginPointRequest;

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
    const ReturnToLoginPointRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ReturnToLoginPointRequest::Code == "lO", "lO code");
    failures += Expect(
        ReturnToLoginPointRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "lO direction");
    failures += Expect(ReturnToLoginPointRequest::Fields.empty(), "lO fields");
    failures += Expect(
        ReturnToLoginPointRequest::Spec.code == ReturnToLoginPointRequest::Code,
        "lO Spec code");
    failures += Expect(
        ReturnToLoginPointRequest::Spec.direction == ReturnToLoginPointRequest::Direction,
        "lO Spec direction");
    failures += Expect(ReturnToLoginPointRequest::Spec.fieldCount == 0U, "lO Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("lO"), "lO packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "lO packet direction");
    failures += Expect(packet.fields.empty(), "lO packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "lO Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "lO ", "lO Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("lO", ReturnToLoginPointRequest::Spec);

    int failures = Expect(parsed.HasValue(), "lO ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.empty(), "lO parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), ReturnToLoginPointRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "lO BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "lO ", "lO canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", ReturnToLoginPointRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "lO wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("lO 1 ", ReturnToLoginPointRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lO",
        true,
        0U,
        "lO extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " return to login point request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
