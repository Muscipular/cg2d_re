#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
// 图鉴记录、同步与存档的共享声明。

namespace cg::world {

// 原80H相册记录；518870生产，列表／详情／423800借用同一对象。
struct AlbumRecord {
    std::uint8_t used; // +00：记录存在标记。
    std::uint8_t mark1; // +01：详情第一图标索引，消费时解释为S8。
    std::uint8_t mark2; // +02：详情第二图标索引，消费时解释为S8。
    std::uint8_t tribe; // +03：种族索引，消费时解释为S8。
    std::int32_t number; // +04：协议编号。
    std::int32_t imageNo; // +08：预览图号。
    std::array<std::int32_t, 5> ratings; // +0C：五组评级。
    std::array<std::int32_t, 4> attributes; // +20：四项属性，战斗匹配只判断非零。
    std::int32_t value30; // +30：详情独立显示值，具体业务名称未证。
    std::array<char, 17> name; // +34：原编码名称及NUL。
    std::array<std::array<char, 29>, 2> description; // +45：两行原编码说明。
    std::uint8_t fresh; // +7F：新记录标记。
};

static_assert(std::is_standard_layout_v<AlbumRecord>);
static_assert(std::is_trivially_copyable_v<AlbumRecord>);
static_assert(sizeof(AlbumRecord) == 0x80);
static_assert(alignof(AlbumRecord) == 4);
static_assert(offsetof(AlbumRecord, used) == 0x0);
static_assert(offsetof(AlbumRecord, mark1) == 0x1);
static_assert(offsetof(AlbumRecord, mark2) == 0x2);
static_assert(offsetof(AlbumRecord, tribe) == 0x3);
static_assert(offsetof(AlbumRecord, number) == 0x4);
static_assert(offsetof(AlbumRecord, imageNo) == 0x8);
static_assert(offsetof(AlbumRecord, ratings) == 0xC);
static_assert(offsetof(AlbumRecord, attributes) == 0x20);
static_assert(offsetof(AlbumRecord, value30) == 0x30);
static_assert(offsetof(AlbumRecord, name) == 0x34);
static_assert(offsetof(AlbumRecord, description) == 0x45);
static_assert(offsetof(AlbumRecord, fresh) == 0x7F);

// 423800借用当前角色960条记录，入口固定一次角色S16。
const AlbumRecord* GetSelectedAlbumRecords();
// 492F70固定角色/记录后清+7F，返回同记录名称；不保存或重新取角色。
const char* ClearAlbumNewFlagAndGetName(std::int16_t selected, std::int32_t index);
// 原函数：0x005186A0。
void Sub_5186A0(); // 原相册保存：codec容量4，短写保留文件句柄。
// 原函数：0x00518BE0。
void Sub_518BE0(); // 原所选角色相册组清理及保存。
// 原函数：0x0050E5B0。
void Sub_50E5B0(
    std::uintptr_t socket, std::int32_t version, const char* payload); // 相册同步回复，socket未用。
// 原函数：0x0050E5D0。
void Sub_50E5D0(
    std::uintptr_t socket, std::int32_t number, std::int32_t imageNo,
    std::int32_t mark1, std::int32_t mark2, std::int32_t tribe,
    std::int32_t value0C, std::int32_t value10, std::int32_t value14,
    std::int32_t value18, std::int32_t value1C, std::int32_t value20,
    std::int32_t value24, std::int32_t value28, std::int32_t value2C,
    std::int32_t value30, char* text, const char* name); // 相册记录回复，socket未用。

namespace detail {
// 原函数：0x00518780。
void Sub_518780(); // 原相册加载：读取4字节，长度不符调用5186A0。
// 原函数：0x00518870。
void Sub_518870(
    std::int32_t number, std::int32_t imageNo,
    std::int32_t mark1, std::int32_t mark2, std::int32_t tribe,
    std::int32_t value0C, std::int32_t value10, std::int32_t value14,
    std::int32_t value18, std::int32_t value1C, std::int32_t value20,
    std::int32_t value24, std::int32_t value28, std::int32_t value2C,
    std::int32_t value30, char* text, const char* name); // 同源相册记录逐字段更新。
// 原函数：0x00518A40。
void Sub_518A40(std::int32_t version, const char* payload); // 位掩码同步，先请求后发布版本。

} // namespace detail

} // namespace cg::world
