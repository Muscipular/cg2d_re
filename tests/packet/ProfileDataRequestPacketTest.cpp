#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ProfileDataRequest;

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
    const ProfileDataRequest request(3, 10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ProfileDataRequest::Code == "TLKH", "TLKH code");
    failures += Expect(
        ProfileDataRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "TLKH direction");
    failures += Expect(ProfileDataRequest::Fields.size() == 2U, "TLKH fields");
    failures += Expect(
        ProfileDataRequest::Fields[0].name == "flg",
        "TLKH flg name");
    failures += Expect(
        ProfileDataRequest::Fields[1].name == "index",
        "TLKH index name");
    for (const cg::packet::PacketFieldSpec& field : ProfileDataRequest::Fields) {
        failures += Expect(
            field.type == cg::packet::PacketFieldType::Int62,
            "TLKH field type");
    }
    failures += Expect(
        ProfileDataRequest::Spec.code == ProfileDataRequest::Code,
        "TLKH Spec code");
    failures += Expect(
        ProfileDataRequest::Spec.direction == ProfileDataRequest::Direction,
        "TLKH Spec direction");
    failures += Expect(
        ProfileDataRequest::Spec.fieldCount == 2U,
        "TLKH Spec field count");
    failures += Expect(request.flg == 3, "TLKH flg member");
    failures += Expect(request.index == 10, "TLKH index member");
    failures += Expect(
        packet.code == cg::packet::PacketCode("TLKH"),
        "TLKH packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "TLKH packet direction");
    failures += Expect(packet.fields.size() == 2U, "TLKH packet fields");
    failures += ExpectField(packet, 0U, "flg", "3", "TLKH flg field");
    failures += ExpectField(packet, 1U, "index", "10", "TLKH index field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "TLKH Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "TLKH 3 a ",
            "TLKH Encode canonical");
    }
    return failures;
}

int TestKnownCallerModes()
{
    struct Case
    {
        int flg;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "TLKH 0 a "},
        {1, "TLKH 1 a "},
        {2, "TLKH 2 a "},
        {3, "TLKH 3 a "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const cg::packet::PacketResult<std::string> encoded =
            ProfileDataRequest(testCase.flg, 10).Encode();
        failures += Expect(encoded.HasValue(), "TLKH known caller mode Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "TLKH known caller mode canonical");
        }
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("TLKH 3 a", ProfileDataRequest::Spec);

    int failures = Expect(parsed.HasValue(), "TLKH generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "flg", "3", "TLKH parsed flg");
    failures += ExpectField(
        parsed.Value(),
        1U,
        "index",
        "10",
        "TLKH parsed index");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), ProfileDataRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "TLKH generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "TLKH 3 a ",
            "TLKH generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int flg;
        int index;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, 0, "TLKH 0 0 "},
        {4, 10, "TLKH 4 a "},
        {-1, -2, "TLKH -1 -2 "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "TLKH -2lkCB2 2lkCB1 ",
        },
        {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            "TLKH 2lkCB1 -2lkCB2 ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ProfileDataRequest request(testCase.flg, testCase.index);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "flg",
            std::to_string(testCase.flg),
            "TLKH signed flg");
        failures += ExpectField(
            packet,
            1U,
            "index",
            std::to_string(testCase.index),
            "TLKH signed index");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "TLKH signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "TLKH signed canonical");
        }
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "TLKH " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 3 a ", ProfileDataRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "TLKH wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("TLKH", ProfileDataRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TLKH",
        true,
        0U,
        "TLKH missing both fields");
    failures += ExpectError(
        cg::packet::ParsePacket("TLKH 3", ProfileDataRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TLKH",
        true,
        1U,
        "TLKH missing index");
    failures += ExpectError(
        cg::packet::ParsePacket("TLKH 3 a 1 ", ProfileDataRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TLKH",
        true,
        2U,
        "TLKH extra field");

    const std::array<std::string, 2U> validTokens = {{"3", "a"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens),
                ProfileDataRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "TLKH",
            true,
            index,
            "TLKH invalid i62");

        std::array<std::string, 2U> positiveOverflowTokens = validTokens;
        positiveOverflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(positiveOverflowTokens),
                ProfileDataRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "TLKH",
            true,
            index,
            "TLKH positive i62 overflow");

        std::array<std::string, 2U> negativeOverflowTokens = validTokens;
        negativeOverflowTokens[index] = "-2lkCB3";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(negativeOverflowTokens),
                ProfileDataRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "TLKH",
            true,
            index,
            "TLKH negative i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestKnownCallerModes()
        + TestGenericRoundtrip()
        + TestSignedAndBoundaryValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " profile data request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
