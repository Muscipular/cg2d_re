#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::MagicUseRequest::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::MagicUseRequest::Spec);
}

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
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }

    if (hasFieldIndex) {
        failures += Expect(error.fieldIndex.has_value(), message);
        if (error.fieldIndex.has_value()) {
            failures += Expect(*error.fieldIndex == fieldIndex, message);
        }
    } else {
        failures += Expect(!error.fieldIndex.has_value(), message);
    }
    return failures;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectMagicUseRequestPacket(const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::MagicUseRequest::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 4U, message);
    if (packet.fields.size() != 4U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "x", "1", message);
    failures += ExpectField(packet, 1U, "y", "2", message);
    failures += ExpectField(packet, 2U, "array", "3", message);
    failures += ExpectField(packet, 3U, "toindex", "4", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::MagicUseRequest request(1, 2, 3, 4);

    int failures = ExpectMagicUseRequestPacket(request.ToPacket(), "RXr ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "RXr BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "RXr 1 2 3 4 ", "RXr BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "RXr Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "RXr 1 2 3 4 ", "RXr Encode value");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("RXr 1 2 3 4 ");

    int failures = Expect(static_cast<bool>(parsed), "parse RXr");
    if (parsed.HasValue()) {
        failures += ExpectMagicUseRequestPacket(parsed.Value(), "parse RXr packet");
    }
    return failures;
}

int TestInt62Values()
{
    const cg::packet::send::MagicUseRequest request(10, -1, 62, -62);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "RXr Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "RXr a -1 10 -10 ", "RXr i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "RXr parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectField(parsed.Value(), 0U, "x", "10", "RXr parsed x 10");
        failures += ExpectField(parsed.Value(), 1U, "y", "-1", "RXr parsed y -1");
        failures += ExpectField(parsed.Value(), 2U, "array", "62", "RXr parsed array 62");
        failures += ExpectField(parsed.Value(), 3U, "toindex", "-62", "RXr parsed toindex -62");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("RXr 1 2 3"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "RXr",
        true,
        3U,
        "RXr missing toindex");
    failures += ExpectError(
        ParsePacket("RXr 1 2 3 4 5 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "RXr",
        true,
        4U,
        "RXr extra field");
    failures += ExpectError(
        ParsePacket("RXr _ 2 3 4 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RXr",
        true,
        0U,
        "RXr bad x");
    failures += ExpectError(
        ParsePacket("RXr 1 _ 3 4 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RXr",
        true,
        1U,
        "RXr bad y");
    failures += ExpectError(
        ParsePacket("RXr 1 2 _ 4 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RXr",
        true,
        2U,
        "RXr bad array");
    failures += ExpectError(
        ParsePacket("RXr 1 2 3 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "RXr",
        true,
        3U,
        "RXr bad toindex");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::MagicUseRequest(1, 2, 3, 4).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "RXr",
        false,
        0U,
        "RXr direction mismatch");

    cg::packet::Packet wrongCode = cg::packet::send::MagicUseRequest(1, 2, 3, 4).ToPacket();
    wrongCode.code = cg::packet::PacketCode("RXx");
    failures += ExpectError(
        BuildPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "RXx",
        false,
        0U,
        "RXr wrong code");

    cg::packet::Packet wrongName = cg::packet::send::MagicUseRequest(1, 2, 3, 4).ToPacket();
    wrongName.fields[2].name = "magic";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "RXr",
        true,
        2U,
        "RXr field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::MagicUseRequest(1, 2, 3, 4).ToPacket();
    wrongType.fields[3].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "RXr",
        true,
        3U,
        "RXr field type mismatch");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestParsePacketRoundtrip();
    failures += TestInt62Values();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
