#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

using StallStart = cg::packet::send::StallStart;

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
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
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
    cg::packet::Packet packet(
        cg::packet::PacketCode(StallStart::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::array<int, StallStart::PriceCount> MakeSequentialPrices()
{
    std::array<int, StallStart::PriceCount> prices = StallStart::MakeZeroPrices();
    for (std::size_t index = 0; index < prices.size(); ++index) {
        prices[index] = static_cast<int>(index + 1U);
    }
    return prices;
}

std::string BuildStallList(const std::array<int, StallStart::PriceCount>& prices)
{
    std::string payload;
    for (const int price : prices) {
        payload.append(std::to_string(price));
        payload.push_back('|');
    }
    return payload;
}

std::string BuildStallListWithPrice(
    const std::array<int, StallStart::PriceCount>& prices,
    std::size_t priceIndex,
    std::string_view priceText)
{
    std::string payload;
    for (std::size_t index = 0; index < prices.size(); ++index) {
        if (index == priceIndex) {
            payload.append(priceText);
        } else {
            payload.append(std::to_string(prices[index]));
        }
        payload.push_back('|');
    }
    return payload;
}

std::string EncodeOuter(std::string_view text)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(text);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string BuildPlainYlGn(std::string_view name, std::string_view description, std::string_view stallList)
{
    std::string plain = "ylGN ";
    plain.append(EncodeOuter(name));
    plain.push_back(' ');
    plain.append(EncodeOuter(description));
    plain.push_back(' ');
    plain.append(EncodeOuter(stallList));
    plain.push_back(' ');
    return plain;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectStallStartPacket(
    const cg::packet::Packet& packet,
    std::string_view stallName,
    std::string_view stallDescription,
    std::string_view stallList,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(StallStart::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "stallName", cg::packet::PacketFieldType::EscapedString, stallName, message);
    failures += ExpectField(
        packet,
        1U,
        "stallDescription",
        cg::packet::PacketFieldType::EscapedString,
        stallDescription,
        message);
    failures += ExpectField(packet, 2U, "stallList", cg::packet::PacketFieldType::EscapedString, stallList, message);
    return failures;
}

int ExpectStallStartValue(
    const StallStart& packet,
    std::string_view stallName,
    std::string_view stallDescription,
    const std::array<int, StallStart::PriceCount>& prices,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.stallName == stallName, message);
    failures += Expect(packet.stallDescription == stallDescription, message);
    for (std::size_t index = 0; index < prices.size(); ++index) {
        failures += Expect(packet.prices[index] == prices[index], message);
    }
    return failures;
}

int TestToPacketAndEncode()
{
    const std::array<int, StallStart::PriceCount> prices = MakeSequentialPrices();
    const std::string stallList = BuildStallList(prices);
    const StallStart request("Shop", "Desc", prices);
    int failures = 0;

    const cg::packet::PacketResult<std::string> data = request.BuildData();
    failures += Expect(static_cast<bool>(data), "ylGN BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == stallList, "ylGN BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = request.ToPacket();
    failures += Expect(static_cast<bool>(packet), "ylGN ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectStallStartPacket(packet.Value(), "Shop", "Desc", stallList, "ylGN ToPacket value");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "ylGN BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainYlGn("Shop", "Desc", stallList), "ylGN BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "ylGN Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == BuildPlainYlGn("Shop", "Desc", stallList), "ylGN Encode value");
    }
    return failures;
}

int TestTypedParseRoundtrip()
{
    std::array<int, StallStart::PriceCount> prices = MakeSequentialPrices();
    prices[0] = 1;
    prices[20] = 50000;

    const StallStart request("Shop Name", "Line\nTwo\\Tail", prices);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "ylGN escaped encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<StallStart> parsed = StallStart::Parse(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "ylGN typed parse");
    if (parsed.HasValue()) {
        failures += ExpectStallStartValue(
            parsed.Value(),
            "Shop Name",
            "Line\nTwo\\Tail",
            prices,
            "ylGN typed parse value");
    }
    return failures;
}

int TestEmptyNameDescription()
{
    const std::array<int, StallStart::PriceCount> prices = MakeSequentialPrices();
    const std::string stallList = BuildStallList(prices);
    const StallStart request("", "", prices);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "ylGN empty name desc encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == BuildPlainYlGn("", "", stallList), "ylGN empty name desc value");

    const cg::packet::PacketResult<StallStart> parsed = StallStart::Parse(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "ylGN empty name desc parse");
    if (parsed.HasValue()) {
        failures += ExpectStallStartValue(parsed.Value(), "", "", prices, "ylGN empty name desc parsed value");
    }
    return failures;
}

int TestDefaultZeroPrices()
{
    const StallStart request("Shop", "Desc");
    const cg::packet::PacketResult<std::string> data = request.BuildData();

    int failures = Expect(static_cast<bool>(data), "ylGN zero price BuildData");
    if (!data.HasValue()) {
        return failures;
    }

    const std::array<int, StallStart::PriceCount> prices = StallStart::MakeZeroPrices();
    failures += Expect(data.Value() == BuildStallList(prices), "ylGN zero price payload");

    const cg::packet::PacketResult<StallStart> parsed =
        StallStart::Parse(BuildPlainYlGn("Shop", "Desc", data.Value()));
    failures += Expect(static_cast<bool>(parsed), "ylGN zero price parse");
    if (parsed.HasValue()) {
        failures += ExpectStallStartValue(parsed.Value(), "Shop", "Desc", prices, "ylGN zero price value");
    }
    return failures;
}

int TestParsePacketOuterFields()
{
    const std::array<int, StallStart::PriceCount> prices = MakeSequentialPrices();
    const std::string stallList = BuildStallList(prices);
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainYlGn("Shop", "Desc", stallList));

    int failures = Expect(static_cast<bool>(parsed), "ylGN ParsePacket outer");
    if (parsed.HasValue()) {
        failures += ExpectStallStartPacket(parsed.Value(), "Shop", "Desc", stallList, "ylGN ParsePacket fields");
    }
    return failures;
}

int TestPayloadParseErrors()
{
    int failures = 0;
    std::array<int, StallStart::PriceCount> prices = MakeSequentialPrices();

    std::string missingTail = BuildStallList(prices);
    missingTail.pop_back();
    failures += ExpectError(
        StallStart::Parse(BuildPlainYlGn("Shop", "Desc", missingTail)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ylGN",
        true,
        2U,
        "ylGN missing payload tail");

    std::string shortPayload;
    for (std::size_t index = 0; index < prices.size() - 1U; ++index) {
        shortPayload.append(std::to_string(prices[index]));
        shortPayload.push_back('|');
    }
    failures += ExpectError(
        StallStart::Parse(BuildPlainYlGn("Shop", "Desc", shortPayload)),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ylGN",
        true,
        2U,
        "ylGN short payload");

    std::string longPayload = BuildStallList(prices);
    longPayload.append("26|");
    failures += ExpectError(
        StallStart::Parse(BuildPlainYlGn("Shop", "Desc", longPayload)),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ylGN",
        true,
        2U,
        "ylGN long payload");

    const std::string emptyPricePayload = BuildStallListWithPrice(prices, 4U, "");
    failures += ExpectError(
        StallStart::Parse(BuildPlainYlGn("Shop", "Desc", emptyPricePayload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ylGN",
        true,
        4U,
        "ylGN empty price");

    const std::string badDecimalPayload = BuildStallListWithPrice(prices, 20U, "12a");
    failures += ExpectError(
        StallStart::Parse(BuildPlainYlGn("Shop", "Desc", badDecimalPayload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ylGN",
        true,
        20U,
        "ylGN bad decimal price");

    const std::string signedPayload = BuildStallListWithPrice(prices, 20U, "-1");
    failures += ExpectError(
        StallStart::Parse(BuildPlainYlGn("Shop", "Desc", signedPayload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ylGN",
        true,
        20U,
        "ylGN signed price rejected");
    return failures;
}

int TestBuildDataErrors()
{
    int failures = 0;
    std::array<int, StallStart::PriceCount> prices = MakeSequentialPrices();

    prices[2] = -1;
    failures += ExpectError(
        StallStart("Shop", "Desc", prices).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ylGN",
        true,
        2U,
        "ylGN BuildData bad price");
    return failures;
}

int TestBuildPacketErrors()
{
    const std::array<int, StallStart::PriceCount> prices = MakeSequentialPrices();
    const std::string stallList = BuildStallList(prices);
    int failures = 0;

    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("stallName", cg::packet::PacketFieldType::EscapedString, "Shop"),
                Field("stallDescription", cg::packet::PacketFieldType::EscapedString, "Desc"),
                Field("stallList", cg::packet::PacketFieldType::EscapedString, stallList),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ylGN",
        false,
        0U,
        "ylGN build wrong direction");

    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("stallName", cg::packet::PacketFieldType::EscapedString, "Shop"),
                Field("description", cg::packet::PacketFieldType::EscapedString, "Desc"),
                Field("stallList", cg::packet::PacketFieldType::EscapedString, stallList),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ylGN",
        true,
        1U,
        "ylGN build wrong field name");

    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("stallName", cg::packet::PacketFieldType::EscapedString, "Shop"),
                Field("stallDescription", cg::packet::PacketFieldType::EscapedString, "Desc"),
                Field("stallList", cg::packet::PacketFieldType::RawString, stallList),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ylGN",
        true,
        2U,
        "ylGN build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestTypedParseRoundtrip();
    failures += TestEmptyNameDescription();
    failures += TestDefaultZeroPrices();
    failures += TestParsePacketOuterFields();
    failures += TestPayloadParseErrors();
    failures += TestBuildDataErrors();
    failures += TestBuildPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
