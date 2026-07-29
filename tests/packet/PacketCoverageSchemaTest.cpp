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
    bool hasResolvedFormat = false;
    std::string resolvedFormat;
    std::string resolvedSource;
    std::vector<FieldSchema> fields;
    bool hasResolvedFields = false;
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
    failures += Expect(cg::packet::test::SendTypedPackets.size() == 38U, "send typed manifest count 38");
    failures += Expect(cg::packet::test::ReceiveTypedPackets.size() == 69U, "receive typed manifest count 69");
    failures += Expect(
        cg::packet::test::SendTypedPackets.size() + cg::packet::test::ReceiveTypedPackets.size() == 107U,
        "typed manifest count 107");

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
    failures += Expect(codes.size() == 107U, "typed manifest unique code count 107");
    return failures;
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
