#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::send::CharacterLogoutRequest;

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
    const CharacterLogoutRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(CharacterLogoutRequest::Code == "WA", "WA code");
    failures += Expect(
        CharacterLogoutRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "WA direction");
    failures += Expect(CharacterLogoutRequest::Fields.empty(), "WA fields");
    failures += Expect(CharacterLogoutRequest::Spec.code == CharacterLogoutRequest::Code, "WA Spec code");
    failures += Expect(
        CharacterLogoutRequest::Spec.direction == CharacterLogoutRequest::Direction,
        "WA Spec direction");
    failures += Expect(CharacterLogoutRequest::Spec.fieldCount == 0U, "WA Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("WA"), "WA packet code");
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, "WA packet direction");
    failures += Expect(packet.fields.empty(), "WA packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "WA Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "WA ", "WA Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("WA", CharacterLogoutRequest::Spec);

    int failures = Expect(parsed.HasValue(), "WA ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.empty(), "WA parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), CharacterLogoutRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "WA BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "WA ", "WA canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", CharacterLogoutRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "WA wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("WA 1 ", CharacterLogoutRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "WA",
        true,
        0U,
        "WA extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " character logout request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
