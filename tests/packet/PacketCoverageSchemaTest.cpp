#include "PacketFixtureInventory.hpp"
#include "TypedPacketManifest.hpp"

#include <charconv>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
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

struct RepeatedTailSchema
{
    int fieldIndex = -1;
    int countFieldIndex = -1;
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
    std::string contractStatus;
    bool hasResolvedFormat = false;
    std::string resolvedFormat;
    std::string resolvedSource;
    std::vector<FieldSchema> fields;
    bool hasResolvedFields = false;
    std::vector<FieldSchema> resolvedFields;
    bool hasRepeatedTail = false;
    RepeatedTailSchema repeatedTail;
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
    packet.contractStatus = OptionalString(object, "contractStatus", failures);
    packet.resolvedSource = OptionalString(object, "resolvedSource", failures);

    const JsonValue* resolvedFormat = object.Find("resolvedFormat");
    packet.hasResolvedFormat = resolvedFormat != nullptr;
    if (resolvedFormat != nullptr) {
        failures += Expect(resolvedFormat->kind == JsonValue::Kind::String, "resolvedFormat string");
        if (resolvedFormat->kind == JsonValue::Kind::String) {
            packet.resolvedFormat = resolvedFormat->text;
        }
    }

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
    packet.hasResolvedFields = resolvedFields != nullptr;
    if (resolvedFields != nullptr) {
        failures += Expect(resolvedFields->kind == JsonValue::Kind::Array, "resolvedFields array");
        if (resolvedFields->kind == JsonValue::Kind::Array) {
            packet.resolvedFields.reserve(resolvedFields->array.size());
            for (const JsonValue& field : resolvedFields->array) {
                packet.resolvedFields.push_back(ReadFieldSchema(field, failures));
            }
        }
    }

    const JsonValue* repeatedTail = object.Find("repeatedTail");
    packet.hasRepeatedTail = repeatedTail != nullptr;
    if (repeatedTail != nullptr) {
        failures += Expect(repeatedTail->kind == JsonValue::Kind::Object, "repeatedTail object");
        if (repeatedTail->kind == JsonValue::Kind::Object) {
            packet.repeatedTail.fieldIndex = RequiredInt(*repeatedTail, "fieldIndex", failures);
            packet.repeatedTail.countFieldIndex =
                RequiredInt(*repeatedTail, "countFieldIndex", failures);
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

struct PacketFixtureSample
{
    std::string name;
    std::string plain;
    std::string canonical;
    std::optional<std::size_t> payloadField;
    std::string payload;
};

struct PacketFixture
{
    std::string code;
    cg::packet::PacketDirection direction = cg::packet::PacketDirection::ClientToServer;
    std::vector<PacketFixtureSample> samples;
};

class PacketFixtureParser
{
public:
    explicit PacketFixtureParser(std::string_view text)
        : text_(text)
    {
    }

    [[nodiscard]] bool Parse(PacketFixture& fixture)
    {
        fixture = {};
        lines_.clear();
        error_.clear();

        if (text_.empty() || text_.back() != '\n') {
            return Fail("fixture 缺少最终 LF");
        }
        for (const unsigned char byte : text_) {
            if (byte != '\n' && (byte < 0x20U || byte > 0x7eU)) {
                return Fail("fixture 含非 ASCII printable/LF 字节");
            }
        }

        std::size_t position = 0U;
        while (position < text_.size()) {
            const std::size_t newline = text_.find('\n', position);
            if (newline == std::string_view::npos) {
                return Fail("fixture 缺少 LF");
            }
            const std::string_view line = text_.substr(position, newline - position);
            if (line.empty()) {
                return Fail("fixture 含空行");
            }
            lines_.push_back(line);
            position = newline + 1U;
        }

        std::size_t lineIndex = 0U;
        if (!TakeExact(lineIndex, "pkt_fixture=1")) {
            return false;
        }

        std::string_view value;
        if (!TakeValue(lineIndex, "code=", false, value) || !IsCode(value)) {
            return Fail("fixture code 非法");
        }
        fixture.code = value;

        if (!TakeValue(lineIndex, "direction=", false, value)) {
            return false;
        }
        if (value == "client_to_server") {
            fixture.direction = cg::packet::PacketDirection::ClientToServer;
        } else if (value == "server_to_client") {
            fixture.direction = cg::packet::PacketDirection::ServerToClient;
        } else {
            return Fail("fixture direction 非法");
        }

        std::set<std::string, std::less<>> sampleNames;
        while (lineIndex < lines_.size()) {
            PacketFixtureSample sample;
            if (!TakeValue(lineIndex, "sample=", false, value) || !IsSampleName(value)) {
                return Fail("fixture sample 非法");
            }
            sample.name = value;
            if (!sampleNames.emplace(sample.name).second) {
                return Fail("fixture sample 重复");
            }

            if (!TakeValue(lineIndex, "plain_hex=", false, value)
                || !DecodeHex(value, false, sample.plain)) {
                return Fail("fixture plain_hex 非法");
            }
            if (!ValidatePlain(sample.plain)) {
                return Fail("fixture plain 控制字节非法");
            }

            if (!TakeValue(lineIndex, "canonical_hex=", false, value)
                || !DecodeHex(value, false, sample.canonical)) {
                return Fail("fixture canonical_hex 非法");
            }
            if (!ValidateCanonical(sample.canonical)) {
                return Fail("fixture canonical 控制字节或结尾非法");
            }

            if (lineIndex < lines_.size() && StartsWith(lines_[lineIndex], "payload_field=")) {
                if (!TakeValue(lineIndex, "payload_field=", false, value)) {
                    return false;
                }
                std::size_t fieldIndex = 0U;
                if (!ParseFieldIndex(value, fieldIndex)) {
                    return Fail("fixture payload_field 非法");
                }
                sample.payloadField = fieldIndex;

                if (!TakeValue(lineIndex, "payload_hex=", true, value)
                    || !DecodeHex(value, true, sample.payload)) {
                    return Fail("fixture payload_hex 非法");
                }
            }

            if (!TakeExact(lineIndex, "end_sample")) {
                return false;
            }
            fixture.samples.push_back(std::move(sample));
        }

        if (fixture.samples.empty()) {
            return Fail("fixture 至少需要一个 sample");
        }
        return true;
    }

    [[nodiscard]] std::string_view Error() const noexcept
    {
        return error_;
    }

private:
    [[nodiscard]] static bool StartsWith(std::string_view text, std::string_view prefix) noexcept
    {
        return text.size() >= prefix.size() && text.substr(0U, prefix.size()) == prefix;
    }

    [[nodiscard]] bool TakeExact(std::size_t& lineIndex, std::string_view expected)
    {
        if (lineIndex >= lines_.size() || lines_[lineIndex] != expected) {
            return Fail("fixture key 缺失、乱序或未知");
        }
        ++lineIndex;
        return true;
    }

    [[nodiscard]] bool TakeValue(
        std::size_t& lineIndex,
        std::string_view prefix,
        bool allowEmpty,
        std::string_view& value)
    {
        if (lineIndex >= lines_.size() || !StartsWith(lines_[lineIndex], prefix)) {
            return Fail("fixture key 缺失、乱序或未知");
        }
        value = lines_[lineIndex].substr(prefix.size());
        if (!allowEmpty && value.empty()) {
            return Fail("fixture value 为空");
        }
        ++lineIndex;
        return true;
    }

    [[nodiscard]] static bool IsCode(std::string_view value) noexcept
    {
        if (value.empty()) {
            return false;
        }
        for (const char ch : value) {
            if (!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')
                    || (ch >= '0' && ch <= '9'))) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] static bool IsSampleName(std::string_view value) noexcept
    {
        if (value.empty() || !((value.front() >= 'a' && value.front() <= 'z')
                || (value.front() >= '0' && value.front() <= '9'))) {
            return false;
        }
        for (const char ch : value) {
            if (!((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')
                    || ch == '_' || ch == '-')) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] static int HexDigit(char ch) noexcept
    {
        if (ch >= '0' && ch <= '9') {
            return ch - '0';
        }
        if (ch >= 'A' && ch <= 'F') {
            return ch - 'A' + 10;
        }
        return -1;
    }

    [[nodiscard]] static bool DecodeHex(
        std::string_view value,
        bool allowEmpty,
        std::string& bytes)
    {
        bytes.clear();
        if (value.empty()) {
            return allowEmpty;
        }
        if ((value.size() + 1U) % 3U != 0U) {
            return false;
        }

        bytes.reserve((value.size() + 1U) / 3U);
        for (std::size_t index = 0U; index < value.size(); index += 3U) {
            const int high = HexDigit(value[index]);
            const int low = HexDigit(value[index + 1U]);
            if (high < 0 || low < 0) {
                return false;
            }
            bytes.push_back(static_cast<char>((high << 4) | low));
            if (index + 2U < value.size() && value[index + 2U] != ' ') {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] static bool ParseFieldIndex(
        std::string_view value,
        std::size_t& fieldIndex) noexcept
    {
        if (value.empty() || (value.size() > 1U && value.front() == '0')) {
            return false;
        }
        const char* const begin = value.data();
        const char* const end = begin + value.size();
        const auto [parsedEnd, error] = std::from_chars(begin, end, fieldIndex);
        return error == std::errc() && parsedEnd == end;
    }

    [[nodiscard]] static bool ValidatePlain(std::string_view bytes) noexcept
    {
        std::size_t controlCount = 0U;
        for (std::size_t index = 0U; index < bytes.size(); ++index) {
            const unsigned char byte = static_cast<unsigned char>(bytes[index]);
            if (byte == 0x00U || byte == 0x0aU || byte == 0x0dU) {
                ++controlCount;
                if (controlCount > 1U || index + 1U != bytes.size()) {
                    return false;
                }
            }
        }
        return true;
    }

    [[nodiscard]] static bool ValidateCanonical(std::string_view bytes) noexcept
    {
        if (bytes.empty() || static_cast<unsigned char>(bytes.back()) != 0x20U) {
            return false;
        }
        for (const unsigned char byte : bytes) {
            if (byte == 0x00U || byte == 0x0aU || byte == 0x0dU) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool Fail(std::string_view error)
    {
        if (error_.empty()) {
            error_ = error;
        }
        return false;
    }

    std::string_view text_;
    std::vector<std::string_view> lines_;
    std::string error_;
};

int TestFixtureParserGrammar()
{
    const std::string valid =
        "pkt_fixture=1\n"
        "code=SWITMP2\n"
        "direction=client_to_server\n"
        "sample=basic-0\n"
        "plain_hex=53 57 49 54 4D 50 32\n"
        "canonical_hex=53 57 49 54 4D 50 32 20\n"
        "payload_field=0\n"
        "payload_hex=\n"
        "end_sample\n";

    int failures = 0;
    PacketFixture fixture;
    PacketFixtureParser validParser(valid);
    failures += Expect(validParser.Parse(fixture), "fixture grammar valid");

    const std::string bom = std::string("\xef\xbb\xbf", 3U) + valid;
    PacketFixtureParser bomParser(bom);
    failures += Expect(!bomParser.Parse(fixture), "fixture grammar rejects BOM");

    std::string crlf;
    for (const char ch : valid) {
        if (ch == '\n') {
            crlf.append("\r\n");
        } else {
            crlf.push_back(ch);
        }
    }
    PacketFixtureParser crlfParser(crlf);
    failures += Expect(!crlfParser.Parse(fixture), "fixture grammar rejects CRLF");

    PacketFixtureParser missingFinalLfParser(
        std::string_view(valid).substr(0U, valid.size() - 1U));
    failures += Expect(
        !missingFinalLfParser.Parse(fixture),
        "fixture grammar rejects missing final LF");

    const std::string disordered =
        "pkt_fixture=1\n"
        "code=A1\n"
        "direction=client_to_server\n"
        "sample=basic\n"
        "canonical_hex=41 31 20\n"
        "plain_hex=41 31\n"
        "end_sample\n";
    PacketFixtureParser disorderedParser(disordered);
    failures += Expect(!disorderedParser.Parse(fixture), "fixture grammar rejects disorder");

    const std::string badHex =
        "pkt_fixture=1\n"
        "code=A1\n"
        "direction=client_to_server\n"
        "sample=basic\n"
        "plain_hex=41 3a\n"
        "canonical_hex=41 31 20\n"
        "end_sample\n";
    PacketFixtureParser badHexParser(badHex);
    failures += Expect(!badHexParser.Parse(fixture), "fixture grammar rejects bad hex");

    const std::string duplicateSample =
        "pkt_fixture=1\n"
        "code=A1\n"
        "direction=client_to_server\n"
        "sample=basic\n"
        "plain_hex=41 31\n"
        "canonical_hex=41 31 20\n"
        "end_sample\n"
        "sample=basic\n"
        "plain_hex=41 31\n"
        "canonical_hex=41 31 20\n"
        "end_sample\n";
    PacketFixtureParser duplicateSampleParser(duplicateSample);
    failures += Expect(
        !duplicateSampleParser.Parse(fixture),
        "fixture grammar rejects duplicate sample");
    return failures;
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

bool IsFormatWhitespace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

std::vector<FieldSchema> ParseResolvedFormat(std::string_view format, int& failures)
{
    std::vector<FieldSchema> fields;
    std::size_t position = 0U;
    while (position < format.size()) {
        while (position < format.size() && IsFormatWhitespace(format[position])) {
            ++position;
        }
        if (position == format.size()) {
            break;
        }

        failures += Expect(format[position] == '{', "resolvedFormat field open");
        if (format[position] != '{') {
            return fields;
        }

        const std::size_t close = format.find('}', position + 1U);
        failures += Expect(close != std::string_view::npos, "resolvedFormat field close");
        if (close == std::string_view::npos) {
            return fields;
        }

        const std::string_view token = format.substr(position + 1U, close - position - 1U);
        const std::size_t colon = token.find(':');
        failures += Expect(colon != std::string_view::npos, "resolvedFormat field type separator");
        failures += Expect(colon != 0U, "resolvedFormat field name non-empty");
        failures += Expect(
            colon != std::string_view::npos && colon + 1U < token.size(),
            "resolvedFormat field type non-empty");
        failures += Expect(
            colon == std::string_view::npos || token.find(':', colon + 1U) == std::string_view::npos,
            "resolvedFormat field single type separator");
        if (colon == std::string_view::npos || colon == 0U || colon + 1U >= token.size()
            || token.find(':', colon + 1U) != std::string_view::npos) {
            return fields;
        }

        FieldSchema field;
        field.index = static_cast<int>(fields.size());
        field.name = token.substr(0U, colon);
        field.typeToken = token.substr(colon + 1U);
        fields.push_back(std::move(field));
        position = close + 1U;
    }
    return fields;
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

int ExpectRepeatedTailFieldsMatch(
    const PacketSchema& schema,
    const cg::packet::Packet& packet,
    const char* message)
{
    int failures = 0;
    failures += Expect(schema.hasRepeatedTail, message);
    failures += Expect(!schema.resolvedFields.empty(), message);
    if (!schema.hasRepeatedTail || schema.resolvedFields.empty()) {
        return failures;
    }

    const std::size_t repeatedFieldIndex =
        static_cast<std::size_t>(schema.repeatedTail.fieldIndex);
    failures += Expect(repeatedFieldIndex + 1U == schema.resolvedFields.size(), message);
    failures += Expect(packet.fields.size() >= repeatedFieldIndex, message);
    if (repeatedFieldIndex + 1U != schema.resolvedFields.size() ||
        packet.fields.size() < repeatedFieldIndex) {
        return failures;
    }

    for (std::size_t index = 0U; index < packet.fields.size(); ++index) {
        const std::size_t schemaIndex =
            index < repeatedFieldIndex ? index : repeatedFieldIndex;
        failures += Expect(
            schema.resolvedFields[schemaIndex].name == packet.fields[index].name,
            message);
        failures += Expect(
            schema.resolvedFields[schemaIndex].typeToken ==
                TypeToken(packet.fields[index].type),
            message);
    }
    return failures;
}

int ExpectEffectiveFieldsMatch(
    const PacketSchema& schema,
    const cg::packet::PacketSpec& spec,
    const char* message)
{
    int failures = 0;
    std::vector<FieldSchema> parsedFields;
    const std::vector<FieldSchema>* effectiveFields = &schema.fields;
    if (schema.hasResolvedFields) {
        effectiveFields = &schema.resolvedFields;
    } else if (schema.hasResolvedFormat) {
        parsedFields = ParseResolvedFormat(schema.resolvedFormat, failures);
        effectiveFields = &parsedFields;
    }

    failures += Expect(effectiveFields->size() == spec.fieldCount, message);
    if (effectiveFields->size() != spec.fieldCount) {
        return failures;
    }
    for (std::size_t index = 0U; index < spec.fieldCount; ++index) {
        failures += Expect((*effectiveFields)[index].index == static_cast<int>(index), message);
        failures += Expect((*effectiveFields)[index].name == spec.fields[index].name, message);
        failures += Expect((*effectiveFields)[index].typeToken == TypeToken(spec.fields[index].type), message);
    }
    failures += Expect(
        schema.hasRepeatedTail == spec.repeatedTail.enabled,
        message);
    if (schema.hasRepeatedTail && spec.repeatedTail.enabled) {
        failures += Expect(
            schema.repeatedTail.fieldIndex ==
                static_cast<int>(spec.fieldCount - 1U),
            message);
        failures += Expect(
            schema.repeatedTail.countFieldIndex ==
                static_cast<int>(spec.repeatedTail.countFieldIndex),
            message);
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
    int repeatedTailCount = 0;

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

        if (packet.hasRepeatedTail) {
            ++repeatedTailCount;
            failures += Expect(packet.hasResolvedFields, "repeatedTail resolvedFields present");
            failures += Expect(
                packet.repeatedTail.fieldIndex >= 0 &&
                    packet.repeatedTail.fieldIndex <
                        static_cast<int>(packet.resolvedFields.size()),
                "repeatedTail fieldIndex in resolved fields");
            failures += Expect(
                packet.repeatedTail.countFieldIndex >= 0 &&
                    packet.repeatedTail.countFieldIndex <
                        static_cast<int>(packet.resolvedFields.size()),
                "repeatedTail countFieldIndex in resolved fields");
            failures += Expect(
                packet.repeatedTail.fieldIndex != packet.repeatedTail.countFieldIndex,
                "repeatedTail field and count distinct");
        }
    }

    failures += Expect(codes.size() == 209U, "unique code count 209");
    failures += Expect(clientToServer == 100, "client->server count 100");
    failures += Expect(serverToClient == 109, "server->client count 109");
    failures += Expect(emptyOriginalFormat == 53, "empty original format count 53");
    failures += Expect(normalizedEmptyFormat == 16, "normalized empty format count 16");
    failures += Expect(repeatedTailCount == 1, "repeatedTail schema count 1");
    failures += Expect(FindPacket(packets, "CCL") != nullptr, "CCL exists");
    failures += Expect(FindPacket(packets, "TgX") != nullptr, "TgX exists");
    failures += Expect(FindPacket(packets, "JFVf") != nullptr, "JFVf exists");
    failures += Expect(FindPacket(packets, "whV") != nullptr, "whV exists");
    return failures;
}

template <std::size_t Size>
int TestTypedManifestGroup(
    const std::vector<PacketSchema>& packets,
    const std::array<cg::packet::test::TypedPacketManifestEntry, Size>& entries,
    cg::packet::PacketDirection expectedDirection,
    std::string_view expectedSchemaDirection,
    std::string_view expectedNamespace,
    std::set<std::string, std::less<>>& codes)
{
    int failures = 0;
    for (const cg::packet::test::TypedPacketManifestEntry& entry : entries) {
        failures += Expect(entry.spec != nullptr, "typed manifest Spec exists");
        if (entry.spec == nullptr) {
            continue;
        }

        const cg::packet::PacketSpec& spec = *entry.spec;
        failures += Expect(!spec.code.empty(), "typed manifest code non-empty");
        failures += Expect(spec.direction == expectedDirection, "typed manifest category matches Spec direction");
        failures += Expect(codes.emplace(spec.code).second, "typed manifest code case-sensitive unique");

        const PacketSchema* schema = FindPacket(packets, spec.code);
        failures += Expect(schema != nullptr, "typed manifest schema exists");
        if (schema == nullptr) {
            continue;
        }

        failures += Expect(schema->direction == expectedSchemaDirection, "typed manifest schema direction");
        failures += Expect(schema->clientNamespace == expectedNamespace, "typed manifest schema namespace");
        failures += ExpectEffectiveFieldsMatch(*schema, spec, "typed manifest effective fields");
    }
    return failures;
}

int TestTypedManifest(const std::vector<PacketSchema>& packets)
{
    int failures = 0;
    failures += Expect(cg::packet::test::SendTypedPackets.size() == 75U, "send typed manifest count 75");
    failures += Expect(cg::packet::test::ReceiveTypedPackets.size() == 84U, "receive typed manifest count 84");
    failures += Expect(
        cg::packet::test::SendTypedPackets.size() + cg::packet::test::ReceiveTypedPackets.size() == 159U,
        "typed manifest count 159");

    std::set<std::string, std::less<>> codes;
    failures += TestTypedManifestGroup(
        packets,
        cg::packet::test::SendTypedPackets,
        cg::packet::PacketDirection::ClientToServer,
        "客户端->服务端",
        "send",
        codes);
    failures += TestTypedManifestGroup(
        packets,
        cg::packet::test::ReceiveTypedPackets,
        cg::packet::PacketDirection::ServerToClient,
        "服务端->客户端",
        "receive",
        codes);
    failures += Expect(codes.size() == 159U, "typed manifest unique code count 159");
    return failures;
}

template <std::size_t Size>
bool TypedManifestContainsCode(
    const std::array<cg::packet::test::TypedPacketManifestEntry, Size>& entries,
    std::string_view code)
{
    for (const cg::packet::test::TypedPacketManifestEntry& entry : entries) {
        if (entry.spec != nullptr && entry.spec->code == code) {
            return true;
        }
    }
    return false;
}

template <std::size_t Size>
void AddTypedSpecs(
    const std::array<cg::packet::test::TypedPacketManifestEntry, Size>& entries,
    std::map<std::string, const cg::packet::PacketSpec*, std::less<>>& specs,
    int& failures)
{
    for (const cg::packet::test::TypedPacketManifestEntry& entry : entries) {
        failures += Expect(entry.spec != nullptr, "fixture inventory typed Spec exists");
        if (entry.spec == nullptr) {
            continue;
        }
        failures += Expect(
            specs.emplace(std::string(entry.spec->code), entry.spec).second,
            "fixture inventory typed code unique");
    }
}

template <std::size_t Size>
std::set<std::string, std::less<>> MakeCodeSet(
    const std::array<std::string_view, Size>& codes,
    int& failures,
    const char* uniqueMessage)
{
    std::set<std::string, std::less<>> result;
    for (const std::string_view code : codes) {
        failures += Expect(result.emplace(code).second, uniqueMessage);
    }
    return result;
}

bool PacketsEqual(const cg::packet::Packet& lhs, const cg::packet::Packet& rhs)
{
    if (lhs.code.View() != rhs.code.View() || lhs.direction != rhs.direction
        || lhs.fields.size() != rhs.fields.size()) {
        return false;
    }
    for (std::size_t index = 0U; index < lhs.fields.size(); ++index) {
        if (lhs.fields[index].name != rhs.fields[index].name
            || lhs.fields[index].type != rhs.fields[index].type
            || lhs.fields[index].value != rhs.fields[index].value) {
            return false;
        }
    }
    return true;
}

bool IsPayloadMetadataAllowed(
    const PacketFixtureSample& sample,
    const cg::packet::test::FixturePayloadAudit& audit)
{
    if (audit.kind == cg::packet::test::FixturePayloadAuditKind::NoStructuredPayload) {
        return !sample.payloadField.has_value();
    }
    return !sample.payloadField.has_value() || *sample.payloadField == audit.fieldIndex;
}

int TestFixtureRoundtrip(
    const PacketFixture& fixture,
    const cg::packet::PacketSpec& spec,
    const cg::packet::test::FixturePayloadAudit& audit)
{
    int failures = 0;
    bool hasStructuredPayloadSample = false;

    if (audit.kind == cg::packet::test::FixturePayloadAuditKind::StructuredPayload) {
        failures += Expect(audit.fieldIndex < spec.fieldCount, "structured payload field index in range");
        if (audit.fieldIndex < spec.fieldCount) {
            const cg::packet::PacketFieldType type = spec.fields[audit.fieldIndex].type;
            failures += Expect(
                type == cg::packet::PacketFieldType::EscapedString
                    || type == cg::packet::PacketFieldType::SubPayload,
                "structured payload Spec field is es/SubPayload");
        }
    }

    for (const PacketFixtureSample& sample : fixture.samples) {
        if (audit.kind == cg::packet::test::FixturePayloadAuditKind::NoStructuredPayload) {
            failures += Expect(
                IsPayloadMetadataAllowed(sample, audit),
                "NoStructuredPayload sample forbids payload metadata");
        } else if (sample.payloadField.has_value()) {
            failures += Expect(
                IsPayloadMetadataAllowed(sample, audit),
                "structured payload sample field matches audit");
            if (*sample.payloadField == audit.fieldIndex && !sample.payload.empty()) {
                hasStructuredPayloadSample = true;
            }
        }

        const cg::packet::PacketResult<cg::packet::Packet> parsedInput =
            cg::packet::ParsePacket(sample.plain, spec);
        failures += Expect(parsedInput.HasValue(), "fixture generic Parse(input, Spec)");
        if (!parsedInput.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<std::string> builtInput =
            cg::packet::BuildPacket(parsedInput.Value(), spec);
        failures += Expect(builtInput.HasValue(), "fixture generic Build(parsed input, Spec)");
        if (!builtInput.HasValue()) {
            continue;
        }
        failures += Expect(
            builtInput.Value() == sample.canonical,
            "fixture input generic canonical exact");

        const cg::packet::PacketResult<cg::packet::Packet> parsedCanonical =
            cg::packet::ParsePacket(sample.canonical, spec);
        failures += Expect(
            parsedCanonical.HasValue(),
            "fixture generic Parse(canonical, Spec)");
        if (!parsedCanonical.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<std::string> rebuiltCanonical =
            cg::packet::BuildPacket(parsedCanonical.Value(), spec);
        failures += Expect(
            rebuiltCanonical.HasValue(),
            "fixture generic Build(parsed canonical, Spec)");
        if (rebuiltCanonical.HasValue()) {
            failures += Expect(
                rebuiltCanonical.Value() == sample.canonical,
                "fixture canonical generic rebuild exact");
        }

        failures += Expect(
            PacketsEqual(parsedInput.Value(), parsedCanonical.Value()),
            "fixture two generic Packet values identical");

        if (audit.kind == cg::packet::test::FixturePayloadAuditKind::StructuredPayload
            && sample.payloadField.has_value()
            && *sample.payloadField == audit.fieldIndex
            && audit.fieldIndex < parsedInput.Value().fields.size()) {
            failures += Expect(
                parsedInput.Value().fields[audit.fieldIndex].value == sample.payload,
                "fixture parsed payload bytes exact");
        }
    }

    if (audit.kind == cg::packet::test::FixturePayloadAuditKind::StructuredPayload) {
        failures += Expect(
            hasStructuredPayloadSample,
            "structured payload audit has non-empty payload sample");
    }
    return failures;
}

int TestFixtureRoundtripMemory()
{
    int failures = 0;

    PacketFixture noStructured;
    noStructured.code = "jv";
    noStructured.direction = cg::packet::PacketDirection::ServerToClient;
    noStructured.samples.push_back({
        "basic",
        "jv 1 Soldier",
        "jv 1 Soldier ",
        std::nullopt,
        {},
    });
    const cg::packet::test::FixturePayloadAudit noStructuredAudit {
        "jv",
        cg::packet::test::FixturePayloadAuditKind::NoStructuredPayload,
        0U,
    };
    failures += Expect(
        TestFixtureRoundtrip(
            noStructured,
            cg::packet::receive::JobName::Spec,
            noStructuredAudit)
            == 0,
        "fixture memory NoStructured generic roundtrip");

    PacketFixtureSample forbiddenPayloadMetadata {
        "forbidden-payload",
        "jv 1 Soldier",
        "jv 1 Soldier ",
        1U,
        "Soldier",
    };
    failures += Expect(
        !IsPayloadMetadataAllowed(forbiddenPayloadMetadata, noStructuredAudit),
        "fixture memory NoStructured rejects payload metadata");

    PacketFixture structured;
    structured.code = "eZT";
    structured.direction = cg::packet::PacketDirection::ServerToClient;
    structured.samples.push_back({
        "nonempty-payload",
        "eZT payload",
        "eZT payload ",
        0U,
        "payload",
    });
    structured.samples.push_back({
        "empty-payload",
        "eZT  ",
        "eZT  ",
        0U,
        {},
    });
    structured.samples.push_back({
        "payload-metadata-omitted",
        "eZT other",
        "eZT other ",
        std::nullopt,
        {},
    });
    const cg::packet::test::FixturePayloadAudit structuredAudit {
        "eZT",
        cg::packet::test::FixturePayloadAuditKind::StructuredPayload,
        0U,
    };
    failures += Expect(
        TestFixtureRoundtrip(
            structured,
            cg::packet::receive::ProfileList::Spec,
            structuredAudit)
            == 0,
        "fixture memory Structured generic roundtrip");
    return failures;
}

int TestFixtureInventory(const std::filesystem::path& root)
{
    int failures = 0;

    std::map<std::string, const cg::packet::PacketSpec*, std::less<>> typedSpecs;
    AddTypedSpecs(cg::packet::test::SendTypedPackets, typedSpecs, failures);
    AddTypedSpecs(cg::packet::test::ReceiveTypedPackets, typedSpecs, failures);

    const std::set<std::string, std::less<>> historicalCodes = MakeCodeSet(
        cg::packet::test::HistoricalTypedBaselineCodes,
        failures,
        "historical typed baseline code unique");
    const std::set<std::string, std::less<>> debtCodes = MakeCodeSet(
        cg::packet::test::FixtureDebtCodes,
        failures,
        "fixture debt code unique");

    std::set<std::string, std::less<>> typedCodes;
    for (const auto& [code, spec] : typedSpecs) {
        (void)spec;
        typedCodes.emplace(code);
    }
    for (const std::string& code : historicalCodes) {
        failures += Expect(
            typedCodes.contains(code),
            "historical typed baseline remains in typed manifest");
    }
    for (const std::string& code : debtCodes) {
        failures += Expect(
            historicalCodes.contains(code),
            "fixture debt is subset of frozen historical baseline");
    }

    std::map<std::string, const cg::packet::test::FixturePayloadAudit*, std::less<>> audits;
    for (const cg::packet::test::FixturePayloadAudit& audit :
         cg::packet::test::FixturePayloadAudits) {
        failures += Expect(!audit.code.empty(), "fixture payload audit code non-empty");
        failures += Expect(
            audits.emplace(std::string(audit.code), &audit).second,
            "fixture payload audit code unique");
    }

    std::size_t expectedActualCount = 0U;
    for (const std::string& code : typedCodes) {
        if (!debtCodes.contains(code)) {
            ++expectedActualCount;
        }
    }

    std::error_code filesystemError;
    const bool rootExists = std::filesystem::exists(root, filesystemError);
    failures += Expect(!filesystemError, "fixture root exists check");
    if (filesystemError) {
        return failures;
    }
    std::map<std::string, PacketFixture, std::less<>> actualFixtures;
    if (!rootExists) {
        failures += Expect(
            expectedActualCount == 0U,
            "missing fixture root allowed only when expected actual count is zero");
    } else {
        const bool rootIsDirectory = std::filesystem::is_directory(root, filesystemError);
        failures += Expect(rootIsDirectory, "fixture root is directory");
        failures += Expect(!filesystemError, "fixture root directory check");
        if (!filesystemError && rootIsDirectory) {
            std::filesystem::recursive_directory_iterator iterator(root, filesystemError);
            const std::filesystem::recursive_directory_iterator end;
            failures += Expect(!filesystemError, "fixture root iteration starts");
            while (!filesystemError && iterator != end) {
                const std::filesystem::directory_entry& entry = *iterator;
                const std::filesystem::path relative = entry.path().lexically_relative(root);
                std::vector<std::string> components;
                for (const std::filesystem::path& component : relative) {
                    components.push_back(component.string());
                }

                const std::filesystem::file_status status = entry.symlink_status(filesystemError);
                if (filesystemError) {
                    break;
                }
                if (std::filesystem::is_directory(status)) {
                    const bool validDirectionDirectory = components.size() == 1U
                        && (components[0] == "client_to_server"
                            || components[0] == "server_to_client");
                    failures += Expect(
                        validDirectionDirectory,
                        "fixture directory is an exact direction directory");
                    if (!validDirectionDirectory) {
                        iterator.disable_recursion_pending();
                    }
                    iterator.increment(filesystemError);
                    continue;
                }

                const bool validFilePath = std::filesystem::is_regular_file(status)
                    && components.size() == 2U
                    && (components[0] == "client_to_server"
                        || components[0] == "server_to_client")
                    && entry.path().extension() == ".pkt";
                failures += Expect(validFilePath, "fixture file path and extension");
                if (!validFilePath) {
                    iterator.increment(filesystemError);
                    continue;
                }

                std::ifstream file(entry.path(), std::ios::binary);
                failures += Expect(file.is_open(), "fixture file opens");
                if (!file.is_open()) {
                    iterator.increment(filesystemError);
                    continue;
                }
                std::ostringstream buffer;
                buffer << file.rdbuf();
                failures += Expect(file.good() || file.eof(), "fixture file reads");

                const std::string fixtureText = buffer.str();
                PacketFixture fixture;
                PacketFixtureParser parser(fixtureText);
                if (!parser.Parse(fixture)) {
                    std::cerr << entry.path().string() << ": " << parser.Error() << '\n';
                    ++failures;
                    iterator.increment(filesystemError);
                    continue;
                }

                failures += Expect(
                    entry.path().filename().string() == fixture.code + ".pkt",
                    "fixture code equals filename stem");
                const cg::packet::PacketDirection pathDirection =
                    components[0] == "client_to_server"
                    ? cg::packet::PacketDirection::ClientToServer
                    : cg::packet::PacketDirection::ServerToClient;
                failures += Expect(
                    fixture.direction == pathDirection,
                    "fixture direction equals parent directory");
                const std::string fixtureCode = fixture.code;
                failures += Expect(
                    actualFixtures.emplace(fixtureCode, std::move(fixture)).second,
                    "fixture code has exactly one file");

                iterator.increment(filesystemError);
            }
            failures += Expect(!filesystemError, "fixture root iteration completes");
        }
    }

    std::set<std::string, std::less<>> actualCodes;
    for (const auto& [code, fixture] : actualFixtures) {
        actualCodes.emplace(code);
        failures += Expect(
            !debtCodes.contains(code),
            "actual fixture and fixture debt are disjoint");

        const auto specIt = typedSpecs.find(code);
        failures += Expect(specIt != typedSpecs.end(), "actual fixture code exists in typed manifest");
        const auto auditIt = audits.find(code);
        failures += Expect(
            auditIt != audits.end(),
            "actual fixture code has exactly one payload audit");
        if (specIt == typedSpecs.end() || auditIt == audits.end()) {
            continue;
        }

        failures += Expect(
            fixture.direction == specIt->second->direction,
            "fixture direction equals typed Spec direction");
        failures += TestFixtureRoundtrip(fixture, *specIt->second, *auditIt->second);
    }

    std::set<std::string, std::less<>> auditCodes;
    for (const auto& [code, audit] : audits) {
        (void)audit;
        auditCodes.emplace(code);
    }
    failures += Expect(
        auditCodes == actualCodes,
        "payload audit codes equal actual fixture codes");

    std::set<std::string, std::less<>> accountedCodes = debtCodes;
    accountedCodes.insert(actualCodes.begin(), actualCodes.end());
    failures += Expect(
        accountedCodes == typedCodes,
        "actual fixtures plus fixture debt equal typed manifest");
    failures += Expect(
        actualFixtures.size() == expectedActualCount,
        "actual fixture count equals inventory expectation");
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

    const PacketSchema* characterLogoutRequestSchema = FindPacket(packets, "WA");
    failures += Expect(characterLogoutRequestSchema != nullptr, "WA schema exists");
    if (characterLogoutRequestSchema != nullptr) {
        const cg::packet::Packet packet = cg::packet::send::CharacterLogoutRequest {}.ToPacket();
        failures += Expect(
            characterLogoutRequestSchema->direction == "客户端->服务端",
            "WA direction");
        failures += Expect(characterLogoutRequestSchema->legacyKind == "RECV", "WA legacyKind");
        failures += Expect(characterLogoutRequestSchema->rawFormat.empty(), "WA rawFormat empty");
        failures += Expect(characterLogoutRequestSchema->originalFormat == "WA", "WA originalFormat");
        failures += Expect(!characterLogoutRequestSchema->formatEmpty, "WA formatEmpty false");
        failures += Expect(characterLogoutRequestSchema->hasResolvedFormat, "WA resolvedFormat present");
        failures += Expect(characterLogoutRequestSchema->resolvedFormat.empty(), "WA resolvedFormat empty");
        failures += Expect(
            characterLogoutRequestSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "WA resolvedSource");
        failures += Expect(characterLogoutRequestSchema->fields.empty(), "WA raw fields empty");
        failures += Expect(characterLogoutRequestSchema->hasResolvedFields, "WA resolvedFields present");
        failures += Expect(characterLogoutRequestSchema->resolvedFields.empty(), "WA resolvedFields empty");
        failures += ExpectEffectiveFieldsMatch(
            *characterLogoutRequestSchema,
            cg::packet::send::CharacterLogoutRequest::Spec,
            "WA Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterLogoutRequestSchema,
            packet,
            "WA resolved field order");
    }

    const PacketSchema* characterLoginRequestSchema = FindPacket(packets, "emRV");
    failures += Expect(characterLoginRequestSchema != nullptr, "emRV schema exists");
    if (characterLoginRequestSchema != nullptr) {
        const cg::packet::Packet packet =
            cg::packet::send::CharacterLoginRequest(62, -1, 36).ToPacket();
        failures += Expect(
            characterLoginRequestSchema->direction == "客户端->服务端",
            "emRV direction");
        failures += Expect(characterLoginRequestSchema->legacyKind == "RECV", "emRV legacyKind");
        failures += Expect(
            characterLoginRequestSchema->rawFormat
                == "{registnumber:i62} {renewal:i62} {face:i62}",
            "emRV rawFormat");
        failures += Expect(characterLoginRequestSchema->fields.size() == 3U, "emRV raw field count");
        if (characterLoginRequestSchema->fields.size() == 3U) {
            failures += Expect(
                characterLoginRequestSchema->fields[0].name == "registnumber",
                "emRV raw registnumber name");
            failures += Expect(
                characterLoginRequestSchema->fields[0].typeToken == "i62",
                "emRV raw registnumber type");
            failures += Expect(
                characterLoginRequestSchema->fields[0].source == "xlsx",
                "emRV raw registnumber source");
            failures += Expect(
                characterLoginRequestSchema->fields[1].name == "renewal",
                "emRV raw renewal name");
            failures += Expect(
                characterLoginRequestSchema->fields[1].typeToken == "i62",
                "emRV raw renewal type");
            failures += Expect(
                characterLoginRequestSchema->fields[1].source == "xlsx",
                "emRV raw renewal source");
            failures += Expect(
                characterLoginRequestSchema->fields[2].name == "face",
                "emRV raw face name");
            failures += Expect(
                characterLoginRequestSchema->fields[2].typeToken == "i62",
                "emRV raw face type");
            failures += Expect(
                characterLoginRequestSchema->fields[2].source == "xlsx",
                "emRV raw face source");
        }
        failures += Expect(characterLoginRequestSchema->hasResolvedFormat, "emRV resolvedFormat present");
        failures += Expect(
            characterLoginRequestSchema->resolvedFormat
                == "{registnumber:i62} {renewal:i62} {face:i62}",
            "emRV resolvedFormat");
        failures += Expect(
            characterLoginRequestSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "emRV resolvedSource");
        failures += Expect(characterLoginRequestSchema->hasResolvedFields, "emRV resolvedFields present");
        failures += Expect(
            characterLoginRequestSchema->resolvedFields.size() == 3U,
            "emRV resolved field count");
        if (characterLoginRequestSchema->resolvedFields.size() == 3U) {
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[0].name == "registnumber",
                "emRV resolved registnumber name");
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[0].typeToken == "i62",
                "emRV resolved registnumber type");
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[0].source
                    == "xlsx+ServerC+ClientIda+src/packet",
                "emRV resolved registnumber source");
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[1].name == "renewal",
                "emRV resolved renewal name");
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[1].typeToken == "i62",
                "emRV resolved renewal type");
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[1].source
                    == "xlsx+ServerC+ClientIda+src/packet",
                "emRV resolved renewal source");
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[2].name == "face",
                "emRV resolved face name");
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[2].typeToken == "i62",
                "emRV resolved face type");
            failures += Expect(
                characterLoginRequestSchema->resolvedFields[2].source
                    == "xlsx+ServerC+ClientIda+src/packet",
                "emRV resolved face source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *characterLoginRequestSchema,
            cg::packet::send::CharacterLoginRequest::Spec,
            "emRV Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterLoginRequestSchema,
            packet,
            "emRV resolved field order");
    }

    const PacketSchema* profileStatusUpdateRequestSchema = FindPacket(packets, "lz");
    failures += Expect(profileStatusUpdateRequestSchema != nullptr, "lz schema exists");
    if (profileStatusUpdateRequestSchema != nullptr) {
        const cg::packet::Packet packet =
            cg::packet::send::ProfileStatusUpdateRequest(1).ToPacket();
        failures += Expect(
            profileStatusUpdateRequestSchema->direction == "客户端->服务端",
            "lz direction");
        failures += Expect(
            profileStatusUpdateRequestSchema->legacyKind == "RECV",
            "lz legacyKind");
        failures += Expect(
            profileStatusUpdateRequestSchema->rawFormat == "{0:i62}",
            "lz rawFormat");
        failures += Expect(
            profileStatusUpdateRequestSchema->fields.size() == 1U,
            "lz raw field count");
        if (profileStatusUpdateRequestSchema->fields.size() == 1U) {
            failures += Expect(
                profileStatusUpdateRequestSchema->fields[0].name == "0",
                "lz raw field name");
            failures += Expect(
                profileStatusUpdateRequestSchema->fields[0].typeToken == "i62",
                "lz raw field type");
            failures += Expect(
                profileStatusUpdateRequestSchema->fields[0].rawSpec == "{0:i62}",
                "lz raw field spec");
            failures += Expect(
                profileStatusUpdateRequestSchema->fields[0].source == "xlsx",
                "lz raw field source");
        }
        failures += Expect(
            profileStatusUpdateRequestSchema->hasResolvedFormat,
            "lz resolvedFormat present");
        failures += Expect(
            profileStatusUpdateRequestSchema->resolvedFormat == "{profileStatus:i62}",
            "lz resolvedFormat");
        failures += Expect(
            profileStatusUpdateRequestSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "lz resolvedSource");
        failures += Expect(
            profileStatusUpdateRequestSchema->hasResolvedFields,
            "lz resolvedFields present");
        failures += Expect(
            profileStatusUpdateRequestSchema->resolvedFields.size() == 1U,
            "lz resolved field count");
        if (profileStatusUpdateRequestSchema->resolvedFields.size() == 1U) {
            failures += Expect(
                profileStatusUpdateRequestSchema->resolvedFields[0].name == "profileStatus",
                "lz resolved field name");
            failures += Expect(
                profileStatusUpdateRequestSchema->resolvedFields[0].typeToken == "i62",
                "lz resolved field type");
            failures += Expect(
                profileStatusUpdateRequestSchema->resolvedFields[0].rawSpec
                    == "{profileStatus:i62}",
                "lz resolved field spec");
            failures += Expect(
                profileStatusUpdateRequestSchema->resolvedFields[0].source
                    == "xlsx+ServerC+ClientIda+src/packet",
                "lz resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *profileStatusUpdateRequestSchema,
            cg::packet::send::ProfileStatusUpdateRequest::Spec,
            "lz Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *profileStatusUpdateRequestSchema,
            packet,
            "lz resolved field order");
    }

    const PacketSchema* echoRequestSchema = FindPacket(packets, "Or");
    failures += Expect(echoRequestSchema != nullptr, "Or schema exists");
    if (echoRequestSchema != nullptr) {
        const cg::packet::Packet packet = cg::packet::send::EchoRequest("nr").ToPacket();
        failures += Expect(echoRequestSchema->direction == "客户端->服务端", "Or direction");
        failures += Expect(echoRequestSchema->legacyKind == "RECV", "Or legacyKind");
        failures += Expect(echoRequestSchema->rawFormat == "{stamp:es}", "Or rawFormat");
        failures += Expect(echoRequestSchema->originalFormat.empty(), "Or originalFormat empty");
        failures += Expect(echoRequestSchema->fields.size() == 1U, "Or raw field count");
        if (echoRequestSchema->fields.size() == 1U) {
            failures += Expect(echoRequestSchema->fields[0].name == "stamp", "Or raw field name");
            failures += Expect(
                echoRequestSchema->fields[0].typeToken == "es",
                "Or raw field type");
            failures += Expect(
                echoRequestSchema->fields[0].rawSpec == "{stamp:es}",
                "Or raw field spec");
            failures += Expect(echoRequestSchema->fields[0].source == "xlsx", "Or raw field source");
        }
        failures += Expect(echoRequestSchema->hasResolvedFormat, "Or resolvedFormat present");
        failures += Expect(
            echoRequestSchema->resolvedFormat == "{payload:es}",
            "Or resolvedFormat");
        failures += Expect(
            echoRequestSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "Or resolvedSource");
        failures += Expect(echoRequestSchema->hasResolvedFields, "Or resolvedFields present");
        failures += Expect(
            echoRequestSchema->resolvedFields.size() == 1U,
            "Or resolved field count");
        if (echoRequestSchema->resolvedFields.size() == 1U) {
            failures += Expect(
                echoRequestSchema->resolvedFields[0].name == "payload",
                "Or resolved field name");
            failures += Expect(
                echoRequestSchema->resolvedFields[0].typeToken == "es",
                "Or resolved field type");
            failures += Expect(
                echoRequestSchema->resolvedFields[0].rawSpec == "{payload:es}",
                "Or resolved field spec");
            failures += Expect(
                echoRequestSchema->resolvedFields[0].source
                    == "xlsx+ServerC+ClientIda+src/packet",
                "Or resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *echoRequestSchema,
            cg::packet::send::EchoRequest::Spec,
            "Or Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *echoRequestSchema,
            packet,
            "Or resolved field order");
    }

    const PacketSchema* keepAliveRequestSchema = FindPacket(packets, "DE");
    failures += Expect(keepAliveRequestSchema != nullptr, "DE schema exists");
    if (keepAliveRequestSchema != nullptr) {
        const cg::packet::Packet packet = cg::packet::send::KeepAliveRequest {}.ToPacket();
        failures += Expect(keepAliveRequestSchema->direction == "客户端->服务端", "DE direction");
        failures += Expect(keepAliveRequestSchema->legacyKind == "RECV", "DE legacyKind");
        failures += Expect(keepAliveRequestSchema->rawFormat.empty(), "DE rawFormat empty");
        failures += Expect(keepAliveRequestSchema->originalFormat == "DE", "DE originalFormat");
        failures += Expect(!keepAliveRequestSchema->formatEmpty, "DE formatEmpty false");
        failures += Expect(keepAliveRequestSchema->hasResolvedFormat, "DE resolvedFormat present");
        failures += Expect(keepAliveRequestSchema->resolvedFormat.empty(), "DE resolvedFormat empty");
        failures += Expect(
            keepAliveRequestSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "DE resolvedSource");
        failures += Expect(keepAliveRequestSchema->fields.empty(), "DE raw fields empty");
        failures += Expect(keepAliveRequestSchema->hasResolvedFields, "DE resolvedFields present");
        failures += Expect(keepAliveRequestSchema->resolvedFields.empty(), "DE resolvedFields empty");
        failures += ExpectEffectiveFieldsMatch(
            *keepAliveRequestSchema,
            cg::packet::send::KeepAliveRequest::Spec,
            "DE Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *keepAliveRequestSchema,
            packet,
            "DE resolved field order");
    }

    const PacketSchema* npcEventRequestSchema = FindPacket(packets, "pg");
    failures += Expect(npcEventRequestSchema != nullptr, "pg schema exists");
    if (npcEventRequestSchema != nullptr) {
        const cg::packet::Packet packet =
            cg::packet::send::NpcEventRequest(62, -1, 36, 1, 0).ToPacket();
        constexpr std::array<std::string_view, 5U> fieldNames = {
            "event",
            "seqno",
            "x",
            "y",
            "dir",
        };
        constexpr std::string_view format =
            "{event:i62} {seqno:i62} {x:i62} {y:i62} {dir:i62}";
        constexpr std::string_view resolvedSource =
            "xlsx+ServerC+ClientIda+src/packet";

        failures += Expect(npcEventRequestSchema->direction == "客户端->服务端", "pg direction");
        failures += Expect(npcEventRequestSchema->legacyKind == "RECV", "pg legacyKind");
        failures += Expect(npcEventRequestSchema->rawFormat == format, "pg rawFormat");
        failures += Expect(npcEventRequestSchema->fields.size() == fieldNames.size(), "pg raw field count");
        if (npcEventRequestSchema->fields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                failures += Expect(
                    npcEventRequestSchema->fields[index].name == fieldNames[index],
                    "pg raw field name");
                failures += Expect(
                    npcEventRequestSchema->fields[index].typeToken == "i62",
                    "pg raw field type");
                failures += Expect(
                    npcEventRequestSchema->fields[index].source == "xlsx",
                    "pg raw field source");
            }
        }
        failures += Expect(npcEventRequestSchema->hasResolvedFormat, "pg resolvedFormat present");
        failures += Expect(npcEventRequestSchema->resolvedFormat == format, "pg resolvedFormat");
        failures += Expect(
            npcEventRequestSchema->resolvedSource == resolvedSource,
            "pg resolvedSource");
        failures += Expect(npcEventRequestSchema->hasResolvedFields, "pg resolvedFields present");
        failures += Expect(
            npcEventRequestSchema->resolvedFields.size() == fieldNames.size(),
            "pg resolved field count");
        if (npcEventRequestSchema->resolvedFields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                failures += Expect(
                    npcEventRequestSchema->resolvedFields[index].name == fieldNames[index],
                    "pg resolved field name");
                failures += Expect(
                    npcEventRequestSchema->resolvedFields[index].typeToken == "i62",
                    "pg resolved field type");
                failures += Expect(
                    npcEventRequestSchema->resolvedFields[index].source == resolvedSource,
                    "pg resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *npcEventRequestSchema,
            cg::packet::send::NpcEventRequest::Spec,
            "pg Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *npcEventRequestSchema,
            packet,
            "pg resolved field order");
    }

    const PacketSchema* duelRequestSchema = FindPacket(packets, "ik");
    failures += Expect(duelRequestSchema != nullptr, "ik schema exists");
    if (duelRequestSchema != nullptr) {
        const cg::packet::Packet packet = cg::packet::send::DuelRequest(62, 124).ToPacket();
        constexpr std::array<std::string_view, 2U> fieldNames = {"x", "y"};
        constexpr std::string_view resolvedSource =
            "xlsx+ServerC+ClientIda+src/packet";

        failures += Expect(duelRequestSchema->direction == "客户端->服务端", "ik direction");
        failures += Expect(duelRequestSchema->legacyKind == "RECV", "ik legacyKind");
        failures += Expect(duelRequestSchema->rawFormat == "{x} {y}", "ik rawFormat");
        failures += Expect(
            duelRequestSchema->fields.size() == fieldNames.size(),
            "ik raw field count");
        if (duelRequestSchema->fields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                failures += Expect(
                    duelRequestSchema->fields[index].name == fieldNames[index],
                    "ik raw field name");
                failures += Expect(
                    duelRequestSchema->fields[index].typeToken == "unknown",
                    "ik raw field type");
                failures += Expect(
                    duelRequestSchema->fields[index].source == "xlsx",
                    "ik raw field source");
            }
        }
        failures += Expect(duelRequestSchema->hasResolvedFormat, "ik resolvedFormat present");
        failures += Expect(
            duelRequestSchema->resolvedFormat == "{x:i62} {y:i62}",
            "ik resolvedFormat");
        failures += Expect(
            duelRequestSchema->resolvedSource == resolvedSource,
            "ik resolvedSource");
        failures += Expect(duelRequestSchema->hasResolvedFields, "ik resolvedFields present");
        failures += Expect(
            duelRequestSchema->resolvedFields.size() == fieldNames.size(),
            "ik resolved field count");
        if (duelRequestSchema->resolvedFields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                failures += Expect(
                    duelRequestSchema->resolvedFields[index].name == fieldNames[index],
                    "ik resolved field name");
                failures += Expect(
                    duelRequestSchema->resolvedFields[index].typeToken == "i62",
                    "ik resolved field type");
                failures += Expect(
                    duelRequestSchema->resolvedFields[index].source == resolvedSource,
                    "ik resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *duelRequestSchema,
            cg::packet::send::DuelRequest::Spec,
            "ik Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *duelRequestSchema,
            packet,
            "ik resolved field order");
    }

    const PacketSchema* battleExitRequestSchema = FindPacket(packets, "eL");
    failures += Expect(battleExitRequestSchema != nullptr, "eL schema exists");
    if (battleExitRequestSchema != nullptr) {
        const cg::packet::Packet packet = cg::packet::send::BattleExitRequest {}.ToPacket();
        constexpr std::string_view resolvedSource =
            "xlsx+ServerC+ClientIda+src/packet";

        failures += Expect(
            battleExitRequestSchema->direction == "客户端->服务端",
            "eL direction");
        failures += Expect(battleExitRequestSchema->legacyKind == "RECV", "eL legacyKind");
        failures += Expect(battleExitRequestSchema->rawFormat.empty(), "eL rawFormat empty");
        failures += Expect(
            battleExitRequestSchema->originalFormat == "eL 0",
            "eL originalFormat");
        failures += Expect(!battleExitRequestSchema->formatEmpty, "eL formatEmpty false");
        failures += Expect(battleExitRequestSchema->fields.empty(), "eL raw fields empty");
        failures += Expect(
            battleExitRequestSchema->hasResolvedFormat,
            "eL resolvedFormat present");
        failures += Expect(
            battleExitRequestSchema->resolvedFormat == "{dummy:i62}",
            "eL resolvedFormat");
        failures += Expect(
            battleExitRequestSchema->resolvedSource == resolvedSource,
            "eL resolvedSource");
        failures += Expect(
            battleExitRequestSchema->hasResolvedFields,
            "eL resolvedFields present");
        failures += Expect(
            battleExitRequestSchema->resolvedFields.size() == 1U,
            "eL resolved field count");
        if (battleExitRequestSchema->resolvedFields.size() == 1U) {
            failures += Expect(
                battleExitRequestSchema->resolvedFields[0].name == "dummy",
                "eL resolved field name");
            failures += Expect(
                battleExitRequestSchema->resolvedFields[0].typeToken == "i62",
                "eL resolved field type");
            failures += Expect(
                battleExitRequestSchema->resolvedFields[0].rawSpec == "{dummy:i62}",
                "eL resolved field spec");
            failures += Expect(
                battleExitRequestSchema->resolvedFields[0].source == resolvedSource,
                "eL resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *battleExitRequestSchema,
            cg::packet::send::BattleExitRequest::Spec,
            "eL Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *battleExitRequestSchema,
            packet,
            "eL resolved field order");
    }

    const PacketSchema* battleWatchStopSchema = FindPacket(packets, "NgMy");
    failures += Expect(battleWatchStopSchema != nullptr, "NgMy schema exists");
    if (battleWatchStopSchema != nullptr) {
        const cg::packet::send::BattleWatchStopRequest request;
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            battleWatchStopSchema->direction == "客户端->服务端",
            "NgMy direction");
        failures += Expect(
            battleWatchStopSchema->legacyKind == "RECV",
            "NgMy legacyKind");
        failures += Expect(
            battleWatchStopSchema->originalFormat == "NgMy 0",
            "NgMy originalFormat");
        failures += Expect(
            battleWatchStopSchema->rawFormat.empty(),
            "NgMy rawFormat empty");
        failures += Expect(
            !battleWatchStopSchema->formatEmpty,
            "NgMy formatEmpty false");
        failures += Expect(
            battleWatchStopSchema->source == "xlsx",
            "NgMy raw source");
        failures += Expect(
            battleWatchStopSchema->fields.empty(),
            "NgMy raw fields empty");
        failures += Expect(
            battleWatchStopSchema->hasResolvedFormat,
            "NgMy resolvedFormat present");
        failures += Expect(
            battleWatchStopSchema->resolvedFormat == "{dummy:i62}",
            "NgMy resolvedFormat");
        failures += Expect(
            battleWatchStopSchema->resolvedSource == "xlsx+ServerC+src/packet",
            "NgMy resolvedSource");
        failures += Expect(
            battleWatchStopSchema->resolvedSource.find("ClientIda") == std::string::npos,
            "NgMy resolvedSource excludes ClientIda");
        failures += Expect(
            battleWatchStopSchema->hasResolvedFields,
            "NgMy resolvedFields present");
        failures += Expect(
            battleWatchStopSchema->resolvedFields.size() == 1U,
            "NgMy resolved field count");
        if (battleWatchStopSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = battleWatchStopSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "dummy" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{dummy:i62}" &&
                    field.source == "xlsx+ServerC+src/packet",
                "NgMy resolved field");
            failures += Expect(
                field.source.find("ClientIda") == std::string::npos,
                "NgMy resolved field source excludes ClientIda");
        }

        failures += ExpectEffectiveFieldsMatch(
            *battleWatchStopSchema,
            cg::packet::send::BattleWatchStopRequest::Spec,
            "NgMy Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *battleWatchStopSchema,
            packet,
            "NgMy resolved field order");
    }

    const PacketSchema* loginResultSchema = FindPacket(packets, "whV");
    failures += Expect(loginResultSchema != nullptr, "whV schema exists");
    if (loginResultSchema != nullptr) {
        const cg::packet::receive::ClientLoginResult loginResult(0, "busy");
        failures += Expect(loginResultSchema->direction == "服务端->客户端", "whV direction");
        failures += Expect(loginResultSchema->legacyKind == "SEND", "whV legacyKind");
        failures += ExpectPacketFieldsMatch(*loginResultSchema, loginResult.ToPacket(), "whV field order");
    }

    const PacketSchema* pfmNotificationSchema = FindPacket(packets, "PfM");
    failures += Expect(pfmNotificationSchema != nullptr, "PfM schema exists");
    if (pfmNotificationSchema != nullptr) {
        const cg::packet::Packet packet = cg::packet::receive::PfMNotification(1).ToPacket();
        failures += Expect(pfmNotificationSchema->direction == "服务端->客户端", "PfM direction");
        failures += Expect(pfmNotificationSchema->legacyKind == "SEND", "PfM legacyKind");
        failures += Expect(
            pfmNotificationSchema->rawFormat == "{TrialFlg:i62}",
            "PfM rawFormat");
        failures += Expect(pfmNotificationSchema->fields.size() == 1U, "PfM raw field count");
        if (pfmNotificationSchema->fields.size() == 1U) {
            failures += Expect(
                pfmNotificationSchema->fields[0].name == "TrialFlg",
                "PfM raw field name");
            failures += Expect(
                pfmNotificationSchema->fields[0].typeToken == "i62",
                "PfM raw field type");
            failures += Expect(
                pfmNotificationSchema->fields[0].source == "xlsx",
                "PfM raw field source");
        }
        failures += Expect(pfmNotificationSchema->hasResolvedFormat, "PfM resolvedFormat present");
        failures += Expect(
            pfmNotificationSchema->resolvedFormat == "{TrialFlg:i62}",
            "PfM resolvedFormat");
        failures += Expect(
            pfmNotificationSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "PfM resolvedSource");
        failures += Expect(pfmNotificationSchema->hasResolvedFields, "PfM resolvedFields present");
        failures += Expect(
            pfmNotificationSchema->resolvedFields.size() == 1U,
            "PfM resolved field count");
        if (pfmNotificationSchema->resolvedFields.size() == 1U) {
            failures += Expect(
                pfmNotificationSchema->resolvedFields[0].name == "TrialFlg",
                "PfM resolved field name");
            failures += Expect(
                pfmNotificationSchema->resolvedFields[0].typeToken == "i62",
                "PfM resolved field type");
            failures += Expect(
                pfmNotificationSchema->resolvedFields[0].source
                    == "xlsx+ServerC+ClientIda+src/packet",
                "PfM resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *pfmNotificationSchema,
            cg::packet::receive::PfMNotification::Spec,
            "PfM Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *pfmNotificationSchema,
            packet,
            "PfM resolved field order");
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

    const PacketSchema* profileMailReceiptSchema = FindPacket(packets, "ow");
    failures += Expect(profileMailReceiptSchema != nullptr, "ow schema exists");
    if (profileMailReceiptSchema != nullptr) {
        const cg::packet::receive::ProfileMailReceipt profileMailReceipt(
            1,
            "2026/ 7/29 12:34",
            "message|legacy",
            2);
        const cg::packet::PacketResult<cg::packet::Packet> packet = profileMailReceipt.ToPacket();
        failures += Expect(profileMailReceiptSchema->direction == "服务端->客户端", "ow direction");
        failures += Expect(profileMailReceiptSchema->legacyKind == "SEND", "ow legacyKind");
        failures += Expect(
            profileMailReceiptSchema->rawFormat == "{profileIndex} {dtext} {color}",
            "ow rawFormat");
        failures += Expect(profileMailReceiptSchema->fields.size() == 3U, "ow raw field count");
        if (profileMailReceiptSchema->fields.size() == 3U) {
            failures += Expect(profileMailReceiptSchema->fields[0].name == "profileIndex", "ow raw profileIndex");
            failures += Expect(profileMailReceiptSchema->fields[1].name == "dtext", "ow raw dtext");
            failures += Expect(profileMailReceiptSchema->fields[2].name == "color", "ow raw color");
            failures += Expect(profileMailReceiptSchema->fields[0].typeToken == "unknown", "ow raw profileIndex type");
            failures += Expect(profileMailReceiptSchema->fields[1].typeToken == "unknown", "ow raw dtext type");
            failures += Expect(profileMailReceiptSchema->fields[2].typeToken == "unknown", "ow raw color type");
        }
        failures += Expect(
            profileMailReceiptSchema->resolvedFormat == "{profileIndex:i62} {dtext:es} {color:i62}",
            "ow resolvedFormat");
        failures += Expect(
            profileMailReceiptSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "ow resolvedSource");
        failures += Expect(static_cast<bool>(packet), "ow ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *profileMailReceiptSchema,
                packet.Value(),
                "ow resolved/ToPacket field order");
        }
    }

    const PacketSchema* profileWindowSchema = FindPacket(packets, "DyAG");
    failures += Expect(profileWindowSchema != nullptr, "DyAG schema exists");
    if (profileWindowSchema != nullptr) {
        const cg::packet::receive::ProfileWindow profileWindow(38, 0, 0, 0, std::vector<int> {});
        const cg::packet::PacketResult<cg::packet::Packet> packet = profileWindow.ToPacket();
        const std::array<std::string_view, 5U> rawNames = {{"", "", "", "", "data"}};
        const std::array<std::string_view, 5U> rawSpecs = {{"{}", "{}", "{}", "{}", "{data}"}};
        const std::array<std::string_view, 5U> resolvedNames = {{
            "windowType", "buttonType", "seqNo", "objectIndex", "data",
        }};
        const std::array<std::string_view, 5U> resolvedTypes = {{"i62", "i62", "i62", "i62", "es"}};
        const std::array<std::string_view, 5U> resolvedSpecs = {{
            "{windowType:i62}", "{buttonType:i62}", "{seqNo:i62}", "{objectIndex:i62}", "{data:es}",
        }};
        failures += Expect(profileWindowSchema->direction == "服务端->客户端", "DyAG direction");
        failures += Expect(profileWindowSchema->legacyKind == "SEND", "DyAG legacyKind");
        failures += Expect(profileWindowSchema->source == "xlsx", "DyAG raw source");
        failures += Expect(profileWindowSchema->rawFormat == "{} {} {} {} {data}", "DyAG rawFormat");
        failures += Expect(profileWindowSchema->fields.size() == rawNames.size(), "DyAG raw field count");
        if (profileWindowSchema->fields.size() == rawNames.size()) {
            for (std::size_t index = 0; index < rawNames.size(); ++index) {
                failures += Expect(
                    profileWindowSchema->fields[index].index == static_cast<int>(index),
                    "DyAG raw field index");
                failures += Expect(profileWindowSchema->fields[index].name == rawNames[index], "DyAG raw field name");
                failures += Expect(profileWindowSchema->fields[index].typeToken == "unknown", "DyAG raw field type");
                failures += Expect(
                    profileWindowSchema->fields[index].rawSpec == rawSpecs[index],
                    "DyAG raw field spec");
                failures += Expect(profileWindowSchema->fields[index].source == "xlsx", "DyAG raw field source");
            }
        }
        failures += Expect(
            profileWindowSchema->resolvedFormat ==
                "{windowType:i62} {buttonType:i62} {seqNo:i62} {objectIndex:i62} {data:es}",
            "DyAG resolvedFormat");
        failures += Expect(
            profileWindowSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "DyAG resolvedSource");
        failures += Expect(static_cast<bool>(packet), "DyAG ToPacket");
        failures += Expect(
            profileWindowSchema->resolvedFields.size() == resolvedNames.size(),
            "DyAG resolved field count");
        if (profileWindowSchema->resolvedFields.size() == resolvedNames.size()) {
            for (std::size_t index = 0; index < resolvedNames.size(); ++index) {
                failures += Expect(
                    profileWindowSchema->resolvedFields[index].index == static_cast<int>(index),
                    "DyAG resolved field index");
                failures += Expect(
                    profileWindowSchema->resolvedFields[index].name == resolvedNames[index],
                    "DyAG resolved field name");
                failures += Expect(
                    profileWindowSchema->resolvedFields[index].typeToken == resolvedTypes[index],
                    "DyAG resolved field type");
                failures += Expect(
                    profileWindowSchema->resolvedFields[index].rawSpec == resolvedSpecs[index],
                    "DyAG resolved field spec");
                failures += Expect(
                    profileWindowSchema->resolvedFields[index].source ==
                        "xlsx+ServerC+ClientIda+src/packet",
                    "DyAG resolved field source");
            }
        }
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *profileWindowSchema,
                packet.Value(),
                "DyAG resolved/ToPacket field order");
        }
    }

    const PacketSchema* profileMailArriveSchema = FindPacket(packets, "KyN");
    failures += Expect(profileMailArriveSchema != nullptr, "KyN schema exists");
    if (profileMailArriveSchema != nullptr) {
        const cg::packet::receive::ProfileMailArrive profileMailArrive(
            1,
            100,
            50,
            "Alice",
            "Guild",
            "Title",
            2,
            "2026/ 7/29 12:34",
            "message",
            3);
        const cg::packet::PacketResult<cg::packet::Packet> packet = profileMailArrive.ToPacket();
        failures += Expect(packet.HasValue(), "KyN ToPacket");
        failures += Expect(profileMailArriveSchema->direction == "服务端->客户端", "KyN direction");
        failures += Expect(profileMailArriveSchema->legacyKind == "SEND", "KyN legacyKind");
        failures += Expect(profileMailArriveSchema->source == "xlsx", "KyN raw source");
        failures += Expect(profileMailArriveSchema->rawFormat == "{data}", "KyN rawFormat");
        failures += Expect(profileMailArriveSchema->fields.size() == 1U, "KyN raw field count");
        if (profileMailArriveSchema->fields.size() == 1U) {
            failures += Expect(profileMailArriveSchema->fields[0].index == 0, "KyN raw field index");
            failures += Expect(profileMailArriveSchema->fields[0].name == "data", "KyN raw field name");
            failures += Expect(profileMailArriveSchema->fields[0].typeToken == "unknown", "KyN raw field type");
            failures += Expect(profileMailArriveSchema->fields[0].rawSpec == "{data}", "KyN raw field spec");
            failures += Expect(profileMailArriveSchema->fields[0].source == "xlsx", "KyN raw field source");
        }
        failures += Expect(profileMailArriveSchema->resolvedFormat == "{data:es}", "KyN resolvedFormat");
        failures += Expect(
            profileMailArriveSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "KyN resolvedSource");
        failures += Expect(profileMailArriveSchema->resolvedFields.size() == 1U, "KyN resolved field count");
        if (profileMailArriveSchema->resolvedFields.size() == 1U) {
            failures += Expect(profileMailArriveSchema->resolvedFields[0].index == 0, "KyN resolved field index");
            failures += Expect(profileMailArriveSchema->resolvedFields[0].name == "data", "KyN resolved field name");
            failures += Expect(profileMailArriveSchema->resolvedFields[0].typeToken == "es", "KyN resolved field type");
            failures += Expect(
                profileMailArriveSchema->resolvedFields[0].rawSpec == "{data:es}",
                "KyN resolved field spec");
            failures += Expect(
                profileMailArriveSchema->resolvedFields[0].source ==
                    "xlsx+ServerC+ClientIda+src/packet",
                "KyN resolved field source");
        }
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *profileMailArriveSchema,
                packet.Value(),
                "KyN resolved/ToPacket field order");
        }
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

    const PacketSchema* movementRequestSchema = FindPacket(packets, "zA");
    failures += Expect(movementRequestSchema != nullptr, "zA schema exists");
    if (movementRequestSchema != nullptr) {
        const cg::packet::send::MoveRequest movementRequest(10, 20, "a", 256);
        const cg::packet::PacketResult<cg::packet::Packet> packet = movementRequest.ToPacket();
        failures += Expect(movementRequestSchema->direction == "客户端->服务端", "zA direction");
        failures += Expect(movementRequestSchema->legacyKind == "RECV", "zA legacyKind");
        failures += Expect(
            movementRequestSchema->rawFormat ==
                "{x:i62} {y:i62} {direction:es} {warpnum::i62}",
            "zA malformed rawFormat preserved");
        failures += Expect(movementRequestSchema->source == "xlsx", "zA raw source");
        failures += Expect(movementRequestSchema->fields.size() == 4U, "zA raw field count");
        if (movementRequestSchema->fields.size() == 4U) {
            const FieldSchema& rawWarpNumber = movementRequestSchema->fields[3];
            failures += Expect(
                rawWarpNumber.index == 3 &&
                    rawWarpNumber.name == "warpnum" &&
                    rawWarpNumber.typeToken == ":i62" &&
                    rawWarpNumber.rawSpec == "{warpnum::i62}" &&
                    rawWarpNumber.source == "xlsx",
                "zA malformed raw warpnum field preserved");
        }
        failures += Expect(movementRequestSchema->hasResolvedFormat, "zA resolvedFormat present");
        failures += Expect(
            movementRequestSchema->resolvedFormat ==
                "{x:i62} {y:i62} {direction:es} {warpnum:i62}",
            "zA resolvedFormat");
        failures += Expect(
            movementRequestSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "zA resolvedSource");
        failures += Expect(movementRequestSchema->hasResolvedFields, "zA resolvedFields present");
        failures += Expect(
            movementRequestSchema->resolvedFields.size() == 4U,
            "zA resolved field count");
        if (movementRequestSchema->resolvedFields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "x",
                "y",
                "direction",
                "warpnum",
            }};
            constexpr std::array<std::string_view, 4U> types = {{
                "i62",
                "i62",
                "es",
                "i62",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{x:i62}",
                "{y:i62}",
                "{direction:es}",
                "{warpnum:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = movementRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "zA resolved field");
            }
        }
        failures += Expect(packet.HasValue(), "zA ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *movementRequestSchema,
                packet.Value(),
                "zA resolved/Spec field order");
        }
    }

    const PacketSchema* deprecatedMovementRequestSchema = FindPacket(packets, "sz");
    failures += Expect(deprecatedMovementRequestSchema != nullptr, "sz schema exists");
    if (deprecatedMovementRequestSchema != nullptr) {
        constexpr std::array<std::string_view, 3U> fieldNames = {{
            "x",
            "y",
            "direction",
        }};
        constexpr std::array<std::string_view, 3U> fieldTypes = {{
            "i62",
            "i62",
            "es",
        }};
        constexpr std::array<std::string_view, 3U> fieldSpecs = {{
            "{x:i62}",
            "{y:i62}",
            "{direction:es}",
        }};

        failures += Expect(
            deprecatedMovementRequestSchema->direction == "客户端->服务端",
            "sz direction");
        failures += Expect(
            deprecatedMovementRequestSchema->legacyKind == "RECV",
            "sz legacyKind");
        failures += Expect(
            deprecatedMovementRequestSchema->clientNamespace == "send",
            "sz namespace");
        failures += Expect(
            deprecatedMovementRequestSchema->xlsxStatus == "完成",
            "sz xlsxStatus");
        failures += Expect(
            deprecatedMovementRequestSchema->rawFormat ==
                "{x:i62} {y:i62} {direction:es}",
            "sz rawFormat");
        failures += Expect(deprecatedMovementRequestSchema->source == "xlsx", "sz raw source");
        failures += Expect(
            deprecatedMovementRequestSchema->contractStatus ==
                "no-effective-wire-contract",
            "sz no-effective-wire-contract sentinel");
        failures += Expect(
            deprecatedMovementRequestSchema->resolvedSource == "xlsx+ServerC",
            "sz resolvedSource");
        failures += Expect(
            !deprecatedMovementRequestSchema->hasResolvedFormat,
            "sz resolvedFormat absent");
        failures += Expect(
            !deprecatedMovementRequestSchema->hasResolvedFields,
            "sz resolvedFields absent");
        failures += Expect(
            deprecatedMovementRequestSchema->fields.size() == fieldNames.size(),
            "sz raw field count");
        if (deprecatedMovementRequestSchema->fields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                const FieldSchema& field = deprecatedMovementRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == fieldTypes[index] &&
                        field.rawSpec == fieldSpecs[index] &&
                        field.source == "xlsx",
                    "sz raw field");
            }
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "sz"),
        "sz absent from send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "sz"),
        "sz absent from receive typed manifest");

    const PacketSchema* encounterRequestSchema = FindPacket(packets, "KeOX");
    failures += Expect(encounterRequestSchema != nullptr, "KeOX schema exists");
    if (encounterRequestSchema != nullptr) {
        const cg::packet::Packet packet = cg::packet::send::EncounterRequest(10, 20).ToPacket();
        constexpr std::array<std::string_view, 2U> fieldNames = {{"x", "y"}};
        constexpr std::array<std::string_view, 2U> rawSpecs = {{"{x}", "{y}"}};
        constexpr std::array<std::string_view, 2U> resolvedSpecs = {{
            "{x:i62}",
            "{y:i62}",
        }};

        failures += Expect(
            encounterRequestSchema->direction == "客户端->服务端",
            "KeOX direction");
        failures += Expect(encounterRequestSchema->legacyKind == "RECV", "KeOX legacyKind");
        failures += Expect(encounterRequestSchema->clientNamespace == "send", "KeOX namespace");
        failures += Expect(encounterRequestSchema->xlsxStatus == "完成", "KeOX xlsxStatus");
        failures += Expect(
            encounterRequestSchema->rawFormat == "{x} {y}",
            "KeOX rawFormat");
        failures += Expect(!encounterRequestSchema->formatEmpty, "KeOX formatEmpty false");
        failures += Expect(encounterRequestSchema->source == "xlsx", "KeOX raw source");
        failures += Expect(
            encounterRequestSchema->fields.size() == fieldNames.size(),
            "KeOX raw field count");
        if (encounterRequestSchema->fields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                const FieldSchema& field = encounterRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "KeOX raw field");
            }
        }

        failures += Expect(
            encounterRequestSchema->hasResolvedFormat,
            "KeOX resolvedFormat present");
        failures += Expect(
            encounterRequestSchema->resolvedFormat == "{x:i62} {y:i62}",
            "KeOX resolvedFormat");
        failures += Expect(
            encounterRequestSchema->resolvedSource == "xlsx+ClientIda+src/packet",
            "KeOX resolvedSource");
        failures += Expect(
            encounterRequestSchema->resolvedSource.find("ServerC") == std::string::npos,
            "KeOX resolvedSource excludes ServerC");
        failures += Expect(
            encounterRequestSchema->hasResolvedFields,
            "KeOX resolvedFields present");
        failures += Expect(
            encounterRequestSchema->resolvedFields.size() == fieldNames.size(),
            "KeOX resolved field count");
        if (encounterRequestSchema->resolvedFields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                const FieldSchema& field = encounterRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == resolvedSpecs[index] &&
                        field.source == "xlsx+ClientIda+src/packet",
                    "KeOX resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *encounterRequestSchema,
            cg::packet::send::EncounterRequest::Spec,
            "KeOX Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *encounterRequestSchema,
            packet,
            "KeOX resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "KeOX"),
        "KeOX present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "KeOX"),
        "KeOX absent from receive typed manifest");

    const PacketSchema* objectDeleteSchema = FindPacket(packets, "uSB");
    failures += Expect(objectDeleteSchema != nullptr, "uSB schema exists");
    if (objectDeleteSchema != nullptr) {
        const cg::packet::receive::ObjectDelete objectDelete({1});
        const cg::packet::Packet packet = objectDelete.ToPacket();
        const cg::packet::PacketSpec& spec = cg::packet::receive::ObjectDelete::Spec;
        failures += Expect(objectDeleteSchema->direction == "服务端->客户端", "uSB direction");
        failures += Expect(objectDeleteSchema->legacyKind == "SEND", "uSB legacyKind");
        failures += Expect(
            objectDeleteSchema->rawFormat == "{data:es} {objectIndex:i62}",
            "uSB rawFormat");
        failures += Expect(objectDeleteSchema->source == "xlsx", "uSB raw source");
        failures += Expect(objectDeleteSchema->fields.size() == 2U, "uSB raw field count");
        if (objectDeleteSchema->fields.size() == 2U) {
            failures += Expect(
                objectDeleteSchema->fields[0].index == 0 &&
                    objectDeleteSchema->fields[0].name == "data" &&
                    objectDeleteSchema->fields[0].typeToken == "es" &&
                    objectDeleteSchema->fields[0].rawSpec == "{data:es}" &&
                    objectDeleteSchema->fields[0].source == "xlsx",
                "uSB raw data field");
            failures += Expect(
                objectDeleteSchema->fields[1].index == 1 &&
                    objectDeleteSchema->fields[1].name == "objectIndex" &&
                    objectDeleteSchema->fields[1].typeToken == "i62" &&
                    objectDeleteSchema->fields[1].rawSpec == "{objectIndex:i62}" &&
                    objectDeleteSchema->fields[1].source == "xlsx",
                "uSB raw objectIndex conflict field");
        }
        failures += Expect(objectDeleteSchema->resolvedFormat == "{data:es}", "uSB resolvedFormat");
        failures += Expect(
            objectDeleteSchema->resolvedSource == "ServerC+src/packet",
            "uSB resolvedSource");
        failures += Expect(spec.code == "uSB", "uSB Spec code");
        failures += Expect(
            spec.direction == cg::packet::PacketDirection::ServerToClient,
            "uSB Spec direction");
        failures += Expect(spec.fieldCount == 1U, "uSB Spec field count");
        if (spec.fieldCount == 1U) {
            failures += Expect(spec.fields != nullptr, "uSB Spec fields");
            if (spec.fields != nullptr) {
                failures += Expect(spec.fields[0].name == "data", "uSB Spec field name");
                failures += Expect(
                    spec.fields[0].type == cg::packet::PacketFieldType::EscapedString,
                    "uSB Spec field type");
            }
        }
        failures += Expect(objectDeleteSchema->resolvedFields.size() == 1U, "uSB resolved field count");
        if (objectDeleteSchema->resolvedFields.size() == 1U) {
            failures += Expect(
                objectDeleteSchema->resolvedFields[0].rawSpec == "{data:es}",
                "uSB resolved field spec");
            failures += Expect(
                objectDeleteSchema->resolvedFields[0].source == "ServerC+src/packet",
                "uSB resolved field source");
        }
        failures += ExpectResolvedFieldsMatch(*objectDeleteSchema, packet, "uSB resolved/Spec field order");
    }

    const PacketSchema* guildMemberAddedSchema = FindPacket(packets, "GC");
    failures += Expect(guildMemberAddedSchema != nullptr, "GC schema exists");
    if (guildMemberAddedSchema != nullptr) {
        const cg::packet::receive::GuildJoinNotification notification;
        const cg::packet::Packet packet = notification.ToPacket();
        failures += Expect(
            guildMemberAddedSchema->direction == "服务端->客户端",
            "GC direction");
        failures += Expect(guildMemberAddedSchema->legacyKind == "SEND", "GC legacyKind");
        failures += Expect(
            guildMemberAddedSchema->originalFormat == "GC",
            "GC originalFormat");
        failures += Expect(guildMemberAddedSchema->rawFormat.empty(), "GC rawFormat empty");
        failures += Expect(!guildMemberAddedSchema->formatEmpty, "GC formatEmpty false");
        failures += Expect(guildMemberAddedSchema->source == "xlsx", "GC raw source");
        failures += Expect(guildMemberAddedSchema->fields.empty(), "GC raw fields empty");
        failures += Expect(
            guildMemberAddedSchema->hasResolvedFormat,
            "GC resolvedFormat present");
        failures += Expect(
            guildMemberAddedSchema->resolvedFormat.empty(),
            "GC resolvedFormat empty");
        failures += Expect(
            guildMemberAddedSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "GC resolvedSource");
        failures += Expect(
            guildMemberAddedSchema->hasResolvedFields,
            "GC resolvedFields present");
        failures += Expect(
            guildMemberAddedSchema->resolvedFields.empty(),
            "GC resolvedFields empty");

        failures += ExpectResolvedFieldsMatch(
            *guildMemberAddedSchema,
            packet,
            "GC resolved/Spec field order");
    }

    const PacketSchema* tradeOutcomeSchema = FindPacket(packets, "We");
    failures += Expect(tradeOutcomeSchema != nullptr, "We schema exists");
    if (tradeOutcomeSchema != nullptr) {
        const cg::packet::receive::TradeStatus tradeStatus(
            cg::packet::receive::TradeStatusKind::Terminated);
        const cg::packet::Packet packet = tradeStatus.ToPacket();
        failures += Expect(
            tradeOutcomeSchema->direction == "服务端->客户端",
            "We direction");
        failures += Expect(tradeOutcomeSchema->legacyKind == "SEND", "We legacyKind");
        failures += Expect(
            tradeOutcomeSchema->originalFormat == "0终止 1确认 2完成",
            "We originalFormat");
        failures += Expect(tradeOutcomeSchema->rawFormat.empty(), "We rawFormat empty");
        failures += Expect(!tradeOutcomeSchema->formatEmpty, "We formatEmpty false");
        failures += Expect(tradeOutcomeSchema->source == "xlsx", "We raw source");
        failures += Expect(tradeOutcomeSchema->fields.empty(), "We raw fields empty");
        failures += Expect(tradeOutcomeSchema->hasResolvedFormat, "We resolvedFormat present");
        failures += Expect(
            tradeOutcomeSchema->resolvedFormat == "{status:i62}",
            "We resolvedFormat");
        failures += Expect(
            tradeOutcomeSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "We resolvedSource");
        failures += Expect(tradeOutcomeSchema->hasResolvedFields, "We resolvedFields present");
        failures += Expect(
            tradeOutcomeSchema->resolvedFields.size() == 1U,
            "We resolved field count");
        if (tradeOutcomeSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = tradeOutcomeSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "status" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{status:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "We resolved field");
        }
        failures += ExpectResolvedFieldsMatch(
            *tradeOutcomeSchema,
            packet,
            "We resolved/Spec field order");
    }

    const PacketSchema* separationPositionSchema = FindPacket(packets, "cZt");
    failures += Expect(separationPositionSchema != nullptr, "cZt schema exists");
    if (separationPositionSchema != nullptr) {
        const cg::packet::send::PartyLeavePositionSync positionSync(10, 20, 7);
        const cg::packet::Packet packet = positionSync.ToPacket();
        failures += Expect(
            separationPositionSchema->direction == "客户端->服务端",
            "cZt direction");
        failures += Expect(separationPositionSchema->legacyKind == "RECV", "cZt legacyKind");
        failures += Expect(
            separationPositionSchema->originalFormat == "cZt x:i62 y:i62 dir:i62",
            "cZt originalFormat");
        failures += Expect(separationPositionSchema->rawFormat.empty(), "cZt rawFormat empty");
        failures += Expect(!separationPositionSchema->formatEmpty, "cZt formatEmpty false");
        failures += Expect(separationPositionSchema->source == "xlsx", "cZt raw source");
        failures += Expect(separationPositionSchema->fields.empty(), "cZt raw fields empty");
        failures += Expect(
            separationPositionSchema->hasResolvedFormat,
            "cZt resolvedFormat present");
        failures += Expect(
            separationPositionSchema->resolvedFormat == "{x:i62} {y:i62} {dir:i62}",
            "cZt resolvedFormat");
        failures += Expect(
            separationPositionSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "cZt resolvedSource");
        failures += Expect(
            separationPositionSchema->hasResolvedFields,
            "cZt resolvedFields present");
        failures += Expect(
            separationPositionSchema->resolvedFields.size() == 3U,
            "cZt resolved field count");
        if (separationPositionSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{"x", "y", "dir"}};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{x:i62}",
                "{y:i62}",
                "{dir:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = separationPositionSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "cZt resolved field");
            }
        }
        failures += ExpectResolvedFieldsMatch(
            *separationPositionSchema,
            packet,
            "cZt resolved/Spec field order");
    }

    const PacketSchema* guildDissolveSchema = FindPacket(packets, "nkda");
    failures += Expect(guildDissolveSchema != nullptr, "nkda schema exists");
    if (guildDissolveSchema != nullptr) {
        const cg::packet::send::GuildDissolveRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            guildDissolveSchema->direction == "客户端->服务端",
            "nkda direction");
        failures += Expect(guildDissolveSchema->legacyKind == "RECV", "nkda legacyKind");
        failures += Expect(
            guildDissolveSchema->originalFormat == "nkda",
            "nkda originalFormat");
        failures += Expect(guildDissolveSchema->rawFormat.empty(), "nkda rawFormat empty");
        failures += Expect(!guildDissolveSchema->formatEmpty, "nkda formatEmpty false");
        failures += Expect(guildDissolveSchema->source == "xlsx", "nkda raw source");
        failures += Expect(guildDissolveSchema->fields.empty(), "nkda raw fields empty");
        failures += Expect(
            guildDissolveSchema->hasResolvedFormat,
            "nkda resolvedFormat present");
        failures += Expect(
            guildDissolveSchema->resolvedFormat == "{guildId:i62}",
            "nkda resolvedFormat");
        failures += Expect(
            guildDissolveSchema->resolvedSource == "xlsx+ServerC+src/packet",
            "nkda resolvedSource");
        failures += Expect(
            guildDissolveSchema->resolvedSource.find("ClientIda") == std::string::npos,
            "nkda resolvedSource excludes ClientIda");
        failures += Expect(
            guildDissolveSchema->hasResolvedFields,
            "nkda resolvedFields present");
        failures += Expect(
            guildDissolveSchema->resolvedFields.size() == 1U,
            "nkda resolved field count");
        if (guildDissolveSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = guildDissolveSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "guildId" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{guildId:i62}" &&
                    field.source == "xlsx+ServerC+src/packet",
                "nkda resolved field");
            failures += Expect(
                field.source.find("ClientIda") == std::string::npos,
                "nkda resolved field source excludes ClientIda");
        }

        failures += ExpectResolvedFieldsMatch(
            *guildDissolveSchema,
            packet,
            "nkda resolved/Spec field order");
    }

    const PacketSchema* bagPageManagerSchema = FindPacket(packets, "SWITMM");
    failures += Expect(bagPageManagerSchema != nullptr, "SWITMM schema exists");
    if (bagPageManagerSchema != nullptr) {
        const cg::packet::send::BagPageManagerRequest request;
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            bagPageManagerSchema->direction == "客户端->服务端",
            "SWITMM direction");
        failures += Expect(
            bagPageManagerSchema->legacyKind == "RECV",
            "SWITMM legacyKind");
        failures += Expect(
            bagPageManagerSchema->originalFormat == "SWITMM",
            "SWITMM originalFormat");
        failures += Expect(
            bagPageManagerSchema->rawFormat.empty(),
            "SWITMM rawFormat empty");
        failures += Expect(
            !bagPageManagerSchema->formatEmpty,
            "SWITMM formatEmpty false");
        failures += Expect(bagPageManagerSchema->source == "xlsx", "SWITMM raw source");
        failures += Expect(bagPageManagerSchema->fields.empty(), "SWITMM raw fields empty");
        failures += Expect(
            bagPageManagerSchema->hasResolvedFormat,
            "SWITMM resolvedFormat present");
        failures += Expect(
            bagPageManagerSchema->resolvedFormat.empty(),
            "SWITMM resolvedFormat empty");
        failures += Expect(
            bagPageManagerSchema->resolvedSource == "xlsx+ServerC+src/packet",
            "SWITMM resolvedSource");
        failures += Expect(
            bagPageManagerSchema->resolvedSource.find("ClientIda") == std::string::npos,
            "SWITMM resolvedSource excludes ClientIda");
        failures += Expect(
            bagPageManagerSchema->hasResolvedFields,
            "SWITMM resolvedFields present");
        failures += Expect(
            bagPageManagerSchema->resolvedFields.empty(),
            "SWITMM resolvedFields empty");

        failures += ExpectResolvedFieldsMatch(
            *bagPageManagerSchema,
            packet,
            "SWITMM resolved/Spec field order");
    }

    const PacketSchema* itemUseSchema = FindPacket(packets, "Ak");
    failures += Expect(itemUseSchema != nullptr, "Ak schema exists");
    if (itemUseSchema != nullptr) {
        const cg::packet::send::ItemUseRequest request(10, 20, 8, 1);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            itemUseSchema->direction == "客户端->服务端",
            "Ak direction");
        failures += Expect(itemUseSchema->legacyKind == "RECV", "Ak legacyKind");
        failures += Expect(
            itemUseSchema->rawFormat == "{x} {y} {haveitemindex} {toindex}",
            "Ak rawFormat");
        failures += Expect(!itemUseSchema->formatEmpty, "Ak formatEmpty false");
        failures += Expect(itemUseSchema->source == "xlsx", "Ak raw source");
        failures += Expect(itemUseSchema->fields.size() == 4U, "Ak raw field count");
        if (itemUseSchema->fields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "x",
                "y",
                "haveitemindex",
                "toindex",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{x}",
                "{y}",
                "{haveitemindex}",
                "{toindex}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = itemUseSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "Ak raw field");
            }
        }

        failures += Expect(
            itemUseSchema->hasResolvedFormat,
            "Ak resolvedFormat present");
        failures += Expect(
            itemUseSchema->resolvedFormat ==
                "{x:i62} {y:i62} {haveitemindex:i62} {toindex:i62}",
            "Ak resolvedFormat");
        failures += Expect(
            itemUseSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "Ak resolvedSource");
        failures += Expect(
            itemUseSchema->hasResolvedFields,
            "Ak resolvedFields present");
        failures += Expect(
            itemUseSchema->resolvedFields.size() == 4U,
            "Ak resolved field count");
        if (itemUseSchema->resolvedFields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "x",
                "y",
                "haveitemindex",
                "toindex",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{x:i62}",
                "{y:i62}",
                "{haveitemindex:i62}",
                "{toindex:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = itemUseSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "Ak resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *itemUseSchema,
            cg::packet::send::ItemUseRequest::Spec,
            "Ak Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *itemUseSchema,
            packet,
            "Ak resolved field order");
    }

    const PacketSchema* fieldRecoveryUseSchema = FindPacket(packets, "iVfo");
    failures += Expect(fieldRecoveryUseSchema != nullptr, "iVfo schema exists");
    if (fieldRecoveryUseSchema != nullptr) {
        const cg::packet::send::FieldRecoveryItemUseRequest request(10, 20, 8, 1);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            fieldRecoveryUseSchema->direction == "客户端->服务端",
            "iVfo direction");
        failures += Expect(
            fieldRecoveryUseSchema->legacyKind == "RECV",
            "iVfo legacyKind");
        failures += Expect(
            fieldRecoveryUseSchema->rawFormat ==
                "{x} {y} {haveitemindex} {toindex}",
            "iVfo rawFormat");
        failures += Expect(
            !fieldRecoveryUseSchema->formatEmpty,
            "iVfo formatEmpty false");
        failures += Expect(
            fieldRecoveryUseSchema->source == "xlsx",
            "iVfo raw source");
        failures += Expect(
            fieldRecoveryUseSchema->fields.size() == 4U,
            "iVfo raw field count");
        if (fieldRecoveryUseSchema->fields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "x",
                "y",
                "haveitemindex",
                "toindex",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{x}",
                "{y}",
                "{haveitemindex}",
                "{toindex}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = fieldRecoveryUseSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "iVfo raw field");
            }
        }

        failures += Expect(
            fieldRecoveryUseSchema->hasResolvedFormat,
            "iVfo resolvedFormat present");
        failures += Expect(
            fieldRecoveryUseSchema->resolvedFormat ==
                "{x:i62} {y:i62} {haveitemindex:i62} {toindex:i62}",
            "iVfo resolvedFormat");
        failures += Expect(
            fieldRecoveryUseSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "iVfo resolvedSource");
        failures += Expect(
            fieldRecoveryUseSchema->hasResolvedFields,
            "iVfo resolvedFields present");
        failures += Expect(
            fieldRecoveryUseSchema->resolvedFields.size() == 4U,
            "iVfo resolved field count");
        if (fieldRecoveryUseSchema->resolvedFields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "x",
                "y",
                "haveitemindex",
                "toindex",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{x:i62}",
                "{y:i62}",
                "{haveitemindex:i62}",
                "{toindex:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    fieldRecoveryUseSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "iVfo resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *fieldRecoveryUseSchema,
            cg::packet::send::FieldRecoveryItemUseRequest::Spec,
            "iVfo Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *fieldRecoveryUseSchema,
            packet,
            "iVfo resolved field order");
    }

    const PacketSchema* itemPickupSchema = FindPacket(packets, "fIM");
    failures += Expect(itemPickupSchema != nullptr, "fIM schema exists");
    if (itemPickupSchema != nullptr) {
        const cg::packet::send::PickUpItem item(10, 20, 3);
        const cg::packet::Packet packet = item.ToPacket();
        failures += Expect(
            itemPickupSchema->direction == "客户端->服务端",
            "fIM direction");
        failures += Expect(itemPickupSchema->legacyKind == "RECV", "fIM legacyKind");
        failures += Expect(
            itemPickupSchema->rawFormat == "{x} {y} {dir}",
            "fIM rawFormat");
        failures += Expect(!itemPickupSchema->formatEmpty, "fIM formatEmpty false");
        failures += Expect(itemPickupSchema->source == "xlsx", "fIM raw source");
        failures += Expect(itemPickupSchema->fields.size() == 3U, "fIM raw field count");
        if (itemPickupSchema->fields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{"x", "y", "dir"}};
            constexpr std::array<std::string_view, 3U> specs = {{"{x}", "{y}", "{dir}"}};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = itemPickupSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "fIM raw field");
            }
        }
        failures += Expect(
            itemPickupSchema->hasResolvedFormat,
            "fIM resolvedFormat present");
        failures += Expect(
            itemPickupSchema->resolvedFormat == "{x:i62} {y:i62} {dir:i62}",
            "fIM resolvedFormat");
        failures += Expect(
            itemPickupSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "fIM resolvedSource");
        failures += Expect(
            itemPickupSchema->hasResolvedFields,
            "fIM resolvedFields present");
        failures += Expect(
            itemPickupSchema->resolvedFields.size() == 3U,
            "fIM resolved field count");
        if (itemPickupSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{"x", "y", "dir"}};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{x:i62}",
                "{y:i62}",
                "{dir:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = itemPickupSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "fIM resolved field");
            }
        }
        failures += ExpectResolvedFieldsMatch(
            *itemPickupSchema,
            packet,
            "fIM resolved/Spec field order");
    }

    const PacketSchema* petDropSchema = FindPacket(packets, "FvWm");
    failures += Expect(petDropSchema != nullptr, "FvWm schema exists");
    if (petDropSchema != nullptr) {
        const cg::packet::send::DropPet request(10, 20, 0);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            petDropSchema->direction == "客户端->服务端",
            "FvWm direction");
        failures += Expect(petDropSchema->legacyKind == "RECV", "FvWm legacyKind");
        failures += Expect(
            petDropSchema->rawFormat == "{x} {y} {petindex}",
            "FvWm rawFormat");
        failures += Expect(!petDropSchema->formatEmpty, "FvWm formatEmpty false");
        failures += Expect(petDropSchema->source == "xlsx", "FvWm raw source");
        failures += Expect(petDropSchema->fields.size() == 3U, "FvWm raw field count");
        if (petDropSchema->fields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{"x", "y", "petindex"}};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{x}",
                "{y}",
                "{petindex}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = petDropSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "FvWm raw field");
            }
        }

        failures += Expect(
            petDropSchema->hasResolvedFormat,
            "FvWm resolvedFormat present");
        failures += Expect(
            petDropSchema->resolvedFormat == "{x:i62} {y:i62} {petindex:i62}",
            "FvWm resolvedFormat");
        failures += Expect(
            petDropSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "FvWm resolvedSource");
        failures += Expect(
            petDropSchema->hasResolvedFields,
            "FvWm resolvedFields present");
        failures += Expect(
            petDropSchema->resolvedFields.size() == 3U,
            "FvWm resolved field count");
        if (petDropSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{"x", "y", "petindex"}};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{x:i62}",
                "{y:i62}",
                "{petindex:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = petDropSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "FvWm resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *petDropSchema,
            cg::packet::send::DropPet::Spec,
            "FvWm Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petDropSchema,
            packet,
            "FvWm resolved field order");
    }

    const PacketSchema* itemMoveSchema = FindPacket(packets, "yi");
    failures += Expect(itemMoveSchema != nullptr, "yi schema exists");
    if (itemMoveSchema != nullptr) {
        const cg::packet::send::ItemMoveRequest request(10, 20, 3);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            itemMoveSchema->direction == "客户端->服务端",
            "yi direction");
        failures += Expect(itemMoveSchema->legacyKind == "RECV", "yi legacyKind");
        failures += Expect(
            itemMoveSchema->rawFormat == "{fromindex} {toindex} {num}",
            "yi rawFormat");
        failures += Expect(!itemMoveSchema->formatEmpty, "yi formatEmpty false");
        failures += Expect(itemMoveSchema->source == "xlsx", "yi raw source");
        failures += Expect(itemMoveSchema->fields.size() == 3U, "yi raw field count");
        if (itemMoveSchema->fields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{
                "fromindex",
                "toindex",
                "num",
            }};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{fromindex}",
                "{toindex}",
                "{num}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = itemMoveSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "yi raw field");
            }
        }

        failures += Expect(
            itemMoveSchema->hasResolvedFormat,
            "yi resolvedFormat present");
        failures += Expect(
            itemMoveSchema->resolvedFormat ==
                "{fromindex:i62} {toindex:i62} {num:i62}",
            "yi resolvedFormat");
        failures += Expect(
            itemMoveSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "yi resolvedSource");
        failures += Expect(
            itemMoveSchema->hasResolvedFields,
            "yi resolvedFields present");
        failures += Expect(
            itemMoveSchema->resolvedFields.size() == 3U,
            "yi resolved field count");
        if (itemMoveSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{
                "fromindex",
                "toindex",
                "num",
            }};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{fromindex:i62}",
                "{toindex:i62}",
                "{num:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = itemMoveSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "yi resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *itemMoveSchema,
            cg::packet::send::ItemMoveRequest::Spec,
            "yi Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *itemMoveSchema,
            packet,
            "yi resolved field order");
    }

    const PacketSchema* itemRecipeRequestSchema = FindPacket(packets, "fS");
    failures += Expect(itemRecipeRequestSchema != nullptr, "fS schema exists");
    if (itemRecipeRequestSchema != nullptr) {
        const cg::packet::send::ItemRecipeRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            itemRecipeRequestSchema->direction == "客户端->服务端",
            "fS direction");
        failures += Expect(
            itemRecipeRequestSchema->legacyKind == "RECV",
            "fS legacyKind");
        failures += Expect(
            itemRecipeRequestSchema->clientNamespace == "send",
            "fS namespace");
        failures += Expect(
            itemRecipeRequestSchema->rawFormat == "{haveskillindex}",
            "fS rawFormat");
        failures += Expect(
            !itemRecipeRequestSchema->formatEmpty,
            "fS formatEmpty false");
        failures += Expect(
            itemRecipeRequestSchema->source == "xlsx",
            "fS raw source");
        failures += Expect(
            itemRecipeRequestSchema->fields.size() == 1U,
            "fS raw field count");
        if (itemRecipeRequestSchema->fields.size() == 1U) {
            const FieldSchema& field = itemRecipeRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "haveskillindex" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{haveskillindex}" &&
                    field.source == "xlsx",
                "fS raw field");
        }

        failures += Expect(
            itemRecipeRequestSchema->hasResolvedFormat,
            "fS resolvedFormat present");
        failures += Expect(
            itemRecipeRequestSchema->resolvedFormat == "{haveskillindex:i62}",
            "fS resolvedFormat");
        failures += Expect(
            itemRecipeRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "fS resolvedSource");
        failures += Expect(
            itemRecipeRequestSchema->hasResolvedFields,
            "fS resolvedFields present");
        failures += Expect(
            itemRecipeRequestSchema->resolvedFields.size() == 1U,
            "fS resolved field count");
        if (itemRecipeRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = itemRecipeRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "haveskillindex" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{haveskillindex:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "fS resolved field");
        }

        failures += ExpectEffectiveFieldsMatch(
            *itemRecipeRequestSchema,
            cg::packet::send::ItemRecipeRequest::Spec,
            "fS Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *itemRecipeRequestSchema,
            packet,
            "fS resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "fS"),
        "fS present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "fS"),
        "fS absent from receive typed manifest");

    const PacketSchema* albumInfoRequestSchema = FindPacket(packets, "kza");
    failures += Expect(albumInfoRequestSchema != nullptr, "kza schema exists");
    if (albumInfoRequestSchema != nullptr) {
        const cg::packet::send::AlbumInfoRequest request({10, 20});
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            albumInfoRequestSchema->direction == "客户端->服务端",
            "kza direction");
        failures += Expect(
            albumInfoRequestSchema->legacyKind == "RECV",
            "kza legacyKind");
        failures += Expect(
            albumInfoRequestSchema->clientNamespace == "send",
            "kza namespace");
        failures += Expect(
            albumInfoRequestSchema->xlsxStatus == "完成",
            "kza xlsxStatus");
        failures += Expect(
            albumInfoRequestSchema->rawFormat == "{albumid}",
            "kza rawFormat");
        failures += Expect(
            !albumInfoRequestSchema->formatEmpty,
            "kza formatEmpty false");
        failures += Expect(
            albumInfoRequestSchema->source == "xlsx",
            "kza raw source");
        failures += Expect(
            albumInfoRequestSchema->fields.size() == 1U,
            "kza raw field count");
        if (albumInfoRequestSchema->fields.size() == 1U) {
            const FieldSchema& field = albumInfoRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "albumid" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{albumid}" &&
                    field.source == "xlsx",
                "kza raw field");
        }

        failures += Expect(
            albumInfoRequestSchema->hasResolvedFormat,
            "kza resolvedFormat present");
        failures += Expect(
            albumInfoRequestSchema->resolvedFormat ==
                "{count:i62} {albumid:i62}",
            "kza resolvedFormat");
        failures += Expect(
            albumInfoRequestSchema->resolvedSource ==
                "xlsx+ClientIda+src/packet",
            "kza resolvedSource");
        failures += Expect(
            albumInfoRequestSchema->resolvedSource.find("ServerC") ==
                std::string::npos,
            "kza resolvedSource excludes ServerC");
        failures += Expect(
            albumInfoRequestSchema->hasResolvedFields,
            "kza resolvedFields present");
        failures += Expect(
            albumInfoRequestSchema->resolvedFields.size() == 2U,
            "kza resolved field count");
        if (albumInfoRequestSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> fieldNames = {{
                "count",
                "albumid",
            }};
            constexpr std::array<std::string_view, 2U> fieldSpecs = {{
                "{count:i62}",
                "{albumid:i62}",
            }};
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                const FieldSchema& field = albumInfoRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == fieldSpecs[index] &&
                        field.source == "xlsx+ClientIda+src/packet",
                    "kza resolved field");
            }
        }

        failures += Expect(
            albumInfoRequestSchema->hasRepeatedTail,
            "kza repeatedTail present");
        failures += Expect(
            albumInfoRequestSchema->repeatedTail.fieldIndex == 1,
            "kza repeatedTail fieldIndex");
        failures += Expect(
            albumInfoRequestSchema->repeatedTail.countFieldIndex == 0,
            "kza repeatedTail countFieldIndex");
        failures += ExpectEffectiveFieldsMatch(
            *albumInfoRequestSchema,
            cg::packet::send::AlbumInfoRequest::Spec,
            "kza Spec matches repeated template");
        failures += ExpectRepeatedTailFieldsMatch(
            *albumInfoRequestSchema,
            packet,
            "kza runtime repeated field order");
        failures += Expect(packet.fields.size() == 3U, "kza runtime field count");
        if (packet.fields.size() == 3U) {
            failures += Expect(packet.fields[0].value == "2", "kza runtime count");
            failures += Expect(packet.fields[1].value == "10", "kza runtime first albumid");
            failures += Expect(packet.fields[2].value == "20", "kza runtime second albumid");
        }
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "kza"),
        "kza present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "kza"),
        "kza absent from receive typed manifest");

    const PacketSchema* albumNameRequestSchema = FindPacket(packets, "LaR");
    failures += Expect(albumNameRequestSchema != nullptr, "LaR schema exists");
    if (albumNameRequestSchema != nullptr) {
        const cg::packet::send::AlbumNameRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            albumNameRequestSchema->direction == "客户端->服务端",
            "LaR direction");
        failures += Expect(
            albumNameRequestSchema->legacyKind == "RECV",
            "LaR legacyKind");
        failures += Expect(
            albumNameRequestSchema->clientNamespace == "send",
            "LaR namespace");
        failures += Expect(
            albumNameRequestSchema->rawFormat == "{albumid}",
            "LaR rawFormat");
        failures += Expect(
            !albumNameRequestSchema->formatEmpty,
            "LaR formatEmpty false");
        failures += Expect(
            albumNameRequestSchema->source == "xlsx",
            "LaR raw source");
        failures += Expect(
            albumNameRequestSchema->fields.size() == 1U,
            "LaR raw field count");
        if (albumNameRequestSchema->fields.size() == 1U) {
            const FieldSchema& field = albumNameRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "albumid" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{albumid}" &&
                    field.source == "xlsx",
                "LaR raw field");
        }

        failures += Expect(
            albumNameRequestSchema->hasResolvedFormat,
            "LaR resolvedFormat present");
        failures += Expect(
            albumNameRequestSchema->resolvedFormat == "{albumid:i62}",
            "LaR resolvedFormat");
        failures += Expect(
            albumNameRequestSchema->resolvedSource == "xlsx+ServerC+src/packet",
            "LaR resolvedSource");
        failures += Expect(
            albumNameRequestSchema->resolvedSource.find("ClientIda") ==
                std::string::npos,
            "LaR resolvedSource excludes ClientIda");
        failures += Expect(
            albumNameRequestSchema->hasResolvedFields,
            "LaR resolvedFields present");
        failures += Expect(
            albumNameRequestSchema->resolvedFields.size() == 1U,
            "LaR resolved field count");
        if (albumNameRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = albumNameRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "albumid" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{albumid:i62}" &&
                    field.source == "xlsx+ServerC+src/packet" &&
                    field.source.find("ClientIda") == std::string::npos,
                "LaR resolved field");
        }

        failures += ExpectEffectiveFieldsMatch(
            *albumNameRequestSchema,
            cg::packet::send::AlbumNameRequest::Spec,
            "LaR Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *albumNameRequestSchema,
            packet,
            "LaR resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "LaR"),
        "LaR present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "LaR"),
        "LaR absent from receive typed manifest");

    const PacketSchema* addressBookDeleteSchema = FindPacket(packets, "FPkR");
    failures += Expect(addressBookDeleteSchema != nullptr, "FPkR schema exists");
    if (addressBookDeleteSchema != nullptr) {
        const cg::packet::send::AddressBookDeleteRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            addressBookDeleteSchema->direction == "客户端->服务端",
            "FPkR direction");
        failures += Expect(
            addressBookDeleteSchema->legacyKind == "RECV",
            "FPkR legacyKind");
        failures += Expect(
            addressBookDeleteSchema->clientNamespace == "send",
            "FPkR namespace");
        failures += Expect(
            addressBookDeleteSchema->rawFormat == "{index}",
            "FPkR rawFormat");
        failures += Expect(
            !addressBookDeleteSchema->formatEmpty,
            "FPkR formatEmpty false");
        failures += Expect(
            addressBookDeleteSchema->source == "xlsx",
            "FPkR raw source");
        failures += Expect(
            addressBookDeleteSchema->fields.size() == 1U,
            "FPkR raw field count");
        if (addressBookDeleteSchema->fields.size() == 1U) {
            const FieldSchema& field = addressBookDeleteSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "index" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{index}" &&
                    field.source == "xlsx",
                "FPkR raw field");
        }

        failures += Expect(
            addressBookDeleteSchema->hasResolvedFormat,
            "FPkR resolvedFormat present");
        failures += Expect(
            addressBookDeleteSchema->resolvedFormat == "{index:i62}",
            "FPkR resolvedFormat");
        failures += Expect(
            addressBookDeleteSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "FPkR resolvedSource");
        failures += Expect(
            addressBookDeleteSchema->hasResolvedFields,
            "FPkR resolvedFields present");
        failures += Expect(
            addressBookDeleteSchema->resolvedFields.size() == 1U,
            "FPkR resolved field count");
        if (addressBookDeleteSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = addressBookDeleteSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "index" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{index:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "FPkR resolved field");
        }

        failures += ExpectEffectiveFieldsMatch(
            *addressBookDeleteSchema,
            cg::packet::send::AddressBookDeleteRequest::Spec,
            "FPkR Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *addressBookDeleteSchema,
            packet,
            "FPkR resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "FPkR"),
        "FPkR present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "FPkR"),
        "FPkR absent from receive typed manifest");

    const PacketSchema* addressBookExchangeSchema = FindPacket(packets, "vcAz");
    failures += Expect(addressBookExchangeSchema != nullptr, "vcAz schema exists");
    if (addressBookExchangeSchema != nullptr) {
        const cg::packet::send::AddressBookExchangeRequest request(10, 20);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            addressBookExchangeSchema->direction == "客户端->服务端",
            "vcAz direction");
        failures += Expect(
            addressBookExchangeSchema->legacyKind == "RECV",
            "vcAz legacyKind");
        failures += Expect(
            addressBookExchangeSchema->clientNamespace == "send",
            "vcAz namespace");
        failures += Expect(
            addressBookExchangeSchema->rawFormat == "{x} {y}",
            "vcAz rawFormat");
        failures += Expect(
            !addressBookExchangeSchema->formatEmpty,
            "vcAz formatEmpty false");
        failures += Expect(
            addressBookExchangeSchema->source == "xlsx",
            "vcAz raw source");
        failures += Expect(
            addressBookExchangeSchema->fields.size() == 2U,
            "vcAz raw field count");
        if (addressBookExchangeSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"x", "y"}};
            constexpr std::array<std::string_view, 2U> specs = {{"{x}", "{y}"}};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = addressBookExchangeSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "vcAz raw field");
            }
        }

        failures += Expect(
            addressBookExchangeSchema->hasResolvedFormat,
            "vcAz resolvedFormat present");
        failures += Expect(
            addressBookExchangeSchema->resolvedFormat == "{x:i62} {y:i62}",
            "vcAz resolvedFormat");
        failures += Expect(
            addressBookExchangeSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "vcAz resolvedSource");
        failures += Expect(
            addressBookExchangeSchema->hasResolvedFields,
            "vcAz resolvedFields present");
        failures += Expect(
            addressBookExchangeSchema->resolvedFields.size() == 2U,
            "vcAz resolved field count");
        if (addressBookExchangeSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"x", "y"}};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{x:i62}",
                "{y:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    addressBookExchangeSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "vcAz resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *addressBookExchangeSchema,
            cg::packet::send::AddressBookExchangeRequest::Spec,
            "vcAz Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *addressBookExchangeSchema,
            packet,
            "vcAz resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "vcAz"),
        "vcAz present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "vcAz"),
        "vcAz absent from receive typed manifest");

    const PacketSchema* guildMemberListRequestSchema = FindPacket(packets, "CZ");
    failures += Expect(guildMemberListRequestSchema != nullptr, "CZ schema exists");
    if (guildMemberListRequestSchema != nullptr) {
        const cg::packet::send::GuildMemberListRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            guildMemberListRequestSchema->direction == "客户端->服务端",
            "CZ direction");
        failures += Expect(
            guildMemberListRequestSchema->legacyKind == "RECV",
            "CZ legacyKind");
        failures += Expect(
            guildMemberListRequestSchema->clientNamespace == "send",
            "CZ namespace");
        failures += Expect(
            guildMemberListRequestSchema->xlsxStatus == "进行中",
            "CZ xlsxStatus");
        failures += Expect(
            guildMemberListRequestSchema->rawFormat == "{time}",
            "CZ rawFormat");
        failures += Expect(
            !guildMemberListRequestSchema->formatEmpty,
            "CZ formatEmpty false");
        failures += Expect(
            guildMemberListRequestSchema->source == "xlsx",
            "CZ raw source");
        failures += Expect(
            guildMemberListRequestSchema->fields.size() == 1U,
            "CZ raw field count");
        if (guildMemberListRequestSchema->fields.size() == 1U) {
            const FieldSchema& field = guildMemberListRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "time" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{time}" &&
                    field.source == "xlsx",
                "CZ raw field");
        }

        failures += Expect(
            guildMemberListRequestSchema->hasResolvedFormat,
            "CZ resolvedFormat present");
        failures += Expect(
            guildMemberListRequestSchema->resolvedFormat == "{time:i62}",
            "CZ resolvedFormat");
        failures += Expect(
            guildMemberListRequestSchema->resolvedSource ==
                "xlsx+ServerC+src/packet",
            "CZ resolvedSource");
        failures += Expect(
            guildMemberListRequestSchema->resolvedSource.find("ClientIda") ==
                std::string::npos,
            "CZ resolvedSource excludes ClientIda");
        failures += Expect(
            guildMemberListRequestSchema->hasResolvedFields,
            "CZ resolvedFields present");
        failures += Expect(
            guildMemberListRequestSchema->resolvedFields.size() == 1U,
            "CZ resolved field count");
        if (guildMemberListRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = guildMemberListRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "time" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{time:i62}" &&
                    field.source == "xlsx+ServerC+src/packet" &&
                    field.source.find("ClientIda") == std::string::npos,
                "CZ resolved field");
        }

        failures += ExpectEffectiveFieldsMatch(
            *guildMemberListRequestSchema,
            cg::packet::send::GuildMemberListRequest::Spec,
            "CZ Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *guildMemberListRequestSchema,
            packet,
            "CZ resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "CZ"),
        "CZ present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "CZ"),
        "CZ absent from receive typed manifest");

    const PacketSchema* guildMemberTitleChangeRequestSchema =
        FindPacket(packets, "uHb");
    failures += Expect(
        guildMemberTitleChangeRequestSchema != nullptr,
        "uHb schema exists");
    if (guildMemberTitleChangeRequestSchema != nullptr) {
        const cg::packet::send::GuildMemberTitleChangeRequest request(10, 20);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            guildMemberTitleChangeRequestSchema->direction == "客户端->服务端",
            "uHb direction");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->legacyKind == "RECV",
            "uHb legacyKind");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->clientNamespace == "send",
            "uHb namespace");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->xlsxStatus == "进行中",
            "uHb xlsxStatus");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->rawFormat == "{index} {title}",
            "uHb rawFormat");
        failures += Expect(
            !guildMemberTitleChangeRequestSchema->formatEmpty,
            "uHb formatEmpty false");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->source == "xlsx",
            "uHb raw source");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->fields.size() == 2U,
            "uHb raw field count");
        if (guildMemberTitleChangeRequestSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{
                "index",
                "title",
            }};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{index}",
                "{title}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    guildMemberTitleChangeRequestSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "uHb raw field");
            }
        }

        failures += Expect(
            guildMemberTitleChangeRequestSchema->hasResolvedFormat,
            "uHb resolvedFormat present");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->resolvedFormat ==
                "{index:i62} {title:i62}",
            "uHb resolvedFormat");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "uHb resolvedSource");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->hasResolvedFields,
            "uHb resolvedFields present");
        failures += Expect(
            guildMemberTitleChangeRequestSchema->resolvedFields.size() == 2U,
            "uHb resolved field count");
        if (guildMemberTitleChangeRequestSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{
                "index",
                "title",
            }};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{index:i62}",
                "{title:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    guildMemberTitleChangeRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "uHb resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *guildMemberTitleChangeRequestSchema,
            cg::packet::send::GuildMemberTitleChangeRequest::Spec,
            "uHb Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *guildMemberTitleChangeRequestSchema,
            packet,
            "uHb resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "uHb"),
        "uHb present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "uHb"),
        "uHb absent from receive typed manifest");

    const PacketSchema* guildMemberInviteRequestSchema = FindPacket(packets, "npzi");
    failures += Expect(
        guildMemberInviteRequestSchema != nullptr,
        "npzi schema exists");
    if (guildMemberInviteRequestSchema != nullptr) {
        const cg::packet::send::GuildMemberInviteRequest request(10, 20);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            guildMemberInviteRequestSchema->direction == "客户端->服务端",
            "npzi direction");
        failures += Expect(
            guildMemberInviteRequestSchema->legacyKind == "RECV",
            "npzi legacyKind");
        failures += Expect(
            guildMemberInviteRequestSchema->clientNamespace == "send",
            "npzi namespace");
        failures += Expect(
            guildMemberInviteRequestSchema->xlsxStatus == "完成",
            "npzi xlsxStatus");
        failures += Expect(
            guildMemberInviteRequestSchema->rawFormat == "{x} {y}",
            "npzi rawFormat");
        failures += Expect(
            !guildMemberInviteRequestSchema->formatEmpty,
            "npzi formatEmpty false");
        failures += Expect(
            guildMemberInviteRequestSchema->source == "xlsx",
            "npzi raw source");
        failures += Expect(
            guildMemberInviteRequestSchema->fields.size() == 2U,
            "npzi raw field count");
        if (guildMemberInviteRequestSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"x", "y"}};
            constexpr std::array<std::string_view, 2U> specs = {{"{x}", "{y}"}};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = guildMemberInviteRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "npzi raw field");
            }
        }

        failures += Expect(
            guildMemberInviteRequestSchema->hasResolvedFormat,
            "npzi resolvedFormat present");
        failures += Expect(
            guildMemberInviteRequestSchema->resolvedFormat == "{x:i62} {y:i62}",
            "npzi resolvedFormat");
        failures += Expect(
            guildMemberInviteRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "npzi resolvedSource");
        failures += Expect(
            guildMemberInviteRequestSchema->hasResolvedFields,
            "npzi resolvedFields present");
        failures += Expect(
            guildMemberInviteRequestSchema->resolvedFields.size() == 2U,
            "npzi resolved field count");
        if (guildMemberInviteRequestSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"x", "y"}};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{x:i62}",
                "{y:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    guildMemberInviteRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "npzi resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *guildMemberInviteRequestSchema,
            cg::packet::send::GuildMemberInviteRequest::Spec,
            "npzi Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *guildMemberInviteRequestSchema,
            packet,
            "npzi resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "npzi"),
        "npzi present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "npzi"),
        "npzi absent from receive typed manifest");

    const PacketSchema* guildLeaveRequestSchema = FindPacket(packets, "XIsR");
    failures += Expect(guildLeaveRequestSchema != nullptr, "XIsR schema exists");
    if (guildLeaveRequestSchema != nullptr) {
        const cg::packet::send::GuildLeaveRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            guildLeaveRequestSchema->direction == "客户端->服务端",
            "XIsR direction");
        failures += Expect(
            guildLeaveRequestSchema->legacyKind == "RECV",
            "XIsR legacyKind");
        failures += Expect(
            guildLeaveRequestSchema->clientNamespace == "send",
            "XIsR namespace");
        failures += Expect(
            guildLeaveRequestSchema->xlsxStatus == "进行中",
            "XIsR xlsxStatus");
        failures += Expect(
            guildLeaveRequestSchema->rawFormat == "{id}",
            "XIsR rawFormat");
        failures += Expect(
            !guildLeaveRequestSchema->formatEmpty,
            "XIsR formatEmpty false");
        failures += Expect(guildLeaveRequestSchema->source == "xlsx", "XIsR raw source");
        failures += Expect(
            guildLeaveRequestSchema->fields.size() == 1U,
            "XIsR raw field count");
        if (guildLeaveRequestSchema->fields.size() == 1U) {
            const FieldSchema& field = guildLeaveRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "id" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{id}" &&
                    field.source == "xlsx",
                "XIsR raw field");
        }

        failures += Expect(
            guildLeaveRequestSchema->hasResolvedFormat,
            "XIsR resolvedFormat present");
        failures += Expect(
            guildLeaveRequestSchema->resolvedFormat == "{guildId:i62}",
            "XIsR resolvedFormat");
        failures += Expect(
            guildLeaveRequestSchema->resolvedSource == "xlsx+ServerC+src/packet",
            "XIsR resolvedSource");
        failures += Expect(
            guildLeaveRequestSchema->resolvedSource.find("ClientIda") ==
                std::string::npos,
            "XIsR resolvedSource excludes ClientIda");
        failures += Expect(
            guildLeaveRequestSchema->hasResolvedFields,
            "XIsR resolvedFields present");
        failures += Expect(
            guildLeaveRequestSchema->resolvedFields.size() == 1U,
            "XIsR resolved field count");
        if (guildLeaveRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = guildLeaveRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "guildId" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{guildId:i62}" &&
                    field.source == "xlsx+ServerC+src/packet",
                "XIsR resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *guildLeaveRequestSchema,
            cg::packet::send::GuildLeaveRequest::Spec,
            "XIsR Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *guildLeaveRequestSchema,
            packet,
            "XIsR resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "XIsR"),
        "XIsR present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "XIsR"),
        "XIsR absent from receive typed manifest");

    const PacketSchema* guildMemberRemoveRequestSchema = FindPacket(packets, "dbQC");
    failures += Expect(
        guildMemberRemoveRequestSchema != nullptr,
        "dbQC schema exists");
    if (guildMemberRemoveRequestSchema != nullptr) {
        const cg::packet::send::GuildMemberRemoveRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            guildMemberRemoveRequestSchema->direction == "客户端->服务端",
            "dbQC direction");
        failures += Expect(
            guildMemberRemoveRequestSchema->legacyKind == "RECV",
            "dbQC legacyKind");
        failures += Expect(
            guildMemberRemoveRequestSchema->clientNamespace == "send",
            "dbQC namespace");
        failures += Expect(
            guildMemberRemoveRequestSchema->xlsxStatus == "进行中",
            "dbQC xlsxStatus");
        failures += Expect(
            guildMemberRemoveRequestSchema->rawFormat == "{index}",
            "dbQC rawFormat");
        failures += Expect(
            !guildMemberRemoveRequestSchema->formatEmpty,
            "dbQC formatEmpty false");
        failures += Expect(
            guildMemberRemoveRequestSchema->source == "xlsx",
            "dbQC raw source");
        failures += Expect(
            guildMemberRemoveRequestSchema->fields.size() == 1U,
            "dbQC raw field count");
        if (guildMemberRemoveRequestSchema->fields.size() == 1U) {
            const FieldSchema& field = guildMemberRemoveRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "index" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{index}" &&
                    field.source == "xlsx",
                "dbQC raw field");
        }

        failures += Expect(
            guildMemberRemoveRequestSchema->hasResolvedFormat,
            "dbQC resolvedFormat present");
        failures += Expect(
            guildMemberRemoveRequestSchema->resolvedFormat == "{index:i62}",
            "dbQC resolvedFormat");
        failures += Expect(
            guildMemberRemoveRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "dbQC resolvedSource");
        failures += Expect(
            guildMemberRemoveRequestSchema->hasResolvedFields,
            "dbQC resolvedFields present");
        failures += Expect(
            guildMemberRemoveRequestSchema->resolvedFields.size() == 1U,
            "dbQC resolved field count");
        if (guildMemberRemoveRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                guildMemberRemoveRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "index" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{index:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "dbQC resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *guildMemberRemoveRequestSchema,
            cg::packet::send::GuildMemberRemoveRequest::Spec,
            "dbQC Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *guildMemberRemoveRequestSchema,
            packet,
            "dbQC resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "dbQC"),
        "dbQC present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "dbQC"),
        "dbQC absent from receive typed manifest");

    const PacketSchema* profileListsRequestSchema = FindPacket(packets, "HoW");
    failures += Expect(profileListsRequestSchema != nullptr, "HoW schema exists");
    if (profileListsRequestSchema != nullptr) {
        const cg::packet::send::ProfileListsRequest request(3);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            profileListsRequestSchema->direction == "客户端->服务端",
            "HoW direction");
        failures += Expect(
            profileListsRequestSchema->legacyKind == "RECV",
            "HoW legacyKind");
        failures += Expect(
            profileListsRequestSchema->clientNamespace == "send",
            "HoW namespace");
        failures += Expect(
            profileListsRequestSchema->xlsxStatus == "待办",
            "HoW xlsxStatus");
        failures += Expect(
            profileListsRequestSchema->rawFormat == "{flg}",
            "HoW rawFormat");
        failures += Expect(
            !profileListsRequestSchema->formatEmpty,
            "HoW formatEmpty false");
        failures += Expect(
            profileListsRequestSchema->source == "xlsx",
            "HoW raw source");
        failures += Expect(
            profileListsRequestSchema->fields.size() == 1U,
            "HoW raw field count");
        if (profileListsRequestSchema->fields.size() == 1U) {
            const FieldSchema& field = profileListsRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "flg" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{flg}" &&
                    field.source == "xlsx",
                "HoW raw field");
        }

        failures += Expect(
            profileListsRequestSchema->hasResolvedFormat,
            "HoW resolvedFormat present");
        failures += Expect(
            profileListsRequestSchema->resolvedFormat == "{flg:i62}",
            "HoW resolvedFormat");
        failures += Expect(
            profileListsRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "HoW resolvedSource");
        failures += Expect(
            profileListsRequestSchema->hasResolvedFields,
            "HoW resolvedFields present");
        failures += Expect(
            profileListsRequestSchema->resolvedFields.size() == 1U,
            "HoW resolved field count");
        if (profileListsRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = profileListsRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "flg" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{flg:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "HoW resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *profileListsRequestSchema,
            cg::packet::send::ProfileListsRequest::Spec,
            "HoW Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *profileListsRequestSchema,
            packet,
            "HoW resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "HoW"),
        "HoW present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "HoW"),
        "HoW absent from receive typed manifest");

    const PacketSchema* profileDataRequestSchema = FindPacket(packets, "TLKH");
    failures += Expect(profileDataRequestSchema != nullptr, "TLKH schema exists");
    if (profileDataRequestSchema != nullptr) {
        const cg::packet::send::ProfileDataRequest request(3, 10);
        const cg::packet::Packet packet = request.ToPacket();
        constexpr std::array<std::string_view, 2U> fieldNames = {{"flg", "index"}};
        constexpr std::array<std::string_view, 2U> rawSpecs = {{
            "{flg}",
            "{index}",
        }};
        constexpr std::array<std::string_view, 2U> resolvedSpecs = {{
            "{flg:i62}",
            "{index:i62}",
        }};

        failures += Expect(
            profileDataRequestSchema->direction == "客户端->服务端",
            "TLKH direction");
        failures += Expect(
            profileDataRequestSchema->legacyKind == "RECV",
            "TLKH legacyKind");
        failures += Expect(
            profileDataRequestSchema->clientNamespace == "send",
            "TLKH namespace");
        failures += Expect(
            profileDataRequestSchema->xlsxStatus == "进行中",
            "TLKH xlsxStatus");
        failures += Expect(
            profileDataRequestSchema->rawFormat == "{flg} {index}",
            "TLKH rawFormat");
        failures += Expect(
            !profileDataRequestSchema->formatEmpty,
            "TLKH formatEmpty false");
        failures += Expect(
            profileDataRequestSchema->source == "xlsx",
            "TLKH raw source");
        failures += Expect(
            profileDataRequestSchema->fields.size() == fieldNames.size(),
            "TLKH raw field count");
        if (profileDataRequestSchema->fields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                const FieldSchema& field = profileDataRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "TLKH raw field");
            }
        }

        failures += Expect(
            profileDataRequestSchema->hasResolvedFormat,
            "TLKH resolvedFormat present");
        failures += Expect(
            profileDataRequestSchema->resolvedFormat == "{flg:i62} {index:i62}",
            "TLKH resolvedFormat");
        failures += Expect(
            profileDataRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "TLKH resolvedSource");
        failures += Expect(
            profileDataRequestSchema->hasResolvedFields,
            "TLKH resolvedFields present");
        failures += Expect(
            profileDataRequestSchema->resolvedFields.size() == fieldNames.size(),
            "TLKH resolved field count");
        if (profileDataRequestSchema->resolvedFields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                const FieldSchema& field =
                    profileDataRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == resolvedSpecs[index] &&
                        field.source ==
                            "xlsx+ServerC+ClientIda+src/packet",
                    "TLKH resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *profileDataRequestSchema,
            cg::packet::send::ProfileDataRequest::Spec,
            "TLKH Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *profileDataRequestSchema,
            packet,
            "TLKH resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "TLKH"),
        "TLKH present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "TLKH"),
        "TLKH absent from receive typed manifest");

    const PacketSchema* playerStatusFlagsUpdateRequestSchema = FindPacket(packets, "dcMv");
    failures += Expect(
        playerStatusFlagsUpdateRequestSchema != nullptr,
        "dcMv schema exists");
    if (playerStatusFlagsUpdateRequestSchema != nullptr) {
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->direction == "客户端->服务端",
            "dcMv direction");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->legacyKind == "RECV",
            "dcMv legacyKind");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->clientNamespace == "send",
            "dcMv namespace");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->xlsxStatus == "完成",
            "dcMv xlsxStatus");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->rawFormat == "{status}",
            "dcMv rawFormat");
        failures += Expect(
            !playerStatusFlagsUpdateRequestSchema->formatEmpty,
            "dcMv formatEmpty false");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->source == "xlsx",
            "dcMv raw source");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->fields.size() == 1U,
            "dcMv raw field count");
        if (playerStatusFlagsUpdateRequestSchema->fields.size() == 1U) {
            const FieldSchema& field = playerStatusFlagsUpdateRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "status" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{status}" &&
                    field.source == "xlsx",
                "dcMv raw field");
        }

        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->hasResolvedFormat,
            "dcMv resolvedFormat present");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->resolvedFormat == "{flags:i62}",
            "dcMv resolvedFormat");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "dcMv resolvedSource");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->hasResolvedFields,
            "dcMv resolvedFields present");
        failures += Expect(
            playerStatusFlagsUpdateRequestSchema->resolvedFields.size() == 1U,
            "dcMv resolved field count");
        if (playerStatusFlagsUpdateRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                playerStatusFlagsUpdateRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "flags" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{flags:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "dcMv resolved field");
        }
        const cg::packet::send::PlayerStatusFlagsUpdateRequest request(125);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *playerStatusFlagsUpdateRequestSchema,
            cg::packet::send::PlayerStatusFlagsUpdateRequest::Spec,
            "dcMv Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *playerStatusFlagsUpdateRequestSchema,
            packet,
            "dcMv resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "dcMv"),
        "dcMv present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "dcMv"),
        "dcMv absent from receive typed manifest");

    const PacketSchema* partyMembershipRequestSchema = FindPacket(packets, "zn");
    failures += Expect(
        partyMembershipRequestSchema != nullptr,
        "zn schema exists");
    if (partyMembershipRequestSchema != nullptr) {
        constexpr std::array<std::string_view, 3U> fieldNames = {{
            "x",
            "y",
            "join",
        }};
        constexpr std::array<std::string_view, 3U> rawSpecs = {{
            "{x}",
            "{y}",
            "{join}",
        }};
        constexpr std::array<std::string_view, 3U> resolvedSpecs = {{
            "{x:i62}",
            "{y:i62}",
            "{join:i62}",
        }};

        failures += Expect(
            partyMembershipRequestSchema->direction == "客户端->服务端",
            "zn direction");
        failures += Expect(
            partyMembershipRequestSchema->legacyKind == "RECV",
            "zn legacyKind");
        failures += Expect(
            partyMembershipRequestSchema->clientNamespace == "send",
            "zn namespace");
        failures += Expect(
            partyMembershipRequestSchema->xlsxStatus == "完成",
            "zn xlsxStatus");
        failures += Expect(
            partyMembershipRequestSchema->rawFormat == "{x} {y} {join}",
            "zn rawFormat");
        failures += Expect(
            !partyMembershipRequestSchema->formatEmpty,
            "zn formatEmpty false");
        failures += Expect(
            partyMembershipRequestSchema->source == "xlsx",
            "zn raw source");
        failures += Expect(
            partyMembershipRequestSchema->fields.size() == fieldNames.size(),
            "zn raw field count");
        if (partyMembershipRequestSchema->fields.size() == fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                const FieldSchema& field =
                    partyMembershipRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "zn raw field");
            }
        }

        failures += Expect(
            partyMembershipRequestSchema->hasResolvedFormat,
            "zn resolvedFormat present");
        failures += Expect(
            partyMembershipRequestSchema->resolvedFormat ==
                "{x:i62} {y:i62} {join:i62}",
            "zn resolvedFormat");
        failures += Expect(
            partyMembershipRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "zn resolvedSource");
        failures += Expect(
            partyMembershipRequestSchema->hasResolvedFields,
            "zn resolvedFields present");
        failures += Expect(
            partyMembershipRequestSchema->resolvedFields.size() ==
                fieldNames.size(),
            "zn resolved field count");
        if (partyMembershipRequestSchema->resolvedFields.size() ==
            fieldNames.size()) {
            for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
                const FieldSchema& field =
                    partyMembershipRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == resolvedSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "zn resolved field");
            }
        }
        const cg::packet::send::PartyMembershipRequest request(10, 20, 1);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *partyMembershipRequestSchema,
            cg::packet::send::PartyMembershipRequest::Spec,
            "zn Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *partyMembershipRequestSchema,
            packet,
            "zn resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "zn"),
        "zn present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "zn"),
        "zn absent from receive typed manifest");

    const PacketSchema* petBattleStatusUpdateRequestSchema =
        FindPacket(packets, "LKQy");
    failures += Expect(
        petBattleStatusUpdateRequestSchema != nullptr,
        "LKQy schema exists");
    if (petBattleStatusUpdateRequestSchema != nullptr) {
        constexpr std::array<std::string_view, 5U> rawFieldNames = {{
            "statut0",
            "statut1",
            "statut2",
            "statut3",
            "statut4",
        }};
        constexpr std::array<std::string_view, 5U> rawSpecs = {{
            "{statut0}",
            "{statut1}",
            "{statut2}",
            "{statut3}",
            "{statut4}",
        }};
        constexpr std::array<std::string_view, 5U> resolvedFieldNames = {{
            "petStatus0",
            "petStatus1",
            "petStatus2",
            "petStatus3",
            "petStatus4",
        }};
        constexpr std::array<std::string_view, 5U> resolvedSpecs = {{
            "{petStatus0:i62}",
            "{petStatus1:i62}",
            "{petStatus2:i62}",
            "{petStatus3:i62}",
            "{petStatus4:i62}",
        }};

        failures += Expect(
            petBattleStatusUpdateRequestSchema->direction == "客户端->服务端",
            "LKQy direction");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->legacyKind == "RECV",
            "LKQy legacyKind");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->clientNamespace == "send",
            "LKQy namespace");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->xlsxStatus == "完成",
            "LKQy xlsxStatus");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->rawFormat ==
                "{statut0} {statut1} {statut2} {statut3} {statut4}",
            "LKQy rawFormat");
        failures += Expect(
            !petBattleStatusUpdateRequestSchema->formatEmpty,
            "LKQy formatEmpty false");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->source == "xlsx",
            "LKQy raw source");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->fields.size() ==
                rawFieldNames.size(),
            "LKQy raw field count");
        if (petBattleStatusUpdateRequestSchema->fields.size() ==
            rawFieldNames.size()) {
            for (std::size_t index = 0U;
                 index < rawFieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    petBattleStatusUpdateRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == rawFieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "LKQy raw field");
            }
        }

        failures += Expect(
            petBattleStatusUpdateRequestSchema->hasResolvedFormat,
            "LKQy resolvedFormat present");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->resolvedFormat ==
                "{petStatus0:i62} {petStatus1:i62} {petStatus2:i62} "
                "{petStatus3:i62} {petStatus4:i62}",
            "LKQy resolvedFormat");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "LKQy resolvedSource");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->hasResolvedFields,
            "LKQy resolvedFields present");
        failures += Expect(
            petBattleStatusUpdateRequestSchema->resolvedFields.size() ==
                resolvedFieldNames.size(),
            "LKQy resolved field count");
        if (petBattleStatusUpdateRequestSchema->resolvedFields.size() ==
            resolvedFieldNames.size()) {
            for (std::size_t index = 0U;
                 index < resolvedFieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    petBattleStatusUpdateRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == resolvedFieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == resolvedSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "LKQy resolved field");
            }
        }
        const cg::packet::send::PetBattleStatusUpdateRequest request(
            std::array<int, 5U> {{0, 1, 2, 3, 16}});
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *petBattleStatusUpdateRequestSchema,
            cg::packet::send::PetBattleStatusUpdateRequest::Spec,
            "LKQy Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petBattleStatusUpdateRequestSchema,
            packet,
            "LKQy resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "LKQy"),
        "LKQy present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "LKQy"),
        "LKQy absent from receive typed manifest");

    const PacketSchema* petMoveRequestSchema = FindPacket(packets, "Ukuy");
    failures += Expect(petMoveRequestSchema != nullptr, "Ukuy schema exists");
    if (petMoveRequestSchema != nullptr) {
        constexpr std::array<std::string_view, 2U> rawFieldNames = {{
            "from",
            "to",
        }};
        constexpr std::array<std::string_view, 2U> rawSpecs = {{
            "{from}",
            "{to}",
        }};
        constexpr std::array<std::string_view, 2U> resolvedFieldNames = {{
            "fromindex",
            "toindex",
        }};
        constexpr std::array<std::string_view, 2U> resolvedSpecs = {{
            "{fromindex:i62}",
            "{toindex:i62}",
        }};

        failures += Expect(
            petMoveRequestSchema->direction == "客户端->服务端",
            "Ukuy direction");
        failures += Expect(
            petMoveRequestSchema->legacyKind == "RECV",
            "Ukuy legacyKind");
        failures += Expect(
            petMoveRequestSchema->clientNamespace == "send",
            "Ukuy namespace");
        failures += Expect(
            petMoveRequestSchema->xlsxStatus == "完成",
            "Ukuy xlsxStatus");
        failures += Expect(
            petMoveRequestSchema->rawFormat == "{from} {to}",
            "Ukuy rawFormat");
        failures += Expect(
            !petMoveRequestSchema->formatEmpty,
            "Ukuy formatEmpty false");
        failures += Expect(
            petMoveRequestSchema->source == "xlsx",
            "Ukuy raw source");
        failures += Expect(
            petMoveRequestSchema->fields.size() == rawFieldNames.size(),
            "Ukuy raw field count");
        if (petMoveRequestSchema->fields.size() == rawFieldNames.size()) {
            for (std::size_t index = 0U;
                 index < rawFieldNames.size();
                 ++index) {
                const FieldSchema& field = petMoveRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == rawFieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "Ukuy raw field");
            }
        }

        failures += Expect(
            petMoveRequestSchema->hasResolvedFormat,
            "Ukuy resolvedFormat present");
        failures += Expect(
            petMoveRequestSchema->resolvedFormat ==
                "{fromindex:i62} {toindex:i62}",
            "Ukuy resolvedFormat");
        failures += Expect(
            petMoveRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "Ukuy resolvedSource");
        failures += Expect(
            petMoveRequestSchema->hasResolvedFields,
            "Ukuy resolvedFields present");
        failures += Expect(
            petMoveRequestSchema->resolvedFields.size() ==
                resolvedFieldNames.size(),
            "Ukuy resolved field count");
        if (petMoveRequestSchema->resolvedFields.size() ==
            resolvedFieldNames.size()) {
            for (std::size_t index = 0U;
                 index < resolvedFieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    petMoveRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == resolvedFieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == resolvedSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "Ukuy resolved field");
            }
        }

        const cg::packet::send::PetMoveRequest request(0, 1);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *petMoveRequestSchema,
            cg::packet::send::PetMoveRequest::Spec,
            "Ukuy Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petMoveRequestSchema,
            packet,
            "Ukuy resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Ukuy"),
        "Ukuy present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Ukuy"),
        "Ukuy absent from receive typed manifest");

    const PacketSchema* skillFrontCharListRequestSchema =
        FindPacket(packets, "iET");
    failures += Expect(
        skillFrontCharListRequestSchema != nullptr,
        "iET schema exists");
    if (skillFrontCharListRequestSchema != nullptr) {
        failures += Expect(
            skillFrontCharListRequestSchema->direction == "客户端->服务端",
            "iET direction");
        failures += Expect(
            skillFrontCharListRequestSchema->legacyKind == "RECV",
            "iET legacyKind");
        failures += Expect(
            skillFrontCharListRequestSchema->clientNamespace == "send",
            "iET namespace");
        failures += Expect(
            skillFrontCharListRequestSchema->xlsxStatus == "完成",
            "iET xlsxStatus");
        failures += Expect(
            skillFrontCharListRequestSchema->rawFormat == "{skill}",
            "iET rawFormat");
        failures += Expect(
            !skillFrontCharListRequestSchema->formatEmpty,
            "iET formatEmpty false");
        failures += Expect(
            skillFrontCharListRequestSchema->source == "xlsx",
            "iET raw source");
        failures += Expect(
            skillFrontCharListRequestSchema->fields.size() == 1U,
            "iET raw field count");
        if (skillFrontCharListRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                skillFrontCharListRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "skill" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{skill}" &&
                    field.source == "xlsx",
                "iET raw field");
        }

        failures += Expect(
            skillFrontCharListRequestSchema->hasResolvedFormat,
            "iET resolvedFormat present");
        failures += Expect(
            skillFrontCharListRequestSchema->resolvedFormat ==
                "{haveskillindex:i62}",
            "iET resolvedFormat");
        failures += Expect(
            skillFrontCharListRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "iET resolvedSource");
        failures += Expect(
            skillFrontCharListRequestSchema->hasResolvedFields,
            "iET resolvedFields present");
        failures += Expect(
            skillFrontCharListRequestSchema->resolvedFields.size() == 1U,
            "iET resolved field count");
        if (skillFrontCharListRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                skillFrontCharListRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "haveskillindex" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{haveskillindex:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "iET resolved field");
        }

        const cg::packet::send::SkillFrontCharListRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *skillFrontCharListRequestSchema,
            cg::packet::send::SkillFrontCharListRequest::Spec,
            "iET Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *skillFrontCharListRequestSchema,
            packet,
            "iET resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "iET"),
        "iET present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "iET"),
        "iET absent from receive typed manifest");

    const PacketSchema* skillCharacterPetListRequestSchema =
        FindPacket(packets, "McVa");
    failures += Expect(
        skillCharacterPetListRequestSchema != nullptr,
        "McVa schema exists");
    if (skillCharacterPetListRequestSchema != nullptr) {
        failures += Expect(
            skillCharacterPetListRequestSchema->direction ==
                "客户端->服务端",
            "McVa direction");
        failures += Expect(
            skillCharacterPetListRequestSchema->legacyKind == "RECV",
            "McVa legacyKind");
        failures += Expect(
            skillCharacterPetListRequestSchema->clientNamespace == "send",
            "McVa namespace");
        failures += Expect(
            skillCharacterPetListRequestSchema->xlsxStatus == "完成",
            "McVa xlsxStatus");
        failures += Expect(
            skillCharacterPetListRequestSchema->rawFormat == "{charSlot}",
            "McVa rawFormat");
        failures += Expect(
            !skillCharacterPetListRequestSchema->formatEmpty,
            "McVa formatEmpty false");
        failures += Expect(
            skillCharacterPetListRequestSchema->source == "xlsx",
            "McVa raw source");
        failures += Expect(
            skillCharacterPetListRequestSchema->fields.size() == 1U,
            "McVa raw field count");
        if (skillCharacterPetListRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                skillCharacterPetListRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "charSlot" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{charSlot}" &&
                    field.source == "xlsx",
                "McVa raw field");
        }

        failures += Expect(
            skillCharacterPetListRequestSchema->hasResolvedFormat,
            "McVa resolvedFormat present");
        failures += Expect(
            skillCharacterPetListRequestSchema->resolvedFormat ==
                "{charSlot:i62}",
            "McVa resolvedFormat");
        failures += Expect(
            skillCharacterPetListRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "McVa resolvedSource");
        failures += Expect(
            skillCharacterPetListRequestSchema->hasResolvedFields,
            "McVa resolvedFields present");
        failures += Expect(
            skillCharacterPetListRequestSchema->resolvedFields.size() == 1U,
            "McVa resolved field count");
        if (skillCharacterPetListRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                skillCharacterPetListRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "charSlot" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{charSlot:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "McVa resolved field");
        }

        const cg::packet::send::SkillCharacterPetListRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *skillCharacterPetListRequestSchema,
            cg::packet::send::SkillCharacterPetListRequest::Spec,
            "McVa Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *skillCharacterPetListRequestSchema,
            packet,
            "McVa resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "McVa"),
        "McVa present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "McVa"),
        "McVa absent from receive typed manifest");

    const PacketSchema* itemCharacterPetListRequestSchema =
        FindPacket(packets, "mjCv");
    failures += Expect(
        itemCharacterPetListRequestSchema != nullptr,
        "mjCv schema exists");
    if (itemCharacterPetListRequestSchema != nullptr) {
        failures += Expect(
            itemCharacterPetListRequestSchema->direction ==
                "客户端->服务端",
            "mjCv direction");
        failures += Expect(
            itemCharacterPetListRequestSchema->legacyKind == "RECV",
            "mjCv legacyKind");
        failures += Expect(
            itemCharacterPetListRequestSchema->clientNamespace == "send",
            "mjCv namespace");
        failures += Expect(
            itemCharacterPetListRequestSchema->xlsxStatus == "完成",
            "mjCv xlsxStatus");
        failures += Expect(
            itemCharacterPetListRequestSchema->rawFormat == "{charSlot}",
            "mjCv rawFormat");
        failures += Expect(
            !itemCharacterPetListRequestSchema->formatEmpty,
            "mjCv formatEmpty false");
        failures += Expect(
            itemCharacterPetListRequestSchema->source == "xlsx",
            "mjCv raw source");
        failures += Expect(
            itemCharacterPetListRequestSchema->fields.size() == 1U,
            "mjCv raw field count");
        if (itemCharacterPetListRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                itemCharacterPetListRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "charSlot" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{charSlot}" &&
                    field.source == "xlsx",
                "mjCv raw field");
        }

        failures += Expect(
            itemCharacterPetListRequestSchema->hasResolvedFormat,
            "mjCv resolvedFormat present");
        failures += Expect(
            itemCharacterPetListRequestSchema->resolvedFormat ==
                "{charSlot:i62}",
            "mjCv resolvedFormat");
        failures += Expect(
            itemCharacterPetListRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "mjCv resolvedSource");
        failures += Expect(
            itemCharacterPetListRequestSchema->hasResolvedFields,
            "mjCv resolvedFields present");
        failures += Expect(
            itemCharacterPetListRequestSchema->resolvedFields.size() == 1U,
            "mjCv resolved field count");
        if (itemCharacterPetListRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                itemCharacterPetListRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "charSlot" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{charSlot:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "mjCv resolved field");
        }

        const cg::packet::send::ItemCharacterPetListRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *itemCharacterPetListRequestSchema,
            cg::packet::send::ItemCharacterPetListRequest::Spec,
            "mjCv Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *itemCharacterPetListRequestSchema,
            packet,
            "mjCv resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "mjCv"),
        "mjCv present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "mjCv"),
        "mjCv absent from receive typed manifest");

    const PacketSchema* itemStampRequestSchema = FindPacket(packets, "BG");
    failures += Expect(itemStampRequestSchema != nullptr, "BG schema exists");
    if (itemStampRequestSchema != nullptr) {
        constexpr std::array<std::string_view, 2U> rawFieldNames = {{
            "toslot",
            "fromslot",
        }};
        constexpr std::array<std::string_view, 2U> rawSpecs = {{
            "{toslot}",
            "{fromslot}",
        }};
        constexpr std::array<std::string_view, 2U> resolvedFieldNames = {{
            "targetItemSlot",
            "stampItemSlot",
        }};
        constexpr std::array<std::string_view, 2U> resolvedSpecs = {{
            "{targetItemSlot:i62}",
            "{stampItemSlot:i62}",
        }};

        failures += Expect(
            itemStampRequestSchema->direction == "客户端->服务端",
            "BG direction");
        failures += Expect(
            itemStampRequestSchema->legacyKind == "RECV",
            "BG legacyKind");
        failures += Expect(
            itemStampRequestSchema->clientNamespace == "send",
            "BG namespace");
        failures += Expect(
            itemStampRequestSchema->xlsxStatus == "完成",
            "BG xlsxStatus");
        failures += Expect(
            itemStampRequestSchema->rawFormat == "{toslot} {fromslot}",
            "BG rawFormat");
        failures += Expect(
            !itemStampRequestSchema->formatEmpty,
            "BG formatEmpty false");
        failures += Expect(
            itemStampRequestSchema->source == "xlsx",
            "BG raw source");
        failures += Expect(
            itemStampRequestSchema->fields.size() == rawFieldNames.size(),
            "BG raw field count");
        if (itemStampRequestSchema->fields.size() == rawFieldNames.size()) {
            for (std::size_t index = 0U;
                 index < rawFieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    itemStampRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == rawFieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "BG raw field");
            }
        }

        failures += Expect(
            itemStampRequestSchema->hasResolvedFormat,
            "BG resolvedFormat present");
        failures += Expect(
            itemStampRequestSchema->resolvedFormat ==
                "{targetItemSlot:i62} {stampItemSlot:i62}",
            "BG resolvedFormat");
        failures += Expect(
            itemStampRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "BG resolvedSource");
        failures += Expect(
            itemStampRequestSchema->hasResolvedFields,
            "BG resolvedFields present");
        failures += Expect(
            itemStampRequestSchema->resolvedFields.size() ==
                resolvedFieldNames.size(),
            "BG resolved field count");
        if (itemStampRequestSchema->resolvedFields.size() ==
            resolvedFieldNames.size()) {
            for (std::size_t index = 0U;
                 index < resolvedFieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    itemStampRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == resolvedFieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == resolvedSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "BG resolved field");
            }
        }

        const cg::packet::send::ItemStampRequest request(10, 20);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *itemStampRequestSchema,
            cg::packet::send::ItemStampRequest::Spec,
            "BG Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *itemStampRequestSchema,
            packet,
            "BG resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "BG"),
        "BG present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "BG"),
        "BG absent from receive typed manifest");

    const PacketSchema* characterActionRequestSchema =
        FindPacket(packets, "MIS");
    failures += Expect(
        characterActionRequestSchema != nullptr,
        "MIS schema exists");
    if (characterActionRequestSchema != nullptr) {
        constexpr std::array<std::string_view, 3U> fieldNames = {{
            "x",
            "y",
            "action",
        }};
        constexpr std::array<std::string_view, 3U> rawSpecs = {{
            "{x}",
            "{y}",
            "{action}",
        }};
        constexpr std::array<std::string_view, 3U> resolvedSpecs = {{
            "{x:i62}",
            "{y:i62}",
            "{action:i62}",
        }};

        failures += Expect(
            characterActionRequestSchema->direction == "客户端->服务端",
            "MIS direction");
        failures += Expect(
            characterActionRequestSchema->legacyKind == "RECV",
            "MIS legacyKind");
        failures += Expect(
            characterActionRequestSchema->clientNamespace == "send",
            "MIS namespace");
        failures += Expect(
            characterActionRequestSchema->xlsxStatus == "完成",
            "MIS xlsxStatus");
        failures += Expect(
            characterActionRequestSchema->rawFormat == "{x} {y} {action}",
            "MIS rawFormat");
        failures += Expect(
            !characterActionRequestSchema->formatEmpty,
            "MIS formatEmpty false");
        failures += Expect(
            characterActionRequestSchema->source == "xlsx",
            "MIS raw source");
        failures += Expect(
            characterActionRequestSchema->fields.size() == fieldNames.size(),
            "MIS raw field count");
        if (characterActionRequestSchema->fields.size() ==
            fieldNames.size()) {
            for (std::size_t index = 0U;
                 index < fieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    characterActionRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "MIS raw field");
            }
        }

        failures += Expect(
            characterActionRequestSchema->hasResolvedFormat,
            "MIS resolvedFormat present");
        failures += Expect(
            characterActionRequestSchema->resolvedFormat ==
                "{x:i62} {y:i62} {action:i62}",
            "MIS resolvedFormat");
        failures += Expect(
            characterActionRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "MIS resolvedSource");
        failures += Expect(
            characterActionRequestSchema->hasResolvedFields,
            "MIS resolvedFields present");
        failures += Expect(
            characterActionRequestSchema->resolvedFields.size() ==
                fieldNames.size(),
            "MIS resolved field count");
        if (characterActionRequestSchema->resolvedFields.size() ==
            fieldNames.size()) {
            for (std::size_t index = 0U;
                 index < fieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    characterActionRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == resolvedSpecs[index] &&
                        field.source ==
                            "xlsx+ServerC+ClientIda+src/packet",
                    "MIS resolved field");
            }
        }

        const cg::packet::send::CharacterActionRequest request(10, 11, 12);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *characterActionRequestSchema,
            cg::packet::send::CharacterActionRequest::Spec,
            "MIS Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterActionRequestSchema,
            packet,
            "MIS resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "MIS"),
        "MIS present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "MIS"),
        "MIS absent from receive typed manifest");

    const PacketSchema* actionSkillIconRequestSchema =
        FindPacket(packets, "klF");
    failures += Expect(
        actionSkillIconRequestSchema != nullptr,
        "klF schema exists");
    if (actionSkillIconRequestSchema != nullptr) {
        const cg::packet::send::ActionSkillIconRequest request(10, 11, 12);
        const cg::packet::Packet packet = request.ToPacket();
        constexpr std::array<std::string_view, 3U> fieldNames = {{
            "x",
            "y",
            "icon",
        }};
        constexpr std::array<std::string_view, 3U> rawSpecs = {{
            "{x}",
            "{y}",
            "{icon}",
        }};
        constexpr std::array<std::string_view, 3U> resolvedSpecs = {{
            "{x:i62}",
            "{y:i62}",
            "{icon:i62}",
        }};

        failures += Expect(
            actionSkillIconRequestSchema->direction == "客户端->服务端",
            "klF direction");
        failures += Expect(
            actionSkillIconRequestSchema->legacyKind == "RECV",
            "klF legacyKind");
        failures += Expect(
            actionSkillIconRequestSchema->clientNamespace == "send",
            "klF namespace");
        failures += Expect(
            actionSkillIconRequestSchema->xlsxStatus == "完成",
            "klF xlsxStatus");
        failures += Expect(
            actionSkillIconRequestSchema->rawFormat == "{x} {y} {icon}",
            "klF rawFormat");
        failures += Expect(
            !actionSkillIconRequestSchema->formatEmpty,
            "klF formatEmpty false");
        failures += Expect(
            actionSkillIconRequestSchema->source == "xlsx",
            "klF raw source");
        failures += Expect(
            actionSkillIconRequestSchema->fields.size() == fieldNames.size(),
            "klF raw field count");
        if (actionSkillIconRequestSchema->fields.size() ==
            fieldNames.size()) {
            for (std::size_t index = 0U;
                 index < fieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    actionSkillIconRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "klF raw field");
            }
        }

        failures += Expect(
            actionSkillIconRequestSchema->hasResolvedFormat,
            "klF resolvedFormat present");
        failures += Expect(
            actionSkillIconRequestSchema->resolvedFormat ==
                "{x:i62} {y:i62} {icon:i62}",
            "klF resolvedFormat");
        failures += Expect(
            actionSkillIconRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "klF resolvedSource");
        failures += Expect(
            actionSkillIconRequestSchema->hasResolvedFields,
            "klF resolvedFields present");
        failures += Expect(
            actionSkillIconRequestSchema->resolvedFields.size() ==
                fieldNames.size(),
            "klF resolved field count");
        if (actionSkillIconRequestSchema->resolvedFields.size() ==
            fieldNames.size()) {
            for (std::size_t index = 0U;
                 index < fieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    actionSkillIconRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == fieldNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == resolvedSpecs[index] &&
                        field.source ==
                            "xlsx+ServerC+ClientIda+src/packet",
                    "klF resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *actionSkillIconRequestSchema,
            cg::packet::send::ActionSkillIconRequest::Spec,
            "klF Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *actionSkillIconRequestSchema,
            packet,
            "klF resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "klF"),
        "klF present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "klF"),
        "klF absent from receive typed manifest");

    const PacketSchema* techUseRequestSchema = FindPacket(packets, "sM");
    failures += Expect(techUseRequestSchema != nullptr, "sM schema exists");
    if (techUseRequestSchema != nullptr) {
        constexpr std::array<std::string_view, 4U> rawFieldNames = {{
            "skill",
            "tech",
            "to",
            "receipe",
        }};
        constexpr std::array<std::string_view, 4U> rawFieldSpecs = {{
            "{skill}",
            "{tech}",
            "{to}",
            "{receipe}",
        }};
        constexpr std::array<std::string_view, 4U> resolvedFieldNames = {{
            "haveSkillIndex",
            "haveTechIndex",
            "toIndex",
            "data",
        }};
        constexpr std::array<std::string_view, 4U> resolvedFieldTypes = {{
            "i62",
            "i62",
            "i62",
            "es",
        }};
        constexpr std::array<std::string_view, 4U> resolvedFieldSpecs = {{
            "{haveSkillIndex:i62}",
            "{haveTechIndex:i62}",
            "{toIndex:i62}",
            "{data:es}",
        }};

        failures += Expect(
            techUseRequestSchema->direction == "客户端->服务端",
            "sM direction");
        failures += Expect(
            techUseRequestSchema->legacyKind == "RECV",
            "sM legacyKind");
        failures += Expect(
            techUseRequestSchema->clientNamespace == "send",
            "sM namespace");
        failures += Expect(
            techUseRequestSchema->xlsxStatus == "完成",
            "sM xlsxStatus");
        failures += Expect(
            techUseRequestSchema->rawFormat ==
                "{skill} {tech} {to} {receipe}",
            "sM rawFormat");
        failures += Expect(
            !techUseRequestSchema->formatEmpty,
            "sM formatEmpty false");
        failures += Expect(
            techUseRequestSchema->source == "xlsx",
            "sM raw source");
        failures += Expect(
            techUseRequestSchema->fields.size() == rawFieldNames.size(),
            "sM raw field count");
        if (techUseRequestSchema->fields.size() ==
            rawFieldNames.size()) {
            for (std::size_t index = 0U;
                 index < rawFieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    techUseRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == rawFieldNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawFieldSpecs[index] &&
                        field.source == "xlsx",
                    "sM raw field");
            }
        }

        failures += Expect(
            techUseRequestSchema->hasResolvedFormat,
            "sM resolvedFormat present");
        failures += Expect(
            techUseRequestSchema->resolvedFormat ==
                "{haveSkillIndex:i62} {haveTechIndex:i62} "
                "{toIndex:i62} {data:es}",
            "sM resolvedFormat");
        failures += Expect(
            techUseRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda",
            "sM resolvedSource");
        failures += Expect(
            techUseRequestSchema->hasResolvedFields,
            "sM resolvedFields present");
        failures += Expect(
            techUseRequestSchema->resolvedFields.size() ==
                resolvedFieldNames.size(),
            "sM resolved field count");
        if (techUseRequestSchema->resolvedFields.size() ==
            resolvedFieldNames.size()) {
            for (std::size_t index = 0U;
                 index < resolvedFieldNames.size();
                 ++index) {
                const FieldSchema& field =
                    techUseRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == resolvedFieldNames[index] &&
                        field.typeToken == resolvedFieldTypes[index] &&
                        field.rawSpec == resolvedFieldSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda",
                    "sM resolved field");
            }
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "sM"),
        "sM absent from send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "sM"),
        "sM absent from receive typed manifest");

    const PacketSchema* deprecatedBattleJoinSchema = FindPacket(packets, "cEj");
    failures += Expect(deprecatedBattleJoinSchema != nullptr, "cEj schema exists");
    if (deprecatedBattleJoinSchema != nullptr) {
        const cg::packet::send::BattleJoinRequest request(10, 20);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            deprecatedBattleJoinSchema->direction == "客户端->服务端",
            "cEj direction");
        failures += Expect(
            deprecatedBattleJoinSchema->legacyKind == "RECV",
            "cEj legacyKind");
        failures += Expect(
            deprecatedBattleJoinSchema->rawFormat == "{x} {y}",
            "cEj rawFormat");
        failures += Expect(
            !deprecatedBattleJoinSchema->formatEmpty,
            "cEj formatEmpty false");
        failures += Expect(deprecatedBattleJoinSchema->source == "xlsx", "cEj raw source");
        failures += Expect(
            deprecatedBattleJoinSchema->fields.size() == 2U,
            "cEj raw field count");
        if (deprecatedBattleJoinSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"x", "y"}};
            constexpr std::array<std::string_view, 2U> specs = {{"{x}", "{y}"}};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = deprecatedBattleJoinSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "cEj raw field");
            }
        }
        failures += Expect(
            deprecatedBattleJoinSchema->hasResolvedFormat,
            "cEj resolvedFormat present");
        failures += Expect(
            deprecatedBattleJoinSchema->resolvedFormat == "{x:i62} {y:i62}",
            "cEj resolvedFormat");
        failures += Expect(
            deprecatedBattleJoinSchema->resolvedSource == "xlsx+ServerC+src/packet",
            "cEj resolvedSource");
        failures += Expect(
            deprecatedBattleJoinSchema->resolvedSource.find("ClientIda") == std::string::npos,
            "cEj resolvedSource excludes ClientIda");
        failures += Expect(
            deprecatedBattleJoinSchema->hasResolvedFields,
            "cEj resolvedFields present");
        failures += Expect(
            deprecatedBattleJoinSchema->resolvedFields.size() == 2U,
            "cEj resolved field count");
        if (deprecatedBattleJoinSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"x", "y"}};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{x:i62}",
                "{y:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = deprecatedBattleJoinSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+src/packet",
                    "cEj resolved field");
                failures += Expect(
                    field.source.find("ClientIda") == std::string::npos,
                    "cEj resolved field source excludes ClientIda");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *deprecatedBattleJoinSchema,
            cg::packet::send::BattleJoinRequest::Spec,
            "cEj Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *deprecatedBattleJoinSchema,
            packet,
            "cEj resolved field order");
    }

    const PacketSchema* battleWatchRequestSchema = FindPacket(packets, "IMv");
    failures += Expect(battleWatchRequestSchema != nullptr, "IMv schema exists");
    if (battleWatchRequestSchema != nullptr) {
        const cg::packet::send::BattleWatchRequest request(10, 20);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            battleWatchRequestSchema->direction == "客户端->服务端",
            "IMv direction");
        failures += Expect(
            battleWatchRequestSchema->legacyKind == "RECV",
            "IMv legacyKind");
        failures += Expect(
            battleWatchRequestSchema->rawFormat == "{x} {y}",
            "IMv rawFormat");
        failures += Expect(!battleWatchRequestSchema->formatEmpty, "IMv formatEmpty false");
        failures += Expect(battleWatchRequestSchema->source == "xlsx", "IMv raw source");
        failures += Expect(
            battleWatchRequestSchema->fields.size() == 2U,
            "IMv raw field count");
        if (battleWatchRequestSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"x", "y"}};
            constexpr std::array<std::string_view, 2U> specs = {{"{x}", "{y}"}};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = battleWatchRequestSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "IMv raw field");
            }
        }
        failures += Expect(
            battleWatchRequestSchema->hasResolvedFormat,
            "IMv resolvedFormat present");
        failures += Expect(
            battleWatchRequestSchema->resolvedFormat == "{x:i62} {y:i62}",
            "IMv resolvedFormat");
        failures += Expect(
            battleWatchRequestSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "IMv resolvedSource");
        failures += Expect(
            battleWatchRequestSchema->hasResolvedFields,
            "IMv resolvedFields present");
        failures += Expect(
            battleWatchRequestSchema->resolvedFields.size() == 2U,
            "IMv resolved field count");
        if (battleWatchRequestSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"x", "y"}};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{x:i62}",
                "{y:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = battleWatchRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "IMv resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *battleWatchRequestSchema,
            cg::packet::send::BattleWatchRequest::Spec,
            "IMv Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *battleWatchRequestSchema,
            packet,
            "IMv resolved field order");
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
        const cg::packet::send::MailMessageSend request(10, "message", 0);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(mailMessageSendSchema->direction == "客户端->服务端", "CV direction");
        failures += Expect(mailMessageSendSchema->legacyKind == "RECV", "CV legacyKind");
        failures += Expect(mailMessageSendSchema->clientNamespace == "send", "CV namespace");
        failures += Expect(
            mailMessageSendSchema->rawFormat == "{index} {message:es} {color}",
            "CV rawFormat");
        failures += Expect(!mailMessageSendSchema->formatEmpty, "CV formatEmpty false");
        failures += Expect(mailMessageSendSchema->source == "xlsx", "CV raw source");
        failures += Expect(
            mailMessageSendSchema->resolvedFormat == "{index:i62} {message:es} {color:i62}",
            "CV resolvedFormat");
        failures += Expect(
            mailMessageSendSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "CV resolvedSource");
        failures += Expect(mailMessageSendSchema->fields.size() == 3U, "CV raw field count");
        if (mailMessageSendSchema->fields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{
                "index",
                "message",
                "color",
            }};
            constexpr std::array<std::string_view, 3U> types = {{
                "unknown",
                "es",
                "unknown",
            }};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{index}",
                "{message:es}",
                "{color}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = mailMessageSendSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "CV raw field");
            }
        }

        failures += Expect(
            mailMessageSendSchema->hasResolvedFields,
            "CV resolvedFields present");
        failures += Expect(
            mailMessageSendSchema->resolvedFields.size() == 3U,
            "CV resolved field count");
        if (mailMessageSendSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{
                "index",
                "message",
                "color",
            }};
            constexpr std::array<std::string_view, 3U> types = {{
                "i62",
                "es",
                "i62",
            }};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{index:i62}",
                "{message:es}",
                "{color:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = mailMessageSendSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "CV resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *mailMessageSendSchema,
            cg::packet::send::MailMessageSend::Spec,
            "CV Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *mailMessageSendSchema,
            packet,
            "CV resolved field order");
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
        failures += Expect(dropGoldSchema->clientNamespace == "send", "woz namespace");
        failures += Expect(dropGoldSchema->xlsxStatus == "完成", "woz xlsxStatus");
        failures += Expect(dropGoldSchema->rawFormat == "{x} {y} {amount}", "woz rawFormat");
        failures += Expect(!dropGoldSchema->formatEmpty, "woz formatEmpty false");
        failures += Expect(dropGoldSchema->source == "xlsx", "woz raw source");
        failures += Expect(dropGoldSchema->fields.size() == 3U, "woz raw field count");
        if (dropGoldSchema->fields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{"x", "y", "amount"}};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{x}",
                "{y}",
                "{amount}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = dropGoldSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "woz raw field");
            }
        }

        failures += Expect(dropGoldSchema->hasResolvedFormat, "woz resolvedFormat present");
        failures += Expect(
            dropGoldSchema->resolvedFormat == "{x:i62} {y:i62} {amount:i62}",
            "woz resolvedFormat");
        failures += Expect(
            dropGoldSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "woz resolvedSource");
        failures += Expect(dropGoldSchema->hasResolvedFields, "woz resolvedFields present");
        failures += Expect(dropGoldSchema->resolvedFields.size() == 3U, "woz resolved field count");
        if (dropGoldSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{"x", "y", "amount"}};
            constexpr std::array<std::string_view, 3U> specs = {{
                "{x:i62}",
                "{y:i62}",
                "{amount:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = dropGoldSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "woz resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *dropGoldSchema,
            cg::packet::send::DropGold::Spec,
            "woz Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *dropGoldSchema,
            packet,
            "woz resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "woz"),
        "woz present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "woz"),
        "woz absent from receive typed manifest");

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
        failures += Expect(guildInfoChangeSchema->clientNamespace == "send", "Ufm namespace");
        failures += Expect(guildInfoChangeSchema->xlsxStatus == "进行中", "Ufm xlsxStatus");
        failures += Expect(guildInfoChangeSchema->rawFormat == "{c} {data:es}", "Ufm rawFormat");
        failures += Expect(!guildInfoChangeSchema->formatEmpty, "Ufm formatEmpty false");
        failures += Expect(guildInfoChangeSchema->source == "xlsx", "Ufm raw source");
        failures += Expect(guildInfoChangeSchema->fields.size() == 2U, "Ufm raw field count");
        if (guildInfoChangeSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"c", "data"}};
            constexpr std::array<std::string_view, 2U> types = {{"unknown", "es"}};
            constexpr std::array<std::string_view, 2U> specs = {{"{c}", "{data:es}"}};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = guildInfoChangeSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "Ufm raw field");
            }
        }

        failures += Expect(
            guildInfoChangeSchema->hasResolvedFormat,
            "Ufm resolvedFormat present");
        failures += Expect(
            guildInfoChangeSchema->resolvedFormat == "{c:i62} {data:es}",
            "Ufm resolvedFormat");
        failures += Expect(
            guildInfoChangeSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "Ufm resolvedSource");
        failures += Expect(
            guildInfoChangeSchema->hasResolvedFields,
            "Ufm resolvedFields present");
        failures += Expect(
            guildInfoChangeSchema->resolvedFields.size() == 2U,
            "Ufm resolved field count");
        if (guildInfoChangeSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"c", "data"}};
            constexpr std::array<std::string_view, 2U> types = {{"i62", "es"}};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{c:i62}",
                "{data:es}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = guildInfoChangeSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "Ufm resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *guildInfoChangeSchema,
            cg::packet::send::GuildInfoChange::Spec,
            "Ufm Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *guildInfoChangeSchema,
            packet,
            "Ufm resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "Ufm"),
        "Ufm present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "Ufm"),
        "Ufm absent from receive typed manifest");

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
        const cg::packet::send::PetMailSend request(1, 2, 3, "message", 0);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(petMailSendSchema->direction == "客户端->服务端", "rMsv direction");
        failures += Expect(petMailSendSchema->legacyKind == "RECV", "rMsv legacyKind");
        failures += Expect(petMailSendSchema->clientNamespace == "send", "rMsv namespace");
        failures += Expect(
            petMailSendSchema->rawFormat ==
                "{index} {petindex} {itemindex} {message} {color}",
            "rMsv rawFormat");
        failures += Expect(!petMailSendSchema->formatEmpty, "rMsv formatEmpty false");
        failures += Expect(petMailSendSchema->source == "xlsx", "rMsv raw source");
        failures += Expect(
            petMailSendSchema->resolvedFormat == "{index:i62} {petindex:i62} {itemindex:i62} {message:es} {color:i62}",
            "rMsv resolvedFormat");
        failures += Expect(
            petMailSendSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "rMsv resolvedSource");
        failures += Expect(petMailSendSchema->fields.size() == 5U, "rMsv raw field count");
        if (petMailSendSchema->fields.size() == 5U) {
            constexpr std::array<std::string_view, 5U> names = {{
                "index",
                "petindex",
                "itemindex",
                "message",
                "color",
            }};
            constexpr std::array<std::string_view, 5U> specs = {{
                "{index}",
                "{petindex}",
                "{itemindex}",
                "{message}",
                "{color}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = petMailSendSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "rMsv raw field");
            }
        }

        failures += Expect(
            petMailSendSchema->hasResolvedFields,
            "rMsv resolvedFields present");
        failures += Expect(
            petMailSendSchema->resolvedFields.size() == 5U,
            "rMsv resolved field count");
        if (petMailSendSchema->resolvedFields.size() == 5U) {
            constexpr std::array<std::string_view, 5U> names = {{
                "index",
                "petindex",
                "itemindex",
                "message",
                "color",
            }};
            constexpr std::array<std::string_view, 5U> types = {{
                "i62",
                "i62",
                "i62",
                "es",
                "i62",
            }};
            constexpr std::array<std::string_view, 5U> specs = {{
                "{index:i62}",
                "{petindex:i62}",
                "{itemindex:i62}",
                "{message:es}",
                "{color:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = petMailSendSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "rMsv resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *petMailSendSchema,
            cg::packet::send::PetMailSend::Spec,
            "rMsv Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petMailSendSchema,
            packet,
            "rMsv resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "rMsv"),
        "rMsv present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "rMsv"),
        "rMsv absent from receive typed manifest");

    const PacketSchema* petMailRegisterSchema = FindPacket(packets, "PfqV");
    failures += Expect(petMailRegisterSchema != nullptr, "PfqV schema exists");
    if (petMailRegisterSchema != nullptr) {
        const cg::packet::send::PetMailRegister request(1, 2, 3, "text", 0);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(petMailRegisterSchema->direction == "客户端->服务端", "PfqV direction");
        failures += Expect(petMailRegisterSchema->legacyKind == "RECV", "PfqV legacyKind");
        failures += Expect(petMailRegisterSchema->clientNamespace == "send", "PfqV namespace");
        failures += Expect(petMailRegisterSchema->xlsxStatus == "进行中", "PfqV xlsxStatus");
        failures += Expect(
            petMailRegisterSchema->rawFormat ==
                "{toregist} {petSlot} {itemSlot} {text} {color}",
            "PfqV rawFormat");
        failures += Expect(!petMailRegisterSchema->formatEmpty, "PfqV formatEmpty false");
        failures += Expect(petMailRegisterSchema->source == "xlsx", "PfqV raw source");
        failures += Expect(
            petMailRegisterSchema->resolvedFormat ==
                "{toregist:i62} {petSlot:i62} {itemSlot:i62} {text:es} {color:i62}",
            "PfqV resolvedFormat");
        failures += Expect(
            petMailRegisterSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "PfqV resolvedSource");
        failures += Expect(petMailRegisterSchema->fields.size() == 5U, "PfqV raw field count");
        if (petMailRegisterSchema->fields.size() == 5U) {
            constexpr std::array<std::string_view, 5U> names = {{
                "toregist",
                "petSlot",
                "itemSlot",
                "text",
                "color",
            }};
            constexpr std::array<std::string_view, 5U> specs = {{
                "{toregist}",
                "{petSlot}",
                "{itemSlot}",
                "{text}",
                "{color}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = petMailRegisterSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "PfqV raw field");
            }
        }

        failures += Expect(
            petMailRegisterSchema->hasResolvedFields,
            "PfqV resolvedFields present");
        failures += Expect(
            petMailRegisterSchema->resolvedFields.size() == 5U,
            "PfqV resolved field count");
        if (petMailRegisterSchema->resolvedFields.size() == 5U) {
            constexpr std::array<std::string_view, 5U> names = {{
                "toregist",
                "petSlot",
                "itemSlot",
                "text",
                "color",
            }};
            constexpr std::array<std::string_view, 5U> types = {{
                "i62",
                "i62",
                "i62",
                "es",
                "i62",
            }};
            constexpr std::array<std::string_view, 5U> specs = {{
                "{toregist:i62}",
                "{petSlot:i62}",
                "{itemSlot:i62}",
                "{text:es}",
                "{color:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = petMailRegisterSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "PfqV resolved field");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *petMailRegisterSchema,
            cg::packet::send::PetMailRegister::Spec,
            "PfqV Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petMailRegisterSchema,
            packet,
            "PfqV resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "PfqV"),
        "PfqV present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "PfqV"),
        "PfqV absent from receive typed manifest");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "usage: cg-packet-coverage-schema-test <schema-json> <fixture-root>\n";
        return 1;
    }

    const std::string jsonText = ReadTextFile(argv[1]);
    int failures = TestFixtureParserGrammar();
    failures += TestFixtureRoundtripMemory();
    failures += Expect(!jsonText.empty(), "schema file non-empty");
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
    failures += TestTypedManifest(packets);
    failures += TestFixtureInventory(argv[2]);
    failures += TestKnownPackets(packets);
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
