#include "packet/Packet.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using ObjectList = cg::packet::receive::ObjectList;
using ObjectListRecord = cg::packet::receive::ObjectListRecord;

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
    int failures = Expect(!result.HasValue(), message);
    failures += Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().code == code, message);
    failures += Expect(result.Error().packetCode.has_value(), message);
    if (result.Error().packetCode.has_value()) {
        failures += Expect(result.Error().packetCode->View() == packetCode, message);
    }
    return failures;
}

template <typename T>
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t recordIndex,
    const char* message)
{
    int failures = ExpectError(result, code, ObjectList::Code, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == recordIndex, message);
    }
    return failures;
}

std::string LegacyDoubleByteText(unsigned char trail, std::string_view suffix)
{
    std::string text;
    text.push_back(static_cast<char>(0x81U));
    text.push_back(static_cast<char>(trail));
    text.append(suffix);
    return text;
}

std::string JoinFields(const std::vector<std::string>& fields)
{
    std::string payload;
    for (std::size_t index = 0U; index < fields.size(); ++index) {
        if (index != 0U) {
            payload.push_back('|');
        }
        payload.append(fields[index]);
    }
    return payload;
}

std::string JoinRecords(const std::vector<std::string>& records)
{
    std::string payload;
    for (std::size_t index = 0U; index < records.size(); ++index) {
        if (index != 0U) {
            payload.push_back('\t');
        }
        payload.append(records[index]);
    }
    return payload;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain("nsY ");
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return BuildRawPlain("");
    }
    return BuildRawPlain(encoded.Value());
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plain)
{
    return cg::packet::ParsePacket(plain, ObjectList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, ObjectList::Spec);
}

std::vector<std::string> RoleFields()
{
    return {
        "0",
        "a",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "RoleName",
        "OwnTitle",
        "1",
        "7",
        "8",
        "TitleName",
        "9",
        "10",
        "0",
    };
}

std::vector<std::string> ItemFields()
{
    return {"b", "1", "2", "3", "4", "ItemName", "6"};
}

std::vector<std::string> GoldFields()
{
    return {"c", "1", "2", "3"};
}

int TestSpecAndThreeRecordParse()
{
    const std::string role = JoinFields(RoleFields());
    const std::string item = JoinFields(ItemFields());
    const std::string gold = JoinFields(GoldFields());
    const std::string payload = JoinRecords({role, item, gold});

    int failures = Expect(ObjectList::Spec.fieldCount == 1U, "nsY Spec exact1");
    failures += Expect(
        ObjectList::Fields[0].type == cg::packet::PacketFieldType::SubPayload,
        "nsY Spec data SubPayload");
    failures += Expect(ObjectList::RecordDelimiter == "\t", "nsY actual TAB delimiter");

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        ParsePacket(BuildPlain(payload));
    failures += Expect(generic.HasValue(), "nsY generic parse");
    if (generic.HasValue() && generic.Value().fields.size() == 1U) {
        failures += Expect(
            generic.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "nsY runtime data EscapedString");
    }

    const cg::packet::PacketResult<ObjectList> parsed =
        ObjectList::Parse(BuildPlain(payload));
    failures += Expect(parsed.HasValue(), "nsY three-record Parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 3U, "nsY three records");
    if (parsed.Value().records.size() == 3U) {
        failures += Expect(parsed.Value().records[0].fields.size() == 17U, "nsY role exact17");
        failures += Expect(parsed.Value().records[1].fields.size() == 7U, "nsY item exact7");
        failures += Expect(parsed.Value().records[2].fields.size() == 4U, "nsY gold exact4");
        failures += Expect(parsed.Value().records[0].objectIndex == 10, "nsY role objectIndex field1");
        failures += Expect(parsed.Value().records[1].objectIndex == 11, "nsY item objectIndex field0");
        failures += Expect(parsed.Value().records[2].objectIndex == 12, "nsY gold objectIndex field0");
    }
    return failures;
}

int TestBuildAndOuterRoundtrip()
{
    std::vector<ObjectListRecord> records;
    records.emplace_back(RoleFields(), 10);
    records.emplace_back(ItemFields(), 11);
    records.emplace_back(GoldFields(), 12);
    const ObjectList value(std::move(records));

    const cg::packet::PacketResult<std::string> data = value.BuildData();
    int failures = Expect(data.HasValue(), "nsY BuildData");
    if (!data.HasValue()) {
        return failures;
    }

    failures += Expect(data.Value().find('\t') != std::string::npos, "nsY BuildData actual TAB");
    failures += Expect(!data.Value().ends_with("\t"), "nsY BuildData no trailing TAB");

    const cg::packet::PacketResult<cg::packet::Packet> packet = value.ToPacket();
    failures += Expect(packet.HasValue(), "nsY ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += Expect(packet.Value().fields.size() == 1U, "nsY ToPacket exact1");
    if (packet.Value().fields.size() == 1U) {
        failures += Expect(packet.Value().fields[0].name == "data", "nsY ToPacket data name");
        failures += Expect(
            packet.Value().fields[0].type == cg::packet::PacketFieldType::SubPayload,
            "nsY ToPacket data SubPayload");
        failures += Expect(packet.Value().fields[0].value == data.Value(), "nsY ToPacket data value");
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(built.HasValue(), "nsY generic build");
    if (built.HasValue()) {
        const cg::packet::PacketResult<ObjectList> parsed = ObjectList::Parse(built.Value());
        failures += Expect(parsed.HasValue(), "nsY outer roundtrip");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().records.size() == 3U, "nsY outer record count");
        }
    }
    return failures;
}

int TestRoleLegacyAndOpaqueSuffix()
{
    const std::string semanticName =
        LegacyDoubleByteText(0x7CU, "A|B,C\\D\nE");
    const std::string semanticOwnTitle =
        LegacyDoubleByteText(0x5CU, "Own|Guild");
    std::vector<std::string> role = RoleFields();
    role[8] = semanticName;
    role[9] = semanticOwnTitle;
    role[13] = "Title|Name";
    role.push_back("opaque\\zsuffix\\ttext");

    const ObjectList value({ObjectListRecord(role, 10)});
    const cg::packet::PacketResult<std::string> data = value.BuildData();
    int failures = Expect(data.HasValue(), "nsY role semantic BuildData");
    if (!data.HasValue()) {
        return failures;
    }

    failures += Expect(data.Value().find("\\z") != std::string::npos, "nsY role pipe encoded");
    failures += Expect(data.Value().find("\\c") != std::string::npos, "nsY role comma encoded");
    failures += Expect(data.Value().find("\\y") != std::string::npos, "nsY role slash encoded");
    failures += Expect(data.Value().find("\\n") != std::string::npos, "nsY role newline encoded");
    failures += Expect(data.Value().find("\\t") != std::string::npos, "nsY literal slash-t preserved");

    const cg::packet::PacketResult<ObjectList> parsed =
        ObjectList::Parse(BuildPlain(data.Value()));
    failures += Expect(parsed.HasValue(), "nsY role semantic Parse");
    if (parsed.HasValue() && parsed.Value().records.size() == 1U) {
        const std::vector<std::string>& fields = parsed.Value().records[0].fields;
        failures += Expect(fields.size() == 18U, "nsY role opaque suffix retained");
        failures += Expect(fields[8] == semanticName, "nsY role name semantic");
        failures += Expect(fields[9] == semanticOwnTitle, "nsY role ownTitle semantic");
        failures += Expect(fields[13] == "Title|Name", "nsY role title semantic");
        failures += Expect(fields[17] == "opaque\\zsuffix\\ttext", "nsY role suffix stays raw");
    }
    return failures;
}

int TestItemRawSemanticAsymmetry()
{
    const std::string rawName =
        LegacyDoubleByteText(0x7CU, "A\\zB") +
        LegacyDoubleByteText(0x5CU, "C");
    std::vector<std::string> item = ItemFields();
    item[5] = rawName;
    const ObjectList value({ObjectListRecord(item, 11)});

    const cg::packet::PacketResult<std::string> data = value.BuildData();
    int failures = Expect(data.HasValue(), "nsY item Server raw BuildData");
    if (!data.HasValue()) {
        return failures;
    }
    failures += Expect(data.Value().find("\\z") != std::string::npos, "nsY item raw token preserved");

    const cg::packet::PacketResult<ObjectList> parsed =
        ObjectList::Parse(BuildPlain(data.Value()));
    failures += Expect(parsed.HasValue(), "nsY item raw Parse");
    if (!parsed.HasValue() || parsed.Value().records.size() != 1U) {
        return failures;
    }

    const std::string expectedSemantic =
        LegacyDoubleByteText(0x7CU, "A|B") +
        LegacyDoubleByteText(0x5CU, "C");
    failures += Expect(
        parsed.Value().records[0].fields[5] == expectedSemantic,
        "nsY item name decodes once");
    failures += ExpectFieldError(
        parsed.Value().BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "nsY parsed item literal pipe cannot raw BuildData");
    return failures;
}

int TestStrictRecordGrammar()
{
    int failures = ExpectError(
        ObjectList::Parse(BuildPlain("")),
        cg::packet::PacketErrorCode::EmptyInput,
        "nsY",
        "nsY empty payload");
    failures += ExpectError(
        ObjectList::Parse(BuildPlain("\t" + JoinFields(GoldFields()))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "nsY leading TAB");
    failures += ExpectError(
        ObjectList::Parse(BuildPlain(JoinFields(GoldFields()) + "\t\t" + JoinFields(ItemFields()))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "nsY consecutive TAB");
    failures += ExpectError(
        ObjectList::Parse(BuildPlain(JoinFields(GoldFields()) + "\t")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "nsY trailing TAB");
    failures += ExpectError(
        ObjectList(std::vector<ObjectListRecord> {}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "nsY",
        "nsY empty records BuildData");
    return failures;
}

int TestShapeAndBranchErrors()
{
    int failures = 0;
    for (std::size_t fieldCount : {3U, 5U, 6U, 8U, 16U}) {
        std::vector<std::string> fields(fieldCount, "1");
        failures += ExpectError(
            ObjectList::Parse(BuildPlain(JoinFields(fields))),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "nsY",
            "nsY rejects noncanonical record shape");
    }

    std::vector<std::string> role = RoleFields();
    role[10].clear();
    failures += ExpectError(
        ObjectList::Parse(BuildPlain(JoinFields(role))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "nsY role predicate required");

    std::vector<std::string> item = ItemFields();
    item[5].clear();
    failures += ExpectError(
        ObjectList::Parse(BuildPlain(JoinFields(item))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "nsY item predicate required");

    std::vector<std::string> gold = GoldFields();
    gold[3].clear();
    failures += ExpectError(
        ObjectList::Parse(BuildPlain(JoinFields(gold))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nsY",
        "nsY gold predicate required");
    return failures;
}

int TestBuildAndDecodeErrors()
{
    std::vector<std::string> badDelimiter = GoldFields();
    badDelimiter[2] = "bad|field";
    int failures = ExpectFieldError(
        ObjectList({ObjectListRecord(std::move(badDelimiter), 12)}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "nsY BuildData literal pipe");

    std::vector<std::string> badTab = GoldFields();
    badTab[2] = "bad\tfield";
    failures += ExpectFieldError(
        ObjectList({ObjectListRecord(std::move(badTab), 12)}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "nsY BuildData literal TAB");

    std::vector<std::string> truncated = GoldFields();
    truncated[2].assign(1U, static_cast<char>(0x81U));
    failures += ExpectFieldError(
        ObjectList({ObjectListRecord(std::move(truncated), 12)}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "nsY BuildData truncated GBK");

    std::vector<std::string> badRoleText = RoleFields();
    badRoleText[8] = "Bad\\";
    failures += ExpectFieldError(
        ObjectList::Parse(BuildPlain(JoinFields(badRoleText))),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "nsY Parse truncated role legacy");

    std::vector<std::string> badIndex = GoldFields();
    badIndex[0] = "_";
    failures += ExpectFieldError(
        ObjectList::Parse(BuildPlain(JoinFields(badIndex))),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "nsY invalid objectIndex i62");
    return failures;
}

int TestOuterErrors()
{
    int failures = ExpectError(
        ObjectList::Parse("NI data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "NI",
        "nsY wrong code");
    failures += Expect(!ObjectList::Parse("nsY").HasValue(), "nsY missing data");
    failures += Expect(!ObjectList::Parse("nsY one two ").HasValue(), "nsY extra outer field");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSpecAndThreeRecordParse();
    failures += TestBuildAndOuterRoundtrip();
    failures += TestRoleLegacyAndOpaqueSuffix();
    failures += TestItemRawSemanticAsymmetry();
    failures += TestStrictRecordGrammar();
    failures += TestShapeAndBranchErrors();
    failures += TestBuildAndDecodeErrors();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " nsY expectations failed\n";
        return 1;
    }
    return 0;
}
