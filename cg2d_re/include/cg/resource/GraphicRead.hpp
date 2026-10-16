#pragma once


#include <cstdio>
#include <cstddef>
#include <cstdint>

namespace cg::render {
struct PaletteEntry;
} // namespace cg::render

namespace cg::resource {

// 原553104/553108：独立队列与DBF分别构造后即时登记真实退出。
// 当前仅模块适配调用；接全应用序时同小步移除适配，不重复构析。
// 原函数：0x00522620。
void InitializeIndependentPaletteQueue_522620();
// 原函数：0x00522690。
void InitializePaletteDbfState_522690();

// 原 0x927EF4 的九个 Graphic 文件槽，统一由资源初始化/关闭链接原FILE核心。
extern std::FILE* gGraphicFiles[9];

// 新端选定读取承载范围为原 0x833CB0..0x927EF0，解压承载范围为 0x73F138..0x833378。
// 两段彼此独立，不证明原数组声明或跨界行为；读盘复用前者，实际解码写入才修改后者。
inline constexpr std::size_t GraphicScratchCapacity = 1'000'000;
extern std::uint8_t gGraphicReadBuffer[GraphicScratchCapacity]; // 原全局：0x00833CB0。
extern std::uint8_t gGraphicDecodeBuffer[GraphicScratchCapacity]; // 原全局：0x0073F138。

// 原 1020EC0/1020ED0/1020ED4/1020ED8/1020EE0：跨资源读取与渲染共享的当前图状态。
// data 为非拥有解码区引用，宽高与调色板长度为完整 W32；图号按 S32 比较。
extern std::uint8_t* gGraphicDataPtr; // 原全局：0x01020EC0。
extern std::uint32_t gCurrentGraphicHeight; // 原全局：0x01020ED0。
extern std::uint32_t gCurrentGraphicWidth; // 原全局：0x01020ED4。
extern std::uint32_t gCurrentGraphicPaletteSize; // 原全局：0x01020ED8。
extern std::int32_t gGraphicDataBuffer; // 原全局：0x01020EE0。

// 原 0x53AF90：DBF 压缩字节的字面量/回溯解码；仅遇结束标记才写实际输出长度。
// 调用者须提供完整有效的压缩流和足以容纳解压结果的目标区。
// 结束标记恰好耗尽输入返回 0；输入有余返回 -8，越过声明末尾返回 -4。
std::int32_t DecompressDbf_53AF90(
    const std::uint8_t* source,
    std::int32_t sourceLength,
    std::uint8_t* destination,
    std::uint32_t* decodedLength) noexcept;

// 原 522B30：重置同一队列，追加静态表和 DBF 索引，再发布独立调色板双池。
// 返回实际计数；不以 DBF 打开、magic 或解压结果新增初始化失败门禁。
std::int32_t InitializeIndependentPalettes_522B30();

// 原 522C50：先释放记录池、再释放索引池，分别清指针；保留队列、计数与 DBF 数据。
void ReleaseIndependentPalettes_522C50();

// 原 523140 的资源侧：非法索引清零返回 2，版本相同返回 0，否则经 522CB0 刷新返回 1。
// index 为绘制记录 +22 的 S16；只借用该值，不复制资源记录或调色板。
std::uint8_t RefreshIndependentPalette32_523140(std::int16_t& index);

// 原 520C81..520CA1：一次借同一记录的原色/色调 W32 表地址；记录空则两者为空。
// 不检查索引范围；调用者取得地址后才读取绘制混合位，选择同一记录的色表。
// 调用者已由 523140 处理绘制索引；池释放后这两个非拥有引用失效。
void GetIndependentPaletteColors32(
    std::int16_t index,
    const std::uint32_t*& originalColors,
    const std::uint32_t*& tonedColors);

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

// 原 451D80：读入同一解码区，对非零索引平均 RGB，再选原中间调色板范围。
std::int32_t GetGraphicAutoMapColor_451D80(
    std::int32_t index, const render::PaletteEntry* palette);

// 原 0x45A6C0：先释放 mapNo 索引，再按槽序关闭文件；不释放 GraphicInfo 主表。
void CloseGraphicFiles();

} // namespace cg::resource
