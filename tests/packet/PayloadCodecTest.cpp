#include "packet/Packet.hpp"

#include <cstddef>
#include <iostream>
#include <span>
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

int ExpectParts(
    const std::vector<std::string>& actual,
    const std::vector<std::string>& expected,
    const char* message)
{
    int failures = Expect(actual.size() == expected.size(), message);
    if (actual.size() != expected.size()) {
        return failures;
    }

    for (std::size_t index = 0; index < expected.size(); ++index) {
        failures += Expect(actual[index] == expected[index], message);
    }
    return failures;
}

int ExpectSplit(
    std::string_view text,
    std::string_view delimiter,
    const std::vector<std::string>& expected,
    const char* message)
{
    const cg::packet::PacketResult<std::vector<std::string>> result =
        cg::packet::SplitPayload(text, delimiter);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += ExpectParts(result.Value(), expected, message);
    }
    return failures;
}

int ExpectBuild(
    const std::vector<std::string>& parts,
    std::string_view delimiter,
    std::string_view expected,
    const char* message)
{
    const cg::packet::PacketResult<std::string> result =
        cg::packet::BuildPayload(std::span<const std::string>(parts.data(), parts.size()), delimiter);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectRoundTrip(
    const std::vector<std::string>& parts,
    std::string_view delimiter,
    const char* message)
{
    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPayload(std::span<const std::string>(parts.data(), parts.size()), delimiter);
    int failures = Expect(static_cast<bool>(built), message);
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::vector<std::string>> split =
        cg::packet::SplitPayload(built.Value(), delimiter);
    failures += Expect(static_cast<bool>(split), message);
    if (split.HasValue()) {
        failures += ExpectParts(split.Value(), parts, message);
    }
    return failures;
}

int ExpectSplitError(
    std::string_view text,
    std::string_view delimiter,
    cg::packet::PacketErrorCode expected,
    const char* message)
{
    const cg::packet::PacketResult<std::vector<std::string>> result =
        cg::packet::SplitPayload(text, delimiter);
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (result.HasError()) {
        failures += Expect(result.Error().code == expected, message);
    }
    return failures;
}

int ExpectBuildError(
    const std::vector<std::string>& parts,
    std::string_view delimiter,
    cg::packet::PacketErrorCode expected,
    const char* message)
{
    const cg::packet::PacketResult<std::string> result =
        cg::packet::BuildPayload(std::span<const std::string>(parts.data(), parts.size()), delimiter);
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (result.HasError()) {
        failures += Expect(result.Error().code == expected, message);
    }
    return failures;
}

int TestSplitAndBuild()
{
    int failures = 0;
    failures += ExpectSplit(
        "name|payload|name2|payload2|",
        "|",
        {"name", "payload", "name2", "payload2", ""},
        "split pipe payload");
    failures += ExpectBuild(
        {"name", "payload", "name2", "payload2", ""},
        "|",
        "name|payload|name2|payload2|",
        "build pipe payload");
    failures += ExpectSplit(
        "0\\z2\\z99\\z-1",
        "\\z",
        {"0", "2", "99", "-1"},
        "split z delimiter");
    failures += ExpectSplit(
        "a,b,,c,",
        ",",
        {"a", "b", "", "c", ""},
        "split comma empty fields");
    failures += ExpectSplit(
        "|a||b|",
        "|",
        {"", "a", "", "b", ""},
        "split leading repeated trailing delimiter");
    failures += ExpectSplit("", "|", {""}, "split empty string as one empty field");
    failures += ExpectBuild({""}, "|", "", "build one empty field");
    return failures;
}

int TestNestedDajSample()
{
    const std::string dajPayload = "0|100\\zAlice\\y1\\z200\\yBob|1|ok";

    int failures = 0;
    const cg::packet::PacketResult<std::vector<std::string>> outer =
        cg::packet::SplitPayload(dajPayload, "|");
    failures += Expect(static_cast<bool>(outer), "Daj outer split");
    if (!outer.HasValue()) {
        return failures;
    }

    failures += ExpectParts(outer.Value(), {"0", "100\\zAlice\\y1\\z200\\yBob", "1", "ok"}, "Daj outer fields");

    const cg::packet::PacketResult<std::vector<std::string>> chars =
        cg::packet::SplitPayload(outer.Value()[1], "\\z");
    failures += Expect(static_cast<bool>(chars), "Daj nested split z");
    if (!chars.HasValue()) {
        return failures;
    }

    failures += ExpectParts(chars.Value(), {"100", "Alice\\y1", "200\\yBob"}, "Daj nested z fields");

    const cg::packet::PacketResult<std::vector<std::string>> charFields =
        cg::packet::SplitPayload(chars.Value()[1], "\\y");
    failures += Expect(static_cast<bool>(charFields), "Daj nested split y");
    if (charFields.HasValue()) {
        failures += ExpectParts(charFields.Value(), {"Alice", "1"}, "Daj nested y fields");
    }
    return failures;
}

int TestDelimiterRoundTrip()
{
    int failures = 0;
    const std::vector<std::string> parts = {"", "alpha", "", "beta", ""};
    for (const std::string& delimiter : std::vector<std::string>{"|", ",", "\\z", "\\c", "\\y", "\\n"}) {
        failures += ExpectRoundTrip(parts, delimiter, "delimiter roundtrip");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectSplitError("abc", "", cg::packet::PacketErrorCode::DecodeFailed, "split empty delimiter");
    failures += ExpectBuildError({"abc"}, "", cg::packet::PacketErrorCode::EncodeFailed, "build empty delimiter");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSplitAndBuild();
    failures += TestNestedDajSample();
    failures += TestDelimiterRoundTrip();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
