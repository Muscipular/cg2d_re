#include "cg/resource/GraphicDecode.hpp"
#include "cg/resource/GraphicInfo.hpp"

#include <bit>
#include <cstring>

namespace cg::resource {

bool IsPngGraphicData(const std::uint8_t* const source) noexcept
{
    constexpr std::uint8_t signature[] {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    return std::memcmp(source, signature, sizeof(signature)) == 0;
}

// 原函数：0x0052D5C0。
std::uint8_t* ReadGraphicData(
    std::uint8_t const* source,
    std::uint8_t* const* destination,
    std::uint32_t* widthOut,
    std::uint32_t* heightOut,
    std::uint32_t* lengthOut,
    std::uint32_t* paletteLengthOut) noexcept {
    enum : std::uint8_t {
        Compressed = 0x01,
        ExtendedHeader = 0x02,
        ExtendedRunLength = 0x10,
        LongRunLength = 0x20,
        ZeroFill = 0x40,
        Repeat = 0x80,
        RunLengthMask = 0x0F
    };

    // 源头字段按小端逐字节读取，不依赖未对齐 uint32_t 解引用或结构体布局。
    const auto readUint32Le = [](const std::uint8_t* bytes) noexcept {
        return static_cast<std::uint32_t>(bytes[0])
            | (static_cast<std::uint32_t>(bytes[1]) << 8)
            | (static_cast<std::uint32_t>(bytes[2]) << 16)
            | (static_cast<std::uint32_t>(bytes[3]) << 24);
    };

    // 0x52D5CA..0x52D5EA：先读目标槽，再按顺序检查目标地址和两个签名字节。
    std::uint8_t* const target = *destination;
    if (target == nullptr) {
        return nullptr;
    }
    if (IsPngGraphicData(source)) {
        const auto index = std::bit_cast<std::int32_t>(*lengthOut);
        const auto& info = gGraphicInfo[index];
        *paletteLengthOut = 0;
        *widthOut = static_cast<std::uint32_t>(info.w);
        *heightOut = static_cast<std::uint32_t>(info.h);
        *lengthOut = info.length;
        std::memcpy(target, source, info.length);
        return target;
    }
    if (source[0] != 0x52 || source[1] != 0x44) {
        return nullptr;
    }

    const std::uint8_t flags = source[2];
    const std::uint32_t width = readUint32Le(source + 0x04);
    const std::uint32_t height = readUint32Le(source + 0x08);

    if ((flags & Compressed) == 0) {
        const std::uint8_t* cursor = source + 0x10;
        std::uint32_t length;
        if ((flags & ExtendedHeader) != 0) {
            const std::uint32_t paletteLength = readUint32Le(cursor);
            cursor += 4;
            *paletteLengthOut = paletteLength;
            length = width * height;
            length += *paletteLengthOut;
        } else {
            length = width * height;
            *paletteLengthOut = 0;
        }

        // 0x52D62F..0x52D63E：不读取 header.length，按低 32 位长度正向复制。
        std::uint32_t copied = 0;
        while (copied < length) {
            const std::uint8_t value = *cursor;
            ++cursor;
            target[copied] = value;
            ++copied;
        }

        *lengthOut = copied;
        *widthOut = width;
        *heightOut = height;
        return target;
    }

    // 0x52D660..0x52D687：先读压缩边界，再处理扩展头并写调色板长度。
    const std::uint8_t* const sourceEnd = source + readUint32Le(source + 0x0C);
    const std::uint8_t* cursor = source + 0x10;
    std::uint8_t* output = target;
    if ((flags & ExtendedHeader) != 0) {
        const std::uint32_t paletteLength = readUint32Le(cursor);
        cursor += 4;
        *paletteLengthOut = paletteLength;
    } else {
        *paletteLengthOut = 0;
    }

    // 只在命令之间比较源边界；不按像素数停止，不检查命令内部的剩余字节。
    while (cursor < sourceEnd) {
        const std::uint8_t command = *cursor;
        ++cursor;
        if ((command & Repeat) != 0) {
            std::uint8_t value = 0;
            if ((command & ZeroFill) == 0) {
                value = *cursor;
                ++cursor;
            }

            // 0x52D6A0..0x52D6E9：先取填充值，0x20 长度优先于 0x10。
            std::uint32_t count = command & RunLengthMask;
            if ((command & LongRunLength) != 0) {
                count = (count << 8) | *cursor;
                ++cursor;
                count = (count << 8) | *cursor;
                ++cursor;
            } else if ((command & ExtendedRunLength) != 0) {
                count = (count << 8) | *cursor;
                ++cursor;
            }

            // 0x52D707/0x52D712 的 stosd/stosb 等价字节写入，不引入未对齐存储。
            for (std::uint32_t written = 0; written < count; ++written) {
                *output = value;
                ++output;
            }
        } else {
            // literal 只看 0x10；0x20、0x40 不改变其长度或操作。
            std::uint32_t count = command & RunLengthMask;
            if ((command & ExtendedRunLength) != 0) {
                count = (count << 8) | *cursor;
                ++cursor;
            }

            // 0x52D73E..0x52D743：保留原检查，虽按上述解码 count 最大仅为 0xFFF。
            if (count >= 0xFFFFFu) {
                return nullptr;
            }
            while (count != 0) {
                const std::uint8_t value = *cursor;
                *output = value;
                ++output;
                ++cursor;
                --count;
            }
        }
    }

    // 0x52D75E..0x52D77B：报告像素数而非实际写入量，不计调色板长度。
    *lengthOut = width * height;
    *widthOut = width;
    *heightOut = height;
    return target;
}

} // namespace cg::resource
