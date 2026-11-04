#pragma once

#include <cstdint>

namespace cg::session { class ClientSession; }
namespace cg::render {

// 原 553018/55301C：固定缓存构造后立即登记退出，返回实际宿主 atexit 的 S32。
// 当前仅本模块启动适配调用；全应用表接入时必须同时移除该适配，禁止双执行。
// 私有树节点由宿主 STL 承接，退出不替代 44D490 的逐面业务释放。
// 原函数：0x0044CC20。
void InitializeSurfaceTransformCache629300_44CC20();
// 原函数：0x0044CD30。
void InitializeSurfaceTransformCache62A630_44CD30();

// 原 Graphic::LoadGraphic@522570：读共享索引图，发布 DX 尺寸并领取节点链。
// 5222D0 按用户裁决仅实现恒空第二参数路径；部分建链也不改写本函数的成功返回。
std::int32_t LoadGraphic_522570(std::int32_t imageNo);

// 原播放/绘制消费者按阶段读取 DX 宽、高的低 S16；不检查图号，不触发加载。
std::int16_t ReadGraphicDxWidth(std::int32_t imageNo) noexcept;
std::int16_t ReadGraphicDxHeight(std::int32_t imageNo) noexcept;

// AF406：按graphic共享的16位动作属性和Coordinate；无图号返回零，不触发纹理上传。
// RMO01：两个Set仅在分页分配失败时返回false，由动画驱动/加载器传递失败。
std::uint16_t ReadGraphicAnimeActionFlags(std::int32_t imageNo) noexcept;
bool SetGraphicAnimeActionFlags(std::uint32_t imageNo, std::uint16_t flags) noexcept;
void ReadGraphicCoordinate(std::int32_t imageNo, std::int16_t& x, std::int16_t& y) noexcept;
bool SetGraphicCoordinate(std::uint32_t imageNo, std::int16_t x, std::int16_t y) noexcept;
// 原43C0B8先借DX表，43C0E6/43C0EA随后从同表取高度/宽度低S16。
// 仅调用期非拥有T*；不检查图号、不加载、不扩宽原W16字段。
class GraphicDxCache;
extern GraphicDxCache* gGraphicDxCache103CFA4; // 原全局：0x0103CFA4；同一缓存指针。
std::int16_t ReadGraphicDxHeightFromEntries(
    const GraphicDxCache* entries, std::int32_t imageNo) noexcept;
std::int16_t ReadGraphicDxWidthFromEntries(
    const GraphicDxCache* entries, std::int32_t imageNo) noexcept;

// 原42A324先取DX表，再读取调用期借用的W32图号字段；不检查图号或触发加载。
std::int16_t ReadGraphicDxHeightFromField(const std::int32_t& imageNo) noexcept;

// 原 44D490：仅清当前场景对应的变换树，按完整地址键次序释放面、清槽并摘项。
// 原两个 caller 均不消费残留 EAX，不向上层暴露树哨兵地址。
void ClearSurfaceTransformCache_44D490(session::ClientSession& session);

} // namespace cg::render
