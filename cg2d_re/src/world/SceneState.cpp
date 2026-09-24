#include "cg/world/SceneState.hpp"

#include "../LoginRequestDependencies.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/render/TextPendingQueue.hpp"
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

// 原 0x55BAE0 的可写八字节区，正文六字节、NUL 后保留零填充。
char gText55BAE0[8] = "\xB5\x6E\xA5\x58\xA4\xA4";

// 原 0x55DF10：保留可写数据身份，不以运行期三角函数重算原表。
std::array<std::uint64_t, 360> gAngleTableBits55DF10 {
#include "SceneAngleTable55DF10.inc"
};

static_assert(sizeof(kCutPath55DE50) == 23);
static_assert(sizeof(kCutReadMode554920) == 3);
static_assert(sizeof(kCutMessage55DE3C) == 20);
static_assert(sizeof(kCutCaption5536B8) == 5);
static_assert(sizeof(kCutMessage55DE20) == 14);
static_assert(sizeof(kCutCaption55DE30) == 9);
static_assert(sizeof(kAutPath55DE7C) == 23);
static_assert(sizeof(kAutMessage55DE68) == 20);
static_assert(sizeof(gText55BAE0) == 8);
static_assert(sizeof(gAngleTableBits55DF10) == 2880);

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

// 原 0xC82230/0xC82234/0xC82236；供 46A9E0 初始化和后续消费者共享。
std::uint32_t gValueC82230 = 0;
std::int16_t gValueC82234 = 0;
std::int16_t gValueC82236 = 0;

// 原 0xC3500C/0xC75010/0x9EE8A0/0x9ED438；468F30 的独立 W32 槽。
std::uint32_t gValueC3500C = 0;
std::uint32_t gValueC75010 = 0;
std::uint32_t gValue9EE8A0 = 0;
std::uint32_t gValue9ED438 = 0;

// 原 0xC35008/0x9DAD2C/0x9FB930/0xC821D4/0x9A86DC；4682E0 的 W32 目标。
std::uint32_t gValueC35008 = 0;
std::uint32_t gValue9DAD2C = 0;
std::uint32_t gValue9FB930 = 0;
std::uint32_t gValueC821D4 = 0;
std::uint32_t gValue9A86DC = 0;

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

// 原 [0x4D3170,0x4D317B)：仅将 0x581800 写为 -1 后返回。
void Sub_4D3170()
{
    gValue581800 = -1;
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
