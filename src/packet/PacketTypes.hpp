#ifndef CG_ENGINE_PACKET_TYPES_H
#define CG_ENGINE_PACKET_TYPES_H

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cg::packet
{
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
        Payload,
        Unknown,
    };

    struct PacketField
    {
        std::string name;
        PacketFieldType type = PacketFieldType::Unknown;
        std::string value;
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

        static PacketError Make(PacketErrorCode errorCode, std::string errorMessage)
        {
            PacketError error;
            error.code = errorCode;
            error.message = std::move(errorMessage);
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
