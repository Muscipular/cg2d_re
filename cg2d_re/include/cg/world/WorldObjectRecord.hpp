#pragma once

#include "cg/platform/LegacyCryptInt.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace cg::session { class ClientSession; }
namespace cg::yobi { struct YobiAction; }
namespace cg::world::detail {

// 原0x13C记录的增量映射；未消费区不计最终指针/字段映射，见World对象SPEC。
struct WorldObjectRecord {
    // 原0x0043D7C0：成员正序构造及失败前缀清理由C++承接。
    WorldObjectRecord() = default;
    // 原0x0043D810：成员自动逆序析构。
    ~WorldObjectRecord() = default;
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t category; // 原成员偏移：+0x02。
    std::int32_t objectIndex; // 原成员偏移：+0x04。
    std::uint32_t imageNo; // 原成员偏移：+0x08。
    platform::LegacyCryptInt coordinate0C; // 原成员偏移：+0x0C。
    platform::LegacyCryptInt coordinate1C; // 原成员偏移：+0x1C。
    std::int32_t level2C; // 原成员偏移：+0x2C。
    std::int32_t direction; // 原成员偏移：+0x30。
    std::int32_t value34; // 原成员偏移：+0x34。
    std::int32_t value38; // 原+38；value34待转方向对应的目标X。
    std::int32_t value3C; // 原+3C；同一待转方向对应的目标Y。
    std::uint16_t value40; // 原成员偏移：+0x40。
    std::uint16_t flags42; // 原成员偏移：+0x42。
    std::uint8_t marker44; // 原成员偏移：+0x44。 442BD0置1，449CC0读取；与flags42分属不同槽。
    std::array<std::byte, 3> bytes45; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
    std::int32_t value4C; // 原成员偏移：+0x4C。
    std::int32_t value50; // 原成员偏移：+0x50。
    std::wstring name54; // 原成员偏移：+0x54。 446470清空名称；协议入口按原GBK字节长度门槛解码。
    std::wstring title65; // 原成员偏移：+0x65。
    std::wstring text76; // 原成员偏移：+0x76。
    std::wstring text97; // 原成员偏移：+0x97。
    std::wstring textB8; // 原成员偏移：+0xB8。
    std::wstring nameC9; // 原成员偏移：+0xC9。 445D20/445F40保留60个GBK字节限制，内部保存宽文本。
    std::array<std::byte, 6> bytes106; // 原成员偏移：+0x106。
    std::int32_t value10C; // 原成员偏移：+0x10C。 442CA0/442D00/442D60的共同数值槽。
    std::array<std::byte, 4> bytes110; // 原成员偏移：+0x110。
    std::uint32_t flags114; // 原成员偏移：+0x114。
    std::int32_t value118; // 原成员偏移：+0x118。
    cg::yobi::YobiAction* action; // 原+11C，Yobi链拥有，本表只借用。
    std::uint16_t flags120; // 原成员偏移：+0x120。
    std::array<std::byte, 2> bytes122; // 原成员偏移：+0x122。
    std::int32_t value124; // 原+124，442F70状态图号表下标。
    std::wstring text128; // 原+128；445350的第4段称号。
    std::array<std::byte, 3> bytes139; // 原成员偏移：+0x139。
};

inline constexpr std::size_t kWorldObjectCapacity = 1000;

// 首会话沿原中央位置构造，后续会话由宿主调用，不重复创建。
void InitializeWorldObjectStorage(session::ClientSession& session);

} // namespace cg::world::detail
