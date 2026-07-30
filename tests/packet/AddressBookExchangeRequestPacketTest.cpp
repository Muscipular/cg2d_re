#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::AddressBookExchangeRequest;

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

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(
        packet.fields[index].type == cg::packet::PacketFieldType::Int62,
        message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int TestSpecAndEncode()
{
    const AddressBookExchangeRequest request(10, 20);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(AddressBookExchangeRequest::Code == "vcAz", "vcAz code");
    failures += Expect(
        AddressBookExchangeRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "vcAz direction");
    failures += Expect(
        AddressBookExchangeRequest::Fields.size() == 2U,
        "vcAz fields");
    failures += Expect(
        AddressBookExchangeRequest::Fields[0].name == "x",
        "vcAz x name");
    failures += Expect(
        AddressBookExchangeRequest::Fields[1].name == "y",
        "vcAz y name");
    for (const cg::packet::PacketFieldSpec& field :
         AddressBookExchangeRequest::Fields) {
        failures += Expect(
            field.type == cg::packet::PacketFieldType::Int62,
            "vcAz field type");
    }
    failures += Expect(
        AddressBookExchangeRequest::Spec.code == AddressBookExchangeRequest::Code,
        "vcAz Spec code");
    failures += Expect(
        AddressBookExchangeRequest::Spec.direction ==
            AddressBookExchangeRequest::Direction,
        "vcAz Spec direction");
    failures += Expect(
        AddressBookExchangeRequest::Spec.fieldCount == 2U,
        "vcAz Spec field count");
    failures += Expect(request.x == 10, "vcAz x member");
    failures += Expect(request.y == 20, "vcAz y member");
    failures += Expect(packet.code == cg::packet::PacketCode("vcAz"), "vcAz packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "vcAz packet direction");
    failures += Expect(packet.fields.size() == 2U, "vcAz packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "vcAz x field");
    failures += ExpectField(packet, 1U, "y", "20", "vcAz y field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "vcAz Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "vcAz a k ",
            "vcAz Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("vcAz a k", AddressBookExchangeRequest::Spec);

    int failures = Expect(parsed.HasValue(), "vcAz generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "vcAz parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "vcAz parsed y");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), AddressBookExchangeRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "vcAz generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "vcAz a k ",
            "vcAz generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int x;
        int y;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, -1, "vcAz 0 -1 "},
        {60, 61, "vcAz Y Z "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "vcAz -2lkCB2 2lkCB1 ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const AddressBookExchangeRequest request(testCase.x, testCase.y);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "x",
            std::to_string(testCase.x),
            "vcAz signed x");
        failures += ExpectField(
            packet,
            1U,
            "y",
            std::to_string(testCase.y),
            "vcAz signed y");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "vcAz signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "vcAz signed canonical");
        }
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "vcAz " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k ", AddressBookExchangeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "vcAz wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("vcAz", AddressBookExchangeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vcAz",
        true,
        0U,
        "vcAz missing both fields");
    failures += ExpectError(
        cg::packet::ParsePacket("vcAz a", AddressBookExchangeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vcAz",
        true,
        1U,
        "vcAz missing y");
    failures += ExpectError(
        cg::packet::ParsePacket("vcAz a k 3 ", AddressBookExchangeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vcAz",
        true,
        2U,
        "vcAz extra field");

    const std::array<std::string, 2U> validTokens = {{"a", "k"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens),
                AddressBookExchangeRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vcAz",
            true,
            index,
            "vcAz invalid i62");

        std::array<std::string, 2U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(overflowTokens),
                AddressBookExchangeRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vcAz",
            true,
            index,
            "vcAz i62 overflow");
    }
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
        std::cerr << failures
                  << " address book exchange request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
