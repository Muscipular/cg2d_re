#include "packet/utils.hpp"

#include <initializer_list>
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

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string text;
    text.reserve(values.size());
    for (const unsigned int value : values) {
        text.push_back(static_cast<char>(value));
    }
    return text;
}

int ExpectSplit(std::string_view text, const std::vector<std::string>& expected, const char* message)
{
    const cg::packet::PacketResult<std::vector<std::string>> result = cg::packet::SplitPlainTokens(text);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int ExpectBuild(const std::vector<std::string>& tokens, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::BuildPlainTokenString(tokens);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
    }
    return failures;
}

int TestSplit()
{
    int failures = 0;
    failures += ExpectSplit("", {}, "split empty");
    failures += ExpectSplit("AB C", {"AB", "C"}, "split first byte token start");
    failures += ExpectSplit(" A", {"", "A"}, "split leading space");
    failures += ExpectSplit("A  B ", {"A", "", "B", ""}, "split repeated and trailing spaces");
    failures += ExpectSplit("A\nB", {"A"}, "split newline terminates");
    failures += ExpectSplit("A\rB", {"A"}, "split carriage return terminates");
    failures += ExpectSplit(Bytes({'A', 0x00U, 'B'}), {"A"}, "split nul terminates");
    failures += ExpectSplit(Bytes({0x00U, 'A'}), {}, "split leading nul as empty input");
    failures += ExpectSplit(Bytes({'A', 0x17U, 'B', '\n'}), {Bytes({'A', 0x17U, 'B'}), "B"}, "split analysis token marker");
    failures += ExpectSplit(Bytes({'A', 0x17U, 'B', ' ', 'C'}), {Bytes({'A', 0x17U, 'B'}), "B", "C"}, "split token marker before space");
    return failures;
}

int TestBuild()
{
    int failures = 0;
    failures += ExpectBuild({}, "", "build empty");
    failures += ExpectBuild({"A", "B"}, "A B ", "build trailing space");
    failures += ExpectBuild({"", "B", ""}, " B  ", "build empty tokens");
    return failures;
}

int TestEscapedStringBoundary()
{
    int failures = 0;
    failures += ExpectSplit("A\\SB", {"A\\SB"}, "split does not decode es space");
    failures += ExpectBuild({"A B"}, "A B ", "build does not encode es space");

    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString("A B");
    failures += Expect(static_cast<bool>(encoded), "es encode relation");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "A\\SB", "es encode keeps separate responsibility");
        failures += ExpectBuild({encoded.Value()}, "A\\SB ", "build keeps encoded token unchanged");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSplit();
    failures += TestBuild();
    failures += TestEscapedStringBoundary();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
