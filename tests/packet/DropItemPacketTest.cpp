#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::DropItem;

constexpr std::array<std::string_view, 4U> FieldNames = {{
    "x",
    "y",
    "who",
    "itemindex",
}};

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
    const std::array<std::string, 4U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("QpfE"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == FieldNames.size(), message);
    if (packet.fields.size() != FieldNames.size()) {
        return failures;
    }

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            packet.fields[index].name == FieldNames[index],
            message);
        failures += Expect(
            packet.fields[index].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(
            packet.fields[index].value == expected[index],
            message);
    }
    return failures;
}

std::string BuildWireWithField(std::size_t fieldIndex, std::string_view token)
{
    constexpr std::array<std::string_view, 4U> ValidTokens = {{
        "a",
        "k",
        "0",
        "10",
    }};

    std::string wire = "QpfE";
    for (std::size_t index = 0U; index < ValidTokens.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : ValidTokens[index]);
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    const DropItem value(10, 20, 0, 62);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(DropItem::Code == "QpfE", "QpfE code");
    failures += Expect(
        DropItem::Direction == cg::packet::PacketDirection::ClientToServer,
        "QpfE direction");
    failures += Expect(
        DropItem::Fields.size() == FieldNames.size(),
        "QpfE field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            DropItem::Fields[index].name == FieldNames[index],
            "QpfE field name order");
        failures += Expect(
            DropItem::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "QpfE field type order");
    }
    failures += Expect(
        DropItem::Spec.code == DropItem::Code,
        "QpfE Spec code");
    failures += Expect(
        DropItem::Spec.direction == DropItem::Direction,
        "QpfE Spec direction");
    failures += Expect(
        DropItem::Spec.fieldCount == FieldNames.size(),
        "QpfE Spec field count");
    failures += Expect(value.x == 10, "QpfE x member");
    failures += Expect(value.y == 20, "QpfE y member");
    failures += Expect(value.who == 0, "QpfE who member");
    failures += Expect(value.itemIndex == 62, "QpfE itemIndex member");
    failures += ExpectPacket(
        packet,
        {{"10", "20", "0", "62"}},
        "QpfE ToPacket");

    const cg::packet::PacketResult<std::string> encoded = value.Encode();
    failures += Expect(encoded.HasValue(), "QpfE Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "QpfE a k 0 10 ",
            "QpfE canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("QpfE a k 0 10", DropItem::Spec);
    failures += Expect(parsed.HasValue(), "QpfE generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            {{"10", "20", "0", "62"}},
            "QpfE generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(parsed.Value(), DropItem::Spec);
        failures += Expect(rebuilt.HasValue(), "QpfE generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "QpfE a k 0 10 ",
                "QpfE generic canonical");
        }
    }
    return failures;
}

int TestWhoValues()
{
    struct Case
    {
        int who;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {-1, "QpfE a k -1 10 "},
        {0, "QpfE a k 0 10 "},
        {1, "QpfE a k 1 10 "},
        {5, "QpfE a k 5 10 "},
        {6, "QpfE a k 6 10 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const DropItem value(10, 20, testCase.who, 62);
        const cg::packet::PacketResult<std::string> encoded = value.Encode();
        failures += Expect(encoded.HasValue(), "QpfE who Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "QpfE who wire fidelity");
        }

        failures += ExpectPacket(
            value.ToPacket(),
            {{"10", "20", std::to_string(testCase.who), "62"}},
            "QpfE who ToPacket fidelity");
    }
    return failures;
}

int TestArbitraryAndExtremeValues()
{
    struct Case
    {
        std::array<int, 4U> values;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {{{-123, 456, -789, 12345}}, "QpfE -1Z 7m -cJ 3d7 "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "QpfE -2lkCB2 2lkCB1 -2lkCB2 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const DropItem value(
            testCase.values[0],
            testCase.values[1],
            testCase.values[2],
            testCase.values[3]);
        failures += ExpectPacket(
            value.ToPacket(),
            {{
                std::to_string(testCase.values[0]),
                std::to_string(testCase.values[1]),
                std::to_string(testCase.values[2]),
                std::to_string(testCase.values[3]),
            }},
            "QpfE arbitrary ToPacket");

        const cg::packet::PacketResult<std::string> encoded = value.Encode();
        failures += Expect(encoded.HasValue(), "QpfE arbitrary Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "QpfE arbitrary canonical");
        }

        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(testCase.canonical, DropItem::Spec);
        failures += Expect(parsed.HasValue(), "QpfE arbitrary parse");
        if (parsed.HasValue()) {
            failures += ExpectPacket(
                parsed.Value(),
                {{
                    std::to_string(testCase.values[0]),
                    std::to_string(testCase.values[1]),
                    std::to_string(testCase.values[2]),
                    std::to_string(testCase.values[3]),
                }},
                "QpfE arbitrary parsed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k 0 10", DropItem::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "QpfE wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("QpfE a k 0", DropItem::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "QpfE",
        true,
        3U,
        "QpfE missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("QpfE a k 0 10 1", DropItem::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "QpfE",
        true,
        4U,
        "QpfE extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "_"),
                DropItem::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "QpfE",
            true,
            index,
            "QpfE invalid i62 field");
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "2lkCB2"),
                DropItem::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "QpfE",
            true,
            index,
            "QpfE positive i62 overflow field");
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "-2lkCB3"),
                DropItem::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "QpfE",
            true,
            index,
            "QpfE negative i62 overflow field");
    }

    const std::string pollutedCode = "QpfE\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " a k 0 10",
            DropItem::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "QpfE polluted code");

    const std::string pollutedField = "QpfE a k 0\x17" "x 10";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, DropItem::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "QpfE",
        true,
        4U,
        "QpfE polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = DropItem(10, 20, 0, 62).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, DropItem::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "QpfE Build wrong code");

    cg::packet::Packet wrongDirection = DropItem(10, 20, 0, 62).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, DropItem::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "QpfE",
        false,
        0U,
        "QpfE Build wrong direction");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = DropItem(10, 20, 0, 62).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            cg::packet::BuildPacket(wrongName, DropItem::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "QpfE",
            true,
            index,
            "QpfE Build wrong field name");

        cg::packet::Packet wrongType = DropItem(10, 20, 0, 62).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            cg::packet::BuildPacket(wrongType, DropItem::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "QpfE",
            true,
            index,
            "QpfE Build wrong field type");

        cg::packet::Packet emptyValue = DropItem(10, 20, 0, 62).ToPacket();
        emptyValue.fields[index].value.clear();
        failures += ExpectError(
            cg::packet::BuildPacket(emptyValue, DropItem::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "QpfE",
            true,
            index,
            "QpfE Build empty decimal");

        cg::packet::Packet invalidValue = DropItem(10, 20, 0, 62).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            cg::packet::BuildPacket(invalidValue, DropItem::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "QpfE",
            true,
            index,
            "QpfE Build non-decimal");

        cg::packet::Packet positiveOverflow =
            DropItem(10, 20, 0, 62).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            cg::packet::BuildPacket(positiveOverflow, DropItem::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "QpfE",
            true,
            index,
            "QpfE Build positive overflow");

        cg::packet::Packet negativeOverflow =
            DropItem(10, 20, 0, 62).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            cg::packet::BuildPacket(negativeOverflow, DropItem::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "QpfE",
            true,
            index,
            "QpfE Build negative overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndRoundtrip()
        + TestWhoValues()
        + TestArbitraryAndExtremeValues()
        + TestParseErrors()
        + TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " drop item packet test(s) failed\n";
        return 1;
    }
    return 0;
}
