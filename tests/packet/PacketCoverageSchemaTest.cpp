#include "packet/Receive.hpp"
#include "packet/Send.hpp"

#include <charconv>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

struct JsonValue
{
    enum class Kind
    {
        Null,
        Bool,
        Number,
        String,
        Array,
        Object,
    };

    Kind kind = Kind::Null;
    bool boolValue = false;
    std::string text;
    std::vector<JsonValue> array;
    std::map<std::string, JsonValue> object;

    [[nodiscard]] const JsonValue* Find(std::string_view key) const
    {
        const auto it = object.find(std::string(key));
        if (it == object.end()) {
            return nullptr;
        }
        return &it->second;
    }
};

class JsonParser
{
public:
    explicit JsonParser(std::string_view text)
        : text_(text)
    {
    }

    [[nodiscard]] bool Parse(JsonValue& value)
    {
        SkipWhitespace();
        if (!ParseValue(value)) {
            return false;
        }
        SkipWhitespace();
        return pos_ == text_.size();
    }

    [[nodiscard]] std::string_view Error() const noexcept
    {
        return error_;
    }

private:
    [[nodiscard]] bool ParseValue(JsonValue& value)
    {
        SkipWhitespace();
        if (pos_ >= text_.size()) {
            return Fail("JSON 值缺失");
        }

        const char ch = text_[pos_];
        if (ch == '"') {
            value.kind = JsonValue::Kind::String;
            return ParseString(value.text);
        }
        if (ch == '{') {
            return ParseObject(value);
        }
        if (ch == '[') {
            return ParseArray(value);
        }
        if (StartsWith("true")) {
            pos_ += 4;
            value.kind = JsonValue::Kind::Bool;
            value.boolValue = true;
            return true;
        }
        if (StartsWith("false")) {
            pos_ += 5;
            value.kind = JsonValue::Kind::Bool;
            value.boolValue = false;
            return true;
        }
        if (StartsWith("null")) {
            pos_ += 4;
            value.kind = JsonValue::Kind::Null;
            return true;
        }
        if (ch == '-' || (ch >= '0' && ch <= '9')) {
            value.kind = JsonValue::Kind::Number;
            return ParseNumber(value.text);
        }
        return Fail("JSON 值类型未知");
    }

    [[nodiscard]] bool ParseObject(JsonValue& value)
    {
        value.kind = JsonValue::Kind::Object;
        ++pos_;
        SkipWhitespace();
        if (Consume('}')) {
            return true;
        }

        while (pos_ < text_.size()) {
            std::string key;
            if (!ParseString(key)) {
                return false;
            }
            SkipWhitespace();
            if (!Consume(':')) {
                return Fail("JSON object 缺少冒号");
            }
            JsonValue child;
            if (!ParseValue(child)) {
                return false;
            }
            value.object.emplace(std::move(key), std::move(child));
            SkipWhitespace();
            if (Consume('}')) {
                return true;
            }
            if (!Consume(',')) {
                return Fail("JSON object 缺少逗号");
            }
            SkipWhitespace();
        }
        return Fail("JSON object 未闭合");
    }

    [[nodiscard]] bool ParseArray(JsonValue& value)
    {
        value.kind = JsonValue::Kind::Array;
        ++pos_;
        SkipWhitespace();
        if (Consume(']')) {
            return true;
        }

        while (pos_ < text_.size()) {
            JsonValue child;
            if (!ParseValue(child)) {
                return false;
            }
            value.array.push_back(std::move(child));
            SkipWhitespace();
            if (Consume(']')) {
                return true;
            }
            if (!Consume(',')) {
                return Fail("JSON array 缺少逗号");
            }
            SkipWhitespace();
        }
        return Fail("JSON array 未闭合");
    }

    [[nodiscard]] bool ParseString(std::string& value)
    {
        if (!Consume('"')) {
            return Fail("JSON string 缺少引号");
        }

        value.clear();
        while (pos_ < text_.size()) {
            const char ch = text_[pos_++];
            if (ch == '"') {
                return true;
            }
            if (ch != '\\') {
                value.push_back(ch);
                continue;
            }
            if (pos_ >= text_.size()) {
                return Fail("JSON string 转义未完成");
            }
            const char escaped = text_[pos_++];
            switch (escaped) {
            case '"':
            case '\\':
            case '/':
                value.push_back(escaped);
                break;
            case 'b':
                value.push_back('\b');
                break;
            case 'f':
                value.push_back('\f');
                break;
            case 'n':
                value.push_back('\n');
                break;
            case 'r':
                value.push_back('\r');
                break;
            case 't':
                value.push_back('\t');
                break;
            case 'u':
                if (!ParseUnicodeEscape(value)) {
                    return false;
                }
                break;
            default:
                return Fail("JSON string 转义未知");
            }
        }
        return Fail("JSON string 未闭合");
    }

    [[nodiscard]] bool ParseUnicodeEscape(std::string& value)
    {
        if (pos_ + 4 > text_.size()) {
            return Fail("JSON unicode 转义太短");
        }
        unsigned int codepoint = 0;
        for (int i = 0; i < 4; ++i) {
            const char ch = text_[pos_++];
            codepoint <<= 4U;
            if (ch >= '0' && ch <= '9') {
                codepoint += static_cast<unsigned int>(ch - '0');
            } else if (ch >= 'a' && ch <= 'f') {
                codepoint += static_cast<unsigned int>(ch - 'a' + 10);
            } else if (ch >= 'A' && ch <= 'F') {
                codepoint += static_cast<unsigned int>(ch - 'A' + 10);
            } else {
                return Fail("JSON unicode 转义非法");
            }
        }
        AppendUtf8(value, codepoint);
        return true;
    }

    static void AppendUtf8(std::string& value, unsigned int codepoint)
    {
        if (codepoint <= 0x7FU) {
            value.push_back(static_cast<char>(codepoint));
        } else if (codepoint <= 0x7FFU) {
            value.push_back(static_cast<char>(0xC0U | (codepoint >> 6U)));
            value.push_back(static_cast<char>(0x80U | (codepoint & 0x3FU)));
        } else {
            value.push_back(static_cast<char>(0xE0U | (codepoint >> 12U)));
            value.push_back(static_cast<char>(0x80U | ((codepoint >> 6U) & 0x3FU)));
            value.push_back(static_cast<char>(0x80U | (codepoint & 0x3FU)));
        }
    }

    [[nodiscard]] bool ParseNumber(std::string& value)
    {
        const std::size_t begin = pos_;
        if (text_[pos_] == '-') {
            ++pos_;
        }
        while (pos_ < text_.size() && text_[pos_] >= '0' && text_[pos_] <= '9') {
            ++pos_;
        }
        if (pos_ < text_.size() && text_[pos_] == '.') {
            ++pos_;
            while (pos_ < text_.size() && text_[pos_] >= '0' && text_[pos_] <= '9') {
                ++pos_;
            }
        }
        if (pos_ < text_.size() && (text_[pos_] == 'e' || text_[pos_] == 'E')) {
            ++pos_;
            if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) {
                ++pos_;
            }
            while (pos_ < text_.size() && text_[pos_] >= '0' && text_[pos_] <= '9') {
                ++pos_;
            }
        }
        value.assign(text_.substr(begin, pos_ - begin));
        return !value.empty();
    }

    void SkipWhitespace()
    {
        while (pos_ < text_.size()) {
            const char ch = text_[pos_];
            if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t') {
                return;
            }
            ++pos_;
        }
    }

    [[nodiscard]] bool Consume(char ch)
    {
        if (pos_ < text_.size() && text_[pos_] == ch) {
            ++pos_;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool StartsWith(std::string_view token) const
    {
        return text_.substr(pos_, token.size()) == token;
    }

    [[nodiscard]] bool Fail(std::string_view message)
    {
        error_ = message;
        return false;
    }

    std::string_view text_;
    std::size_t pos_ = 0;
    std::string_view error_;
};

struct FieldSchema
{
    int index = -1;
    std::string name;
    std::string typeToken;
    std::string rawSpec;
    std::string source;
};

struct PacketSchema
{
    std::string code;
    std::string direction;
    std::string legacyKind;
    std::string clientNamespace;
    std::string xlsxStatus;
    std::string description;
    std::string rawFormat;
    std::string originalFormat;
    bool formatEmpty = false;
    std::string source;
    std::string resolvedFormat;
    std::string resolvedSource;
    std::vector<FieldSchema> fields;
    std::vector<FieldSchema> resolvedFields;
};

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

const JsonValue* RequiredMember(const JsonValue& object, const char* key, int& failures)
{
    const JsonValue* value = object.Find(key);
    failures += Expect(value != nullptr, key);
    return value;
}

std::string RequiredString(const JsonValue& object, const char* key, int& failures)
{
    const JsonValue* value = RequiredMember(object, key, failures);
    if (value == nullptr) {
        return {};
    }
    failures += Expect(value->kind == JsonValue::Kind::String, key);
    return value->text;
}

std::string OptionalString(const JsonValue& object, const char* key, int& failures)
{
    const JsonValue* value = object.Find(key);
    if (value == nullptr) {
        return {};
    }
    failures += Expect(value->kind == JsonValue::Kind::String, key);
    return value->text;
}

bool RequiredBool(const JsonValue& object, const char* key, int& failures)
{
    const JsonValue* value = RequiredMember(object, key, failures);
    if (value == nullptr) {
        return false;
    }
    failures += Expect(value->kind == JsonValue::Kind::Bool, key);
    return value->boolValue;
}

int RequiredInt(const JsonValue& object, const char* key, int& failures)
{
    const JsonValue* value = RequiredMember(object, key, failures);
    if (value == nullptr) {
        return 0;
    }
    failures += Expect(value->kind == JsonValue::Kind::Number, key);
    int result = 0;
    const char* begin = value->text.data();
    const char* end = begin + value->text.size();
    const std::from_chars_result parsed = std::from_chars(begin, end, result);
    failures += Expect(parsed.ec == std::errc() && parsed.ptr == end, key);
    return result;
}

FieldSchema ReadFieldSchema(const JsonValue& object, int& failures)
{
    FieldSchema field;
    failures += Expect(object.kind == JsonValue::Kind::Object, "field object");
    if (object.kind != JsonValue::Kind::Object) {
        return field;
    }
    field.index = RequiredInt(object, "index", failures);
    field.name = RequiredString(object, "name", failures);
    field.typeToken = RequiredString(object, "typeToken", failures);
    field.rawSpec = RequiredString(object, "rawSpec", failures);
    field.source = RequiredString(object, "source", failures);
    return field;
}

PacketSchema ReadPacketSchema(const JsonValue& object, int& failures)
{
    PacketSchema packet;
    failures += Expect(object.kind == JsonValue::Kind::Object, "packet object");
    if (object.kind != JsonValue::Kind::Object) {
        return packet;
    }

    packet.code = RequiredString(object, "code", failures);
    packet.direction = RequiredString(object, "direction", failures);
    packet.legacyKind = RequiredString(object, "legacyKind", failures);
    packet.clientNamespace = RequiredString(object, "clientNamespace", failures);
    packet.xlsxStatus = RequiredString(object, "xlsxStatus", failures);
    packet.description = RequiredString(object, "description", failures);
    packet.rawFormat = RequiredString(object, "rawFormat", failures);
    packet.originalFormat = OptionalString(object, "originalFormat", failures);
    packet.formatEmpty = RequiredBool(object, "formatEmpty", failures);
    packet.source = RequiredString(object, "source", failures);
    packet.resolvedFormat = OptionalString(object, "resolvedFormat", failures);
    packet.resolvedSource = OptionalString(object, "resolvedSource", failures);

    const JsonValue* fields = RequiredMember(object, "fields", failures);
    if (fields != nullptr) {
        failures += Expect(fields->kind == JsonValue::Kind::Array, "fields array");
        if (fields->kind == JsonValue::Kind::Array) {
            packet.fields.reserve(fields->array.size());
            for (const JsonValue& field : fields->array) {
                packet.fields.push_back(ReadFieldSchema(field, failures));
            }
        }
    }

    const JsonValue* resolvedFields = object.Find("resolvedFields");
    if (resolvedFields != nullptr) {
        failures += Expect(resolvedFields->kind == JsonValue::Kind::Array, "resolvedFields array");
        if (resolvedFields->kind == JsonValue::Kind::Array) {
            packet.resolvedFields.reserve(resolvedFields->array.size());
            for (const JsonValue& field : resolvedFields->array) {
                packet.resolvedFields.push_back(ReadFieldSchema(field, failures));
            }
        }
    }
    return packet;
}

std::string ReadTextFile(const char* path)
{
    std::ifstream file(path, std::ios::binary);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string TypeToken(cg::packet::PacketFieldType type)
{
    switch (type) {
    case cg::packet::PacketFieldType::Int62:
        return "i62";
    case cg::packet::PacketFieldType::EscapedString:
    case cg::packet::PacketFieldType::SubPayload:
        return "es";
    case cg::packet::PacketFieldType::Unknown:
        return "unknown";
    }
    return "unknown";
}

const PacketSchema* FindPacket(const std::vector<PacketSchema>& packets, std::string_view code)
{
    for (const PacketSchema& packet : packets) {
        if (packet.code == code) {
            return &packet;
        }
    }
    return nullptr;
}

int ExpectPacketFieldsMatch(const PacketSchema& schema, const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(schema.fields.size() == packet.fields.size(), message);
    if (schema.fields.size() != packet.fields.size()) {
        return failures;
    }
    for (std::size_t index = 0; index < packet.fields.size(); ++index) {
        failures += Expect(schema.fields[index].index == static_cast<int>(index), message);
        failures += Expect(schema.fields[index].name == packet.fields[index].name, message);
        failures += Expect(schema.fields[index].typeToken == TypeToken(packet.fields[index].type), message);
    }
    return failures;
}

int ExpectResolvedFieldsMatch(const PacketSchema& schema, const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(schema.resolvedFields.size() == packet.fields.size(), message);
    if (schema.resolvedFields.size() != packet.fields.size()) {
        return failures;
    }
    for (std::size_t index = 0; index < packet.fields.size(); ++index) {
        failures += Expect(schema.resolvedFields[index].index == static_cast<int>(index), message);
        failures += Expect(schema.resolvedFields[index].name == packet.fields[index].name, message);
        failures += Expect(schema.resolvedFields[index].typeToken == TypeToken(packet.fields[index].type), message);
    }
    return failures;
}

int TestShape(const JsonValue& root, std::vector<PacketSchema>& packets)
{
    int failures = 0;
    failures += Expect(root.kind == JsonValue::Kind::Object, "root object");
    if (root.kind != JsonValue::Kind::Object) {
        return failures;
    }

    failures += Expect(RequiredString(root, "schema", failures) == "cg.packet.coverage.v1", "schema id");
    failures += Expect(RequiredInt(root, "packetCount", failures) == 209, "packetCount 209");
    const JsonValue* packetArray = RequiredMember(root, "packets", failures);
    if (packetArray == nullptr) {
        return failures;
    }

    failures += Expect(packetArray->kind == JsonValue::Kind::Array, "packets array");
    if (packetArray->kind != JsonValue::Kind::Array) {
        return failures;
    }

    failures += Expect(packetArray->array.size() == 209U, "packets size 209");
    packets.reserve(packetArray->array.size());
    for (const JsonValue& packet : packetArray->array) {
        packets.push_back(ReadPacketSchema(packet, failures));
    }
    return failures;
}

int TestCoverage(const std::vector<PacketSchema>& packets)
{
    int failures = 0;
    std::set<std::string> codes;
    int clientToServer = 0;
    int serverToClient = 0;
    int emptyOriginalFormat = 0;
    int normalizedEmptyFormat = 0;

    for (const PacketSchema& packet : packets) {
        failures += Expect(!packet.code.empty(), "code non-empty");
        failures += Expect(codes.insert(packet.code).second, "code unique");
        failures += Expect(packet.source == "xlsx", "packet source xlsx");

        if (packet.direction == "客户端->服务端") {
            ++clientToServer;
            failures += Expect(packet.legacyKind == "RECV", "client->server legacyKind");
            failures += Expect(packet.clientNamespace == "send", "client->server namespace");
        } else if (packet.direction == "服务端->客户端") {
            ++serverToClient;
            failures += Expect(packet.legacyKind == "SEND", "server->client legacyKind");
            failures += Expect(packet.clientNamespace == "receive", "server->client namespace");
        } else {
            failures += Expect(false, "direction known");
        }

        if (packet.rawFormat.empty()) {
            failures += Expect(packet.fields.empty(), "empty rawFormat fields");
            if (packet.formatEmpty) {
                ++emptyOriginalFormat;
                failures += Expect(packet.originalFormat.empty(), "empty xlsx format originalFormat");
            } else {
                ++normalizedEmptyFormat;
                failures += Expect(!packet.originalFormat.empty(), "normalized empty format originalFormat");
            }
        } else {
            failures += Expect(!packet.formatEmpty, "non-empty rawFormat formatEmpty");
        }
        for (std::size_t index = 0; index < packet.fields.size(); ++index) {
            failures += Expect(packet.fields[index].index == static_cast<int>(index), "field index order");
            failures += Expect(
                !packet.fields[index].name.empty() || packet.fields[index].rawSpec == "{}",
                "field name raw-preserved");
            failures += Expect(!packet.fields[index].typeToken.empty(), "field typeToken non-empty");
            failures += Expect(!packet.fields[index].rawSpec.empty(), "field rawSpec non-empty");
            failures += Expect(packet.fields[index].source == "xlsx", "field source xlsx");
        }
    }

    failures += Expect(codes.size() == 209U, "unique code count 209");
    failures += Expect(clientToServer == 100, "client->server count 100");
    failures += Expect(serverToClient == 109, "server->client count 109");
    failures += Expect(emptyOriginalFormat == 53, "empty original format count 53");
    failures += Expect(normalizedEmptyFormat == 16, "normalized empty format count 16");
    failures += Expect(FindPacket(packets, "CCL") != nullptr, "CCL exists");
    failures += Expect(FindPacket(packets, "TgX") != nullptr, "TgX exists");
    failures += Expect(FindPacket(packets, "JFVf") != nullptr, "JFVf exists");
    failures += Expect(FindPacket(packets, "whV") != nullptr, "whV exists");
    return failures;
}

int TestKnownPackets(const std::vector<PacketSchema>& packets)
{
    int failures = 0;

    const PacketSchema* clientLoginSchema = FindPacket(packets, "JFVf");
    failures += Expect(clientLoginSchema != nullptr, "JFVf schema exists");
    if (clientLoginSchema != nullptr) {
        const cg::packet::send::ClientLogin clientLogin("hash", "account", "password");
        failures += Expect(clientLoginSchema->direction == "客户端->服务端", "JFVf direction");
        failures += Expect(clientLoginSchema->legacyKind == "RECV", "JFVf legacyKind");
        failures += ExpectPacketFieldsMatch(*clientLoginSchema, clientLogin.ToPacket(), "JFVf field order");
    }

    const PacketSchema* loginResultSchema = FindPacket(packets, "whV");
    failures += Expect(loginResultSchema != nullptr, "whV schema exists");
    if (loginResultSchema != nullptr) {
        const cg::packet::receive::ClientLoginResult loginResult(0, "busy");
        failures += Expect(loginResultSchema->direction == "服务端->客户端", "whV direction");
        failures += Expect(loginResultSchema->legacyKind == "SEND", "whV legacyKind");
        failures += ExpectPacketFieldsMatch(*loginResultSchema, loginResult.ToPacket(), "whV field order");
    }

    const PacketSchema* characterListSchema = FindPacket(packets, "Daj");
    failures += Expect(characterListSchema != nullptr, "Daj schema exists");
    if (characterListSchema != nullptr) {
        const cg::packet::receive::CharacterList characterList(
            std::vector<cg::packet::receive::CharacterListRecord> {});
        const cg::packet::PacketResult<cg::packet::Packet> packet = characterList.ToPacket();
        failures += Expect(characterListSchema->direction == "服务端->客户端", "Daj direction");
        failures += Expect(characterListSchema->legacyKind == "SEND", "Daj legacyKind");
        failures += Expect(characterListSchema->rawFormat == "{result:i62} {data:es}", "Daj rawFormat");
        failures += Expect(characterListSchema->fields.size() == 2U, "Daj raw field count");
        failures += Expect(characterListSchema->resolvedFormat == "{result:i62} {data:es}", "Daj resolvedFormat");
        failures += Expect(
            characterListSchema->resolvedSource == "xlsx+ServerC+ClientIda+src",
            "Daj resolvedSource");
        failures += Expect(packet.HasValue(), "Daj packet exists");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(*characterListSchema, packet.Value(), "Daj resolved field order");
        }
    }

    const PacketSchema* createNewCharResultSchema = FindPacket(packets, "DZ");
    failures += Expect(createNewCharResultSchema != nullptr, "DZ schema exists");
    if (createNewCharResultSchema != nullptr) {
        const cg::packet::receive::CreateNewCharResult createNewCharResult("successful", 1, "");
        const cg::packet::Packet packet = createNewCharResult.ToPacket();
        failures += Expect(createNewCharResultSchema->direction == "服务端->客户端", "DZ direction");
        failures += Expect(createNewCharResultSchema->legacyKind == "SEND", "DZ legacyKind");
        failures += Expect(createNewCharResultSchema->rawFormat.empty(), "DZ rawFormat empty");
        failures += Expect(createNewCharResultSchema->formatEmpty, "DZ formatEmpty");
        failures += Expect(
            createNewCharResultSchema->resolvedFormat == "{result:es} {registnumber:i62} {data:es}",
            "DZ resolvedFormat");
        failures += Expect(
            createNewCharResultSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "DZ resolvedSource");
        failures += Expect(createNewCharResultSchema->fields.empty(), "DZ raw fields empty");
        failures += ExpectResolvedFieldsMatch(*createNewCharResultSchema, packet, "DZ resolved field order");
    }

    const PacketSchema* characterDeleteResultSchema = FindPacket(packets, "oy");
    failures += Expect(characterDeleteResultSchema != nullptr, "oy schema exists");
    if (characterDeleteResultSchema != nullptr) {
        const cg::packet::receive::CharacterDeleteResult characterDeleteResult("successful", "");
        const cg::packet::Packet packet = characterDeleteResult.ToPacket();
        failures += Expect(characterDeleteResultSchema->direction == "服务端->客户端", "oy direction");
        failures += Expect(characterDeleteResultSchema->legacyKind == "SEND", "oy legacyKind");
        failures += Expect(characterDeleteResultSchema->rawFormat.empty(), "oy rawFormat empty");
        failures += Expect(characterDeleteResultSchema->formatEmpty, "oy formatEmpty");
        failures += Expect(
            characterDeleteResultSchema->resolvedFormat == "{result:es} {data:es}",
            "oy resolvedFormat");
        failures += Expect(
            characterDeleteResultSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "oy resolvedSource");
        failures += Expect(characterDeleteResultSchema->fields.empty(), "oy raw fields empty");
        failures += ExpectResolvedFieldsMatch(*characterDeleteResultSchema, packet, "oy resolved field order");
    }

    const PacketSchema* processStatusSchema = FindPacket(packets, "cJzI");
    failures += Expect(processStatusSchema != nullptr, "cJzI schema exists");
    if (processStatusSchema != nullptr) {
        const cg::packet::receive::ProcessStatus processStatus("proc status");
        const cg::packet::Packet packet = processStatus.ToPacket();
        failures += Expect(processStatusSchema->direction == "服务端->客户端", "cJzI direction");
        failures += Expect(processStatusSchema->legacyKind == "SEND", "cJzI legacyKind");
        failures += Expect(processStatusSchema->rawFormat.empty(), "cJzI rawFormat empty");
        failures += Expect(processStatusSchema->formatEmpty, "cJzI formatEmpty");
        failures += Expect(
            processStatusSchema->resolvedFormat == "{data:es}",
            "cJzI resolvedFormat");
        failures += Expect(
            processStatusSchema->resolvedSource == "ServerC+src/packet",
            "cJzI resolvedSource");
        failures += Expect(processStatusSchema->fields.empty(), "cJzI raw fields empty");
        failures += ExpectResolvedFieldsMatch(*processStatusSchema, packet, "cJzI resolved field order");
    }

    const PacketSchema* profileListSchema = FindPacket(packets, "eZT");
    failures += Expect(profileListSchema != nullptr, "eZT schema exists");
    if (profileListSchema != nullptr) {
        const cg::packet::receive::ProfileList profileList(1, {{10, 100, "Hero"}});
        const cg::packet::PacketResult<cg::packet::Packet> packet = profileList.ToPacket();
        failures += Expect(profileListSchema->direction == "服务端->客户端", "eZT direction");
        failures += Expect(profileListSchema->legacyKind == "SEND", "eZT legacyKind");
        failures += Expect(profileListSchema->rawFormat == "{data}", "eZT rawFormat");
        failures += Expect(profileListSchema->fields.size() == 1U, "eZT raw field count");
        failures += Expect(profileListSchema->resolvedFormat == "{data:es}", "eZT resolvedFormat");
        failures += Expect(
            profileListSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "eZT resolvedSource");
        failures += Expect(packet.HasValue(), "eZT ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *profileListSchema,
                packet.Value(),
                "eZT resolved field order");
        }
    }

    const PacketSchema* aboutListSchema = FindPacket(packets, "IUn");
    failures += Expect(aboutListSchema != nullptr, "IUn schema exists");
    if (aboutListSchema != nullptr) {
        const cg::packet::receive::AboutList aboutList(1, {{10, 100, "Hero"}});
        const cg::packet::PacketResult<cg::packet::Packet> packet = aboutList.ToPacket();
        failures += Expect(aboutListSchema->direction == "服务端->客户端", "IUn direction");
        failures += Expect(aboutListSchema->legacyKind == "SEND", "IUn legacyKind");
        failures += Expect(aboutListSchema->rawFormat == "{data}", "IUn rawFormat");
        failures += Expect(aboutListSchema->fields.size() == 1U, "IUn raw field count");
        failures += Expect(aboutListSchema->resolvedFormat == "{data:es}", "IUn resolvedFormat");
        failures += Expect(
            aboutListSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "IUn resolvedSource");
        failures += Expect(packet.HasValue(), "IUn ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *aboutListSchema,
                packet.Value(),
                "IUn resolved field order");
        }
    }

    const PacketSchema* profileDataSchema = FindPacket(packets, "DpSY");
    failures += Expect(profileDataSchema != nullptr, "DpSY schema exists");
    if (profileDataSchema != nullptr) {
        const cg::packet::receive::ProfileData profileData(
            "Alice", 50, "Warrior", "Hero", "Guild", 100, 1, "sell", 2, "buy", 3, "about", "profile");
        const cg::packet::Packet packet = profileData.ToPacket();
        const std::vector<std::string_view> fieldNames {
            "name", "lv", "job", "title", "guildName", "graphic", "sid",
            "smsg", "bid", "bmsg", "aid", "amsg", "pmsg",
        };
        const std::vector<std::string_view> fieldTypes {
            "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
            "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
        };
        failures += Expect(profileDataSchema->direction == "服务端->客户端", "DpSY direction");
        failures += Expect(profileDataSchema->legacyKind == "SEND", "DpSY legacyKind");
        failures += Expect(
            profileDataSchema->rawFormat ==
                "{name} {lv} {job} {title} {guildName} {graphic} {sid} "
                "{smsg} {bid} {bmsg} {aid} {amsg} {pmsg}",
            "DpSY rawFormat");
        failures += Expect(profileDataSchema->fields.size() == fieldNames.size(), "DpSY raw field count");
        if (profileDataSchema->fields.size() == fieldNames.size()) {
            for (std::size_t index = 0; index < fieldNames.size(); ++index) {
                failures += Expect(profileDataSchema->fields[index].index == static_cast<int>(index), "DpSY raw field index");
                failures += Expect(profileDataSchema->fields[index].name == fieldNames[index], "DpSY raw field name");
                failures += Expect(profileDataSchema->fields[index].typeToken == fieldTypes[index], "DpSY raw field type");
            }
        }
        failures += Expect(
            profileDataSchema->resolvedFormat ==
                "{name:es} {lv:i62} {job:es} {title:es} {guildName:es} {graphic:i62} {sid:i62} "
                "{smsg:es} {bid:i62} {bmsg:es} {aid:i62} {amsg:es} {pmsg:es}",
            "DpSY resolvedFormat");
        failures += Expect(
            profileDataSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "DpSY resolvedSource");
        failures += ExpectResolvedFieldsMatch(*profileDataSchema, packet, "DpSY resolved/ToPacket field order");
    }

    const PacketSchema* profileMailDeleteSchema = FindPacket(packets, "cNB");
    failures += Expect(profileMailDeleteSchema != nullptr, "cNB schema exists");
    if (profileMailDeleteSchema != nullptr) {
        const cg::packet::receive::ProfileMailDelete profileMailDelete(1, "Alice");
        const cg::packet::Packet packet = profileMailDelete.ToPacket();
        failures += Expect(profileMailDeleteSchema->direction == "服务端->客户端", "cNB direction");
        failures += Expect(profileMailDeleteSchema->legacyKind == "SEND", "cNB legacyKind");
        failures += Expect(profileMailDeleteSchema->rawFormat == "{data}", "cNB rawFormat");
        failures += Expect(profileMailDeleteSchema->fields.size() == 1U, "cNB raw field count");
        if (profileMailDeleteSchema->fields.size() == 1U) {
            failures += Expect(profileMailDeleteSchema->fields[0].name == "data", "cNB raw field name");
            failures += Expect(profileMailDeleteSchema->fields[0].typeToken == "unknown", "cNB raw field type");
            failures += Expect(profileMailDeleteSchema->fields[0].rawSpec == "{data}", "cNB raw field spec");
        }
        failures += Expect(profileMailDeleteSchema->resolvedFormat == "{data:es}", "cNB resolvedFormat");
        failures += Expect(
            profileMailDeleteSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "cNB resolvedSource");
        failures += Expect(profileMailDeleteSchema->resolvedFields.size() == 1U, "cNB resolved field count");
        if (profileMailDeleteSchema->resolvedFields.size() == 1U) {
            failures += Expect(profileMailDeleteSchema->resolvedFields[0].name == "data", "cNB resolved field name");
            failures += Expect(profileMailDeleteSchema->resolvedFields[0].typeToken == "es", "cNB resolved field type");
            failures += Expect(
                profileMailDeleteSchema->resolvedFields[0].rawSpec == "{data:es}",
                "cNB resolved field spec");
        }
        failures += ExpectResolvedFieldsMatch(
            *profileMailDeleteSchema,
            packet,
            "cNB resolved/ToPacket field order");
    }

    const PacketSchema* characterProfileSchema = FindPacket(packets, "aPc");
    failures += Expect(characterProfileSchema != nullptr, "aPc schema exists");
    if (characterProfileSchema != nullptr) {
        cg::packet::receive::CharacterProfile characterProfile;
        characterProfile.flags = 2;
        characterProfile.hp = 100;
        const cg::packet::PacketResult<cg::packet::Packet> packet = characterProfile.ToPacket();
        failures += Expect(characterProfileSchema->direction == "服务端->客户端", "aPc direction");
        failures += Expect(characterProfileSchema->legacyKind == "SEND", "aPc legacyKind");
        failures += Expect(
            characterProfileSchema->rawFormat == "{data:es} {flag:%d} {flagValue:%d}",
            "aPc rawFormat");
        failures += Expect(characterProfileSchema->fields.size() == 3U, "aPc raw field count");
        failures += Expect(characterProfileSchema->resolvedFormat == "{data:es}", "aPc resolvedFormat");
        failures += Expect(
            characterProfileSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "aPc resolvedSource");
        failures += Expect(static_cast<bool>(packet), "aPc ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *characterProfileSchema,
                packet.Value(),
                "aPc resolved field order");
        }
    }

    const PacketSchema* characterProfile2Schema = FindPacket(packets, "UU");
    failures += Expect(characterProfile2Schema != nullptr, "UU schema exists");
    if (characterProfile2Schema != nullptr) {
        cg::packet::receive::CharacterProfile2 characterProfile2;
        characterProfile2.flagMask = 2;
        characterProfile2.critical = 100;
        const cg::packet::PacketResult<cg::packet::Packet> packet = characterProfile2.ToPacket();
        failures += Expect(characterProfile2Schema->direction == "服务端->客户端", "UU direction");
        failures += Expect(characterProfile2Schema->legacyKind == "SEND", "UU legacyKind");
        failures += Expect(
            characterProfile2Schema->rawFormat == "{data:es} {flag:%d} {flagValue:%d}",
            "UU rawFormat");
        failures += Expect(characterProfile2Schema->fields.size() == 3U, "UU raw field count");
        if (characterProfile2Schema->fields.size() == 3U) {
            failures += Expect(
                characterProfile2Schema->fields[0].name == "data" &&
                    characterProfile2Schema->fields[0].typeToken == "es",
                "UU raw data field");
            failures += Expect(
                characterProfile2Schema->fields[1].name == "flag" &&
                    characterProfile2Schema->fields[1].typeToken == "%d",
                "UU raw flag conflict field");
            failures += Expect(
                characterProfile2Schema->fields[2].name == "flagValue" &&
                    characterProfile2Schema->fields[2].typeToken == "%d",
                "UU raw flagValue conflict field");
        }
        failures += Expect(characterProfile2Schema->resolvedFormat == "{data:es}", "UU resolvedFormat");
        failures += Expect(
            characterProfile2Schema->resolvedSource == "ServerC+ClientIda+src/packet",
            "UU resolvedSource");
        failures += Expect(static_cast<bool>(packet), "UU ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *characterProfile2Schema,
                packet.Value(),
                "UU resolved field order");
        }
    }

    const PacketSchema* titleListSchema = FindPacket(packets, "Sc");
    failures += Expect(titleListSchema != nullptr, "Sc schema exists");
    if (titleListSchema != nullptr) {
        std::vector<cg::packet::receive::TitleSlot> slots;
        slots.emplace_back("Hero", 1, 100);
        const cg::packet::receive::TitleList titleList(std::move(slots));
        const cg::packet::PacketResult<cg::packet::Packet> packet = titleList.ToPacket();
        failures += Expect(titleListSchema->direction == "服务端->客户端", "Sc direction");
        failures += Expect(titleListSchema->legacyKind == "SEND", "Sc legacyKind");
        failures += Expect(
            titleListSchema->rawFormat == "{data:es} {titleName:%s}",
            "Sc rawFormat");
        failures += Expect(titleListSchema->fields.size() == 2U, "Sc raw field count");
        if (titleListSchema->fields.size() == 2U) {
            failures += Expect(
                titleListSchema->fields[0].name == "data" &&
                    titleListSchema->fields[0].typeToken == "es",
                "Sc raw data field");
            failures += Expect(
                titleListSchema->fields[1].name == "titleName" &&
                    titleListSchema->fields[1].typeToken == "%s",
                "Sc raw titleName conflict field");
        }
        failures += Expect(titleListSchema->resolvedFormat == "{data:es}", "Sc resolvedFormat");
        failures += Expect(
            titleListSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "Sc resolvedSource");
        failures += Expect(static_cast<bool>(packet), "Sc ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *titleListSchema,
                packet.Value(),
                "Sc resolved field order");
        }
    }

    const PacketSchema* characterIconSchema = FindPacket(packets, "NI");
    failures += Expect(characterIconSchema != nullptr, "NI schema exists");
    if (characterIconSchema != nullptr) {
        const cg::packet::receive::CharacterIcon characterIcon(
            {cg::packet::receive::CharacterIconEntry(1, 10, 20, 3, 4)});
        const cg::packet::PacketResult<cg::packet::Packet> packet = characterIcon.ToPacket();
        failures += Expect(characterIconSchema->direction == "服务端->客户端", "NI direction");
        failures += Expect(characterIconSchema->legacyKind == "SEND", "NI legacyKind");
        failures += Expect(
            characterIconSchema->rawFormat == "{data:es} {caInfo:s}",
            "NI rawFormat");
        failures += Expect(characterIconSchema->fields.size() == 2U, "NI raw field count");
        if (characterIconSchema->fields.size() == 2U) {
            failures += Expect(
                characterIconSchema->fields[0].name == "data" &&
                    characterIconSchema->fields[0].typeToken == "es",
                "NI raw data field");
            failures += Expect(
                characterIconSchema->fields[1].name == "caInfo" &&
                    characterIconSchema->fields[1].typeToken == "s",
                "NI raw caInfo conflict field");
        }
        failures += Expect(characterIconSchema->resolvedFormat == "{data:es}", "NI resolvedFormat");
        failures += Expect(
            characterIconSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "NI resolvedSource");
        failures += Expect(static_cast<bool>(packet), "NI ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *characterIconSchema,
                packet.Value(),
                "NI resolved field order");
        }
    }

    const PacketSchema* createNewCharSchema = FindPacket(packets, "aSEl");
    failures += Expect(createNewCharSchema != nullptr, "aSEl schema exists");
    if (createNewCharSchema != nullptr) {
        const cg::packet::send::CreateNewChar createNewChar(1, "Alice", 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        const cg::packet::Packet packet = createNewChar.ToPacket();
        failures += Expect(createNewCharSchema->direction == "客户端->服务端", "aSEl direction");
        failures += Expect(createNewCharSchema->legacyKind == "RECV", "aSEl legacyKind");
        failures += Expect(
            createNewCharSchema->rawFormat ==
                "{dataPlace} {name} {image} {face} {vital} {strength} {tough} {quick} {magic} {earth} {water} {fire} {wind}",
            "aSEl rawFormat");
        failures += Expect(createNewCharSchema->fields.size() == 13U, "aSEl raw field count");
        failures += Expect(
            createNewCharSchema->resolvedFormat ==
                "{dataPlace:i62} {name:es} {image:i62} {face:i62} {vital:i62} {strength:i62} {tough:i62} {quick:i62} {magic:i62} {earth:i62} {water:i62} {fire:i62} {wind:i62}",
            "aSEl resolvedFormat");
        failures += Expect(createNewCharSchema->resolvedSource == "xlsx+src/packet", "aSEl resolvedSource");
        failures += ExpectResolvedFieldsMatch(*createNewCharSchema, packet, "aSEl resolved field order");
    }

    const PacketSchema* magicUseSchema = FindPacket(packets, "RXr");
    failures += Expect(magicUseSchema != nullptr, "RXr schema exists");
    if (magicUseSchema != nullptr) {
        const cg::packet::send::MagicUseRequest magicUse(1, 2, 3, 4);
        const cg::packet::Packet packet = magicUse.ToPacket();
        failures += Expect(magicUseSchema->direction == "客户端->服务端", "RXr direction");
        failures += Expect(magicUseSchema->legacyKind == "RECV", "RXr legacyKind");
        failures += Expect(magicUseSchema->rawFormat.empty(), "RXr rawFormat empty");
        failures += Expect(magicUseSchema->formatEmpty, "RXr formatEmpty");
        failures += Expect(
            magicUseSchema->resolvedFormat == "{x:i62} {y:i62} {array:i62} {toindex:i62}",
            "RXr resolvedFormat");
        failures += Expect(magicUseSchema->resolvedSource == "ServerC+src/packet", "RXr resolvedSource");
        failures += Expect(magicUseSchema->fields.empty(), "RXr raw fields empty");
        failures += ExpectResolvedFieldsMatch(*magicUseSchema, packet, "RXr resolved field order");
    }

    const PacketSchema* effectSchema = FindPacket(packets, "zzd");
    failures += Expect(effectSchema != nullptr, "zzd schema exists");
    if (effectSchema != nullptr) {
        const cg::packet::receive::Effect effect(1, 2, "data");
        failures += Expect(effectSchema->direction == "服务端->客户端", "zzd direction");
        failures += Expect(effectSchema->legacyKind == "SEND", "zzd legacyKind");
        failures += Expect(effectSchema->rawFormat == "{EffType:i62} {TM_EffSize:i62} {data:es}", "zzd rawFormat");
        failures += ExpectPacketFieldsMatch(*effectSchema, effect.ToPacket(), "zzd field order");
    }

    const PacketSchema* battleCommandSchema = FindPacket(packets, "LYIm");
    failures += Expect(battleCommandSchema != nullptr, "LYIm schema exists");
    if (battleCommandSchema != nullptr) {
        failures += Expect(battleCommandSchema->direction == "服务端->客户端", "LYIm direction");
        failures += Expect(battleCommandSchema->legacyKind == "SEND", "LYIm legacyKind");
        failures += Expect(battleCommandSchema->rawFormat.empty(), "LYIm rawFormat empty");
        failures += Expect(battleCommandSchema->formatEmpty, "LYIm formatEmpty");
        failures += Expect(battleCommandSchema->resolvedFormat == "{command:es}", "LYIm resolvedFormat");
        failures += Expect(
            battleCommandSchema->resolvedSource == "ServerC+ClientIda+src",
            "LYIm resolvedSource");
        failures += Expect(battleCommandSchema->fields.empty(), "LYIm raw fields empty");
    }

    const PacketSchema* battleRewardSchema = FindPacket(packets, "Gn");
    failures += Expect(battleRewardSchema != nullptr, "Gn schema exists");
    if (battleRewardSchema != nullptr) {
        failures += Expect(battleRewardSchema->direction == "服务端->客户端", "Gn direction");
        failures += Expect(battleRewardSchema->legacyKind == "SEND", "Gn legacyKind");
        failures += Expect(battleRewardSchema->rawFormat.empty(), "Gn rawFormat empty");
        failures += Expect(battleRewardSchema->formatEmpty, "Gn formatEmpty");
        failures += Expect(battleRewardSchema->resolvedFormat == "{data:es}", "Gn resolvedFormat");
        failures += Expect(battleRewardSchema->resolvedSource == "ClientIda+src", "Gn resolvedSource");
        failures += Expect(battleRewardSchema->fields.empty(), "Gn raw fields empty");
    }

    const PacketSchema* duelPointSchema = FindPacket(packets, "hB");
    failures += Expect(duelPointSchema != nullptr, "hB schema exists");
    if (duelPointSchema != nullptr) {
        failures += Expect(duelPointSchema->direction == "服务端->客户端", "hB direction");
        failures += Expect(duelPointSchema->legacyKind == "SEND", "hB legacyKind");
        failures += Expect(duelPointSchema->rawFormat.empty(), "hB rawFormat empty");
        failures += Expect(duelPointSchema->formatEmpty, "hB formatEmpty");
        failures += Expect(duelPointSchema->resolvedFormat == "{data:es}", "hB resolvedFormat");
        failures += Expect(duelPointSchema->resolvedSource == "ServerC+ClientIda+src", "hB resolvedSource");
        failures += Expect(duelPointSchema->fields.empty(), "hB raw fields empty");
    }

    const PacketSchema* tradePlayerListSchema = FindPacket(packets, "Cxo");
    failures += Expect(tradePlayerListSchema != nullptr, "Cxo schema exists");
    if (tradePlayerListSchema != nullptr) {
        const cg::packet::receive::TradePlayerList tradePlayerList({{"Alice"}});
        const cg::packet::PacketResult<cg::packet::Packet> packet = tradePlayerList.ToPacket();
        failures += Expect(tradePlayerListSchema->direction == "服务端->客户端", "Cxo direction");
        failures += Expect(tradePlayerListSchema->legacyKind == "SEND", "Cxo legacyKind");
        failures += Expect(tradePlayerListSchema->rawFormat.empty(), "Cxo rawFormat empty");
        failures += Expect(tradePlayerListSchema->formatEmpty, "Cxo formatEmpty");
        failures += Expect(tradePlayerListSchema->resolvedFormat == "{data:es}", "Cxo resolvedFormat");
        failures += Expect(
            tradePlayerListSchema->resolvedSource == "ServerC+src/packet",
            "Cxo resolvedSource");
        failures += Expect(tradePlayerListSchema->fields.empty(), "Cxo raw fields empty");
        failures += Expect(packet.HasValue(), "Cxo packet build");
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 1U, "Cxo packet field count");
            if (packet.Value().fields.size() == 1U) {
                failures += Expect(packet.Value().fields[0].name == "data", "Cxo packet data field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
                    "Cxo packet data type");
            }
        }
    }

    const PacketSchema* tradeStartSchema = FindPacket(packets, "ms");
    failures += Expect(tradeStartSchema != nullptr, "ms schema exists");
    if (tradeStartSchema != nullptr) {
        const cg::packet::receive::TradeStart tradeStart("Alice", 10);
        const cg::packet::Packet packet = tradeStart.ToPacket();
        failures += Expect(tradeStartSchema->direction == "服务端->客户端", "ms direction");
        failures += Expect(tradeStartSchema->legacyKind == "SEND", "ms legacyKind");
        failures += Expect(tradeStartSchema->rawFormat.empty(), "ms rawFormat empty");
        failures += Expect(tradeStartSchema->formatEmpty, "ms formatEmpty");
        failures += Expect(tradeStartSchema->resolvedFormat == "{name:es} {level:i62}", "ms resolvedFormat");
        failures += Expect(tradeStartSchema->resolvedSource == "ServerC+src/packet", "ms resolvedSource");
        failures += Expect(tradeStartSchema->fields.empty(), "ms raw fields empty");
        failures += Expect(packet.fields.size() == 2U, "ms packet field count");
        if (packet.fields.size() == 2U) {
            failures += Expect(packet.fields[0].name == "name", "ms packet name field");
            failures += Expect(
                packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
                "ms packet name type");
            failures += Expect(packet.fields[1].name == "level", "ms packet level field");
            failures += Expect(
                packet.fields[1].type == cg::packet::PacketFieldType::Int62,
                "ms packet level type");
        }
    }

    const PacketSchema* tradeItemInfoSchema = FindPacket(packets, "nUbJ");
    failures += Expect(tradeItemInfoSchema != nullptr, "nUbJ schema exists");
    if (tradeItemInfoSchema != nullptr) {
        const cg::packet::receive::TradeItemInfo tradeItemInfo({{0, "Potion"}});
        const cg::packet::PacketResult<cg::packet::Packet> packet = tradeItemInfo.ToPacket();
        failures += Expect(tradeItemInfoSchema->direction == "服务端->客户端", "nUbJ direction");
        failures += Expect(tradeItemInfoSchema->legacyKind == "SEND", "nUbJ legacyKind");
        failures += Expect(tradeItemInfoSchema->rawFormat.empty(), "nUbJ rawFormat empty");
        failures += Expect(tradeItemInfoSchema->formatEmpty, "nUbJ formatEmpty");
        failures += Expect(tradeItemInfoSchema->resolvedFormat == "{data:es}", "nUbJ resolvedFormat");
        failures += Expect(tradeItemInfoSchema->resolvedSource == "ServerC+src/packet", "nUbJ resolvedSource");
        failures += Expect(tradeItemInfoSchema->fields.empty(), "nUbJ raw fields empty");
        failures += Expect(packet.HasValue(), "nUbJ packet build");
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 1U, "nUbJ packet field count");
            if (packet.Value().fields.size() == 1U) {
                failures += Expect(packet.Value().fields[0].name == "data", "nUbJ packet data field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
                    "nUbJ packet data type");
            }
        }
    }

    const PacketSchema* tradePetInfoSchema = FindPacket(packets, "RwK");
    failures += Expect(tradePetInfoSchema != nullptr, "RwK schema exists");
    if (tradePetInfoSchema != nullptr) {
        const cg::packet::receive::TradePetInfoData data {
            "Free",
            1,
            2,
            3,
            4,
            5,
            6,
            7,
            8,
            9,
            10,
            11,
            "Pet",
            12,
            13,
            14,
            15,
            16,
            17,
            18,
            19,
            20,
            21,
        };
        const cg::packet::receive::TradePetInfo tradePetInfo(2, data);
        const cg::packet::PacketResult<cg::packet::Packet> packet = tradePetInfo.ToPacket();
        failures += Expect(tradePetInfoSchema->direction == "服务端->客户端", "RwK direction");
        failures += Expect(tradePetInfoSchema->legacyKind == "SEND", "RwK legacyKind");
        failures += Expect(tradePetInfoSchema->rawFormat.empty(), "RwK rawFormat empty");
        failures += Expect(tradePetInfoSchema->formatEmpty, "RwK formatEmpty");
        failures += Expect(
            tradePetInfoSchema->resolvedFormat == "{petPlace:i62} {data:es}",
            "RwK resolvedFormat");
        failures += Expect(tradePetInfoSchema->resolvedSource == "ServerC+src/packet", "RwK resolvedSource");
        failures += Expect(tradePetInfoSchema->fields.empty(), "RwK raw fields empty");
        failures += Expect(packet.HasValue(), "RwK packet build");
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 2U, "RwK packet field count");
            if (packet.Value().fields.size() == 2U) {
                failures += Expect(packet.Value().fields[0].name == "petPlace", "RwK packet petPlace field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::Int62,
                    "RwK packet petPlace type");
                failures += Expect(packet.Value().fields[1].name == "data", "RwK packet data field");
                failures += Expect(
                    packet.Value().fields[1].type == cg::packet::PacketFieldType::EscapedString,
                    "RwK packet data type");
            }
        }
    }

    const PacketSchema* tradePetSkillInfoSchema = FindPacket(packets, "Yq");
    failures += Expect(tradePetSkillInfoSchema != nullptr, "Yq schema exists");
    if (tradePetSkillInfoSchema != nullptr) {
        const cg::packet::receive::TradePetSkillInfo tradePetSkillInfo(2, {"Bite"});
        const cg::packet::PacketResult<cg::packet::Packet> packet = tradePetSkillInfo.ToPacket();
        failures += Expect(tradePetSkillInfoSchema->direction == "服务端->客户端", "Yq direction");
        failures += Expect(tradePetSkillInfoSchema->legacyKind == "SEND", "Yq legacyKind");
        failures += Expect(tradePetSkillInfoSchema->rawFormat.empty(), "Yq rawFormat empty");
        failures += Expect(tradePetSkillInfoSchema->formatEmpty, "Yq formatEmpty");
        failures += Expect(
            tradePetSkillInfoSchema->resolvedFormat == "{petIndex:i62} {data:es}",
            "Yq resolvedFormat");
        failures += Expect(tradePetSkillInfoSchema->resolvedSource == "ServerC+src/packet", "Yq resolvedSource");
        failures += Expect(tradePetSkillInfoSchema->fields.empty(), "Yq raw fields empty");
        failures += Expect(packet.HasValue(), "Yq packet build");
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 2U, "Yq packet field count");
            if (packet.Value().fields.size() == 2U) {
                failures += Expect(packet.Value().fields[0].name == "petIndex", "Yq packet petIndex field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::Int62,
                    "Yq packet petIndex type");
                failures += Expect(packet.Value().fields[1].name == "data", "Yq packet data field");
                failures += Expect(
                    packet.Value().fields[1].type == cg::packet::PacketFieldType::EscapedString,
                    "Yq packet data type");
            }
        }
    }

    const PacketSchema* profileMailMessageSchema = FindPacket(packets, "Mhbt");
    failures += Expect(profileMailMessageSchema != nullptr, "Mhbt schema exists");
    if (profileMailMessageSchema != nullptr) {
        failures += Expect(profileMailMessageSchema->direction == "服务端->客户端", "Mhbt direction");
        failures += Expect(profileMailMessageSchema->legacyKind == "SEND", "Mhbt legacyKind");
        failures += Expect(profileMailMessageSchema->rawFormat.empty(), "Mhbt rawFormat empty");
        failures += Expect(profileMailMessageSchema->formatEmpty, "Mhbt formatEmpty");
        failures += Expect(
            profileMailMessageSchema->resolvedFormat == "{aindex:i62} {text:es} {color:i62}",
            "Mhbt resolvedFormat");
        failures += Expect(
            profileMailMessageSchema->resolvedSource == "ServerC+ClientIda+src",
            "Mhbt resolvedSource");
        failures += Expect(profileMailMessageSchema->fields.empty(), "Mhbt raw fields empty");
    }

    const PacketSchema* chatMessageSchema = FindPacket(packets, "zdr");
    failures += Expect(chatMessageSchema != nullptr, "zdr schema exists");
    if (chatMessageSchema != nullptr) {
        failures += Expect(chatMessageSchema->direction == "服务端->客户端", "zdr direction");
        failures += Expect(chatMessageSchema->legacyKind == "SEND", "zdr legacyKind");
        failures += Expect(chatMessageSchema->rawFormat.empty(), "zdr rawFormat empty");
        failures += Expect(chatMessageSchema->formatEmpty, "zdr formatEmpty");
        failures += Expect(
            chatMessageSchema->resolvedFormat == "{objectIndex:i62} {message:es} {color:i62} {fontSize:i62}",
            "zdr resolvedFormat");
        failures += Expect(
            chatMessageSchema->resolvedSource == "ServerC+ClientIda+src",
            "zdr resolvedSource");
        failures += Expect(chatMessageSchema->fields.empty(), "zdr raw fields empty");
    }

    const PacketSchema* windowPacketSchema = FindPacket(packets, "BZ");
    failures += Expect(windowPacketSchema != nullptr, "BZ schema exists");
    if (windowPacketSchema != nullptr) {
        failures += Expect(windowPacketSchema->direction == "服务端->客户端", "BZ direction");
        failures += Expect(windowPacketSchema->legacyKind == "SEND", "BZ legacyKind");
        failures += Expect(
            windowPacketSchema->rawFormat == "{windowtype} {buttontype} {seqno} {objindex} {data:es}",
            "BZ rawFormat");
        failures += Expect(
            windowPacketSchema->resolvedFormat == "{windowtype:i62} {buttontype:i62} {seqno:i62} {objindex:i62} {data:es}",
            "BZ resolvedFormat");
        failures += Expect(
            windowPacketSchema->resolvedSource == "xlsx+ServerC+ClientIda+src",
            "BZ resolvedSource");
        failures += Expect(windowPacketSchema->fields.size() == 5U, "BZ raw field count");
        if (windowPacketSchema->fields.size() == 5U) {
            failures += Expect(windowPacketSchema->fields[0].name == "windowtype", "BZ field windowtype");
            failures += Expect(windowPacketSchema->fields[1].name == "buttontype", "BZ field buttontype");
            failures += Expect(windowPacketSchema->fields[2].name == "seqno", "BZ field seqno");
            failures += Expect(windowPacketSchema->fields[3].name == "objindex", "BZ field objindex");
            failures += Expect(windowPacketSchema->fields[4].name == "data", "BZ field data");
            failures += Expect(windowPacketSchema->fields[4].typeToken == "es", "BZ data raw field type");
        }
    }

    const PacketSchema* stallBrowseSchema = FindPacket(packets, "ft");
    failures += Expect(stallBrowseSchema != nullptr, "ft schema exists");
    if (stallBrowseSchema != nullptr) {
        failures += Expect(stallBrowseSchema->direction == "服务端->客户端", "ft direction");
        failures += Expect(stallBrowseSchema->legacyKind == "SEND", "ft legacyKind");
        failures += Expect(
            stallBrowseSchema->rawFormat == "{result} {name} {description} {item} {pet}",
            "ft rawFormat");
        failures += Expect(
            stallBrowseSchema->resolvedFormat ==
                "{result:i62} {objIndex:i62} {name:es} {description:es} {item:es} {pet:es}",
            "ft resolvedFormat");
        failures += Expect(
            stallBrowseSchema->resolvedSource == "xlsx+ServerC+ClientIda+src",
            "ft resolvedSource");
        failures += Expect(stallBrowseSchema->fields.size() == 5U, "ft raw field count");
        if (stallBrowseSchema->fields.size() == 5U) {
            failures += Expect(stallBrowseSchema->fields[0].name == "result", "ft raw field result");
            failures += Expect(stallBrowseSchema->fields[1].name == "name", "ft raw field name");
            failures += Expect(stallBrowseSchema->fields[2].name == "description", "ft raw field description");
            failures += Expect(stallBrowseSchema->fields[3].name == "item", "ft raw field item");
            failures += Expect(stallBrowseSchema->fields[4].name == "pet", "ft raw field pet");
            failures += Expect(stallBrowseSchema->fields[1].typeToken == "unknown", "ft raw type unknown");
        }
    }

    const PacketSchema* mailMessageSendSchema = FindPacket(packets, "CV");
    failures += Expect(mailMessageSendSchema != nullptr, "CV schema exists");
    if (mailMessageSendSchema != nullptr) {
        failures += Expect(mailMessageSendSchema->direction == "客户端->服务端", "CV direction");
        failures += Expect(mailMessageSendSchema->legacyKind == "RECV", "CV legacyKind");
        failures += Expect(
            mailMessageSendSchema->rawFormat == "{index} {message:es} {color}",
            "CV rawFormat");
        failures += Expect(
            mailMessageSendSchema->resolvedFormat == "{index:i62} {message:es} {color:i62}",
            "CV resolvedFormat");
        failures += Expect(
            mailMessageSendSchema->resolvedSource == "xlsx+ServerC+ClientIda+src",
            "CV resolvedSource");
        failures += Expect(mailMessageSendSchema->fields.size() == 3U, "CV raw field count");
        if (mailMessageSendSchema->fields.size() == 3U) {
            failures += Expect(mailMessageSendSchema->fields[0].name == "index", "CV raw field index");
            failures += Expect(mailMessageSendSchema->fields[1].name == "message", "CV raw field message");
            failures += Expect(mailMessageSendSchema->fields[2].name == "color", "CV raw field color");
            failures += Expect(mailMessageSendSchema->fields[0].typeToken == "unknown", "CV raw type unknown");
            failures += Expect(mailMessageSendSchema->fields[1].typeToken == "es", "CV raw message type");
            failures += Expect(mailMessageSendSchema->fields[2].typeToken == "unknown", "CV raw color type");
        }
    }

    const PacketSchema* chatMessageSendSchema = FindPacket(packets, "uSr");
    failures += Expect(chatMessageSendSchema != nullptr, "uSr schema exists");
    if (chatMessageSendSchema != nullptr) {
        failures += Expect(chatMessageSendSchema->direction == "客户端->服务端", "uSr direction");
        failures += Expect(chatMessageSendSchema->legacyKind == "RECV", "uSr legacyKind");
        failures += Expect(
            chatMessageSendSchema->rawFormat == "{x} {y} {msg} {color} {area} {fontSize}",
            "uSr rawFormat");
        failures += Expect(
            chatMessageSendSchema->resolvedFormat ==
                "{x:i62} {y:i62} {msg:es} {color:i62} {area:i62} {fontSize:i62}",
            "uSr resolvedFormat");
        failures += Expect(
            chatMessageSendSchema->resolvedSource == "xlsx+ServerC+ClientIda+src",
            "uSr resolvedSource");
        failures += Expect(chatMessageSendSchema->fields.size() == 6U, "uSr raw field count");
        if (chatMessageSendSchema->fields.size() == 6U) {
            failures += Expect(chatMessageSendSchema->fields[0].name == "x", "uSr raw field x");
            failures += Expect(chatMessageSendSchema->fields[1].name == "y", "uSr raw field y");
            failures += Expect(chatMessageSendSchema->fields[2].name == "msg", "uSr raw field msg");
            failures += Expect(chatMessageSendSchema->fields[3].name == "color", "uSr raw field color");
            failures += Expect(chatMessageSendSchema->fields[4].name == "area", "uSr raw field area");
            failures += Expect(chatMessageSendSchema->fields[5].name == "fontSize", "uSr raw field fontSize");
            failures += Expect(chatMessageSendSchema->fields[2].typeToken == "unknown", "uSr raw msg type");
        }
    }

    const PacketSchema* dropGoldSchema = FindPacket(packets, "woz");
    failures += Expect(dropGoldSchema != nullptr, "woz schema exists");
    if (dropGoldSchema != nullptr) {
        const cg::packet::send::DropGold dropGold(1, 2, 3);
        const cg::packet::Packet packet = dropGold.ToPacket();
        failures += Expect(dropGoldSchema->direction == "客户端->服务端", "woz direction");
        failures += Expect(dropGoldSchema->legacyKind == "RECV", "woz legacyKind");
        failures += Expect(dropGoldSchema->rawFormat == "{x} {y} {amount}", "woz rawFormat");
        failures += Expect(
            dropGoldSchema->resolvedFormat == "{x:i62} {y:i62} {amount:i62}",
            "woz resolvedFormat");
        failures += Expect(dropGoldSchema->resolvedSource == "xlsx+src/packet", "woz resolvedSource");
        failures += Expect(dropGoldSchema->fields.size() == 3U, "woz raw field count");
        if (dropGoldSchema->fields.size() == 3U) {
            failures += Expect(dropGoldSchema->fields[0].name == "x", "woz raw field x");
            failures += Expect(dropGoldSchema->fields[1].name == "y", "woz raw field y");
            failures += Expect(dropGoldSchema->fields[2].name == "amount", "woz raw field amount");
            failures += Expect(dropGoldSchema->fields[0].typeToken == "unknown", "woz raw x type");
            failures += Expect(dropGoldSchema->fields[1].typeToken == "unknown", "woz raw y type");
            failures += Expect(dropGoldSchema->fields[2].typeToken == "unknown", "woz raw amount type");
        }
        failures += Expect(packet.fields.size() == 3U, "woz packet field count");
        if (packet.fields.size() == 3U) {
            failures += Expect(packet.fields[0].name == "x", "woz packet x field");
            failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, "woz packet x type");
            failures += Expect(packet.fields[1].name == "y", "woz packet y field");
            failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::Int62, "woz packet y type");
            failures += Expect(packet.fields[2].name == "amount", "woz packet amount field");
            failures += Expect(
                packet.fields[2].type == cg::packet::PacketFieldType::Int62,
                "woz packet amount type");
        }
    }

    const PacketSchema* shutdownRequestSchema = FindPacket(packets, "IcE");
    failures += Expect(shutdownRequestSchema != nullptr, "IcE schema exists");
    if (shutdownRequestSchema != nullptr) {
        const cg::packet::send::ShutdownRequest shutdownRequest("pass", 10);
        const cg::packet::Packet packet = shutdownRequest.ToPacket();
        failures += Expect(shutdownRequestSchema->direction == "客户端->服务端", "IcE direction");
        failures += Expect(shutdownRequestSchema->legacyKind == "RECV", "IcE legacyKind");
        failures += Expect(shutdownRequestSchema->rawFormat.empty(), "IcE rawFormat empty");
        failures += Expect(shutdownRequestSchema->formatEmpty, "IcE formatEmpty");
        failures += Expect(
            shutdownRequestSchema->resolvedFormat == "{passwd:es} {min:i62}",
            "IcE resolvedFormat");
        failures += Expect(shutdownRequestSchema->resolvedSource == "ServerC+src/packet", "IcE resolvedSource");
        failures += Expect(shutdownRequestSchema->fields.empty(), "IcE raw fields empty");
        failures += ExpectResolvedFieldsMatch(*shutdownRequestSchema, packet, "IcE resolved field order");
    }

    const PacketSchema* tradeTakeOutSchema = FindPacket(packets, "aiAb");
    failures += Expect(tradeTakeOutSchema != nullptr, "aiAb schema exists");
    if (tradeTakeOutSchema != nullptr) {
        const cg::packet::send::TradeTakeOut tradeTakeOut("8|12", "0|2", 10);
        const cg::packet::Packet packet = tradeTakeOut.ToPacket();
        failures += Expect(tradeTakeOutSchema->direction == "客户端->服务端", "aiAb direction");
        failures += Expect(tradeTakeOutSchema->legacyKind == "RECV", "aiAb legacyKind");
        failures += Expect(tradeTakeOutSchema->rawFormat == "{item} {pet} {gold}", "aiAb rawFormat");
        failures += Expect(
            tradeTakeOutSchema->resolvedFormat == "{item:es} {pet:es} {gold:i62}",
            "aiAb resolvedFormat");
        failures += Expect(tradeTakeOutSchema->resolvedSource == "xlsx+src/packet", "aiAb resolvedSource");
        failures += Expect(tradeTakeOutSchema->fields.size() == 3U, "aiAb raw field count");
        if (tradeTakeOutSchema->fields.size() == 3U) {
            failures += Expect(tradeTakeOutSchema->fields[0].name == "item", "aiAb raw field item");
            failures += Expect(tradeTakeOutSchema->fields[1].name == "pet", "aiAb raw field pet");
            failures += Expect(tradeTakeOutSchema->fields[2].name == "gold", "aiAb raw field gold");
            failures += Expect(tradeTakeOutSchema->fields[0].typeToken == "unknown", "aiAb raw item type");
            failures += Expect(tradeTakeOutSchema->fields[1].typeToken == "unknown", "aiAb raw pet type");
            failures += Expect(tradeTakeOutSchema->fields[2].typeToken == "unknown", "aiAb raw gold type");
        }
        failures += Expect(packet.fields.size() == 3U, "aiAb packet field count");
        if (packet.fields.size() == 3U) {
            failures += Expect(packet.fields[0].name == "item", "aiAb packet item field");
            failures += Expect(
                packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
                "aiAb packet item type");
            failures += Expect(packet.fields[1].name == "pet", "aiAb packet pet field");
            failures += Expect(
                packet.fields[1].type == cg::packet::PacketFieldType::EscapedString,
                "aiAb packet pet type");
            failures += Expect(packet.fields[2].name == "gold", "aiAb packet gold field");
            failures += Expect(
                packet.fields[2].type == cg::packet::PacketFieldType::Int62,
                "aiAb packet gold type");
        }
    }

    const PacketSchema* stallStartSchema = FindPacket(packets, "ylGN");
    failures += Expect(stallStartSchema != nullptr, "ylGN schema exists");
    if (stallStartSchema != nullptr) {
        const cg::packet::send::StallStart stallStart("Shop", "Desc");
        const cg::packet::PacketResult<cg::packet::Packet> packet = stallStart.ToPacket();
        failures += Expect(stallStartSchema->direction == "客户端->服务端", "ylGN direction");
        failures += Expect(stallStartSchema->legacyKind == "RECV", "ylGN legacyKind");
        failures += Expect(
            stallStartSchema->rawFormat == "{stallName} {stallDescription} {stallList}",
            "ylGN rawFormat");
        failures += Expect(
            stallStartSchema->resolvedFormat == "{stallName:es} {stallDescription:es} {stallList:es}",
            "ylGN resolvedFormat");
        failures += Expect(stallStartSchema->resolvedSource == "xlsx+src/packet", "ylGN resolvedSource");
        failures += Expect(stallStartSchema->fields.size() == 3U, "ylGN raw field count");
        if (stallStartSchema->fields.size() == 3U) {
            failures += Expect(stallStartSchema->fields[0].name == "stallName", "ylGN raw field stallName");
            failures += Expect(
                stallStartSchema->fields[1].name == "stallDescription",
                "ylGN raw field stallDescription");
            failures += Expect(stallStartSchema->fields[2].name == "stallList", "ylGN raw field stallList");
            failures += Expect(stallStartSchema->fields[0].typeToken == "unknown", "ylGN raw stallName type");
            failures += Expect(
                stallStartSchema->fields[1].typeToken == "unknown",
                "ylGN raw stallDescription type");
            failures += Expect(stallStartSchema->fields[2].typeToken == "unknown", "ylGN raw stallList type");
        }
        failures += Expect(static_cast<bool>(packet), "ylGN packet build");
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 3U, "ylGN packet field count");
            if (packet.Value().fields.size() == 3U) {
                failures += Expect(packet.Value().fields[0].name == "stallName", "ylGN packet stallName field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
                    "ylGN packet stallName type");
                failures += Expect(
                    packet.Value().fields[1].name == "stallDescription",
                    "ylGN packet stallDescription field");
                failures += Expect(
                    packet.Value().fields[1].type == cg::packet::PacketFieldType::EscapedString,
                    "ylGN packet stallDescription type");
                failures += Expect(packet.Value().fields[2].name == "stallList", "ylGN packet stallList field");
                failures += Expect(
                    packet.Value().fields[2].type == cg::packet::PacketFieldType::EscapedString,
                    "ylGN packet stallList type");
            }
        }
    }

    const PacketSchema* guildInfoChangeSchema = FindPacket(packets, "Ufm");
    failures += Expect(guildInfoChangeSchema != nullptr, "Ufm schema exists");
    if (guildInfoChangeSchema != nullptr) {
        const cg::packet::send::GuildInfoChange guildInfoChange(2, "data");
        const cg::packet::Packet packet = guildInfoChange.ToPacket();
        failures += Expect(guildInfoChangeSchema->direction == "客户端->服务端", "Ufm direction");
        failures += Expect(guildInfoChangeSchema->legacyKind == "RECV", "Ufm legacyKind");
        failures += Expect(guildInfoChangeSchema->rawFormat == "{c} {data:es}", "Ufm rawFormat");
        failures += Expect(
            guildInfoChangeSchema->resolvedFormat == "{c:i62} {data:es}",
            "Ufm resolvedFormat");
        failures += Expect(guildInfoChangeSchema->resolvedSource == "xlsx+src/packet", "Ufm resolvedSource");
        failures += Expect(guildInfoChangeSchema->fields.size() == 2U, "Ufm raw field count");
        if (guildInfoChangeSchema->fields.size() == 2U) {
            failures += Expect(guildInfoChangeSchema->fields[0].name == "c", "Ufm raw field c");
            failures += Expect(guildInfoChangeSchema->fields[1].name == "data", "Ufm raw field data");
            failures += Expect(guildInfoChangeSchema->fields[0].typeToken == "unknown", "Ufm raw c type");
            failures += Expect(guildInfoChangeSchema->fields[1].typeToken == "es", "Ufm raw data type");
        }
        failures += Expect(packet.fields.size() == 2U, "Ufm packet field count");
        if (packet.fields.size() == 2U) {
            failures += Expect(packet.fields[0].name == "c", "Ufm packet c field");
            failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, "Ufm packet c type");
            failures += Expect(packet.fields[1].name == "data", "Ufm packet data field");
            failures += Expect(
                packet.fields[1].type == cg::packet::PacketFieldType::EscapedString,
                "Ufm packet data type");
        }
    }

    const PacketSchema* guildTitleSetSchema = FindPacket(packets, "GY");
    failures += Expect(guildTitleSetSchema != nullptr, "GY schema exists");
    if (guildTitleSetSchema != nullptr) {
        const cg::packet::send::GuildTitleSet guildTitleSet(10, 128, "data");
        const cg::packet::Packet packet = guildTitleSet.ToPacket();
        failures += Expect(guildTitleSetSchema->direction == "客户端->服务端", "GY direction");
        failures += Expect(guildTitleSetSchema->legacyKind == "RECV", "GY legacyKind");
        failures += Expect(guildTitleSetSchema->rawFormat == "{index} {bitflag} {data:es}", "GY rawFormat");
        failures += Expect(
            guildTitleSetSchema->resolvedFormat == "{index:i62} {bitflag:i62} {data:es}",
            "GY resolvedFormat");
        failures += Expect(guildTitleSetSchema->resolvedSource == "xlsx+src/packet", "GY resolvedSource");
        failures += Expect(guildTitleSetSchema->fields.size() == 3U, "GY raw field count");
        if (guildTitleSetSchema->fields.size() == 3U) {
            failures += Expect(guildTitleSetSchema->fields[0].name == "index", "GY raw field index");
            failures += Expect(guildTitleSetSchema->fields[1].name == "bitflag", "GY raw field bitflag");
            failures += Expect(guildTitleSetSchema->fields[2].name == "data", "GY raw field data");
            failures += Expect(guildTitleSetSchema->fields[0].typeToken == "unknown", "GY raw index type");
            failures += Expect(guildTitleSetSchema->fields[1].typeToken == "unknown", "GY raw bitflag type");
            failures += Expect(guildTitleSetSchema->fields[2].typeToken == "es", "GY raw data type");
        }
        failures += Expect(packet.fields.size() == 3U, "GY packet field count");
        if (packet.fields.size() == 3U) {
            failures += Expect(packet.fields[0].name == "index", "GY packet index field");
            failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, "GY packet index type");
            failures += Expect(packet.fields[1].name == "bitflag", "GY packet bitflag field");
            failures += Expect(
                packet.fields[1].type == cg::packet::PacketFieldType::Int62,
                "GY packet bitflag type");
            failures += Expect(packet.fields[2].name == "data", "GY packet data field");
            failures += Expect(
                packet.fields[2].type == cg::packet::PacketFieldType::EscapedString,
                "GY packet data type");
        }
    }

    const PacketSchema* guildMailSendSchema = FindPacket(packets, "uId");
    failures += Expect(guildMailSendSchema != nullptr, "uId schema exists");
    if (guildMailSendSchema != nullptr) {
        const cg::packet::send::GuildMailSend guildMailSend(10, "text", 6);
        const cg::packet::Packet packet = guildMailSend.ToPacket();
        failures += Expect(guildMailSendSchema->direction == "客户端->服务端", "uId direction");
        failures += Expect(guildMailSendSchema->legacyKind == "RECV", "uId legacyKind");
        failures += Expect(
            guildMailSendSchema->rawFormat == "{toregistNumber} {text} {color}",
            "uId rawFormat");
        failures += Expect(
            guildMailSendSchema->resolvedFormat == "{toregistNumber:i62} {text:es} {color:i62}",
            "uId resolvedFormat");
        failures += Expect(guildMailSendSchema->resolvedSource == "xlsx+src/packet", "uId resolvedSource");
        failures += Expect(guildMailSendSchema->fields.size() == 3U, "uId raw field count");
        if (guildMailSendSchema->fields.size() == 3U) {
            failures += Expect(guildMailSendSchema->fields[0].name == "toregistNumber", "uId raw field toregistNumber");
            failures += Expect(guildMailSendSchema->fields[1].name == "text", "uId raw field text");
            failures += Expect(guildMailSendSchema->fields[2].name == "color", "uId raw field color");
            failures += Expect(guildMailSendSchema->fields[0].typeToken == "unknown", "uId raw toregistNumber type");
            failures += Expect(guildMailSendSchema->fields[1].typeToken == "unknown", "uId raw text type");
            failures += Expect(guildMailSendSchema->fields[2].typeToken == "unknown", "uId raw color type");
        }
        failures += Expect(packet.fields.size() == 3U, "uId packet field count");
        if (packet.fields.size() == 3U) {
            failures += Expect(packet.fields[0].name == "toregistNumber", "uId packet toregistNumber field");
            failures += Expect(
                packet.fields[0].type == cg::packet::PacketFieldType::Int62,
                "uId packet toregistNumber type");
            failures += Expect(packet.fields[1].name == "text", "uId packet text field");
            failures += Expect(
                packet.fields[1].type == cg::packet::PacketFieldType::EscapedString,
                "uId packet text type");
            failures += Expect(packet.fields[2].name == "color", "uId packet color field");
            failures += Expect(packet.fields[2].type == cg::packet::PacketFieldType::Int62, "uId packet color type");
        }
    }

    const PacketSchema* profileMailFriendDeleteSchema = FindPacket(packets, "wD");
    failures += Expect(profileMailFriendDeleteSchema != nullptr, "wD schema exists");
    if (profileMailFriendDeleteSchema != nullptr) {
        const cg::packet::send::ProfileMailFriendDelete profileMailFriendDelete(12, "Alice");
        const cg::packet::Packet packet = profileMailFriendDelete.ToPacket();
        failures += Expect(profileMailFriendDeleteSchema->direction == "客户端->服务端", "wD direction");
        failures += Expect(profileMailFriendDeleteSchema->legacyKind == "RECV", "wD legacyKind");
        failures += Expect(profileMailFriendDeleteSchema->rawFormat == "{} {name}", "wD rawFormat");
        failures += Expect(
            profileMailFriendDeleteSchema->resolvedFormat == "{profileIndex:i62} {name:es}",
            "wD resolvedFormat");
        failures += Expect(profileMailFriendDeleteSchema->resolvedSource == "xlsx+src/packet", "wD resolvedSource");
        failures += Expect(profileMailFriendDeleteSchema->fields.size() == 2U, "wD raw field count");
        if (profileMailFriendDeleteSchema->fields.size() == 2U) {
            failures += Expect(profileMailFriendDeleteSchema->fields[0].name.empty(), "wD raw field profileIndex");
            failures += Expect(profileMailFriendDeleteSchema->fields[1].name == "name", "wD raw field name");
            failures += Expect(profileMailFriendDeleteSchema->fields[0].typeToken == "unknown", "wD raw profileIndex type");
            failures += Expect(profileMailFriendDeleteSchema->fields[1].typeToken == "unknown", "wD raw name type");
        }
        failures += Expect(packet.fields.size() == 2U, "wD packet field count");
        if (packet.fields.size() == 2U) {
            failures += Expect(packet.fields[0].name == "profileIndex", "wD packet profileIndex field");
            failures += Expect(
                packet.fields[0].type == cg::packet::PacketFieldType::Int62,
                "wD packet profileIndex type");
            failures += Expect(packet.fields[1].name == "name", "wD packet name field");
            failures += Expect(
                packet.fields[1].type == cg::packet::PacketFieldType::EscapedString,
                "wD packet name type");
        }
    }

    const PacketSchema* profileMiniMailSendSchema = FindPacket(packets, "GH");
    failures += Expect(profileMiniMailSendSchema != nullptr, "GH schema exists");
    if (profileMiniMailSendSchema != nullptr) {
        const cg::packet::send::ProfileMiniMailSend profileMiniMailSend(12, "Alice", "hello", 3);
        const cg::packet::PacketResult<cg::packet::Packet> packet = profileMiniMailSend.ToPacket();
        failures += Expect(profileMiniMailSendSchema->direction == "客户端->服务端", "GH direction");
        failures += Expect(profileMiniMailSendSchema->legacyKind == "RECV", "GH legacyKind");
        failures += Expect(profileMiniMailSendSchema->rawFormat == "{data} {color}", "GH rawFormat");
        failures += Expect(
            profileMiniMailSendSchema->resolvedFormat == "{data:es} {color:i62}",
            "GH resolvedFormat");
        failures += Expect(profileMiniMailSendSchema->resolvedSource == "xlsx+src/packet", "GH resolvedSource");
        failures += Expect(profileMiniMailSendSchema->fields.size() == 2U, "GH raw field count");
        if (profileMiniMailSendSchema->fields.size() == 2U) {
            failures += Expect(profileMiniMailSendSchema->fields[0].name == "data", "GH raw field data");
            failures += Expect(profileMiniMailSendSchema->fields[1].name == "color", "GH raw field color");
            failures += Expect(profileMiniMailSendSchema->fields[0].typeToken == "unknown", "GH raw data type");
            failures += Expect(profileMiniMailSendSchema->fields[1].typeToken == "unknown", "GH raw color type");
        }
        failures += Expect(static_cast<bool>(packet), "GH packet build");
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 2U, "GH packet field count");
            if (packet.Value().fields.size() == 2U) {
                failures += Expect(packet.Value().fields[0].name == "data", "GH packet data field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
                    "GH packet data type");
                failures += Expect(packet.Value().fields[1].name == "color", "GH packet color field");
                failures += Expect(
                    packet.Value().fields[1].type == cg::packet::PacketFieldType::Int62,
                    "GH packet color type");
            }
        }
    }

    const PacketSchema* checkCommandSchema = FindPacket(packets, "CCL");
    failures += Expect(checkCommandSchema != nullptr, "CCL schema exists");
    if (checkCommandSchema != nullptr) {
        failures += Expect(checkCommandSchema->rawFormat == "{time:i62}", "CCL rawFormat");
        failures += Expect(checkCommandSchema->fields.size() == 1U, "CCL field count");
        if (checkCommandSchema->fields.size() == 1U) {
            failures += Expect(checkCommandSchema->fields[0].name == "time", "CCL field name");
            failures += Expect(checkCommandSchema->fields[0].typeToken == "i62", "CCL field type");
            failures += Expect(checkCommandSchema->fields[0].rawSpec == "{time:i62}", "CCL rawSpec");
            failures += Expect(checkCommandSchema->fields[0].source == "xlsx", "CCL field source");
        }
    }

    const PacketSchema* profileVersionSchema = FindPacket(packets, "HNP");
    failures += Expect(profileVersionSchema != nullptr, "HNP schema exists");
    if (profileVersionSchema != nullptr) {
        failures += Expect(
            profileVersionSchema->description == "SEND_PRV \nCHAR_sendProfileVersion简介版本内容",
            "HNP description newline");
    }

    const PacketSchema* equipSuitSchema = FindPacket(packets, "TgX");
    failures += Expect(equipSuitSchema != nullptr, "TgX schema exists");
    if (equipSuitSchema != nullptr) {
        failures += Expect(equipSuitSchema->rawFormat == "{data}", "TgX rawFormat");
        failures += Expect(equipSuitSchema->resolvedFormat == "{data:es}", "TgX resolvedFormat");
        failures += Expect(
            equipSuitSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "TgX resolvedSource");
        failures += Expect(equipSuitSchema->fields.size() == 1U, "TgX field count");
        if (equipSuitSchema->fields.size() == 1U) {
            failures += Expect(equipSuitSchema->fields[0].name == "data", "TgX field name");
            failures += Expect(equipSuitSchema->fields[0].typeToken == "unknown", "TgX field type");
            failures += Expect(equipSuitSchema->fields[0].rawSpec == "{data}", "TgX rawSpec");
        }
    }

    const PacketSchema* addressBookSchema = FindPacket(packets, "gnxd");
    failures += Expect(addressBookSchema != nullptr, "gnxd schema exists");
    if (addressBookSchema != nullptr) {
        const cg::packet::receive::AddressBook addressBook;
        const cg::packet::PacketResult<cg::packet::Packet> packet = addressBook.ToPacket();
        failures += Expect(addressBookSchema->direction == "服务端->客户端", "gnxd direction");
        failures += Expect(addressBookSchema->legacyKind == "SEND", "gnxd legacyKind");
        failures += Expect(addressBookSchema->rawFormat == "{data:es}", "gnxd rawFormat");
        failures += Expect(addressBookSchema->resolvedFormat == "{data:es}", "gnxd resolvedFormat");
        failures += Expect(addressBookSchema->resolvedSource == "xlsx+src/packet", "gnxd resolvedSource");
        failures += Expect(packet.HasValue(), "gnxd packet build");
        if (packet.HasValue()) {
            failures += ExpectPacketFieldsMatch(*addressBookSchema, packet.Value(), "gnxd field order");
        }
    }

    const PacketSchema* addressBookGuildInfoSchema = FindPacket(packets, "PhdQ");
    failures += Expect(addressBookGuildInfoSchema != nullptr, "PhdQ schema exists");
    if (addressBookGuildInfoSchema != nullptr) {
        const cg::packet::receive::AddressBookGuildInfo guildInfo;
        const cg::packet::PacketResult<cg::packet::Packet> packet = guildInfo.ToPacket();
        failures += Expect(addressBookGuildInfoSchema->direction == "服务端->客户端", "PhdQ direction");
        failures += Expect(addressBookGuildInfoSchema->legacyKind == "SEND", "PhdQ legacyKind");
        failures += Expect(addressBookGuildInfoSchema->rawFormat == "{data:es}", "PhdQ rawFormat");
        failures += Expect(addressBookGuildInfoSchema->resolvedFormat == "{data:es}", "PhdQ resolvedFormat");
        failures += Expect(addressBookGuildInfoSchema->resolvedSource == "xlsx+src/packet", "PhdQ resolvedSource");
        failures += Expect(packet.HasValue(), "PhdQ packet build");
        if (packet.HasValue()) {
            failures += ExpectPacketFieldsMatch(*addressBookGuildInfoSchema, packet.Value(), "PhdQ field order");
        }
    }

    const PacketSchema* addressBookInfoSchema = FindPacket(packets, "vI");
    failures += Expect(addressBookInfoSchema != nullptr, "vI schema exists");
    if (addressBookInfoSchema != nullptr) {
        const cg::packet::receive::AddressBookInfo info;
        const cg::packet::Packet packet = info.ToPacket();
        failures += Expect(addressBookInfoSchema->direction == "服务端->客户端", "vI direction");
        failures += Expect(addressBookInfoSchema->legacyKind == "SEND", "vI legacyKind");
        failures += Expect(addressBookInfoSchema->rawFormat == "{registry} {data}", "vI rawFormat");
        failures += Expect(addressBookInfoSchema->resolvedFormat == "{registry:i62} {data:es}", "vI resolvedFormat");
        failures += Expect(addressBookInfoSchema->resolvedSource == "xlsx+ClientIda+src/packet", "vI resolvedSource");
        failures += Expect(addressBookInfoSchema->fields.size() == 2U, "vI field count");
        if (addressBookInfoSchema->fields.size() == 2U) {
            failures += Expect(addressBookInfoSchema->fields[0].name == packet.fields[0].name, "vI registry field");
            failures += Expect(addressBookInfoSchema->fields[1].name == packet.fields[1].name, "vI data field");
            failures += Expect(addressBookInfoSchema->fields[0].typeToken == "unknown", "vI raw registry type");
            failures += Expect(addressBookInfoSchema->fields[1].typeToken == "unknown", "vI raw data type");
        }
    }

    const PacketSchema* guildInfoSchema = FindPacket(packets, "aDn");
    failures += Expect(guildInfoSchema != nullptr, "aDn schema exists");
    if (guildInfoSchema != nullptr) {
        const cg::packet::receive::GuildInfo guildInfo;
        const cg::packet::PacketResult<cg::packet::Packet> packet = guildInfo.ToPacket();
        failures += Expect(guildInfoSchema->direction == "服务端->客户端", "aDn direction");
        failures += Expect(guildInfoSchema->legacyKind == "SEND", "aDn legacyKind");
        failures += Expect(guildInfoSchema->rawFormat == "{data}", "aDn rawFormat");
        failures += Expect(guildInfoSchema->resolvedFormat == "{data:es}", "aDn resolvedFormat");
        failures += Expect(guildInfoSchema->resolvedSource == "xlsx+ClientIda+src/packet", "aDn resolvedSource");
        failures += Expect(guildInfoSchema->fields.size() == 1U, "aDn field count");
        failures += Expect(packet.HasValue(), "aDn packet build");
        if (guildInfoSchema->fields.size() == 1U) {
            failures += Expect(guildInfoSchema->fields[0].name == "data", "aDn data field");
            failures += Expect(guildInfoSchema->fields[0].typeToken == "unknown", "aDn raw data type");
            failures += Expect(guildInfoSchema->fields[0].rawSpec == "{data}", "aDn rawSpec");
        }
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 1U, "aDn packet field count");
            if (packet.Value().fields.size() == 1U) {
                failures += Expect(packet.Value().fields[0].name == "data", "aDn packet data field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
                    "aDn packet data type");
            }
        }
    }

    const PacketSchema* guildTitleSchema = FindPacket(packets, "znv");
    failures += Expect(guildTitleSchema != nullptr, "znv schema exists");
    if (guildTitleSchema != nullptr) {
        const cg::packet::receive::GuildTitle guildTitle;
        const cg::packet::Packet packet = guildTitle.ToPacket();
        failures += Expect(guildTitleSchema->direction == "服务端->客户端", "znv direction");
        failures += Expect(guildTitleSchema->legacyKind == "SEND", "znv legacyKind");
        failures += Expect(guildTitleSchema->rawFormat == "{data}", "znv rawFormat");
        failures += Expect(guildTitleSchema->resolvedFormat == "{data:es}", "znv resolvedFormat");
        failures += Expect(guildTitleSchema->resolvedSource == "xlsx+src/packet", "znv resolvedSource");
        failures += Expect(guildTitleSchema->fields.size() == 1U, "znv field count");
        if (guildTitleSchema->fields.size() == 1U) {
            failures += Expect(guildTitleSchema->fields[0].name == "data", "znv data field");
            failures += Expect(guildTitleSchema->fields[0].typeToken == "unknown", "znv raw data type");
            failures += Expect(guildTitleSchema->fields[0].rawSpec == "{data}", "znv rawSpec");
        }
        failures += Expect(packet.fields.size() == 1U, "znv packet field count");
        if (packet.fields.size() == 1U) {
            failures += Expect(packet.fields[0].name == "data", "znv packet data field");
            failures += Expect(
                packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
                "znv packet data type");
        }
    }

    const PacketSchema* guildMemberSchema = FindPacket(packets, "yuD");
    failures += Expect(guildMemberSchema != nullptr, "yuD schema exists");
    if (guildMemberSchema != nullptr) {
        const cg::packet::receive::GuildMember guildMember;
        const cg::packet::PacketResult<cg::packet::Packet> packet = guildMember.ToPacket();
        failures += Expect(guildMemberSchema->direction == "服务端->客户端", "yuD direction");
        failures += Expect(guildMemberSchema->legacyKind == "SEND", "yuD legacyKind");
        failures += Expect(guildMemberSchema->rawFormat == "{data}", "yuD rawFormat");
        failures += Expect(guildMemberSchema->resolvedFormat == "{data:es}", "yuD resolvedFormat");
        failures += Expect(guildMemberSchema->resolvedSource == "xlsx+src/packet", "yuD resolvedSource");
        failures += Expect(guildMemberSchema->fields.size() == 1U, "yuD field count");
        failures += Expect(packet.HasValue(), "yuD packet build");
        if (guildMemberSchema->fields.size() == 1U) {
            failures += Expect(guildMemberSchema->fields[0].name == "data", "yuD data field");
            failures += Expect(guildMemberSchema->fields[0].typeToken == "unknown", "yuD raw data type");
            failures += Expect(guildMemberSchema->fields[0].rawSpec == "{data}", "yuD rawSpec");
        }
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 1U, "yuD packet field count");
            if (packet.Value().fields.size() == 1U) {
                failures += Expect(packet.Value().fields[0].name == "data", "yuD packet data field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
                    "yuD packet data type");
            }
        }
    }

    const PacketSchema* guildMemberInfoSchema = FindPacket(packets, "Kka");
    failures += Expect(guildMemberInfoSchema != nullptr, "Kka schema exists");
    if (guildMemberInfoSchema != nullptr) {
        const cg::packet::receive::GuildMemberInfo guildMemberInfo(10, 1, 123, "Alice", 123, 45, 1, 987, 2);
        const cg::packet::PacketResult<cg::packet::Packet> packet = guildMemberInfo.ToPacket();
        failures += Expect(guildMemberInfoSchema->direction == "服务端->客户端", "Kka direction");
        failures += Expect(guildMemberInfoSchema->legacyKind == "SEND", "Kka legacyKind");
        failures += Expect(guildMemberInfoSchema->rawFormat == "{index} {data}", "Kka rawFormat");
        failures += Expect(guildMemberInfoSchema->resolvedFormat == "{index:i62} {data:es}", "Kka resolvedFormat");
        failures += Expect(guildMemberInfoSchema->resolvedSource == "xlsx+ServerC+src", "Kka resolvedSource");
        failures += Expect(guildMemberInfoSchema->fields.size() == 2U, "Kka field count");
        failures += Expect(packet.HasValue(), "Kka packet build");
        if (guildMemberInfoSchema->fields.size() == 2U) {
            failures += Expect(guildMemberInfoSchema->fields[0].name == "index", "Kka index field");
            failures += Expect(guildMemberInfoSchema->fields[1].name == "data", "Kka data field");
            failures += Expect(guildMemberInfoSchema->fields[0].typeToken == "unknown", "Kka raw index type");
            failures += Expect(guildMemberInfoSchema->fields[1].typeToken == "unknown", "Kka raw data type");
        }
        if (packet.HasValue()) {
            failures += Expect(packet.Value().fields.size() == 2U, "Kka packet field count");
            if (packet.Value().fields.size() == 2U) {
                failures += Expect(packet.Value().fields[0].name == "index", "Kka packet index field");
                failures += Expect(
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::Int62,
                    "Kka packet index type");
                failures += Expect(packet.Value().fields[1].name == "data", "Kka packet data field");
                failures += Expect(
                    packet.Value().fields[1].type == cg::packet::PacketFieldType::EscapedString,
                    "Kka packet data type");
            }
        }
    }

    const PacketSchema* guildMailSchema = FindPacket(packets, "Rll");
    failures += Expect(guildMailSchema != nullptr, "Rll schema exists");
    if (guildMailSchema != nullptr) {
        cg::packet::receive::GuildMailPayload payload;
        payload.sentTimeText = "time";
        payload.messageRaw = "message";
        payload.petBaseImageNumberOrMarker = -1;
        const cg::packet::receive::GuildMail guildMail(1, std::move(payload), 3);
        const cg::packet::Packet packet = guildMail.ToPacket();
        failures += Expect(guildMailSchema->direction == "服务端->客户端", "Rll direction");
        failures += Expect(guildMailSchema->legacyKind == "SEND", "Rll legacyKind");
        failures += Expect(guildMailSchema->rawFormat == "{fromregistseqnumber} {text} {color}", "Rll rawFormat");
        failures += Expect(
            guildMailSchema->resolvedFormat == "{fromregistseqnumber:i62} {text:es} {color:i62}",
            "Rll resolvedFormat");
        failures += Expect(guildMailSchema->resolvedSource == "xlsx+src/packet", "Rll resolvedSource");
        failures += Expect(guildMailSchema->fields.size() == 3U, "Rll field count");
        if (guildMailSchema->fields.size() == 3U) {
            failures += Expect(guildMailSchema->fields[0].name == "fromregistseqnumber", "Rll from field");
            failures += Expect(guildMailSchema->fields[1].name == "text", "Rll text field");
            failures += Expect(guildMailSchema->fields[2].name == "color", "Rll color field");
            failures += Expect(guildMailSchema->fields[0].typeToken == "unknown", "Rll raw from type");
            failures += Expect(guildMailSchema->fields[1].typeToken == "unknown", "Rll raw text type");
            failures += Expect(guildMailSchema->fields[2].typeToken == "unknown", "Rll raw color type");
        }
        failures += Expect(packet.fields.size() == 3U, "Rll packet field count");
        if (packet.fields.size() == 3U) {
            failures += Expect(packet.fields[0].name == "fromregistseqnumber", "Rll packet from field");
            failures += Expect(
                packet.fields[0].type == cg::packet::PacketFieldType::Int62,
                "Rll packet from type");
            failures += Expect(packet.fields[1].name == "text", "Rll packet text field");
            failures += Expect(
                packet.fields[1].type == cg::packet::PacketFieldType::EscapedString,
                "Rll packet text type");
            failures += Expect(packet.fields[2].name == "color", "Rll packet color field");
            failures += Expect(
                packet.fields[2].type == cg::packet::PacketFieldType::Int62,
                "Rll packet color type");
        }
    }

    const PacketSchema* petMailSendSchema = FindPacket(packets, "rMsv");
    failures += Expect(petMailSendSchema != nullptr, "rMsv schema exists");
    if (petMailSendSchema != nullptr) {
        failures += Expect(petMailSendSchema->direction == "客户端->服务端", "rMsv direction");
        failures += Expect(petMailSendSchema->legacyKind == "RECV", "rMsv legacyKind");
        failures += Expect(
            petMailSendSchema->resolvedFormat == "{index:i62} {petindex:i62} {itemindex:i62} {message:es} {color:i62}",
            "rMsv resolvedFormat");
        failures += Expect(
            petMailSendSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "rMsv resolvedSource");
        failures += Expect(petMailSendSchema->fields.size() == 5U, "rMsv field count");
        if (petMailSendSchema->fields.size() == 5U) {
            failures += Expect(petMailSendSchema->fields[0].name == "index", "rMsv index field");
            failures += Expect(petMailSendSchema->fields[1].name == "petindex", "rMsv petindex field");
            failures += Expect(petMailSendSchema->fields[2].name == "itemindex", "rMsv itemindex field");
            failures += Expect(petMailSendSchema->fields[3].name == "message", "rMsv message field");
            failures += Expect(petMailSendSchema->fields[4].name == "color", "rMsv color field");
            failures += Expect(petMailSendSchema->fields[0].typeToken == "unknown", "rMsv raw type");
            failures += Expect(petMailSendSchema->fields[3].rawSpec == "{message}", "rMsv message rawSpec");
        }
    }

    const PacketSchema* petMailRegisterSchema = FindPacket(packets, "PfqV");
    failures += Expect(petMailRegisterSchema != nullptr, "PfqV schema exists");
    if (petMailRegisterSchema != nullptr) {
        failures += Expect(petMailRegisterSchema->direction == "客户端->服务端", "PfqV direction");
        failures += Expect(petMailRegisterSchema->legacyKind == "RECV", "PfqV legacyKind");
        failures += Expect(
            petMailRegisterSchema->resolvedFormat ==
                "{toregist:i62} {petSlot:i62} {itemSlot:i62} {text:es} {color:i62}",
            "PfqV resolvedFormat");
        failures += Expect(
            petMailRegisterSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "PfqV resolvedSource");
        failures += Expect(petMailRegisterSchema->fields.size() == 5U, "PfqV field count");
        if (petMailRegisterSchema->fields.size() == 5U) {
            failures += Expect(petMailRegisterSchema->fields[0].name == "toregist", "PfqV toregist field");
            failures += Expect(petMailRegisterSchema->fields[1].name == "petSlot", "PfqV petSlot field");
            failures += Expect(petMailRegisterSchema->fields[2].name == "itemSlot", "PfqV itemSlot field");
            failures += Expect(petMailRegisterSchema->fields[3].name == "text", "PfqV text field");
            failures += Expect(petMailRegisterSchema->fields[4].name == "color", "PfqV color field");
            failures += Expect(petMailRegisterSchema->fields[0].typeToken == "unknown", "PfqV raw type");
            failures += Expect(petMailRegisterSchema->fields[3].rawSpec == "{text}", "PfqV text rawSpec");
        }
    }
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-coverage-schema-test <schema-json>\n";
        return 1;
    }

    const std::string jsonText = ReadTextFile(argv[1]);
    int failures = Expect(!jsonText.empty(), "schema file non-empty");
    if (jsonText.empty()) {
        return failures;
    }

    JsonValue root;
    JsonParser parser(jsonText);
    failures += Expect(parser.Parse(root), "schema json parse");
    if (failures != 0) {
        std::cerr << "JSON parse error: " << parser.Error() << '\n';
        return failures;
    }

    std::vector<PacketSchema> packets;
    failures += TestShape(root, packets);
    failures += TestCoverage(packets);
    failures += TestKnownPackets(packets);
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
