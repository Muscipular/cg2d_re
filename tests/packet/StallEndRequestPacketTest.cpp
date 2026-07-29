#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::send::StallEndRequest;

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
    const StallEndRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(StallEndRequest::Code == "JMh", "JMh code");
    failures += Expect(
        StallEndRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "JMh direction");
    failures += Expect(StallEndRequest::Fields.empty(), "JMh fields");
    failures += Expect(StallEndRequest::Spec.code == StallEndRequest::Code, "JMh Spec code");
    failures += Expect(
        StallEndRequest::Spec.direction == StallEndRequest::Direction,
        "JMh Spec direction");
    failures += Expect(StallEndRequest::Spec.fieldCount == 0U, "JMh Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("JMh"), "JMh packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "JMh packet direction");
    failures += Expect(packet.fields.empty(), "JMh packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "JMh Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "JMh ", "JMh Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("JMh", StallEndRequest::Spec);

    int failures = Expect(parsed.HasValue(), "JMh ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.empty(), "JMh parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), StallEndRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "JMh BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "JMh ", "JMh canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", StallEndRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "JMh wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("JMh 1 ", StallEndRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JMh",
        true,
        0U,
        "JMh extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " stall end request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
