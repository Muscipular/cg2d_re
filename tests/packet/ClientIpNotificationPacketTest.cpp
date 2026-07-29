#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::ClientIpNotification;

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
    const ClientIpNotification notification("127.0.0.1");
    const cg::packet::Packet packet = notification.ToPacket();

    int failures = 0;
    failures += Expect(ClientIpNotification::Code == "lSZ", "lSZ code");
    failures += Expect(
        ClientIpNotification::Direction == cg::packet::PacketDirection::ServerToClient,
        "lSZ direction");
    failures += Expect(ClientIpNotification::Fields.size() == 1U, "lSZ fields");
    failures += Expect(ClientIpNotification::Fields[0].name == "clientIP", "lSZ field name");
    failures += Expect(
        ClientIpNotification::Fields[0].type == cg::packet::PacketFieldType::EscapedString,
        "lSZ field type");
    failures += Expect(ClientIpNotification::Spec.code == ClientIpNotification::Code, "lSZ Spec code");
    failures += Expect(
        ClientIpNotification::Spec.direction == ClientIpNotification::Direction,
        "lSZ Spec direction");
    failures += Expect(ClientIpNotification::Spec.fieldCount == 1U, "lSZ Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("lSZ"), "lSZ packet code");
    failures += Expect(packet.direction == ClientIpNotification::Direction, "lSZ packet direction");
    failures += Expect(packet.fields.size() == 1U, "lSZ packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "clientIP", "lSZ packet field name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "lSZ packet field type");
        failures += Expect(packet.fields[0].value == "127.0.0.1", "lSZ packet field value");
    }
    return failures;
}

int TestCanonicalAndGenericRoundtrip()
{
    const cg::packet::PacketResult<ClientIpNotification> typed =
        ClientIpNotification::Parse("lSZ 127.0.0.1 ");

    int failures = Expect(typed.HasValue(), "lSZ typed parse");
    if (!typed.HasValue()) {
        return failures;
    }
    failures += Expect(typed.Value().clientIp == "127.0.0.1", "lSZ typed value");

    const cg::packet::PacketResult<std::string> typedBuilt =
        cg::packet::BuildPacket(typed.Value().ToPacket(), ClientIpNotification::Spec);
    failures += Expect(typedBuilt.HasValue(), "lSZ typed build");
    if (typedBuilt.HasValue()) {
        failures += Expect(typedBuilt.Value() == "lSZ 127.0.0.1 ", "lSZ typed canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("lSZ 127.0.0.1", ClientIpNotification::Spec);
    failures += Expect(generic.HasValue(), "lSZ generic parse");
    if (generic.HasValue()) {
        const cg::packet::PacketResult<std::string> genericBuilt =
            cg::packet::BuildPacket(generic.Value(), ClientIpNotification::Spec);
        failures += Expect(genericBuilt.HasValue(), "lSZ generic build");
        if (genericBuilt.HasValue()) {
            failures += Expect(genericBuilt.Value() == "lSZ 127.0.0.1 ", "lSZ generic canonical");
        }
    }
    return failures;
}

int TestEmptyAndEscapedValues()
{
    const cg::packet::PacketResult<ClientIpNotification> empty =
        ClientIpNotification::Parse("lSZ  ");

    int failures = Expect(empty.HasValue(), "lSZ empty parse");
    if (empty.HasValue()) {
        failures += Expect(empty.Value().clientIp.empty(), "lSZ empty value");
        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(empty.Value().ToPacket(), ClientIpNotification::Spec);
        failures += Expect(built.HasValue(), "lSZ empty build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == "lSZ  ", "lSZ empty canonical");
        }
    }

    const std::string wire = "lSZ office\\Spath\\\\name ";
    const cg::packet::PacketResult<ClientIpNotification> escaped =
        ClientIpNotification::Parse(wire);
    failures += Expect(escaped.HasValue(), "lSZ escaped parse");
    if (escaped.HasValue()) {
        failures += Expect(
            escaped.Value().clientIp == "office path\\name",
            "lSZ escaped value");
        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(escaped.Value().ToPacket(), ClientIpNotification::Spec);
        failures += Expect(built.HasValue(), "lSZ escaped build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == wire, "lSZ escaped canonical");
        }
    }
    return failures;
}

int TestGbkRoundtrip()
{
    std::string wire = "lSZ ";
    wire.push_back(static_cast<char>(0x81U));
    wire.push_back(static_cast<char>(0x7CU));
    wire.push_back(' ');

    const cg::packet::PacketResult<ClientIpNotification> parsed =
        ClientIpNotification::Parse(wire);
    int failures = Expect(parsed.HasValue(), "lSZ GBK parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().clientIp.size() == 2U, "lSZ GBK value size");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), ClientIpNotification::Spec);
    failures += Expect(built.HasValue(), "lSZ GBK build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == wire, "lSZ GBK canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ClientIpNotification::Parse("bad 127.0.0.1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "lSZ wrong code");
    failures += ExpectError(
        ClientIpNotification::Parse("lSZ"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lSZ",
        true,
        0U,
        "lSZ missing field");
    failures += ExpectError(
        ClientIpNotification::Parse("lSZ value extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lSZ",
        true,
        1U,
        "lSZ extra field");

    std::string invalid = "lSZ ";
    invalid.push_back(static_cast<char>(0x80U));
    invalid.push_back(' ');
    failures += ExpectError(
        ClientIpNotification::Parse(invalid),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lSZ",
        true,
        0U,
        "lSZ invalid es");

    std::string truncated = "lSZ ";
    truncated.push_back(static_cast<char>(0x81U));
    truncated.push_back(' ');
    failures += ExpectError(
        ClientIpNotification::Parse(truncated),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lSZ",
        true,
        0U,
        "lSZ truncated GBK");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestCanonicalAndGenericRoundtrip()
        + TestEmptyAndEscapedValues()
        + TestGbkRoundtrip()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " client IP notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
