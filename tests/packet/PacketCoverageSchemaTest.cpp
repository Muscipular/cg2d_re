#include "packet/receive.hpp"
#include "packet/send.hpp"

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
        return "es";
    case cg::packet::PacketFieldType::RawString:
        return "raw";
    case cg::packet::PacketFieldType::Payload:
        return "payload";
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
