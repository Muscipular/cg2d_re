#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::TechUseRequest;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string text;
    text.reserve(values.size());
    for (const unsigned int value : values) {
        text.push_back(static_cast<char>(value));
    }
    return text;
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
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

std::string BuildPlain(
    const std::array<std::string, 3U>& integerTokens,
    std::string_view dataToken)
{
    std::string plain = "sM ";
    plain.append(integerTokens[0]);
    plain.push_back(' ');
    plain.append(integerTokens[1]);
    plain.push_back(' ');
    plain.append(integerTokens[2]);
    plain.push_back(' ');
    plain.append(dataToken);
    plain.push_back(' ');
    return plain;
}

int TestSpecAndRecipeEncode()
{
    const TechUseRequest request(10, 11, -1, "100|18|19");
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(TechUseRequest::Code == "sM", "sM code");
    failures += Expect(
        TechUseRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "sM direction");
    failures += Expect(TechUseRequest::Fields.size() == 4U, "sM fields");

    constexpr std::array<std::string_view, 4U> names = {{
        "haveSkillIndex",
        "haveTechIndex",
        "toIndex",
        "data",
    }};
    constexpr std::array<cg::packet::PacketFieldType, 4U> types = {{
        cg::packet::PacketFieldType::Int62,
        cg::packet::PacketFieldType::Int62,
        cg::packet::PacketFieldType::Int62,
        cg::packet::PacketFieldType::EscapedString,
    }};
    for (std::size_t index = 0U; index < names.size(); ++index) {
        failures += Expect(
            TechUseRequest::Fields[index].name == names[index],
            "sM field name");
        failures += Expect(
            TechUseRequest::Fields[index].type == types[index],
            "sM field type");
    }
    failures += Expect(
        TechUseRequest::Spec.code == TechUseRequest::Code,
        "sM Spec code");
    failures += Expect(
        TechUseRequest::Spec.direction == TechUseRequest::Direction,
        "sM Spec direction");
    failures += Expect(
        TechUseRequest::Spec.fieldCount == 4U,
        "sM Spec field count");

    failures += Expect(request.haveSkillIndex == 10, "sM haveSkillIndex member");
    failures += Expect(request.haveTechIndex == 11, "sM haveTechIndex member");
    failures += Expect(request.toIndex == -1, "sM toIndex member");
    failures += Expect(request.data == "100|18|19", "sM data member");
    failures += Expect(
        packet.code == cg::packet::PacketCode("sM"),
        "sM packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "sM packet direction");
    failures += Expect(packet.fields.size() == 4U, "sM packet fields");
    failures += ExpectField(
        packet,
        0U,
        "haveSkillIndex",
        cg::packet::PacketFieldType::Int62,
        "10",
        "sM haveSkillIndex field");
    failures += ExpectField(
        packet,
        1U,
        "haveTechIndex",
        cg::packet::PacketFieldType::Int62,
        "11",
        "sM haveTechIndex field");
    failures += ExpectField(
        packet,
        2U,
        "toIndex",
        cg::packet::PacketFieldType::Int62,
        "-1",
        "sM toIndex field");
    failures += ExpectField(
        packet,
        3U,
        "data",
        cg::packet::PacketFieldType::EscapedString,
        "100|18|19",
        "sM data field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "sM Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "sM a b -1 100|18|19 ",
            "sM recipe canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "sM a b -1 100|18|19",
            TechUseRequest::Spec);

    int failures = Expect(parsed.HasValue(), "sM generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(
        parsed.Value(),
        0U,
        "haveSkillIndex",
        cg::packet::PacketFieldType::Int62,
        "10",
        "sM parsed haveSkillIndex");
    failures += ExpectField(
        parsed.Value(),
        1U,
        "haveTechIndex",
        cg::packet::PacketFieldType::Int62,
        "11",
        "sM parsed haveTechIndex");
    failures += ExpectField(
        parsed.Value(),
        2U,
        "toIndex",
        cg::packet::PacketFieldType::Int62,
        "-1",
        "sM parsed toIndex");
    failures += ExpectField(
        parsed.Value(),
        3U,
        "data",
        cg::packet::PacketFieldType::EscapedString,
        "100|18|19",
        "sM parsed data");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), TechUseRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "sM generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "sM a b -1 100|18|19 ",
            "sM generic canonical");
    }
    return failures;
}

int TestOpaqueData()
{
    int failures = 0;

    const TechUseRequest empty(10, 11, -1, "");
    const cg::packet::PacketResult<std::string> emptyEncoded = empty.Encode();
    failures += Expect(emptyEncoded.HasValue(), "sM empty data encode");
    if (emptyEncoded.HasValue()) {
        failures += Expect(
            emptyEncoded.Value() == "sM a b -1  ",
            "sM empty data canonical");
    }

    const std::string freeText = "A B\nC\rD\\E";
    const TechUseRequest freeTextRequest(10, 11, -1, freeText);
    const cg::packet::PacketResult<std::string> freeTextEncoded =
        freeTextRequest.Encode();
    failures += Expect(freeTextEncoded.HasValue(), "sM free text encode");
    if (freeTextEncoded.HasValue()) {
        failures += Expect(
            freeTextEncoded.Value() == "sM a b -1 A\\SB\\nC\\rD\\\\E ",
            "sM free text canonical");
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(
                freeTextEncoded.Value(),
                TechUseRequest::Spec);
        failures += Expect(parsed.HasValue(), "sM free text parse");
        if (parsed.HasValue()) {
            failures += ExpectField(
                parsed.Value(),
                3U,
                "data",
                cg::packet::PacketFieldType::EscapedString,
                freeText,
                "sM free text roundtrip");
        }
    }

    constexpr std::array<std::string_view, 2U> opaquePayloads = {{
        "100|18|19",
        "18|301",
    }};
    for (const std::string_view payload : opaquePayloads) {
        const TechUseRequest request(10, 11, -1, std::string(payload));
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            3U,
            "data",
            cg::packet::PacketFieldType::EscapedString,
            payload,
            "sM pipe payload remains opaque");
        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "sM pipe payload encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() ==
                    "sM a b -1 " + std::string(payload) + " ",
                "sM pipe payload canonical");
        }
    }

    const std::string gbkTrailPipe = Bytes({0x81U, 0x7CU});
    const TechUseRequest gbkRequest(10, 11, -1, gbkTrailPipe);
    const cg::packet::PacketResult<std::string> gbkEncoded =
        gbkRequest.Encode();
    failures += Expect(gbkEncoded.HasValue(), "sM GBK trail pipe encode");
    if (gbkEncoded.HasValue()) {
        std::string expected = "sM a b -1 ";
        expected.append(gbkTrailPipe);
        expected.push_back(' ');
        failures += Expect(
            gbkEncoded.Value() == expected,
            "sM GBK trail pipe canonical");
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(expected, TechUseRequest::Spec);
        failures += Expect(parsed.HasValue(), "sM GBK trail pipe parse");
        if (parsed.HasValue()) {
            failures += ExpectField(
                parsed.Value(),
                3U,
                "data",
                cg::packet::PacketFieldType::EscapedString,
                gbkTrailPipe,
                "sM GBK trail pipe roundtrip");
        }
    }
    return failures;
}

int TestSignedIntegers()
{
    struct Case
    {
        int haveSkillIndex;
        int haveTechIndex;
        int toIndex;
        std::string_view expected;
    };

    // INT_MIN/INT_MAX 仅证明 cg3a codec 保留完整 signed int。
    constexpr Case Cases[] = {
        {0, 0, 0, "sM 0 0 0 data "},
        {-1, -2, -3, "sM -1 -2 -3 data "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            -1,
            "sM -2lkCB2 2lkCB1 -1 data ",
        },
        {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "sM 2lkCB1 -2lkCB2 2lkCB1 data ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const TechUseRequest request(
            testCase.haveSkillIndex,
            testCase.haveTechIndex,
            testCase.toIndex,
            "data");
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "haveSkillIndex",
            cg::packet::PacketFieldType::Int62,
            std::to_string(testCase.haveSkillIndex),
            "sM signed haveSkillIndex");
        failures += ExpectField(
            packet,
            1U,
            "haveTechIndex",
            cg::packet::PacketFieldType::Int62,
            std::to_string(testCase.haveTechIndex),
            "sM signed haveTechIndex");
        failures += ExpectField(
            packet,
            2U,
            "toIndex",
            cg::packet::PacketFieldType::Int62,
            std::to_string(testCase.toIndex),
            "sM signed toIndex");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "sM signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "sM signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad a b -1 data ",
            TechUseRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "sM wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("sM", TechUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sM",
        true,
        0U,
        "sM missing all fields");
    failures += ExpectError(
        cg::packet::ParsePacket("sM a", TechUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sM",
        true,
        1U,
        "sM missing haveTechIndex");
    failures += ExpectError(
        cg::packet::ParsePacket("sM a b", TechUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sM",
        true,
        2U,
        "sM missing toIndex");
    failures += ExpectError(
        cg::packet::ParsePacket("sM a b -1", TechUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sM",
        true,
        3U,
        "sM missing data");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "sM a b -1 data extra ",
            TechUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sM",
        true,
        4U,
        "sM extra field");

    const std::array<std::string, 3U> validTokens = {{"a", "b", "-1"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 3U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens, "data"),
                TechUseRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sM",
            true,
            index,
            "sM invalid i62");

        std::array<std::string, 3U> positiveOverflowTokens = validTokens;
        positiveOverflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(positiveOverflowTokens, "data"),
                TechUseRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sM",
            true,
            index,
            "sM positive i62 overflow");

        std::array<std::string, 3U> negativeOverflowTokens = validTokens;
        negativeOverflowTokens[index] = "-2lkCB3";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(negativeOverflowTokens, "data"),
                TechUseRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sM",
            true,
            index,
            "sM negative i62 overflow");
    }

    failures += ExpectError(
        cg::packet::ParsePacket(
            BuildPlain(validTokens, Bytes({0x81U})),
            TechUseRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sM",
        true,
        3U,
        "sM truncated escape");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndRecipeEncode()
        + TestGenericRoundtrip()
        + TestOpaqueData()
        + TestSignedIntegers()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " tech use request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
