#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::AlbumNameRequest;

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

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode("LaR"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "albumid", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const AlbumNameRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(AlbumNameRequest::Code == "LaR", "LaR code");
    failures += Expect(
        AlbumNameRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "LaR direction");
    failures += Expect(AlbumNameRequest::Fields.size() == 1U, "LaR fields");
    failures += Expect(
        AlbumNameRequest::Fields[0].name == "albumid",
        "LaR field name");
    failures += Expect(
        AlbumNameRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "LaR field type");
    failures += Expect(
        AlbumNameRequest::Spec.code == AlbumNameRequest::Code,
        "LaR Spec code");
    failures += Expect(
        AlbumNameRequest::Spec.direction == AlbumNameRequest::Direction,
        "LaR Spec direction");
    failures += Expect(AlbumNameRequest::Spec.fieldCount == 1U, "LaR Spec field count");
    failures += Expect(request.albumId == 10, "LaR member");
    failures += ExpectPacket(packet, "10", "LaR packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "LaR Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "LaR a ", "LaR Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("LaR a", AlbumNameRequest::Spec);

    int failures = Expect(parsed.HasValue(), "LaR generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "LaR generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), AlbumNameRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "LaR generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "LaR a ", "LaR generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int albumId;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "LaR 0 "},
        {-1, "LaR -1 "},
        {960, "LaR fu "},
        {std::numeric_limits<int>::min(), "LaR -2lkCB2 "},
        {std::numeric_limits<int>::max(), "LaR 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const AlbumNameRequest request(testCase.albumId);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.albumId),
            "LaR signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "LaR signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(encoded.Value() == testCase.expected, "LaR signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a ", AlbumNameRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "LaR wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("LaR", AlbumNameRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LaR",
        true,
        0U,
        "LaR missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("LaR a 1 ", AlbumNameRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LaR",
        true,
        1U,
        "LaR extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("LaR _ ", AlbumNameRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LaR",
        true,
        0U,
        "LaR invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("LaR 2lkCB2 ", AlbumNameRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LaR",
        true,
        0U,
        "LaR i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndBoundaryValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " album name request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
