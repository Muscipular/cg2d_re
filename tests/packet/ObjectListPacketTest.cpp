#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

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
    return failures;
}

std::string BuildPlainNsY(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "nsY ";
    }

    std::string plain = "nsY ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectObjectListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ObjectList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectRecord(
    const cg::packet::receive::ObjectListRecord& record,
    int objectIndex,
    const std::vector<std::string>& fields,
    const char* message)
{
    int failures = 0;
    failures += Expect(record.objectIndex == objectIndex, message);
    failures += Expect(record.fields.size() == fields.size(), message);
    if (record.fields.size() != fields.size()) {
        return failures;
    }

    for (std::size_t index = 0; index < fields.size(); ++index) {
        failures += Expect(record.fields[index] == fields[index], message);
    }
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "1|20|30|500";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainNsY(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse nsY outer");
    if (parsed.HasValue()) {
        failures += ExpectObjectListPacket(parsed.Value(), payload, "parse nsY outer packet");
    }
    return failures;
}

int TestTypedSingleRecords()
{
    int failures = 0;

    const std::string gold = "1|20|30|500";
    const cg::packet::PacketResult<cg::packet::receive::ObjectList> goldParsed =
        cg::packet::receive::ObjectList::Parse(BuildPlainNsY(gold));
    failures += Expect(static_cast<bool>(goldParsed), "parse nsY gold");
    if (goldParsed.HasValue()) {
        failures += Expect(goldParsed.Value().data == gold, "parse nsY gold data");
        failures += Expect(goldParsed.Value().records.size() == 1U, "parse nsY gold record count");
        if (goldParsed.Value().records.size() == 1U) {
            failures += ExpectRecord(goldParsed.Value().records[0], 1, {"1", "20", "30", "500"}, "parse nsY gold record");
        }
    }

    const std::string item = "2|21|31|1000|4|Potion\\zSmall|remain";
    const cg::packet::PacketResult<cg::packet::receive::ObjectList> itemParsed =
        cg::packet::receive::ObjectList::Parse(BuildPlainNsY(item));
    failures += Expect(static_cast<bool>(itemParsed), "parse nsY item");
    if (itemParsed.HasValue()) {
        failures += Expect(itemParsed.Value().records.size() == 1U, "parse nsY item record count");
        if (itemParsed.Value().records.size() == 1U) {
            failures += ExpectRecord(
                itemParsed.Value().records[0],
                2,
                {"2", "21", "31", "1000", "4", "Potion\\zSmall", "remain"},
                "parse nsY item record");
        }
    }

    const std::string character = "7|3|22|32|4|5000|12|5|Alice|Title\\zRaw|0|1|6|Hero|tw0|tw1|petbit\\zmore";
    const cg::packet::PacketResult<cg::packet::receive::ObjectList> characterParsed =
        cg::packet::receive::ObjectList::Parse(BuildPlainNsY(character));
    failures += Expect(static_cast<bool>(characterParsed), "parse nsY character");
    if (characterParsed.HasValue()) {
        failures += Expect(characterParsed.Value().records.size() == 1U, "parse nsY character record count");
        if (characterParsed.Value().records.size() == 1U) {
            failures += ExpectRecord(
                characterParsed.Value().records[0],
                3,
                {
                    "7",
                    "3",
                    "22",
                    "32",
                    "4",
                    "5000",
                    "12",
                    "5",
                    "Alice",
                    "Title\\zRaw",
                    "0",
                    "1",
                    "6",
                    "Hero",
                    "tw0",
                    "tw1",
                    "petbit\\zmore",
                },
                "parse nsY character record");
        }
    }

    return failures;
}

int TestMultiRecordRoundTrip()
{
    const std::string payload =
        "1|20|30|500\\t"
        "2|21|31|1000|4|Potion|remain\\t"
        "7|3|22|32|4|5000|12|5|Alice|Title|0|1|6|Hero|tw0|tw1|petbit";

    const cg::packet::PacketResult<cg::packet::receive::ObjectList> parsed =
        cg::packet::receive::ObjectList::Parse(BuildPlainNsY(payload));
    int failures = Expect(static_cast<bool>(parsed), "parse nsY multi record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 3U, "parse nsY multi record count");
    failures += Expect(parsed.Value().BuildData() == payload, "build nsY payload data");

    const cg::packet::Packet packet = parsed.Value().ToPacket();
    failures += ExpectObjectListPacket(packet, payload, "nsY ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build nsY packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::ObjectList> parsedAgain =
        cg::packet::receive::ObjectList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsedAgain), "parse nsY built packet");
    if (parsedAgain.HasValue()) {
        failures += Expect(parsedAgain.Value().BuildData() == payload, "roundtrip nsY payload data");
        failures += Expect(parsedAgain.Value().records.size() == 3U, "roundtrip nsY record count");
    }
    return failures;
}

int TestPayloadZPreserved()
{
    const std::string payload = "2|21|31|1000|4|Potion\\zSmall|remain";
    const cg::packet::PacketResult<cg::packet::receive::ObjectList> parsed =
        cg::packet::receive::ObjectList::Parse(BuildPlainNsY(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse nsY z payload");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "parse nsY z record count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].fields[5] == "Potion\\zSmall", "preserve nsY z field");
    }

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build nsY z payload");
    if (built.HasValue()) {
        const cg::packet::PacketResult<cg::packet::receive::ObjectList> parsedAgain =
            cg::packet::receive::ObjectList::Parse(built.Value());
        failures += Expect(static_cast<bool>(parsedAgain), "parse rebuilt nsY z payload");
        if (parsedAgain.HasValue()) {
            failures += Expect(parsedAgain.Value().BuildData() == payload, "roundtrip nsY z payload");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("nsY"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "nsY missing data");
    failures += ExpectError(
        cg::packet::receive::ObjectList::Parse("nsY"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "typed nsY missing data");
    failures += ExpectError(
        cg::packet::ParsePacket("nsY one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "nsY extra data field");
    failures += ExpectError(
        cg::packet::receive::ObjectList::Parse("nsY one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "typed nsY extra data field");
    failures += ExpectError(
        cg::packet::receive::ObjectList::Parse(BuildPlainNsY("1|2|3|4|5")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "typed nsY bad record field count");
    failures += ExpectError(
        cg::packet::receive::ObjectList::Parse(BuildPlainNsY("_|2|3|4")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nsY",
        "typed nsY bad object index");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedSingleRecords();
    failures += TestMultiRecordRoundTrip();
    failures += TestPayloadZPreserved();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
