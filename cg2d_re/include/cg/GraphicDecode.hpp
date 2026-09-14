#pragma once

#include <cstdint>

namespace cg::resource {

// 对应原 Graphic::readGraphicData@0x52D5C0；证据见 doc/cg2d_graphic_decode.md。
// 仅暴露已还原的核心函数，不声明原类或目标描述符的完整布局。
//
// destination 指向存活且正确对齐的原生 uint8_t* 槽，只读取槽中的目标地址。
// 目标地址为空时不读取 source；否则依次检查 source[0]、source[1] 是否为 RD。
// 上述失败返回 nullptr，不写目标数据和四个数值输出；成功返回最初的目标地址。
//
// 调用者保证各路径实际访问的源字节、目标字节和四个 uint32_t 输出对象有效；
// 指针运算必须位于对应数组内或恰好越过末尾，数值输出须存活且正确对齐。
// 源、目标、指针槽、四个数值输出两两不重叠，调用期间不被并发修改。
// 必须按实际命令提供足够内存：压缩命令可跨过 header.length，也可写出超过
// width * height 的数据；本函数没有源长度、目标容量检查或失败回滚。
//
// widthOut、heightOut 保留原 32 位位模式；paletteLengthOut 先于解码写入。
// 解码成功后依次写 lengthOut、widthOut、heightOut。压缩 lengthOut 为
// width * height，无压缩为 width * height + *paletteLengthOut，均按 uint32_t 回绕。
// 仅还原满足上述条件且不依赖原 x86 地址回绕的行为；不解决原越界访问或
// 32 位地址回绕，不提供原 x86 描述符/调用 ABI 的二进制兼容。
std::uint8_t* ReadGraphicData(
    std::uint8_t const* source,
    std::uint8_t* const* destination,
    std::uint32_t* widthOut,
    std::uint32_t* heightOut,
    std::uint32_t* lengthOut,
    std::uint32_t* paletteLengthOut) noexcept;

} // namespace cg::resource
