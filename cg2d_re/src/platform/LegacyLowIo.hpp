#pragma once

#include "StartupNative.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原8B低层记录；只拓宽HANDLE，W8标志/暂存字节及原未解释2字节保持。
struct LegacyDescriptorRecord final {
    HANDLE handle;
    std::uint8_t flags;
    std::uint8_t pipeByte;
    std::uint8_t remaining06[2];
};
static_assert(offsetof(LegacyDescriptorRecord, flags) == 8);
static_assert(offsetof(LegacyDescriptorRecord, pipeByte) == 9);
static_assert(offsetof(LegacyDescriptorRecord, remaining06) == 10);
static_assert(sizeof(LegacyDescriptorRecord) == 16);

extern LegacyDescriptorRecord* gLegacyDescriptorBlocks10869C0[64];
extern std::uint32_t gLegacyDescriptorCapacity1086AC0;

// 原53CCD0的24H文件状态；全部为定宽数值，+0E两字节从未由本体写入。
struct LegacyFileStatus32 final {
    std::uint32_t deviceBits;          // 原+00；设备/管道为descriptor，磁盘为0。
    std::uint16_t inode;               // 原+04。
    std::uint16_t mode;                // 原+06；类型/权限位型。
    std::uint16_t linkCount;           // 原+08。
    std::uint16_t user;                // 原+0A。
    std::uint16_t group;               // 原+0C。
    std::uint8_t unwritten0E[2];       // 原+0E；不预清或补写。
    std::uint32_t relatedDeviceBits;   // 原+10；与+00同源写入。
    std::uint32_t sizeLowBits;         // 原+14；只消费文件长度低W32。
    std::int32_t accessTime;           // 原+18。
    std::int32_t modifiedTime;         // 原+1C。
    std::int32_t creationTime;         // 原+20；auto.dat只消费本字段。
};
static_assert(sizeof(LegacyFileStatus32) == 0x24);
static_assert(alignof(LegacyFileStatus32) == 4);
static_assert(offsetof(LegacyFileStatus32, deviceBits) == 0x00);
static_assert(offsetof(LegacyFileStatus32, inode) == 0x04);
static_assert(offsetof(LegacyFileStatus32, mode) == 0x06);
static_assert(offsetof(LegacyFileStatus32, linkCount) == 0x08);
static_assert(offsetof(LegacyFileStatus32, user) == 0x0A);
static_assert(offsetof(LegacyFileStatus32, group) == 0x0C);
static_assert(offsetof(LegacyFileStatus32, unwritten0E) == 0x0E);
static_assert(offsetof(LegacyFileStatus32, relatedDeviceBits) == 0x10);
static_assert(offsetof(LegacyFileStatus32, sizeLowBits) == 0x14);
static_assert(offsetof(LegacyFileStatus32, accessTime) == 0x18);
static_assert(offsetof(LegacyFileStatus32, modifiedTime) == 0x1C);
static_assert(offsetof(LegacyFileStatus32, creationTime) == 0x20);

// 原53CCD0；只验证同表容量/FOPEN，保留逐字段发布及查询失败的已写前缀。
std::int32_t ReadLegacyDescriptorStatus32_53CCD0(
    std::int32_t descriptor, LegacyFileStatus32* status);

// 原53F52D；实际分配/继承/标准句柄生产，原生HANDLE继承块，返回SetHandleCount。
std::uint32_t InitializeLegacyDescriptors_53F52D();
// 原53F493；低W32定位，成功清原EOF位，不预清系统LastError。
std::int32_t SeekLegacyDescriptor_53F493(
    std::int32_t descriptor, std::int32_t offset, std::uint32_t origin);

// 原54B658；S64定位拆两半交SetFilePointer，仅低返回全1才查询系统错误。
std::int64_t SeekLegacyDescriptor64_54B658(
    std::int32_t descriptor, std::int64_t offset, std::uint32_t origin);

// 原58FB40；描述符-1的静态回退记录，不作为有效描述符发布。
extern LegacyDescriptorRecord gLegacyInvalidDescriptor58FB40;

// 原541426；原位CRLF压缩、Ctrl-Z和peek字节，返回S32已读数或原错误。
std::int32_t ReadLegacyDescriptor_541426(
    std::int32_t descriptor, void* buffer, std::uint32_t count);
// 原544705返回0/40H，不额外校验FOPEN；原53D3BD保留查询/恢复次序。
std::int32_t IsLegacyDescriptorTty_544705(std::int32_t descriptor);
std::int32_t GetLegacyDescriptorLength_53D3BD(std::int32_t descriptor);

// 原默认模式/权限屏蔽为W32可变槽，初值沿映像虚拟零合同。
extern std::uint32_t gLegacyDefaultOpenMode1086178;
extern std::uint32_t gLegacyPermissionMask1085FE8;

// 原打开链；申请只置handle=-1，set不提前置FOPEN，路径保持ANSI原字节。
std::int32_t AllocateLegacyDescriptor_543711();
std::int32_t SetLegacyDescriptorHandle_5437A6(std::int32_t descriptor, HANDLE handle);
std::int32_t SetLegacyDescriptorMode_5463A3(std::int32_t descriptor, std::uint32_t mode);
std::int32_t ResizeLegacyDescriptor_5438D4(std::int32_t descriptor, std::int32_t size);
std::int32_t OpenSharedLegacyDescriptor_53D44C(const char* filename,
    std::uint32_t flags, std::uint32_t shareFlags, std::uint32_t permissions);
std::int32_t OpenLegacyDescriptor_53D435(
    const char* filename, std::uint32_t flags, std::uint32_t permissions);

// 原句柄访问/释放及关闭/提交，不操作宿主CRT描述符表。
HANDLE GetLegacyDescriptorHandle_543897(std::int32_t descriptor);
std::int32_t ReleaseLegacyDescriptorHandle_54381D(std::int32_t descriptor);
std::int32_t CloseLegacyDescriptor_53D30A(std::int32_t descriptor);
std::int32_t CommitLegacyDescriptor_541222(std::int32_t descriptor);

// 原541279；二进制直写或LF扩CRLF，保留短写、错误槽及已生成CR扣除次序。
std::int32_t WriteLegacyDescriptor_541279(
    std::int32_t descriptor, const void* buffer, std::uint32_t count);

} // namespace cg::platform
