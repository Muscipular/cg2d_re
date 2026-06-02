#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::send::MapDataRequest::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
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

int ExpectParsedField(
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

int ExpectMapDataRequestPacket(
    const cg::packet::Packet& packet,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::MapDataRequest::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 6U, message);
    if (packet.fields.size() != 6U) {
        return failures;
    }

    failures += ExpectParsedField(packet, 0U, "mapid", "1", message);
    failures += ExpectParsedField(packet, 1U, "floor", "2", message);
    failures += ExpectParsedField(packet, 2U, "x1", "1", message);
    failures += ExpectParsedField(packet, 3U, "y1", "2", message);
    failures += ExpectParsedField(packet, 4U, "x2", "3", message);
    failures += ExpectParsedField(packet, 5U, "y2", "4", message);
    return failures;
}

int TestBuildAndEncode()
{
    const cg::packet::send::MapDataRequest request(1, 2, 1, 2, 3, 4);

    int failures = ExpectMapDataRequestPacket(request.ToPacket(), "UUN ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "UUN BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "UUN 1 2 1 2 3 4 ", "UUN BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "UUN Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "UUN 1 2 1 2 3 4 ", "UUN Encode value");
    }
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("UUN 1 2 1 2 3 4 ");

    int failures = Expect(static_cast<bool>(parsed), "parse UUN");
    if (parsed.HasValue()) {
        failures += ExpectMapDataRequestPacket(parsed.Value(), "parse UUN packet");
    }
    return failures;
}

int TestInt62Values()
{
    const cg::packet::send::MapDataRequest request(10, -1, 1, 2, 3, 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "UUN Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "UUN a -1 1 2 3 4 ", "UUN i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "UUN parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectParsedField(parsed.Value(), 0U, "mapid", "10", "UUN parsed mapid 10");
        failures += ExpectParsedField(parsed.Value(), 1U, "floor", "-1", "UUN parsed floor -1");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("UUN 1 2 1 2 3"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UUN",
        true,
        5U,
        "UUN missing field");
    failures += ExpectError(
        ParsePacket("UUN 1 2 1 2 3 4 5 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UUN",
        true,
        6U,
        "UUN extra field");
    failures += ExpectError(
        ParsePacket("UUN _ 2 1 2 3 4 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UUN",
        true,
        0U,
        "UUN bad i62");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestBuildAndEncode();
    failures += TestParsePacket();
    failures += TestInt62Values();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
