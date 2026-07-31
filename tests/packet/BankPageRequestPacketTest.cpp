#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::BankPageRequest;

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
    std::string_view page,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode("rh"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "page", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == page, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const BankPageRequest request(2);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BankPageRequest::Code == "rh", "rh code");
    failures += Expect(
        BankPageRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "rh direction");
    failures += Expect(
        BankPageRequest::Fields.size() == 1U,
        "rh field count");
    failures += Expect(
        BankPageRequest::Fields[0].name == "page",
        "rh field name");
    failures += Expect(
        BankPageRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "rh field type");
    failures += Expect(
        BankPageRequest::Spec.code == BankPageRequest::Code,
        "rh Spec code");
    failures += Expect(
        BankPageRequest::Spec.direction == BankPageRequest::Direction,
        "rh Spec direction");
    failures += Expect(
        BankPageRequest::Spec.fieldCount == 1U,
        "rh Spec field count");
    failures += Expect(request.page == 2, "rh member");
    failures += ExpectPacket(packet, "2", "rh ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "rh Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "rh 2 ", "rh canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("rh 2", BankPageRequest::Spec);
    failures += Expect(parsed.HasValue(), "rh generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(parsed.Value(), "2", "rh generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(parsed.Value(), BankPageRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "rh generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "rh 2 ",
                "rh generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int page;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "rh 0 "},
        {1, "rh 1 "},
        {2, "rh 2 "},
        {3, "rh 3 "},
        {-1, "rh -1 "},
        {-123, "rh -1Z "},
        {std::numeric_limits<int>::min(), "rh -2lkCB2 "},
        {std::numeric_limits<int>::max(), "rh 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const BankPageRequest request(testCase.page);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.page),
            "rh signed packet");
        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "rh signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "rh signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 2", BankPageRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "rh wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("rh", BankPageRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rh",
        true,
        0U,
        "rh missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("rh 2 1", BankPageRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rh",
        true,
        1U,
        "rh extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("rh _", BankPageRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rh",
        true,
        0U,
        "rh invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("rh 2lkCB2", BankPageRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rh",
        true,
        0U,
        "rh positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("rh -2lkCB3", BankPageRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rh",
        true,
        0U,
        "rh negative i62 overflow");

    const std::string pollutedCode = "rh\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 2",
            BankPageRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "rh polluted code");
    const std::string pollutedField = "rh 2\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rh",
        true,
        1U,
        "rh polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    cg::packet::Packet wrongCode = BankPageRequest(2).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "rh Build wrong code");

    cg::packet::Packet wrongDirection = BankPageRequest(2).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rh",
        false,
        0U,
        "rh Build wrong direction");

    cg::packet::Packet wrongName = BankPageRequest(2).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rh",
        true,
        0U,
        "rh Build wrong field name");

    cg::packet::Packet wrongType = BankPageRequest(2).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rh",
        true,
        0U,
        "rh Build wrong field type");

    cg::packet::Packet emptyValue = BankPageRequest(2).ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        cg::packet::BuildPacket(emptyValue, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rh",
        true,
        0U,
        "rh Build empty decimal");

    cg::packet::Packet invalidValue = BankPageRequest(2).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        cg::packet::BuildPacket(invalidValue, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rh",
        true,
        0U,
        "rh Build non-decimal");

    cg::packet::Packet positiveOverflow = BankPageRequest(2).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        cg::packet::BuildPacket(positiveOverflow, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rh",
        true,
        0U,
        "rh Build positive overflow");

    cg::packet::Packet negativeOverflow = BankPageRequest(2).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        cg::packet::BuildPacket(negativeOverflow, BankPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rh",
        true,
        0U,
        "rh Build negative overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " bank page request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
