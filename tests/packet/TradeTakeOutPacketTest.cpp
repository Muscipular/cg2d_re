#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using TradeTakeOut = cg::packet::send::TradeTakeOut;
using TradeTakeOutSelection = cg::packet::send::TradeTakeOutSelection;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, TradeTakeOut::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, TradeTakeOut::Spec);
}

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
        failures += Expect(error.packetCode->View() == TradeTakeOut::Code, message);
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

cg::packet::PacketField Field(
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::PacketField field;
    field.name = std::string(name);
    field.type = type;
    field.value = std::move(value);
    return field;
}

cg::packet::Packet PacketOf(
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(cg::packet::PacketCode(TradeTakeOut::Code), direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(
    std::string_view itemToken,
    std::string_view petToken,
    std::string_view goldToken)
{
    std::string plain = "aiAb ";
    plain.append(itemToken);
    plain.push_back(' ');
    plain.append(petToken);
    plain.push_back(' ');
    plain.append(goldToken);
    plain.push_back(' ');
    return plain;
}

std::string MakePairs(std::size_t count)
{
    std::string payload;
    for (std::size_t index = 0U; index < count; ++index) {
        if (!payload.empty()) {
            payload.push_back('|');
        }
        payload.append(std::to_string(index));
        payload.push_back('|');
        payload.append(std::to_string(count - index - 1U));
    }
    return payload;
}

int ExpectSelections(
    const std::vector<TradeTakeOutSelection>& actual,
    std::initializer_list<TradeTakeOutSelection> expected,
    const char* message)
{
    int failures = Expect(actual.size() == expected.size(), message);
    if (actual.size() != expected.size()) {
        return failures;
    }

    std::size_t index = 0U;
    for (const TradeTakeOutSelection& value : expected) {
        failures += Expect(actual[index].sourceIndex == value.sourceIndex, message);
        failures += Expect(actual[index].tradeWindowIndex == value.tradeWindowIndex, message);
        ++index;
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    int failures = 0;
    failures += Expect(TradeTakeOut::Spec.fieldCount == 3U, "aiAb Spec exact3");
    failures += Expect(
        TradeTakeOut::Fields[0].type == cg::packet::PacketFieldType::SubPayload,
        "aiAb item Spec SubPayload");
    failures += Expect(
        TradeTakeOut::Fields[1].type == cg::packet::PacketFieldType::SubPayload,
        "aiAb pet Spec SubPayload");
    failures += Expect(
        TradeTakeOut::Fields[2].type == cg::packet::PacketFieldType::Int62,
        "aiAb gold Spec Int62");

    const TradeTakeOut request({{8, 0}, {9, 1}}, {{0, 0}}, 10);
    const cg::packet::PacketResult<cg::packet::Packet> packet = request.ToPacket();
    failures += Expect(static_cast<bool>(packet), "aiAb ToPacket");
    if (packet.HasValue()) {
        failures += Expect(packet.Value().fields.size() == 3U, "aiAb packet exact3");
        if (packet.Value().fields.size() == 3U) {
            failures += Expect(packet.Value().fields[0].name == "item", "aiAb item name");
            failures += Expect(
                packet.Value().fields[0].type == cg::packet::PacketFieldType::SubPayload,
                "aiAb item packet SubPayload");
            failures += Expect(packet.Value().fields[0].value == "8|0|9|1", "aiAb item payload");
            failures += Expect(packet.Value().fields[1].name == "pet", "aiAb pet name");
            failures += Expect(
                packet.Value().fields[1].type == cg::packet::PacketFieldType::SubPayload,
                "aiAb pet packet SubPayload");
            failures += Expect(packet.Value().fields[1].value == "0|0", "aiAb pet payload");
            failures += Expect(packet.Value().fields[2].name == "gold", "aiAb gold name");
            failures += Expect(
                packet.Value().fields[2].type == cg::packet::PacketFieldType::Int62,
                "aiAb gold packet Int62");
            failures += Expect(packet.Value().fields[2].value == "10", "aiAb gold decimal");
        }

        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "aiAb generic build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == "aiAb 8|0|9|1 0|0 a ", "aiAb generic canonical");
        }
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "aiAb Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "aiAb 8|0|9|1 0|0 a ", "aiAb Encode canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        ParsePacket("aiAb 8|0|9|1 0|0 a ");
    failures += Expect(static_cast<bool>(generic), "aiAb generic parse");
    if (generic.HasValue()) {
        failures += Expect(
            generic.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "aiAb parsed item runtime es");
        failures += Expect(
            generic.Value().fields[1].type == cg::packet::PacketFieldType::EscapedString,
            "aiAb parsed pet runtime es");
        failures += Expect(generic.Value().fields[0].value == "8|0|9|1", "aiAb parsed item value");
        failures += Expect(generic.Value().fields[1].value == "0|0", "aiAb parsed pet value");
    }

    const cg::packet::PacketResult<TradeTakeOut> parsed =
        TradeTakeOut::Parse("aiAb 8|0|9|1 0|0 a ");
    failures += Expect(static_cast<bool>(parsed), "aiAb typed parse");
    if (parsed.HasValue()) {
        failures += ExpectSelections(parsed.Value().items, {{8, 0}, {9, 1}}, "aiAb typed items");
        failures += ExpectSelections(parsed.Value().pets, {{0, 0}}, "aiAb typed pets");
        failures += Expect(parsed.Value().gold == 10, "aiAb typed gold");
    }
    return failures;
}

int TestEmptyAndLimits()
{
    int failures = 0;
    const TradeTakeOut empty({}, {}, 0);
    const cg::packet::PacketResult<std::string> emptyEncoded = empty.Encode();
    failures += Expect(static_cast<bool>(emptyEncoded), "aiAb empty encode");
    if (emptyEncoded.HasValue()) {
        failures += Expect(emptyEncoded.Value() == "aiAb   0 ", "aiAb empty canonical");
    }

    const cg::packet::PacketResult<TradeTakeOut> emptyParsed = TradeTakeOut::Parse("aiAb   0 ");
    failures += Expect(static_cast<bool>(emptyParsed), "aiAb empty parse");
    if (emptyParsed.HasValue()) {
        failures += Expect(emptyParsed.Value().items.empty(), "aiAb empty items");
        failures += Expect(emptyParsed.Value().pets.empty(), "aiAb empty pets");
    }

    std::vector<TradeTakeOutSelection> items;
    for (std::size_t index = 0U; index < TradeTakeOut::MaxItemCount; ++index) {
        items.push_back({static_cast<int>(index), static_cast<int>(TradeTakeOut::MaxItemCount - index - 1U)});
    }
    std::vector<TradeTakeOutSelection> pets;
    for (std::size_t index = 0U; index < TradeTakeOut::MaxPetCount; ++index) {
        pets.push_back({static_cast<int>(index), static_cast<int>(TradeTakeOut::MaxPetCount - index - 1U)});
    }

    const TradeTakeOut maximum(items, pets, 0);
    const cg::packet::PacketResult<std::string> maximumEncoded = maximum.Encode();
    failures += Expect(static_cast<bool>(maximumEncoded), "aiAb maximum encode");
    if (maximumEncoded.HasValue()) {
        const cg::packet::PacketResult<TradeTakeOut> maximumParsed =
            TradeTakeOut::Parse(maximumEncoded.Value());
        failures += Expect(static_cast<bool>(maximumParsed), "aiAb maximum parse");
        if (maximumParsed.HasValue()) {
            failures += Expect(
                maximumParsed.Value().items.size() == TradeTakeOut::MaxItemCount,
                "aiAb maximum item count");
            failures += Expect(
                maximumParsed.Value().pets.size() == TradeTakeOut::MaxPetCount,
                "aiAb maximum pet count");
        }
    }

    items.push_back({20, 20});
    failures += ExpectError(
        TradeTakeOut(items, {}, 0).Encode(),
        cg::packet::PacketErrorCode::EncodeFailed,
        true,
        0U,
        "aiAb item encode over limit");

    pets.push_back({5, 5});
    failures += ExpectError(
        TradeTakeOut({}, pets, 0).Encode(),
        cg::packet::PacketErrorCode::EncodeFailed,
        true,
        1U,
        "aiAb pet encode over limit");

    failures += ExpectError(
        TradeTakeOut::Parse(BuildRawPlain(MakePairs(21U), "", "0")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        true,
        0U,
        "aiAb item parse over limit");
    failures += ExpectError(
        TradeTakeOut::Parse(BuildRawPlain("", MakePairs(6U), "0")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        true,
        1U,
        "aiAb pet parse over limit");
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    const std::initializer_list<std::string_view> badItems = {
        "8",
        "8|0|9",
        "8|0|",
        "|0",
        "8||0|1",
        "x|0",
        "+8|0",
        "2147483648|0",
        "-2147483649|0",
    };
    for (const std::string_view item : badItems) {
        const cg::packet::PacketResult<TradeTakeOut> parsed =
            TradeTakeOut::Parse(BuildRawPlain(item, "", "0"));
        failures += Expect(!static_cast<bool>(parsed), "aiAb invalid item rejected");
        failures += Expect(parsed.HasError(), "aiAb invalid item has error");
        if (parsed.HasError()) {
            failures += Expect(parsed.Error().packetCode.has_value(), "aiAb invalid item code");
            failures += Expect(parsed.Error().fieldIndex.has_value(), "aiAb invalid item index");
            if (parsed.Error().fieldIndex.has_value()) {
                failures += Expect(*parsed.Error().fieldIndex == 0U, "aiAb invalid item outer index");
            }
        }
    }

    failures += ExpectError(
        TradeTakeOut::Parse(BuildRawPlain("", "0", "0")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        true,
        1U,
        "aiAb legacy single pet rejected");
    failures += ExpectError(
        TradeTakeOut::Parse(BuildRawPlain("", "0|x", "0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        true,
        1U,
        "aiAb invalid pet decimal");
    failures += ExpectError(
        TradeTakeOut::Parse(BuildRawPlain("", "0|2147483648", "0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        true,
        1U,
        "aiAb pet decimal overflow");
    return failures;
}

int TestSignedValuesAndDuplicates()
{
    const std::string minValue = std::to_string(std::numeric_limits<int>::min());
    const std::string maxValue = std::to_string(std::numeric_limits<int>::max());
    const std::string item = minValue + "|" + maxValue + "|-1|-1";
    const std::string pet = "-1|" + maxValue + "|-1|" + maxValue;

    const cg::packet::PacketResult<TradeTakeOut> parsed =
        TradeTakeOut::Parse(BuildRawPlain(item, pet, "0"));
    int failures = Expect(static_cast<bool>(parsed), "aiAb signed parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectSelections(
        parsed.Value().items,
        {{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}, {-1, -1}},
        "aiAb signed items");
    failures += ExpectSelections(
        parsed.Value().pets,
        {{-1, std::numeric_limits<int>::max()}, {-1, std::numeric_limits<int>::max()}},
        "aiAb duplicate pets");

    const cg::packet::PacketResult<std::string> encoded = parsed.Value().Encode();
    failures += Expect(static_cast<bool>(encoded), "aiAb signed encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == BuildRawPlain(item, pet, "0"), "aiAb signed canonical");
    }
    return failures;
}

int TestOuterErrors()
{
    int failures = 0;
    const cg::packet::PacketResult<TradeTakeOut> wrongCode =
        TradeTakeOut::Parse("other 8|0 0|0 0 ");
    failures += Expect(!static_cast<bool>(wrongCode), "aiAb wrong code");
    failures += Expect(wrongCode.HasError(), "aiAb wrong code error");
    if (wrongCode.HasError()) {
        failures += Expect(
            wrongCode.Error().code == cg::packet::PacketErrorCode::InvalidCode,
            "aiAb wrong code kind");
        failures += Expect(wrongCode.Error().packetCode.has_value(), "aiAb wrong code context");
        if (wrongCode.Error().packetCode.has_value()) {
            failures += Expect(wrongCode.Error().packetCode->View() == "other", "aiAb wrong code value");
        }
        failures += Expect(!wrongCode.Error().fieldIndex.has_value(), "aiAb wrong code no field");
    }
    failures += ExpectError(
        TradeTakeOut::Parse("aiAb 8|0 0|0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        true,
        2U,
        "aiAb missing gold");
    failures += ExpectError(
        TradeTakeOut::Parse("aiAb 8|0 0|0 0 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        true,
        3U,
        "aiAb extra field");
    failures += ExpectError(
        TradeTakeOut::Parse(BuildRawPlain("8|0", "0|0", "_")),
        cg::packet::PacketErrorCode::DecodeFailed,
        true,
        2U,
        "aiAb bad gold");
    failures += ExpectError(
        TradeTakeOut::Parse(BuildRawPlain(Bytes({0x81U}), "", "0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        true,
        0U,
        "aiAb malformed item es");
    failures += ExpectError(
        TradeTakeOut::Parse(BuildRawPlain("", Bytes({0x81U}), "0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        true,
        1U,
        "aiAb malformed pet es");

    const cg::packet::Packet validPacket = PacketOf(
        cg::packet::PacketDirection::ClientToServer,
        {
            Field("item", cg::packet::PacketFieldType::EscapedString, "8|0"),
            Field("pet", cg::packet::PacketFieldType::EscapedString, "0|0"),
            Field("gold", cg::packet::PacketFieldType::Int62, "0"),
        });
    failures += Expect(static_cast<bool>(TradeTakeOut::FromPacket(validPacket)), "aiAb manual valid packet");

    cg::packet::Packet wrongDirection = validPacket;
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        TradeTakeOut::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        false,
        0U,
        "aiAb wrong direction");

    cg::packet::Packet wrongName = validPacket;
    wrongName.fields[1].name = "pets";
    failures += ExpectError(
        TradeTakeOut::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        true,
        1U,
        "aiAb wrong field name");

    cg::packet::Packet wrongType = validPacket;
    wrongType.fields[0].type = cg::packet::PacketFieldType::SubPayload;
    failures += ExpectError(
        TradeTakeOut::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        true,
        0U,
        "aiAb wrong runtime field type");

    cg::packet::Packet missingField = validPacket;
    missingField.fields.pop_back();
    failures += ExpectError(
        TradeTakeOut::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        true,
        2U,
        "aiAb manual missing field");

    cg::packet::Packet badGold = validPacket;
    badGold.fields[2].value = "2147483648";
    failures += ExpectError(
        TradeTakeOut::FromPacket(badGold),
        cg::packet::PacketErrorCode::DecodeFailed,
        true,
        2U,
        "aiAb manual gold overflow");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSpecAndRoundtrip();
    failures += TestEmptyAndLimits();
    failures += TestPayloadErrors();
    failures += TestSignedValuesAndDuplicates();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
