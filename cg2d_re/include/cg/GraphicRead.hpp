#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace cg::resource {

// 原 0x927EF4 的九个 Graphic 文件句柄，所有权属于资源初始化/关闭链。
extern std::FILE* gGraphicFiles[9];

// 新端选定读取承载范围为原 0x833CB0..0x927EF0，解压承载范围为 0x73F138..0x833378。
// 两段彼此独立，不证明原数组声明或跨界行为；读盘复用前者，实际解码写入才修改后者。
inline constexpr std::size_t GraphicScratchCapacity = 1'000'000;
extern std::uint8_t gGraphicReadBuffer[GraphicScratchCapacity];
extern std::uint8_t gGraphicDecodeBuffer[GraphicScratchCapacity];

// 原 1020EC0/1020ED0/1020ED4/1020ED8/1020EE0：跨资源读取与渲染共享的当前图状态。
// data 为非拥有解码区引用，宽高与调色板长度为完整 W32；图号按 S32 比较。
extern std::uint8_t* gGraphicDataPtr;
extern std::uint32_t gCurrentGraphicHeight;
extern std::uint32_t gCurrentGraphicWidth;
extern std::uint32_t gCurrentGraphicPaletteSize;
extern std::int32_t gGraphicDataBuffer;

// 原 0x53AF90：DBF 压缩字节的字面量/回溯解码；仅遇结束标记才写实际输出长度。
// 调用者须提供完整有效的压缩流和足以容纳解压结果的目标区。
// 结束标记恰好耗尽输入返回 0；输入有余返回 -8，越过声明末尾返回 -4。
std::int32_t DecompressDbf_53AF90(
    const std::uint8_t* source,
    std::int32_t sourceLength,
    std::uint8_t* destination,
    std::uint32_t* decodedLength) noexcept;

// 原 0x45A900：按全局图形号 seek/read，再调用 ReadGraphicData；可省略 paletteLength。
// 调用者须先建立元信息和文件表；实际查文件表时 fileIndex 必须在 0..8。
// 读取成功后 destination 指针槽必须可写，记录长度/解压结果须落在相应存储内。
// 源格式的有效性、可访问范围及输出参数约定同 GraphicDecode.hpp，不补原端没有的短读恢复。
int ReadGraphicDataByIndex(
    std::int32_t index,
    std::uint8_t** destination,
    std::uint32_t* width,
    std::uint32_t* height,
    std::uint32_t* paletteLength);

// 原 0x45A6C0：先释放 mapNo 索引，再按槽序关闭文件；不释放 GraphicInfo 主表。
void CloseGraphicFiles();

} // namespace cg::resource
