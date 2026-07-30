#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::AddressBookDeleteRequest;

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
    failures += Expect(packet.code == cg::packet::PacketCode("FPkR"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "index", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const AddressBookDeleteRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(AddressBookDeleteRequest::Code == "FPkR", "FPkR code");
    failures += Expect(
        AddressBookDeleteRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "FPkR direction");
    failures += Expect(AddressBookDeleteRequest::Fields.size() == 1U, "FPkR fields");
    failures += Expect(
        AddressBookDeleteRequest::Fields[0].name == "index",
        "FPkR field name");
    failures += Expect(
        AddressBookDeleteRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "FPkR field type");
    failures += Expect(
        AddressBookDeleteRequest::Spec.code == AddressBookDeleteRequest::Code,
        "FPkR Spec code");
    failures += Expect(
        AddressBookDeleteRequest::Spec.direction == AddressBookDeleteRequest::Direction,
        "FPkR Spec direction");
    failures += Expect(
        AddressBookDeleteRequest::Spec.fieldCount == 1U,
        "FPkR Spec field count");
    failures += Expect(request.index == 10, "FPkR member");
    failures += ExpectPacket(packet, "10", "FPkR packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "FPkR Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "FPkR a ", "FPkR Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("FPkR a", AddressBookDeleteRequest::Spec);

    int failures = Expect(parsed.HasValue(), "FPkR generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "FPkR generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), AddressBookDeleteRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "FPkR generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "FPkR a ", "FPkR generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int index;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "FPkR 0 "},
        {-1, "FPkR -1 "},
        {60, "FPkR Y "},
        {std::numeric_limits<int>::min(), "FPkR -2lkCB2 "},
        {std::numeric_limits<int>::max(), "FPkR 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const AddressBookDeleteRequest request(testCase.index);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.index),
            "FPkR signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "FPkR signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "FPkR signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a ", AddressBookDeleteRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "FPkR wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("FPkR", AddressBookDeleteRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "FPkR",
        true,
        0U,
        "FPkR missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("FPkR a 1 ", AddressBookDeleteRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "FPkR",
        true,
        1U,
        "FPkR extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("FPkR _ ", AddressBookDeleteRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "FPkR",
        true,
        0U,
        "FPkR invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("FPkR 2lkCB2 ", AddressBookDeleteRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "FPkR",
        true,
        0U,
        "FPkR i62 overflow");
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
        std::cerr << failures << " address book delete request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
