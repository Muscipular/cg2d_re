#include "LegacyFormat.hpp"
#include "LegacyFloatFormat.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyStringCompareState.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyMemory.hpp"

#include <bit>
#include <cstring>

namespace cg::platform {

namespace {

// 原58F8A8/58F8B0：供同一位集构造循环消费，含终止]及NUL。
constexpr std::uint8_t kStringScanSet58F8A8[] = {' ', '\t', '-', '\r', ']', 0};
constexpr std::uint8_t kCharacterScanSet58F8B0[] = {']', 0};

} // namespace

std::int32_t ScanLegacyInput_53DEF4(
    LegacyFile* const stream, const char* format, std::va_list arguments)
{
    std::int32_t consumed = 0;
    std::int32_t assigned = 0;
    std::uint8_t completed = 0;
    // 原C与DstCh未设初值；后续字段复用，不能为未读取/未转换路径补零。
    std::int32_t current;
    wchar_t converted;
    char sourceCharacter[4];
    const auto classify = [](const std::int32_t character, const std::uint32_t mask) {
        if (gLegacyCtypeMaxBytes58FE3C > 1) {
            return ClassifyLegacyCharacter_541955(character, mask);
        }
        const auto* const types = reinterpret_cast<const std::uint8_t*>(gCtypePointer58FC30);
        return static_cast<std::uint32_t>(types[character * 2]) & mask;
    };
    const auto read = [&consumed, stream]() {
        ++consumed;
        return ReadLegacyFormatByte_53E950(stream);
    };
    const auto unread = [&consumed, stream](const std::int32_t character) {
        --consumed;
        UnreadLegacyFormatByte_53E96A(character, stream);
    };

    while (*format != '\0') {
        if (classify(static_cast<std::uint8_t>(*format), 8) != 0) {
            --consumed;
            const std::int32_t character = SkipLegacyFormatSpace_53E981(&consumed, stream);
            UnreadLegacyFormatByte_53E96A(character, stream);
            do {
                ++format;
            } while (ClassifyLegacySpace_53BDEF(static_cast<std::uint8_t>(*format)) != 0);
        }
        if (*format != '%') {
            current = read();
            const auto expected = static_cast<std::uint8_t>(*format++);
            if (expected != current) {
                unread(current);
                goto finish;
            }
            const auto* const types = reinterpret_cast<const std::uint8_t*>(gCtypePointer58FC30);
            if ((types[static_cast<std::uint8_t>(current) * 2 + 1] & 0x80u) != 0) {
                const std::int32_t second = read();
                const auto expectedSecond = static_cast<std::uint8_t>(*format++);
                if (expectedSecond != second) {
                    unread(second);
                    unread(current);
                    goto finish;
                }
                --consumed;
            }
        } else {
            std::uint8_t inverted = 0;
            std::uint8_t negative = 0;
            std::uint8_t suppressed = 0;
            std::uint8_t length = 1;
            std::uint8_t wideMode = 0;
            sourceCharacter[3] = 0; // 原var_35与宽转换源+3同址，字段入口清零。
            std::uint32_t numeric = 0;
            std::uint64_t numeric64;
            std::uintptr_t address = 0;
            std::int32_t digits = 0;
            std::int32_t widthSpecified = 0;
            std::int32_t width = 0;
            std::uint32_t useInt64 = 0;
            for (;;) {
                const auto character = static_cast<std::uint8_t>(*++format);
                if (classify(character, 4) != 0) {
                    ++widthSpecified;
                    width = character + width * 10 - '0';
                    continue;
                }
                if (character == 'F' || character == 'N') {
                    continue;
                }
                if (character == '*') {
                    ++suppressed;
                    continue;
                }
                if (character == 'I' && format[1] == '6' && format[2] == '4') {
                    ++useInt64;
                    numeric64 = 0;
                    format += 2;
                    continue;
                }
                if (character == 'L') {
                    ++length;
                    continue;
                }
                if (character == 'h') {
                    --length;
                    --wideMode;
                    continue;
                }
                if (character == 'l') {
                    ++length;
                    ++wideMode;
                    continue;
                }
                if (character == 'w') {
                    ++wideMode;
                    continue;
                }
                break;
            }
            if (wideMode == 0) {
                wideMode = *format == 'S' || *format == 'C' ? 1u : 0xFFu;
            }
            const std::int32_t kind = static_cast<std::uint8_t>(*format) | 0x20;
            const bool textField = kind == 'c' || kind == 's' || kind == '{';
            const bool floatField = kind >= 'e' && kind <= 'g';
            const bool integerField = kind == 'd' || kind == 'i' || kind == 'o'
                || kind == 'u' || kind == 'x' || kind == 'p';
            const bool recognized = textField || floatField || integerField || kind == 'n';
            void* destination;
            if (suppressed == 0 && recognized) {
                // Win64参数均为目标地址；未知字面转换的原预取/回滚不消费参数。
                destination = va_arg(arguments, void*);
            }
            if (kind != 'n') {
                current = kind == 'c' || kind == '{'
                    ? read() : SkipLegacyFormatSpace_53E981(&consumed, stream);
            }
            if (widthSpecified != 0 && width == 0) {
                unread(current);
                goto finish;
            }

            if (floatField) {
                char text[352];
                char* output = text;
                std::int32_t character = current;
                if (character == '-') {
                    *output++ = '-';
                }
                if (character == '-' || character == '+') {
                    --width;
                    character = current = read();
                }
                if (widthSpecified == 0 || width > 349) {
                    width = 349;
                }
                while (classify(character, 4) != 0) {
                    if (width-- == 0) {
                        break;
                    }
                    ++digits;
                    *output++ = std::bit_cast<char>(static_cast<std::uint8_t>(character));
                    character = current = read();
                }
                if (static_cast<std::uint8_t>(character) == gLegacyDecimalPoint58FE40) {
                    if (width-- != 0) {
                        character = read();
                        *output++ = std::bit_cast<char>(gLegacyDecimalPoint58FE40);
                        current = character;
                        while (classify(character, 4) != 0) {
                            if (width-- == 0) {
                                break;
                            }
                            ++digits;
                            *output++ = std::bit_cast<char>(static_cast<std::uint8_t>(character));
                            character = current = read();
                        }
                    }
                }
                if (digits != 0 && (character == 'e' || character == 'E')) {
                    if (width-- != 0) {
                        *output++ = 'e';
                        character = current = read();
                        if (character == '-') {
                            *output++ = '-';
                        }
                        if (character == '-' || character == '+') {
                            if (width-- == 0) {
                                width = 0;
                            } else {
                                character = current = read();
                            }
                        }
                        while (classify(character, 4) != 0) {
                            if (width-- == 0) {
                                break;
                            }
                            ++digits;
                            *output++ = std::bit_cast<char>(static_cast<std::uint8_t>(character));
                            character = current = read();
                        }
                    }
                }
                unread(character);
                if (digits == 0) {
                    goto finish;
                }
                if (suppressed == 0) {
                    ++assigned;
                    *output = '\0';
                    gLegacyFloatAssign58FB58(
                        static_cast<std::int32_t>(std::bit_cast<std::int8_t>(length)) - 1,
                        destination, text);
                }
            } else if (textField) {
                const bool wide = std::bit_cast<std::int8_t>(wideMode) > 0;
                const std::uint8_t* setText;
                if (kind == 'c') {
                    if (widthSpecified == 0) {
                        ++width;
                        widthSpecified = 1;
                    }
                    setText = kCharacterScanSet58F8B0;
                    inverted = 0xFFu;
                } else if (kind == 's') {
                    setText = kStringScanSet58F8A8;
                    inverted = 0xFFu;
                } else {
                    ++format;
                    setText = reinterpret_cast<const std::uint8_t*>(format);
                    if (*setText == '^') {
                        ++setText;
                        inverted = 0xFFu;
                    }
                }
                std::uint8_t set[32];
                static_cast<void>(FillLegacyMemory_5419D0(
                    set, 0, static_cast<std::uint32_t>(sizeof(set))));
                std::uint8_t previous = 0;
                if (kind == '{' && *setText == ']') {
                    previous = ']';
                    ++setText;
                    set[11] = 0x20;
                }
                while (*setText != ']') {
                    const std::uint8_t character = *setText++;
                    if (character == '-' && previous != 0 && *setText != ']') {
                        const std::uint8_t next = *setText++;
                        const std::uint32_t low = previous < next ? previous : next;
                        const std::uint32_t high = previous < next ? next : previous;
                        for (std::uint32_t value = low; value <= high; ++value) {
                            set[value >> 3] |= static_cast<std::uint8_t>(1u << (value & 7u));
                        }
                        previous = 0;
                    } else {
                        previous = character;
                        set[character >> 3] |= static_cast<std::uint8_t>(1u << (character & 7u));
                    }
                }
                if (*setText == 0) {
                    goto finish;
                }
                if (kind == '{') {
                    format = reinterpret_cast<const char*>(setText);
                }
                auto* output = suppressed == 0 ? static_cast<std::uint8_t*>(destination) : nullptr;
                std::uint32_t accepted = 0;
                unread(current);
                for (;;) {
                    if (widthSpecified != 0 && width-- == 0) {
                        break;
                    }
                    current = read();
                    if (current == -1
                        || ((set[current >> 3] ^ inverted) & (1u << (current & 7))) == 0) {
                        unread(current);
                        break;
                    }
                    if (suppressed == 0) {
                        if (wide) {
                            sourceCharacter[0] = std::bit_cast<char>(
                                static_cast<std::uint8_t>(current));
                            const auto* const types =
                                reinterpret_cast<const std::uint8_t*>(gCtypePointer58FC30);
                            if ((types[static_cast<std::uint8_t>(current) * 2 + 1] & 0x80u) != 0) {
                                sourceCharacter[1] = std::bit_cast<char>(
                                    static_cast<std::uint8_t>(read()));
                            }
                            static_cast<void>(Mbtowc_544552(&converted, sourceCharacter,
                                static_cast<std::uint32_t>(gLegacyCtypeMaxBytes58FE3C)));
                            std::memcpy(output, &converted, 2);
                            output += 2;
                        } else {
                            *output++ = static_cast<std::uint8_t>(current);
                        }
                        destination = output;
                    }
                    ++accepted;
                }
                if (accepted == 0) {
                    goto finish;
                }
                if (suppressed == 0) {
                    ++assigned;
                    if (kind != 'c') {
                        if (wide) {
                            const std::uint16_t zero = 0;
                            std::memcpy(destination, &zero, 2);
                        } else {
                            *static_cast<std::uint8_t*>(destination) = 0;
                        }
                    }
                }
            } else if (integerField || kind == 'n') {
                if (kind == 'n') {
                    numeric = static_cast<std::uint32_t>(consumed);
                } else {
                    std::int32_t radixKind = kind == 'i' ? 'd' : kind;
                    std::int32_t character = current;
                    bool stoppedBySign = false;
                    if (kind == 'p') {
                        length = 1;
                    }
                    if (character == '-') {
                        negative = 1;
                    }
                    if (character == '-' || character == '+') {
                        --width;
                        if (width == 0 && widthSpecified != 0) {
                            stoppedBySign = true;
                        } else {
                            character = current = read();
                        }
                    }
                    if ((kind == 'i' || kind == 'x') && character == '0') {
                        character = current = read();
                        if (static_cast<std::uint8_t>(character) == 'x'
                            || static_cast<std::uint8_t>(character) == 'X') {
                            character = current = read();
                            radixKind = 'x';
                        } else {
                            digits = 1;
                            if (radixKind != 'x') {
                                radixKind = 'o';
                            } else {
                                unread(character);
                                character = current = '0';
                            }
                        }
                    }
                    if (!stoppedBySign) {
                        for (;;) {
                            const bool hex = radixKind == 'x'
                                || (useInt64 == 0 && radixKind == 'p');
                            if (classify(character, hex ? 0x80u : 4u) == 0
                                || (!hex && radixKind == 'o' && character >= '8')) {
                                unread(character);
                                break;
                            }
                            if (useInt64 != 0) {
                                if (hex) {
                                    numeric64 <<= 4;
                                } else if (radixKind == 'o') {
                                    numeric64 <<= 3;
                                } else {
                                    numeric64 *= 10;
                                }
                            } else if (kind == 'p') {
                                address <<= 4;
                            } else if (hex) {
                                numeric <<= 4;
                            } else if (radixKind == 'o') {
                                numeric <<= 3;
                            } else {
                                numeric *= 10;
                            }
                            if (hex) {
                                character = current = NormalizeLegacyHexCharacter_53E919(character);
                            }
                            ++digits;
                            if (useInt64 != 0) {
                                const auto digit = static_cast<std::int32_t>(character - '0');
                                numeric64 += static_cast<std::uint64_t>(
                                    static_cast<std::int64_t>(digit));
                            } else if (kind == 'p') {
                                address += static_cast<std::uintptr_t>(character - '0');
                            } else {
                                numeric += static_cast<std::uint32_t>(character - '0');
                            }
                            if (widthSpecified != 0 && --width == 0) {
                                break;
                            }
                            character = current = read();
                        }
                    }
                    if (negative != 0) {
                        if (useInt64 != 0) {
                            numeric64 = 0u - numeric64;
                        } else if (kind == 'p') {
                            address = 0u - address;
                        } else {
                            numeric = 0u - numeric;
                        }
                    }
                    if (digits == 0) {
                        goto finish;
                    }
                    if (suppressed == 0) {
                        ++assigned;
                    }
                }
                if (suppressed == 0) {
                    if (kind == 'p') {
                        if (useInt64 != 0) {
                            address = static_cast<std::uintptr_t>(numeric64);
                        }
                        void* const pointer = reinterpret_cast<void*>(address);
                        std::memcpy(destination, &pointer, sizeof(pointer));
                    } else if (useInt64 != 0) {
                        // 原I64n仍写入口清零的numeric64；原顺序先低W32再高W32。
                        const auto low = static_cast<std::uint32_t>(numeric64);
                        const auto high = static_cast<std::uint32_t>(numeric64 >> 32);
                        std::memcpy(destination, &low, 4);
                        std::memcpy(static_cast<std::uint8_t*>(destination) + 4, &high, 4);
                    } else if (length != 0) {
                        std::memcpy(destination, &numeric, 4);
                    } else {
                        const auto low = static_cast<std::uint16_t>(numeric);
                        std::memcpy(destination, &low, 2);
                    }
                }
            } else {
                if (static_cast<std::uint8_t>(*format) != current) {
                    unread(current);
                    goto finish;
                }
                --completed;
            }
            ++completed;
            ++format;
        }
        if (current == -1 && !(format[0] == '%' && format[1] == 'n')) {
            goto eof;
        }
    }
finish:
    if (current != -1) {
        return assigned;
    }
eof:
    return assigned == 0 && completed == 0 ? -1 : assigned;
}

std::int32_t ScanLegacyString_53CADC(const char* const buffer, const char* const format, ...)
{
    LegacyFile stream;
    stream.flags = 0x49;
    stream.base = const_cast<char*>(buffer);
    stream.current = const_cast<char*>(buffer);
    stream.remaining = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(StrLen_53F9E0(buffer)));
    std::va_list arguments;
    va_start(arguments, format);
    const std::int32_t result = ScanLegacyInput_53DEF4(&stream, format, arguments);
    va_end(arguments);
    return result;
}

} // namespace cg::platform
