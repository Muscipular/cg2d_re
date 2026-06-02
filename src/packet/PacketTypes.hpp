#ifndef CG_ENGINE_PACKET_TYPES_H
#define CG_ENGINE_PACKET_TYPES_H

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifndef CG_PACKET_ENABLE_DIAGNOSTIC_TEXT
#if !defined(NDEBUG)
#define CG_PACKET_ENABLE_DIAGNOSTIC_TEXT 1
#else
#define CG_PACKET_ENABLE_DIAGNOSTIC_TEXT 0
#endif
#endif

#if CG_PACKET_ENABLE_DIAGNOSTIC_TEXT
#define CG_PACKET_DIAGNOSTIC_TEXT(text) (text)
#define CG_PACKET_FIELD_NAME_MATCHES(lhs, rhs) ((lhs) == (rhs))
#else
#define CG_PACKET_DIAGNOSTIC_TEXT(text) ("")
#define CG_PACKET_FIELD_NAME_MATCHES(lhs, rhs) ((void)sizeof(lhs), (void)sizeof(rhs), true)
#endif

namespace cg::packet
{
    [[nodiscard]] inline constexpr std::string_view PacketDiagnosticText(std::string_view text) noexcept
    {
#if CG_PACKET_ENABLE_DIAGNOSTIC_TEXT
        return text;
#else
        (void)text;
        return {};
#endif
    }

    [[nodiscard]] inline std::string PacketDiagnosticString(std::string_view text)
    {
#if CG_PACKET_ENABLE_DIAGNOSTIC_TEXT
        return std::string(text);
#else
        (void)text;
        return {};
#endif
    }

    struct PacketCode
    {
        std::string value;

        PacketCode() = default;

        explicit PacketCode(std::string_view code)
            : value(code)
        {
        }

        [[nodiscard]] std::string_view View() const noexcept
        {
            return value;
        }
    };

    inline bool operator==(const PacketCode& lhs, const PacketCode& rhs) noexcept
    {
        return lhs.value == rhs.value;
    }

    inline bool operator!=(const PacketCode& lhs, const PacketCode& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    enum class PacketDirection
    {
        ClientToServer,
        ServerToClient,
    };

    enum class PacketFieldType
    {
        Int62,
        EscapedString,
        SubPayload,
        Unknown,
    };

    struct PacketField
    {
        std::string name;
        PacketFieldType type = PacketFieldType::Unknown;
        std::string value;

        PacketField() = default;

        PacketField(std::string_view fieldName, PacketFieldType fieldType, std::string fieldValue)
            : name(PacketDiagnosticString(fieldName)),
              type(fieldType),
              value(std::move(fieldValue))
        {
        }
    };

    struct Packet
    {
        PacketCode code;
        PacketDirection direction = PacketDirection::ClientToServer;
        std::vector<PacketField> fields;

        Packet() = default;

        Packet(PacketCode packetCode, PacketDirection packetDirection)
            : code(std::move(packetCode)),
              direction(packetDirection)
        {
        }
    };

    enum class PacketErrorCode
    {
        None,
        EmptyInput,
        InvalidCode,
        UnknownCode,
        MissingField,
        FieldCountMismatch,
        InvalidFieldType,
        DecodeFailed,
        EncodeFailed,
    };

    struct PacketError
    {
        PacketErrorCode code = PacketErrorCode::None;
        std::string message;
        std::optional<PacketCode> packetCode;
        std::optional<std::size_t> fieldIndex;

        static PacketError Make(PacketErrorCode errorCode, std::string_view errorMessage)
        {
            PacketError error;
            error.code = errorCode;
            error.message = PacketDiagnosticString(errorMessage);
            return error;
        }
    };

    template <typename T>
    class PacketResult
    {
    public:
        static PacketResult Success(T value)
        {
            PacketResult result;
            result.value_.emplace(std::move(value));
            return result;
        }

        static PacketResult Failure(PacketError error)
        {
            PacketResult result;
            result.error_.emplace(std::move(error));
            return result;
        }

        [[nodiscard]] bool HasValue() const noexcept
        {
            return value_.has_value();
        }

        [[nodiscard]] bool HasError() const noexcept
        {
            return error_.has_value();
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return HasValue();
        }

        [[nodiscard]] const T& Value() const noexcept
        {
            return *value_;
        }

        [[nodiscard]] T& Value() noexcept
        {
            return *value_;
        }

        [[nodiscard]] const PacketError& Error() const noexcept
        {
            return *error_;
        }

    private:
        PacketResult() = default;

        std::optional<T> value_;
        std::optional<PacketError> error_;
    };
}

#endif // CG_ENGINE_PACKET_TYPES_H
