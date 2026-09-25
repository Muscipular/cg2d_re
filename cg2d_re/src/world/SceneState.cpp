#include "cg/world/SceneState.hpp"

#include "../LoginRequestDependencies.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/GameTime.hpp"
#include "WorldUpdateDependencies.hpp"

#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstring>

namespace cg::world {
namespace {

// 原 0x9A7FA8；由 LegacyMouse.cpp 的 CRT 顺序适配显式构造、析构。
platform::LegacyCryptInt gRippleGate {};

constexpr std::array<std::int32_t, 6> kAllowedMapFloors {
    59000,
    59001,
    59100,
    37623,
    37619,
    37631,
};

// 原 0x5541F4；Sub_4493C0 按原顺序匹配的四项地图楼层表。
constexpr std::array<std::int32_t, 4> kMapFloors5541F4 {
    59000,
    59001,
    37623,
    37619,
};

struct DirectionDelta {
    std::int16_t x;
    std::int16_t y;
};

// 原 .data [0x55D9DC,0x55D9FC)：八组可写的方向 W16 分量。
std::array<DirectionDelta, 8> gDirectionDeltas55D9DC {{
    {0, -1}, {1, -1}, {1, 0}, {1, 1},
    {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
}};

// 原 [0x4B0F10,0x4B0F13)：唯一动作是清 EAX，46AA00 直接调用。
std::int32_t Sub_4B0F10()
{
    return 0;
}

// 原 0x55DE50/0x554920；.cut 文件使用当前目录下的原相对路径。
constexpr char kCutPath55DE50[] = "bin\\PUK2\\cutdat\\%d.cut";
constexpr char kCutReadMode554920[] = "rb";

// 原提示窄字节，不转换编码；两个正文与各自标题按调用顺序配对。
constexpr char kCutMessage55DE3C[] =
    "\x63\x75\x74\xA6\x61\xB9\xCF\xC0\xC9\xB6\x7D\xB1\xD2\xA5\xA2\xB1\xD1\xA1\x43";
constexpr char kCutCaption5536B8[] = "\xBD\x54\xBB\x7B";
constexpr char kCutMessage55DE20[] =
    "\xAD\x6E\xC0\xCB\xB5\xF8\x46\x41\x51\xB6\xDC\xA1\x48";
constexpr char kCutCaption55DE30[] = "\xC0\xC9\xAE\xD7\xBF\xF9\xBB\x7E";

// 原 0x55DE7C/0x55DE68；.aut 相对路径与打开失败的第一正文。
constexpr char kAutPath55DE7C[] = "bin\\PUK2\\cutdat\\%d.aut";
constexpr char kAutMessage55DE68[] =
    "\x61\x75\x74\xA6\x61\xB9\xCF\xC0\xC9\xB6\x7D\xB1\xD2\xA5\xA2\xB1\xD1\xA1\x43";

// 原 468520；与命令行 mapfolder: 配置无关。
constexpr char kMapPathPrefix[] = "map";
constexpr char kMapPathZero[] = "%s\\%d\\%d.dat";
constexpr char kMapPathOther[] = "%s\\%d\\%d\\%d.dat";

// 原 0x55BAE0 的可写八字节区，正文六字节、NUL 后保留零填充。
char gText55BAE0[8] = "\xB5\x6E\xA5\x58\xA4\xA4";

// 原 0x55DF10：保留可写数据身份，不以运行期三角函数重算原表。
std::array<std::uint64_t, 360> gAngleTableBits55DF10 {
#include "SceneAngleTable55DF10.inc"
};

// 原 .data [0x55EA50,0x55EC58)：65 项 F64 位型，按表项原序保存。
std::array<std::uint64_t, 65> gAngleTableBits55EA50 {
    0x0000000000000000ULL, 0x3FECA543F1C75819ULL, 0x3FFCA379B77C02B0ULL, 0x4005785F06F69446ULL,
    0x400C9C55000C953AULL, 0x4011DE5EEDCC20D5ULL, 0x40156C5D63886595ULL, 0x4018F7B84DB9C737ULL,
    0x401C800431BDE82DULL, 0x4020026BD6E8AF81ULL, 0x4021C2E5BC87DB2BULL, 0x4023813DD97F62B7ULL,
    0x40253D436B8F9B13ULL, 0x4026F6C7863BEEC4ULL, 0x4028AD9CCFAEFF5CULL, 0x402A6197C3D68406ULL,
    0x402C128E71476AF9ULL, 0x402DC059641F6449ULL, 0x402F6AD2999567DCULL, 0x403088EB24A6A876ULL,
    0x40315AA161E4F766ULL, 0x40322A7C286F8AD2ULL, 0x4032F86CA89FC6DAULL, 0x4033C46530058149ULL,
    0x40348E58F7121AB5ULL, 0x4035563C64345CFFULL, 0x40361C04C8BC9CD4ULL, 0x4036DFA8826AA8EBULL,
    0x4037A11EEAA6D267ULL, 0x4038606045BAF534ULL, 0x40391D65D3996FA8ULL, 0x4039D829CFDD2285ULL,
    0x403A90A72EAD9275ULL, 0x403B46D9F0A1BE2BULL, 0x403BFABECEDD4845ULL, 0x403CAC535C9E6688ULL,
    0x403D5B95C4220360ULL, 0x403E088509BF9C63ULL, 0x403EB3210BE9424EULL, 0x403F5B69EC2CE465ULL,
    0x404000B063E07A29ULL, 0x404052839042D8C3ULL, 0x4040A32F87AD080BULL, 0x4040F2B599AA6091ULL,
    0x40414117481B21C4ULL, 0x40418E56605EE569ULL, 0x4041DA74D9C6B053ULL, 0x40422574E65BEA0CULL,
    0x40426F58D1526D8BULL, 0x4042B8232096787DULL, 0x4042FFD6733EBBFDULL, 0x40434675A31A4BDCULL,
    0x40438C03925BB7B7ULL, 0x4043D0833C60029FULL, 0x404413F7CED91687ULL, 0x404456646F156191ULL,
    0x404497CC63F14120ULL, 0x4044D83315D701DAULL, 0x4045179BD4067CF2ULL, 0x4045560A2877EE4EULL,
    0x4045938183F91E64ULL, 0x4045D005681ECD4BULL, 0x40460B996FA82E88ULL, 0x404646411C2A0232ULL,
    0x4046800000000000ULL,
};

static_assert(sizeof(kCutPath55DE50) == 23);
static_assert(sizeof(kCutReadMode554920) == 3);
static_assert(sizeof(kCutMessage55DE3C) == 20);
static_assert(sizeof(kCutCaption5536B8) == 5);
static_assert(sizeof(kCutMessage55DE20) == 14);
static_assert(sizeof(kCutCaption55DE30) == 9);
static_assert(sizeof(kAutPath55DE7C) == 23);
static_assert(sizeof(kAutMessage55DE68) == 20);
static_assert(sizeof(kMapPathPrefix) == 4);
static_assert(sizeof(kMapPathZero) == 13);
static_assert(sizeof(kMapPathOther) == 16);
static_assert(sizeof(gText55BAE0) == 8);
static_assert(sizeof(gAngleTableBits55DF10) == 2880);
static_assert(sizeof(gAngleTableBits55EA50) == 520);
static_assert(sizeof(gDirectionDeltas55D9DC) == 32);

} // namespace

std::int32_t& gCurrentSceneType = platform::gValueFF0D40;
std::int32_t gPendingSceneState = 0;
std::int32_t gCurrentSceneState = 0;
std::int32_t gPendingSceneType = 0;

std::uint8_t gValueC82204 = 0;
std::uint32_t gValue5ABFC4 = 0;
std::uint32_t gValue9A6E0C = 0;
std::uint32_t gValueFF0D5C = 0;
std::uint32_t gValueFF0D58 = 0;
std::uint32_t gValueD67E78 = 0;
std::uint32_t gValueFF0D74 = 0;
std::uint8_t gValue9DAD30 = 0;
std::array<std::uint32_t, 7> gValueD67B08 {};
std::uint32_t gValue58B6D0 = 1;
std::uint8_t gValueD9F2CA = 0;

std::uint32_t gWorldProjectionSourceA = 0;
std::uint32_t gWorldProjectionSourceB = 0;
std::uint32_t gWorldProjectionSourceC = 0;
std::uint32_t gWorldProjectionSourceD = 0;
std::uint32_t gWorldProjectionSnapshotA = 0;
std::uint32_t gWorldProjectionSnapshotB = 0;
std::uint32_t gWorldProjectionSnapshotC = 0;
std::uint32_t gWorldProjectionSnapshotD = 0;

volatile std::int32_t gMapFloor = 0; // 原 0x9EE8F0。
volatile std::uint32_t gWorldCurrentXBits = 0; // 原 0x9DAD4C。
volatile std::uint32_t gWorldCurrentYBits = 0; // 原 0x9DAD50。
volatile std::uint32_t gWorldProjectionReferenceXBits = 0; // 原 0xC751A8。
volatile std::uint32_t gWorldProjectionReferenceYBits = 0; // 原 0xC751A4。

static_assert(sizeof(gValueD67B08) == 7 * sizeof(std::uint32_t));

void ConstructPlayerCoordinates()
{
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gPlayerX);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gPlayerY);
}

void DestroyPlayerCoordinates()
{
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gPlayerY);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gPlayerX);
}

void ConstructWorldCoordinateStates()
{
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gPlayerSnapshotX);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gPlayerSnapshotY);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValue9EE8D0);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValue9EE8E0);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValueC27FF8);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValueC27FE8);
}

void DestroyWorldCoordinateStates()
{
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValueC27FE8);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValueC27FF8);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValue9EE8E0);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValue9EE8D0);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gPlayerSnapshotY);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gPlayerSnapshotX);
}

void ConstructWorldMoveStates()
{
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValue9A7F88);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValue9A7F98);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValueC821E8);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValue9E03C0);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValue9FB918);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValueAF0FE8);
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValueC821D8);
}

void DestroyWorldMoveStates()
{
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValueC821D8);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValueAF0FE8);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValue9FB918);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValue9E03C0);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValueC821E8);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValue9A7F98);
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValue9A7F88);
}

void ConstructRippleGate()
{
    platform::legacy_crypt_int::ConstructZero_401C70(&gRippleGate);
}

void DestroyRippleGate()
{
    platform::legacy_crypt_int::Destroy_401CF0(&gRippleGate);
}

std::int32_t ReadRippleGateOnce()
{
    return platform::legacy_crypt_int::Read_401EC0(&gRippleGate);
}

std::int32_t IsMapFloorAllowed_448240() noexcept
{
    const std::int32_t mapFloor = gMapFloor;
    if (mapFloor >= 40000 && mapFloor <= 41999) {
        return 1;
    }
    for (const std::int32_t allowedFloor : kAllowedMapFloors) {
        if (mapFloor == allowedFloor) {
            return 1;
        }
    }
    return 0;
}

// 原 sub_4493C0@[0x4493C0, 0x4493F2)；先判断闭区间，再按原表顺序匹配。
std::int32_t Sub_4493C0() noexcept
{
    const std::int32_t mapFloor = gMapFloor;
    if (mapFloor >= 40000 && mapFloor <= 40999) {
        return 1;
    }
    for (const std::int32_t allowedFloor : kMapFloors5541F4) {
        if (mapFloor == allowedFloor) {
            return 1;
        }
    }
    return 0;
}

void ProjectWorldToScreen_46CE30(
    const float worldX,
    const float worldY,
    float* const outX,
    float* const outY) noexcept
{
    const std::uint32_t referenceXBits = gWorldProjectionReferenceXBits;
    const std::uint32_t referenceYBits = gWorldProjectionReferenceYBits;
    const float referenceX = std::bit_cast<float>(referenceXBits);
    const float referenceY = std::bit_cast<float>(referenceYBits);

    const double u =
        (static_cast<double>(worldX) - static_cast<double>(referenceX))
        * 0.015625;
    const double v =
        (static_cast<double>(worldY) - static_cast<double>(referenceY))
        * 0.015625;
    *outX = static_cast<float>((v + u) * 32.0 + 320.0);
    *outY = static_cast<float>((v - u) * 24.0 + 240.0);
}

std::int32_t SetCurrentSceneType(std::int32_t sceneType)
{
    gCurrentSceneState = 0;
    gCurrentSceneType = sceneType;
    return sceneType;
}

std::int32_t SetCurrentSceneTypeAndState(std::int32_t sceneType, std::int32_t sceneState)
{
    gCurrentSceneType = sceneType;
    gCurrentSceneState = sceneState;
    return sceneType;
}

std::int32_t SetPendingSceneType(std::int32_t sceneType)
{
    gPendingSceneState = 0;
    gPendingSceneType = sceneType;
    return sceneType;
}

std::uint32_t CopyWorldProjectionState_468FF0()
{
    const std::uint32_t a = gWorldProjectionSourceA;
    const std::uint32_t b = gWorldProjectionSourceB;
    const std::uint32_t c = gWorldProjectionSourceC;
    gWorldProjectionSnapshotA = a;
    const std::uint32_t d = gWorldProjectionSourceD;
    gWorldProjectionSnapshotB = b;
    gWorldProjectionSnapshotC = c;
    gWorldProjectionSnapshotD = d;
    return d;
}

namespace detail {

// 原 [0x5198A0,0x5198CB)：逐项前向复制 0x1C 字节，保留每个 W32 的读写顺序。
void Sub_5198A0(
    std::uint8_t* destination,
    const std::uint8_t* source,
    const std::int32_t count)
{
    for (std::int32_t record = 0; record < count; ++record) {
        for (std::int32_t word = 0; word < 7; ++word) {
            std::uint32_t value;
            std::memcpy(&value, source, sizeof(value));
            std::memcpy(destination, &value, sizeof(value));
            source += sizeof(value);
            destination += sizeof(value);
        }
    }
}

// 原 [0x5198F0,0x51991D)：右值为负优先，非负时左负数次之，相等仍返回 -1。
std::int32_t Sub_5198F0(const void* const left, const void* const right)
{
    std::int32_t rightValue;
    std::memcpy(&rightValue, right, sizeof(rightValue));
    if (rightValue < 0) {
        return -1;
    }

    std::int32_t leftValue;
    std::memcpy(&leftValue, left, sizeof(leftValue));
    if (leftValue < 0) {
        return 1;
    }
    return leftValue <= rightValue ? -1 : 1;
}

// 原 0x581800：World::OpenDialog 共用的独立 W32 槽，原始初值 -1。
std::int32_t gValue581800 = -1;

// 原 455BD0 的 23 个独立 W16/W32 共享槽；仅两个 556C 槽的映像初值为 1。
std::uint16_t gValue556C14 = 1;
std::uint16_t gValue62B34E = 0;
std::uint16_t gValue62B34C = 0;
std::uint16_t gValue62B318 = 0;
std::uint32_t gValue62B328 = 0;
std::uint16_t gValue62B3B0 = 0;
std::uint16_t gValue556C34 = 1;
std::uint16_t gValue62B5AA = 0;
std::uint16_t gValue62B5A8 = 0;
std::uint32_t gValue62B5B0 = 0;
std::uint32_t gValue62B308 = 0;
std::uint32_t gValue62B358 = 0;
std::uint32_t gValue62B384 = 0;
std::uint32_t gValue62B310 = 0;
std::uint32_t gValue62B364 = 0;
std::uint32_t gValue62B338 = 0;
std::uint32_t gValue62B36C = 0;
std::uint32_t gValue62B2F8 = 0;
std::uint32_t gValue62B324 = 0;
std::uint32_t gValue62B30C = 0;
std::uint32_t gValue62B388 = 0;
std::uint32_t gValue62B3A0 = 0;
std::uint32_t gValue62B38C = 0;

// 原 46BD40 的四个独立 W32 计数槽；仅承载原位模式。
std::uint32_t gValue9FB934 = 0;
std::uint32_t gValue9FB928 = 0;
std::uint32_t gValue9A86C0 = 0;
std::uint32_t gValue9A7F78 = 0;

// 原 0xD2F788/0xFF0C10：504A30 清零的独立 S32/W16 状态槽。
std::int32_t gValueD2F788 = 0;
std::uint16_t gValueFF0C10 = 0;

// 原 0xFF0D04/0xFF0D08：50D700 写入、展示侧按 S32 读取的独立槽。
std::int32_t gValueFF0D04 = 0;
std::int32_t gValueFF0D08 = 0;

// 原 0xFF0D34：50D3B0 写入、5199A0 清零的独立 W16 槽。
std::uint16_t gValueFF0D34 = 0;

// 原 0xCAFEA0：504100 清零的独立 W32 状态槽。
std::uint32_t gValueCAFEA0 = 0;

// 原 0x9A6204：45ED80 重置、45ED90 读取并推进的场景 7 W32 状态。
std::uint32_t gValue9A6204 = 0;
// 原 45ED90 的四个布局 W32 与非拥有 action 槽，均只由本体直接读写。
std::int32_t gValue9A5674 = 0;
std::int32_t gValue9A594C = 0;
std::int32_t gValue9A61F8 = 0;
std::int32_t gValue9A5AC0 = 0;
login::YobiAction* gAction9A6E98 = nullptr;

// 原 0x5ABB70/0x5ABB6C/0x553F90/0x553F84；43C8F0 顺序重置的独立 S32 槽。
std::int32_t gValue5ABB70 = 0;
std::int32_t gValue5ABB6C = 0;
std::int32_t gValue553F90 = -1;
std::int32_t gValue553F84 = -1;

// 原 0xC8222E；pending 消费与世界循环尾部共用的 W16 门禁。
std::uint16_t gValueC8222E = 0;

// 原 0xC82230/0xC82234/0xC82236；供 46A9E0 初始化和后续消费者共享。
std::uint32_t gValueC82230 = 0;
std::int16_t gValueC82234 = 0;
std::int16_t gValueC82236 = 0;

// 原 0xC3500C/0xC75010/0x9EE8A0/0x9ED438；468F30 的独立 W32 槽。
std::uint32_t gValueC3500C = 0;
std::uint32_t gValueC75010 = 0;
std::uint32_t gValue9EE8A0 = 0;
std::uint32_t gValue9ED438 = 0;
std::uint32_t gValue9ED440 = 0;

// 原 0xC35008/0x9DAD2C/0x9FB930/0xC821D4/0x9A86DC；4682E0 的 W32 目标。
std::uint32_t gValueC35008 = 0;
std::uint32_t gValue9DAD2C = 0;
std::uint32_t gValue9FB930 = 0;
std::uint32_t gValueC821D4 = 0;
std::uint32_t gValue9A86DC = 0;

std::int16_t gMapXStart = 0;
std::int16_t gMapYStart = 0;
std::int16_t gMapXEnd = 0;
std::int16_t gMapYEnd = 0;
std::int16_t gMapSpanX = 0;
std::int16_t gMapSpanY = 0;
platform::LegacyCryptInt gPlayerX {};
platform::LegacyCryptInt gPlayerY {};
platform::LegacyCryptInt gPlayerSnapshotX {};
platform::LegacyCryptInt gPlayerSnapshotY {};
platform::LegacyCryptInt gValue9EE8D0 {};
platform::LegacyCryptInt gValue9EE8E0 {};
platform::LegacyCryptInt gValueC27FF8 {};
platform::LegacyCryptInt gValueC27FE8 {};
platform::LegacyCryptInt gValue9A7F88 {};
platform::LegacyCryptInt gValue9A7F98 {};
platform::LegacyCryptInt gValueC821E8 {};
platform::LegacyCryptInt gValue9E03C0 {};
platform::LegacyCryptInt gValue9FB918 {};
platform::LegacyCryptInt gValueAF0FE8 {};
platform::LegacyCryptInt gValueC821D8 {};
std::uint32_t gValueAF10C0 = 0;
std::uint32_t gValue9EE8FC = 0;
std::uint32_t gValue9EE900 = 0;
std::uint32_t gValue9DAD78 = 0;
std::uint8_t gValue9A4B94 = 0;
std::uint16_t gValueC82200 = 0;
std::uint16_t gValueC82202 = 0;
std::array<std::uint16_t, 2> gPendingDirections9A86D4 {};
std::array<std::uint16_t, kMovePathCapacity> gMovePathDirections {};
std::array<std::uint16_t, kMovePathCapacity> gMovePathX {};
std::array<std::uint16_t, kMovePathCapacity> gMovePathY {};
static_assert(sizeof(gMovePathDirections) == 0xC8);
static_assert(sizeof(gMovePathX) == 0xC8);
static_assert(sizeof(gMovePathY) == 0xC8);
static_assert(sizeof(gPendingDirections9A86D4) == 4);
std::uint32_t gValueC821FC = 0;
std::uint32_t gValue9EE8B8 = 0;
std::uint16_t gValueC8A79C = 0;
std::uint16_t gValue9A86D8 = 0;
std::uint16_t gValue9FB92E = 0;
std::uint16_t gValueB65A7C = 0;
std::uint16_t gValueC82220 = 0;
std::uint32_t gValueC8A778 = 0;
std::uint32_t gValueC8A794 = 0;
std::uint32_t gValueC8220C = 0;
std::uint32_t gValueC82210 = 0;
std::uint32_t gValueC82214 = 0;
std::uint32_t gValueC82218 = 0;
std::uint32_t gValueC82224 = 0;
std::uint32_t gValueC8221C = 0;
std::uint16_t gValue9DAD54 = 0;
std::uint16_t gValue9FB92C = 0;
std::uint16_t gValue55D9FC = 0xFFFFu;
std::uint16_t gValueC821D0 = 0;
std::uint16_t gValue9EE910 = 0;
std::array<std::uint16_t, kMapBufferCells> gMapTileArray {};
std::array<std::uint16_t, kMapBufferCells> gMapObjArray {};
std::array<std::uint16_t, kMapBufferCells> gMapHitArray {};
std::uint16_t gLastDetectedCode = 0;
std::int32_t gValue9E0434 = 0;

static_assert(sizeof(gMapTileArray) == 5202);
static_assert(sizeof(gMapObjArray) == 5202);
static_assert(sizeof(gMapHitArray) == 5202);

// 原 [0x468520,0x46856A)；零 mapId 忽略第三参，两路均透传 sprintf 返回。
std::int32_t __cdecl Sub_468520(
    char* const buffer,
    const std::int32_t mapId,
    const std::int32_t value9E0434,
    const std::int32_t floor)
{
    if (mapId == 0) {
        return std::sprintf(buffer, kMapPathZero, kMapPathPrefix, 0, floor);
    }
    return std::sprintf(
        buffer, kMapPathOther, kMapPathPrefix, mapId, value9E0434, floor);
}

// 原 0x9DAD5C/0x9DAD68/0x9DAD58/0x9DAD60/0x9DAD64；46DA80 的 W32 目标。
std::uint32_t gValue9DAD5C = 0;
std::uint32_t gValue9DAD68 = 0;
std::int32_t gValue9DAD58 = 0;
std::int32_t gValue9DAD60 = 0;
std::int32_t gValue9DAD64 = 0;

// 原 [0x9FB948,0xAEFB94)；供 .cut 重置、加载及后续 46CF60 共享。
CutData9FB948 gCutData9FB948 {};

// 原 [0xAF10D8,0xB65A7B)；.aut 单次读取的唯一共享表。
AutRecordAF10D8 gAutDataAF10D8[159201] {};
static_assert(sizeof(gAutDataAF10D8) == 477603);

// 原 [0xC35010, 0xC75010)；尾地址的 gValueC75010 是独立标量。
std::int32_t gValuesC35010[65536] {};
static_assert(sizeof(gValuesC35010) == 0x40000);

// 原 [0x46DC60,0x46DC85)；整块清零完成后才逐项写 +4 哨兵。
void __cdecl Sub_46DC60()
{
    std::memset(&gCutData9FB948, 0, sizeof(gCutData9FB948));
    for (CutRecord9FB954& record : gCutData9FB948.records) {
        record.value04 = -1;
    }
}

// 原 [0x46DB00,0x46DC55)；保留短读后继续、失败消息及完整返回值。
std::int32_t __cdecl Sub_46DB00(std::int32_t selected)
{
    Sub_46DC60();
    char path[256];
    std::sprintf(path, kCutPath55DE50, selected);
    std::FILE* const file = std::fopen(path, kCutReadMode554920);
    if (file == nullptr) {
        platform::ShowLegacyMessage(kCutMessage55DE3C, kCutCaption5536B8, 0x10);
        const std::int32_t response =
            platform::ShowLegacyMessage(kCutMessage55DE20, kCutCaption55DE30, 0x11);
        if (response != 1) {
            return response;
        }
        return platform::PostLegacyWindowMessage(platform::gLegacyWindow, 0x10, 0, 0);
    }

    std::fread(&gCutData9FB948, 12, 1, file);
    const std::int32_t count0A = static_cast<std::int32_t>(gCutData9FB948.count0A);
    const std::int32_t count08 = static_cast<std::int32_t>(gCutData9FB948.count08);
    const std::int32_t count06 = static_cast<std::int32_t>(gCutData9FB948.count06);
    const std::int32_t count = count0A + count08 + count06;
    for (std::int32_t index = 0; index < count; ++index) {
        CutRecord9FB954& record = gCutData9FB948.records[index];
        std::fread(&record.value00, 4, 1, file);
        std::fread(&record.value04, 2, 1, file);
        std::fread(&record.value06, 2, 1, file);
        std::fread(&record.value08, 2, 1, file);
        std::fread(&record.value0A, 2, 1, file);
        std::fread(&record.value0C, 2, 1, file);
        std::fread(&record.value0E, 2, 1, file);
        std::fread(&record.value10, 2, 1, file);
    }
    return std::fclose(file);
}

// 原 [0x46DC90,0x46DD35)；失败保留表，成功单次读取并返回完整关闭结果。
std::int32_t __cdecl Sub_46DC90(std::int32_t selected)
{
    char path[256];
    std::sprintf(path, kAutPath55DE7C, selected);
    std::FILE* const file = std::fopen(path, kCutReadMode554920);
    if (file == nullptr) {
        platform::ShowLegacyMessage(kAutMessage55DE68, kCutCaption5536B8, 0x10);
        const std::int32_t response =
            platform::ShowLegacyMessage(kCutMessage55DE20, kCutCaption55DE30, 0x11);
        if (response != 1) {
            return response;
        }
        return platform::PostLegacyWindowMessage(platform::gLegacyWindow, 0x10, 0, 0);
    }

    std::fread(gAutDataAF10D8, 1, 477603, file);
    return std::fclose(file);
}

// 原 [0x468200,0x46828D)：四次独立读取玩家坐标，裁剪后发布两个跨度。
std::uint16_t __cdecl Sub_468200()
{
    gMapXStart = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Add_401E10(&gPlayerX, -30)));
    gMapYStart = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Add_401E10(&gPlayerY, -20)));
    gMapXEnd = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Add_401E10(&gPlayerX, 21)));
    gMapYEnd = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Add_401E10(&gPlayerY, 31)));

    Sub_468290(&gMapXStart, &gMapYStart, &gMapXEnd, &gMapYEnd);
    const std::uint16_t spanX =
        static_cast<std::uint16_t>(gMapXEnd)
        - static_cast<std::uint16_t>(gMapXStart);
    const std::uint16_t spanY =
        static_cast<std::uint16_t>(gMapYEnd)
        - static_cast<std::uint16_t>(gMapYStart);
    gMapSpanX = std::bit_cast<std::int16_t>(spanX);
    gMapSpanY = std::bit_cast<std::int16_t>(spanY);
    return spanX;
}

// 原 [0x46CEC0,0x46CED6)：Y 的读取先于 X 的带参读取。
std::int32_t __cdecl Sub_46CEC0()
{
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    return platform::legacy_crypt_int::Add_401E10(&gPlayerX, y);
}

// 原 [0x46CEE0,0x46CEFB)：同一读序，结果写入独立的 AF10C0。
std::int32_t __cdecl Sub_46CEE0()
{
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t result =
        platform::legacy_crypt_int::Add_401E10(&gPlayerX, y);
    gValueAF10C0 = std::bit_cast<std::uint32_t>(result);
    return result;
}

// 原 [0x468330,0x468511)：保留六次临时构造/析构及快照与 F32 写入交错。
std::uint32_t __cdecl Sub_468330(const std::int32_t x, const std::int32_t y)
{
    static_cast<void>(Sub_46CEC0());

    platform::LegacyCryptInt temporary;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    gPlayerX.encoded = temporary.encoded;
    gPlayerX.mask = temporary.mask;
    gPlayerX.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    gPlayerY.readCount = temporary.readCount;
    gPlayerY.encoded = temporary.encoded;
    gPlayerY.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    static_cast<void>(Sub_46CEE0());
    const double currentX =
        static_cast<double>(platform::legacy_crypt_int::Read_401EC0(&gPlayerX));
    gWorldCurrentXBits = std::bit_cast<std::uint32_t>(
        static_cast<float>(currentX * 64.0));
    const double currentY =
        static_cast<double>(platform::legacy_crypt_int::Read_401EC0(&gPlayerY));
    const double scaledY = currentY * 64.0;

    gPlayerSnapshotX.readCount = gPlayerX.readCount;
    gPlayerSnapshotX.encoded = gPlayerX.encoded;
    gPlayerSnapshotX.mask = gPlayerX.mask;
    gPlayerSnapshotY.readCount = gPlayerY.readCount;
    gWorldCurrentYBits = std::bit_cast<std::uint32_t>(
        static_cast<float>(scaledY));
    gPlayerSnapshotY.encoded = gPlayerY.encoded;
    gPlayerSnapshotY.mask = gPlayerY.mask;

    gValue9EE8FC = 0;
    gValue9EE900 = 0;
    gValue9DAD78 = 0x3F800000u;

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, -1);
    gValue9EE8D0.readCount = temporary.readCount;
    gValue9EE8D0.encoded = temporary.encoded;
    gValue9EE8D0.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, -1);
    gValue9EE8E0.readCount = temporary.readCount;
    gValue9EE8E0.encoded = temporary.encoded;
    gValue9EE8E0.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, -1);
    gValueC27FF8.encoded = temporary.encoded;
    gValueC27FF8.mask = temporary.mask;
    gValueC27FF8.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, -1);
    gValueC27FE8.readCount = temporary.readCount;
    gValueC27FE8.encoded = temporary.encoded;
    gValueC27FE8.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    const std::uint32_t xBits = gWorldCurrentXBits;
    const std::uint32_t finalYBits = gWorldCurrentYBits;
    gWorldProjectionReferenceXBits = xBits;
    gWorldProjectionReferenceYBits = finalYBits;
    return finalYBits;
}

// 原 [0x467F30,0x4681AB)：八次独立临时构造，末尾透传表的一过尾指针。
std::int32_t* Sub_467F30()
{
    Sub_4682E0(0, 0, 0, 0, 0, 0, 0, 0);
    static_cast<void>(Sub_468330(0, 0));

    platform::LegacyCryptInt first;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&first, 0);
    gValue9EE8D0.mask = first.mask;
    gValue9EE8D0.encoded = first.encoded;
    gValue9EE8D0.readCount = first.readCount;

    platform::LegacyCryptInt second;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&second, 0);
    gValue9EE8E0.mask = second.mask;
    gValue9EE8E0.encoded = second.encoded;
    gValue9EE8E0.readCount = second.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&second);
    platform::legacy_crypt_int::Destroy_401CF0(&first);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&second, 0);
    gValueC27FF8.mask = second.mask;
    gValueC27FF8.encoded = second.encoded;
    gValueC27FF8.readCount = second.readCount;

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&first, 0);
    gValueC27FE8.mask = first.mask;
    gValueC27FE8.encoded = first.encoded;
    gValueC27FE8.readCount = first.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&first);
    platform::legacy_crypt_int::Destroy_401CF0(&second);

    gValue9A4B94 = 3;
    static_cast<void>(Sub_468200());
    gValueC82200 = 0;
    gValueC82202 = 0;
    gValueC821FC = 0;
    gValue9EE8B8 = 0;
    gValueC8A79C = 0;

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&first, 0);
    gValueC821E8.encoded = first.encoded;
    gValueC821E8.mask = first.mask;
    gValueC821E8.readCount = first.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&first);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&first, 0);
    gValue9E03C0.mask = first.mask;
    gValue9E03C0.encoded = first.encoded;
    gValue9E03C0.readCount = first.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&first);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&first, 0);
    gValueC821D8.mask = first.mask;
    gValueC821D8.encoded = first.encoded;
    gValueC821D8.readCount = first.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&first);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&first, 0);
    gRippleGate.mask = first.mask;
    gRippleGate.encoded = first.encoded;
    gRippleGate.readCount = first.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&first);

    gValue9A86D8 = 0;
    gValue9FB92E = 0;
    gValueB65A7C = 0;
    gValueC82220 = 0;
    gValueC8A778 = 0;
    gValueC8A794 = 0;
    gValueC8220C = 0;
    gValueC82210 = 0;
    gValueC82214 = 0;
    gValueC82218 = 0;
    gValueC82224 = 0;
    gValue9DAD54 = 0xFFFFu;
    gValue9FB92C = 0xFFFFu;
    gValue55D9FC = 0xFFFFu;

    static_cast<void>(Sub_468F30());
    gValueC821D0 = 0;
    gValue9EE910 = 0;
    return Sub_46CF20();
}

// 原 [0x4681B0,0x4681F6)：先读 Y/X 快照并恢复窗口，随后顺序清六槽。
std::int32_t __cdecl Sub_4681B0()
{
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotX);
    static_cast<void>(Sub_468330(x, y));
    static_cast<void>(Sub_468200());
    gValueC82200 = 0;
    gValueC82202 = 0;
    gValueC821FC = 0;
    gValueC8A778 = 0;
    gValueC8A794 = 0;
    gValueC82224 = 0;
    return 0;
}

// 原 [0x468290,0x4682D7)：四个 S16 原地裁剪，末参数指针透传。
std::int16_t* __cdecl Sub_468290(
    std::int16_t* xStart,
    std::int16_t* yStart,
    std::int16_t* xEnd,
    std::int16_t* yEnd)
{
    if (*xStart < 0) {
        *xStart = 0;
    }
    if (*yStart < 0) {
        *yStart = 0;
    }
    const std::uint32_t xBound = gValue9FB930;
    if (*xEnd > std::bit_cast<std::int32_t>(xBound)) {
        *xEnd = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(xBound));
    }
    const std::uint32_t yBound = gValueC821D4;
    if (*yEnd > std::bit_cast<std::int32_t>(yBound)) {
        *yEnd = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(yBound));
    }
    return yEnd;
}

// 原 [0x468C00,0x468C32)：独立重读窗口状态，写完整 WORD 并返回左边界。
std::int32_t __cdecl Sub_468C00(
    const std::int32_t x, const std::int32_t y, const std::uint16_t value)
{
    const std::int32_t top = gMapYStart;
    const std::int32_t span = gMapSpanX;
    const std::int32_t left = gMapXStart;
    const std::int32_t index = x - left + span * (y - top);
    gMapHitArray[static_cast<std::size_t>(index)] = value;
    return left;
}

// 原 [0x46AE60,0x46AE94)：仅读 hit WORD 的低 U8 比较。
std::int32_t __cdecl Sub_46AE60(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t top = gMapYStart;
    const std::int32_t span = gMapSpanX;
    const std::int32_t left = gMapXStart;
    const std::int32_t index = x - left + span * (y - top);
    return static_cast<std::uint8_t>(gMapHitArray[static_cast<std::size_t>(index)]) == 2;
}

// 原 [0x46AD00,0x46AD97)：分类前无条件写独立的低 U8 探测值。
std::int32_t __cdecl Sub_46AD00(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t localX = x - gMapXStart;
    const std::int32_t localY = y - gMapYStart;
    const std::int32_t category = GetCate_458EC0(&gCgTime);
    const std::int32_t index = localX + gMapSpanX * localY;
    const std::uint8_t value =
        static_cast<std::uint8_t>(gMapHitArray[static_cast<std::size_t>(index)]);
    const bool isThree = value == 3;
    gLastDetectedCode = value;
    if (isThree || value == 9 || value == 11 || value == 13 || value == 10) {
        return 1;
    }
    if (value == 6) {
        return category == 3;
    }
    if (value == 7) {
        return category == 0 || category == 1;
    }
    return value == 8 && category == 2;
}

// 原 [0x4682E0, 0x468330)；六次写入后传递四个原参数。
void __cdecl Sub_4682E0(
    std::uint32_t a1,
    std::int32_t a2,
    std::uint32_t a3,
    std::uint32_t a4,
    std::uint32_t a5,
    std::uint32_t a6,
    std::uint32_t a7,
    std::int32_t a8)
{
    gValueC35008 = a5;
    gValue9DAD2C = a6;
    gValue9FB930 = a3;
    gValueC821D4 = a4;
    gValue9A86DC = a1;
    gMapFloor = a2;
    Sub_46DA80(a1, a2, a8, a7);
}

// 原 [0x46DA80, 0x46DAF5)；模式 1 的加载器必须保留调用次序。
void __cdecl Sub_46DA80(
    std::uint32_t a1,
    std::int32_t a2,
    std::int32_t a3,
    std::uint32_t a4)
{
    audio::Sub_52CD40();
    gValue9DAD5C = a1;
    gValue9DAD68 = a4;
    gValue9DAD58 = a2;
    if (a4 != 1) {
        return;
    }

    const std::int32_t selected = a3 == -1 ? a2 : a3;
    gValue9DAD60 = selected;
    gValue9DAD64 = selected;
    Sub_46DB00(selected);
    Sub_46DC90(selected);
    login::detail::Sub_46DD40();
}

// 原 [0x46A9E0, 0x46A9F4)；依次清三槽并返回完整 32 位零。
std::int32_t __cdecl Sub_46A9E0()
{
    gValueC82230 = 0;
    gValueC82234 = 0;
    gValueC82236 = 0;
    return 0;
}

// 原 [0x46AA00,0x46AB56)：按路径方向检测并写 pending，保留早退 Y 返回。
std::int32_t __cdecl Sub_46AA00()
{
    std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    if (std::bit_cast<std::int16_t>(gValueC82202) <= 0) {
        return y;
    }

    std::int32_t step = 0;
    std::int32_t result = 0;
    std::int32_t previousX = x;
    std::int32_t previousY = y;
    for (;;) {
        const std::int16_t direction =
            std::bit_cast<std::int16_t>(
                gPendingDirections9A86D4[static_cast<std::size_t>(step)]);
        const DirectionDelta delta =
            gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)];
        x += delta.x;
        y += delta.y;
        const float differenceY = static_cast<float>(y - previousY);
        const float differenceX = static_cast<float>(x - previousX);
        const std::int32_t heading = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(
                Sub_46CDF0(differenceX, differenceY)));

        if (Sub_46AE60(x, y) != 0) {
            result = 1;
            gValueC82234 = static_cast<std::int16_t>(step);
            gValueC82230 = 1;
            gValueC82236 = 3;
            gValue9ED440 = std::bit_cast<std::uint32_t>(heading);
            if (heading < 0) {
                gValue9ED440 = std::bit_cast<std::uint32_t>(heading) + 8U;
            }
            --step;
            break;
        }
        if (Sub_46AD00(x, y) != 0) {
            result = 1;
            gValueC82230 = 1;
            gValueC82234 = static_cast<std::int16_t>(step + 1);
            gValueC82236 = 1;
            break;
        }
        if (Sub_4B0F10() != 0) {
            result = 1;
            gValueC82230 = 1;
            gValueC82234 = static_cast<std::int16_t>(step + 1);
            gValueC82236 = 2;
            break;
        }
        ++step;
        previousX = x;
        previousY = y;
        if (step >= std::bit_cast<std::int16_t>(gValueC82202)) {
            break;
        }
    }

    if (step < 2 && result != 0) {
        ++step;
        gValueC82200 = 0;
        gValueC82202 = static_cast<std::uint16_t>(step);
    }
    return result;
}

// 原 [0x46BC90,0x46BCE4)：先减路径数量，剩余项按方向、X、Y 原序左移。
void __cdecl Sub_46BC90()
{
    if (std::bit_cast<std::int16_t>(gValueC82200) <= 0) {
        return;
    }
    --gValueC82200;
    const std::int32_t remaining =
        std::bit_cast<std::int16_t>(gValueC82200);
    for (std::int32_t index = 0; index < remaining; ++index) {
        const std::size_t position = static_cast<std::size_t>(index);
        gMovePathDirections[position] = gMovePathDirections[position + 1];
        gMovePathX[position] = gMovePathX[position + 1];
        gMovePathY[position] = gMovePathY[position + 1];
    }
}

// 原 [0x46ABD0,0x46AC0A)：逐项搬路径方向，再消耗三平面首项。
void __cdecl Sub_46ABD0()
{
    for (std::int32_t iteration = 0; iteration < 2; ++iteration) {
        if (std::bit_cast<std::int16_t>(gValueC82200) <= 0) {
            break;
        }
        const std::int16_t count = std::bit_cast<std::int16_t>(gValueC82202);
        const std::uint16_t direction = gMovePathDirections[0];
        const std::uint16_t nextCount =
            static_cast<std::uint16_t>(gValueC82202 + 1U);
        gPendingDirections9A86D4[static_cast<std::size_t>(count)] = direction;
        gValueC82202 = nextCount;
        Sub_46BC90();
    }
}

// 原 [0x46BCF0,0x46BD1D)：正数量减一后将剩余方向原地左移。
void __cdecl Sub_46BCF0()
{
    if (std::bit_cast<std::int16_t>(gValueC82202) <= 0) {
        return;
    }
    --gValueC82202;
    const std::int32_t remaining =
        std::bit_cast<std::int16_t>(gValueC82202);
    for (std::int32_t index = 0; index < remaining; ++index) {
        const std::size_t position = static_cast<std::size_t>(index);
        gPendingDirections9A86D4[position] =
            gPendingDirections9A86D4[position + 1];
    }
}

// 原 [0x46AB60,0x46ABCD)：先分发，再只清 active/countdown。
std::int32_t __cdecl Sub_46AB60()
{
    if (gValueC82230 == 0) {
        return 0;
    }
    if (gValueC8222E != 0) {
        if (gValueC82234 > 0) {
            --gValueC82234;
            if (gValueC82234 > 0) {
                return 0;
            }
        }
    } else if (gValueC82234 > 0) {
        return 0;
    }

    switch (gValueC82236) {
    case 1:
        Sub_46ADA0();
        break;
    case 2:
        Sub_46AC90();
        break;
    case 3:
        Sub_46AEA0();
        break;
    default:
        break;
    }
    gValueC82230 = 0;
    gValueC82234 = 0;
    return 1;
}

// 原 [0x46AC90,0x46AD00)：临时 CryptInt 赋值后发送玩家坐标。
std::int32_t __cdecl Sub_46AC90()
{
    static_cast<void>(Sub_4681B0());

    platform::LegacyCryptInt temporary {};
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, 1);
    gValueC821D8.mask = temporary.mask;
    gValueC821D8.encoded = temporary.encoded;
    gValueC821D8.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    gValueC82220 = 1;
    gValueB65A7C = 0;
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    return network::Sub_510810(platform::gConnectionSocket, x, y);
}

// 原 [0x46AEA0,0x46AF24)：恢复地图后发送五整数请求，重读序号再自增。
std::int32_t __cdecl Sub_46AEA0()
{
    if (gValueCAFEA0 == 0) {
        gValueCAFEA0 = static_cast<std::uint32_t>(audio::Sub_504410(1));
    }
    static_cast<void>(Sub_4681B0());

    const std::uint32_t mapValue = gValue9ED440;
    const std::uint16_t sequenceLow = static_cast<std::uint16_t>(gValueC8221C);
    gValue9FB92E = 1;
    gValueC82220 = 1;
    gValue9FB92C = sequenceLow;

    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    const std::uint32_t sequence = gValueC8221C;
    static_cast<void>(network::Sub_510630(
        platform::gConnectionSocket,
        2,
        std::bit_cast<std::int32_t>(sequence),
        x,
        y,
        std::bit_cast<std::int32_t>(mapValue)));

    const std::uint32_t nextSequence = gValueC8221C + 1U;
    gValueB65A7C = 1;
    gValueC8221C = nextSequence;
    return std::bit_cast<std::int32_t>(nextSequence);
}

// 原 [0x468F30, 0x468F6F)；保持十二次 W32 写入的指令顺序。
std::int32_t __cdecl Sub_468F30()
{
    gValueC3500C = 0;
    gValueC75010 = 0;
    gWorldProjectionSourceA = 0;
    gWorldProjectionSourceB = 0;
    gWorldProjectionSnapshotA = 0;
    gWorldProjectionSnapshotB = 0;
    gValue9EE8A0 = 0;
    gValue9ED438 = 0;
    gWorldProjectionSourceC = 0;
    gWorldProjectionSourceD = 0;
    gWorldProjectionSnapshotC = 0;
    gWorldProjectionSnapshotD = 0;
    return 0;
}

// 原 [0x468F70,0x468FE4)；F32 局部投影、加 0.5 后截断，依次发布 X/Y 及两槽。
std::int64_t __cdecl Sub_468F70()
{
    float x;
    float y;
    ProjectWorldToScreen_46CE30(0.0F, 0.0F, &x, &y);
    const std::int64_t projectedX = static_cast<std::int64_t>(
        static_cast<double>(x) + 0.5);
    const double roundedY = static_cast<double>(y) + 0.5;
    gWorldProjectionSourceA = static_cast<std::uint32_t>(projectedX);
    const std::int64_t projectedY = static_cast<std::int64_t>(roundedY);
    gWorldProjectionSourceC = static_cast<std::uint32_t>(projectedX);
    gWorldProjectionSnapshotA = static_cast<std::uint32_t>(projectedX);
    gWorldProjectionSourceB = static_cast<std::uint32_t>(projectedY);
    gWorldProjectionSourceD = static_cast<std::uint32_t>(projectedY);
    gWorldProjectionSnapshotB = static_cast<std::uint32_t>(projectedY);
    gValueC3500C = 0;
    gValueC75010 = 0;
    return projectedY;
}

// 原 [0x46CF00, 0x46CF1A)；上界外返回入口位模式，上界内读唯一共享表。
std::int32_t __cdecl Sub_46CF00(const std::int32_t value)
{
    if (static_cast<std::uint32_t>(value) >= 65536u) {
        return value;
    }
    if (value < 0) {
        return 0;
    }
    return gValuesC35010[static_cast<std::size_t>(value)];
}

// 原 [0x46CF20, 0x46CF57)；两段顺序填值，返回原生表一过尾指针。
std::int32_t* __cdecl Sub_46CF20()
{
    for (std::int32_t index = 0; index < 20000; ++index) {
        gValuesC35010[index] = index;
    }
    for (std::int32_t index = 20000; index < 65536; ++index) {
        gValuesC35010[index] = index + 200000;
    }
    return gValuesC35010 + 65536;
}

// 原 [0x46F8F0,0x46F92B)；每轮按 360 上界、0 下界顺序重读并原地修正。
void __cdecl Sub_46F8F0(float* const value)
{
    constexpr float kFullTurn = 360.0F;
    for (;;) {
        const float upperValue = *value;
        if (static_cast<double>(upperValue) >= 360.0) {
            *value = upperValue - kFullTurn;
            continue;
        }

        const float lowerValue = *value;
        if (static_cast<double>(lowerValue) >= 0.0) {
            return;
        }
        *value = lowerValue + kFullTurn;
    }
}

// 原 [0x46F930,0x46F94E)：F32 归一化、向零截断索引，原表 F64 位型返回。
double __cdecl Sub_46F930(float angle)
{
    Sub_46F8F0(&angle);
    const std::int32_t index = static_cast<std::int32_t>(angle);
    return std::bit_cast<double>(
        gAngleTableBits55DF10[static_cast<std::size_t>(index)]);
}

// 原 [0x46F950,0x46F967)：先将 +90 的结果写为 F32，再透传表读取结果。
double __cdecl Sub_46F950(const float angle)
{
    const float shifted = angle + 90.0F;
    return Sub_46F930(shifted);
}

// 原 [0x46F970,0x46FACF)：原表插值及八路象限修正，F32 写槽独立保留。
double __cdecl Sub_46F970(const float first, const float second)
{
    if (first == 0.0F && second == 0.0F) {
        return 0.0;
    }

    const bool firstNegative = first < 0.0F;
    const bool secondNegative = second < 0.0F;
    const double firstAbs = firstNegative ? -static_cast<double>(first)
                                          : static_cast<double>(first);
    const double secondAbs = secondNegative ? -static_cast<double>(second)
                                            : static_cast<double>(second);
    const bool swapAxes = firstAbs < secondAbs;
    const std::int32_t selector =
        (firstNegative ? 1 : 0) + (secondNegative ? 2 : 0) + (swapAxes ? 4 : 0);
    const double scaled =
        (swapAxes ? firstAbs / secondAbs : secondAbs / firstAbs) * 64.0;
    const std::int32_t index = static_cast<std::int32_t>(scaled);
    const std::int32_t nextIndex = index + 1 > 64 ? 64 : index + 1;
    const float fraction = static_cast<float>(scaled - static_cast<double>(index));
    const double firstWeight = 1.0 - static_cast<double>(fraction);
    const double left = std::bit_cast<double>(
        gAngleTableBits55EA50[static_cast<std::size_t>(index)]) * firstWeight;
    const double right = std::bit_cast<double>(
        gAngleTableBits55EA50[static_cast<std::size_t>(nextIndex)])
        * static_cast<double>(fraction);
    const float angle = static_cast<float>(left + right);

    switch (selector) {
    case 0: return static_cast<double>(angle);
    case 1: return 180.0 - static_cast<double>(angle);
    case 2: return 360.0 - static_cast<double>(angle);
    case 3: return static_cast<double>(angle) + 180.0;
    case 4: return 90.0 - static_cast<double>(angle);
    case 5: return static_cast<double>(angle) + 90.0;
    case 6: return static_cast<double>(angle) + 270.0;
    case 7: return 270.0 - static_cast<double>(angle);
    default: return 0.0;
    }
}

// 原 [0x46CDF0,0x46CE25)：角度写回 F32、归一化，再乘原常量截断为 S64。
std::int64_t __cdecl Sub_46CDF0(const float first, const float second)
{
    const double direction = Sub_46F970(first, second);
    const float offset = std::bit_cast<float>(0x42E10000U);
    float angle = static_cast<float>(direction + static_cast<double>(offset));
    Sub_46F8F0(&angle);
    const float multiplier = std::bit_cast<float>(0x3CB60B61U);
    return static_cast<std::int64_t>(
        static_cast<double>(angle) * static_cast<double>(multiplier));
}

// 原 [0x46B580,0x46B5B3)：先写 Y 差 F32，再写 X 差 F32 并透传方向 S64。
std::int64_t __cdecl Sub_46B580(
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth)
{
    const float differenceY = static_cast<float>(fourth - second);
    const float differenceX = static_cast<float>(third - first);
    return Sub_46CDF0(differenceX, differenceY);
}

// 原 [0x4D3170,0x4D317B)：仅将 0x581800 写为 -1 后返回。
void Sub_4D3170()
{
    gValue581800 = -1;
}

// 原 [0x4410E0,0x4410E6)：无状态固定返回叶。
std::int32_t Sub_4410E0()
{
    return 0x11;
}

// 原 [0x455BD0,0x455C56)：按指令顺序写 23 个独立槽，返回完整 EAX32 零。
std::int32_t Sub_455BD0()
{
    gValue556C14 = 1;
    gValue62B34E = 0;
    gValue62B34C = 0;
    gValue62B318 = 0;
    gValue62B328 = 0;
    gValue62B3B0 = 1;
    gValue556C34 = 1;
    gValue62B5AA = 0;
    gValue62B5A8 = 0;
    gValue62B5B0 = 0;
    gValue62B308 = 0;
    gValue62B358 = 0;
    gValue62B384 = 0;
    gValue62B310 = 0;
    gValue62B364 = 0;
    gValue62B338 = 0;
    gValue62B36C = 0;
    gValue62B2F8 = 0;
    gValue62B324 = 0;
    gValue62B30C = 0;
    gValue62B388 = 0;
    gValue62B3A0 = 0;
    gValue62B38C = 0;
    return 0;
}

// 原 [0x46BD40,0x46BD57)：按指令顺序清四个独立 W32 槽，返回完整 EAX32 零。
std::int32_t Sub_46BD40()
{
    gValue9FB934 = 0;
    gValue9FB928 = 0;
    gValue9A86C0 = 0;
    gValue9A7F78 = 0;
    return 0;
}

// 原 [0x504A30,0x504A43)：先加载记录，弃返回，再清两槽并返回完整零。
std::int32_t Sub_504A30()
{
    Sub_44A090();
    gValueD2F788 = 0;
    gValueFF0C10 = 0;
    return 0;
}

// 原 [0x43C8F0,0x43C90A)；按序写四个独立 S32 槽并返回完整 -1。
std::int32_t Sub_43C8F0()
{
    gValue5ABB70 = 0;
    gValue5ABB6C = 0;
    gValue553F90 = -1;
    gValue553F84 = -1;
    return -1;
}

// 原 [0x504100,0x504113)；依次清 W32/W16/W32 三槽并返回完整 EAX32 零。
std::int32_t Sub_504100()
{
    gValueD2F788 = 0;
    gValueFF0C10 = 0;
    gValueCAFEA0 = 0;
    return 0;
}

// 原 [0x45ED80,0x45ED8B)：只清场景 7 阶段槽，不定义返回值。
void Sub_45ED80()
{
    gValue9A6204 = 0;
}

// 原 [0x45ED90,0x45EEDA)：场景 7 请求进度与结果展示。
std::int32_t Sub_45ED90()
{
    std::int32_t result = 0;
    if (gValue9A6204 == 0) {
        const std::uint32_t textLength =
            static_cast<std::uint32_t>(std::strlen(gText55BAE0));
        const std::int32_t columns = static_cast<std::int32_t>(
            ((textLength * 9U) >> 6) + 2U);
        gValue9A6204 = 1;
        gValue9A5674 = 2;
        gValue9A594C = columns;
        const std::int32_t left = ((10 - columns) * 64) / 2;
        gValue9A61F8 = 192;
        gValue9A5AC0 = left;
        gAction9A6E98 = login::detail::Sub_5044E0(
            left, 192, columns * 64, 96, 4, 1);
    }

    const std::uint32_t phase = gValue9A6204;
    if (phase == 1) {
        if (login::detail::gReplyCodeDA3B2E != 2) {
            static_cast<void>(login::detail::Sub_508C40());
        }
        ++gValue9A6204;
    } else if (phase == 2) {
        result = login::detail::Sub_508C90();
        if (result != 0) {
            if (gAction9A6E98 != nullptr) {
                login::detail::MarkDeath(gAction9A6E98);
                gAction9A6E98 = nullptr;
            }
            return result;
        }
    }

    login::YobiAction* const action = gAction9A6E98;
    if (action != nullptr && login::detail::IsYobiActionReady(action, 1)) {
        const std::int32_t rows = gValue9A5674;
        const std::int32_t top = gValue9A61F8;
        const std::int32_t drawY = top + (48 * rows - 16) / 2;
        const std::int32_t textLength =
            static_cast<std::int32_t>(std::strlen(gText55BAE0));
        const std::int32_t columns = gValue9A594C;
        const std::int32_t left = gValue9A5AC0;
        const std::int32_t drawX =
            left + (8 * (8 * columns - textLength)) / 2;
        static_cast<void>(render::DrawText3_456560(
            static_cast<std::int16_t>(drawX),
            static_cast<std::int16_t>(drawY),
            2, 0, gText55BAE0, 0));
    }
    return result;
}

} // namespace detail

} // namespace cg::world
