#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::StallStartResult;

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
    failures += Expect(
        packet.code == cg::packet::PacketCode("Tmrx"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "result", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndSuccess()
{
    const StallStartResult result(0);
    const cg::packet::Packet packet = result.ToPacket();

    int failures = 0;
    failures += Expect(StallStartResult::Code == "Tmrx", "Tmrx code");
    failures += Expect(
        StallStartResult::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "Tmrx direction");
    failures += Expect(
        StallStartResult::Fields.size() == 1U,
        "Tmrx fields");
    failures += Expect(
        StallStartResult::Fields[0].name == "result",
        "Tmrx result name");
    failures += Expect(
        StallStartResult::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "Tmrx result type");
    failures += Expect(
        StallStartResult::Spec.code == StallStartResult::Code,
        "Tmrx Spec code");
    failures += Expect(
        StallStartResult::Spec.direction ==
            StallStartResult::Direction,
        "Tmrx Spec direction");
    failures += Expect(
        StallStartResult::Spec.fieldCount == 1U,
        "Tmrx Spec field count");
    failures += Expect(result.result == 0, "Tmrx member");
    failures += ExpectPacket(packet, "0", "Tmrx packet");

    const cg::packet::PacketResult<StallStartResult> converted =
        StallStartResult::FromPacket(packet);
    failures += Expect(converted.HasValue(), "Tmrx FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(
            converted.Value().result == 0,
            "Tmrx FromPacket result");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, StallStartResult::Spec);
    failures += Expect(built.HasValue(), "Tmrx success build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "Tmrx 0 ",
            "Tmrx success canonical");
    }
    return failures;
}

int TestTypedAndGenericRoundtrip()
{
    const cg::packet::PacketResult<StallStartResult> typed =
        StallStartResult::Parse("Tmrx 0");

    int failures = Expect(typed.HasValue(), "Tmrx typed parse");
    if (!typed.HasValue()) {
        return failures;
    }
    failures += Expect(typed.Value().result == 0, "Tmrx typed value");

    const cg::packet::PacketResult<std::string> typedBuilt =
        cg::packet::BuildPacket(
            typed.Value().ToPacket(),
            StallStartResult::Spec);
    failures += Expect(typedBuilt.HasValue(), "Tmrx typed build");
    if (typedBuilt.HasValue()) {
        failures += Expect(
            typedBuilt.Value() == "Tmrx 0 ",
            "Tmrx typed canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("Tmrx 0", StallStartResult::Spec);
    failures += Expect(generic.HasValue(), "Tmrx generic parse");
    if (!generic.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(
        generic.Value(),
        "0",
        "Tmrx generic packet");

    const cg::packet::PacketResult<std::string> genericBuilt =
        cg::packet::BuildPacket(
            generic.Value(),
            StallStartResult::Spec);
    failures += Expect(genericBuilt.HasValue(), "Tmrx generic build");
    if (genericBuilt.HasValue()) {
        failures += Expect(
            genericBuilt.Value() == "Tmrx 0 ",
            "Tmrx generic canonical");
    }
    return failures;
}

int TestSignedAndBusinessOutsideValues()
{
    struct Case
    {
        int result;
        std::string_view expected;
    };

    // 0..4/100 是已知业务结果；负数与 int 边界仅证明 cg3a codec。
    constexpr Case Cases[] = {
        {0, "Tmrx 0 "},
        {1, "Tmrx 1 "},
        {2, "Tmrx 2 "},
        {3, "Tmrx 3 "},
        {4, "Tmrx 4 "},
        {100, "Tmrx 1C "},
        {-1, "Tmrx -1 "},
        {std::numeric_limits<int>::min(), "Tmrx -2lkCB2 "},
        {std::numeric_limits<int>::max(), "Tmrx 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const StallStartResult value(testCase.result);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(testCase.result),
            "Tmrx signed packet");

        const cg::packet::PacketResult<std::string> encoded =
            cg::packet::BuildPacket(
                value.ToPacket(),
                StallStartResult::Spec);
        failures += Expect(encoded.HasValue(), "Tmrx signed build");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "Tmrx signed canonical");
        }

        const cg::packet::PacketResult<StallStartResult> parsed =
            StallStartResult::Parse(testCase.expected);
        failures += Expect(parsed.HasValue(), "Tmrx signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().result == testCase.result,
                "Tmrx signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        StallStartResult::Parse("bad 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Tmrx wrong code");
    failures += ExpectError(
        StallStartResult::Parse("Tmrx"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Tmrx",
        true,
        0U,
        "Tmrx missing field");
    failures += ExpectError(
        StallStartResult::Parse("Tmrx 0 1 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Tmrx",
        true,
        1U,
        "Tmrx extra field");
    failures += ExpectError(
        StallStartResult::Parse("Tmrx _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Tmrx",
        true,
        0U,
        "Tmrx invalid i62");
    failures += ExpectError(
        StallStartResult::Parse("Tmrx 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Tmrx",
        true,
        0U,
        "Tmrx positive overflow");
    failures += ExpectError(
        StallStartResult::Parse("Tmrx -2lkCB3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Tmrx",
        true,
        0U,
        "Tmrx negative overflow");

    const std::string pollutedCode = "Tmrx\x17" "x";
    const std::string pollutedWire = pollutedCode + " 0";
    failures += ExpectError(
        StallStartResult::Parse(pollutedWire),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "Tmrx polluted code");

    const std::string pollutedField = "Tmrx 0\x17" "x";
    failures += ExpectError(
        StallStartResult::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Tmrx",
        true,
        1U,
        "Tmrx polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = StallStartResult(0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        StallStartResult::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Tmrx FromPacket wrong code");

    cg::packet::Packet wrongDirection = StallStartResult(0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        StallStartResult::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Tmrx",
        false,
        0U,
        "Tmrx FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("Tmrx"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        StallStartResult::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Tmrx",
        true,
        0U,
        "Tmrx FromPacket missing field");

    cg::packet::Packet extraField = StallStartResult(0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "1",
    });
    failures += ExpectError(
        StallStartResult::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Tmrx",
        true,
        1U,
        "Tmrx FromPacket extra field");

    cg::packet::Packet wrongName = StallStartResult(0).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        StallStartResult::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Tmrx",
        true,
        0U,
        "Tmrx FromPacket wrong name");

    cg::packet::Packet wrongType = StallStartResult(0).ToPacket();
    wrongType.fields[0].type =
        cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        StallStartResult::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Tmrx",
        true,
        0U,
        "Tmrx FromPacket wrong type");

    cg::packet::Packet invalidValue = StallStartResult(0).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        StallStartResult::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Tmrx",
        true,
        0U,
        "Tmrx FromPacket invalid decimal");

    cg::packet::Packet overflowValue = StallStartResult(0).ToPacket();
    overflowValue.fields[0].value = "2147483648";
    failures += ExpectError(
        StallStartResult::FromPacket(overflowValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Tmrx",
        true,
        0U,
        "Tmrx FromPacket decimal overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndSuccess()
        + TestTypedAndGenericRoundtrip()
        + TestSignedAndBusinessOutsideValues()
        + TestParseErrors()
        + TestFromPacketErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " stall start result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
