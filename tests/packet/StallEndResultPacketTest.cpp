#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::StallEndResult;

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
        packet.code == cg::packet::PacketCode("KXhH"),
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
    const StallEndResult result(0);
    const cg::packet::Packet packet = result.ToPacket();

    int failures = 0;
    failures += Expect(StallEndResult::Code == "KXhH", "KXhH code");
    failures += Expect(
        StallEndResult::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "KXhH direction");
    failures += Expect(
        StallEndResult::Fields.size() == 1U,
        "KXhH fields");
    failures += Expect(
        StallEndResult::Fields[0].name == "result",
        "KXhH result name");
    failures += Expect(
        StallEndResult::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "KXhH result type");
    failures += Expect(
        StallEndResult::Spec.code == StallEndResult::Code,
        "KXhH Spec code");
    failures += Expect(
        StallEndResult::Spec.direction == StallEndResult::Direction,
        "KXhH Spec direction");
    failures += Expect(
        StallEndResult::Spec.fieldCount == 1U,
        "KXhH Spec field count");
    failures += Expect(result.result == 0, "KXhH member");
    failures += ExpectPacket(packet, "0", "KXhH packet");

    const cg::packet::PacketResult<StallEndResult> converted =
        StallEndResult::FromPacket(packet);
    failures += Expect(converted.HasValue(), "KXhH FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(
            converted.Value().result == 0,
            "KXhH FromPacket result");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, StallEndResult::Spec);
    failures += Expect(built.HasValue(), "KXhH success build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "KXhH 0 ",
            "KXhH success canonical");
    }
    return failures;
}

int TestTypedAndGenericRoundtrip()
{
    const cg::packet::PacketResult<StallEndResult> typed =
        StallEndResult::Parse("KXhH 0");

    int failures = Expect(typed.HasValue(), "KXhH typed parse");
    if (!typed.HasValue()) {
        return failures;
    }
    failures += Expect(typed.Value().result == 0, "KXhH typed value");

    const cg::packet::PacketResult<std::string> typedBuilt =
        cg::packet::BuildPacket(
            typed.Value().ToPacket(),
            StallEndResult::Spec);
    failures += Expect(typedBuilt.HasValue(), "KXhH typed build");
    if (typedBuilt.HasValue()) {
        failures += Expect(
            typedBuilt.Value() == "KXhH 0 ",
            "KXhH typed canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("KXhH 0", StallEndResult::Spec);
    failures += Expect(generic.HasValue(), "KXhH generic parse");
    if (!generic.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(generic.Value(), "0", "KXhH generic packet");

    const cg::packet::PacketResult<std::string> genericBuilt =
        cg::packet::BuildPacket(generic.Value(), StallEndResult::Spec);
    failures += Expect(genericBuilt.HasValue(), "KXhH generic build");
    if (genericBuilt.HasValue()) {
        failures += Expect(
            genericBuilt.Value() == "KXhH 0 ",
            "KXhH generic canonical");
    }
    return failures;
}

int TestSignedAndBusinessValues()
{
    struct Case
    {
        int result;
        std::string_view expected;
    };

    // 0/100 是已知业务结果；其他非零值、负数与 int 边界仅证明 cg3a codec。
    constexpr Case Cases[] = {
        {0, "KXhH 0 "},
        {100, "KXhH 1C "},
        {7, "KXhH 7 "},
        {-1, "KXhH -1 "},
        {std::numeric_limits<int>::min(), "KXhH -2lkCB2 "},
        {std::numeric_limits<int>::max(), "KXhH 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const StallEndResult value(testCase.result);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(testCase.result),
            "KXhH signed packet");

        const cg::packet::PacketResult<std::string> encoded =
            cg::packet::BuildPacket(
                value.ToPacket(),
                StallEndResult::Spec);
        failures += Expect(encoded.HasValue(), "KXhH signed build");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "KXhH signed canonical");
        }

        const cg::packet::PacketResult<StallEndResult> parsed =
            StallEndResult::Parse(testCase.expected);
        failures += Expect(parsed.HasValue(), "KXhH signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().result == testCase.result,
                "KXhH signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        StallEndResult::Parse("bad 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "KXhH wrong code");
    failures += ExpectError(
        StallEndResult::Parse("KXhH"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KXhH",
        true,
        0U,
        "KXhH missing field");
    failures += ExpectError(
        StallEndResult::Parse("KXhH 0 1 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KXhH",
        true,
        1U,
        "KXhH extra field");
    failures += ExpectError(
        StallEndResult::Parse("KXhH _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KXhH",
        true,
        0U,
        "KXhH invalid i62");
    failures += ExpectError(
        StallEndResult::Parse("KXhH 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KXhH",
        true,
        0U,
        "KXhH positive overflow");
    failures += ExpectError(
        StallEndResult::Parse("KXhH -2lkCB3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KXhH",
        true,
        0U,
        "KXhH negative overflow");

    const std::string pollutedCode = "KXhH\x17" "x";
    const std::string pollutedWire = pollutedCode + " 0";
    failures += ExpectError(
        StallEndResult::Parse(pollutedWire),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "KXhH polluted code");

    const std::string pollutedField = "KXhH 0\x17" "x";
    failures += ExpectError(
        StallEndResult::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KXhH",
        true,
        1U,
        "KXhH polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = StallEndResult(0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        StallEndResult::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "KXhH FromPacket wrong code");

    cg::packet::Packet wrongDirection = StallEndResult(0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        StallEndResult::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KXhH",
        false,
        0U,
        "KXhH FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("KXhH"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        StallEndResult::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KXhH",
        true,
        0U,
        "KXhH FromPacket missing field");

    cg::packet::Packet extraField = StallEndResult(0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "1",
    });
    failures += ExpectError(
        StallEndResult::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KXhH",
        true,
        1U,
        "KXhH FromPacket extra field");

    cg::packet::Packet wrongName = StallEndResult(0).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        StallEndResult::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KXhH",
        true,
        0U,
        "KXhH FromPacket wrong name");

    cg::packet::Packet wrongType = StallEndResult(0).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        StallEndResult::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "KXhH",
        true,
        0U,
        "KXhH FromPacket wrong type");

    cg::packet::Packet invalidValue = StallEndResult(0).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        StallEndResult::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KXhH",
        true,
        0U,
        "KXhH FromPacket invalid decimal");

    cg::packet::Packet positiveOverflow = StallEndResult(0).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        StallEndResult::FromPacket(positiveOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KXhH",
        true,
        0U,
        "KXhH FromPacket positive decimal overflow");

    cg::packet::Packet negativeOverflow = StallEndResult(0).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        StallEndResult::FromPacket(negativeOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "KXhH",
        true,
        0U,
        "KXhH FromPacket negative decimal overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndSuccess()
        + TestTypedAndGenericRoundtrip()
        + TestSignedAndBusinessValues()
        + TestParseErrors()
        + TestFromPacketErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " stall end result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
