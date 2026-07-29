#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::send::KeepAliveRequest;

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

int TestSpecToPacketAndEncode()
{
    const KeepAliveRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(KeepAliveRequest::Code == "DE", "DE code");
    failures += Expect(
        KeepAliveRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "DE direction");
    failures += Expect(KeepAliveRequest::Fields.empty(), "DE fields");
    failures += Expect(KeepAliveRequest::Spec.code == KeepAliveRequest::Code, "DE Spec code");
    failures += Expect(
        KeepAliveRequest::Spec.direction == KeepAliveRequest::Direction,
        "DE Spec direction");
    failures += Expect(KeepAliveRequest::Spec.fieldCount == 0U, "DE Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("DE"), "DE packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "DE packet direction");
    failures += Expect(packet.fields.empty(), "DE packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "DE Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "DE ", "DE Encode value");
    }
    return failures;
}

int TestGenericCanonicalRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> plain =
        cg::packet::ParsePacket("DE", KeepAliveRequest::Spec);

    int failures = Expect(plain.HasValue(), "DE plain ParsePacket");
    if (!plain.HasValue()) {
        return failures;
    }
    failures += Expect(plain.Value().fields.empty(), "DE parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(plain.Value(), KeepAliveRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "DE BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "DE ", "DE canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> canonical =
        cg::packet::ParsePacket("DE ", KeepAliveRequest::Spec);
    failures += Expect(canonical.HasValue(), "DE canonical ParsePacket");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", KeepAliveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "DE wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("DE 1 ", KeepAliveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "DE",
        true,
        0U,
        "DE extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecToPacketAndEncode() + TestGenericCanonicalRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " keep alive request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
