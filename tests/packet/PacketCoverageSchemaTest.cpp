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
    bool hasContractStatus = false;
    std::string contractStatus;
    bool hasExecutionStatus = false;
    std::string executionStatus;
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
    packet.hasContractStatus = object.Find("contractStatus") != nullptr;
    packet.contractStatus = OptionalString(object, "contractStatus", failures);
    packet.hasExecutionStatus = object.Find("executionStatus") != nullptr;
    packet.executionStatus = OptionalString(object, "executionStatus", failures);
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
    std::set<std::string> terminalCodes;
    std::set<std::string> userSkippedCodes;

    for (const PacketSchema& packet : packets) {
        failures += Expect(!packet.code.empty(), "code non-empty");
        failures += Expect(codes.insert(packet.code).second, "code unique");
        failures += Expect(packet.source == "xlsx", "packet source xlsx");
        failures += Expect(
            !(packet.hasContractStatus && packet.hasExecutionStatus),
            "packet terminal and execution status mutually exclusive");
        if (packet.hasContractStatus) {
            failures += Expect(
                packet.contractStatus == "no-effective-wire-contract",
                "contractStatus known");
            terminalCodes.insert(packet.code);
        } else {
            failures += Expect(packet.contractStatus.empty(), "contractStatus absent is empty");
        }
        if (packet.hasExecutionStatus) {
            failures += Expect(
                packet.executionStatus == "user-skipped",
                "executionStatus known");
            userSkippedCodes.insert(packet.code);
        } else {
            failures += Expect(packet.executionStatus.empty(), "executionStatus absent is empty");
        }

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
    failures += Expect(
        terminalCodes == std::set<std::string>({"Expire", "sz"}),
        "terminal codes exactly Expire and sz");
    failures += Expect(
        userSkippedCodes == std::set<std::string>({"yp"}),
        "user-skipped code exactly yp");
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
    failures += Expect(cg::packet::test::SendTypedPackets.size() == 98U, "send typed manifest count 98");
    failures += Expect(cg::packet::test::ReceiveTypedPackets.size() == 108U, "receive typed manifest count 108");
    failures += Expect(
        cg::packet::test::SendTypedPackets.size() + cg::packet::test::ReceiveTypedPackets.size() == 206U,
        "typed manifest count 206");

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
    failures += Expect(codes.size() == 206U, "typed manifest unique code count 206");
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
        failures += Expect(clientLoginSchema->direction == "客户端->服务端", "JFVf direction");
        failures += Expect(clientLoginSchema->legacyKind == "RECV", "JFVf legacyKind");
        failures += Expect(
            clientLoginSchema->rawFormat == "{clientHash:es} {password:es} {account:es}",
            "JFVf rawFormat preserved");
        failures += Expect(clientLoginSchema->fields.size() == 3U, "JFVf raw field count");
        if (clientLoginSchema->fields.size() == 3U) {
            failures += Expect(
                clientLoginSchema->fields[0].name == "clientHash"
                    && clientLoginSchema->fields[0].typeToken == "es"
                    && clientLoginSchema->fields[0].source == "xlsx",
                "JFVf raw clientHash preserved");
            failures += Expect(
                clientLoginSchema->fields[1].name == "password"
                    && clientLoginSchema->fields[1].typeToken == "es"
                    && clientLoginSchema->fields[1].source == "xlsx",
                "JFVf raw password preserved");
            failures += Expect(
                clientLoginSchema->fields[2].name == "account"
                    && clientLoginSchema->fields[2].typeToken == "es"
                    && clientLoginSchema->fields[2].source == "xlsx",
                "JFVf raw account preserved");
        }
        failures += Expect(clientLoginSchema->hasResolvedFormat, "JFVf resolvedFormat present");
        failures += Expect(
            clientLoginSchema->resolvedFormat
                == "{clientHash:es} {account:es} {password:es} {macAddress:es}",
            "JFVf resolvedFormat");
        failures += Expect(
            clientLoginSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "JFVf resolvedSource");
        failures += Expect(clientLoginSchema->hasResolvedFields, "JFVf resolvedFields present");
        failures += Expect(
            clientLoginSchema->resolvedFields.size() == 4U,
            "JFVf resolved field count");
        for (const FieldSchema& field : clientLoginSchema->resolvedFields) {
            failures += Expect(
                field.source == "ServerC+ClientIda+src/packet",
                "JFVf resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *clientLoginSchema,
            cg::packet::send::ClientLogin::Spec,
            "JFVf effective field order");
    }

    const PacketSchema* characterListRequestSchema = FindPacket(packets, "Gp");
    failures += Expect(characterListRequestSchema != nullptr, "Gp schema exists");
    if (characterListRequestSchema != nullptr) {
        const cg::packet::Packet packet = cg::packet::send::CharacterListRequest {}.ToPacket();
        failures += Expect(
            characterListRequestSchema->direction == "客户端->服务端",
            "Gp direction");
        failures += Expect(characterListRequestSchema->legacyKind == "RECV", "Gp legacyKind");
        failures += Expect(characterListRequestSchema->clientNamespace == "send", "Gp namespace");
        failures += Expect(characterListRequestSchema->xlsxStatus == "完成", "Gp xlsxStatus");
        failures += Expect(
            characterListRequestSchema->description == "RECV_CharList 获取角色列表",
            "Gp description");
        failures += Expect(characterListRequestSchema->rawFormat.empty(), "Gp rawFormat empty");
        failures += Expect(characterListRequestSchema->originalFormat == "Gp", "Gp originalFormat");
        failures += Expect(!characterListRequestSchema->formatEmpty, "Gp formatEmpty false");
        failures += Expect(characterListRequestSchema->source == "xlsx", "Gp raw source");
        failures += Expect(characterListRequestSchema->fields.empty(), "Gp raw fields empty");
        failures += Expect(characterListRequestSchema->hasResolvedFormat, "Gp resolvedFormat present");
        failures += Expect(characterListRequestSchema->resolvedFormat.empty(), "Gp resolvedFormat empty");
        failures += Expect(
            characterListRequestSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "Gp resolvedSource");
        failures += Expect(characterListRequestSchema->hasResolvedFields, "Gp resolvedFields present");
        failures += Expect(characterListRequestSchema->resolvedFields.empty(), "Gp resolvedFields empty");
        failures += ExpectEffectiveFieldsMatch(
            *characterListRequestSchema,
            cg::packet::send::CharacterListRequest::Spec,
            "Gp Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterListRequestSchema,
            packet,
            "Gp resolved field order");
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
        const cg::packet::Packet packet = loginResult.ToPacket();
        failures += Expect(loginResultSchema->direction == "服务端->客户端", "whV direction");
        failures += Expect(loginResultSchema->legacyKind == "SEND", "whV legacyKind");
        failures += Expect(loginResultSchema->clientNamespace == "receive", "whV namespace");
        failures += Expect(loginResultSchema->rawFormat == "{result:i62} {data:es}", "whV rawFormat");
        failures += Expect(!loginResultSchema->formatEmpty, "whV formatEmpty false");
        failures += Expect(loginResultSchema->source == "xlsx", "whV raw source");
        failures += ExpectPacketFieldsMatch(*loginResultSchema, packet, "whV raw field order");
        failures += Expect(loginResultSchema->hasResolvedFormat, "whV resolvedFormat present");
        failures += Expect(
            loginResultSchema->resolvedFormat == "{result:i62} {data:es}",
            "whV resolvedFormat");
        failures += Expect(
            loginResultSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "whV resolvedSource");
        failures += Expect(loginResultSchema->hasResolvedFields, "whV resolvedFields present");
        failures += Expect(loginResultSchema->resolvedFields.size() == 2U, "whV resolved field count");
        for (const FieldSchema& field : loginResultSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "whV resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *loginResultSchema,
            cg::packet::receive::ClientLoginResult::Spec,
            "whV Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *loginResultSchema,
            packet,
            "whV resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "whV"),
        "whV absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "whV"),
        "whV present in receive typed manifest");

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
            characterListSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "Daj resolvedSource");
        for (const FieldSchema& field : characterListSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "Daj resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *characterListSchema,
            cg::packet::receive::CharacterList::Spec,
            "Daj Spec matches effective fields");
        failures += Expect(packet.HasValue(), "Daj packet exists");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(*characterListSchema, packet.Value(), "Daj resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "Daj"),
        "Daj absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "Daj"),
        "Daj present in receive typed manifest");

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
            processStatusSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "cJzI resolvedSource");
        failures += Expect(
            processStatusSchema->resolvedFields.size() == 1U &&
                processStatusSchema->resolvedFields[0].source ==
                    "ServerC+ClientIda+src/packet",
            "cJzI resolved field source");
        failures += Expect(processStatusSchema->fields.empty(), "cJzI raw fields empty");
        failures += ExpectEffectiveFieldsMatch(
            *processStatusSchema,
            cg::packet::receive::ProcessStatus::Spec,
            "cJzI Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(*processStatusSchema, packet, "cJzI resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "cJzI"),
        "cJzI absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "cJzI"),
        "cJzI present in receive typed manifest");

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
        const cg::packet::PacketResult<cg::packet::Packet> packet = profileData.ToPacket();
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
        failures += Expect(static_cast<bool>(packet), "DpSY ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *profileDataSchema,
                packet.Value(),
                "DpSY resolved/ToPacket field order");
        }
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

    const PacketSchema* partyParamSchema = FindPacket(packets, "TNT");
    failures += Expect(partyParamSchema != nullptr, "TNT schema exists");
    if (partyParamSchema != nullptr) {
        std::vector<cg::packet::receive::PartyParamEntry> entries;
        entries.push_back(cg::packet::receive::PartyParamEntry {10, 100, 120, 30, 40});
        const cg::packet::receive::PartyParam partyParam(std::move(entries));
        const cg::packet::Packet packet = partyParam.ToPacket();
        failures += Expect(partyParamSchema->direction == "服务端->客户端", "TNT direction");
        failures += Expect(partyParamSchema->legacyKind == "SEND", "TNT legacyKind");
        failures += Expect(partyParamSchema->clientNamespace == "receive", "TNT namespace");
        failures += Expect(partyParamSchema->rawFormat.empty(), "TNT rawFormat empty");
        failures += Expect(partyParamSchema->formatEmpty, "TNT formatEmpty true");
        failures += Expect(partyParamSchema->source == "xlsx", "TNT raw source");
        failures += Expect(partyParamSchema->fields.empty(), "TNT raw fields empty");
        failures += Expect(partyParamSchema->hasResolvedFormat, "TNT resolvedFormat present");
        failures += Expect(partyParamSchema->resolvedFormat == "{data:es}", "TNT resolvedFormat");
        failures += Expect(
            partyParamSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "TNT resolvedSource");
        failures += Expect(partyParamSchema->hasResolvedFields, "TNT resolvedFields present");
        failures += Expect(partyParamSchema->resolvedFields.size() == 1U, "TNT resolved field count");
        if (partyParamSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = partyParamSchema->resolvedFields[0];
            failures += Expect(field.index == 0, "TNT resolved field index");
            failures += Expect(field.name == "data", "TNT resolved field name");
            failures += Expect(field.typeToken == "es", "TNT resolved field type");
            failures += Expect(field.rawSpec == "{data:es}", "TNT resolved field spec");
            failures += Expect(
                field.source == "ServerC+ClientIda+src/packet",
                "TNT resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *partyParamSchema,
            cg::packet::receive::PartyParam::Spec,
            "TNT Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *partyParamSchema,
            packet,
            "TNT resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "TNT"),
        "TNT absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "TNT"),
        "TNT present in receive typed manifest");

    const PacketSchema* petMailAnimationSchema = FindPacket(packets, "lDuJ");
    failures += Expect(petMailAnimationSchema != nullptr, "lDuJ schema exists");
    if (petMailAnimationSchema != nullptr) {
        const cg::packet::receive::PetMailAnimation petMailAnimation(
            1,
            62,
            -3,
            4,
            5,
            0,
            7,
            std::string {});
        const cg::packet::PacketResult<cg::packet::Packet> packet = petMailAnimation.ToPacket();
        const std::array<std::string_view, 8U> names = {{
            "objIndex", "graphicsno", "x", "y", "dir", "flg", "no", "data",
        }};
        const std::array<std::string_view, 8U> types = {{
            "i62", "i62", "i62", "i62", "i62", "i62", "i62", "es",
        }};
        const std::array<std::string_view, 8U> rawSpecs = {{
            "{objIndex}", "{graphicsno}", "{x}", "{y}", "{dir}", "{flg}", "{no}", "{data}",
        }};
        const std::array<std::string_view, 8U> resolvedSpecs = {{
            "{objIndex:i62}",
            "{graphicsno:i62}",
            "{x:i62}",
            "{y:i62}",
            "{dir:i62}",
            "{flg:i62}",
            "{no:i62}",
            "{data:es}",
        }};
        constexpr std::string_view resolvedSource = "xlsx+ServerC+ClientIda+src/packet";

        failures += Expect(petMailAnimationSchema->direction == "服务端->客户端", "lDuJ direction");
        failures += Expect(petMailAnimationSchema->legacyKind == "SEND", "lDuJ legacyKind");
        failures += Expect(petMailAnimationSchema->clientNamespace == "receive", "lDuJ namespace");
        failures += Expect(petMailAnimationSchema->source == "xlsx", "lDuJ raw source");
        failures += Expect(
            petMailAnimationSchema->rawFormat ==
                "{objIndex} {graphicsno} {x} {y} {dir} {flg} {no} {data}",
            "lDuJ rawFormat");
        failures += Expect(!petMailAnimationSchema->formatEmpty, "lDuJ formatEmpty false");
        failures += Expect(petMailAnimationSchema->fields.size() == names.size(), "lDuJ raw field count");
        if (petMailAnimationSchema->fields.size() == names.size()) {
            for (std::size_t index = 0; index < names.size(); ++index) {
                const FieldSchema& field = petMailAnimationSchema->fields[index];
                failures += Expect(field.index == static_cast<int>(index), "lDuJ raw field index");
                failures += Expect(field.name == names[index], "lDuJ raw field name");
                failures += Expect(field.typeToken == "unknown", "lDuJ raw field type");
                failures += Expect(field.rawSpec == rawSpecs[index], "lDuJ raw field spec");
                failures += Expect(field.source == "xlsx", "lDuJ raw field source");
            }
        }
        failures += Expect(
            petMailAnimationSchema->resolvedFormat ==
                "{objIndex:i62} {graphicsno:i62} {x:i62} {y:i62} {dir:i62} {flg:i62} {no:i62} {data:es}",
            "lDuJ resolvedFormat");
        failures += Expect(
            petMailAnimationSchema->resolvedSource == resolvedSource,
            "lDuJ resolvedSource");
        failures += Expect(
            petMailAnimationSchema->resolvedFields.size() == names.size(),
            "lDuJ resolved field count");
        if (petMailAnimationSchema->resolvedFields.size() == names.size()) {
            for (std::size_t index = 0; index < names.size(); ++index) {
                const FieldSchema& field = petMailAnimationSchema->resolvedFields[index];
                failures += Expect(field.index == static_cast<int>(index), "lDuJ resolved field index");
                failures += Expect(field.name == names[index], "lDuJ resolved field name");
                failures += Expect(field.typeToken == types[index], "lDuJ resolved field type");
                failures += Expect(field.rawSpec == resolvedSpecs[index], "lDuJ resolved field spec");
                failures += Expect(field.source == resolvedSource, "lDuJ resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *petMailAnimationSchema,
            cg::packet::receive::PetMailAnimation::Spec,
            "lDuJ Spec matches effective fields");
        failures += Expect(packet.HasValue(), "lDuJ ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *petMailAnimationSchema,
                packet.Value(),
                "lDuJ resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "lDuJ"),
        "lDuJ absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "lDuJ"),
        "lDuJ present in receive typed manifest");

    const PacketSchema* ridePetDataSchema = FindPacket(packets, "IsSy");
    failures += Expect(ridePetDataSchema != nullptr, "IsSy schema exists");
    if (ridePetDataSchema != nullptr) {
        const cg::packet::receive::RidePetData ridePetData(
            2,
            cg::packet::receive::RidePetPayload::Clear());
        const cg::packet::PacketResult<cg::packet::Packet> packet = ridePetData.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";
        const std::array<std::string_view, 2U> names = {{"petIndex", "data"}};
        const std::array<std::string_view, 2U> types = {{"i62", "es"}};
        const std::array<std::string_view, 2U> specs = {{
            "{petIndex:i62}",
            "{data:es}",
        }};

        failures += Expect(ridePetDataSchema->direction == "服务端->客户端", "IsSy direction");
        failures += Expect(ridePetDataSchema->legacyKind == "SEND", "IsSy legacyKind");
        failures += Expect(ridePetDataSchema->clientNamespace == "receive", "IsSy namespace");
        failures += Expect(ridePetDataSchema->rawFormat.empty(), "IsSy rawFormat empty");
        failures += Expect(ridePetDataSchema->formatEmpty, "IsSy formatEmpty true");
        failures += Expect(ridePetDataSchema->source == "xlsx", "IsSy raw source");
        failures += Expect(ridePetDataSchema->fields.empty(), "IsSy raw fields empty");
        failures += Expect(ridePetDataSchema->hasResolvedFormat, "IsSy resolvedFormat present");
        failures += Expect(
            ridePetDataSchema->resolvedFormat == "{petIndex:i62} {data:es}",
            "IsSy resolvedFormat");
        failures += Expect(
            ridePetDataSchema->resolvedSource == resolvedSource,
            "IsSy resolvedSource");
        failures += Expect(ridePetDataSchema->hasResolvedFields, "IsSy resolvedFields present");
        failures += Expect(
            ridePetDataSchema->resolvedFields.size() == names.size(),
            "IsSy resolved field count");
        if (ridePetDataSchema->resolvedFields.size() == names.size()) {
            for (std::size_t index = 0; index < names.size(); ++index) {
                const FieldSchema& field = ridePetDataSchema->resolvedFields[index];
                failures += Expect(field.index == static_cast<int>(index), "IsSy resolved field index");
                failures += Expect(field.name == names[index], "IsSy resolved field name");
                failures += Expect(field.typeToken == types[index], "IsSy resolved field type");
                failures += Expect(field.rawSpec == specs[index], "IsSy resolved field spec");
                failures += Expect(field.source == resolvedSource, "IsSy resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *ridePetDataSchema,
            cg::packet::receive::RidePetData::Spec,
            "IsSy Spec matches effective fields");
        failures += Expect(packet.HasValue(), "IsSy ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *ridePetDataSchema,
                packet.Value(),
                "IsSy resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "IsSy"),
        "IsSy absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "IsSy"),
        "IsSy present in receive typed manifest");

    const PacketSchema* ridePetExtraDataSchema = FindPacket(packets, "AU");
    failures += Expect(ridePetExtraDataSchema != nullptr, "AU schema exists");
    if (ridePetExtraDataSchema != nullptr) {
        const cg::packet::receive::RidePetExtraData ridePetExtraData(
            2,
            cg::packet::receive::RidePetExtraPayload::Clear());
        const cg::packet::Packet packet = ridePetExtraData.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";
        const std::array<std::string_view, 2U> names = {{"petIndex", "data"}};
        const std::array<std::string_view, 2U> types = {{"i62", "es"}};
        const std::array<std::string_view, 2U> specs = {{
            "{petIndex:i62}",
            "{data:es}",
        }};

        failures += Expect(ridePetExtraDataSchema->direction == "服务端->客户端", "AU direction");
        failures += Expect(ridePetExtraDataSchema->legacyKind == "SEND", "AU legacyKind");
        failures += Expect(ridePetExtraDataSchema->clientNamespace == "receive", "AU namespace");
        failures += Expect(ridePetExtraDataSchema->rawFormat.empty(), "AU rawFormat empty");
        failures += Expect(ridePetExtraDataSchema->formatEmpty, "AU formatEmpty true");
        failures += Expect(ridePetExtraDataSchema->source == "xlsx", "AU raw source");
        failures += Expect(ridePetExtraDataSchema->fields.empty(), "AU raw fields empty");
        failures += Expect(ridePetExtraDataSchema->hasResolvedFormat, "AU resolvedFormat present");
        failures += Expect(
            ridePetExtraDataSchema->resolvedFormat == "{petIndex:i62} {data:es}",
            "AU resolvedFormat");
        failures += Expect(
            ridePetExtraDataSchema->resolvedSource == resolvedSource,
            "AU resolvedSource");
        failures += Expect(ridePetExtraDataSchema->hasResolvedFields, "AU resolvedFields present");
        failures += Expect(
            ridePetExtraDataSchema->resolvedFields.size() == names.size(),
            "AU resolved field count");
        if (ridePetExtraDataSchema->resolvedFields.size() == names.size()) {
            for (std::size_t index = 0; index < names.size(); ++index) {
                const FieldSchema& field = ridePetExtraDataSchema->resolvedFields[index];
                failures += Expect(field.index == static_cast<int>(index), "AU resolved field index");
                failures += Expect(field.name == names[index], "AU resolved field name");
                failures += Expect(field.typeToken == types[index], "AU resolved field type");
                failures += Expect(field.rawSpec == specs[index], "AU resolved field spec");
                failures += Expect(field.source == resolvedSource, "AU resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *ridePetExtraDataSchema,
            cg::packet::receive::RidePetExtraData::Spec,
            "AU Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *ridePetExtraDataSchema,
            packet,
            "AU resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "AU"),
        "AU absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "AU"),
        "AU present in receive typed manifest");

    const PacketSchema* techUseResultSchema = FindPacket(packets, "JgO");
    failures += Expect(techUseResultSchema != nullptr, "JgO schema exists");
    if (techUseResultSchema != nullptr) {
        const cg::packet::receive::TechUseResult techUseResult(
            1,
            2,
            cg::packet::receive::TechUsePayload {});
        const cg::packet::PacketResult<cg::packet::Packet> packet = techUseResult.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";
        const std::array<std::string_view, 3U> names = {{"result", "type", "data"}};
        const std::array<std::string_view, 3U> types = {{"i62", "i62", "es"}};
        const std::array<std::string_view, 3U> specs = {{
            "{result:i62}",
            "{type:i62}",
            "{data:es}",
        }};

        failures += Expect(techUseResultSchema->direction == "服务端->客户端", "JgO direction");
        failures += Expect(techUseResultSchema->legacyKind == "SEND", "JgO legacyKind");
        failures += Expect(techUseResultSchema->clientNamespace == "receive", "JgO namespace");
        failures += Expect(techUseResultSchema->rawFormat.empty(), "JgO rawFormat empty");
        failures += Expect(techUseResultSchema->formatEmpty, "JgO formatEmpty true");
        failures += Expect(techUseResultSchema->source == "xlsx", "JgO raw source");
        failures += Expect(techUseResultSchema->fields.empty(), "JgO raw fields empty");
        failures += Expect(techUseResultSchema->hasResolvedFormat, "JgO resolvedFormat present");
        failures += Expect(
            techUseResultSchema->resolvedFormat == "{result:i62} {type:i62} {data:es}",
            "JgO resolvedFormat");
        failures += Expect(
            techUseResultSchema->resolvedSource == resolvedSource,
            "JgO resolvedSource");
        failures += Expect(techUseResultSchema->hasResolvedFields, "JgO resolvedFields present");
        failures += Expect(
            techUseResultSchema->resolvedFields.size() == names.size(),
            "JgO resolved field count");
        if (techUseResultSchema->resolvedFields.size() == names.size()) {
            for (std::size_t index = 0; index < names.size(); ++index) {
                const FieldSchema& field = techUseResultSchema->resolvedFields[index];
                failures += Expect(field.index == static_cast<int>(index), "JgO resolved field index");
                failures += Expect(field.name == names[index], "JgO resolved field name");
                failures += Expect(field.typeToken == types[index], "JgO resolved field type");
                failures += Expect(field.rawSpec == specs[index], "JgO resolved field spec");
                failures += Expect(field.source == resolvedSource, "JgO resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *techUseResultSchema,
            cg::packet::receive::TechUseResult::Spec,
            "JgO Spec matches effective fields");
        failures += Expect(packet.HasValue(), "JgO ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *techUseResultSchema,
                packet.Value(),
                "JgO resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "JgO"),
        "JgO absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "JgO"),
        "JgO present in receive typed manifest");

    const PacketSchema* petTechDataSchema = FindPacket(packets, "OBee");
    failures += Expect(petTechDataSchema != nullptr, "OBee schema exists");
    if (petTechDataSchema != nullptr) {
        std::vector<cg::packet::receive::PetTechRecord> records;
        records.push_back(cg::packet::receive::PetTechRecord {
            0,
            7300,
            "Name",
            "Description",
            0,
            1,
            1141,
            7,
        });
        const cg::packet::receive::PetTechData petTechData(0, std::move(records));
        const cg::packet::PacketResult<cg::packet::Packet> packet = petTechData.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";
        const std::array<std::string_view, 2U> names = {{"petIndex", "data"}};
        const std::array<std::string_view, 2U> types = {{"i62", "es"}};
        const std::array<std::string_view, 2U> specs = {{
            "{petIndex:i62}",
            "{data:es}",
        }};

        failures += Expect(petTechDataSchema->direction == "服务端->客户端", "OBee direction");
        failures += Expect(petTechDataSchema->legacyKind == "SEND", "OBee legacyKind");
        failures += Expect(petTechDataSchema->clientNamespace == "receive", "OBee namespace");
        failures += Expect(petTechDataSchema->rawFormat.empty(), "OBee rawFormat empty");
        failures += Expect(petTechDataSchema->formatEmpty, "OBee formatEmpty true");
        failures += Expect(petTechDataSchema->source == "xlsx", "OBee raw source");
        failures += Expect(petTechDataSchema->fields.empty(), "OBee raw fields empty");
        failures += Expect(petTechDataSchema->hasResolvedFormat, "OBee resolvedFormat present");
        failures += Expect(
            petTechDataSchema->resolvedFormat == "{petIndex:i62} {data:es}",
            "OBee resolvedFormat");
        failures += Expect(
            petTechDataSchema->resolvedSource == resolvedSource,
            "OBee resolvedSource");
        failures += Expect(petTechDataSchema->hasResolvedFields, "OBee resolvedFields present");
        failures += Expect(
            petTechDataSchema->resolvedFields.size() == names.size(),
            "OBee resolved field count");
        if (petTechDataSchema->resolvedFields.size() == names.size()) {
            for (std::size_t index = 0; index < names.size(); ++index) {
                const FieldSchema& field = petTechDataSchema->resolvedFields[index];
                failures += Expect(field.index == static_cast<int>(index), "OBee resolved field index");
                failures += Expect(field.name == names[index], "OBee resolved field name");
                failures += Expect(field.typeToken == types[index], "OBee resolved field type");
                failures += Expect(field.rawSpec == specs[index], "OBee resolved field spec");
                failures += Expect(field.source == resolvedSource, "OBee resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *petTechDataSchema,
            cg::packet::receive::PetTechData::Spec,
            "OBee Spec matches effective fields");
        failures += Expect(packet.HasValue(), "OBee ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *petTechDataSchema,
                packet.Value(),
                "OBee resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "OBee"),
        "OBee absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "OBee"),
        "OBee present in receive typed manifest");

    const PacketSchema* profileStatusSchema = FindPacket(packets, "Yo");
    failures += Expect(profileStatusSchema != nullptr, "Yo schema exists");
    if (profileStatusSchema != nullptr) {
        const cg::packet::receive::ProfileStatus profileStatus("opaque|data");
        const cg::packet::Packet packet = profileStatus.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";
        const std::array<std::string_view, 1U> names = {{"data"}};
        const std::array<std::string_view, 1U> types = {{"es"}};
        const std::array<std::string_view, 1U> specs = {{"{data:es}"}};

        failures += Expect(profileStatusSchema->direction == "服务端->客户端", "Yo direction");
        failures += Expect(profileStatusSchema->legacyKind == "SEND", "Yo legacyKind");
        failures += Expect(profileStatusSchema->clientNamespace == "receive", "Yo namespace");
        failures += Expect(profileStatusSchema->rawFormat.empty(), "Yo rawFormat empty");
        failures += Expect(profileStatusSchema->formatEmpty, "Yo formatEmpty true");
        failures += Expect(profileStatusSchema->source == "xlsx", "Yo raw source");
        failures += Expect(profileStatusSchema->fields.empty(), "Yo raw fields empty");
        failures += Expect(profileStatusSchema->hasResolvedFormat, "Yo resolvedFormat present");
        failures += Expect(profileStatusSchema->resolvedFormat == "{data:es}", "Yo resolvedFormat");
        failures += Expect(profileStatusSchema->resolvedSource == resolvedSource, "Yo resolvedSource");
        failures += Expect(profileStatusSchema->hasResolvedFields, "Yo resolvedFields present");
        failures += Expect(
            profileStatusSchema->resolvedFields.size() == names.size(),
            "Yo resolved field count");
        if (profileStatusSchema->resolvedFields.size() == names.size()) {
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = profileStatusSchema->resolvedFields[index];
                failures += Expect(field.index == static_cast<int>(index), "Yo resolved field index");
                failures += Expect(field.name == names[index], "Yo resolved field name");
                failures += Expect(field.typeToken == types[index], "Yo resolved field type");
                failures += Expect(field.rawSpec == specs[index], "Yo resolved field spec");
                failures += Expect(field.source == resolvedSource, "Yo resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *profileStatusSchema,
            cg::packet::receive::ProfileStatus::Spec,
            "Yo Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *profileStatusSchema,
            packet,
            "Yo resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "Yo"),
        "Yo absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "Yo"),
        "Yo present in receive typed manifest");

    const PacketSchema* frontCharListSchema = FindPacket(packets, "ysVc");
    failures += Expect(frontCharListSchema != nullptr, "ysVc schema exists");
    if (frontCharListSchema != nullptr) {
        const cg::packet::receive::FrontCharList frontCharList(
            std::vector<cg::packet::receive::FrontCharRecord> {});
        const cg::packet::PacketResult<cg::packet::Packet> packet = frontCharList.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";

        failures += Expect(frontCharListSchema->direction == "服务端->客户端", "ysVc direction");
        failures += Expect(frontCharListSchema->legacyKind == "SEND", "ysVc legacyKind");
        failures += Expect(frontCharListSchema->clientNamespace == "receive", "ysVc namespace");
        failures += Expect(frontCharListSchema->rawFormat.empty(), "ysVc rawFormat empty");
        failures += Expect(frontCharListSchema->formatEmpty, "ysVc formatEmpty true");
        failures += Expect(frontCharListSchema->source == "xlsx", "ysVc raw source");
        failures += Expect(frontCharListSchema->fields.empty(), "ysVc raw fields empty");
        failures += Expect(frontCharListSchema->hasResolvedFormat, "ysVc resolvedFormat present");
        failures += Expect(frontCharListSchema->resolvedFormat == "{data:es}", "ysVc resolvedFormat");
        failures += Expect(
            frontCharListSchema->resolvedSource == resolvedSource,
            "ysVc resolvedSource");
        failures += Expect(frontCharListSchema->hasResolvedFields, "ysVc resolvedFields present");
        failures += Expect(
            frontCharListSchema->resolvedFields.size() == 1U,
            "ysVc resolved field count");
        if (frontCharListSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = frontCharListSchema->resolvedFields[0U];
            failures += Expect(field.index == 0, "ysVc resolved field index");
            failures += Expect(field.name == "data", "ysVc resolved field name");
            failures += Expect(field.typeToken == "es", "ysVc resolved field type");
            failures += Expect(field.rawSpec == "{data:es}", "ysVc resolved field spec");
            failures += Expect(field.source == resolvedSource, "ysVc resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *frontCharListSchema,
            cg::packet::receive::FrontCharList::Spec,
            "ysVc Spec matches effective fields");
        failures += Expect(packet.HasValue(), "ysVc ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *frontCharListSchema,
                packet.Value(),
                "ysVc resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "ysVc"),
        "ysVc absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "ysVc"),
        "ysVc present in receive typed manifest");

    const PacketSchema* itemFrontCharListSchema = FindPacket(packets, "LY");
    failures += Expect(itemFrontCharListSchema != nullptr, "LY schema exists");
    if (itemFrontCharListSchema != nullptr) {
        const cg::packet::receive::ItemFrontCharList itemFrontCharList(
            std::vector<cg::packet::receive::FrontCharRecord> {});
        const cg::packet::PacketResult<cg::packet::Packet> packet = itemFrontCharList.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";

        failures += Expect(itemFrontCharListSchema->direction == "服务端->客户端", "LY direction");
        failures += Expect(itemFrontCharListSchema->legacyKind == "SEND", "LY legacyKind");
        failures += Expect(itemFrontCharListSchema->clientNamespace == "receive", "LY namespace");
        failures += Expect(itemFrontCharListSchema->rawFormat.empty(), "LY rawFormat empty");
        failures += Expect(itemFrontCharListSchema->formatEmpty, "LY formatEmpty true");
        failures += Expect(itemFrontCharListSchema->source == "xlsx", "LY raw source");
        failures += Expect(itemFrontCharListSchema->fields.empty(), "LY raw fields empty");
        failures += Expect(itemFrontCharListSchema->hasResolvedFormat, "LY resolvedFormat present");
        failures += Expect(itemFrontCharListSchema->resolvedFormat == "{data:es}", "LY resolvedFormat");
        failures += Expect(
            itemFrontCharListSchema->resolvedSource == resolvedSource,
            "LY resolvedSource");
        failures += Expect(itemFrontCharListSchema->hasResolvedFields, "LY resolvedFields present");
        failures += Expect(
            itemFrontCharListSchema->resolvedFields.size() == 1U,
            "LY resolved field count");
        if (itemFrontCharListSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = itemFrontCharListSchema->resolvedFields[0U];
            failures += Expect(field.index == 0, "LY resolved field index");
            failures += Expect(field.name == "data", "LY resolved field name");
            failures += Expect(field.typeToken == "es", "LY resolved field type");
            failures += Expect(field.rawSpec == "{data:es}", "LY resolved field spec");
            failures += Expect(field.source == resolvedSource, "LY resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *itemFrontCharListSchema,
            cg::packet::receive::ItemFrontCharList::Spec,
            "LY Spec matches effective fields");
        failures += Expect(packet.HasValue(), "LY ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *itemFrontCharListSchema,
                packet.Value(),
                "LY resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "LY"),
        "LY absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "LY"),
        "LY present in receive typed manifest");

    const PacketSchema* characterPetListSchema = FindPacket(packets, "ND");
    failures += Expect(characterPetListSchema != nullptr, "ND schema exists");
    if (characterPetListSchema != nullptr) {
        const cg::packet::receive::CharacterPetList characterPetList(
            std::vector<cg::packet::receive::CharacterPetRecord> {});
        const cg::packet::PacketResult<cg::packet::Packet> packet = characterPetList.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";

        failures += Expect(characterPetListSchema->direction == "服务端->客户端", "ND direction");
        failures += Expect(characterPetListSchema->legacyKind == "SEND", "ND legacyKind");
        failures += Expect(characterPetListSchema->clientNamespace == "receive", "ND namespace");
        failures += Expect(characterPetListSchema->rawFormat.empty(), "ND rawFormat empty");
        failures += Expect(characterPetListSchema->formatEmpty, "ND formatEmpty true");
        failures += Expect(characterPetListSchema->source == "xlsx", "ND raw source");
        failures += Expect(characterPetListSchema->fields.empty(), "ND raw fields empty");
        failures += Expect(characterPetListSchema->hasResolvedFormat, "ND resolvedFormat present");
        failures += Expect(characterPetListSchema->resolvedFormat == "{data:es}", "ND resolvedFormat");
        failures += Expect(
            characterPetListSchema->resolvedSource == resolvedSource,
            "ND resolvedSource");
        failures += Expect(characterPetListSchema->hasResolvedFields, "ND resolvedFields present");
        failures += Expect(
            characterPetListSchema->resolvedFields.size() == 1U,
            "ND resolved field count");
        if (characterPetListSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = characterPetListSchema->resolvedFields[0U];
            failures += Expect(field.index == 0, "ND resolved field index");
            failures += Expect(field.name == "data", "ND resolved field name");
            failures += Expect(field.typeToken == "es", "ND resolved field type");
            failures += Expect(field.rawSpec == "{data:es}", "ND resolved field spec");
            failures += Expect(field.source == resolvedSource, "ND resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *characterPetListSchema,
            cg::packet::receive::CharacterPetList::Spec,
            "ND Spec matches effective fields");
        failures += Expect(packet.HasValue(), "ND ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *characterPetListSchema,
                packet.Value(),
                "ND resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "ND"),
        "ND absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "ND"),
        "ND present in receive typed manifest");

    const PacketSchema* itemCharacterPetListSchema = FindPacket(packets, "nFV");
    failures += Expect(itemCharacterPetListSchema != nullptr, "nFV schema exists");
    if (itemCharacterPetListSchema != nullptr) {
        const cg::packet::receive::ItemCharacterPetList itemCharacterPetList(
            std::vector<cg::packet::receive::CharacterPetRecord> {});
        const cg::packet::PacketResult<cg::packet::Packet> packet = itemCharacterPetList.ToPacket();
        constexpr std::string_view resolvedSource = "ServerC+ClientIda+src/packet";

        failures += Expect(itemCharacterPetListSchema->direction == "服务端->客户端", "nFV direction");
        failures += Expect(itemCharacterPetListSchema->legacyKind == "SEND", "nFV legacyKind");
        failures += Expect(itemCharacterPetListSchema->clientNamespace == "receive", "nFV namespace");
        failures += Expect(itemCharacterPetListSchema->rawFormat.empty(), "nFV rawFormat empty");
        failures += Expect(itemCharacterPetListSchema->formatEmpty, "nFV formatEmpty true");
        failures += Expect(itemCharacterPetListSchema->source == "xlsx", "nFV raw source");
        failures += Expect(itemCharacterPetListSchema->fields.empty(), "nFV raw fields empty");
        failures += Expect(itemCharacterPetListSchema->hasResolvedFormat, "nFV resolvedFormat present");
        failures += Expect(itemCharacterPetListSchema->resolvedFormat == "{data:es}", "nFV resolvedFormat");
        failures += Expect(
            itemCharacterPetListSchema->resolvedSource == resolvedSource,
            "nFV resolvedSource");
        failures += Expect(itemCharacterPetListSchema->hasResolvedFields, "nFV resolvedFields present");
        failures += Expect(
            itemCharacterPetListSchema->resolvedFields.size() == 1U,
            "nFV resolved field count");
        if (itemCharacterPetListSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = itemCharacterPetListSchema->resolvedFields[0U];
            failures += Expect(field.index == 0, "nFV resolved field index");
            failures += Expect(field.name == "data", "nFV resolved field name");
            failures += Expect(field.typeToken == "es", "nFV resolved field type");
            failures += Expect(field.rawSpec == "{data:es}", "nFV resolved field spec");
            failures += Expect(field.source == resolvedSource, "nFV resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *itemCharacterPetListSchema,
            cg::packet::receive::ItemCharacterPetList::Spec,
            "nFV Spec matches effective fields");
        failures += Expect(packet.HasValue(), "nFV ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *itemCharacterPetListSchema,
                packet.Value(),
                "nFV resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "nFV"),
        "nFV absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "nFV"),
        "nFV present in receive typed manifest");

    const PacketSchema* techListSchema = FindPacket(packets, "Rspn");
    failures += Expect(techListSchema != nullptr, "Rspn schema exists");
    if (techListSchema != nullptr) {
        std::vector<cg::packet::receive::TechRecord> records;
        records.push_back(cg::packet::receive::TechRecord {"Fire", "Burn", 10, 2, 3, 1});
        const cg::packet::receive::TechList techList(1, std::move(records));
        const cg::packet::PacketResult<cg::packet::Packet> packet = techList.ToPacket();
        failures += Expect(packet.HasValue(), "Rspn ToPacket");
        failures += Expect(techListSchema->direction == "服务端->客户端", "Rspn direction");
        failures += Expect(techListSchema->legacyKind == "SEND", "Rspn legacyKind");
        failures += Expect(techListSchema->clientNamespace == "receive", "Rspn namespace");
        failures += Expect(techListSchema->rawFormat.empty(), "Rspn rawFormat empty");
        failures += Expect(techListSchema->formatEmpty, "Rspn formatEmpty true");
        failures += Expect(techListSchema->source == "xlsx", "Rspn raw source");
        failures += Expect(techListSchema->fields.empty(), "Rspn raw fields empty");
        failures += Expect(techListSchema->hasResolvedFormat, "Rspn resolvedFormat present");
        failures += Expect(
            techListSchema->resolvedFormat == "{skillIndex:i62} {data:es}",
            "Rspn resolvedFormat");
        failures += Expect(
            techListSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "Rspn resolvedSource");
        failures += Expect(techListSchema->hasResolvedFields, "Rspn resolvedFields present");
        failures += Expect(techListSchema->resolvedFields.size() == 2U, "Rspn resolved field count");
        if (techListSchema->resolvedFields.size() == 2U) {
            const std::array<std::string_view, 2U> names = {{"skillIndex", "data"}};
            const std::array<std::string_view, 2U> types = {{"i62", "es"}};
            const std::array<std::string_view, 2U> specs = {{
                "{skillIndex:i62}",
                "{data:es}",
            }};
            for (std::size_t index = 0; index < names.size(); ++index) {
                const FieldSchema& field = techListSchema->resolvedFields[index];
                failures += Expect(field.index == static_cast<int>(index), "Rspn resolved field index");
                failures += Expect(field.name == names[index], "Rspn resolved field name");
                failures += Expect(field.typeToken == types[index], "Rspn resolved field type");
                failures += Expect(field.rawSpec == specs[index], "Rspn resolved field spec");
                failures += Expect(
                    field.source == "ServerC+ClientIda+src/packet",
                    "Rspn resolved field source");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *techListSchema,
            cg::packet::receive::TechList::Spec,
            "Rspn Spec matches effective fields");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *techListSchema,
                packet.Value(),
                "Rspn resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "Rspn"),
        "Rspn absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "Rspn"),
        "Rspn present in receive typed manifest");

    const PacketSchema* characterSkillDataSchema = FindPacket(packets, "cNE");
    failures += Expect(characterSkillDataSchema != nullptr, "cNE schema exists");
    if (characterSkillDataSchema != nullptr) {
        std::vector<cg::packet::receive::CharacterSkillRecord> records;
        records.push_back(cg::packet::receive::CharacterSkillRecord {
            0,
            61,
            "Fire",
            1,
            0,
            3,
            500,
            1,
            0,
            100,
            1,
            false,
        });
        const cg::packet::receive::CharacterSkillData characterSkillData(std::move(records));
        const cg::packet::PacketResult<cg::packet::Packet> packet = characterSkillData.ToPacket();
        failures += Expect(packet.HasValue(), "cNE ToPacket");
        failures += Expect(characterSkillDataSchema->direction == "服务端->客户端", "cNE direction");
        failures += Expect(characterSkillDataSchema->legacyKind == "SEND", "cNE legacyKind");
        failures += Expect(characterSkillDataSchema->clientNamespace == "receive", "cNE namespace");
        failures += Expect(characterSkillDataSchema->rawFormat == "{data:es}", "cNE rawFormat");
        failures += Expect(!characterSkillDataSchema->formatEmpty, "cNE formatEmpty false");
        failures += Expect(characterSkillDataSchema->source == "xlsx", "cNE raw source");
        failures += Expect(characterSkillDataSchema->fields.size() == 1U, "cNE raw field count");
        if (characterSkillDataSchema->fields.size() == 1U) {
            const FieldSchema& field = characterSkillDataSchema->fields[0];
            failures += Expect(field.index == 0, "cNE raw field index");
            failures += Expect(field.name == "data", "cNE raw field name");
            failures += Expect(field.typeToken == "es", "cNE raw field type");
            failures += Expect(field.rawSpec == "{data:es}", "cNE raw field spec");
            failures += Expect(field.source == "xlsx", "cNE raw field source");
        }
        failures += Expect(characterSkillDataSchema->hasResolvedFormat, "cNE resolvedFormat present");
        failures += Expect(
            characterSkillDataSchema->resolvedFormat == "{data:es}",
            "cNE resolvedFormat");
        failures += Expect(
            characterSkillDataSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "cNE resolvedSource");
        failures += Expect(characterSkillDataSchema->hasResolvedFields, "cNE resolvedFields present");
        failures += Expect(
            characterSkillDataSchema->resolvedFields.size() == 1U,
            "cNE resolved field count");
        if (characterSkillDataSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = characterSkillDataSchema->resolvedFields[0];
            failures += Expect(field.index == 0, "cNE resolved field index");
            failures += Expect(field.name == "data", "cNE resolved field name");
            failures += Expect(field.typeToken == "es", "cNE resolved field type");
            failures += Expect(field.rawSpec == "{data:es}", "cNE resolved field spec");
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "cNE resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *characterSkillDataSchema,
            cg::packet::receive::CharacterSkillData::Spec,
            "cNE Spec matches effective fields");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *characterSkillDataSchema,
                packet.Value(),
                "cNE resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "cNE"),
        "cNE absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "cNE"),
        "cNE present in receive typed manifest");

    const PacketSchema* itemDataSchema = FindPacket(packets, "rU");
    failures += Expect(itemDataSchema != nullptr, "rU schema exists");
    if (itemDataSchema != nullptr) {
        std::vector<cg::packet::receive::ItemRecord> records;
        records.push_back(cg::packet::receive::ItemRecord {
            0,
            "Potion",
            2,
            "Heal",
            100,
            1,
            0,
            3,
            1,
            0,
            500,
            7,
            2,
            "Memo",
            false,
        });
        const cg::packet::receive::ItemData itemData(std::move(records));
        const cg::packet::PacketResult<cg::packet::Packet> packet = itemData.ToPacket();
        failures += Expect(packet.HasValue(), "rU ToPacket");
        failures += Expect(itemDataSchema->direction == "服务端->客户端", "rU direction");
        failures += Expect(itemDataSchema->legacyKind == "SEND", "rU legacyKind");
        failures += Expect(itemDataSchema->clientNamespace == "receive", "rU namespace");
        failures += Expect(itemDataSchema->rawFormat == "{data:es}", "rU rawFormat");
        failures += Expect(!itemDataSchema->formatEmpty, "rU formatEmpty false");
        failures += Expect(itemDataSchema->source == "xlsx", "rU raw source");
        failures += Expect(itemDataSchema->fields.size() == 1U, "rU raw field count");
        if (itemDataSchema->fields.size() == 1U) {
            const FieldSchema& field = itemDataSchema->fields[0];
            failures += Expect(field.index == 0, "rU raw field index");
            failures += Expect(field.name == "data", "rU raw field name");
            failures += Expect(field.typeToken == "es", "rU raw field type");
            failures += Expect(field.rawSpec == "{data:es}", "rU raw field spec");
            failures += Expect(field.source == "xlsx", "rU raw field source");
        }
        failures += Expect(itemDataSchema->hasResolvedFormat, "rU resolvedFormat present");
        failures += Expect(itemDataSchema->resolvedFormat == "{data:es}", "rU resolvedFormat");
        failures += Expect(
            itemDataSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "rU resolvedSource");
        failures += Expect(itemDataSchema->hasResolvedFields, "rU resolvedFields present");
        failures += Expect(itemDataSchema->resolvedFields.size() == 1U, "rU resolved field count");
        if (itemDataSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = itemDataSchema->resolvedFields[0];
            failures += Expect(field.index == 0, "rU resolved field index");
            failures += Expect(field.name == "data", "rU resolved field name");
            failures += Expect(field.typeToken == "es", "rU resolved field type");
            failures += Expect(field.rawSpec == "{data:es}", "rU resolved field spec");
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "rU resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *itemDataSchema,
            cg::packet::receive::ItemData::Spec,
            "rU Spec matches effective fields");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *itemDataSchema,
                packet.Value(),
                "rU resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "rU"),
        "rU absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "rU"),
        "rU present in receive typed manifest");

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

    const PacketSchema* mapNameSchema = FindPacket(packets, "TeD");
    failures += Expect(mapNameSchema != nullptr, "TeD schema exists");
    if (mapNameSchema != nullptr) {
        const cg::packet::receive::MapName mapName("Map", 12);
        const cg::packet::PacketResult<cg::packet::Packet> packet = mapName.ToPacket();
        failures += Expect(mapNameSchema->direction == "服务端->客户端", "TeD direction");
        failures += Expect(mapNameSchema->legacyKind == "SEND", "TeD legacyKind");
        failures += Expect(mapNameSchema->clientNamespace == "receive", "TeD namespace");
        failures += Expect(mapNameSchema->xlsxStatus == "完成", "TeD xlsxStatus");
        failures += Expect(mapNameSchema->description == "SEND_MN SendMapName", "TeD description");
        failures += Expect(mapNameSchema->rawFormat == "{data:es}", "TeD rawFormat");
        failures += Expect(!mapNameSchema->formatEmpty, "TeD formatEmpty false");
        failures += Expect(mapNameSchema->source == "xlsx", "TeD raw source");
        failures += Expect(mapNameSchema->fields.size() == 1U, "TeD raw field count");
        if (mapNameSchema->fields.size() == 1U) {
            const FieldSchema& field = mapNameSchema->fields[0];
            failures += Expect(
                field.index == 0U &&
                    field.name == "data" &&
                    field.typeToken == "es" &&
                    field.rawSpec == "{data:es}" &&
                    field.source == "xlsx",
                "TeD raw field");
        }
        failures += Expect(mapNameSchema->hasResolvedFormat, "TeD resolvedFormat present");
        failures += Expect(mapNameSchema->resolvedFormat == "{data:es}", "TeD resolvedFormat");
        failures += Expect(
            mapNameSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "TeD resolvedSource");
        failures += Expect(mapNameSchema->hasResolvedFields, "TeD resolvedFields present");
        failures += Expect(mapNameSchema->resolvedFields.size() == 1U, "TeD resolved field count");
        if (mapNameSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = mapNameSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0U &&
                    field.name == "data" &&
                    field.typeToken == "es" &&
                    field.rawSpec == "{data:es}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "TeD resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *mapNameSchema,
            cg::packet::receive::MapName::Spec,
            "TeD Spec matches effective fields");
        failures += Expect(packet.HasValue(), "TeD ToPacket");
        if (packet.HasValue()) {
            failures += Expect(
                packet.Value().fields.size() == 1U &&
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::SubPayload,
                "TeD ToPacket payload type");
            failures += ExpectResolvedFieldsMatch(
                *mapNameSchema,
                packet.Value(),
                "TeD ToPacket field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "TeD"),
        "TeD absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "TeD"),
        "TeD present in receive typed manifest");

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
        const cg::packet::send::TechUseRequest request(
            10,
            11,
            -1,
            "100|18|19");
        const cg::packet::Packet packet = request.ToPacket();
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
                "xlsx+ServerC+ClientIda+src/packet",
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
                        field.source ==
                            "xlsx+ServerC+ClientIda+src/packet",
                    "sM resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *techUseRequestSchema,
            cg::packet::send::TechUseRequest::Spec,
            "sM Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *techUseRequestSchema,
            packet,
            "sM resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "sM"),
        "sM present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "sM"),
        "sM absent from receive typed manifest");

    const PacketSchema* tradeStartRequestSchema =
        FindPacket(packets, "QeuC");
    failures += Expect(
        tradeStartRequestSchema != nullptr,
        "QeuC schema exists");
    if (tradeStartRequestSchema != nullptr) {
        failures += Expect(
            tradeStartRequestSchema->direction == "客户端->服务端",
            "QeuC direction");
        failures += Expect(
            tradeStartRequestSchema->legacyKind == "RECV",
            "QeuC legacyKind");
        failures += Expect(
            tradeStartRequestSchema->clientNamespace == "send",
            "QeuC namespace");
        failures += Expect(
            tradeStartRequestSchema->xlsxStatus == "完成",
            "QeuC xlsxStatus");
        failures += Expect(
            tradeStartRequestSchema->rawFormat == "{slot}",
            "QeuC rawFormat");
        failures += Expect(
            !tradeStartRequestSchema->formatEmpty,
            "QeuC formatEmpty false");
        failures += Expect(
            tradeStartRequestSchema->source == "xlsx",
            "QeuC raw source");
        failures += Expect(
            tradeStartRequestSchema->fields.size() == 1U,
            "QeuC raw field count");
        if (tradeStartRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                tradeStartRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "slot" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{slot}" &&
                    field.source == "xlsx",
                "QeuC raw field");
        }

        failures += Expect(
            tradeStartRequestSchema->hasResolvedFormat,
            "QeuC resolvedFormat present");
        failures += Expect(
            tradeStartRequestSchema->resolvedFormat ==
                "{tradeListSlot:i62}",
            "QeuC resolvedFormat");
        failures += Expect(
            tradeStartRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "QeuC resolvedSource");
        failures += Expect(
            tradeStartRequestSchema->hasResolvedFields,
            "QeuC resolvedFields present");
        failures += Expect(
            tradeStartRequestSchema->resolvedFields.size() == 1U,
            "QeuC resolved field count");
        if (tradeStartRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                tradeStartRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "tradeListSlot" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{tradeListSlot:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "QeuC resolved field");
        }

        const cg::packet::send::TradeStartRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *tradeStartRequestSchema,
            cg::packet::send::TradeStartRequest::Spec,
            "QeuC Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *tradeStartRequestSchema,
            packet,
            "QeuC resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "QeuC"),
        "QeuC present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "QeuC"),
        "QeuC absent from receive typed manifest");

    const PacketSchema* tradeConfirmationRequestSchema =
        FindPacket(packets, "OU");
    failures += Expect(
        tradeConfirmationRequestSchema != nullptr,
        "OU schema exists");
    if (tradeConfirmationRequestSchema != nullptr) {
        failures += Expect(
            tradeConfirmationRequestSchema->direction ==
                "客户端->服务端",
            "OU direction");
        failures += Expect(
            tradeConfirmationRequestSchema->legacyKind == "RECV",
            "OU legacyKind");
        failures += Expect(
            tradeConfirmationRequestSchema->clientNamespace == "send",
            "OU namespace");
        failures += Expect(
            tradeConfirmationRequestSchema->xlsxStatus == "完成",
            "OU xlsxStatus");
        failures += Expect(
            tradeConfirmationRequestSchema->rawFormat == "{result}",
            "OU rawFormat");
        failures += Expect(
            !tradeConfirmationRequestSchema->formatEmpty,
            "OU formatEmpty false");
        failures += Expect(
            tradeConfirmationRequestSchema->source == "xlsx",
            "OU raw source");
        failures += Expect(
            tradeConfirmationRequestSchema->fields.size() == 1U,
            "OU raw field count");
        if (tradeConfirmationRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                tradeConfirmationRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "result" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{result}" &&
                    field.source == "xlsx",
                "OU raw field");
        }

        failures += Expect(
            tradeConfirmationRequestSchema->hasResolvedFormat,
            "OU resolvedFormat present");
        failures += Expect(
            tradeConfirmationRequestSchema->resolvedFormat ==
                "{result:i62}",
            "OU resolvedFormat");
        failures += Expect(
            tradeConfirmationRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "OU resolvedSource");
        failures += Expect(
            tradeConfirmationRequestSchema->hasResolvedFields,
            "OU resolvedFields present");
        failures += Expect(
            tradeConfirmationRequestSchema->resolvedFields.size() == 1U,
            "OU resolved field count");
        if (tradeConfirmationRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                tradeConfirmationRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "result" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{result:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "OU resolved field");
        }

        const cg::packet::send::TradeConfirmationRequest request(1);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *tradeConfirmationRequestSchema,
            cg::packet::send::TradeConfirmationRequest::Spec,
            "OU Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *tradeConfirmationRequestSchema,
            packet,
            "OU resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "OU"),
        "OU present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "OU"),
        "OU absent from receive typed manifest");

    const PacketSchema* titleSelectionRequestSchema =
        FindPacket(packets, "zGp");
    failures += Expect(
        titleSelectionRequestSchema != nullptr,
        "zGp schema exists");
    if (titleSelectionRequestSchema != nullptr) {
        failures += Expect(
            titleSelectionRequestSchema->direction == "客户端->服务端",
            "zGp direction");
        failures += Expect(
            titleSelectionRequestSchema->legacyKind == "RECV",
            "zGp legacyKind");
        failures += Expect(
            titleSelectionRequestSchema->clientNamespace == "send",
            "zGp namespace");
        failures += Expect(
            titleSelectionRequestSchema->xlsxStatus == "完成",
            "zGp xlsxStatus");
        failures += Expect(
            titleSelectionRequestSchema->rawFormat == "{slot}",
            "zGp rawFormat");
        failures += Expect(
            !titleSelectionRequestSchema->formatEmpty,
            "zGp formatEmpty false");
        failures += Expect(
            titleSelectionRequestSchema->source == "xlsx",
            "zGp raw source");
        failures += Expect(
            titleSelectionRequestSchema->fields.size() == 1U,
            "zGp raw field count");
        if (titleSelectionRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                titleSelectionRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "slot" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{slot}" &&
                    field.source == "xlsx",
                "zGp raw field");
        }

        failures += Expect(
            titleSelectionRequestSchema->hasResolvedFormat,
            "zGp resolvedFormat present");
        failures += Expect(
            titleSelectionRequestSchema->resolvedFormat ==
                "{titleSlot:i62}",
            "zGp resolvedFormat");
        failures += Expect(
            titleSelectionRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "zGp resolvedSource");
        failures += Expect(
            titleSelectionRequestSchema->hasResolvedFields,
            "zGp resolvedFields present");
        failures += Expect(
            titleSelectionRequestSchema->resolvedFields.size() == 1U,
            "zGp resolved field count");
        if (titleSelectionRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                titleSelectionRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "titleSlot" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{titleSlot:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "zGp resolved field");
        }

        const cg::packet::send::TitleSelectionRequest request(-1);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *titleSelectionRequestSchema,
            cg::packet::send::TitleSelectionRequest::Spec,
            "zGp Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *titleSelectionRequestSchema,
            packet,
            "zGp resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "zGp"),
        "zGp present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "zGp"),
        "zGp absent from receive typed manifest");

    const PacketSchema* titleDeletionRequestSchema =
        FindPacket(packets, "tj");
    failures += Expect(
        titleDeletionRequestSchema != nullptr,
        "tj schema exists");
    if (titleDeletionRequestSchema != nullptr) {
        failures += Expect(
            titleDeletionRequestSchema->direction == "客户端->服务端",
            "tj direction");
        failures += Expect(
            titleDeletionRequestSchema->legacyKind == "RECV",
            "tj legacyKind");
        failures += Expect(
            titleDeletionRequestSchema->clientNamespace == "send",
            "tj namespace");
        failures += Expect(
            titleDeletionRequestSchema->xlsxStatus == "完成",
            "tj xlsxStatus");
        failures += Expect(
            titleDeletionRequestSchema->rawFormat == "{slot}",
            "tj rawFormat");
        failures += Expect(
            !titleDeletionRequestSchema->formatEmpty,
            "tj formatEmpty false");
        failures += Expect(
            titleDeletionRequestSchema->source == "xlsx",
            "tj raw source");
        failures += Expect(
            titleDeletionRequestSchema->fields.size() == 1U,
            "tj raw field count");
        if (titleDeletionRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                titleDeletionRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "slot" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{slot}" &&
                    field.source == "xlsx",
                "tj raw field");
        }

        failures += Expect(
            titleDeletionRequestSchema->hasResolvedFormat,
            "tj resolvedFormat present");
        failures += Expect(
            titleDeletionRequestSchema->resolvedFormat ==
                "{titleSlot:i62}",
            "tj resolvedFormat");
        failures += Expect(
            titleDeletionRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "tj resolvedSource");
        failures += Expect(
            titleDeletionRequestSchema->hasResolvedFields,
            "tj resolvedFields present");
        failures += Expect(
            titleDeletionRequestSchema->resolvedFields.size() == 1U,
            "tj resolved field count");
        if (titleDeletionRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                titleDeletionRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "titleSlot" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{titleSlot:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "tj resolved field");
        }

        const cg::packet::send::TitleDeletionRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *titleDeletionRequestSchema,
            cg::packet::send::TitleDeletionRequest::Spec,
            "tj Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *titleDeletionRequestSchema,
            packet,
            "tj resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "tj"),
        "tj present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "tj"),
        "tj absent from receive typed manifest");

    const PacketSchema* customTitleInputRequestSchema =
        FindPacket(packets, "ACwB");
    failures += Expect(
        customTitleInputRequestSchema != nullptr,
        "ACwB schema exists");
    if (customTitleInputRequestSchema != nullptr) {
        failures += Expect(
            customTitleInputRequestSchema->direction == "客户端->服务端",
            "ACwB direction");
        failures += Expect(
            customTitleInputRequestSchema->legacyKind == "RECV",
            "ACwB legacyKind");
        failures += Expect(
            customTitleInputRequestSchema->clientNamespace == "send",
            "ACwB namespace");
        failures += Expect(
            customTitleInputRequestSchema->xlsxStatus == "完成",
            "ACwB xlsxStatus");
        failures += Expect(
            customTitleInputRequestSchema->rawFormat == "{title}",
            "ACwB rawFormat");
        failures += Expect(
            !customTitleInputRequestSchema->formatEmpty,
            "ACwB formatEmpty false");
        failures += Expect(
            customTitleInputRequestSchema->source == "xlsx",
            "ACwB raw source");
        failures += Expect(
            customTitleInputRequestSchema->fields.size() == 1U,
            "ACwB raw field count");
        if (customTitleInputRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                customTitleInputRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "title" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{title}" &&
                    field.source == "xlsx",
                "ACwB raw field");
        }

        failures += Expect(
            customTitleInputRequestSchema->hasResolvedFormat,
            "ACwB resolvedFormat present");
        failures += Expect(
            customTitleInputRequestSchema->resolvedFormat == "{title:es}",
            "ACwB resolvedFormat");
        failures += Expect(
            customTitleInputRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "ACwB resolvedSource");
        failures += Expect(
            customTitleInputRequestSchema->hasResolvedFields,
            "ACwB resolvedFields present");
        failures += Expect(
            customTitleInputRequestSchema->resolvedFields.size() == 1U,
            "ACwB resolved field count");
        if (customTitleInputRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                customTitleInputRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "title" &&
                    field.typeToken == "es" &&
                    field.rawSpec == "{title:es}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "ACwB resolved field");
        }

        const cg::packet::send::CustomTitleInputRequest request("title");
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *customTitleInputRequestSchema,
            cg::packet::send::CustomTitleInputRequest::Spec,
            "ACwB Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *customTitleInputRequestSchema,
            packet,
            "ACwB resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "ACwB"),
        "ACwB present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "ACwB"),
        "ACwB absent from receive typed manifest");

    const PacketSchema* characterLevelUpPointSchema =
        FindPacket(packets, "IHw");
    failures += Expect(
        characterLevelUpPointSchema != nullptr,
        "IHw schema exists");
    if (characterLevelUpPointSchema != nullptr) {
        failures += Expect(
            characterLevelUpPointSchema->direction == "客户端->服务端",
            "IHw direction");
        failures += Expect(
            characterLevelUpPointSchema->legacyKind == "RECV",
            "IHw legacyKind");
        failures += Expect(
            characterLevelUpPointSchema->clientNamespace == "send",
            "IHw namespace");
        failures += Expect(
            characterLevelUpPointSchema->xlsxStatus == "完成",
            "IHw xlsxStatus");
        failures += Expect(
            characterLevelUpPointSchema->rawFormat == "{sqeno}",
            "IHw rawFormat");
        failures += Expect(
            !characterLevelUpPointSchema->formatEmpty,
            "IHw formatEmpty false");
        failures += Expect(
            characterLevelUpPointSchema->source == "xlsx",
            "IHw raw source");
        failures += Expect(
            characterLevelUpPointSchema->fields.size() == 1U,
            "IHw raw field count");
        if (characterLevelUpPointSchema->fields.size() == 1U) {
            const FieldSchema& field =
                characterLevelUpPointSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "sqeno" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{sqeno}" &&
                    field.source == "xlsx",
                "IHw raw field");
        }

        failures += Expect(
            characterLevelUpPointSchema->hasResolvedFormat,
            "IHw resolvedFormat present");
        failures += Expect(
            characterLevelUpPointSchema->resolvedFormat == "{sqeno:i62}",
            "IHw resolvedFormat");
        failures += Expect(
            characterLevelUpPointSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "IHw resolvedSource");
        failures += Expect(
            characterLevelUpPointSchema->hasResolvedFields,
            "IHw resolvedFields present");
        failures += Expect(
            characterLevelUpPointSchema->resolvedFields.size() == 1U,
            "IHw resolved field count");
        if (characterLevelUpPointSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                characterLevelUpPointSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "sqeno" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{sqeno:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "IHw resolved field");
        }

        const cg::packet::send::CharacterLevelUpPoint request(4);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *characterLevelUpPointSchema,
            cg::packet::send::CharacterLevelUpPoint::Spec,
            "IHw Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterLevelUpPointSchema,
            packet,
            "IHw resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "IHw"),
        "IHw present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "IHw"),
        "IHw absent from receive typed manifest");

    const PacketSchema* characterSkillSwapSchema =
        FindPacket(packets, "Noeb");
    failures += Expect(
        characterSkillSwapSchema != nullptr,
        "Noeb schema exists");
    if (characterSkillSwapSchema != nullptr) {
        failures += Expect(
            characterSkillSwapSchema->direction == "客户端->服务端",
            "Noeb direction");
        failures += Expect(
            characterSkillSwapSchema->legacyKind == "RECV",
            "Noeb legacyKind");
        failures += Expect(
            characterSkillSwapSchema->clientNamespace == "send",
            "Noeb namespace");
        failures += Expect(
            characterSkillSwapSchema->xlsxStatus == "完成",
            "Noeb xlsxStatus");
        failures += Expect(
            characterSkillSwapSchema->rawFormat == "{from} {to}",
            "Noeb rawFormat");
        failures += Expect(
            !characterSkillSwapSchema->formatEmpty,
            "Noeb formatEmpty false");
        failures += Expect(
            characterSkillSwapSchema->source == "xlsx",
            "Noeb raw source");
        failures += Expect(
            characterSkillSwapSchema->fields.size() == 2U,
            "Noeb raw field count");
        if (characterSkillSwapSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{
                "from",
                "to",
            }};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{from}",
                "{to}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    characterSkillSwapSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "Noeb raw field");
            }
        }

        failures += Expect(
            characterSkillSwapSchema->hasResolvedFormat,
            "Noeb resolvedFormat present");
        failures += Expect(
            characterSkillSwapSchema->resolvedFormat ==
                "{from:i62} {to:i62}",
            "Noeb resolvedFormat");
        failures += Expect(
            characterSkillSwapSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "Noeb resolvedSource");
        failures += Expect(
            characterSkillSwapSchema->hasResolvedFields,
            "Noeb resolvedFields present");
        failures += Expect(
            characterSkillSwapSchema->resolvedFields.size() == 2U,
            "Noeb resolved field count");
        if (characterSkillSwapSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{
                "from",
                "to",
            }};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{from:i62}",
                "{to:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    characterSkillSwapSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "Noeb resolved field");
            }
        }

        const cg::packet::send::CharacterSkillSwap request(0, -1);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *characterSkillSwapSchema,
            cg::packet::send::CharacterSkillSwap::Spec,
            "Noeb Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterSkillSwapSchema,
            packet,
            "Noeb resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Noeb"),
        "Noeb present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Noeb"),
        "Noeb absent from receive typed manifest");

    const PacketSchema* characterDeleteRequestSchema =
        FindPacket(packets, "Godf");
    failures += Expect(
        characterDeleteRequestSchema != nullptr,
        "Godf schema exists");
    if (characterDeleteRequestSchema != nullptr) {
        failures += Expect(
            characterDeleteRequestSchema->direction == "客户端->服务端",
            "Godf direction");
        failures += Expect(
            characterDeleteRequestSchema->legacyKind == "RECV",
            "Godf legacyKind");
        failures += Expect(
            characterDeleteRequestSchema->clientNamespace == "send",
            "Godf namespace");
        failures += Expect(
            characterDeleteRequestSchema->xlsxStatus == "完成",
            "Godf xlsxStatus");
        failures += Expect(
            characterDeleteRequestSchema->rawFormat == "{registNumber}",
            "Godf rawFormat");
        failures += Expect(
            !characterDeleteRequestSchema->formatEmpty,
            "Godf formatEmpty false");
        failures += Expect(
            characterDeleteRequestSchema->source == "xlsx",
            "Godf raw source");
        failures += Expect(
            characterDeleteRequestSchema->fields.size() == 1U,
            "Godf raw field count");
        if (characterDeleteRequestSchema->fields.size() == 1U) {
            const FieldSchema& field =
                characterDeleteRequestSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "registNumber" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{registNumber}" &&
                    field.source == "xlsx",
                "Godf raw field");
        }

        failures += Expect(
            characterDeleteRequestSchema->hasResolvedFormat,
            "Godf resolvedFormat present");
        failures += Expect(
            characterDeleteRequestSchema->resolvedFormat ==
                "{registNumber:i62}",
            "Godf resolvedFormat");
        failures += Expect(
            characterDeleteRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "Godf resolvedSource");
        failures += Expect(
            characterDeleteRequestSchema->hasResolvedFields,
            "Godf resolvedFields present");
        failures += Expect(
            characterDeleteRequestSchema->resolvedFields.size() == 1U,
            "Godf resolved field count");
        if (characterDeleteRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                characterDeleteRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "registNumber" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{registNumber:i62}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "Godf resolved field");
        }

        const cg::packet::send::CharacterDeleteRequest request(10);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *characterDeleteRequestSchema,
            cg::packet::send::CharacterDeleteRequest::Spec,
            "Godf Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterDeleteRequestSchema,
            packet,
            "Godf resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Godf"),
        "Godf present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Godf"),
        "Godf absent from receive typed manifest");

    const PacketSchema* processStatusRequestSchema =
        FindPacket(packets, "yp");
    failures += Expect(
        processStatusRequestSchema != nullptr,
        "yp schema exists");
    if (processStatusRequestSchema != nullptr) {
        failures += Expect(
            processStatusRequestSchema->direction == "客户端->服务端",
            "yp direction");
        failures += Expect(
            processStatusRequestSchema->legacyKind == "RECV",
            "yp legacyKind");
        failures += Expect(
            processStatusRequestSchema->clientNamespace == "send",
            "yp namespace");
        failures += Expect(
            processStatusRequestSchema->xlsxStatus == "暂缓",
            "yp xlsxStatus");
        failures += Expect(
            processStatusRequestSchema->rawFormat.empty(),
            "yp rawFormat empty");
        failures += Expect(
            processStatusRequestSchema->formatEmpty,
            "yp formatEmpty true");
        failures += Expect(
            processStatusRequestSchema->source == "xlsx",
            "yp raw source");
        failures += Expect(
            processStatusRequestSchema->fields.empty(),
            "yp raw fields empty");
        failures += Expect(
            !processStatusRequestSchema->hasContractStatus,
            "yp contract status absent");
        failures += Expect(
            processStatusRequestSchema->hasExecutionStatus &&
                processStatusRequestSchema->executionStatus == "user-skipped",
            "yp user-skipped execution status");
        failures += Expect(
            !processStatusRequestSchema->hasResolvedFormat,
            "yp resolvedFormat absent");
        failures += Expect(
            processStatusRequestSchema->resolvedSource.empty(),
            "yp resolvedSource absent");
        failures += Expect(
            !processStatusRequestSchema->hasResolvedFields,
            "yp resolvedFields absent");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "yp"),
        "yp absent from send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "yp"),
        "yp absent from receive typed manifest");

    const PacketSchema* playerCountRequestSchema =
        FindPacket(packets, "rvb");
    failures += Expect(
        playerCountRequestSchema != nullptr,
        "rvb schema exists");
    if (playerCountRequestSchema != nullptr) {
        const cg::packet::send::PlayerCountRequest request;
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            playerCountRequestSchema->direction == "客户端->服务端",
            "rvb direction");
        failures += Expect(
            playerCountRequestSchema->legacyKind == "RECV",
            "rvb legacyKind");
        failures += Expect(
            playerCountRequestSchema->clientNamespace == "send",
            "rvb namespace");
        failures += Expect(
            playerCountRequestSchema->xlsxStatus == "暂缓",
            "rvb xlsxStatus");
        failures += Expect(
            playerCountRequestSchema->rawFormat.empty(),
            "rvb rawFormat empty");
        failures += Expect(
            playerCountRequestSchema->formatEmpty,
            "rvb formatEmpty true");
        failures += Expect(
            playerCountRequestSchema->source == "xlsx",
            "rvb raw source");
        failures += Expect(
            playerCountRequestSchema->fields.empty(),
            "rvb raw fields empty");
        failures += Expect(
            playerCountRequestSchema->hasResolvedFormat,
            "rvb resolvedFormat present");
        failures += Expect(
            playerCountRequestSchema->resolvedFormat.empty(),
            "rvb resolvedFormat empty");
        failures += Expect(
            playerCountRequestSchema->resolvedSource == "xlsx+ServerC+src/packet",
            "rvb resolvedSource");
        failures += Expect(
            playerCountRequestSchema->hasResolvedFields,
            "rvb resolvedFields present");
        failures += Expect(
            playerCountRequestSchema->resolvedFields.empty(),
            "rvb resolvedFields empty");
        failures += ExpectEffectiveFieldsMatch(
            *playerCountRequestSchema,
            cg::packet::send::PlayerCountRequest::Spec,
            "rvb Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *playerCountRequestSchema,
            packet,
            "rvb resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "rvb"),
        "rvb present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "rvb"),
        "rvb absent from receive typed manifest");

    const PacketSchema* playerCountResponseSchema =
        FindPacket(packets, "Bi");
    failures += Expect(
        playerCountResponseSchema != nullptr,
        "Bi schema exists");
    if (playerCountResponseSchema != nullptr) {
        failures += Expect(
            playerCountResponseSchema->direction == "服务端->客户端",
            "Bi direction");
        failures += Expect(
            playerCountResponseSchema->legacyKind == "SEND",
            "Bi legacyKind");
        failures += Expect(
            playerCountResponseSchema->clientNamespace == "receive",
            "Bi namespace");
        failures += Expect(
            playerCountResponseSchema->xlsxStatus == "暂缓",
            "Bi xlsxStatus");
        failures += Expect(
            playerCountResponseSchema->description ==
                "SEND_PlayerNumGet",
            "Bi description");
        failures += Expect(
            playerCountResponseSchema->rawFormat.empty(),
            "Bi rawFormat empty");
        failures += Expect(
            playerCountResponseSchema->formatEmpty,
            "Bi formatEmpty true");
        failures += Expect(
            playerCountResponseSchema->source == "xlsx",
            "Bi raw source");
        failures += Expect(
            playerCountResponseSchema->fields.empty(),
            "Bi raw fields empty");
        failures += Expect(
            playerCountResponseSchema->hasResolvedFormat,
            "Bi resolvedFormat present");
        failures += Expect(
            playerCountResponseSchema->resolvedFormat ==
                "{logincount:i62} {player:i62}",
            "Bi resolvedFormat");
        failures += Expect(
            playerCountResponseSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "Bi resolvedSource");
        failures += Expect(
            playerCountResponseSchema->hasResolvedFields,
            "Bi resolvedFields present");

        constexpr std::array<std::string_view, 2U> Names = {{
            "logincount",
            "player",
        }};
        constexpr std::array<std::string_view, 2U> Specs = {{
            "{logincount:i62}",
            "{player:i62}",
        }};
        failures += Expect(
            playerCountResponseSchema->resolvedFields.size() ==
                Names.size(),
            "Bi resolved field count");
        if (playerCountResponseSchema->resolvedFields.size() ==
            Names.size()) {
            for (std::size_t index = 0U; index < Names.size(); ++index) {
                const FieldSchema& field =
                    playerCountResponseSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == Names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == Specs[index] &&
                        field.source == "ServerC+ClientIda+src/packet",
                    "Bi resolved field order");
            }
        }

        const cg::packet::receive::PlayerCountResponse playerCountResponse(2, 1);
        const cg::packet::Packet packet = playerCountResponse.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *playerCountResponseSchema,
            cg::packet::receive::PlayerCountResponse::Spec,
            "Bi Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *playerCountResponseSchema,
            packet,
            "Bi resolved field order");
        failures += Expect(
            playerCountResponse.clientConnectionCount == 2 &&
                playerCountResponse.playerCount == 1,
            "Bi typed members");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Bi"),
        "Bi absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Bi"),
        "Bi present in receive typed manifest");

    const PacketSchema* versionUpgradeResultSchema =
        FindPacket(packets, "ed");
    failures += Expect(
        versionUpgradeResultSchema != nullptr,
        "ed schema exists");
    if (versionUpgradeResultSchema != nullptr) {
        failures += Expect(
            versionUpgradeResultSchema->direction == "服务端->客户端",
            "ed direction");
        failures += Expect(
            versionUpgradeResultSchema->legacyKind == "SEND",
            "ed legacyKind");
        failures += Expect(
            versionUpgradeResultSchema->clientNamespace == "receive",
            "ed namespace");
        failures += Expect(
            versionUpgradeResultSchema->xlsxStatus == "暂缓",
            "ed xlsxStatus");
        failures += Expect(
            versionUpgradeResultSchema->rawFormat.empty(),
            "ed rawFormat empty");
        failures += Expect(
            versionUpgradeResultSchema->formatEmpty,
            "ed formatEmpty true");
        failures += Expect(
            versionUpgradeResultSchema->source == "xlsx",
            "ed raw source");
        failures += Expect(
            versionUpgradeResultSchema->fields.empty(),
            "ed raw fields empty");
        failures += Expect(
            versionUpgradeResultSchema->hasResolvedFormat,
            "ed resolvedFormat present");
        failures += Expect(
            versionUpgradeResultSchema->resolvedFormat == "{result:i62}",
            "ed resolvedFormat");
        failures += Expect(
            versionUpgradeResultSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "ed resolvedSource");
        failures += Expect(
            versionUpgradeResultSchema->hasResolvedFields,
            "ed resolvedFields present");
        failures += Expect(
            versionUpgradeResultSchema->resolvedFields.size() == 1U,
            "ed resolved field count");
        if (versionUpgradeResultSchema->resolvedFields.size() == 1U) {
            const FieldSchema& result =
                versionUpgradeResultSchema->resolvedFields[0];
            failures += Expect(
                result.index == 0 &&
                    result.name == "result" &&
                    result.typeToken == "i62" &&
                    result.rawSpec == "{result:i62}" &&
                    result.source == "ServerC+ClientIda+src/packet",
                "ed resolved result field");
        }

        const cg::packet::receive::VersionUpgradeResult result(0);
        const cg::packet::Packet packet = result.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *versionUpgradeResultSchema,
            cg::packet::receive::VersionUpgradeResult::Spec,
            "ed Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *versionUpgradeResultSchema,
            packet,
            "ed resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "ed"),
        "ed absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "ed"),
        "ed present in receive typed manifest");

    const PacketSchema* expireSchema = FindPacket(packets, "Expire");
    failures += Expect(expireSchema != nullptr, "Expire schema exists");
    if (expireSchema != nullptr) {
        failures += Expect(
            expireSchema->direction == "服务端->客户端",
            "Expire direction");
        failures += Expect(
            expireSchema->legacyKind == "SEND",
            "Expire legacyKind");
        failures += Expect(
            expireSchema->clientNamespace == "receive",
            "Expire namespace");
        failures += Expect(
            expireSchema->xlsxStatus == "暂缓",
            "Expire xlsxStatus");
        failures += Expect(
            expireSchema->rawFormat.empty(),
            "Expire rawFormat empty");
        failures += Expect(
            expireSchema->formatEmpty,
            "Expire formatEmpty true");
        failures += Expect(
            expireSchema->source == "xlsx",
            "Expire raw source");
        failures += Expect(
            expireSchema->fields.empty(),
            "Expire raw fields empty");
        failures += Expect(
            expireSchema->contractStatus == "no-effective-wire-contract",
            "Expire no-effective-wire-contract sentinel");
        failures += Expect(
            expireSchema->resolvedSource == "xlsx+ServerC",
            "Expire resolvedSource");
        failures += Expect(
            !expireSchema->hasResolvedFormat,
            "Expire resolvedFormat absent");
        failures += Expect(
            !expireSchema->hasResolvedFields,
            "Expire resolvedFields absent");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Expire"),
        "Expire absent from send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Expire"),
        "Expire absent from receive typed manifest");

    const PacketSchema* stallStartResultSchema =
        FindPacket(packets, "Tmrx");
    failures += Expect(
        stallStartResultSchema != nullptr,
        "Tmrx schema exists");
    if (stallStartResultSchema != nullptr) {
        failures += Expect(
            stallStartResultSchema->direction == "服务端->客户端",
            "Tmrx direction");
        failures += Expect(
            stallStartResultSchema->legacyKind == "SEND",
            "Tmrx legacyKind");
        failures += Expect(
            stallStartResultSchema->clientNamespace == "receive",
            "Tmrx namespace");
        failures += Expect(
            stallStartResultSchema->xlsxStatus == "完成",
            "Tmrx xlsxStatus");
        failures += Expect(
            stallStartResultSchema->rawFormat == "{result}",
            "Tmrx rawFormat");
        failures += Expect(
            !stallStartResultSchema->formatEmpty,
            "Tmrx formatEmpty false");
        failures += Expect(
            stallStartResultSchema->source == "xlsx",
            "Tmrx raw source");
        failures += Expect(
            stallStartResultSchema->fields.size() == 1U,
            "Tmrx raw field count");
        if (stallStartResultSchema->fields.size() == 1U) {
            const FieldSchema& result =
                stallStartResultSchema->fields[0];
            failures += Expect(
                result.index == 0 &&
                    result.name == "result" &&
                    result.typeToken == "unknown" &&
                    result.rawSpec == "{result}" &&
                    result.source == "xlsx",
                "Tmrx raw result field");
        }

        failures += Expect(
            stallStartResultSchema->hasResolvedFormat,
            "Tmrx resolvedFormat present");
        failures += Expect(
            stallStartResultSchema->resolvedFormat == "{result:i62}",
            "Tmrx resolvedFormat");
        failures += Expect(
            stallStartResultSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "Tmrx resolvedSource");
        failures += Expect(
            stallStartResultSchema->hasResolvedFields,
            "Tmrx resolvedFields present");
        failures += Expect(
            stallStartResultSchema->resolvedFields.size() == 1U,
            "Tmrx resolved field count");
        if (stallStartResultSchema->resolvedFields.size() == 1U) {
            const FieldSchema& result =
                stallStartResultSchema->resolvedFields[0];
            failures += Expect(
                result.index == 0 &&
                    result.name == "result" &&
                    result.typeToken == "i62" &&
                    result.rawSpec == "{result:i62}" &&
                    result.source == "xlsx+ServerC+ClientIda+src/packet",
                "Tmrx resolved result field");
        }

        const cg::packet::receive::StallStartResult result(0);
        const cg::packet::Packet packet = result.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *stallStartResultSchema,
            cg::packet::receive::StallStartResult::Spec,
            "Tmrx Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *stallStartResultSchema,
            packet,
            "Tmrx resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Tmrx"),
        "Tmrx absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Tmrx"),
        "Tmrx present in receive typed manifest");

    const PacketSchema* stallEndResultSchema =
        FindPacket(packets, "KXhH");
    failures += Expect(
        stallEndResultSchema != nullptr,
        "KXhH schema exists");
    if (stallEndResultSchema != nullptr) {
        failures += Expect(
            stallEndResultSchema->direction == "服务端->客户端",
            "KXhH direction");
        failures += Expect(
            stallEndResultSchema->legacyKind == "SEND",
            "KXhH legacyKind");
        failures += Expect(
            stallEndResultSchema->clientNamespace == "receive",
            "KXhH namespace");
        failures += Expect(
            stallEndResultSchema->xlsxStatus == "完成",
            "KXhH xlsxStatus");
        failures += Expect(
            stallEndResultSchema->rawFormat == "{result}",
            "KXhH rawFormat");
        failures += Expect(
            !stallEndResultSchema->formatEmpty,
            "KXhH formatEmpty false");
        failures += Expect(
            stallEndResultSchema->source == "xlsx",
            "KXhH raw source");
        failures += Expect(
            stallEndResultSchema->fields.size() == 1U,
            "KXhH raw field count");
        if (stallEndResultSchema->fields.size() == 1U) {
            const FieldSchema& result =
                stallEndResultSchema->fields[0];
            failures += Expect(
                result.index == 0 &&
                    result.name == "result" &&
                    result.typeToken == "unknown" &&
                    result.rawSpec == "{result}" &&
                    result.source == "xlsx",
                "KXhH raw result field");
        }

        failures += Expect(
            stallEndResultSchema->hasResolvedFormat,
            "KXhH resolvedFormat present");
        failures += Expect(
            stallEndResultSchema->resolvedFormat == "{result:i62}",
            "KXhH resolvedFormat");
        failures += Expect(
            stallEndResultSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "KXhH resolvedSource");
        failures += Expect(
            stallEndResultSchema->hasResolvedFields,
            "KXhH resolvedFields present");
        failures += Expect(
            stallEndResultSchema->resolvedFields.size() == 1U,
            "KXhH resolved field count");
        if (stallEndResultSchema->resolvedFields.size() == 1U) {
            const FieldSchema& result =
                stallEndResultSchema->resolvedFields[0];
            failures += Expect(
                result.index == 0 &&
                    result.name == "result" &&
                    result.typeToken == "i62" &&
                    result.rawSpec == "{result:i62}" &&
                    result.source == "xlsx+ServerC+ClientIda+src/packet",
                "KXhH resolved result field");
        }

        const cg::packet::receive::StallEndResult result(0);
        const cg::packet::Packet packet = result.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *stallEndResultSchema,
            cg::packet::receive::StallEndResult::Spec,
            "KXhH Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *stallEndResultSchema,
            packet,
            "KXhH resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "KXhH"),
        "KXhH absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "KXhH"),
        "KXhH present in receive typed manifest");

    const PacketSchema* stallBuyResultSchema =
        FindPacket(packets, "Yx");
    failures += Expect(
        stallBuyResultSchema != nullptr,
        "Yx schema exists");
    if (stallBuyResultSchema != nullptr) {
        failures += Expect(
            stallBuyResultSchema->direction == "服务端->客户端",
            "Yx direction");
        failures += Expect(
            stallBuyResultSchema->legacyKind == "SEND",
            "Yx legacyKind");
        failures += Expect(
            stallBuyResultSchema->clientNamespace == "receive",
            "Yx namespace");
        failures += Expect(
            stallBuyResultSchema->xlsxStatus == "完成",
            "Yx xlsxStatus");
        failures += Expect(
            stallBuyResultSchema->rawFormat == "{result}",
            "Yx rawFormat");
        failures += Expect(
            !stallBuyResultSchema->formatEmpty,
            "Yx formatEmpty false");
        failures += Expect(
            stallBuyResultSchema->source == "xlsx",
            "Yx raw source");
        failures += Expect(
            stallBuyResultSchema->fields.size() == 1U,
            "Yx raw field count");
        if (stallBuyResultSchema->fields.size() == 1U) {
            const FieldSchema& result =
                stallBuyResultSchema->fields[0];
            failures += Expect(
                result.index == 0 &&
                    result.name == "result" &&
                    result.typeToken == "unknown" &&
                    result.rawSpec == "{result}" &&
                    result.source == "xlsx",
                "Yx raw result field");
        }

        failures += Expect(
            stallBuyResultSchema->hasResolvedFormat,
            "Yx resolvedFormat present");
        failures += Expect(
            stallBuyResultSchema->resolvedFormat == "{result:i62}",
            "Yx resolvedFormat");
        failures += Expect(
            stallBuyResultSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "Yx resolvedSource");
        failures += Expect(
            stallBuyResultSchema->hasResolvedFields,
            "Yx resolvedFields present");
        failures += Expect(
            stallBuyResultSchema->resolvedFields.size() == 1U,
            "Yx resolved field count");
        if (stallBuyResultSchema->resolvedFields.size() == 1U) {
            const FieldSchema& result =
                stallBuyResultSchema->resolvedFields[0];
            failures += Expect(
                result.index == 0 &&
                    result.name == "result" &&
                    result.typeToken == "i62" &&
                    result.rawSpec == "{result:i62}" &&
                    result.source == "xlsx+ServerC+ClientIda+src/packet",
                "Yx resolved result field order");
        }

        const cg::packet::receive::StallBuyResult result(0);
        const cg::packet::Packet packet = result.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *stallBuyResultSchema,
            cg::packet::receive::StallBuyResult::Spec,
            "Yx Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *stallBuyResultSchema,
            packet,
            "Yx resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Yx"),
        "Yx absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Yx"),
        "Yx present in receive typed manifest");

    const PacketSchema* petItemSwapSchema =
        FindPacket(packets, "kiG");
    failures += Expect(
        petItemSwapSchema != nullptr,
        "kiG schema exists");
    if (petItemSwapSchema != nullptr) {
        failures += Expect(
            petItemSwapSchema->direction == "服务端->客户端",
            "kiG direction");
        failures += Expect(
            petItemSwapSchema->legacyKind == "SEND",
            "kiG legacyKind");
        failures += Expect(
            petItemSwapSchema->clientNamespace == "receive",
            "kiG namespace");
        failures += Expect(
            petItemSwapSchema->xlsxStatus == "待办",
            "kiG xlsxStatus");
        failures += Expect(
            petItemSwapSchema->rawFormat ==
                "{fromChar} {fromSlot} {toChar} {toSlot}",
            "kiG rawFormat");
        failures += Expect(
            !petItemSwapSchema->formatEmpty,
            "kiG formatEmpty false");
        failures += Expect(
            petItemSwapSchema->source == "xlsx",
            "kiG raw source");
        failures += Expect(
            petItemSwapSchema->fields.size() == 4U,
            "kiG raw field count");
        if (petItemSwapSchema->fields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "fromChar",
                "fromSlot",
                "toChar",
                "toSlot",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{fromChar}",
                "{fromSlot}",
                "{toChar}",
                "{toSlot}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    petItemSwapSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "kiG raw field order");
            }
        }

        failures += Expect(
            petItemSwapSchema->hasResolvedFormat,
            "kiG resolvedFormat present");
        failures += Expect(
            petItemSwapSchema->resolvedFormat ==
                "{fromChar:i62} {fromSlot:i62} {toChar:i62} {toSlot:i62}",
            "kiG resolvedFormat");
        failures += Expect(
            petItemSwapSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "kiG resolvedSource");
        failures += Expect(
            petItemSwapSchema->hasResolvedFields,
            "kiG resolvedFields present");
        failures += Expect(
            petItemSwapSchema->resolvedFields.size() == 4U,
            "kiG resolved field count");
        if (petItemSwapSchema->resolvedFields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "fromChar",
                "fromSlot",
                "toChar",
                "toSlot",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{fromChar:i62}",
                "{fromSlot:i62}",
                "{toChar:i62}",
                "{toSlot:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    petItemSwapSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "kiG resolved field order");
            }
        }

        const cg::packet::receive::PetItemSwap swap(0, 27, 2, 5);
        const cg::packet::Packet packet = swap.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *petItemSwapSchema,
            cg::packet::receive::PetItemSwap::Spec,
            "kiG Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petItemSwapSchema,
            packet,
            "kiG resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "kiG"),
        "kiG absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "kiG"),
        "kiG present in receive typed manifest");

    const PacketSchema* cheatDetectionReportSchema =
        FindPacket(packets, "PGXE");
    failures += Expect(
        cheatDetectionReportSchema != nullptr,
        "PGXE schema exists");
    if (cheatDetectionReportSchema != nullptr) {
        const cg::packet::send::CheatDetectionReport report(1);
        failures += Expect(
            cheatDetectionReportSchema->direction == "客户端->服务端",
            "PGXE direction");
        failures += Expect(
            cheatDetectionReportSchema->legacyKind == "RECV",
            "PGXE legacyKind");
        failures += Expect(
            cheatDetectionReportSchema->clientNamespace == "send",
            "PGXE namespace");
        failures += Expect(
            cheatDetectionReportSchema->xlsxStatus == "完成",
            "PGXE xlsxStatus");
        failures += Expect(
            cheatDetectionReportSchema->rawFormat == "{type}",
            "PGXE rawFormat");
        failures += Expect(
            !cheatDetectionReportSchema->formatEmpty,
            "PGXE formatEmpty false");
        failures += Expect(
            cheatDetectionReportSchema->source == "xlsx",
            "PGXE raw source");
        failures += Expect(
            cheatDetectionReportSchema->fields.size() == 1U,
            "PGXE raw field count");
        if (cheatDetectionReportSchema->fields.size() == 1U) {
            const FieldSchema& type =
                cheatDetectionReportSchema->fields[0];
            failures += Expect(
                type.index == 0 &&
                    type.name == "type" &&
                    type.typeToken == "unknown" &&
                    type.rawSpec == "{type}" &&
                    type.source == "xlsx",
                "PGXE raw type field");
        }

        failures += Expect(
            cheatDetectionReportSchema->hasResolvedFormat,
            "PGXE resolvedFormat present");
        failures += Expect(
            cheatDetectionReportSchema->resolvedFormat == "{type:i62}",
            "PGXE resolvedFormat");
        failures += Expect(
            cheatDetectionReportSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "PGXE resolvedSource");
        failures += Expect(
            cheatDetectionReportSchema->hasResolvedFields,
            "PGXE resolvedFields present");
        failures += Expect(
            cheatDetectionReportSchema->resolvedFields.size() == 1U,
            "PGXE resolved field count");
        if (cheatDetectionReportSchema->resolvedFields.size() == 1U) {
            const FieldSchema& type =
                cheatDetectionReportSchema->resolvedFields[0];
            failures += Expect(
                type.index == 0 &&
                    type.name == "type" &&
                    type.typeToken == "i62" &&
                    type.rawSpec == "{type:i62}" &&
                    type.source == "xlsx+ServerC+ClientIda+src/packet",
                "PGXE resolved type field order");
        }
        failures += ExpectEffectiveFieldsMatch(
            *cheatDetectionReportSchema,
            cg::packet::send::CheatDetectionReport::Spec,
            "PGXE Spec matches effective fields");
        failures += Expect(
            report.type == 1,
            "PGXE typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "PGXE"),
        "PGXE present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "PGXE"),
        "PGXE absent from receive typed manifest");

    const PacketSchema* stallBrowseRequestSchema =
        FindPacket(packets, "ElVN");
    failures += Expect(
        stallBrowseRequestSchema != nullptr,
        "ElVN schema exists");
    if (stallBrowseRequestSchema != nullptr) {
        const cg::packet::send::StallBrowseRequest request(10);
        failures += Expect(
            stallBrowseRequestSchema->direction == "客户端->服务端",
            "ElVN direction");
        failures += Expect(
            stallBrowseRequestSchema->legacyKind == "RECV",
            "ElVN legacyKind");
        failures += Expect(
            stallBrowseRequestSchema->clientNamespace == "send",
            "ElVN namespace");
        failures += Expect(
            stallBrowseRequestSchema->xlsxStatus == "完成",
            "ElVN xlsxStatus");
        failures += Expect(
            stallBrowseRequestSchema->rawFormat == "{objIndex}",
            "ElVN rawFormat");
        failures += Expect(
            !stallBrowseRequestSchema->formatEmpty,
            "ElVN formatEmpty false");
        failures += Expect(
            stallBrowseRequestSchema->source == "xlsx",
            "ElVN raw source");
        failures += Expect(
            stallBrowseRequestSchema->fields.size() == 1U,
            "ElVN raw field count");
        if (stallBrowseRequestSchema->fields.size() == 1U) {
            const FieldSchema& objIndex =
                stallBrowseRequestSchema->fields[0];
            failures += Expect(
                objIndex.index == 0 &&
                    objIndex.name == "objIndex" &&
                    objIndex.typeToken == "unknown" &&
                    objIndex.rawSpec == "{objIndex}" &&
                    objIndex.source == "xlsx",
                "ElVN raw objIndex field");
        }

        failures += Expect(
            stallBrowseRequestSchema->hasResolvedFormat,
            "ElVN resolvedFormat present");
        failures += Expect(
            stallBrowseRequestSchema->resolvedFormat ==
                "{objIndex:i62}",
            "ElVN resolvedFormat");
        failures += Expect(
            stallBrowseRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "ElVN resolvedSource");
        failures += Expect(
            stallBrowseRequestSchema->hasResolvedFields,
            "ElVN resolvedFields present");
        failures += Expect(
            stallBrowseRequestSchema->resolvedFields.size() == 1U,
            "ElVN resolved field count");
        if (stallBrowseRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& objIndex =
                stallBrowseRequestSchema->resolvedFields[0];
            failures += Expect(
                objIndex.index == 0 &&
                    objIndex.name == "objIndex" &&
                    objIndex.typeToken == "i62" &&
                    objIndex.rawSpec == "{objIndex:i62}" &&
                    objIndex.source == "xlsx+ServerC+ClientIda+src/packet",
                "ElVN resolved objIndex field order");
        }
        failures += ExpectEffectiveFieldsMatch(
            *stallBrowseRequestSchema,
            cg::packet::send::StallBrowseRequest::Spec,
            "ElVN Spec matches effective fields");
        failures += Expect(
            request.objectIndex == 10,
            "ElVN typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "ElVN"),
        "ElVN present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "ElVN"),
        "ElVN absent from receive typed manifest");

    const PacketSchema* stallBuyRequestSchema =
        FindPacket(packets, "wAo");
    failures += Expect(
        stallBuyRequestSchema != nullptr,
        "wAo schema exists");
    if (stallBuyRequestSchema != nullptr) {
        const cg::packet::send::StallBuyRequest request(8);
        failures += Expect(
            stallBuyRequestSchema->direction == "客户端->服务端",
            "wAo direction");
        failures += Expect(
            stallBuyRequestSchema->legacyKind == "RECV",
            "wAo legacyKind");
        failures += Expect(
            stallBuyRequestSchema->clientNamespace == "send",
            "wAo namespace");
        failures += Expect(
            stallBuyRequestSchema->xlsxStatus == "完成",
            "wAo xlsxStatus");
        failures += Expect(
            stallBuyRequestSchema->rawFormat == "{slot}",
            "wAo rawFormat");
        failures += Expect(
            !stallBuyRequestSchema->formatEmpty,
            "wAo formatEmpty false");
        failures += Expect(
            stallBuyRequestSchema->source == "xlsx",
            "wAo raw source");
        failures += Expect(
            stallBuyRequestSchema->fields.size() == 1U,
            "wAo raw field count");
        if (stallBuyRequestSchema->fields.size() == 1U) {
            const FieldSchema& slot = stallBuyRequestSchema->fields[0];
            failures += Expect(
                slot.index == 0 &&
                    slot.name == "slot" &&
                    slot.typeToken == "unknown" &&
                    slot.rawSpec == "{slot}" &&
                    slot.source == "xlsx",
                "wAo raw slot field");
        }

        failures += Expect(
            stallBuyRequestSchema->hasResolvedFormat,
            "wAo resolvedFormat present");
        failures += Expect(
            stallBuyRequestSchema->resolvedFormat == "{position:i62}",
            "wAo resolvedFormat");
        failures += Expect(
            stallBuyRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "wAo resolvedSource");
        failures += Expect(
            stallBuyRequestSchema->hasResolvedFields,
            "wAo resolvedFields present");
        failures += Expect(
            stallBuyRequestSchema->resolvedFields.size() == 1U,
            "wAo resolved field count");
        if (stallBuyRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& position =
                stallBuyRequestSchema->resolvedFields[0];
            failures += Expect(
                position.index == 0 &&
                    position.name == "position" &&
                    position.typeToken == "i62" &&
                    position.rawSpec == "{position:i62}" &&
                    position.source == "xlsx+ServerC+ClientIda+src/packet",
                "wAo resolved position field order");
        }
        failures += ExpectEffectiveFieldsMatch(
            *stallBuyRequestSchema,
            cg::packet::send::StallBuyRequest::Spec,
            "wAo Spec matches effective fields");
        failures += Expect(
            request.position == 8,
            "wAo typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "wAo"),
        "wAo present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "wAo"),
        "wAo absent from receive typed manifest");

    const PacketSchema* produceTimeRequestSchema =
        FindPacket(packets, "IPy");
    failures += Expect(
        produceTimeRequestSchema != nullptr,
        "IPy schema exists");
    if (produceTimeRequestSchema != nullptr) {
        const cg::packet::send::ProduceTimeRequest request(10);
        failures += Expect(
            produceTimeRequestSchema->direction == "客户端->服务端",
            "IPy direction");
        failures += Expect(
            produceTimeRequestSchema->legacyKind == "RECV",
            "IPy legacyKind");
        failures += Expect(
            produceTimeRequestSchema->clientNamespace == "send",
            "IPy namespace");
        failures += Expect(
            produceTimeRequestSchema->xlsxStatus == "完成",
            "IPy xlsxStatus");
        failures += Expect(
            produceTimeRequestSchema->rawFormat == "{time(s)}",
            "IPy rawFormat");
        failures += Expect(
            !produceTimeRequestSchema->formatEmpty,
            "IPy formatEmpty false");
        failures += Expect(
            produceTimeRequestSchema->source == "xlsx",
            "IPy raw source");
        failures += Expect(
            produceTimeRequestSchema->fields.size() == 1U,
            "IPy raw field count");
        if (produceTimeRequestSchema->fields.size() == 1U) {
            const FieldSchema& time =
                produceTimeRequestSchema->fields[0];
            failures += Expect(
                time.index == 0 &&
                    time.name == "time(s)" &&
                    time.typeToken == "unknown" &&
                    time.rawSpec == "{time(s)}" &&
                    time.source == "xlsx",
                "IPy raw time field");
        }

        failures += Expect(
            produceTimeRequestSchema->hasResolvedFormat,
            "IPy resolvedFormat present");
        failures += Expect(
            produceTimeRequestSchema->resolvedFormat == "{time:i62}",
            "IPy resolvedFormat");
        failures += Expect(
            produceTimeRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "IPy resolvedSource");
        failures += Expect(
            produceTimeRequestSchema->hasResolvedFields,
            "IPy resolvedFields present");
        failures += Expect(
            produceTimeRequestSchema->resolvedFields.size() == 1U,
            "IPy resolved field count");
        if (produceTimeRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& time =
                produceTimeRequestSchema->resolvedFields[0];
            failures += Expect(
                time.index == 0 &&
                    time.name == "time" &&
                    time.typeToken == "i62" &&
                    time.rawSpec == "{time:i62}" &&
                    time.source == "xlsx+ServerC+ClientIda+src/packet",
                "IPy resolved time field order");
        }
        failures += ExpectEffectiveFieldsMatch(
            *produceTimeRequestSchema,
            cg::packet::send::ProduceTimeRequest::Spec,
            "IPy Spec matches effective fields");
        failures += Expect(
            request.timeSeconds == 10,
            "IPy typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "IPy"),
        "IPy present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "IPy"),
        "IPy absent from receive typed manifest");

    const PacketSchema* bankPageRequestSchema =
        FindPacket(packets, "rh");
    failures += Expect(
        bankPageRequestSchema != nullptr,
        "rh schema exists");
    if (bankPageRequestSchema != nullptr) {
        const cg::packet::send::BankPageRequest request(2);
        failures += Expect(
            bankPageRequestSchema->direction == "客户端->服务端",
            "rh direction");
        failures += Expect(
            bankPageRequestSchema->legacyKind == "RECV",
            "rh legacyKind");
        failures += Expect(
            bankPageRequestSchema->clientNamespace == "send",
            "rh namespace");
        failures += Expect(
            bankPageRequestSchema->xlsxStatus == "完成",
            "rh xlsxStatus");
        failures += Expect(
            bankPageRequestSchema->rawFormat == "{page}",
            "rh rawFormat");
        failures += Expect(
            !bankPageRequestSchema->formatEmpty,
            "rh formatEmpty false");
        failures += Expect(
            bankPageRequestSchema->source == "xlsx",
            "rh raw source");
        failures += Expect(
            bankPageRequestSchema->fields.size() == 1U,
            "rh raw field count");
        if (bankPageRequestSchema->fields.size() == 1U) {
            const FieldSchema& page = bankPageRequestSchema->fields[0];
            failures += Expect(
                page.index == 0 &&
                    page.name == "page" &&
                    page.typeToken == "unknown" &&
                    page.rawSpec == "{page}" &&
                    page.source == "xlsx",
                "rh raw page field");
        }

        failures += Expect(
            bankPageRequestSchema->hasResolvedFormat,
            "rh resolvedFormat present");
        failures += Expect(
            bankPageRequestSchema->resolvedFormat == "{page:i62}",
            "rh resolvedFormat");
        failures += Expect(
            bankPageRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "rh resolvedSource");
        failures += Expect(
            bankPageRequestSchema->hasResolvedFields,
            "rh resolvedFields present");
        failures += Expect(
            bankPageRequestSchema->resolvedFields.size() == 1U,
            "rh resolved field count");
        if (bankPageRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& page =
                bankPageRequestSchema->resolvedFields[0];
            failures += Expect(
                page.index == 0 &&
                    page.name == "page" &&
                    page.typeToken == "i62" &&
                    page.rawSpec == "{page:i62}" &&
                    page.source == "xlsx+ServerC+ClientIda+src/packet",
                "rh resolved page field order");
        }
        failures += ExpectEffectiveFieldsMatch(
            *bankPageRequestSchema,
            cg::packet::send::BankPageRequest::Spec,
            "rh Spec matches effective fields");
        failures += Expect(request.page == 2, "rh typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "rh"),
        "rh present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "rh"),
        "rh absent from receive typed manifest");

    const PacketSchema* petItemMoveRequestSchema =
        FindPacket(packets, "Cpa");
    failures += Expect(
        petItemMoveRequestSchema != nullptr,
        "Cpa schema exists");
    if (petItemMoveRequestSchema != nullptr) {
        const cg::packet::send::PetItemMoveRequest request(
            0,
            9,
            2,
            1,
            -1);
        failures += Expect(
            petItemMoveRequestSchema->direction == "客户端->服务端",
            "Cpa direction");
        failures += Expect(
            petItemMoveRequestSchema->legacyKind == "RECV",
            "Cpa legacyKind");
        failures += Expect(
            petItemMoveRequestSchema->clientNamespace == "send",
            "Cpa namespace");
        failures += Expect(
            petItemMoveRequestSchema->xlsxStatus == "完成",
            "Cpa xlsxStatus");
        failures += Expect(
            petItemMoveRequestSchema->rawFormat ==
                "{fromChar} {fromItem} {toChar} {toItem} {pile}",
            "Cpa rawFormat");
        failures += Expect(
            !petItemMoveRequestSchema->formatEmpty,
            "Cpa formatEmpty false");
        failures += Expect(
            petItemMoveRequestSchema->source == "xlsx",
            "Cpa raw source");

        constexpr std::array<std::string_view, 5U> RawNames = {{
            "fromChar",
            "fromItem",
            "toChar",
            "toItem",
            "pile",
        }};
        constexpr std::array<std::string_view, 5U> RawSpecs = {{
            "{fromChar}",
            "{fromItem}",
            "{toChar}",
            "{toItem}",
            "{pile}",
        }};
        failures += Expect(
            petItemMoveRequestSchema->fields.size() == RawNames.size(),
            "Cpa raw field count");
        if (petItemMoveRequestSchema->fields.size() == RawNames.size()) {
            for (std::size_t index = 0U; index < RawNames.size(); ++index) {
                const FieldSchema& field =
                    petItemMoveRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == RawNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == RawSpecs[index] &&
                        field.source == "xlsx",
                    "Cpa raw field order");
            }
        }

        failures += Expect(
            petItemMoveRequestSchema->hasResolvedFormat,
            "Cpa resolvedFormat present");
        failures += Expect(
            petItemMoveRequestSchema->resolvedFormat ==
                "{petSlotFrom:i62} {itemSlotFrom:i62} {petSlotTo:i62} {itemSlotTo:i62} {pile:i62}",
            "Cpa resolvedFormat");
        failures += Expect(
            petItemMoveRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "Cpa resolvedSource");
        failures += Expect(
            petItemMoveRequestSchema->hasResolvedFields,
            "Cpa resolvedFields present");

        constexpr std::array<std::string_view, 5U> ResolvedNames = {{
            "petSlotFrom",
            "itemSlotFrom",
            "petSlotTo",
            "itemSlotTo",
            "pile",
        }};
        constexpr std::array<std::string_view, 5U> ResolvedSpecs = {{
            "{petSlotFrom:i62}",
            "{itemSlotFrom:i62}",
            "{petSlotTo:i62}",
            "{itemSlotTo:i62}",
            "{pile:i62}",
        }};
        failures += Expect(
            petItemMoveRequestSchema->resolvedFields.size() ==
                ResolvedNames.size(),
            "Cpa resolved field count");
        if (petItemMoveRequestSchema->resolvedFields.size() ==
            ResolvedNames.size()) {
            for (std::size_t index = 0U;
                 index < ResolvedNames.size();
                 ++index) {
                const FieldSchema& field =
                    petItemMoveRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == ResolvedNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == ResolvedSpecs[index] &&
                        field.source ==
                            "xlsx+ServerC+ClientIda+src/packet",
                    "Cpa resolved field order");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *petItemMoveRequestSchema,
            cg::packet::send::PetItemMoveRequest::Spec,
            "Cpa Spec matches effective fields");
        failures += Expect(
            request.petSlotFrom == 0 &&
                request.itemSlotFrom == 9 &&
                request.petSlotTo == 2 &&
                request.itemSlotTo == 1 &&
                request.pile == -1,
            "Cpa typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Cpa"),
        "Cpa present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Cpa"),
        "Cpa absent from receive typed manifest");

    const PacketSchema* bagPageRequestSchema =
        FindPacket(packets, "SWITM");
    failures += Expect(
        bagPageRequestSchema != nullptr,
        "SWITM schema exists");
    if (bagPageRequestSchema != nullptr) {
        const cg::packet::send::BagPageRequest request(1);
        failures += Expect(
            bagPageRequestSchema->direction == "客户端->服务端",
            "SWITM direction");
        failures += Expect(
            bagPageRequestSchema->legacyKind == "RECV",
            "SWITM legacyKind");
        failures += Expect(
            bagPageRequestSchema->clientNamespace == "send",
            "SWITM namespace");
        failures += Expect(
            bagPageRequestSchema->xlsxStatus == "完成",
            "SWITM xlsxStatus");
        failures += Expect(
            bagPageRequestSchema->rawFormat == "{page}",
            "SWITM rawFormat");
        failures += Expect(
            !bagPageRequestSchema->formatEmpty,
            "SWITM formatEmpty false");
        failures += Expect(
            bagPageRequestSchema->source == "xlsx",
            "SWITM raw source");
        failures += Expect(
            bagPageRequestSchema->fields.size() == 1U,
            "SWITM raw field count");
        if (bagPageRequestSchema->fields.size() == 1U) {
            const FieldSchema& page = bagPageRequestSchema->fields[0];
            failures += Expect(
                page.index == 0 &&
                    page.name == "page" &&
                    page.typeToken == "unknown" &&
                    page.rawSpec == "{page}" &&
                    page.source == "xlsx",
                "SWITM raw page field");
        }

        failures += Expect(
            bagPageRequestSchema->hasResolvedFormat,
            "SWITM resolvedFormat present");
        failures += Expect(
            bagPageRequestSchema->resolvedFormat == "{page:i62}",
            "SWITM resolvedFormat");
        failures += Expect(
            bagPageRequestSchema->resolvedSource ==
                "xlsx+ServerC+src/packet",
            "SWITM resolvedSource");
        failures += Expect(
            bagPageRequestSchema->hasResolvedFields,
            "SWITM resolvedFields present");
        failures += Expect(
            bagPageRequestSchema->resolvedFields.size() == 1U,
            "SWITM resolved field count");
        if (bagPageRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& page =
                bagPageRequestSchema->resolvedFields[0];
            failures += Expect(
                page.index == 0 &&
                    page.name == "page" &&
                    page.typeToken == "i62" &&
                    page.rawSpec == "{page:i62}" &&
                    page.source == "xlsx+ServerC+src/packet",
                "SWITM resolved page field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *bagPageRequestSchema,
            cg::packet::send::BagPageRequest::Spec,
            "SWITM Spec matches effective fields");
        failures += Expect(request.page == 1, "SWITM typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "SWITM"),
        "SWITM present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "SWITM"),
        "SWITM absent from receive typed manifest");

    const PacketSchema* bagPageMoveRequestSchema =
        FindPacket(packets, "SWITMP");
    failures += Expect(
        bagPageMoveRequestSchema != nullptr,
        "SWITMP schema exists");
    if (bagPageMoveRequestSchema != nullptr) {
        const cg::packet::send::BagPageMoveRequest request(10, 1, -1);
        failures += Expect(
            bagPageMoveRequestSchema->direction == "客户端->服务端",
            "SWITMP direction");
        failures += Expect(
            bagPageMoveRequestSchema->legacyKind == "RECV",
            "SWITMP legacyKind");
        failures += Expect(
            bagPageMoveRequestSchema->clientNamespace == "send",
            "SWITMP namespace");
        failures += Expect(
            bagPageMoveRequestSchema->xlsxStatus == "完成",
            "SWITMP xlsxStatus");
        failures += Expect(
            bagPageMoveRequestSchema->rawFormat ==
                "{fromSlot} {toPage} {toSlot}",
            "SWITMP rawFormat");
        failures += Expect(
            !bagPageMoveRequestSchema->formatEmpty,
            "SWITMP formatEmpty false");
        failures += Expect(
            bagPageMoveRequestSchema->source == "xlsx",
            "SWITMP raw source");

        constexpr std::array<std::string_view, 3U> RawNames = {{
            "fromSlot",
            "toPage",
            "toSlot",
        }};
        constexpr std::array<std::string_view, 3U> RawSpecs = {{
            "{fromSlot}",
            "{toPage}",
            "{toSlot}",
        }};
        failures += Expect(
            bagPageMoveRequestSchema->fields.size() == RawNames.size(),
            "SWITMP raw field count");
        if (bagPageMoveRequestSchema->fields.size() == RawNames.size()) {
            for (std::size_t index = 0U;
                 index < RawNames.size();
                 ++index) {
                const FieldSchema& field =
                    bagPageMoveRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == RawNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == RawSpecs[index] &&
                        field.source == "xlsx",
                    "SWITMP raw field order");
            }
        }

        failures += Expect(
            bagPageMoveRequestSchema->hasResolvedFormat,
            "SWITMP resolvedFormat present");
        failures += Expect(
            bagPageMoveRequestSchema->resolvedFormat ==
                "{fromSlot:i62} {toPage:i62} {toSlot:i62}",
            "SWITMP resolvedFormat");
        failures += Expect(
            bagPageMoveRequestSchema->resolvedSource ==
                "xlsx+ServerC+HookCpp+src/packet",
            "SWITMP resolvedSource");
        failures += Expect(
            bagPageMoveRequestSchema->hasResolvedFields,
            "SWITMP resolvedFields present");

        constexpr std::array<std::string_view, 3U> ResolvedSpecs = {{
            "{fromSlot:i62}",
            "{toPage:i62}",
            "{toSlot:i62}",
        }};
        failures += Expect(
            bagPageMoveRequestSchema->resolvedFields.size() ==
                RawNames.size(),
            "SWITMP resolved field count");
        if (bagPageMoveRequestSchema->resolvedFields.size() ==
            RawNames.size()) {
            for (std::size_t index = 0U;
                 index < RawNames.size();
                 ++index) {
                const FieldSchema& field =
                    bagPageMoveRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == RawNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == ResolvedSpecs[index] &&
                        field.source ==
                            "xlsx+ServerC+HookCpp+src/packet",
                    "SWITMP resolved field order");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *bagPageMoveRequestSchema,
            cg::packet::send::BagPageMoveRequest::Spec,
            "SWITMP Spec matches effective fields");
        failures += Expect(
            request.fromSlot == 10 &&
                request.toPage == 1 &&
                request.toSlot == -1,
            "SWITMP typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "SWITMP"),
        "SWITMP present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "SWITMP"),
        "SWITMP absent from receive typed manifest");

    const PacketSchema* bagPageMoveExRequestSchema =
        FindPacket(packets, "SWITMP2");
    failures += Expect(
        bagPageMoveExRequestSchema != nullptr,
        "SWITMP2 schema exists");
    if (bagPageMoveExRequestSchema != nullptr) {
        const cg::packet::send::BagPageMoveExRequest request(0, 8, 1, -1);
        failures += Expect(
            bagPageMoveExRequestSchema->direction == "客户端->服务端",
            "SWITMP2 direction");
        failures += Expect(
            bagPageMoveExRequestSchema->legacyKind == "RECV",
            "SWITMP2 legacyKind");
        failures += Expect(
            bagPageMoveExRequestSchema->clientNamespace == "send",
            "SWITMP2 namespace");
        failures += Expect(
            bagPageMoveExRequestSchema->xlsxStatus == "完成",
            "SWITMP2 xlsxStatus");
        failures += Expect(
            bagPageMoveExRequestSchema->rawFormat ==
                "{fromPage} {fromSlot} {toPage} {toSlot}",
            "SWITMP2 rawFormat");
        failures += Expect(
            !bagPageMoveExRequestSchema->formatEmpty,
            "SWITMP2 formatEmpty false");
        failures += Expect(
            bagPageMoveExRequestSchema->source == "xlsx",
            "SWITMP2 raw source");

        constexpr std::array<std::string_view, 4U> RawNames = {{
            "fromPage",
            "fromSlot",
            "toPage",
            "toSlot",
        }};
        constexpr std::array<std::string_view, 4U> RawSpecs = {{
            "{fromPage}",
            "{fromSlot}",
            "{toPage}",
            "{toSlot}",
        }};
        failures += Expect(
            bagPageMoveExRequestSchema->fields.size() == RawNames.size(),
            "SWITMP2 raw field count");
        if (bagPageMoveExRequestSchema->fields.size() ==
            RawNames.size()) {
            for (std::size_t index = 0U;
                 index < RawNames.size();
                 ++index) {
                const FieldSchema& field =
                    bagPageMoveExRequestSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == RawNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == RawSpecs[index] &&
                        field.source == "xlsx",
                    "SWITMP2 raw field order");
            }
        }

        failures += Expect(
            bagPageMoveExRequestSchema->hasResolvedFormat,
            "SWITMP2 resolvedFormat present");
        failures += Expect(
            bagPageMoveExRequestSchema->resolvedFormat ==
                "{fromPage:i62} {fromSlot:i62} {toPage:i62} {toSlot:i62}",
            "SWITMP2 resolvedFormat");
        failures += Expect(
            bagPageMoveExRequestSchema->resolvedSource ==
                "xlsx+ServerC+src/packet",
            "SWITMP2 resolvedSource");
        failures += Expect(
            bagPageMoveExRequestSchema->hasResolvedFields,
            "SWITMP2 resolvedFields present");

        constexpr std::array<std::string_view, 4U> ResolvedSpecs = {{
            "{fromPage:i62}",
            "{fromSlot:i62}",
            "{toPage:i62}",
            "{toSlot:i62}",
        }};
        failures += Expect(
            bagPageMoveExRequestSchema->resolvedFields.size() ==
                RawNames.size(),
            "SWITMP2 resolved field count");
        if (bagPageMoveExRequestSchema->resolvedFields.size() ==
            RawNames.size()) {
            for (std::size_t index = 0U;
                 index < RawNames.size();
                 ++index) {
                const FieldSchema& field =
                    bagPageMoveExRequestSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == RawNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == ResolvedSpecs[index] &&
                        field.source == "xlsx+ServerC+src/packet",
                    "SWITMP2 resolved field order");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *bagPageMoveExRequestSchema,
            cg::packet::send::BagPageMoveExRequest::Spec,
            "SWITMP2 Spec matches effective fields");
        failures += Expect(
            request.fromPage == 0 &&
                request.fromSlot == 8 &&
                request.toPage == 1 &&
                request.toSlot == -1,
            "SWITMP2 typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "SWITMP2"),
        "SWITMP2 present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "SWITMP2"),
        "SWITMP2 absent from receive typed manifest");

    const PacketSchema* versionUpgradeRequestSchema =
        FindPacket(packets, "Xoee");
    failures += Expect(
        versionUpgradeRequestSchema != nullptr,
        "Xoee schema exists");
    if (versionUpgradeRequestSchema != nullptr) {
        failures += Expect(
            versionUpgradeRequestSchema->direction == "客户端->服务端",
            "Xoee direction");
        failures += Expect(
            versionUpgradeRequestSchema->legacyKind == "RECV",
            "Xoee legacyKind");
        failures += Expect(
            versionUpgradeRequestSchema->clientNamespace == "send",
            "Xoee namespace");
        failures += Expect(
            versionUpgradeRequestSchema->xlsxStatus == "暂缓",
            "Xoee xlsxStatus");
        failures += Expect(
            versionUpgradeRequestSchema->rawFormat == "{mode} {productkey}",
            "Xoee rawFormat");
        failures += Expect(
            !versionUpgradeRequestSchema->formatEmpty,
            "Xoee formatEmpty false");
        failures += Expect(
            versionUpgradeRequestSchema->source == "xlsx",
            "Xoee raw source");
        failures += Expect(
            versionUpgradeRequestSchema->fields.size() == 2U,
            "Xoee raw field count");
        if (versionUpgradeRequestSchema->fields.size() == 2U) {
            const FieldSchema& mode =
                versionUpgradeRequestSchema->fields[0];
            const FieldSchema& productKey =
                versionUpgradeRequestSchema->fields[1];
            failures += Expect(
                mode.index == 0 &&
                    mode.name == "mode" &&
                    mode.typeToken == "unknown" &&
                    mode.rawSpec == "{mode}" &&
                    mode.source == "xlsx",
                "Xoee raw mode field");
            failures += Expect(
                productKey.index == 1 &&
                    productKey.name == "productkey" &&
                    productKey.typeToken == "unknown" &&
                    productKey.rawSpec == "{productkey}" &&
                    productKey.source == "xlsx",
                "Xoee raw productkey field");
        }

        failures += Expect(
            versionUpgradeRequestSchema->hasResolvedFormat,
            "Xoee resolvedFormat present");
        failures += Expect(
            versionUpgradeRequestSchema->resolvedFormat ==
                "{mode:i62} {productKey:es}",
            "Xoee resolvedFormat");
        failures += Expect(
            versionUpgradeRequestSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "Xoee resolvedSource");
        failures += Expect(
            versionUpgradeRequestSchema->hasResolvedFields,
            "Xoee resolvedFields present");
        failures += Expect(
            versionUpgradeRequestSchema->resolvedFields.size() == 2U,
            "Xoee resolved field count");
        if (versionUpgradeRequestSchema->resolvedFields.size() == 2U) {
            const FieldSchema& mode =
                versionUpgradeRequestSchema->resolvedFields[0];
            const FieldSchema& productKey =
                versionUpgradeRequestSchema->resolvedFields[1];
            failures += Expect(
                mode.index == 0 &&
                    mode.name == "mode" &&
                    mode.typeToken == "i62" &&
                    mode.rawSpec == "{mode:i62}" &&
                    mode.source == "xlsx+ServerC+ClientIda+src/packet",
                "Xoee resolved mode field");
            failures += Expect(
                productKey.index == 1 &&
                    productKey.name == "productKey" &&
                    productKey.typeToken == "es" &&
                    productKey.rawSpec == "{productKey:es}" &&
                    productKey.source == "xlsx+ServerC+ClientIda+src/packet",
                "Xoee resolved productKey field");
        }

        const cg::packet::send::VersionUpgradeRequest request(
            2,
            "20031229_20");
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *versionUpgradeRequestSchema,
            cg::packet::send::VersionUpgradeRequest::Spec,
            "Xoee Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *versionUpgradeRequestSchema,
            packet,
            "Xoee resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Xoee"),
        "Xoee present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Xoee"),
        "Xoee absent from receive typed manifest");

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
        failures += Expect(createNewCharSchema->clientNamespace == "send", "aSEl namespace");
        failures += Expect(createNewCharSchema->xlsxStatus == "完成", "aSEl xlsxStatus");
        failures += Expect(
            createNewCharSchema->description == "RECV_CreateNewChar 创建角色",
            "aSEl description");
        failures += Expect(
            createNewCharSchema->rawFormat ==
                "{dataPlace} {name} {image} {face} {vital} {strength} {tough} {quick} {magic} {earth} {water} {fire} {wind}",
            "aSEl rawFormat");
        failures += Expect(!createNewCharSchema->formatEmpty, "aSEl formatEmpty false");
        failures += Expect(createNewCharSchema->source == "xlsx", "aSEl raw source");
        failures += Expect(createNewCharSchema->fields.size() == 13U, "aSEl raw field count");
        failures += Expect(createNewCharSchema->hasResolvedFormat, "aSEl resolvedFormat present");
        failures += Expect(
            createNewCharSchema->resolvedFormat ==
                "{dataPlace:i62} {name:es} {image:i62} {face:i62} {vital:i62} {strength:i62} {tough:i62} {quick:i62} {magic:i62} {earth:i62} {water:i62} {fire:i62} {wind:i62}",
            "aSEl resolvedFormat");
        failures += Expect(
            createNewCharSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "aSEl resolvedSource");
        failures += Expect(createNewCharSchema->hasResolvedFields, "aSEl resolvedFields present");
        failures += Expect(
            createNewCharSchema->resolvedFields.size() == 13U,
            "aSEl resolved field count");
        for (const FieldSchema& field : createNewCharSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "aSEl resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *createNewCharSchema,
            cg::packet::send::CreateNewChar::Spec,
            "aSEl Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(*createNewCharSchema, packet, "aSEl resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "aSEl"),
        "aSEl present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "aSEl"),
        "aSEl absent from receive typed manifest");

    const PacketSchema* mapDataRequestSchema = FindPacket(packets, "UUN");
    failures += Expect(mapDataRequestSchema != nullptr, "UUN schema exists");
    if (mapDataRequestSchema != nullptr) {
        const cg::packet::send::MapDataRequest value(1, 2, 1, 2, 3, 4);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(mapDataRequestSchema->direction == "客户端->服务端", "UUN direction");
        failures += Expect(mapDataRequestSchema->legacyKind == "RECV", "UUN legacyKind");
        failures += Expect(mapDataRequestSchema->clientNamespace == "send", "UUN namespace");
        failures += Expect(mapDataRequestSchema->xlsxStatus == "完成", "UUN xlsxStatus");
        failures += Expect(
            mapDataRequestSchema->description == "RECV_M MAP_getdataFromRECT 获取地图信息",
            "UUN description");
        failures += Expect(
            mapDataRequestSchema->rawFormat
                == "{mapid:i62} {floor:i62} {x1:i62} {y1:i62} {x2:i62} {y2:i62}",
            "UUN rawFormat");
        failures += Expect(!mapDataRequestSchema->formatEmpty, "UUN formatEmpty false");
        failures += Expect(mapDataRequestSchema->source == "xlsx", "UUN raw source");
        failures += Expect(mapDataRequestSchema->fields.size() == 6U, "UUN raw field count");
        for (const FieldSchema& field : mapDataRequestSchema->fields) {
            failures += Expect(field.source == "xlsx", "UUN raw field source");
        }
        failures += Expect(mapDataRequestSchema->hasResolvedFormat, "UUN resolvedFormat present");
        failures += Expect(
            mapDataRequestSchema->resolvedFormat
                == "{mapid:i62} {floor:i62} {x1:i62} {y1:i62} {x2:i62} {y2:i62}",
            "UUN resolvedFormat");
        failures += Expect(
            mapDataRequestSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "UUN resolvedSource");
        failures += Expect(mapDataRequestSchema->hasResolvedFields, "UUN resolvedFields present");
        failures += Expect(
            mapDataRequestSchema->resolvedFields.size() == 6U,
            "UUN resolved field count");
        for (const FieldSchema& field : mapDataRequestSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "UUN resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *mapDataRequestSchema,
            cg::packet::send::MapDataRequest::Spec,
            "UUN Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *mapDataRequestSchema,
            packet,
            "UUN resolved field order");
        failures += Expect(
            value.mapId == 1 &&
                value.floor == 2 &&
                value.x1 == 1 &&
                value.y1 == 2 &&
                value.x2 == 3 &&
                value.y2 == 4,
            "UUN typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "UUN"),
        "UUN present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "UUN"),
        "UUN absent from receive typed manifest");

    const PacketSchema* setProfileDataSchema = FindPacket(packets, "Ot");
    failures += Expect(setProfileDataSchema != nullptr, "Ot schema exists");
    if (setProfileDataSchema != nullptr) {
        const cg::packet::send::SetProfileData value(
            1,
            "Sell",
            2,
            "Buy",
            3,
            "About",
            "Profile");
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(setProfileDataSchema->direction == "客户端->服务端", "Ot direction");
        failures += Expect(setProfileDataSchema->legacyKind == "RECV", "Ot legacyKind");
        failures += Expect(setProfileDataSchema->clientNamespace == "send", "Ot namespace");
        failures += Expect(setProfileDataSchema->xlsxStatus == "完成", "Ot xlsxStatus");
        failures += Expect(
            setProfileDataSchema->description == "RECV_PRS SetProfileData 简介内容",
            "Ot description");
        failures += Expect(
            setProfileDataSchema->rawFormat
                == "{sellFlag:i62} {sellMsg:es} {buyFlag:i62} {buyMsg:es} {aboutFlg:i62} {aboutMsg:es} {ProfileMsg:es}",
            "Ot rawFormat");
        failures += Expect(!setProfileDataSchema->formatEmpty, "Ot formatEmpty false");
        failures += Expect(setProfileDataSchema->source == "xlsx", "Ot raw source");
        failures += Expect(setProfileDataSchema->fields.size() == 7U, "Ot raw field count");
        for (const FieldSchema& field : setProfileDataSchema->fields) {
            failures += Expect(field.source == "xlsx", "Ot raw field source");
        }
        failures += Expect(setProfileDataSchema->hasResolvedFormat, "Ot resolvedFormat present");
        failures += Expect(
            setProfileDataSchema->resolvedFormat
                == "{sellFlag:i62} {sellMsg:es} {buyFlag:i62} {buyMsg:es} {aboutFlg:i62} {aboutMsg:es} {ProfileMsg:es}",
            "Ot resolvedFormat");
        failures += Expect(
            setProfileDataSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "Ot resolvedSource");
        failures += Expect(setProfileDataSchema->hasResolvedFields, "Ot resolvedFields present");
        failures += Expect(
            setProfileDataSchema->resolvedFields.size() == 7U,
            "Ot resolved field count");
        for (const FieldSchema& field : setProfileDataSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "Ot resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *setProfileDataSchema,
            cg::packet::send::SetProfileData::Spec,
            "Ot Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *setProfileDataSchema,
            packet,
            "Ot resolved field order");
        failures += Expect(
            value.sellFlag == 1 &&
                value.sellMessage == "Sell" &&
                value.buyFlag == 2 &&
                value.buyMessage == "Buy" &&
                value.aboutFlag == 3 &&
                value.aboutMessage == "About" &&
                value.profileMessage == "Profile",
            "Ot typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Ot"),
        "Ot present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Ot"),
        "Ot absent from receive typed manifest");

    const PacketSchema* magicUseSchema = FindPacket(packets, "RXr");
    failures += Expect(magicUseSchema != nullptr, "RXr schema exists");
    if (magicUseSchema != nullptr) {
        const cg::packet::send::MagicUseRequest magicUse(1, 2, 3, 4);
        const cg::packet::Packet packet = magicUse.ToPacket();
        failures += Expect(magicUseSchema->direction == "客户端->服务端", "RXr direction");
        failures += Expect(magicUseSchema->legacyKind == "RECV", "RXr legacyKind");
        failures += Expect(magicUseSchema->clientNamespace == "send", "RXr namespace");
        failures += Expect(magicUseSchema->xlsxStatus == "完成", "RXr xlsxStatus");
        failures += Expect(magicUseSchema->description == "RECV_MU", "RXr description");
        failures += Expect(magicUseSchema->rawFormat.empty(), "RXr rawFormat empty");
        failures += Expect(magicUseSchema->formatEmpty, "RXr formatEmpty");
        failures += Expect(magicUseSchema->source == "xlsx", "RXr raw source");
        failures += Expect(magicUseSchema->fields.empty(), "RXr raw fields empty");
        failures += Expect(magicUseSchema->hasResolvedFormat, "RXr resolvedFormat present");
        failures += Expect(
            magicUseSchema->resolvedFormat == "{x:i62} {y:i62} {array:i62} {toindex:i62}",
            "RXr resolvedFormat");
        failures += Expect(magicUseSchema->resolvedSource == "ServerC+src/packet", "RXr resolvedSource");
        failures += Expect(magicUseSchema->hasResolvedFields, "RXr resolvedFields present");
        failures += Expect(magicUseSchema->resolvedFields.size() == 4U, "RXr resolved field count");
        for (const FieldSchema& field : magicUseSchema->resolvedFields) {
            failures += Expect(
                field.source == "ServerC+src/packet",
                "RXr resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *magicUseSchema,
            cg::packet::send::MagicUseRequest::Spec,
            "RXr Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(*magicUseSchema, packet, "RXr resolved field order");
        failures += Expect(
            magicUse.x == 1 &&
                magicUse.y == 2 &&
                magicUse.array == 3 &&
                magicUse.toindex == 4,
            "RXr typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "RXr"),
        "RXr present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "RXr"),
        "RXr absent from receive typed manifest");

    const PacketSchema* petSkillUseSchema = FindPacket(packets, "Flvp");
    failures += Expect(petSkillUseSchema != nullptr, "Flvp schema exists");
    if (petSkillUseSchema != nullptr) {
        const cg::packet::send::PetSkillUseRequest value(1, 2, 3, "option");
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            petSkillUseSchema->direction == "客户端->服务端",
            "Flvp direction");
        failures += Expect(petSkillUseSchema->legacyKind == "RECV", "Flvp legacyKind");
        failures += Expect(petSkillUseSchema->clientNamespace == "send", "Flvp namespace");
        failures += Expect(petSkillUseSchema->xlsxStatus == "完成", "Flvp xlsxStatus");
        failures += Expect(petSkillUseSchema->description == "RECV_PS", "Flvp description");
        failures += Expect(petSkillUseSchema->rawFormat.empty(), "Flvp rawFormat empty");
        failures += Expect(petSkillUseSchema->formatEmpty, "Flvp formatEmpty");
        failures += Expect(petSkillUseSchema->source == "xlsx", "Flvp raw source");
        failures += Expect(petSkillUseSchema->fields.empty(), "Flvp raw fields empty");
        failures += Expect(
            petSkillUseSchema->hasResolvedFormat,
            "Flvp resolvedFormat present");
        failures += Expect(
            petSkillUseSchema->resolvedFormat
                == "{havepetindex:i62} {havepetskill:i62} {toindex:i62} {data:es}",
            "Flvp resolvedFormat");
        failures += Expect(
            petSkillUseSchema->resolvedSource == "ServerC+src/packet",
            "Flvp resolvedSource");
        failures += Expect(
            petSkillUseSchema->hasResolvedFields,
            "Flvp resolvedFields present");
        failures += Expect(
            petSkillUseSchema->resolvedFields.size() == 4U,
            "Flvp resolved field count");
        for (const FieldSchema& field : petSkillUseSchema->resolvedFields) {
            failures += Expect(
                field.source == "ServerC+src/packet",
                "Flvp resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *petSkillUseSchema,
            cg::packet::send::PetSkillUseRequest::Spec,
            "Flvp Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petSkillUseSchema,
            packet,
            "Flvp resolved field order");
        failures += Expect(
            value.havepetindex == 1 &&
                value.havepetskill == 2 &&
                value.toindex == 3 &&
                value.data == "option",
            "Flvp typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "Flvp"),
        "Flvp present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "Flvp"),
        "Flvp absent from receive typed manifest");

    const PacketSchema* petLevelUpPointSchema = FindPacket(packets, "kjSK");
    failures += Expect(petLevelUpPointSchema != nullptr, "kjSK schema exists");
    if (petLevelUpPointSchema != nullptr) {
        const cg::packet::send::PetLevelUpPoint value(2, 4);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            petLevelUpPointSchema->direction == "客户端->服务端",
            "kjSK direction");
        failures += Expect(
            petLevelUpPointSchema->legacyKind == "RECV",
            "kjSK legacyKind");
        failures += Expect(
            petLevelUpPointSchema->clientNamespace == "send",
            "kjSK namespace");
        failures += Expect(
            petLevelUpPointSchema->xlsxStatus == "完成",
            "kjSK xlsxStatus");
        failures += Expect(
            petLevelUpPointSchema->description == "RECV_PLVUP 宠物加点",
            "kjSK description");
        failures += Expect(
            petLevelUpPointSchema->rawFormat == "{petSlot} {sqeno}",
            "kjSK rawFormat");
        failures += Expect(
            !petLevelUpPointSchema->formatEmpty,
            "kjSK formatEmpty false");
        failures += Expect(petLevelUpPointSchema->source == "xlsx", "kjSK raw source");
        failures += Expect(petLevelUpPointSchema->fields.size() == 2U, "kjSK raw field count");
        if (petLevelUpPointSchema->fields.size() == 2U) {
            failures += Expect(
                petLevelUpPointSchema->fields[0].index == 0U &&
                    petLevelUpPointSchema->fields[0].name == "petSlot" &&
                    petLevelUpPointSchema->fields[0].typeToken == "unknown" &&
                    petLevelUpPointSchema->fields[0].rawSpec == "{petSlot}" &&
                    petLevelUpPointSchema->fields[0].source == "xlsx",
                "kjSK raw petSlot field");
            failures += Expect(
                petLevelUpPointSchema->fields[1].index == 1U &&
                    petLevelUpPointSchema->fields[1].name == "sqeno" &&
                    petLevelUpPointSchema->fields[1].typeToken == "unknown" &&
                    petLevelUpPointSchema->fields[1].rawSpec == "{sqeno}" &&
                    petLevelUpPointSchema->fields[1].source == "xlsx",
                "kjSK raw sqeno field");
        }
        failures += Expect(
            petLevelUpPointSchema->hasResolvedFormat,
            "kjSK resolvedFormat present");
        failures += Expect(
            petLevelUpPointSchema->resolvedFormat == "{petSlot:i62} {sqeno:i62}",
            "kjSK resolvedFormat");
        failures += Expect(
            petLevelUpPointSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "kjSK resolvedSource");
        failures += Expect(
            petLevelUpPointSchema->hasResolvedFields,
            "kjSK resolvedFields present");
        failures += Expect(
            petLevelUpPointSchema->resolvedFields.size() == 2U,
            "kjSK resolved field count");
        for (const FieldSchema& field : petLevelUpPointSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "kjSK resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *petLevelUpPointSchema,
            cg::packet::send::PetLevelUpPoint::Spec,
            "kjSK Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petLevelUpPointSchema,
            packet,
            "kjSK resolved field order");
        failures += Expect(
            value.petSlot == 2 && value.sqeno == 4,
            "kjSK typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "kjSK"),
        "kjSK present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "kjSK"),
        "kjSK absent from receive typed manifest");

    const PacketSchema* petSkillSwapSchema = FindPacket(packets, "HJQp");
    failures += Expect(petSkillSwapSchema != nullptr, "HJQp schema exists");
    if (petSkillSwapSchema != nullptr) {
        const cg::packet::send::PetSkillSwap value(1, 2, -1);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            petSkillSwapSchema->direction == "客户端->服务端",
            "HJQp direction");
        failures += Expect(petSkillSwapSchema->legacyKind == "RECV", "HJQp legacyKind");
        failures += Expect(petSkillSwapSchema->clientNamespace == "send", "HJQp namespace");
        failures += Expect(petSkillSwapSchema->xlsxStatus == "完成", "HJQp xlsxStatus");
        failures += Expect(
            petSkillSwapSchema->description
                == "RECV_PSSW SKILL_swapPetSkill 宠物技能移动",
            "HJQp description");
        failures += Expect(
            petSkillSwapSchema->rawFormat == "{petSlot} {from} {to}",
            "HJQp rawFormat");
        failures += Expect(!petSkillSwapSchema->formatEmpty, "HJQp formatEmpty false");
        failures += Expect(petSkillSwapSchema->source == "xlsx", "HJQp raw source");
        failures += Expect(petSkillSwapSchema->fields.size() == 3U, "HJQp raw field count");
        if (petSkillSwapSchema->fields.size() == 3U) {
            const std::array<std::string_view, 3U> names = {{"petSlot", "from", "to"}};
            const std::array<std::string_view, 3U> rawSpecs = {{
                "{petSlot}",
                "{from}",
                "{to}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                failures += Expect(
                    petSkillSwapSchema->fields[index].index == index &&
                        petSkillSwapSchema->fields[index].name == names[index] &&
                        petSkillSwapSchema->fields[index].typeToken == "unknown" &&
                        petSkillSwapSchema->fields[index].rawSpec == rawSpecs[index] &&
                        petSkillSwapSchema->fields[index].source == "xlsx",
                    "HJQp raw field");
            }
        }
        failures += Expect(
            petSkillSwapSchema->hasResolvedFormat,
            "HJQp resolvedFormat present");
        failures += Expect(
            petSkillSwapSchema->resolvedFormat
                == "{petSlot:i62} {from:i62} {to:i62}",
            "HJQp resolvedFormat");
        failures += Expect(
            petSkillSwapSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "HJQp resolvedSource");
        failures += Expect(
            petSkillSwapSchema->hasResolvedFields,
            "HJQp resolvedFields present");
        failures += Expect(
            petSkillSwapSchema->resolvedFields.size() == 3U,
            "HJQp resolved field count");
        for (const FieldSchema& field : petSkillSwapSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "HJQp resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *petSkillSwapSchema,
            cg::packet::send::PetSkillSwap::Spec,
            "HJQp Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petSkillSwapSchema,
            packet,
            "HJQp resolved field order");
        failures += Expect(
            value.petSlot == 1 && value.from == 2 && value.to == -1,
            "HJQp typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "HJQp"),
        "HJQp present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "HJQp"),
        "HJQp absent from receive typed manifest");

    const PacketSchema* userPetNameSchema = FindPacket(packets, "ycX");
    failures += Expect(userPetNameSchema != nullptr, "ycX schema exists");
    if (userPetNameSchema != nullptr) {
        const cg::packet::send::UserPetNameInput value(1, "Pet");
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            userPetNameSchema->direction == "客户端->服务端",
            "ycX direction");
        failures += Expect(userPetNameSchema->legacyKind == "RECV", "ycX legacyKind");
        failures += Expect(userPetNameSchema->clientNamespace == "send", "ycX namespace");
        failures += Expect(userPetNameSchema->xlsxStatus == "完成", "ycX xlsxStatus");
        failures += Expect(
            userPetNameSchema->description
                == "RECV_KN CHAR_inputUserPetName 宠物姓名",
            "ycX description");
        failures += Expect(
            userPetNameSchema->rawFormat == "{petSlot} {name}",
            "ycX rawFormat");
        failures += Expect(!userPetNameSchema->formatEmpty, "ycX formatEmpty false");
        failures += Expect(userPetNameSchema->source == "xlsx", "ycX raw source");
        failures += Expect(userPetNameSchema->fields.size() == 2U, "ycX raw field count");
        if (userPetNameSchema->fields.size() == 2U) {
            failures += Expect(
                userPetNameSchema->fields[0].index == 0U &&
                    userPetNameSchema->fields[0].name == "petSlot" &&
                    userPetNameSchema->fields[0].typeToken == "unknown" &&
                    userPetNameSchema->fields[0].rawSpec == "{petSlot}" &&
                    userPetNameSchema->fields[0].source == "xlsx",
                "ycX raw petSlot field");
            failures += Expect(
                userPetNameSchema->fields[1].index == 1U &&
                    userPetNameSchema->fields[1].name == "name" &&
                    userPetNameSchema->fields[1].typeToken == "unknown" &&
                    userPetNameSchema->fields[1].rawSpec == "{name}" &&
                    userPetNameSchema->fields[1].source == "xlsx",
                "ycX raw name field");
        }
        failures += Expect(
            userPetNameSchema->hasResolvedFormat,
            "ycX resolvedFormat present");
        failures += Expect(
            userPetNameSchema->resolvedFormat == "{petSlot:i62} {name:es}",
            "ycX resolvedFormat");
        failures += Expect(
            userPetNameSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "ycX resolvedSource");
        failures += Expect(
            userPetNameSchema->hasResolvedFields,
            "ycX resolvedFields present");
        failures += Expect(
            userPetNameSchema->resolvedFields.size() == 2U,
            "ycX resolved field count");
        for (const FieldSchema& field : userPetNameSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "ycX resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *userPetNameSchema,
            cg::packet::send::UserPetNameInput::Spec,
            "ycX Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *userPetNameSchema,
            packet,
            "ycX resolved field order");
        failures += Expect(
            value.petSlot == 1 && value.name == "Pet",
            "ycX typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "ycX"),
        "ycX present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "ycX"),
        "ycX absent from receive typed manifest");

    const PacketSchema* effectSchema = FindPacket(packets, "zzd");
    failures += Expect(effectSchema != nullptr, "zzd schema exists");
    if (effectSchema != nullptr) {
        const cg::packet::receive::Effect effect(32, -1, "data");
        const cg::packet::Packet packet = effect.ToPacket();
        failures += Expect(effectSchema->direction == "服务端->客户端", "zzd direction");
        failures += Expect(effectSchema->legacyKind == "SEND", "zzd legacyKind");
        failures += Expect(effectSchema->clientNamespace == "receive", "zzd namespace");
        failures += Expect(
            effectSchema->rawFormat == "{EffType:i62} {TM_EffSize:i62} {data:es}",
            "zzd rawFormat");
        failures += Expect(!effectSchema->formatEmpty, "zzd raw format non-empty");
        failures += Expect(effectSchema->source == "xlsx", "zzd raw source");
        failures += Expect(effectSchema->fields.size() == 3U, "zzd raw field count");
        for (const FieldSchema& field : effectSchema->fields) {
            failures += Expect(field.source == "xlsx", "zzd raw field source");
        }
        failures += ExpectPacketFieldsMatch(*effectSchema, packet, "zzd raw field order");
        failures += Expect(
            effectSchema->resolvedFormat == "{EffType:i62} {TM_EffSize:i62} {data:es}",
            "zzd resolvedFormat");
        failures += Expect(
            effectSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "zzd resolvedSource");
        failures += Expect(effectSchema->resolvedFields.size() == 3U, "zzd resolved field count");
        for (const FieldSchema& field : effectSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "zzd resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *effectSchema,
            cg::packet::receive::Effect::Spec,
            "zzd Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(*effectSchema, packet, "zzd ToPacket field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "zzd"),
        "zzd absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "zzd"),
        "zzd present in receive typed manifest");

    const PacketSchema* battleCommandSchema = FindPacket(packets, "LYIm");
    failures += Expect(battleCommandSchema != nullptr, "LYIm schema exists");
    if (battleCommandSchema != nullptr) {
        const cg::packet::receive::BattleCommand battleCommand("BU");
        const cg::packet::Packet packet = battleCommand.ToPacket();
        failures += Expect(battleCommandSchema->direction == "服务端->客户端", "LYIm direction");
        failures += Expect(battleCommandSchema->legacyKind == "SEND", "LYIm legacyKind");
        failures += Expect(battleCommandSchema->clientNamespace == "receive", "LYIm namespace");
        failures += Expect(battleCommandSchema->xlsxStatus == "完成", "LYIm xlsxStatus");
        failures += Expect(battleCommandSchema->description == "SEND_B", "LYIm description");
        failures += Expect(battleCommandSchema->rawFormat.empty(), "LYIm rawFormat empty");
        failures += Expect(battleCommandSchema->formatEmpty, "LYIm formatEmpty");
        failures += Expect(battleCommandSchema->source == "xlsx", "LYIm raw source");
        failures += Expect(battleCommandSchema->fields.empty(), "LYIm raw fields empty");
        failures += Expect(battleCommandSchema->hasResolvedFormat, "LYIm resolvedFormat present");
        failures += Expect(battleCommandSchema->resolvedFormat == "{command:es}", "LYIm resolvedFormat");
        failures += Expect(
            battleCommandSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "LYIm resolvedSource");
        failures += Expect(battleCommandSchema->hasResolvedFields, "LYIm resolvedFields present");
        failures += Expect(battleCommandSchema->resolvedFields.size() == 1U, "LYIm resolved field count");
        if (battleCommandSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = battleCommandSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0U &&
                    field.name == "command" &&
                    field.typeToken == "es" &&
                    field.rawSpec == "{command:es}" &&
                    field.source == "ServerC+ClientIda+src/packet",
                "LYIm resolved command field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *battleCommandSchema,
            cg::packet::receive::BattleCommand::Spec,
            "LYIm Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *battleCommandSchema,
            packet,
            "LYIm ToPacket field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "LYIm"),
        "LYIm absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "LYIm"),
        "LYIm present in receive typed manifest");

    const PacketSchema* battleRewardSchema = FindPacket(packets, "Gn");
    failures += Expect(battleRewardSchema != nullptr, "Gn schema exists");
    if (battleRewardSchema != nullptr) {
        const cg::packet::receive::BattleReward reward;
        const cg::packet::PacketResult<cg::packet::Packet> packet = reward.ToPacket();
        failures += Expect(battleRewardSchema->direction == "服务端->客户端", "Gn direction");
        failures += Expect(battleRewardSchema->legacyKind == "SEND", "Gn legacyKind");
        failures += Expect(battleRewardSchema->clientNamespace == "receive", "Gn namespace");
        failures += Expect(battleRewardSchema->xlsxStatus == "完成", "Gn xlsxStatus");
        failures += Expect(
            battleRewardSchema->description == "SEND_RS 战斗获得物品",
            "Gn description");
        failures += Expect(battleRewardSchema->rawFormat.empty(), "Gn rawFormat empty");
        failures += Expect(battleRewardSchema->formatEmpty, "Gn formatEmpty");
        failures += Expect(battleRewardSchema->source == "xlsx", "Gn raw source");
        failures += Expect(battleRewardSchema->fields.empty(), "Gn raw fields empty");
        failures += Expect(battleRewardSchema->hasResolvedFormat, "Gn resolvedFormat present");
        failures += Expect(battleRewardSchema->resolvedFormat == "{data:es}", "Gn resolvedFormat");
        failures += Expect(
            battleRewardSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "Gn resolvedSource");
        failures += Expect(battleRewardSchema->hasResolvedFields, "Gn resolvedFields present");
        failures += Expect(battleRewardSchema->resolvedFields.size() == 1U, "Gn resolved field count");
        if (battleRewardSchema->resolvedFields.size() == 1U) {
            failures += Expect(
                battleRewardSchema->resolvedFields[0].source
                    == "ServerC+ClientIda+src/packet",
                "Gn resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *battleRewardSchema,
            cg::packet::receive::BattleReward::Spec,
            "Gn Spec matches effective fields");
        failures += Expect(packet.HasValue(), "Gn ToPacket");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *battleRewardSchema,
                packet.Value(),
                "Gn ToPacket field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "Gn"),
        "Gn absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "Gn"),
        "Gn present in receive typed manifest");

    const PacketSchema* duelPointSchema = FindPacket(packets, "hB");
    failures += Expect(duelPointSchema != nullptr, "hB schema exists");
    if (duelPointSchema != nullptr) {
        const cg::packet::receive::DuelPoint duelPoint(-1, 100);
        const cg::packet::Packet packet = duelPoint.ToPacket();
        failures += Expect(duelPointSchema->direction == "服务端->客户端", "hB direction");
        failures += Expect(duelPointSchema->legacyKind == "SEND", "hB legacyKind");
        failures += Expect(duelPointSchema->clientNamespace == "receive", "hB namespace");
        failures += Expect(duelPointSchema->xlsxStatus == "完成", "hB xlsxStatus");
        failures += Expect(
            duelPointSchema->description == "SEND_RD BATTLE_GetDuelPoint",
            "hB description");
        failures += Expect(duelPointSchema->rawFormat.empty(), "hB rawFormat empty");
        failures += Expect(duelPointSchema->formatEmpty, "hB formatEmpty");
        failures += Expect(duelPointSchema->source == "xlsx", "hB raw source");
        failures += Expect(duelPointSchema->fields.empty(), "hB raw fields empty");
        failures += Expect(duelPointSchema->hasResolvedFormat, "hB resolvedFormat present");
        failures += Expect(duelPointSchema->resolvedFormat == "{data:es}", "hB resolvedFormat");
        failures += Expect(
            duelPointSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "hB resolvedSource");
        failures += Expect(duelPointSchema->hasResolvedFields, "hB resolvedFields present");
        failures += Expect(duelPointSchema->resolvedFields.size() == 1U, "hB resolved field count");
        if (duelPointSchema->resolvedFields.size() == 1U) {
            failures += Expect(
                duelPointSchema->resolvedFields[0].source
                    == "ServerC+ClientIda+src/packet",
                "hB resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *duelPointSchema,
            cg::packet::receive::DuelPoint::Spec,
            "hB Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *duelPointSchema,
            packet,
            "hB ToPacket field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "hB"),
        "hB absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "hB"),
        "hB present in receive typed manifest");

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
        cg::packet::receive::ProfileMailMessagePayload payload;
        payload.sentTimeText = "2026/ 8/ 1 12:34";
        payload.messageRaw = "message";
        payload.petBaseImageNumberOrMarker = -1;
        const cg::packet::receive::ProfileMailMessage profileMailMessage(
            1,
            std::move(payload),
            3);
        const cg::packet::PacketResult<cg::packet::Packet> packet = profileMailMessage.ToPacket();
        failures += Expect(profileMailMessageSchema->direction == "服务端->客户端", "Mhbt direction");
        failures += Expect(profileMailMessageSchema->legacyKind == "SEND", "Mhbt legacyKind");
        failures += Expect(profileMailMessageSchema->clientNamespace == "receive", "Mhbt namespace");
        failures += Expect(profileMailMessageSchema->xlsxStatus == "完成", "Mhbt xlsxStatus");
        failures += Expect(profileMailMessageSchema->description == "SEND_MSG recvmsg", "Mhbt description");
        failures += Expect(profileMailMessageSchema->rawFormat.empty(), "Mhbt rawFormat empty");
        failures += Expect(profileMailMessageSchema->formatEmpty, "Mhbt formatEmpty");
        failures += Expect(profileMailMessageSchema->source == "xlsx", "Mhbt raw source");
        failures += Expect(profileMailMessageSchema->fields.empty(), "Mhbt raw fields empty");
        failures += Expect(profileMailMessageSchema->hasResolvedFormat, "Mhbt resolvedFormat present");
        failures += Expect(
            profileMailMessageSchema->resolvedFormat == "{aindex:i62} {text:es} {color:i62}",
            "Mhbt resolvedFormat");
        failures += Expect(
            profileMailMessageSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "Mhbt resolvedSource");
        failures += Expect(profileMailMessageSchema->hasResolvedFields, "Mhbt resolvedFields present");
        failures += Expect(profileMailMessageSchema->resolvedFields.size() == 3U, "Mhbt resolved field count");
        if (profileMailMessageSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> Names = {"aindex", "text", "color"};
            constexpr std::array<std::string_view, 3U> Types = {"i62", "es", "i62"};
            constexpr std::array<std::string_view, 3U> RawSpecs = {
                "{aindex:i62}",
                "{text:es}",
                "{color:i62}",
            };
            for (std::size_t index = 0U; index < Names.size(); ++index) {
                const FieldSchema& field = profileMailMessageSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == Names[index] &&
                        field.typeToken == Types[index] &&
                        field.rawSpec == RawSpecs[index] &&
                        field.source == "ServerC+ClientIda+src/packet",
                    "Mhbt resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *profileMailMessageSchema,
            cg::packet::receive::ProfileMailMessage::Spec,
            "Mhbt Spec matches effective fields");
        failures += Expect(packet.HasValue(), "Mhbt ToPacket");
        if (packet.HasValue()) {
            failures += Expect(
                packet.Value().fields.size() == 3U &&
                    packet.Value().fields[1].type == cg::packet::PacketFieldType::SubPayload,
                "Mhbt ToPacket payload type");
            failures += ExpectResolvedFieldsMatch(
                *profileMailMessageSchema,
                packet.Value(),
                "Mhbt ToPacket field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "Mhbt"),
        "Mhbt absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "Mhbt"),
        "Mhbt present in receive typed manifest");

    const PacketSchema* chatMessageSchema = FindPacket(packets, "zdr");
    failures += Expect(chatMessageSchema != nullptr, "zdr schema exists");
    if (chatMessageSchema != nullptr) {
        cg::packet::receive::ChatMessagePayload payload;
        payload.textRaw = "message";
        const cg::packet::receive::ChatMessage chatMessage(-1, std::move(payload), 4, 16);
        const cg::packet::PacketResult<cg::packet::Packet> packet = chatMessage.ToPacket();
        failures += Expect(chatMessageSchema->direction == "服务端->客户端", "zdr direction");
        failures += Expect(chatMessageSchema->legacyKind == "SEND", "zdr legacyKind");
        failures += Expect(chatMessageSchema->clientNamespace == "receive", "zdr namespace");
        failures += Expect(chatMessageSchema->xlsxStatus == "完成", "zdr xlsxStatus");
        failures += Expect(chatMessageSchema->description == "SEND_TK", "zdr description");
        failures += Expect(chatMessageSchema->rawFormat.empty(), "zdr rawFormat empty");
        failures += Expect(chatMessageSchema->formatEmpty, "zdr formatEmpty");
        failures += Expect(chatMessageSchema->source == "xlsx", "zdr raw source");
        failures += Expect(chatMessageSchema->fields.empty(), "zdr raw fields empty");
        failures += Expect(chatMessageSchema->hasResolvedFormat, "zdr resolvedFormat present");
        failures += Expect(
            chatMessageSchema->resolvedFormat == "{objectIndex:i62} {message:es} {color:i62} {fontSize:i62}",
            "zdr resolvedFormat");
        failures += Expect(
            chatMessageSchema->resolvedSource == "ServerC+ClientIda+src",
            "zdr resolvedSource");
        failures += Expect(chatMessageSchema->hasResolvedFields, "zdr resolvedFields present");
        failures += Expect(chatMessageSchema->resolvedFields.size() == 4U, "zdr resolved field count");
        if (chatMessageSchema->resolvedFields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> Names = {
                "objectIndex",
                "message",
                "color",
                "fontSize",
            };
            constexpr std::array<std::string_view, 4U> Types = {
                "i62",
                "es",
                "i62",
                "i62",
            };
            constexpr std::array<std::string_view, 4U> RawSpecs = {
                "{objectIndex:i62}",
                "{message:es}",
                "{color:i62}",
                "{fontSize:i62}",
            };
            for (std::size_t index = 0U; index < Names.size(); ++index) {
                const FieldSchema& field = chatMessageSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == Names[index] &&
                        field.typeToken == Types[index] &&
                        field.rawSpec == RawSpecs[index] &&
                        field.source == "ServerC+ClientIda+src",
                    "zdr resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *chatMessageSchema,
            cg::packet::receive::ChatMessage::Spec,
            "zdr Spec matches effective fields");
        failures += Expect(packet.HasValue(), "zdr ToPacket");
        if (packet.HasValue()) {
            failures += Expect(
                packet.Value().fields.size() == 4U &&
                    packet.Value().fields[1].type == cg::packet::PacketFieldType::SubPayload,
                "zdr ToPacket payload type");
            failures += ExpectResolvedFieldsMatch(
                *chatMessageSchema,
                packet.Value(),
                "zdr ToPacket field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "zdr"),
        "zdr absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "zdr"),
        "zdr present in receive typed manifest");

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
        const cg::packet::receive::StallBrowse stallBrowse(
            0,
            62,
            "name",
            "description",
            "0|",
            "0|");
        const cg::packet::Packet packet = stallBrowse.ToPacket();
        failures += Expect(stallBrowseSchema->direction == "服务端->客户端", "ft direction");
        failures += Expect(stallBrowseSchema->legacyKind == "SEND", "ft legacyKind");
        failures += Expect(stallBrowseSchema->clientNamespace == "receive", "ft namespace");
        failures += Expect(stallBrowseSchema->xlsxStatus == "完成", "ft xlsxStatus");
        failures += Expect(stallBrowseSchema->description == "SEND_StallBrowse", "ft description");
        failures += Expect(
            stallBrowseSchema->rawFormat == "{result} {name} {description} {item} {pet}",
            "ft rawFormat");
        failures += Expect(!stallBrowseSchema->formatEmpty, "ft formatEmpty false");
        failures += Expect(stallBrowseSchema->source == "xlsx", "ft raw source");
        failures += Expect(
            stallBrowseSchema->resolvedFormat ==
                "{result:i62} {objIndex:i62} {name:es} {description:es} {item:es} {pet:es}",
            "ft resolvedFormat");
        failures += Expect(
            stallBrowseSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "ft resolvedSource");
        failures += Expect(stallBrowseSchema->fields.size() == 5U, "ft raw field count");
        if (stallBrowseSchema->fields.size() == 5U) {
            constexpr std::array<std::string_view, 5U> Names = {
                "result",
                "name",
                "description",
                "item",
                "pet",
            };
            constexpr std::array<std::string_view, 5U> RawSpecs = {
                "{result}",
                "{name}",
                "{description}",
                "{item}",
                "{pet}",
            };
            for (std::size_t index = 0U; index < Names.size(); ++index) {
                const FieldSchema& field = stallBrowseSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == Names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == RawSpecs[index] &&
                        field.source == "xlsx",
                    "ft raw field");
            }
        }
        failures += Expect(stallBrowseSchema->hasResolvedFormat, "ft resolvedFormat present");
        failures += Expect(stallBrowseSchema->hasResolvedFields, "ft resolvedFields present");
        failures += Expect(stallBrowseSchema->resolvedFields.size() == 6U, "ft resolved field count");
        if (stallBrowseSchema->resolvedFields.size() == 6U) {
            constexpr std::array<std::string_view, 6U> Names = {
                "result",
                "objIndex",
                "name",
                "description",
                "item",
                "pet",
            };
            constexpr std::array<std::string_view, 6U> Types = {
                "i62",
                "i62",
                "es",
                "es",
                "es",
                "es",
            };
            constexpr std::array<std::string_view, 6U> RawSpecs = {
                "{result:i62}",
                "{objIndex:i62}",
                "{name:es}",
                "{description:es}",
                "{item:es}",
                "{pet:es}",
            };
            for (std::size_t index = 0U; index < Names.size(); ++index) {
                const FieldSchema& field = stallBrowseSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == Names[index] &&
                        field.typeToken == Types[index] &&
                        field.rawSpec == RawSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "ft resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *stallBrowseSchema,
            cg::packet::receive::StallBrowse::Spec,
            "ft Spec matches effective fields");
        failures += Expect(
            packet.fields.size() == 6U &&
                packet.fields[4].type == cg::packet::PacketFieldType::EscapedString &&
                packet.fields[5].type == cg::packet::PacketFieldType::EscapedString,
            "ft ToPacket payload fields remain outer es");
        failures += ExpectResolvedFieldsMatch(
            *stallBrowseSchema,
            packet,
            "ft ToPacket field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "ft"),
        "ft absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "ft"),
        "ft present in receive typed manifest");

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
        const cg::packet::send::ChatMessageSend value(1, 2, "P|hello", 4, 5, 16);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(chatMessageSendSchema->direction == "客户端->服务端", "uSr direction");
        failures += Expect(chatMessageSendSchema->legacyKind == "RECV", "uSr legacyKind");
        failures += Expect(chatMessageSendSchema->clientNamespace == "send", "uSr namespace");
        failures += Expect(chatMessageSendSchema->xlsxStatus == "完成", "uSr xlsxStatus");
        failures += Expect(
            chatMessageSendSchema->description == "RECV_TK 发送聊天",
            "uSr description");
        failures += Expect(
            chatMessageSendSchema->rawFormat == "{x} {y} {msg} {color} {area} {fontSize}",
            "uSr rawFormat");
        failures += Expect(!chatMessageSendSchema->formatEmpty, "uSr formatEmpty false");
        failures += Expect(chatMessageSendSchema->source == "xlsx", "uSr raw source");
        failures += Expect(
            chatMessageSendSchema->resolvedFormat ==
                "{x:i62} {y:i62} {msg:es} {color:i62} {area:i62} {fontSize:i62}",
            "uSr resolvedFormat");
        failures += Expect(
            chatMessageSendSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "uSr resolvedSource");
        failures += Expect(chatMessageSendSchema->fields.size() == 6U, "uSr raw field count");
        if (chatMessageSendSchema->fields.size() == 6U) {
            const std::array<std::string_view, 6U> names = {{
                "x",
                "y",
                "msg",
                "color",
                "area",
                "fontSize",
            }};
            const std::array<std::string_view, 6U> rawSpecs = {{
                "{x}",
                "{y}",
                "{msg}",
                "{color}",
                "{area}",
                "{fontSize}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                failures += Expect(
                    chatMessageSendSchema->fields[index].index == index &&
                        chatMessageSendSchema->fields[index].name == names[index] &&
                        chatMessageSendSchema->fields[index].typeToken == "unknown" &&
                        chatMessageSendSchema->fields[index].rawSpec == rawSpecs[index] &&
                        chatMessageSendSchema->fields[index].source == "xlsx",
                    "uSr raw field");
            }
        }
        failures += Expect(
            chatMessageSendSchema->hasResolvedFormat,
            "uSr resolvedFormat present");
        failures += Expect(
            chatMessageSendSchema->hasResolvedFields,
            "uSr resolvedFields present");
        failures += Expect(
            chatMessageSendSchema->resolvedFields.size() == 6U,
            "uSr resolved field count");
        for (const FieldSchema& field : chatMessageSendSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "uSr resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *chatMessageSendSchema,
            cg::packet::send::ChatMessageSend::Spec,
            "uSr Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *chatMessageSendSchema,
            packet,
            "uSr resolved field order");
        failures += Expect(
            value.x == 1 &&
                value.y == 2 &&
                value.msg == "P|hello" &&
                value.color == 4 &&
                value.area == 5 &&
                value.fontSize == 16,
            "uSr typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "uSr"),
        "uSr present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "uSr"),
        "uSr absent from receive typed manifest");

    const PacketSchema* windowOpenSchema = FindPacket(packets, "xD");
    failures += Expect(windowOpenSchema != nullptr, "xD schema exists");
    if (windowOpenSchema != nullptr) {
        const cg::packet::send::WindowOpenSend value(
            10,
            20,
            3,
            -1,
            0,
            "I\\z8\\z9\\z1");
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            windowOpenSchema->direction == "客户端->服务端",
            "xD direction");
        failures += Expect(windowOpenSchema->legacyKind == "RECV", "xD legacyKind");
        failures += Expect(windowOpenSchema->clientNamespace == "send", "xD namespace");
        failures += Expect(windowOpenSchema->xlsxStatus == "完成", "xD xlsxStatus");
        failures += Expect(
            windowOpenSchema->description == "RECV_WN 打开系统窗口",
            "xD description");
        failures += Expect(
            windowOpenSchema->rawFormat
                == "{x} {y} {seqno} {objindex} {select} {data:es}",
            "xD rawFormat");
        failures += Expect(!windowOpenSchema->formatEmpty, "xD formatEmpty false");
        failures += Expect(windowOpenSchema->source == "xlsx", "xD raw source");
        failures += Expect(windowOpenSchema->fields.size() == 6U, "xD raw field count");
        if (windowOpenSchema->fields.size() == 6U) {
            const std::array<std::string_view, 6U> names = {{
                "x",
                "y",
                "seqno",
                "objindex",
                "select",
                "data",
            }};
            const std::array<std::string_view, 6U> types = {{
                "unknown",
                "unknown",
                "unknown",
                "unknown",
                "unknown",
                "es",
            }};
            const std::array<std::string_view, 6U> rawSpecs = {{
                "{x}",
                "{y}",
                "{seqno}",
                "{objindex}",
                "{select}",
                "{data:es}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                failures += Expect(
                    windowOpenSchema->fields[index].index == index &&
                        windowOpenSchema->fields[index].name == names[index] &&
                        windowOpenSchema->fields[index].typeToken == types[index] &&
                        windowOpenSchema->fields[index].rawSpec == rawSpecs[index] &&
                        windowOpenSchema->fields[index].source == "xlsx",
                    "xD raw field");
            }
        }
        failures += Expect(
            windowOpenSchema->resolvedFormat
                == "{x:i62} {y:i62} {seqno:i62} {objindex:i62} {select:i62} {data:es}",
            "xD resolvedFormat");
        failures += Expect(
            windowOpenSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "xD resolvedSource");
        failures += Expect(
            windowOpenSchema->hasResolvedFields,
            "xD resolvedFields present");
        failures += Expect(
            windowOpenSchema->resolvedFields.size() == 6U,
            "xD resolved field count");
        for (const FieldSchema& field : windowOpenSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "xD resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *windowOpenSchema,
            cg::packet::send::WindowOpenSend::Spec,
            "xD Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *windowOpenSchema,
            packet,
            "xD resolved field order");
        failures += Expect(
            value.x == 10 &&
                value.y == 20 &&
                value.seqno == 3 &&
                value.objindex == -1 &&
                value.select == 0 &&
                value.data == "I\\z8\\z9\\z1",
            "xD typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "xD"),
        "xD present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "xD"),
        "xD absent from receive typed manifest");

    const PacketSchema* dropItemSchema = FindPacket(packets, "QpfE");
    failures += Expect(dropItemSchema != nullptr, "QpfE schema exists");
    if (dropItemSchema != nullptr) {
        const cg::packet::send::DropItem dropItem(10, 20, 0, 62);
        const cg::packet::Packet packet = dropItem.ToPacket();
        failures += Expect(
            dropItemSchema->direction == "客户端->服务端",
            "QpfE direction");
        failures += Expect(
            dropItemSchema->legacyKind == "RECV",
            "QpfE legacyKind");
        failures += Expect(
            dropItemSchema->clientNamespace == "send",
            "QpfE namespace");
        failures += Expect(
            dropItemSchema->xlsxStatus == "完成",
            "QpfE xlsxStatus");
        failures += Expect(
            dropItemSchema->rawFormat == "{x} {y} {who} {itemindex}",
            "QpfE rawFormat");
        failures += Expect(
            !dropItemSchema->formatEmpty,
            "QpfE formatEmpty false");
        failures += Expect(
            dropItemSchema->source == "xlsx",
            "QpfE raw source");
        failures += Expect(
            dropItemSchema->fields.size() == 4U,
            "QpfE raw field count");
        if (dropItemSchema->fields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "x",
                "y",
                "who",
                "itemindex",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{x}",
                "{y}",
                "{who}",
                "{itemindex}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = dropItemSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx",
                    "QpfE raw field order");
            }
        }

        failures += Expect(
            dropItemSchema->hasResolvedFormat,
            "QpfE resolvedFormat present");
        failures += Expect(
            dropItemSchema->resolvedFormat ==
                "{x:i62} {y:i62} {who:i62} {itemindex:i62}",
            "QpfE resolvedFormat");
        failures += Expect(
            dropItemSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "QpfE resolvedSource");
        failures += Expect(
            dropItemSchema->hasResolvedFields,
            "QpfE resolvedFields present");
        failures += Expect(
            dropItemSchema->resolvedFields.size() == 4U,
            "QpfE resolved field count");
        if (dropItemSchema->resolvedFields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "x",
                "y",
                "who",
                "itemindex",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{x:i62}",
                "{y:i62}",
                "{who:i62}",
                "{itemindex:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    dropItemSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "QpfE resolved field order");
            }
        }

        failures += ExpectEffectiveFieldsMatch(
            *dropItemSchema,
            cg::packet::send::DropItem::Spec,
            "QpfE Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *dropItemSchema,
            packet,
            "QpfE resolved field order");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "QpfE"),
        "QpfE present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "QpfE"),
        "QpfE absent from receive typed manifest");

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
        failures += Expect(shutdownRequestSchema->clientNamespace == "send", "IcE namespace");
        failures += Expect(shutdownRequestSchema->xlsxStatus == "暂缓", "IcE xlsxStatus");
        failures += Expect(
            shutdownRequestSchema->description == "RECV_Shutdown",
            "IcE description");
        failures += Expect(shutdownRequestSchema->rawFormat.empty(), "IcE rawFormat empty");
        failures += Expect(shutdownRequestSchema->formatEmpty, "IcE formatEmpty");
        failures += Expect(shutdownRequestSchema->source == "xlsx", "IcE raw source");
        failures += Expect(shutdownRequestSchema->fields.empty(), "IcE raw fields empty");
        failures += Expect(
            shutdownRequestSchema->hasResolvedFormat,
            "IcE resolvedFormat present");
        failures += Expect(
            shutdownRequestSchema->resolvedFormat == "{passwd:es} {min:i62}",
            "IcE resolvedFormat");
        failures += Expect(shutdownRequestSchema->resolvedSource == "ServerC+src/packet", "IcE resolvedSource");
        failures += Expect(
            shutdownRequestSchema->hasResolvedFields,
            "IcE resolvedFields present");
        failures += Expect(
            shutdownRequestSchema->resolvedFields.size() == 2U,
            "IcE resolved field count");
        for (const FieldSchema& field : shutdownRequestSchema->resolvedFields) {
            failures += Expect(
                field.source == "ServerC+src/packet",
                "IcE resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *shutdownRequestSchema,
            cg::packet::send::ShutdownRequest::Spec,
            "IcE Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(*shutdownRequestSchema, packet, "IcE resolved field order");
        failures += Expect(
            shutdownRequest.passwd == "pass" && shutdownRequest.min == 10,
            "IcE typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "IcE"),
        "IcE present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "IcE"),
        "IcE absent from receive typed manifest");

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
        const cg::packet::PacketResult<std::string> data = stallStart.BuildData();
        failures += Expect(stallStartSchema->direction == "客户端->服务端", "ylGN direction");
        failures += Expect(stallStartSchema->legacyKind == "RECV", "ylGN legacyKind");
        failures += Expect(stallStartSchema->clientNamespace == "send", "ylGN namespace");
        failures += Expect(stallStartSchema->xlsxStatus == "完成", "ylGN xlsxStatus");
        failures += Expect(
            stallStartSchema->description == "RECV_StallStart 开始摆摊请求",
            "ylGN description");
        failures += Expect(
            stallStartSchema->rawFormat == "{stallName} {stallDescription} {stallList}",
            "ylGN rawFormat");
        failures += Expect(!stallStartSchema->formatEmpty, "ylGN formatEmpty false");
        failures += Expect(stallStartSchema->source == "xlsx", "ylGN raw source");
        failures += Expect(
            stallStartSchema->hasResolvedFormat,
            "ylGN resolvedFormat present");
        failures += Expect(
            stallStartSchema->resolvedFormat == "{stallName:es} {stallDescription:es} {stallList:es}",
            "ylGN resolvedFormat");
        failures += Expect(
            stallStartSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "ylGN resolvedSource");
        failures += Expect(stallStartSchema->fields.size() == 3U, "ylGN raw field count");
        if (stallStartSchema->fields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{
                "stallName",
                "stallDescription",
                "stallList",
            }};
            constexpr std::array<std::string_view, 3U> rawSpecs = {{
                "{stallName}",
                "{stallDescription}",
                "{stallList}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = stallStartSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "ylGN raw field");
            }
        }
        failures += Expect(
            stallStartSchema->hasResolvedFields,
            "ylGN resolvedFields present");
        failures += Expect(
            stallStartSchema->resolvedFields.size() == 3U,
            "ylGN resolved field count");
        if (stallStartSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{
                "stallName",
                "stallDescription",
                "stallList",
            }};
            constexpr std::array<std::string_view, 3U> rawSpecs = {{
                "{stallName:es}",
                "{stallDescription:es}",
                "{stallList:es}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = stallStartSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "es" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "ylGN resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *stallStartSchema,
            cg::packet::send::StallStart::Spec,
            "ylGN Spec matches effective fields");
        std::string zeroPrices;
        for (std::size_t index = 0U; index < cg::packet::send::StallStart::PriceCount; ++index) {
            zeroPrices.append("0\\z");
        }
        failures += Expect(static_cast<bool>(data), "ylGN data build");
        if (data.HasValue()) {
            failures += Expect(data.Value() == zeroPrices, "ylGN normalized zero prices");
        }
        failures += Expect(static_cast<bool>(packet), "ylGN packet build");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *stallStartSchema,
                packet.Value(),
                "ylGN resolved field order");
        }
        bool pricesAreZero = true;
        for (const int price : stallStart.prices) {
            pricesAreZero = pricesAreZero && price == 0;
        }
        failures += Expect(
            stallStart.stallName == "Shop" &&
                stallStart.stallDescription == "Desc" &&
                pricesAreZero,
            "ylGN typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "ylGN"),
        "ylGN present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "ylGN"),
        "ylGN absent from receive typed manifest");

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
        failures += Expect(guildTitleSetSchema->clientNamespace == "send", "GY namespace");
        failures += Expect(guildTitleSetSchema->xlsxStatus == "完成", "GY xlsxStatus");
        failures += Expect(
            guildTitleSetSchema->description == "RECV_GT 设置家族职位信息",
            "GY description");
        failures += Expect(guildTitleSetSchema->rawFormat == "{index} {bitflag} {data:es}", "GY rawFormat");
        failures += Expect(!guildTitleSetSchema->formatEmpty, "GY formatEmpty false");
        failures += Expect(guildTitleSetSchema->source == "xlsx", "GY raw source");
        failures += Expect(
            guildTitleSetSchema->resolvedFormat == "{index:i62} {bitflag:i62} {data:es}",
            "GY resolvedFormat");
        failures += Expect(
            guildTitleSetSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "GY resolvedSource");
        failures += Expect(guildTitleSetSchema->fields.size() == 3U, "GY raw field count");
        if (guildTitleSetSchema->fields.size() == 3U) {
            const std::array<std::string_view, 3U> names = {{"index", "bitflag", "data"}};
            const std::array<std::string_view, 3U> types = {{"unknown", "unknown", "es"}};
            const std::array<std::string_view, 3U> rawSpecs = {{
                "{index}",
                "{bitflag}",
                "{data:es}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                failures += Expect(
                    guildTitleSetSchema->fields[index].index == index &&
                        guildTitleSetSchema->fields[index].name == names[index] &&
                        guildTitleSetSchema->fields[index].typeToken == types[index] &&
                        guildTitleSetSchema->fields[index].rawSpec == rawSpecs[index] &&
                        guildTitleSetSchema->fields[index].source == "xlsx",
                    "GY raw field");
            }
        }
        failures += Expect(
            guildTitleSetSchema->hasResolvedFields,
            "GY resolvedFields present");
        failures += Expect(
            guildTitleSetSchema->resolvedFields.size() == 3U,
            "GY resolved field count");
        for (const FieldSchema& field : guildTitleSetSchema->resolvedFields) {
            failures += Expect(
                field.source == "xlsx+ServerC+ClientIda+src/packet",
                "GY resolved field source");
        }
        failures += ExpectEffectiveFieldsMatch(
            *guildTitleSetSchema,
            cg::packet::send::GuildTitleSet::Spec,
            "GY Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *guildTitleSetSchema,
            packet,
            "GY resolved field order");
        failures += Expect(
            guildTitleSet.index == 10 &&
                guildTitleSet.bitflag == 128 &&
                guildTitleSet.data == "data",
            "GY typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "GY"),
        "GY present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "GY"),
        "GY absent from receive typed manifest");

    const PacketSchema* guildMailSendSchema = FindPacket(packets, "uId");
    failures += Expect(guildMailSendSchema != nullptr, "uId schema exists");
    if (guildMailSendSchema != nullptr) {
        const cg::packet::send::GuildMailSend guildMailSend(10, "text", 6);
        const cg::packet::Packet packet = guildMailSend.ToPacket();
        failures += Expect(guildMailSendSchema->direction == "客户端->服务端", "uId direction");
        failures += Expect(guildMailSendSchema->legacyKind == "RECV", "uId legacyKind");
        failures += Expect(guildMailSendSchema->clientNamespace == "send", "uId namespace");
        failures += Expect(guildMailSendSchema->xlsxStatus == "完成", "uId xlsxStatus");
        failures += Expect(
            guildMailSendSchema->description == "RECV_GML 家族邮件",
            "uId description");
        failures += Expect(
            guildMailSendSchema->rawFormat == "{toregistNumber} {text} {color}",
            "uId rawFormat");
        failures += Expect(!guildMailSendSchema->formatEmpty, "uId formatEmpty false");
        failures += Expect(guildMailSendSchema->source == "xlsx", "uId raw source");
        failures += Expect(
            guildMailSendSchema->hasResolvedFormat,
            "uId resolvedFormat present");
        failures += Expect(
            guildMailSendSchema->resolvedFormat == "{toregistNumber:i62} {text:es} {color:i62}",
            "uId resolvedFormat");
        failures += Expect(
            guildMailSendSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "uId resolvedSource");
        failures += Expect(guildMailSendSchema->fields.size() == 3U, "uId raw field count");
        if (guildMailSendSchema->fields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{
                "toregistNumber",
                "text",
                "color",
            }};
            constexpr std::array<std::string_view, 3U> rawSpecs = {{
                "{toregistNumber}",
                "{text}",
                "{color}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = guildMailSendSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "uId raw field");
            }
        }
        failures += Expect(
            guildMailSendSchema->hasResolvedFields,
            "uId resolvedFields present");
        failures += Expect(
            guildMailSendSchema->resolvedFields.size() == 3U,
            "uId resolved field count");
        if (guildMailSendSchema->resolvedFields.size() == 3U) {
            constexpr std::array<std::string_view, 3U> names = {{
                "toregistNumber",
                "text",
                "color",
            }};
            constexpr std::array<std::string_view, 3U> types = {{
                "i62",
                "es",
                "i62",
            }};
            constexpr std::array<std::string_view, 3U> rawSpecs = {{
                "{toregistNumber:i62}",
                "{text:es}",
                "{color:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = guildMailSendSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "uId resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *guildMailSendSchema,
            cg::packet::send::GuildMailSend::Spec,
            "uId Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *guildMailSendSchema,
            packet,
            "uId resolved field order");
        failures += Expect(
            guildMailSend.toregistNumber == 10 &&
                guildMailSend.text == "text" &&
                guildMailSend.color == 6,
            "uId typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "uId"),
        "uId present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "uId"),
        "uId absent from receive typed manifest");

    const PacketSchema* profileMailFriendDeleteSchema = FindPacket(packets, "wD");
    failures += Expect(profileMailFriendDeleteSchema != nullptr, "wD schema exists");
    if (profileMailFriendDeleteSchema != nullptr) {
        const cg::packet::send::ProfileMailFriendDelete profileMailFriendDelete(12, "Alice");
        const cg::packet::Packet packet = profileMailFriendDelete.ToPacket();
        failures += Expect(profileMailFriendDeleteSchema->direction == "客户端->服务端", "wD direction");
        failures += Expect(profileMailFriendDeleteSchema->legacyKind == "RECV", "wD legacyKind");
        failures += Expect(profileMailFriendDeleteSchema->clientNamespace == "send", "wD namespace");
        failures += Expect(profileMailFriendDeleteSchema->xlsxStatus == "完成", "wD xlsxStatus");
        failures += Expect(
            profileMailFriendDeleteSchema->description == "RECV_PRE 删除迷你邮件好友",
            "wD description");
        failures += Expect(profileMailFriendDeleteSchema->rawFormat == "{} {name}", "wD rawFormat");
        failures += Expect(!profileMailFriendDeleteSchema->formatEmpty, "wD formatEmpty false");
        failures += Expect(profileMailFriendDeleteSchema->source == "xlsx", "wD raw source");
        failures += Expect(
            profileMailFriendDeleteSchema->hasResolvedFormat,
            "wD resolvedFormat present");
        failures += Expect(
            profileMailFriendDeleteSchema->resolvedFormat == "{profileIndex:i62} {name:es}",
            "wD resolvedFormat");
        failures += Expect(
            profileMailFriendDeleteSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "wD resolvedSource");
        failures += Expect(profileMailFriendDeleteSchema->fields.size() == 2U, "wD raw field count");
        if (profileMailFriendDeleteSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"", "name"}};
            constexpr std::array<std::string_view, 2U> rawSpecs = {{"{}", "{name}"}};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = profileMailFriendDeleteSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "wD raw field");
            }
        }
        failures += Expect(
            profileMailFriendDeleteSchema->hasResolvedFields,
            "wD resolvedFields present");
        failures += Expect(
            profileMailFriendDeleteSchema->resolvedFields.size() == 2U,
            "wD resolved field count");
        if (profileMailFriendDeleteSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{
                "profileIndex",
                "name",
            }};
            constexpr std::array<std::string_view, 2U> types = {{"i62", "es"}};
            constexpr std::array<std::string_view, 2U> rawSpecs = {{
                "{profileIndex:i62}",
                "{name:es}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = profileMailFriendDeleteSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "wD resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *profileMailFriendDeleteSchema,
            cg::packet::send::ProfileMailFriendDelete::Spec,
            "wD Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *profileMailFriendDeleteSchema,
            packet,
            "wD resolved field order");
        failures += Expect(
            profileMailFriendDelete.profileIndex == 12 &&
                profileMailFriendDelete.name == "Alice",
            "wD typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "wD"),
        "wD present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "wD"),
        "wD absent from receive typed manifest");

    const PacketSchema* profileMiniMailSendSchema = FindPacket(packets, "GH");
    failures += Expect(profileMiniMailSendSchema != nullptr, "GH schema exists");
    if (profileMiniMailSendSchema != nullptr) {
        const cg::packet::send::ProfileMiniMailSend profileMiniMailSend(12, "Alice", "hello", 3);
        const cg::packet::PacketResult<cg::packet::Packet> packet = profileMiniMailSend.ToPacket();
        const cg::packet::PacketResult<std::string> data = profileMiniMailSend.BuildData();
        failures += Expect(profileMiniMailSendSchema->direction == "客户端->服务端", "GH direction");
        failures += Expect(profileMiniMailSendSchema->legacyKind == "RECV", "GH legacyKind");
        failures += Expect(profileMiniMailSendSchema->clientNamespace == "send", "GH namespace");
        failures += Expect(profileMiniMailSendSchema->xlsxStatus == "完成", "GH xlsxStatus");
        failures += Expect(
            profileMiniMailSendSchema->description == "RECV_PRM 发送迷你邮件",
            "GH description");
        failures += Expect(profileMiniMailSendSchema->rawFormat == "{data} {color}", "GH rawFormat");
        failures += Expect(!profileMiniMailSendSchema->formatEmpty, "GH formatEmpty false");
        failures += Expect(profileMiniMailSendSchema->source == "xlsx", "GH raw source");
        failures += Expect(
            profileMiniMailSendSchema->hasResolvedFormat,
            "GH resolvedFormat present");
        failures += Expect(
            profileMiniMailSendSchema->resolvedFormat == "{data:es} {color:i62}",
            "GH resolvedFormat");
        failures += Expect(
            profileMiniMailSendSchema->resolvedSource
                == "xlsx+ServerC+ClientIda+src/packet",
            "GH resolvedSource");
        failures += Expect(profileMiniMailSendSchema->fields.size() == 2U, "GH raw field count");
        if (profileMiniMailSendSchema->fields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"data", "color"}};
            constexpr std::array<std::string_view, 2U> rawSpecs = {{
                "{data}",
                "{color}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = profileMiniMailSendSchema->fields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx",
                    "GH raw field");
            }
        }
        failures += Expect(
            profileMiniMailSendSchema->hasResolvedFields,
            "GH resolvedFields present");
        failures += Expect(
            profileMiniMailSendSchema->resolvedFields.size() == 2U,
            "GH resolved field count");
        if (profileMiniMailSendSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{"data", "color"}};
            constexpr std::array<std::string_view, 2U> types = {{"es", "i62"}};
            constexpr std::array<std::string_view, 2U> rawSpecs = {{
                "{data:es}",
                "{color:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field = profileMiniMailSendSchema->resolvedFields[index];
                failures += Expect(
                    field.index == index &&
                        field.name == names[index] &&
                        field.typeToken == types[index] &&
                        field.rawSpec == rawSpecs[index] &&
                        field.source == "xlsx+ServerC+ClientIda+src/packet",
                    "GH resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *profileMiniMailSendSchema,
            cg::packet::send::ProfileMiniMailSend::Spec,
            "GH Spec matches effective fields");
        failures += Expect(static_cast<bool>(data), "GH data build");
        if (data.HasValue()) {
            failures += Expect(
                data.Value() == "12\\zAlice\\zhello",
                "GH data exact3 legacy escape");
        }
        failures += Expect(static_cast<bool>(packet), "GH packet build");
        if (packet.HasValue()) {
            failures += ExpectResolvedFieldsMatch(
                *profileMiniMailSendSchema,
                packet.Value(),
                "GH resolved field order");
        }
        failures += Expect(
            profileMiniMailSend.profileIndex == 12 &&
                profileMiniMailSend.name == "Alice" &&
                profileMiniMailSend.message == "hello" &&
                profileMiniMailSend.color == 3,
            "GH typed members");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "GH"),
        "GH present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "GH"),
        "GH absent from receive typed manifest");

    const PacketSchema* characterLookRequestSchema = FindPacket(packets, "nB");
    failures += Expect(
        characterLookRequestSchema != nullptr,
        "nB schema exists");
    if (characterLookRequestSchema != nullptr) {
        const cg::packet::send::CharacterLookRequest request(0);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            characterLookRequestSchema->direction == "客户端->服务端",
            "nB direction");
        failures += Expect(
            characterLookRequestSchema->legacyKind == "RECV",
            "nB legacyKind");
        failures += Expect(
            characterLookRequestSchema->clientNamespace == "send",
            "nB namespace");
        failures += Expect(
            characterLookRequestSchema->xlsxStatus == "暂缓",
            "nB xlsxStatus");
        failures += Expect(
            characterLookRequestSchema->rawFormat.empty(),
            "nB rawFormat empty");
        failures += Expect(
            characterLookRequestSchema->formatEmpty,
            "nB formatEmpty true");
        failures += Expect(
            characterLookRequestSchema->source == "xlsx",
            "nB raw source");
        failures += Expect(
            characterLookRequestSchema->fields.empty(),
            "nB raw fields empty");
        failures += Expect(
            characterLookRequestSchema->hasResolvedFormat,
            "nB resolvedFormat present");
        failures += Expect(
            characterLookRequestSchema->resolvedFormat == "{dir:i62}",
            "nB resolvedFormat");
        failures += Expect(
            characterLookRequestSchema->resolvedSource == "ServerC+src/packet",
            "nB resolvedSource");
        failures += Expect(
            characterLookRequestSchema->hasResolvedFields,
            "nB resolvedFields present");
        failures += Expect(
            characterLookRequestSchema->resolvedFields.size() == 1U,
            "nB resolved field count");
        if (characterLookRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                characterLookRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "dir" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{dir:i62}" &&
                    field.source == "ServerC+src/packet",
                "nB resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *characterLookRequestSchema,
            cg::packet::send::CharacterLookRequest::Spec,
            "nB Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterLookRequestSchema,
            packet,
            "nB resolved field order");
        failures += Expect(request.direction == 0, "nB typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "nB"),
        "nB present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "nB"),
        "nB absent from receive typed manifest");

    const PacketSchema* battleHelpModeRequestSchema =
        FindPacket(packets, "FVm");
    failures += Expect(
        battleHelpModeRequestSchema != nullptr,
        "FVm schema exists");
    if (battleHelpModeRequestSchema != nullptr) {
        const cg::packet::send::BattleHelpModeRequest request(1);
        const cg::packet::Packet packet = request.ToPacket();
        failures += Expect(
            battleHelpModeRequestSchema->direction == "客户端->服务端",
            "FVm direction");
        failures += Expect(
            battleHelpModeRequestSchema->legacyKind == "RECV",
            "FVm legacyKind");
        failures += Expect(
            battleHelpModeRequestSchema->clientNamespace == "send",
            "FVm namespace");
        failures += Expect(
            battleHelpModeRequestSchema->xlsxStatus == "暂缓",
            "FVm xlsxStatus");
        failures += Expect(
            battleHelpModeRequestSchema->rawFormat.empty(),
            "FVm rawFormat empty");
        failures += Expect(
            battleHelpModeRequestSchema->formatEmpty,
            "FVm formatEmpty true");
        failures += Expect(
            battleHelpModeRequestSchema->source == "xlsx",
            "FVm raw source");
        failures += Expect(
            battleHelpModeRequestSchema->fields.empty(),
            "FVm raw fields empty");
        failures += Expect(
            battleHelpModeRequestSchema->hasResolvedFormat,
            "FVm resolvedFormat present");
        failures += Expect(
            battleHelpModeRequestSchema->resolvedFormat == "{flg:i62}",
            "FVm resolvedFormat");
        failures += Expect(
            battleHelpModeRequestSchema->resolvedSource == "ServerC+src/packet",
            "FVm resolvedSource");
        failures += Expect(
            battleHelpModeRequestSchema->hasResolvedFields,
            "FVm resolvedFields present");
        failures += Expect(
            battleHelpModeRequestSchema->resolvedFields.size() == 1U,
            "FVm resolved field count");
        if (battleHelpModeRequestSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                battleHelpModeRequestSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "flg" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{flg:i62}" &&
                    field.source == "ServerC+src/packet",
                "FVm resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *battleHelpModeRequestSchema,
            cg::packet::send::BattleHelpModeRequest::Spec,
            "FVm Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *battleHelpModeRequestSchema,
            packet,
            "FVm resolved field order");
        failures += Expect(request.helpFlag == 1, "FVm typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "FVm"),
        "FVm present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "FVm"),
        "FVm absent from receive typed manifest");

    const PacketSchema* itemMoveNotificationSchema =
        FindPacket(packets, "vWpn");
    failures += Expect(
        itemMoveNotificationSchema != nullptr,
        "vWpn schema exists");
    if (itemMoveNotificationSchema != nullptr) {
        const cg::packet::receive::ItemMoveNotification notification(0, 1);
        const cg::packet::Packet packet = notification.ToPacket();
        failures += Expect(
            itemMoveNotificationSchema->direction == "服务端->客户端",
            "vWpn direction");
        failures += Expect(
            itemMoveNotificationSchema->legacyKind == "SEND",
            "vWpn legacyKind");
        failures += Expect(
            itemMoveNotificationSchema->clientNamespace == "receive",
            "vWpn namespace");
        failures += Expect(
            itemMoveNotificationSchema->xlsxStatus == "完成",
            "vWpn xlsxStatus");
        failures += Expect(
            itemMoveNotificationSchema->description == "SEND_SI moveitem",
            "vWpn description");
        failures += Expect(
            itemMoveNotificationSchema->rawFormat.empty(),
            "vWpn rawFormat empty");
        failures += Expect(
            itemMoveNotificationSchema->formatEmpty,
            "vWpn formatEmpty true");
        failures += Expect(
            itemMoveNotificationSchema->source == "xlsx",
            "vWpn raw source");
        failures += Expect(
            itemMoveNotificationSchema->fields.empty(),
            "vWpn raw fields empty");
        failures += Expect(
            itemMoveNotificationSchema->hasResolvedFormat,
            "vWpn resolvedFormat present");
        failures += Expect(
            itemMoveNotificationSchema->resolvedFormat ==
                "{fromindex:i62} {toindex:i62}",
            "vWpn resolvedFormat");
        failures += Expect(
            itemMoveNotificationSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "vWpn resolvedSource");
        failures += Expect(
            itemMoveNotificationSchema->hasResolvedFields,
            "vWpn resolvedFields present");
        failures += Expect(
            itemMoveNotificationSchema->resolvedFields.size() == 2U,
            "vWpn resolved field count");
        if (itemMoveNotificationSchema->resolvedFields.size() == 2U) {
            const FieldSchema& fromField =
                itemMoveNotificationSchema->resolvedFields[0];
            const FieldSchema& toField =
                itemMoveNotificationSchema->resolvedFields[1];
            failures += Expect(
                fromField.index == 0 &&
                    fromField.name == "fromindex" &&
                    fromField.typeToken == "i62" &&
                    fromField.rawSpec == "{fromindex:i62}" &&
                    fromField.source == "ServerC+ClientIda+src/packet",
                "vWpn resolved from field");
            failures += Expect(
                toField.index == 1 &&
                    toField.name == "toindex" &&
                    toField.typeToken == "i62" &&
                    toField.rawSpec == "{toindex:i62}" &&
                    toField.source == "ServerC+ClientIda+src/packet",
                "vWpn resolved to field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *itemMoveNotificationSchema,
            cg::packet::receive::ItemMoveNotification::Spec,
            "vWpn Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *itemMoveNotificationSchema,
            packet,
            "vWpn resolved field order");
        failures += Expect(
            notification.fromIndex == 0 && notification.toIndex == 1,
            "vWpn typed members");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "vWpn"),
        "vWpn absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "vWpn"),
        "vWpn present in receive typed manifest");

    const PacketSchema* albumIdNotificationSchema = FindPacket(packets, "IYo");
    failures += Expect(albumIdNotificationSchema != nullptr, "IYo schema exists");
    if (albumIdNotificationSchema != nullptr) {
        const cg::packet::receive::AlbumRegistrationNotification notification(1);
        const cg::packet::Packet packet = notification.ToPacket();
        failures += Expect(
            albumIdNotificationSchema->direction == "服务端->客户端",
            "IYo direction");
        failures += Expect(
            albumIdNotificationSchema->legacyKind == "SEND",
            "IYo legacyKind");
        failures += Expect(
            albumIdNotificationSchema->clientNamespace == "receive",
            "IYo namespace");
        failures += Expect(
            albumIdNotificationSchema->xlsxStatus == "完成",
            "IYo xlsxStatus");
        failures += Expect(
            albumIdNotificationSchema->description == "SEND_ALO",
            "IYo description");
        failures += Expect(
            albumIdNotificationSchema->rawFormat.empty(),
            "IYo rawFormat empty");
        failures += Expect(
            albumIdNotificationSchema->formatEmpty,
            "IYo formatEmpty true");
        failures += Expect(
            albumIdNotificationSchema->source == "xlsx",
            "IYo raw source");
        failures += Expect(
            albumIdNotificationSchema->fields.empty(),
            "IYo raw fields empty");
        failures += Expect(
            albumIdNotificationSchema->hasResolvedFormat,
            "IYo resolvedFormat present");
        failures += Expect(
            albumIdNotificationSchema->resolvedFormat == "{albumId:i62}",
            "IYo resolvedFormat");
        failures += Expect(
            albumIdNotificationSchema->resolvedSource == "ServerC+ClientIda+src/packet",
            "IYo resolvedSource");
        failures += Expect(
            albumIdNotificationSchema->hasResolvedFields,
            "IYo resolvedFields present");
        failures += Expect(
            albumIdNotificationSchema->resolvedFields.size() == 1U,
            "IYo resolved field count");
        if (albumIdNotificationSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                albumIdNotificationSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "albumId" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{albumId:i62}" &&
                    field.source == "ServerC+ClientIda+src/packet",
                "IYo resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *albumIdNotificationSchema,
            cg::packet::receive::AlbumRegistrationNotification::Spec,
            "IYo Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *albumIdNotificationSchema,
            packet,
            "IYo resolved field order");
        failures += Expect(notification.albumId == 1, "IYo typed member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "IYo"),
        "IYo absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "IYo"),
        "IYo present in receive typed manifest");

    const PacketSchema* battleHelpModeStatusSchema = FindPacket(packets, "sZWu");
    failures += Expect(
        battleHelpModeStatusSchema != nullptr,
        "sZWu schema exists");
    if (battleHelpModeStatusSchema != nullptr) {
        const cg::packet::receive::BattleHelpModeStatus status(1);
        const cg::packet::Packet packet = status.ToPacket();
        failures += Expect(
            battleHelpModeStatusSchema->direction == "服务端->客户端",
            "sZWu direction");
        failures += Expect(
            battleHelpModeStatusSchema->legacyKind == "SEND",
            "sZWu legacyKind");
        failures += Expect(
            battleHelpModeStatusSchema->clientNamespace == "receive",
            "sZWu namespace");
        failures += Expect(
            battleHelpModeStatusSchema->xlsxStatus == "暂缓",
            "sZWu xlsxStatus");
        failures += Expect(
            battleHelpModeStatusSchema->description == "SEND_HL",
            "sZWu description");
        failures += Expect(
            battleHelpModeStatusSchema->rawFormat.empty(),
            "sZWu rawFormat empty");
        failures += Expect(
            battleHelpModeStatusSchema->formatEmpty,
            "sZWu formatEmpty true");
        failures += Expect(
            battleHelpModeStatusSchema->source == "xlsx",
            "sZWu raw source");
        failures += Expect(
            battleHelpModeStatusSchema->fields.empty(),
            "sZWu raw fields empty");
        failures += Expect(
            battleHelpModeStatusSchema->hasResolvedFormat,
            "sZWu resolvedFormat present");
        failures += Expect(
            battleHelpModeStatusSchema->resolvedFormat == "{flg:i62}",
            "sZWu resolvedFormat");
        failures += Expect(
            battleHelpModeStatusSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "sZWu resolvedSource");
        failures += Expect(
            battleHelpModeStatusSchema->hasResolvedFields,
            "sZWu resolvedFields present");
        failures += Expect(
            battleHelpModeStatusSchema->resolvedFields.size() == 1U,
            "sZWu resolved field count");
        if (battleHelpModeStatusSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = battleHelpModeStatusSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "flg" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{flg:i62}" &&
                    field.source == "ServerC+ClientIda+src/packet",
                "sZWu resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *battleHelpModeStatusSchema,
            cg::packet::receive::BattleHelpModeStatus::Spec,
            "sZWu Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *battleHelpModeStatusSchema,
            packet,
            "sZWu resolved field order");
        failures += Expect(status.helpFlag == 1, "sZWu typed member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "sZWu"),
        "sZWu absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "sZWu"),
        "sZWu present in receive typed manifest");

    const PacketSchema* partyMembershipResultSchema =
        FindPacket(packets, "bNY");
    failures += Expect(
        partyMembershipResultSchema != nullptr,
        "bNY schema exists");
    if (partyMembershipResultSchema != nullptr) {
        const cg::packet::receive::PartyMembershipResult value(1, 0);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            partyMembershipResultSchema->direction == "服务端->客户端",
            "bNY direction");
        failures += Expect(
            partyMembershipResultSchema->legacyKind == "SEND",
            "bNY legacyKind");
        failures += Expect(
            partyMembershipResultSchema->clientNamespace == "receive",
            "bNY namespace");
        failures += Expect(
            partyMembershipResultSchema->xlsxStatus == "完成",
            "bNY xlsxStatus");
        failures += Expect(
            partyMembershipResultSchema->description == "SEND_PR",
            "bNY description");
        failures += Expect(
            partyMembershipResultSchema->rawFormat.empty(),
            "bNY rawFormat empty");
        failures += Expect(
            partyMembershipResultSchema->formatEmpty,
            "bNY formatEmpty true");
        failures += Expect(
            partyMembershipResultSchema->source == "xlsx",
            "bNY raw source");
        failures += Expect(
            partyMembershipResultSchema->fields.empty(),
            "bNY raw fields empty");
        failures += Expect(
            partyMembershipResultSchema->hasResolvedFormat,
            "bNY resolvedFormat present");
        failures += Expect(
            partyMembershipResultSchema->resolvedFormat ==
                "{request:i62} {result:i62}",
            "bNY resolvedFormat");
        failures += Expect(
            partyMembershipResultSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "bNY resolvedSource");
        failures += Expect(
            partyMembershipResultSchema->hasResolvedFields,
            "bNY resolvedFields present");
        failures += Expect(
            partyMembershipResultSchema->resolvedFields.size() == 2U,
            "bNY resolved field count");
        if (partyMembershipResultSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{
                "request",
                "result",
            }};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{request:i62}",
                "{result:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    partyMembershipResultSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "ServerC+ClientIda+src/packet",
                    "bNY resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *partyMembershipResultSchema,
            cg::packet::receive::PartyMembershipResult::Spec,
            "bNY Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *partyMembershipResultSchema,
            packet,
            "bNY resolved field order");
        failures += Expect(value.request == 1, "bNY request member");
        failures += Expect(value.result == 0, "bNY result member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "bNY"),
        "bNY absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "bNY"),
        "bNY present in receive typed manifest");

    const PacketSchema* tradeCounterpartyOfferChangedSchema =
        FindPacket(packets, "NdM");
    failures += Expect(
        tradeCounterpartyOfferChangedSchema != nullptr,
        "NdM schema exists");
    if (tradeCounterpartyOfferChangedSchema != nullptr) {
        const cg::packet::receive::TradeCounterpartyOfferChanged value;
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->direction == "服务端->客户端",
            "NdM direction");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->legacyKind == "SEND",
            "NdM legacyKind");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->clientNamespace == "receive",
            "NdM namespace");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->xlsxStatus == "完成",
            "NdM xlsxStatus");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->description ==
                "SEND_TROP CHAR_Trade_setTradeList 对方取出物品",
            "NdM description");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->rawFormat.empty(),
            "NdM rawFormat empty");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->formatEmpty,
            "NdM formatEmpty true");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->source == "xlsx",
            "NdM raw source");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->fields.empty(),
            "NdM raw fields empty");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->hasResolvedFormat,
            "NdM resolvedFormat present");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->resolvedFormat.empty(),
            "NdM resolvedFormat empty");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "NdM resolvedSource");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->hasResolvedFields,
            "NdM resolvedFields present");
        failures += Expect(
            tradeCounterpartyOfferChangedSchema->resolvedFields.empty(),
            "NdM resolvedFields empty");
        failures += ExpectEffectiveFieldsMatch(
            *tradeCounterpartyOfferChangedSchema,
            cg::packet::receive::TradeCounterpartyOfferChanged::Spec,
            "NdM Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *tradeCounterpartyOfferChangedSchema,
            packet,
            "NdM resolved field order");
        failures += Expect(packet.fields.empty(), "NdM typed packet fields empty");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "NdM"),
        "NdM absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "NdM"),
        "NdM present in receive typed manifest");

    const PacketSchema* tradeGoldInfoSchema = FindPacket(packets, "hJq");
    failures += Expect(tradeGoldInfoSchema != nullptr, "hJq schema exists");
    if (tradeGoldInfoSchema != nullptr) {
        const cg::packet::receive::TradeGoldInfo value(100);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            tradeGoldInfoSchema->direction == "服务端->客户端",
            "hJq direction");
        failures += Expect(
            tradeGoldInfoSchema->legacyKind == "SEND",
            "hJq legacyKind");
        failures += Expect(
            tradeGoldInfoSchema->clientNamespace == "receive",
            "hJq namespace");
        failures += Expect(
            tradeGoldInfoSchema->xlsxStatus == "完成",
            "hJq xlsxStatus");
        failures += Expect(
            tradeGoldInfoSchema->description ==
                "SEND_TRLG CHAR_Trade_setTradeList 对方金币信息",
            "hJq description");
        failures += Expect(
            tradeGoldInfoSchema->rawFormat.empty(),
            "hJq rawFormat empty");
        failures += Expect(
            tradeGoldInfoSchema->formatEmpty,
            "hJq formatEmpty true");
        failures += Expect(
            tradeGoldInfoSchema->source == "xlsx",
            "hJq raw source");
        failures += Expect(
            tradeGoldInfoSchema->fields.empty(),
            "hJq raw fields empty");
        failures += Expect(
            tradeGoldInfoSchema->hasResolvedFormat,
            "hJq resolvedFormat present");
        failures += Expect(
            tradeGoldInfoSchema->resolvedFormat == "{gold:i62}",
            "hJq resolvedFormat");
        failures += Expect(
            tradeGoldInfoSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "hJq resolvedSource");
        failures += Expect(
            tradeGoldInfoSchema->hasResolvedFields,
            "hJq resolvedFields present");
        failures += Expect(
            tradeGoldInfoSchema->resolvedFields.size() == 1U,
            "hJq resolved field count");
        if (tradeGoldInfoSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = tradeGoldInfoSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "gold" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{gold:i62}" &&
                    field.source == "ServerC+ClientIda+src/packet",
                "hJq resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *tradeGoldInfoSchema,
            cg::packet::receive::TradeGoldInfo::Spec,
            "hJq Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *tradeGoldInfoSchema,
            packet,
            "hJq resolved field order");
        failures += Expect(value.gold == 100, "hJq gold member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "hJq"),
        "hJq absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "hJq"),
        "hJq present in receive typed manifest");

    const PacketSchema* tradeSelectionCancelSchema = FindPacket(packets, "xEg");
    failures += Expect(
        tradeSelectionCancelSchema != nullptr,
        "xEg schema exists");
    if (tradeSelectionCancelSchema != nullptr) {
        const cg::packet::receive::TradeSelectionCancelNotification value;
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            tradeSelectionCancelSchema->direction == "服务端->客户端",
            "xEg direction");
        failures += Expect(
            tradeSelectionCancelSchema->legacyKind == "SEND",
            "xEg legacyKind");
        failures += Expect(
            tradeSelectionCancelSchema->clientNamespace == "receive",
            "xEg namespace");
        failures += Expect(
            tradeSelectionCancelSchema->xlsxStatus == "暂缓",
            "xEg xlsxStatus");
        failures += Expect(
            tradeSelectionCancelSchema->description ==
                "SEND_TRCL CHAR_Trade_closeItem",
            "xEg description");
        failures += Expect(
            tradeSelectionCancelSchema->rawFormat.empty(),
            "xEg rawFormat empty");
        failures += Expect(
            tradeSelectionCancelSchema->formatEmpty,
            "xEg formatEmpty true");
        failures += Expect(
            tradeSelectionCancelSchema->source == "xlsx",
            "xEg raw source");
        failures += Expect(
            tradeSelectionCancelSchema->fields.empty(),
            "xEg raw fields empty");
        failures += Expect(
            tradeSelectionCancelSchema->hasResolvedFormat,
            "xEg resolvedFormat present");
        failures += Expect(
            tradeSelectionCancelSchema->resolvedFormat.empty(),
            "xEg resolvedFormat empty");
        failures += Expect(
            tradeSelectionCancelSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "xEg resolvedSource");
        failures += Expect(
            tradeSelectionCancelSchema->hasResolvedFields,
            "xEg resolvedFields present");
        failures += Expect(
            tradeSelectionCancelSchema->resolvedFields.empty(),
            "xEg resolvedFields empty");
        failures += ExpectEffectiveFieldsMatch(
            *tradeSelectionCancelSchema,
            cg::packet::receive::TradeSelectionCancelNotification::Spec,
            "xEg Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *tradeSelectionCancelSchema,
            packet,
            "xEg resolved field order");
        failures += Expect(packet.fields.empty(), "xEg typed packet fields empty");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "xEg"),
        "xEg absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "xEg"),
        "xEg present in receive typed manifest");

    const PacketSchema* petSkillUseResultSchema = FindPacket(packets, "BHe");
    failures += Expect(
        petSkillUseResultSchema != nullptr,
        "BHe schema exists");
    if (petSkillUseResultSchema != nullptr) {
        const cg::packet::receive::PetSkillUseResult value(1, 0, 2, 3);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            petSkillUseResultSchema->direction == "服务端->客户端",
            "BHe direction");
        failures += Expect(
            petSkillUseResultSchema->legacyKind == "SEND",
            "BHe legacyKind");
        failures += Expect(
            petSkillUseResultSchema->clientNamespace == "receive",
            "BHe namespace");
        failures += Expect(
            petSkillUseResultSchema->xlsxStatus == "暂缓",
            "BHe xlsxStatus");
        failures += Expect(
            petSkillUseResultSchema->description == "SEND_PS",
            "BHe description");
        failures += Expect(
            petSkillUseResultSchema->rawFormat.empty(),
            "BHe rawFormat empty");
        failures += Expect(
            petSkillUseResultSchema->formatEmpty,
            "BHe formatEmpty true");
        failures += Expect(
            petSkillUseResultSchema->source == "xlsx",
            "BHe raw source");
        failures += Expect(
            petSkillUseResultSchema->fields.empty(),
            "BHe raw fields empty");
        failures += Expect(
            petSkillUseResultSchema->hasResolvedFormat,
            "BHe resolvedFormat present");
        failures += Expect(
            petSkillUseResultSchema->resolvedFormat ==
                "{result:i62} {havepetindex:i62} {havepetskill:i62} {toindex:i62}",
            "BHe resolvedFormat");
        failures += Expect(
            petSkillUseResultSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "BHe resolvedSource");
        failures += Expect(
            petSkillUseResultSchema->hasResolvedFields,
            "BHe resolvedFields present");
        failures += Expect(
            petSkillUseResultSchema->resolvedFields.size() == 4U,
            "BHe resolved field count");
        if (petSkillUseResultSchema->resolvedFields.size() == 4U) {
            constexpr std::array<std::string_view, 4U> names = {{
                "result",
                "havepetindex",
                "havepetskill",
                "toindex",
            }};
            constexpr std::array<std::string_view, 4U> specs = {{
                "{result:i62}",
                "{havepetindex:i62}",
                "{havepetskill:i62}",
                "{toindex:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    petSkillUseResultSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "ServerC+ClientIda+src/packet",
                    "BHe resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *petSkillUseResultSchema,
            cg::packet::receive::PetSkillUseResult::Spec,
            "BHe Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petSkillUseResultSchema,
            packet,
            "BHe resolved field order");
        failures += Expect(
            value.result == 1 &&
                value.havePetIndex == 0 &&
                value.havePetSkill == 2 &&
                value.toIndex == 3,
            "BHe typed member order");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "BHe"),
        "BHe absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "BHe"),
        "BHe present in receive typed manifest");

    const PacketSchema* characterLevelUpPointStatusSchema =
        FindPacket(packets, "kg");
    failures += Expect(
        characterLevelUpPointStatusSchema != nullptr,
        "kg schema exists");
    if (characterLevelUpPointStatusSchema != nullptr) {
        const cg::packet::receive::CharacterLevelUpPointStatus value(100);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            characterLevelUpPointStatusSchema->direction == "服务端->客户端",
            "kg direction");
        failures += Expect(
            characterLevelUpPointStatusSchema->legacyKind == "SEND",
            "kg legacyKind");
        failures += Expect(
            characterLevelUpPointStatusSchema->clientNamespace == "receive",
            "kg namespace");
        failures += Expect(
            characterLevelUpPointStatusSchema->xlsxStatus == "完成",
            "kg xlsxStatus");
        failures += Expect(
            characterLevelUpPointStatusSchema->description == "SEND_LVUP",
            "kg description");
        failures += Expect(
            characterLevelUpPointStatusSchema->rawFormat.empty(),
            "kg rawFormat empty");
        failures += Expect(
            characterLevelUpPointStatusSchema->formatEmpty,
            "kg formatEmpty true");
        failures += Expect(
            characterLevelUpPointStatusSchema->source == "xlsx",
            "kg raw source");
        failures += Expect(
            characterLevelUpPointStatusSchema->fields.empty(),
            "kg raw fields empty");
        failures += Expect(
            characterLevelUpPointStatusSchema->hasResolvedFormat,
            "kg resolvedFormat present");
        failures += Expect(
            characterLevelUpPointStatusSchema->resolvedFormat == "{point:i62}",
            "kg resolvedFormat");
        failures += Expect(
            characterLevelUpPointStatusSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "kg resolvedSource");
        failures += Expect(
            characterLevelUpPointStatusSchema->hasResolvedFields,
            "kg resolvedFields present");
        failures += Expect(
            characterLevelUpPointStatusSchema->resolvedFields.size() == 1U,
            "kg resolved field count");
        if (characterLevelUpPointStatusSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                characterLevelUpPointStatusSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "point" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{point:i62}" &&
                    field.source == "ServerC+ClientIda+src/packet",
                "kg resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *characterLevelUpPointStatusSchema,
            cg::packet::receive::CharacterLevelUpPointStatus::Spec,
            "kg Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *characterLevelUpPointStatusSchema,
            packet,
            "kg resolved field order");
        failures += Expect(value.point == 100, "kg point member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "kg"),
        "kg absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "kg"),
        "kg present in receive typed manifest");

    const PacketSchema* petLevelUpPointsSchema = FindPacket(packets, "sA");
    failures += Expect(petLevelUpPointsSchema != nullptr, "sA schema exists");
    if (petLevelUpPointsSchema != nullptr) {
        const cg::packet::receive::PetLevelUpPoints value(2, 4);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            petLevelUpPointsSchema->direction == "服务端->客户端",
            "sA direction");
        failures += Expect(
            petLevelUpPointsSchema->legacyKind == "SEND",
            "sA legacyKind");
        failures += Expect(
            petLevelUpPointsSchema->clientNamespace == "receive",
            "sA namespace");
        failures += Expect(
            petLevelUpPointsSchema->xlsxStatus == "完成",
            "sA xlsxStatus");
        failures += Expect(
            petLevelUpPointsSchema->description == "SEND_PLVUP",
            "sA description");
        failures += Expect(
            petLevelUpPointsSchema->rawFormat.empty(),
            "sA rawFormat empty");
        failures += Expect(
            petLevelUpPointsSchema->formatEmpty,
            "sA formatEmpty true");
        failures += Expect(
            petLevelUpPointsSchema->source == "xlsx",
            "sA raw source");
        failures += Expect(
            petLevelUpPointsSchema->fields.empty(),
            "sA raw fields empty");
        failures += Expect(
            petLevelUpPointsSchema->hasResolvedFormat,
            "sA resolvedFormat present");
        failures += Expect(
            petLevelUpPointsSchema->resolvedFormat ==
                "{petSlot:i62} {levelUpPoints:i62}",
            "sA resolvedFormat");
        failures += Expect(
            petLevelUpPointsSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "sA resolvedSource");
        failures += Expect(
            petLevelUpPointsSchema->hasResolvedFields,
            "sA resolvedFields present");
        failures += Expect(
            petLevelUpPointsSchema->resolvedFields.size() == 2U,
            "sA resolved field count");
        if (petLevelUpPointsSchema->resolvedFields.size() == 2U) {
            constexpr std::array<std::string_view, 2U> names = {{
                "petSlot",
                "levelUpPoints",
            }};
            constexpr std::array<std::string_view, 2U> specs = {{
                "{petSlot:i62}",
                "{levelUpPoints:i62}",
            }};
            for (std::size_t index = 0U; index < names.size(); ++index) {
                const FieldSchema& field =
                    petLevelUpPointsSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == specs[index] &&
                        field.source == "ServerC+ClientIda+src/packet",
                    "sA resolved field");
            }
        }
        failures += ExpectEffectiveFieldsMatch(
            *petLevelUpPointsSchema,
            cg::packet::receive::PetLevelUpPoints::Spec,
            "sA Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *petLevelUpPointsSchema,
            packet,
            "sA resolved field order");
        failures += Expect(
            value.petSlot == 2 && value.levelUpPoints == 4,
            "sA typed member order");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "sA"),
        "sA absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "sA"),
        "sA present in receive typed manifest");

    const PacketSchema* soundEffectNotificationSchema =
        FindPacket(packets, "TFCu");
    failures += Expect(
        soundEffectNotificationSchema != nullptr,
        "TFCu schema exists");
    if (soundEffectNotificationSchema != nullptr) {
        const cg::packet::receive::SoundEffectNotification value(2);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            soundEffectNotificationSchema->direction == "服务端->客户端",
            "TFCu direction");
        failures += Expect(
            soundEffectNotificationSchema->legacyKind == "SEND",
            "TFCu legacyKind");
        failures += Expect(
            soundEffectNotificationSchema->clientNamespace == "receive",
            "TFCu namespace");
        failures += Expect(
            soundEffectNotificationSchema->xlsxStatus == "完成",
            "TFCu xlsxStatus");
        failures += Expect(
            soundEffectNotificationSchema->description == "SEND_SE",
            "TFCu description");
        failures += Expect(
            soundEffectNotificationSchema->rawFormat.empty(),
            "TFCu rawFormat empty");
        failures += Expect(
            soundEffectNotificationSchema->formatEmpty,
            "TFCu formatEmpty true");
        failures += Expect(
            soundEffectNotificationSchema->source == "xlsx",
            "TFCu raw source");
        failures += Expect(
            soundEffectNotificationSchema->fields.empty(),
            "TFCu raw fields empty");
        failures += Expect(
            soundEffectNotificationSchema->hasResolvedFormat,
            "TFCu resolvedFormat present");
        failures += Expect(
            soundEffectNotificationSchema->resolvedFormat ==
                "{senumber:i62}",
            "TFCu resolvedFormat");
        failures += Expect(
            soundEffectNotificationSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "TFCu resolvedSource");
        failures += Expect(
            soundEffectNotificationSchema->hasResolvedFields,
            "TFCu resolvedFields present");
        failures += Expect(
            soundEffectNotificationSchema->resolvedFields.size() == 1U,
            "TFCu resolved field count");
        if (soundEffectNotificationSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                soundEffectNotificationSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "senumber" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{senumber:i62}" &&
                    field.source == "ServerC+ClientIda+src/packet",
                "TFCu resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *soundEffectNotificationSchema,
            cg::packet::receive::SoundEffectNotification::Spec,
            "TFCu Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *soundEffectNotificationSchema,
            packet,
            "TFCu resolved field order");
        failures += Expect(
            value.soundNumber == 2,
            "TFCu typed member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "TFCu"),
        "TFCu absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "TFCu"),
        "TFCu present in receive typed manifest");

    const PacketSchema* backgroundMusicChangeSchema =
        FindPacket(packets, "wiB");
    failures += Expect(
        backgroundMusicChangeSchema != nullptr,
        "wiB schema exists");
    if (backgroundMusicChangeSchema != nullptr) {
        const cg::packet::receive::BackgroundMusicChange value(200);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            backgroundMusicChangeSchema->direction == "服务端->客户端",
            "wiB direction");
        failures += Expect(
            backgroundMusicChangeSchema->legacyKind == "SEND",
            "wiB legacyKind");
        failures += Expect(
            backgroundMusicChangeSchema->clientNamespace == "receive",
            "wiB namespace");
        failures += Expect(
            backgroundMusicChangeSchema->xlsxStatus == "完成",
            "wiB xlsxStatus");
        failures += Expect(
            backgroundMusicChangeSchema->description == "SEND_BGMW",
            "wiB description");
        failures += Expect(
            backgroundMusicChangeSchema->rawFormat.empty(),
            "wiB rawFormat empty");
        failures += Expect(
            backgroundMusicChangeSchema->formatEmpty,
            "wiB formatEmpty true");
        failures += Expect(
            backgroundMusicChangeSchema->source == "xlsx",
            "wiB raw source");
        failures += Expect(
            backgroundMusicChangeSchema->fields.empty(),
            "wiB raw fields empty");
        failures += Expect(
            backgroundMusicChangeSchema->hasResolvedFormat,
            "wiB resolvedFormat present");
        failures += Expect(
            backgroundMusicChangeSchema->resolvedFormat == "{sw:i62}",
            "wiB resolvedFormat");
        failures += Expect(
            backgroundMusicChangeSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "wiB resolvedSource");
        failures += Expect(
            backgroundMusicChangeSchema->hasResolvedFields,
            "wiB resolvedFields present");
        failures += Expect(
            backgroundMusicChangeSchema->resolvedFields.size() == 1U,
            "wiB resolved field count");
        if (backgroundMusicChangeSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                backgroundMusicChangeSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "sw" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{sw:i62}" &&
                    field.source == "ServerC+ClientIda+src/packet",
                "wiB resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *backgroundMusicChangeSchema,
            cg::packet::receive::BackgroundMusicChange::Spec,
            "wiB Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *backgroundMusicChangeSchema,
            packet,
            "wiB resolved field order");
        failures += Expect(
            value.backgroundMusicNumber == 200,
            "wiB typed member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "wiB"),
        "wiB absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "wiB"),
        "wiB present in receive typed manifest");

    const PacketSchema* checkCommandSchema = FindPacket(packets, "CCL");
    failures += Expect(checkCommandSchema != nullptr, "CCL schema exists");
    if (checkCommandSchema != nullptr) {
        const cg::packet::send::CheckCommand value(16719);
        const cg::packet::Packet packet = value.ToPacket();
        failures += Expect(
            checkCommandSchema->direction == "客户端->服务端",
            "CCL direction");
        failures += Expect(checkCommandSchema->legacyKind == "RECV", "CCL legacyKind");
        failures += Expect(checkCommandSchema->clientNamespace == "send", "CCL namespace");
        failures += Expect(checkCommandSchema->xlsxStatus == "完成", "CCL xlsxStatus");
        failures += Expect(
            checkCommandSchema->description == "RECV_CheckCommand 发送客户端时间",
            "CCL description");
        failures += Expect(checkCommandSchema->rawFormat == "{time:i62}", "CCL rawFormat");
        failures += Expect(!checkCommandSchema->formatEmpty, "CCL formatEmpty false");
        failures += Expect(checkCommandSchema->source == "xlsx", "CCL raw source");
        failures += Expect(checkCommandSchema->fields.size() == 1U, "CCL field count");
        if (checkCommandSchema->fields.size() == 1U) {
            failures += Expect(checkCommandSchema->fields[0].name == "time", "CCL field name");
            failures += Expect(checkCommandSchema->fields[0].typeToken == "i62", "CCL field type");
            failures += Expect(checkCommandSchema->fields[0].rawSpec == "{time:i62}", "CCL rawSpec");
            failures += Expect(checkCommandSchema->fields[0].source == "xlsx", "CCL field source");
        }
        failures += Expect(checkCommandSchema->hasResolvedFormat, "CCL resolvedFormat present");
        failures += Expect(
            checkCommandSchema->resolvedFormat == "{time:i62}",
            "CCL resolvedFormat");
        failures += Expect(
            checkCommandSchema->resolvedSource == "xlsx+ClientIda+src/packet",
            "CCL resolvedSource");
        failures += Expect(checkCommandSchema->hasResolvedFields, "CCL resolvedFields present");
        failures += Expect(
            checkCommandSchema->resolvedFields.size() == 1U,
            "CCL resolved field count");
        if (checkCommandSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = checkCommandSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "time" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{time:i62}" &&
                    field.source == "xlsx+ClientIda+src/packet",
                "CCL resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *checkCommandSchema,
            cg::packet::send::CheckCommand::Spec,
            "CCL Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *checkCommandSchema,
            packet,
            "CCL resolved field order");
        failures += Expect(value.time == 16719, "CCL typed member");
    }
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "CCL"),
        "CCL present in send typed manifest");
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "CCL"),
        "CCL absent from receive typed manifest");

    const PacketSchema* profileVersionSchema = FindPacket(packets, "HNP");
    failures += Expect(profileVersionSchema != nullptr, "HNP schema exists");
    if (profileVersionSchema != nullptr) {
        const cg::packet::receive::ProfileVersion profileVersion(1, 2, 100, -1);
        const cg::packet::Packet packet = profileVersion.ToPacket();
        failures += Expect(
            profileVersionSchema->direction == "服务端->客户端",
            "HNP direction");
        failures += Expect(profileVersionSchema->legacyKind == "SEND", "HNP legacyKind");
        failures += Expect(
            profileVersionSchema->description == "SEND_PRV \nCHAR_sendProfileVersion简介版本内容",
            "HNP description newline");
        failures += Expect(profileVersionSchema->rawFormat == "{data:es}", "HNP rawFormat");
        failures += Expect(!profileVersionSchema->formatEmpty, "HNP raw format non-empty");
        failures += Expect(profileVersionSchema->source == "xlsx", "HNP raw source");
        failures += Expect(profileVersionSchema->fields.size() == 1U, "HNP raw field count");
        if (profileVersionSchema->fields.size() == 1U) {
            failures += Expect(
                profileVersionSchema->fields[0].index == 0U &&
                    profileVersionSchema->fields[0].name == "data" &&
                    profileVersionSchema->fields[0].typeToken == "es" &&
                    profileVersionSchema->fields[0].rawSpec == "{data:es}" &&
                    profileVersionSchema->fields[0].source == "xlsx",
                "HNP raw data field");
        }
        failures += Expect(
            profileVersionSchema->resolvedFormat == "{data:es}",
            "HNP resolvedFormat");
        failures += Expect(
            profileVersionSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "HNP resolvedSource");
        failures += Expect(
            profileVersionSchema->resolvedFields.size() == 1U &&
                profileVersionSchema->resolvedFields[0].source ==
                    "xlsx+ServerC+ClientIda+src/packet",
            "HNP resolved field source");
        failures += ExpectEffectiveFieldsMatch(
            *profileVersionSchema,
            cg::packet::receive::ProfileVersion::Spec,
            "HNP Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *profileVersionSchema,
            packet,
            "HNP resolved field order");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "HNP"),
        "HNP absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "HNP"),
        "HNP present in receive typed manifest");

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
        failures += Expect(addressBookSchema->clientNamespace == "receive", "gnxd namespace");
        failures += Expect(addressBookSchema->xlsxStatus == "完成", "gnxd xlsxStatus");
        failures += Expect(addressBookSchema->description == "SEND_AB SendAddressbook", "gnxd description");
        failures += Expect(addressBookSchema->rawFormat == "{data:es}", "gnxd rawFormat");
        failures += Expect(!addressBookSchema->formatEmpty, "gnxd formatEmpty false");
        failures += Expect(addressBookSchema->source == "xlsx", "gnxd raw source");
        failures += Expect(addressBookSchema->fields.size() == 1U, "gnxd raw field count");
        if (addressBookSchema->fields.size() == 1U) {
            const FieldSchema& field = addressBookSchema->fields[0];
            failures += Expect(
                field.index == 0U &&
                    field.name == "data" &&
                    field.typeToken == "es" &&
                    field.rawSpec == "{data:es}" &&
                    field.source == "xlsx",
                "gnxd raw field");
        }
        failures += Expect(addressBookSchema->hasResolvedFormat, "gnxd resolvedFormat present");
        failures += Expect(addressBookSchema->resolvedFormat == "{data:es}", "gnxd resolvedFormat");
        failures += Expect(
            addressBookSchema->resolvedSource == "xlsx+ServerC+ClientIda+src/packet",
            "gnxd resolvedSource");
        failures += Expect(addressBookSchema->hasResolvedFields, "gnxd resolvedFields present");
        failures += Expect(addressBookSchema->resolvedFields.size() == 1U, "gnxd resolved field count");
        if (addressBookSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field = addressBookSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0U &&
                    field.name == "data" &&
                    field.typeToken == "es" &&
                    field.rawSpec == "{data:es}" &&
                    field.source == "xlsx+ServerC+ClientIda+src/packet",
                "gnxd resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *addressBookSchema,
            cg::packet::receive::AddressBook::Spec,
            "gnxd Spec matches effective fields");
        failures += Expect(packet.HasValue(), "gnxd packet build");
        if (packet.HasValue()) {
            failures += Expect(
                packet.Value().fields.size() == 1U &&
                    packet.Value().fields[0].type == cg::packet::PacketFieldType::SubPayload,
                "gnxd ToPacket payload type");
            failures += ExpectResolvedFieldsMatch(*addressBookSchema, packet.Value(), "gnxd resolved field order");
        }
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "gnxd"),
        "gnxd absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "gnxd"),
        "gnxd present in receive typed manifest");

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

    const PacketSchema* guildMemberRemovalNotificationSchema =
        FindPacket(packets, "oRut");
    failures += Expect(
        guildMemberRemovalNotificationSchema != nullptr,
        "oRut schema exists");
    if (guildMemberRemovalNotificationSchema != nullptr) {
        const cg::packet::receive::GuildMemberRemovalNotification
            notification(10);
        failures += Expect(
            guildMemberRemovalNotificationSchema->direction ==
                "服务端->客户端",
            "oRut direction");
        failures += Expect(
            guildMemberRemovalNotificationSchema->legacyKind == "SEND",
            "oRut legacyKind");
        failures += Expect(
            guildMemberRemovalNotificationSchema->clientNamespace ==
                "receive",
            "oRut namespace");
        failures += Expect(
            guildMemberRemovalNotificationSchema->xlsxStatus == "进行中",
            "oRut xlsxStatus");
        failures += Expect(
            guildMemberRemovalNotificationSchema->rawFormat ==
                "{registseqnumber}",
            "oRut rawFormat");
        failures += Expect(
            !guildMemberRemovalNotificationSchema->formatEmpty,
            "oRut formatEmpty false");
        failures += Expect(
            guildMemberRemovalNotificationSchema->source == "xlsx",
            "oRut raw source");
        failures += Expect(
            guildMemberRemovalNotificationSchema->fields.size() == 1U,
            "oRut raw field count");
        if (guildMemberRemovalNotificationSchema->fields.size() == 1U) {
            const FieldSchema& field =
                guildMemberRemovalNotificationSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "registseqnumber" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{registseqnumber}" &&
                    field.source == "xlsx",
                "oRut raw field");
        }

        failures += Expect(
            guildMemberRemovalNotificationSchema->hasResolvedFormat,
            "oRut resolvedFormat present");
        failures += Expect(
            guildMemberRemovalNotificationSchema->resolvedFormat ==
                "{registSeqNumber:i62}",
            "oRut resolvedFormat");
        failures += Expect(
            guildMemberRemovalNotificationSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "oRut resolvedSource");
        failures += Expect(
            guildMemberRemovalNotificationSchema->hasResolvedFields,
            "oRut resolvedFields present");
        failures += Expect(
            guildMemberRemovalNotificationSchema->resolvedFields.size() ==
                1U,
            "oRut resolved field count");
        if (guildMemberRemovalNotificationSchema->resolvedFields.size() ==
            1U) {
            const FieldSchema& field =
                guildMemberRemovalNotificationSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "registSeqNumber" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{registSeqNumber:i62}" &&
                    field.source ==
                        "xlsx+ServerC+ClientIda+src/packet",
                "oRut resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *guildMemberRemovalNotificationSchema,
            cg::packet::receive::GuildMemberRemovalNotification::Spec,
            "oRut Spec matches effective fields");
        failures += Expect(
            notification.registSeqNumber == 10,
            "oRut typed member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "oRut"),
        "oRut absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "oRut"),
        "oRut present in receive typed manifest");

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

    const PacketSchema* guildDepartureNotificationSchema =
        FindPacket(packets, "frQp");
    failures += Expect(
        guildDepartureNotificationSchema != nullptr,
        "frQp schema exists");
    if (guildDepartureNotificationSchema != nullptr) {
        const cg::packet::receive::GuildDepartureNotification notification(10);
        failures += Expect(
            guildDepartureNotificationSchema->direction ==
                "服务端->客户端",
            "frQp direction");
        failures += Expect(
            guildDepartureNotificationSchema->legacyKind == "SEND",
            "frQp legacyKind");
        failures += Expect(
            guildDepartureNotificationSchema->clientNamespace ==
                "receive",
            "frQp namespace");
        failures += Expect(
            guildDepartureNotificationSchema->xlsxStatus == "进行中",
            "frQp xlsxStatus");
        failures += Expect(
            guildDepartureNotificationSchema->rawFormat == "{guildId}",
            "frQp rawFormat");
        failures += Expect(
            !guildDepartureNotificationSchema->formatEmpty,
            "frQp formatEmpty false");
        failures += Expect(
            guildDepartureNotificationSchema->source == "xlsx",
            "frQp raw source");
        failures += Expect(
            guildDepartureNotificationSchema->fields.size() == 1U,
            "frQp raw field count");
        if (guildDepartureNotificationSchema->fields.size() == 1U) {
            const FieldSchema& field =
                guildDepartureNotificationSchema->fields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "guildId" &&
                    field.typeToken == "unknown" &&
                    field.rawSpec == "{guildId}" &&
                    field.source == "xlsx",
                "frQp raw field");
        }

        failures += Expect(
            guildDepartureNotificationSchema->hasResolvedFormat,
            "frQp resolvedFormat present");
        failures += Expect(
            guildDepartureNotificationSchema->resolvedFormat ==
                "{guildId:i62}",
            "frQp resolvedFormat");
        failures += Expect(
            guildDepartureNotificationSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "frQp resolvedSource");
        failures += Expect(
            guildDepartureNotificationSchema->hasResolvedFields,
            "frQp resolvedFields present");
        failures += Expect(
            guildDepartureNotificationSchema->resolvedFields.size() == 1U,
            "frQp resolved field count");
        if (guildDepartureNotificationSchema->resolvedFields.size() == 1U) {
            const FieldSchema& field =
                guildDepartureNotificationSchema->resolvedFields[0];
            failures += Expect(
                field.index == 0 &&
                    field.name == "guildId" &&
                    field.typeToken == "i62" &&
                    field.rawSpec == "{guildId:i62}" &&
                    field.source ==
                        "xlsx+ServerC+ClientIda+src/packet",
                "frQp resolved field");
        }
        failures += ExpectEffectiveFieldsMatch(
            *guildDepartureNotificationSchema,
            cg::packet::receive::GuildDepartureNotification::Spec,
            "frQp Spec matches effective fields");
        failures += Expect(
            notification.guildId == 10,
            "frQp typed member");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "frQp"),
        "frQp absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "frQp"),
        "frQp present in receive typed manifest");

    const PacketSchema* paletteChangeSchema =
        FindPacket(packets, "bJg");
    failures += Expect(
        paletteChangeSchema != nullptr,
        "bJg schema exists");
    if (paletteChangeSchema != nullptr) {
        failures += Expect(
            paletteChangeSchema->direction == "服务端->客户端",
            "bJg direction");
        failures += Expect(
            paletteChangeSchema->legacyKind == "SEND",
            "bJg legacyKind");
        failures += Expect(
            paletteChangeSchema->clientNamespace == "receive",
            "bJg namespace");
        failures += Expect(
            paletteChangeSchema->xlsxStatus == "完成",
            "bJg xlsxStatus");
        failures += Expect(
            paletteChangeSchema->rawFormat ==
                "{palnumber} {frameCnt}",
            "bJg rawFormat");
        failures += Expect(
            !paletteChangeSchema->formatEmpty,
            "bJg formatEmpty false");
        failures += Expect(
            paletteChangeSchema->source == "xlsx",
            "bJg raw source");

        constexpr std::array<std::string_view, 2U> RawNames = {{
            "palnumber",
            "frameCnt",
        }};
        constexpr std::array<std::string_view, 2U> RawSpecs = {{
            "{palnumber}",
            "{frameCnt}",
        }};
        failures += Expect(
            paletteChangeSchema->fields.size() == RawNames.size(),
            "bJg raw field count");
        if (paletteChangeSchema->fields.size() == RawNames.size()) {
            for (std::size_t index = 0U;
                 index < RawNames.size();
                 ++index) {
                const FieldSchema& field =
                    paletteChangeSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == RawNames[index] &&
                        field.typeToken == "unknown" &&
                        field.rawSpec == RawSpecs[index] &&
                        field.source == "xlsx",
                    "bJg raw field order");
            }
        }

        failures += Expect(
            paletteChangeSchema->hasResolvedFormat,
            "bJg resolvedFormat present");
        failures += Expect(
            paletteChangeSchema->resolvedFormat ==
                "{palnumber:i62} {frameCnt:i62}",
            "bJg resolvedFormat");
        failures += Expect(
            paletteChangeSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "bJg resolvedSource");
        failures += Expect(
            paletteChangeSchema->hasResolvedFields,
            "bJg resolvedFields present");

        constexpr std::array<std::string_view, 2U> ResolvedSpecs = {{
            "{palnumber:i62}",
            "{frameCnt:i62}",
        }};
        failures += Expect(
            paletteChangeSchema->resolvedFields.size() ==
                RawNames.size(),
            "bJg resolved field count");
        if (paletteChangeSchema->resolvedFields.size() ==
            RawNames.size()) {
            for (std::size_t index = 0U;
                 index < RawNames.size();
                 ++index) {
                const FieldSchema& field =
                    paletteChangeSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == RawNames[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec == ResolvedSpecs[index] &&
                        field.source ==
                            "xlsx+ServerC+ClientIda+src/packet",
                    "bJg resolved field order");
            }
        }

        const cg::packet::receive::PaletteChange paletteChange(13, 180);
        const cg::packet::Packet packet = paletteChange.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *paletteChangeSchema,
            cg::packet::receive::PaletteChange::Spec,
            "bJg Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *paletteChangeSchema,
            packet,
            "bJg resolved field order");
        failures += Expect(
            paletteChange.palNumber == 13 &&
                paletteChange.frameCount == 180,
            "bJg typed members");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "bJg"),
        "bJg absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "bJg"),
        "bJg present in receive typed manifest");

    const PacketSchema* shipActionSchema = FindPacket(packets, "PVfz");
    failures += Expect(shipActionSchema != nullptr, "PVfz schema exists");
    if (shipActionSchema != nullptr) {
        failures += Expect(
            shipActionSchema->direction == "服务端->客户端",
            "PVfz direction");
        failures += Expect(shipActionSchema->legacyKind == "SEND", "PVfz legacyKind");
        failures += Expect(
            shipActionSchema->clientNamespace == "receive",
            "PVfz namespace");
        failures += Expect(shipActionSchema->xlsxStatus == "完成", "PVfz xlsxStatus");
        failures += Expect(
            shipActionSchema->rawFormat == "{type} {mapId} {floorId} {x} {y}",
            "PVfz rawFormat");
        failures += Expect(!shipActionSchema->formatEmpty, "PVfz formatEmpty false");
        failures += Expect(shipActionSchema->source == "xlsx", "PVfz raw source");

        constexpr std::array<std::string_view, 5U> RawNames = {{
            "type",
            "mapId",
            "floorId",
            "x",
            "y",
        }};
        constexpr std::array<std::string_view, 5U> RawSpecs = {{
            "{type}",
            "{mapId}",
            "{floorId}",
            "{x}",
            "{y}",
        }};
        failures += Expect(
            shipActionSchema->fields.size() == RawNames.size(),
            "PVfz raw field count");
        if (shipActionSchema->fields.size() == RawNames.size()) {
            for (std::size_t index = 0U; index < RawNames.size(); ++index) {
                const FieldSchema& field = shipActionSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index)
                        && field.name == RawNames[index]
                        && field.typeToken == "unknown"
                        && field.rawSpec == RawSpecs[index]
                        && field.source == "xlsx",
                    "PVfz raw field order");
            }
        }

        failures += Expect(
            shipActionSchema->hasResolvedFormat,
            "PVfz resolvedFormat present");
        failures += Expect(
            shipActionSchema->resolvedFormat
                == "{type:i62} {mapId:i62} {floorId:i62} {x:i62} {y:i62}",
            "PVfz resolvedFormat");
        failures += Expect(
            shipActionSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "PVfz resolvedSource");
        failures += Expect(
            shipActionSchema->hasResolvedFields,
            "PVfz resolvedFields present");

        constexpr std::array<std::string_view, 5U> ResolvedSpecs = {{
            "{type:i62}",
            "{mapId:i62}",
            "{floorId:i62}",
            "{x:i62}",
            "{y:i62}",
        }};
        failures += Expect(
            shipActionSchema->resolvedFields.size() == RawNames.size(),
            "PVfz resolved field count");
        if (shipActionSchema->resolvedFields.size() == RawNames.size()) {
            for (std::size_t index = 0U; index < RawNames.size(); ++index) {
                const FieldSchema& field = shipActionSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index)
                        && field.name == RawNames[index]
                        && field.typeToken == "i62"
                        && field.rawSpec == ResolvedSpecs[index]
                        && field.source ==
                            "xlsx+ServerC+ClientIda+src/packet",
                    "PVfz resolved field order");
            }
        }

        const cg::packet::receive::ShipAction shipAction(
            -102,
            1000,
            10,
            20,
            30);
        const cg::packet::Packet packet = shipAction.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *shipActionSchema,
            cg::packet::receive::ShipAction::Spec,
            "PVfz Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *shipActionSchema,
            packet,
            "PVfz resolved field order");
        failures += Expect(
            shipAction.type == -102 &&
                shipAction.mapId == 1000 &&
                shipAction.floorId == 10 &&
                shipAction.x == 20 &&
                shipAction.y == 30,
            "PVfz typed members");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "PVfz"),
        "PVfz absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "PVfz"),
        "PVfz present in receive typed manifest");

    const PacketSchema* soundPlaybackSchema = FindPacket(packets, "murD");
    failures += Expect(soundPlaybackSchema != nullptr, "murD schema exists");
    if (soundPlaybackSchema != nullptr) {
        failures += Expect(
            soundPlaybackSchema->direction == "服务端->客户端",
            "murD direction");
        failures += Expect(
            soundPlaybackSchema->legacyKind == "SEND",
            "murD legacyKind");
        failures += Expect(
            soundPlaybackSchema->clientNamespace == "receive",
            "murD namespace");
        failures += Expect(
            soundPlaybackSchema->xlsxStatus == "完成",
            "murD xlsxStatus");
        failures += Expect(
            soundPlaybackSchema->description == "SEND_PLAYSE",
            "murD description");
        failures += Expect(
            soundPlaybackSchema->rawFormat.empty(),
            "murD rawFormat empty");
        failures += Expect(
            soundPlaybackSchema->formatEmpty,
            "murD formatEmpty true");
        failures += Expect(
            soundPlaybackSchema->source == "xlsx",
            "murD raw source");
        failures += Expect(
            soundPlaybackSchema->fields.empty(),
            "murD raw fields empty");
        failures += Expect(
            soundPlaybackSchema->hasResolvedFormat,
            "murD resolvedFormat present");
        failures += Expect(
            soundPlaybackSchema->resolvedFormat ==
                "{seno:i62} {x:i62} {y:i62}",
            "murD resolvedFormat");
        failures += Expect(
            soundPlaybackSchema->resolvedSource ==
                "ServerC+ClientIda+src/packet",
            "murD resolvedSource");
        failures += Expect(
            soundPlaybackSchema->hasResolvedFields,
            "murD resolvedFields present");

        constexpr std::array<std::string_view, 3U> Names = {{
            "seno",
            "x",
            "y",
        }};
        failures += Expect(
            soundPlaybackSchema->resolvedFields.size() == Names.size(),
            "murD resolved field count");
        if (soundPlaybackSchema->resolvedFields.size() == Names.size()) {
            for (std::size_t index = 0U; index < Names.size(); ++index) {
                const FieldSchema& field =
                    soundPlaybackSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index) &&
                        field.name == Names[index] &&
                        field.typeToken == "i62" &&
                        field.rawSpec ==
                            ("{" + std::string(Names[index]) + ":i62}") &&
                        field.source == "ServerC+ClientIda+src/packet",
                    "murD resolved field order");
            }
        }

        const cg::packet::receive::SoundPlayback soundPlayback(13, 27, 30);
        const cg::packet::Packet packet = soundPlayback.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *soundPlaybackSchema,
            cg::packet::receive::SoundPlayback::Spec,
            "murD Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *soundPlaybackSchema,
            packet,
            "murD resolved field order");
        failures += Expect(
            soundPlayback.soundNumber == 13 &&
                soundPlayback.x == 27 &&
                soundPlayback.y == 30,
            "murD typed members");
    }
    failures += Expect(
        !TypedManifestContainsCode(
            cg::packet::test::SendTypedPackets,
            "murD"),
        "murD absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(
            cg::packet::test::ReceiveTypedPackets,
            "murD"),
        "murD present in receive typed manifest");

    const PacketSchema* npcSoundEffectSchema = FindPacket(packets, "ftaN");
    failures += Expect(npcSoundEffectSchema != nullptr, "ftaN schema exists");
    if (npcSoundEffectSchema != nullptr) {
        failures += Expect(
            npcSoundEffectSchema->direction == "服务端->客户端",
            "ftaN direction");
        failures += Expect(npcSoundEffectSchema->legacyKind == "SEND", "ftaN legacyKind");
        failures += Expect(
            npcSoundEffectSchema->clientNamespace == "receive",
            "ftaN namespace");
        failures += Expect(npcSoundEffectSchema->xlsxStatus == "完成", "ftaN xlsxStatus");
        failures += Expect(
            npcSoundEffectSchema->rawFormat == "{seno} {x} {y}",
            "ftaN rawFormat");
        failures += Expect(!npcSoundEffectSchema->formatEmpty, "ftaN formatEmpty false");
        failures += Expect(npcSoundEffectSchema->source == "xlsx", "ftaN raw source");

        constexpr std::array<std::string_view, 3U> RawNames = {{
            "seno",
            "x",
            "y",
        }};
        constexpr std::array<std::string_view, 3U> RawSpecs = {{
            "{seno}",
            "{x}",
            "{y}",
        }};
        failures += Expect(
            npcSoundEffectSchema->fields.size() == RawNames.size(),
            "ftaN raw field count");
        if (npcSoundEffectSchema->fields.size() == RawNames.size()) {
            for (std::size_t index = 0U; index < RawNames.size(); ++index) {
                const FieldSchema& field = npcSoundEffectSchema->fields[index];
                failures += Expect(
                    field.index == static_cast<int>(index)
                        && field.name == RawNames[index]
                        && field.typeToken == "unknown"
                        && field.rawSpec == RawSpecs[index]
                        && field.source == "xlsx",
                    "ftaN raw field order");
            }
        }

        failures += Expect(
            npcSoundEffectSchema->hasResolvedFormat,
            "ftaN resolvedFormat present");
        failures += Expect(
            npcSoundEffectSchema->resolvedFormat == "{seno:i62} {x:i62} {y:i62}",
            "ftaN resolvedFormat");
        failures += Expect(
            npcSoundEffectSchema->resolvedSource ==
                "xlsx+ServerC+ClientIda+src/packet",
            "ftaN resolvedSource");
        failures += Expect(
            npcSoundEffectSchema->hasResolvedFields,
            "ftaN resolvedFields present");

        constexpr std::array<std::string_view, 3U> ResolvedSpecs = {{
            "{seno:i62}",
            "{x:i62}",
            "{y:i62}",
        }};
        failures += Expect(
            npcSoundEffectSchema->resolvedFields.size() == RawNames.size(),
            "ftaN resolved field count");
        if (npcSoundEffectSchema->resolvedFields.size() == RawNames.size()) {
            for (std::size_t index = 0U; index < RawNames.size(); ++index) {
                const FieldSchema& field = npcSoundEffectSchema->resolvedFields[index];
                failures += Expect(
                    field.index == static_cast<int>(index)
                        && field.name == RawNames[index]
                        && field.typeToken == "i62"
                        && field.rawSpec == ResolvedSpecs[index]
                        && field.source ==
                            "xlsx+ServerC+ClientIda+src/packet",
                    "ftaN resolved field order");
            }
        }

        const cg::packet::receive::NpcSoundEffect soundEffect(13, 27, 30);
        const cg::packet::Packet packet = soundEffect.ToPacket();
        failures += ExpectEffectiveFieldsMatch(
            *npcSoundEffectSchema,
            cg::packet::receive::NpcSoundEffect::Spec,
            "ftaN Spec matches effective fields");
        failures += ExpectResolvedFieldsMatch(
            *npcSoundEffectSchema,
            packet,
            "ftaN resolved field order");
        failures += Expect(
            soundEffect.soundNumber == 13 &&
                soundEffect.x == 27 &&
                soundEffect.y == 30,
            "ftaN typed members");
    }
    failures += Expect(
        !TypedManifestContainsCode(cg::packet::test::SendTypedPackets, "ftaN"),
        "ftaN absent from send typed manifest");
    failures += Expect(
        TypedManifestContainsCode(cg::packet::test::ReceiveTypedPackets, "ftaN"),
        "ftaN present in receive typed manifest");

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
