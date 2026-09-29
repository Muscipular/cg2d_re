#include "cg/world/SceneState.hpp"

#include "../CharacterSelectSceneDependencies.hpp"
#include "../LoginMessageDependencies.hpp"
#include "../LoginInputDependencies.hpp"
#include "../LoginRequestDependencies.hpp"
#include "../ui/WindowRuntimeState.hpp"
#include "cg/Input.hpp"
#include "cg/Blacklist.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/Palette.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/GraphicAlloc.hpp"
#include "cg/ResourcePaths.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/GameTime.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "../platform/MapEffectMath.hpp"
#include "WorldUpdateDependencies.hpp"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <direct.h>
#include <new>
#include <utility>
#include <vector>

namespace cg::world {
namespace {

// 原 [C8A7B0,C8A940) 的 100 项 W32 表；C8A940 是独立消费游标。
std::array<std::int32_t, 100> gRotationTableC8A7B0 {};
std::int32_t gRotationCursorC8A940 = 0;
std::uint32_t gValue62B348 = 0;
static_assert(sizeof(gRotationTableC8A7B0) == 0x190);

// 原 [9E03D0,9E0434)：46AC10先写前缀，再按发送门禁决定是否写NUL。
std::array<char, 100> gMoveDirectionText9E03D0 {};
static_assert(sizeof(gMoveDirectionText9E03D0) == 100);

// 原 [DA43E8,DE03F4)：相册头与两组记录的唯一持久字节区；见场景证据 §58。
alignas(std::uint32_t) std::array<std::byte, 0x3C00C> gAlbumStorage {};
static_assert(sizeof(gAlbumStorage) == 0x0C + 2 * 960 * 0x80);

// 借用唯一相册字节区；index从0开始，selected保持原S16符号扩展。
std::byte* AlbumRecordAt(const std::int32_t index, const std::int16_t selected)
{
    const std::ptrdiff_t record = index + static_cast<std::ptrdiff_t>(selected) * 960;
    return gAlbumStorage.data() + 0x0C + record * 0x80;
}

// 原 0x9A7FA8；由 LegacyMouse.cpp 的 CRT 顺序适配显式构造、析构。
platform::LegacyCryptInt gRippleGate {};

// 原62B5B8；455FC0持有非拥有动画引用，4758D0经借用槽地址完成清空。
login::YobiAction* gWindow39Action62B5B8 = nullptr;

// 原FF0C14四条结果记录；50D500按两次W16、一次S32的顺序发布。
struct BattleResultRecord {
    std::uint16_t value00;
    std::uint16_t value02;
    std::int32_t value04;
};
static_assert(offsetof(BattleResultRecord, value00) == 0);
static_assert(offsetof(BattleResultRecord, value02) == 2);
static_assert(offsetof(BattleResultRecord, value04) == 4);
static_assert(sizeof(BattleResultRecord) == 8);
std::array<BattleResultRecord, 4> gBattleResultsFF0C14 {};

// 原FF0C8C十五条技能结果；+2两字节不由50D500写入，末条止FF0D04。
struct BattleSkillResultRecord {
    std::int16_t value00;
    std::array<std::byte, 2> bytes02;
    std::int32_t value04;
};
static_assert(offsetof(BattleSkillResultRecord, value00) == 0);
static_assert(offsetof(BattleSkillResultRecord, bytes02) == 2);
static_assert(offsetof(BattleSkillResultRecord, value04) == 4);
static_assert(sizeof(BattleSkillResultRecord) == 8);
std::array<BattleSkillResultRecord, 15> gBattleSkillResultsFF0C8C {};

// 原FF0C34三条29B名称与FF0D0C三个图号；结果展示须借用同一存储。
std::array<std::array<char, 29>, 3> gBattleResultNamesFF0C34 {};
std::array<std::int32_t, 3> gBattleResultGraphicsFF0D0C {};
static_assert(sizeof(gBattleResultsFF0C14) == 0x20);
static_assert(sizeof(gBattleSkillResultsFF0C8C) == 0x78);
static_assert(sizeof(gBattleResultNamesFF0C34) == 0x57);
static_assert(sizeof(gBattleResultGraphicsFF0D0C) == 0x0C);

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

// 原首块 [FDA700,FF02E0) 已证玩家槽；不将此原生结构当作原连续内存。
struct PlayerRuntimeSlots {
    std::uint16_t value00; // 原FDA700；509BA0仅写低W16。
    std::int32_t level; // 原 FDA704。
    std::int32_t value08; // 原FDA708。
    std::int32_t timerRemaining0C; // 原FDA70C；458F40按帧扣减的S32。
    std::int32_t timerMode10; // 原FDA710；保留完整模式，非零才扣减。
    std::uint32_t timerTick14; // 原FDA714；50C7A0与458F40同源。
    platform::LegacyCryptInt hp; // 原 FDA718；与 action +170 的载荷同步。
    platform::LegacyCryptInt maxHp; // 原 FDA728。
    platform::LegacyCryptInt fp; // 原 FDA738；与宠物数据独立。
    platform::LegacyCryptInt maxFp; // 原 FDA748。
    platform::LegacyCryptInt value58; // 原 FDA758；5184A0第五构造对象。
    platform::LegacyCryptInt value68; // 原 FDA768；5184A0第六构造对象。
    std::array<std::int32_t, 14> values78; // 原FDA778..FDA7AC，509650两段数值。
    std::array<std::int32_t, 2> valuesB0; // 原FDA7B0/FDA7B4，509BA0尾部两值。
    std::int32_t valueB8; // 原FDA7B8。
    std::array<std::int32_t, 6> valuesBC; // 原FDA7BC..FDA7D0。
    std::array<std::int32_t, 4> valuesD4; // 原FDA7D4..FDA7E0。
    std::array<std::int32_t, 4> valuesE4; // 原FDA7E4..FDA7F0。
    std::int32_t valueF4; // 原FDA7F4。
    std::int32_t valueF8; // 原FDA7F8。
    std::int32_t valueFC; // 原FDA7FC；与末尾15BCC共用回复门禁。
    std::int32_t titleIndex100; // 原FDA800，直接索引FF0508原表。
    std::int32_t value104; // 原FDA804。
    std::array<char, 17> name; // 原 FDA808；旧编码字节。
    std::array<char, 17> ownTitle; // 原 FDA819。
    std::array<char, 33> text12A; // 原 FDA82A。
    std::array<char, 33> text14B; // 原 FDA84B。
    std::int32_t value16C; // 原FDA86C。
    std::int32_t walkSpeed; // 原 FDA870；独立于通用 action 的初始步速。
    std::int32_t value174; // 原 FDA874；46A700仅在到达时按S32递减。
    std::int32_t imageNo; // 原 FDA878，不是 FDA87C。
    std::int32_t value17C; // 原FDA87C，不与imageNo合并。
    std::uint32_t objectIndex;
    std::int32_t direction; // 原 FDA884；其他消费者可读取低 S8。
    std::uint16_t value188; // 原 FDA888；519A80 符号扩展读取。
    std::uint16_t flags18A; // 原 FDA88A W16；跨 W32 读取还须拼接 flags18C 的低 W16。
    std::uint32_t flags18C; // 承载原 [FDA88C,FDA890)；18C 低 W8、18E 高 W16 共享字节。
    std::int32_t value190; // 原 FDA890；51BDD0/51BE50 写入的完整动作值。
    std::uint16_t options194; // 原FDA894低W16；选项切换/回复与聊天门禁共用。
    std::uint16_t value1A0; // 原FDA8A0；50BFC0写入低W16。
    std::array<detail::PlayerItemRecord, 28> items; // 原FDA8A4..FF02B4，唯一物品owner。
    login::YobiAction* action;
    std::array<char, 17> text15BB8; // 原 FF02B8，不紧接新端的指针字节布局。
    std::int32_t value15BCC; // 原FF02CC；509650尾字段。
    std::array<std::int32_t, 3> values15BD0; // 原FF02D0/FF02D4/FF02D8。
};

PlayerRuntimeSlots gPlayerRuntime {};

// 原第四清零块[F1BAE0,F633EC)，既有技能/索引表改为本owner成员别名。
struct SkillRuntimeSlots {
    std::array<char, 0x1C> jobName;
    std::array<detail::SkillRuntimeRecord, 15> skills;
    std::array<detail::SkillPairRecord, 15> pairs;
    std::uint32_t valueF633E8; // 509E30只写低U8，保留其余字节。
};

static_assert(std::is_standard_layout_v<SkillRuntimeSlots>);
static_assert(std::is_trivially_copyable_v<SkillRuntimeSlots>);
static_assert(offsetof(SkillRuntimeSlots, jobName) == 0);
static_assert(offsetof(SkillRuntimeSlots, skills) == 0x1C);
static_assert(offsetof(SkillRuntimeSlots, pairs) == 0x47890);
static_assert(offsetof(SkillRuntimeSlots, valueF633E8) == 0x47908);
static_assert(sizeof(SkillRuntimeSlots) == 0x4790C);
SkillRuntimeSlots gSkillRuntimeSlots {};

std::uint32_t gChatPreviousTickDA3B3C = 0;
std::uint32_t gChatCurrentTickDA3B40 = 0;
std::uint8_t gChatDisabledDA3B4C = 0;
std::int32_t gChatGateDA3B54 = 0; // 50CAE0写入，50C940读取同一接收门禁。
constexpr char kChatDisabledText58AFEC[] =
    "\xA5\xD8\xAB\x65\xB8\x54\xA8\xA5\xA4\xA4";
constexpr char kChatRepeatedText58B000[] =
    "\xAD\xAB\xBD\xC6\xB5\x6F\xA8\xA5\xA1\x41"
    "\xB8\x54\xA8\xA5\xA4\x54\xA4\xC0\xC4\xC1";
static_assert(sizeof(kChatDisabledText58AFEC) == 11);
static_assert(sizeof(kChatRepeatedText58B000) == 21);

// 原 [FF02E0,FF03D0) 的五槽表；与玩家首块分属不同清零区。
struct PetMapCharaSlot {
    std::uint16_t flags;
    std::array<std::byte, 2> bytes02;
    std::uint32_t objectIndex;
    std::int32_t level08; // 50B380玩家分支同步第0槽。
    std::int32_t value0C; // 50A3E0按掩码0x11写。
    std::int32_t value10; // 50A3E0按掩码0x09写，在value0C之前消费。
    std::int32_t value14; // 50A3E0按掩码0x21写。
    std::array<char, 17> name18; // 原+18，旧编码名称。
    std::array<std::byte, 3> bytes29;
    login::YobiAction* action; // 原 +2C，非拥有。
};

static_assert(offsetof(PetMapCharaSlot, flags) == 0);
static_assert(offsetof(PetMapCharaSlot, objectIndex) == 4);
static_assert(offsetof(PetMapCharaSlot, level08) == 8);
static_assert(offsetof(PetMapCharaSlot, value0C) == 0x0C);
static_assert(offsetof(PetMapCharaSlot, value10) == 0x10);
static_assert(offsetof(PetMapCharaSlot, value14) == 0x14);
static_assert(offsetof(PetMapCharaSlot, name18) == 0x18);
static_assert(offsetof(PetMapCharaSlot, action) == 0x30);
static_assert(sizeof(PetMapCharaSlot) == 0x38);
std::array<PetMapCharaSlot, 5> gPetMapCharaSlots {};
// 原 [9FB938,9FB942)：两次W32与末次W16清理、后续S16读取共用同一10B。
alignas(std::uint32_t) std::array<std::uint16_t, 5> gPetFollowOrder {};
static_assert(sizeof(gPetFollowOrder) == 10);

// 原 C8A73E/C8A740/C8A744 及效果配置共享槽；第三槽的两个 W16 共用 W32。
struct MapCharaSceneSlots {
    std::uint16_t valueC8A73C;
    std::uint16_t valueC8A73E;
    std::uint16_t valueC8A740;
    std::uint16_t valueC8A742;
    std::uint32_t valueC8A744;
    std::uint16_t valueC8A748;
    std::uint16_t value55DE98 {0xFFFF};
    std::uint16_t valueC8A74A;
    std::uint16_t valueC8A74C;
    std::uint16_t valueC8A74E;
    std::uint16_t valueC8A750;
    std::uint16_t valueC8A752;
    std::uint16_t valueC8A754;
    std::uint16_t valueC8A756;
    std::uint32_t valueC8A758;
    std::uint32_t valueC8A75C;
    std::int16_t valueC8A760;
    std::int16_t valueC8A762;
    login::YobiAction* actionC8A764;
    std::uint32_t valueC8A768;
    std::uint32_t valueC8A76C;
    float value55DE9C {5.0F};
    std::int16_t value55DEA0 {315};
    std::int16_t value55DEA2 {-1};
};

MapCharaSceneSlots gMapCharaSceneSlots {};

// 原 .data [55DEB0,55DF10)：前/后三项分别供 46F1D0 的 kind 6/7 绘图。
std::array<render::GraphicDescriptor16, 6> gEffectDrawModes55DEB0 {{
    {{0x00, 0x00, 0x10, 0xFF, 0x40, 0x01, 0x22, 0x01,
      0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00}},
    {{0xC0, 0xFE, 0x40, 0x00, 0x40, 0x01, 0x0E, 0x01,
      0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00}},
    {{0x00, 0x00, 0x40, 0x00, 0x40, 0x01, 0x72, 0x01,
      0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00}},
    {{0xC0, 0xFE, 0x20, 0xFE, 0x40, 0x01, 0x22, 0x01,
      0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00}},
    {{0x00, 0x00, 0x20, 0xFE, 0x40, 0x01, 0xF0, 0x00,
      0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00}},
    {{0xC0, 0xFE, 0x4B, 0xFF, 0x40, 0x01, 0xF0, 0x00,
      0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00}},
}};

static_assert(sizeof(gEffectDrawModes55DEB0) == 0x60);

// 46AA00的无参调用适配；原4B0F10不读参数，共用UI回调的真实零返回实现。
std::int32_t Sub_4B0F10()
{
    return ui::detail::Sub_4B0F10(0, 0);
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

// 原 468520 路径与 4687B0 创建模式；与命令行 mapfolder: 配置无关。
constexpr char kMapPathPrefix[] = "map";
constexpr char kMapPathZero[] = "%s\\%d\\%d.dat";
constexpr char kMapPathOther[] = "%s\\%d\\%d\\%d.dat";
constexpr char kMapDirectoryOne[] = "%s\\%d";
constexpr char kMapDirectoryTwo[] = "%s\\%d\\%d";
constexpr char kMapCreateMode[] = "wb";
constexpr char kMapReadMode[] = "rb";
constexpr char kMapUpdateMode[] = "rb+";
constexpr char kMapHeaderLabel[] = "MAP";
constexpr char kUnknownText554590[] = "???";

// 原 [9DAD30,9DAD4A)；地图回复的 24 字节正文和 NUL，首字节另有世界重置写者。
std::array<std::uint8_t, 26> gText9DAD30 {};

// 原 55DAAC/55DBAC；两表各 256 项，原始内容分别为逐位反转与 0x1021 多项式表。
constexpr auto kMapBitReverse55DAAC = [] {
    std::array<std::uint8_t, 256> table {};
    for (std::size_t i = 0; i < table.size(); ++i) {
        std::uint8_t value = static_cast<std::uint8_t>(i);
        std::uint8_t reversed = 0;
        for (int bit = 0; bit < 8; ++bit) {
            reversed = static_cast<std::uint8_t>((reversed << 1) | (value & 1u));
            value >>= 1;
        }
        table[i] = reversed;
    }
    return table;
}();

constexpr auto kMapCrcTable55DBAC = [] {
    std::array<std::uint16_t, 256> table {};
    for (std::size_t i = 0; i < table.size(); ++i) {
        std::uint16_t value = static_cast<std::uint16_t>(i << 8);
        for (int bit = 0; bit < 8; ++bit) {
            value = static_cast<std::uint16_t>(
                (value << 1) ^ ((value & 0x8000u) != 0 ? 0x1021u : 0u));
        }
        table[i] = value;
    }
    return table;
}();

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
static_assert(sizeof(kMapDirectoryOne) == 6);
static_assert(sizeof(kMapDirectoryTwo) == 9);
static_assert(sizeof(kMapCreateMode) == 3);
static_assert(sizeof(kMapReadMode) == 3);
static_assert(sizeof(kMapUpdateMode) == 4);
static_assert(sizeof(kMapHeaderLabel) == 4);
static_assert(sizeof(kUnknownText554590) == 4);
static_assert(sizeof(gText9DAD30) == 26);
static_assert(kMapBitReverse55DAAC[1] == 0x80u);
static_assert(kMapCrcTable55DBAC[1] == 0x1021u);
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
std::uint8_t& gValue9DAD30 = gText9DAD30[0];
std::array<std::uint32_t, 7> gValueD67B08 {};
std::uint32_t gValue58B6D0 = 1;
std::uint8_t gValueD9F2CA = 0;

const char* ReadText9DAD30() noexcept
{
    return reinterpret_cast<const char*>(gText9DAD30.data());
}

std::uint32_t ReadPlayerObjectIndex_FDA880() noexcept
{
    return gPlayerRuntime.objectIndex;
}

void Sub_51BEF0() noexcept
{
    gPlayerRuntime.flags18C &= 0xFF7FFFFFu;
}

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

void ConstructPlayerRuntimeValues()
{
    platform::legacy_crypt_int::ConstructZero_401C70(&gPlayerRuntime.hp);
    platform::legacy_crypt_int::ConstructZero_401C70(&gPlayerRuntime.maxHp);
    platform::legacy_crypt_int::ConstructZero_401C70(&gPlayerRuntime.fp);
    platform::legacy_crypt_int::ConstructZero_401C70(&gPlayerRuntime.maxFp);
    platform::legacy_crypt_int::ConstructZero_401C70(&gPlayerRuntime.value58);
    platform::legacy_crypt_int::ConstructZero_401C70(&gPlayerRuntime.value68);
}

void DestroyPlayerRuntimeValues()
{
    platform::legacy_crypt_int::Destroy_401CF0(&gPlayerRuntime.value68);
    platform::legacy_crypt_int::Destroy_401CF0(&gPlayerRuntime.value58);
    platform::legacy_crypt_int::Destroy_401CF0(&gPlayerRuntime.maxFp);
    platform::legacy_crypt_int::Destroy_401CF0(&gPlayerRuntime.fp);
    platform::legacy_crypt_int::Destroy_401CF0(&gPlayerRuntime.maxHp);
    platform::legacy_crypt_int::Destroy_401CF0(&gPlayerRuntime.hp);
}

bool IsValidPetIndex_51C410(const std::int32_t index) noexcept
{
    return index >= 0 && index < 5;
}

void ConstructPetRuntimeValues()
{
    for (detail::PetRuntimeRecord& record : detail::gPetRuntimeF633F8) {
        for (platform::LegacyCryptInt& value : record.values0C) {
            platform::legacy_crypt_int::ConstructZero_401C70(&value);
        }
    }
}

void DestroyPetRuntimeValues()
{
    for (std::size_t record = 5; record != 0; --record) {
        for (std::size_t value = 6; value != 0; --value) {
            platform::legacy_crypt_int::Destroy_401CF0(
                &detail::gPetRuntimeF633F8[record - 1].values0C[value - 1]);
        }
    }
}

void ConstructSceneTransitionValue()
{
    platform::legacy_crypt_int::ConstructZero_401C70(&detail::gValue73ECD0);
}

void DestroySceneTransitionValue()
{
    platform::legacy_crypt_int::Destroy_401CF0(&detail::gValue73ECD0);
}

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

float* ProjectScreenToWorld_46CE80(
    const float screenX, const float screenY,
    float* const outX, float* const outY) noexcept
{
    const double u = static_cast<double>(screenX) - 320.0;
    const double v = (static_cast<double>(screenY) - 240.0)
        * static_cast<double>(std::bit_cast<float>(0x3FAAAAABu));
    const std::uint32_t referenceXBits = gWorldProjectionReferenceXBits;
    *outX = static_cast<float>(u - v + std::bit_cast<float>(referenceXBits));
    const std::uint32_t referenceYBits = gWorldProjectionReferenceYBits;
    *outY = static_cast<float>(v + u + std::bit_cast<float>(referenceYBits));
    return outX;
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

// 原 [46FB20,46FB45)：仅消费一次共享 RAND，生产表时不重置消费游标。
std::int32_t InitializeRotationTable_46FB20()
{
    std::int32_t index = platform::NextRandom_53B4C9() % 100;
    for (std::int32_t value = 0; value < 100; ++value) {
        gRotationTableC8A7B0[static_cast<std::size_t>(index)] = value;
        ++index;
        if (index >= 100) {
            index = 0;
        }
    }
    return 100;
}

// 原 455C70 跳入独占尾块 [46FB50,46FB74)：先清状态，再推进游标。
std::int32_t NextRotationValue_455C70()
{
    gValue62B348 = 0;
    detail::gValue62B308 = 0;
    const std::int32_t value =
        gRotationTableC8A7B0[static_cast<std::size_t>(gRotationCursorC8A940)];
    const std::int32_t next = gRotationCursorC8A940 + 1;
    gRotationCursorC8A940 = next;
    if (next >= 100) {
        gRotationCursorC8A940 = 0;
    }
    return value;
}

std::uint32_t InitializeAlbum_518C20()
{
    constexpr char kAlbumSignature[] = "CCA";
    gAlbumStorage.fill(std::byte{0});
    std::memcpy(gAlbumStorage.data(), kAlbumSignature, sizeof(kAlbumSignature));
    return 4;
}

namespace detail {

platform::LegacyCryptInt gValue73ECD0 {};

std::uint32_t& gMapEffectOffsetXBits = gMapCharaSceneSlots.valueC8A758;
std::uint32_t& gMapEffectOffsetYBits = gMapCharaSceneSlots.valueC8A75C;

// 原[5186A0,518774)：完整快照交旧codec，但原目标容量仅4；短写不关闭文件。
void Sub_5186A0()
{
    auto* const encoded = static_cast<std::uint8_t*>(::operator new(0x78018));
    char path[128];
    std::sprintf(path, "%s\\%s", platform::gDataPath, "album.dat");
    std::FILE* const stream = std::fopen(path, "wb");
    if (stream == nullptr) {
        ::operator delete(encoded);
        return;
    }
    std::array<std::uint8_t, 0x3C00C> snapshot;
    std::memcpy(snapshot.data(), gAlbumStorage.data(), snapshot.size());
    std::int32_t encodedBytes;
    login::detail::EncodeSavedBytes(
        snapshot.data(), 0x3C00C, 0, encoded, &encodedBytes, 4);
    const std::size_t count = static_cast<std::uint32_t>(encodedBytes);
    const std::size_t written = std::fwrite(encoded, 1, count, stream);
    if (written < count) {
        ::operator delete(encoded);
        return;
    }
    static_cast<void>(std::fclose(stream));
    ::operator delete(encoded);
}

// 原[518780,518865)：只读4字节；错误先关闭，长度不符调用同源保存。
void Sub_518780()
{
    auto* const encoded = static_cast<std::uint8_t*>(::operator new(0x78018));
    auto* const decoded = static_cast<std::uint8_t*>(::operator new(0x78018));
    char path[128];
    std::sprintf(path, "%s\\%s", platform::gDataPath, "album.dat");
    std::FILE* const stream = std::fopen(path, "rb");
    if (stream == nullptr) {
        Sub_5186A0();
    } else {
        const std::size_t count = std::fread(encoded, 1, 4, stream);
        if (std::ferror(stream) != 0) {
            static_cast<void>(std::fclose(stream));
            ::operator delete(encoded);
            ::operator delete(decoded);
            return;
        }
        static_cast<void>(std::fclose(stream));
        std::int32_t decodedBytes;
        static_cast<void>(login::detail::DecodeSavedBytes(
            encoded, static_cast<std::int32_t>(count), 0, decoded, &decodedBytes));
        if (decodedBytes == 0x3C00C) {
            std::memcpy(gAlbumStorage.data(), decoded, gAlbumStorage.size());
        } else {
            Sub_5186A0();
        }
    }
    ::operator delete(encoded);
    ::operator delete(decoded);
}

// 原[518BE0,518C12)：固定当前角色，清版本和整组记录后保存。
void Sub_518BE0()
{
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const std::int32_t zero = 0;
    std::memcpy(
        gAlbumStorage.data() + 4 + static_cast<std::ptrdiff_t>(selected) * 4,
        &zero, sizeof(zero));
    std::memset(AlbumRecordAt(0, selected), 0, 0x1E000);
    Sub_5186A0();
}

// 原[518870,518A36)：相册记录更新，分行调用后重读角色而不固定首记录地址。
void Sub_518870(
    const std::int32_t number, const std::int32_t imageNo,
    const std::int32_t mark1, const std::int32_t mark2, const std::int32_t tribe,
    const std::int32_t value0C, const std::int32_t value10, const std::int32_t value14,
    const std::int32_t value18, const std::int32_t value1C, const std::int32_t value20,
    const std::int32_t value24, const std::int32_t value28, const std::int32_t value2C,
    const std::int32_t value30, char* const text, const char* const name)
{
    if (number < 1 || number > std::bit_cast<std::int32_t>(resource::gValueD2D5FC)) {
        return;
    }
    const std::int32_t index = number - 1;
    std::byte* record = AlbumRecordAt(index, login::gSelectedCharacterIndex);
    const bool added = record[0] == std::byte{0};
    record[0] = std::byte{1};
    std::memcpy(record + 4, &number, sizeof(number));
    std::memcpy(record + 8, &imageNo, sizeof(imageNo));
    record[1] = std::byte{static_cast<std::uint8_t>(mark1)};
    record[2] = std::byte{static_cast<std::uint8_t>(mark2)};
    record[3] = std::byte{static_cast<std::uint8_t>(tribe)};
    const std::array<std::int32_t, 10> values {
        value0C, value10, value14, value18, value1C,
        value20, value24, value28, value2C, value30,
    };
    for (std::size_t field = 0; field < values.size(); ++field) {
        std::memcpy(record + 0x0C + field * 4, &values[field], sizeof(values[field]));
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::int16_t selected = login::gSelectedCharacterIndex;
    for (std::int32_t line = 0; line < 2; ++line) {
        const std::ptrdiff_t offset = 0x45 + line * 29;
        char* const destination = reinterpret_cast<char*>(AlbumRecordAt(index, selected) + offset);
        const std::int32_t result = input::ExtractLegacyLine_503AB0(
            destination, 29, text, line, 28);
        selected = login::gSelectedCharacterIndex;
        if (result == 0) {
            AlbumRecordAt(index, selected)[offset] = std::byte{0};
        }
    }
    const char* const displayedName = std::strlen(name) > 16 ? "???" : name;
    record = AlbumRecordAt(index, selected);
    std::strcpy(reinterpret_cast<char*>(record + 0x34), displayedName);
    record[0x7F] = std::byte{static_cast<std::uint8_t>(added)};
    Sub_5186A0();
}

// 原[518A40,518BD5)：掩码扫描后始终发送请求列表，再重读角色写版本。
void Sub_518A40(const std::int32_t version, const char* const payload)
{
    std::vector<std::int32_t> requests;
    std::int32_t limit = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
    std::int32_t groups = limit >> 5;
    if ((static_cast<std::uint8_t>(limit) & 0x20u) != 0) {
        ++groups;
    }
    std::int32_t index = 0;
    std::int32_t field = 1;
    std::int32_t mask;
    for (std::int32_t group = 0; group < groups && index < limit; ++group) {
        const std::int32_t ordinal = field++;
        if (network::ParseDelimitedIntegerStatus_52CFF0(payload, '|', ordinal, &mask) > 1) {
            break;
        }
        limit = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
        std::int16_t selected = login::gSelectedCharacterIndex;
        std::uint32_t bit = 1;
        for (std::int32_t position = 0; position < 32 && index < limit; ++position) {
            std::byte* const record = AlbumRecordAt(index, selected);
            if ((static_cast<std::uint32_t>(mask) & bit) != 0) {
                bool request = record[0] == std::byte{0};
                if (!request) {
                    std::int32_t previousVersion;
                    std::memcpy(
                        &previousVersion,
                        gAlbumStorage.data() + 4 + static_cast<std::ptrdiff_t>(selected) * 4,
                        sizeof(previousVersion));
                    request = previousVersion < version;
                }
                if (request) {
                    requests.push_back(index + 1);
                    limit = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
                    selected = login::gSelectedCharacterIndex;
                }
            } else {
                record[0] = std::byte{0};
            }
            ++index;
            bit <<= 1;
        }
    }
    const std::uintptr_t socket = platform::gConnectionSocket;
    static_cast<void>(network::Sub_511030(
        socket, requests.empty() ? nullptr : requests.data(),
        static_cast<std::int32_t>(requests.size())));
    const std::int16_t selected = login::gSelectedCharacterIndex;
    std::memcpy(
        gAlbumStorage.data() + 4 + static_cast<std::ptrdiff_t>(selected) * 4,
        &version, sizeof(version));
}

// 原[50E5B0,50E5C3)：相册版本及掩码原序转传，网络首参不消费。
void Sub_50E5B0(
    const std::uintptr_t /*socket*/, const std::int32_t version, const char* const payload)
{
    Sub_518A40(version, payload);
}

// 原[50E5D0,50E62E)：十五数值及两文本原序转传，不增加接收门禁。
void Sub_50E5D0(
    const std::uintptr_t /*socket*/, const std::int32_t number, const std::int32_t imageNo,
    const std::int32_t mark1, const std::int32_t mark2, const std::int32_t tribe,
    const std::int32_t value0C, const std::int32_t value10, const std::int32_t value14,
    const std::int32_t value18, const std::int32_t value1C, const std::int32_t value20,
    const std::int32_t value24, const std::int32_t value28, const std::int32_t value2C,
    const std::int32_t value30, char* const text, const char* const name)
{
    Sub_518870(number, imageNo, mark1, mark2, tribe, value0C, value10, value14,
        value18, value1C, value20, value24, value28, value2C, value30, text, name);
}

// 原 [C82258,C8A728) 的 500 项记录；未知载荷保留原字节，
// 链接在 x64 使用独立原生指针，不推定整记录最终布局。
struct MapEffectRecord {
    std::array<std::uint8_t, 0x44> legacyBytes;
    MapEffectRecord* prev;
    MapEffectRecord* next;
};

static_assert(sizeof(MapEffectRecord::legacyBytes) == 0x44);

namespace {

std::array<MapEffectRecord, 500> gMapEffectRecords {};
MapEffectRecord* gFreeMapEffectHead = nullptr;   // 原 C82244。
MapEffectRecord* gActiveMapEffectHead = nullptr; // 原 C8A72C。
std::uint8_t gMapEffectStartedC8A728 = 0;
std::uint32_t gMapEffectTickC8A730 = 0;
std::uint8_t gMapEffectStartedC82240 = 0;
std::uint32_t gMapEffectTickC82250 = 0;
std::uint8_t gMapEffectStartedC8224C = 0;
std::uint32_t gMapEffectTickC8A734 = 0;
std::int32_t gProjectedX55DEA4 = -1;
std::int32_t gProjectedY55DEA8 = -1;

} // namespace

std::uint32_t ReadMapEffectWord(
    const MapEffectRecord& record, const std::size_t offset)
{
    std::uint32_t value;
    std::memcpy(&value, record.legacyBytes.data() + offset, sizeof(value));
    return value;
}

std::uint16_t ReadMapEffectHalf(
    const MapEffectRecord& record, const std::size_t offset)
{
    std::uint16_t value;
    std::memcpy(&value, record.legacyBytes.data() + offset, sizeof(value));
    return value;
}

const MapEffectRecord* BorrowActiveMapEffectHead() noexcept
{
    return gActiveMapEffectHead;
}

const MapEffectRecord* BorrowNextMapEffect(const MapEffectRecord* const record) noexcept
{
    return record->next;
}

namespace {

void WriteMapEffectWord(
    MapEffectRecord& record, const std::size_t offset, const std::uint32_t value)
{
    std::memcpy(record.legacyBytes.data() + offset, &value, sizeof(value));
}

void WriteMapEffectHalf(
    MapEffectRecord& record, const std::size_t offset, const std::uint16_t value)
{
    std::memcpy(record.legacyBytes.data() + offset, &value, sizeof(value));
}

void ClearMapEffectAction(login::YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    login::detail::MarkDeath(action);
    MapEffectRecord* record = gActiveMapEffectHead;
    gMapCharaSceneSlots.actionC8A764 = nullptr;
    while (record != nullptr) {
        if (ReadMapEffectHalf(*record, 0x3C) == 8) {
            WriteMapEffectHalf(*record, 0x3C, 6);
            return;
        }
        record = record->next;
    }
}

} // namespace

MapEffectRecord* Sub_46E2A0()
{
    gFreeMapEffectHead = &gMapEffectRecords[0];
    gMapEffectRecords[0].prev = nullptr;
    for (std::size_t index = 0; index + 1 < gMapEffectRecords.size(); ++index) {
        MapEffectRecord* const current = &gMapEffectRecords[index];
        MapEffectRecord* const next = &gMapEffectRecords[index + 1];
        current->next = next;
        next->prev = current;
    }
    MapEffectRecord* const last = &gMapEffectRecords.back();
    last->next = nullptr;
    gActiveMapEffectHead = nullptr;

    gMapCharaSceneSlots.valueC8A73E = 0;
    gMapCharaSceneSlots.valueC8A742 = 0;
    gMapCharaSceneSlots.valueC8A744 &= 0xFFFF0000u;
    gMapCharaSceneSlots.valueC8A748 = 0;
    gMapCharaSceneSlots.value55DE98 = 0xFFFFu;
    gMapCharaSceneSlots.valueC8A74A = 0;
    gMapCharaSceneSlots.valueC8A74C = 1;
    gMapCharaSceneSlots.valueC8A74E = 0;
    gMapCharaSceneSlots.valueC8A750 = 0;
    gMapCharaSceneSlots.valueC8A752 = 0;
    gMapCharaSceneSlots.valueC8A754 = 0;
    gMapCharaSceneSlots.valueC8A756 = 0;
    gMapCharaSceneSlots.valueC8A758 = 0;
    gMapCharaSceneSlots.valueC8A75C = 0;
    return last;
}

MapEffectRecord* Sub_46E340()
{
    MapEffectRecord* const record = gFreeMapEffectHead;
    if (record == nullptr) {
        return record;
    }

    gFreeMapEffectHead = record->next;
    if (gFreeMapEffectHead != nullptr) {
        gFreeMapEffectHead->prev = nullptr;
    }
    record->prev = nullptr;
    record->next = gActiveMapEffectHead;
    if (gActiveMapEffectHead != nullptr) {
        gActiveMapEffectHead->prev = record;
    }
    gActiveMapEffectHead = record;
    return record;
}

MapEffectRecord* Sub_46E390(MapEffectRecord* const record)
{
    if (record == nullptr) {
        return record;
    }

    if (record->prev != nullptr) {
        record->prev->next = record->next;
    }
    if (record->next != nullptr) {
        record->next->prev = record->prev;
    }
    if (gActiveMapEffectHead == record) {
        gActiveMapEffectHead = record->next;
    }
    record->prev = nullptr;
    record->next = gFreeMapEffectHead;
    gFreeMapEffectHead = record;
    WriteMapEffectHalf(*record, 0x3C, 0);
    return record;
}

std::uintptr_t __cdecl Sub_46E3E0(std::int32_t* const x, std::int32_t* const y)
{
    const std::int16_t direction =
        std::bit_cast<std::int16_t>(gMapCharaSceneSlots.value55DE98);
    switch (direction) {
    case 0:
        ++*x;
        ++*y;
        return reinterpret_cast<std::uintptr_t>(y);
    case 1:
        *y += 2;
        return reinterpret_cast<std::uintptr_t>(y);
    case 2:
        --*x;
        ++*y;
        return reinterpret_cast<std::uintptr_t>(y);
    case 3:
        *x -= 2;
        return reinterpret_cast<std::uintptr_t>(x);
    case 4:
        --*x;
        --*y;
        return reinterpret_cast<std::uintptr_t>(y);
    case 5:
        *y -= 2;
        return reinterpret_cast<std::uintptr_t>(y);
    case 6:
        ++*x;
        --*y;
        return reinterpret_cast<std::uintptr_t>(y);
    case 7:
        *x += 2;
        return reinterpret_cast<std::uintptr_t>(x);
    default:
        return static_cast<std::uint32_t>(static_cast<std::int32_t>(direction));
    }
}

namespace {

void ApplyMapEffectDirection(MapEffectRecord& record)
{
    std::int32_t x = std::bit_cast<std::int32_t>(ReadMapEffectWord(record, 0));
    std::int32_t y = std::bit_cast<std::int32_t>(ReadMapEffectWord(record, 4));
    const std::int16_t direction =
        std::bit_cast<std::int16_t>(gMapCharaSceneSlots.value55DE98);
    static_cast<void>(Sub_46E3E0(&x, &y));
    switch (direction) {
    case 0:
    case 2:
    case 4:
    case 6:
        WriteMapEffectWord(record, 0, static_cast<std::uint32_t>(x));
        WriteMapEffectWord(record, 4, static_cast<std::uint32_t>(y));
        break;
    case 1:
    case 5:
        WriteMapEffectWord(record, 4, static_cast<std::uint32_t>(y));
        break;
    case 3:
    case 7:
        WriteMapEffectWord(record, 0, static_cast<std::uint32_t>(x));
        break;
    default:
        break;
    }
}

MapEffectRecord* CreateMapEffectKind1AtZero()
{
    MapEffectRecord* const record = Sub_46E340();
    if (record == nullptr) {
        return nullptr;
    }
    WriteMapEffectHalf(*record, 0x3C, 1);
    const std::int32_t randomX = platform::NextRandom_53B4C9();
    WriteMapEffectWord(*record, 4, 0);
    WriteMapEffectWord(
        *record, 0, static_cast<std::uint32_t>(randomX % 840 - 200));
    const std::int32_t randomSpeed = platform::NextRandom_53B4C9();
    WriteMapEffectWord(*record, 0x0C, 9);
    WriteMapEffectWord(
        *record, 8, static_cast<std::uint32_t>(randomSpeed % 2 + 2));
    return record;
}

void CreateMapEffectKind1()
{
    MapEffectRecord* const record = CreateMapEffectKind1AtZero();
    if (record == nullptr) {
        return;
    }
    const std::uint32_t tick = platform::ReadTickCount32();
    ++gMapCharaSceneSlots.valueC8A742;
    gMapEffectTickC8A734 = tick;
    WriteMapEffectWord(
        *record, 4, 0u - ReadMapEffectWord(*record, 0x0C));
}

// 46E6C0/46E940 共用初始载荷；tick、计数和 Y 尾写由各自调用者按原序处理。
MapEffectRecord* CreateMapEffectKind2AtZero()
{
    MapEffectRecord* const record = Sub_46E340();
    if (record == nullptr) {
        return nullptr;
    }
    WriteMapEffectHalf(*record, 0x3C, 2);
    const std::int32_t randomX = platform::NextRandom_53B4C9();
    WriteMapEffectWord(*record, 4, 0);
    WriteMapEffectWord(
        *record, 0, static_cast<std::uint32_t>(randomX % 940 - 200));
    WriteMapEffectHalf(
        *record, 0x40,
        static_cast<std::uint16_t>(platform::NextRandom_53B4C9() % 2));
    const std::int32_t direction = platform::NextRandom_53B4C9() % 2;
    if (direction == 0) {
        WriteMapEffectWord(
            *record, 8,
            static_cast<std::uint32_t>(platform::NextRandom_53B4C9() % 2));
        WriteMapEffectWord(
            *record, 0x0C,
            static_cast<std::uint32_t>(platform::NextRandom_53B4C9() % 2 + 3));
    } else if (direction == 1) {
        WriteMapEffectWord(*record, 8, 1);
        WriteMapEffectWord(
            *record, 0x0C,
            static_cast<std::uint32_t>(platform::NextRandom_53B4C9() % 2 + 4));
    }
    return record;
}

void CreateMapEffectKind2()
{
    MapEffectRecord* const record = CreateMapEffectKind2AtZero();
    if (record == nullptr) {
        return;
    }
    const std::uint32_t tick = platform::ReadTickCount32();
    ++gMapCharaSceneSlots.valueC8A748;
    gMapEffectTickC82250 = tick;
    WriteMapEffectWord(
        *record, 4, 0u - ReadMapEffectWord(*record, 0x0C));
}

void CreateMapEffectKind4()
{
    MapEffectRecord* const record = Sub_46E340();
    if (record == nullptr) {
        return;
    }
    WriteMapEffectHalf(*record, 0x3C, 4);
    const std::int32_t randomX = platform::NextRandom_53B4C9();
    WriteMapEffectWord(*record, 4, 0);
    WriteMapEffectWord(
        *record, 0, static_cast<std::uint32_t>(randomX % 1240 - 100));
    WriteMapEffectHalf(
        *record, 0x40,
        static_cast<std::uint16_t>(platform::NextRandom_53B4C9() % 4));
    WriteMapEffectWord(*record, 0x30, 4);
    const std::int32_t direction = platform::NextRandom_53B4C9() % 2;
    if (direction == 0 || direction == 1) {
        WriteMapEffectWord(*record, 8, 0xFFFFFFFFu);
        WriteMapEffectWord(*record, 0x0C, static_cast<std::uint32_t>(direction + 2));
    }
    const std::uint32_t tick = platform::ReadTickCount32();
    ++gMapCharaSceneSlots.valueC8A750;
    gMapEffectTickC8A730 = tick;
    WriteMapEffectWord(
        *record, 4, 0u - ReadMapEffectWord(*record, 0x0C));
}

} // namespace

void Sub_46E480()
{
    if (gMapCharaSceneSlots.valueC8A73C != 0) {
        static_cast<void>(render::DrawMapEffects_453580());
        gMapCharaSceneSlots.valueC8A73C = 0;
    }
}

void Sub_46E4E0()
{
    const std::uint8_t started = gMapEffectStartedC8224C;
    if ((started & 1u) == 0) {
        gMapEffectStartedC8224C = static_cast<std::uint8_t>(started | 1u);
        gMapEffectTickC8A734 = platform::ReadTickCount32();
    }

    const std::uint32_t selected = gCurrentSceneType == 10
        ? gMapCharaSceneSlots.valueC8A768
        : gMapCharaSceneSlots.valueC8A73E;
    const std::int16_t selectedLow =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(selected));
    if (selectedLow != 0) {
        if (selectedLow >= 3) {
            const std::int32_t attempts = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(selected * 4u - 8u));
            for (std::int32_t index = 0; index < attempts; ++index) {
                CreateMapEffectKind1();
            }
        } else {
            const std::uint32_t now = platform::ReadTickCount32();
            const std::int16_t interval = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(51u - selected * 25u));
            const std::uint32_t deadline = gMapEffectTickC8A734
                + static_cast<std::uint32_t>(static_cast<std::int32_t>(interval));
            if (deadline < now) {
                CreateMapEffectKind1();
            }
        }
    }

    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        if (ReadMapEffectHalf(*record, 0x3C) == 1) {
            const std::uint32_t velocityX = ReadMapEffectWord(*record, 8);
            const std::uint32_t oldX = ReadMapEffectWord(*record, 0);
            const std::uint32_t velocityY = ReadMapEffectWord(*record, 0x0C);
            const std::uint32_t oldY = ReadMapEffectWord(*record, 4);
            const std::uint32_t x = oldX + velocityX;
            const std::uint32_t y = oldY + velocityY;
            WriteMapEffectWord(*record, 0, x);
            WriteMapEffectWord(*record, 4, y);
            ApplyMapEffectDirection(*record);
            const std::int32_t finalX =
                std::bit_cast<std::int32_t>(ReadMapEffectWord(*record, 0));
            if (finalX >= 640
                || std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 4)) >= 480) {
                MapEffectRecord* const next = record->next;
                Sub_46E390(record);
                --gMapCharaSceneSlots.valueC8A742;
                record = next;
                continue;
            }
        }
        record = record->next;
        gMapCharaSceneSlots.valueC8A73C = 1;
    }
}

void Sub_46E6C0()
{
    const std::uint8_t started = gMapEffectStartedC82240;
    if ((started & 1u) == 0) {
        gMapEffectStartedC82240 = static_cast<std::uint8_t>(started | 1u);
        gMapEffectTickC82250 = platform::ReadTickCount32();
    }

    const std::uint16_t selected = static_cast<std::uint16_t>(
        gCurrentSceneType == 10
            ? gMapCharaSceneSlots.valueC8A768 >> 16
            : gMapCharaSceneSlots.valueC8A744);
    const std::int16_t selectedLow = std::bit_cast<std::int16_t>(selected);
    if (selectedLow != 0) {
        if (selectedLow >= 3) {
            const std::int32_t attempts = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(2u * selected - 4u));
            for (std::int32_t index = 0; index < attempts; ++index) {
                CreateMapEffectKind2();
            }
        } else {
            const std::uint32_t now = platform::ReadTickCount32();
            const std::int16_t interval = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(51u - 25u * selected));
            const std::uint32_t deadline = gMapEffectTickC82250
                + static_cast<std::uint32_t>(static_cast<std::int32_t>(interval));
            if (deadline < now) {
                CreateMapEffectKind2();
            }
        }
    }

    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        if (ReadMapEffectHalf(*record, 0x3C) == 2) {
            const std::uint32_t velocityX = ReadMapEffectWord(*record, 8);
            const std::uint32_t oldX = ReadMapEffectWord(*record, 0);
            const std::uint32_t oldY = ReadMapEffectWord(*record, 4);
            WriteMapEffectWord(*record, 0, oldX + velocityX);
            const std::uint32_t velocityY = ReadMapEffectWord(*record, 0x0C);
            WriteMapEffectWord(*record, 4, oldY + velocityY);
            ApplyMapEffectDirection(*record);
            const std::int32_t finalX =
                std::bit_cast<std::int32_t>(ReadMapEffectWord(*record, 0));
            if (finalX >= 740
                || std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 4)) >= 480) {
                MapEffectRecord* const next = record->next;
                Sub_46E390(record);
                --gMapCharaSceneSlots.valueC8A748;
                record = next;
                continue;
            }
        }
        record = record->next;
        gMapCharaSceneSlots.valueC8A73C = 1;
    }
}

std::int32_t __cdecl Sub_46E940(std::int32_t cycles)
{
    const std::uint16_t selected = static_cast<std::uint16_t>(
        gCurrentSceneType == 10
            ? gMapCharaSceneSlots.valueC8A768 >> 16
            : gMapCharaSceneSlots.valueC8A744);
    if (cycles <= 0) {
        return cycles;
    }

    const std::int16_t selectedLow = std::bit_cast<std::int16_t>(selected);
    std::int32_t streak = 0;
    do {
        if (selectedLow != 0) {
            if (selectedLow >= 3) {
                const std::int32_t attempts = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(2u * selected - 4u));
                for (std::int32_t index = 0; index < attempts; ++index) {
                    if (CreateMapEffectKind2AtZero() != nullptr) {
                        streak = 0;
                        ++gMapCharaSceneSlots.valueC8A748;
                    }
                }
            } else {
                const std::int32_t threshold = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(2u * (2u - selected)));
                if (streak >= threshold
                    && CreateMapEffectKind2AtZero() != nullptr) {
                    streak = 0;
                    ++gMapCharaSceneSlots.valueC8A748;
                }
            }
        }

        MapEffectRecord* record = gActiveMapEffectHead;
        while (record != nullptr) {
            if (ReadMapEffectHalf(*record, 0x3C) == 2) {
                const std::uint32_t velocityX = ReadMapEffectWord(*record, 8);
                const std::uint32_t oldX = ReadMapEffectWord(*record, 0);
                const std::uint32_t velocityY = ReadMapEffectWord(*record, 0x0C);
                const std::uint32_t oldY = ReadMapEffectWord(*record, 4);
                WriteMapEffectWord(*record, 0, oldX + velocityX);
                WriteMapEffectWord(*record, 4, oldY + velocityY);
                ApplyMapEffectDirection(*record);
                const std::int32_t finalX =
                    std::bit_cast<std::int32_t>(ReadMapEffectWord(*record, 0));
                if (finalX >= 740
                    || std::bit_cast<std::int32_t>(
                        ReadMapEffectWord(*record, 4)) >= 480) {
                    MapEffectRecord* const next = record->next;
                    Sub_46E390(record);
                    --gMapCharaSceneSlots.valueC8A748;
                    record = next;
                    continue;
                }
            }
            record = record->next;
            gMapCharaSceneSlots.valueC8A73C = 1;
        }
        ++streak;
    } while (--cycles != 0);
    return cycles;
}

std::int32_t __cdecl Sub_46EBB0(std::int32_t cycles)
{
    const std::uint16_t selected = static_cast<std::uint16_t>(
        gCurrentSceneType == 10
            ? gMapCharaSceneSlots.valueC8A768
            : gMapCharaSceneSlots.valueC8A73E);
    if (cycles <= 0) {
        return cycles;
    }

    const std::int16_t selectedLow = std::bit_cast<std::int16_t>(selected);
    do {
        if (selectedLow != 0) {
            if (selectedLow >= 3) {
                const std::int32_t attempts = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(4u * selected - 8u));
                for (std::int32_t index = 0; index < attempts; ++index) {
                    if (CreateMapEffectKind1AtZero() != nullptr) {
                        ++gMapCharaSceneSlots.valueC8A742;
                    }
                }
            } else {
                const std::int16_t threshold = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(2u * (2u - selected)));
                if (threshold <= 0
                    && CreateMapEffectKind1AtZero() != nullptr) {
                    ++gMapCharaSceneSlots.valueC8A742;
                }
            }
        }

        MapEffectRecord* record = gActiveMapEffectHead;
        while (record != nullptr) {
            if (ReadMapEffectHalf(*record, 0x3C) == 1) {
                const std::uint32_t velocityX = ReadMapEffectWord(*record, 8);
                const std::uint32_t oldX = ReadMapEffectWord(*record, 0);
                const std::uint32_t velocityY = ReadMapEffectWord(*record, 0x0C);
                const std::uint32_t oldY = ReadMapEffectWord(*record, 4);
                WriteMapEffectWord(*record, 0, oldX + velocityX);
                WriteMapEffectWord(*record, 4, oldY + velocityY);
                ApplyMapEffectDirection(*record);
                const std::int32_t finalX =
                    std::bit_cast<std::int32_t>(ReadMapEffectWord(*record, 0));
                if (finalX >= 640
                    || std::bit_cast<std::int32_t>(
                        ReadMapEffectWord(*record, 4)) >= 480) {
                    MapEffectRecord* const next = record->next;
                    Sub_46E390(record);
                    --gMapCharaSceneSlots.valueC8A742;
                    record = next;
                    continue;
                }
            }
            record = record->next;
            gMapCharaSceneSlots.valueC8A73C = 1;
        }
    } while (--cycles != 0);
    return cycles;
}

std::int32_t __cdecl Sub_46E4C0(const std::int32_t cycles)
{
    static_cast<void>(Sub_46EBB0(cycles));
    return Sub_46E940(cycles);
}

void Sub_46ED50()
{
    const std::uint8_t started = gMapEffectStartedC8A728;
    if ((started & 1u) == 0) {
        gMapEffectStartedC8A728 = static_cast<std::uint8_t>(started | 1u);
        gMapEffectTickC8A730 = platform::ReadTickCount32();
    }

    const std::uint32_t selected = gCurrentSceneType == 10
        ? gMapCharaSceneSlots.valueC8A76C
        : gMapCharaSceneSlots.valueC8A74E;
    const std::int16_t selectedLow =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(selected));
    if (selectedLow != 0) {
        if (selectedLow >= 3) {
            const std::int32_t attempts = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(2u * selected - 4u));
            for (std::int32_t index = 0; index < attempts; ++index) {
                CreateMapEffectKind4();
            }
        } else {
            const std::uint32_t now = platform::ReadTickCount32();
            const std::int16_t interval = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(51u - 25u * selected));
            const std::uint32_t deadline = gMapEffectTickC8A730
                + static_cast<std::uint32_t>(static_cast<std::int32_t>(interval));
            if (deadline < now) {
                CreateMapEffectKind4();
            }
        }
    }

    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        if (ReadMapEffectHalf(*record, 0x3C) == 4) {
            const std::int32_t randomX = platform::NextRandom_53B4C9() % 3;
            const std::uint32_t oldX = ReadMapEffectWord(*record, 0);
            const std::uint32_t velocityX = ReadMapEffectWord(*record, 8);
            WriteMapEffectWord(
                *record, 0,
                oldX + velocityX + static_cast<std::uint32_t>(randomX - 1));

            const std::int32_t randomY = platform::NextRandom_53B4C9() % 3;
            const std::uint32_t velocityY = ReadMapEffectWord(*record, 0x0C);
            const std::uint32_t oldY = ReadMapEffectWord(*record, 4);
            const std::uint32_t remaining = ReadMapEffectWord(*record, 0x30);
            WriteMapEffectWord(
                *record, 4,
                oldY + velocityY + static_cast<std::uint32_t>(randomY - 1));
            if (std::bit_cast<std::int32_t>(remaining) <= 0) {
                WriteMapEffectHalf(
                    *record, 0x3E,
                    static_cast<std::uint16_t>(platform::NextRandom_53B4C9() % 3));
                WriteMapEffectWord(
                    *record, 0x28,
                    static_cast<std::uint32_t>(platform::NextRandom_53B4C9() % 8 + 1));
                WriteMapEffectWord(
                    *record, 0x2C,
                    static_cast<std::uint32_t>(platform::NextRandom_53B4C9() % 8 + 1));
                WriteMapEffectWord(*record, 0x30, 4);
            } else {
                WriteMapEffectWord(*record, 0x30, remaining - 1u);
            }
            ApplyMapEffectDirection(*record);
            if (std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 4)) >= 480) {
                MapEffectRecord* const next = record->next;
                Sub_46E390(record);
                --gMapCharaSceneSlots.valueC8A750;
                record = next;
                continue;
            }
        }
        record = record->next;
        gMapCharaSceneSlots.valueC8A73C = 1;
    }
}

void Sub_46E4A0()
{
    Sub_46E4E0();
    Sub_46E6C0();
    // 原 46E4AA/46E4AF 两次调用的 509080 仅有 retn。
    Sub_46ED50();
    Sub_46F1D0();
}

void __cdecl Sub_46E0D0(const std::int32_t mask, const std::int32_t value)
{
    if (mask == 0) {
        login::YobiAction* const action = gMapCharaSceneSlots.actionC8A764;
        gMapCharaSceneSlots.valueC8A752 = 0;
        gMapCharaSceneSlots.valueC8A754 = 0;
        gMapCharaSceneSlots.value55DEA2 = -1;
        ClearMapEffectAction(action);
        return;
    }

    if ((mask & 8) != 0) {
        gMapCharaSceneSlots.valueC8A752 =
            static_cast<std::uint16_t>(value % 10);
        gMapCharaSceneSlots.value55DEA0 =
            static_cast<std::int16_t>((value % 10000) / 10);
        gMapCharaSceneSlots.value55DE9C =
            static_cast<float>(value / 10000);
    }
    if ((mask & 0x10) != 0) {
        gMapCharaSceneSlots.valueC8A754 =
            static_cast<std::uint16_t>(value % 10);
        gMapCharaSceneSlots.value55DEA0 =
            static_cast<std::int16_t>((value % 10000) / 10);
        gMapCharaSceneSlots.value55DE9C =
            static_cast<float>(value / 10000);
    }
    if ((mask & 0x20) == 0) {
        return;
    }
    ClearMapEffectAction(gMapCharaSceneSlots.actionC8A764);
    if (value == -1) {
        gMapCharaSceneSlots.value55DEA2 = -1;
        return;
    }
    gMapCharaSceneSlots.value55DEA2 = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(60 * (value / 100000)));
    gMapCharaSceneSlots.valueC8A762 =
        static_cast<std::int16_t>((value / 10000 % 10) % 8);
    gMapCharaSceneSlots.valueC8A760 =
        static_cast<std::int16_t>(value % 10000);
}

void Sub_46F160()
{
    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        const std::uint16_t kind = ReadMapEffectHalf(*record, 0x3C);
        if (kind != 6 && kind != 7 && kind != 8) {
            record = record->next;
            continue;
        }
        if (kind == 8) {
            login::detail::MarkDeath(gMapCharaSceneSlots.actionC8A764);
            gMapCharaSceneSlots.actionC8A764 = nullptr;
        }
        MapEffectRecord* const next = record->next;
        Sub_46E390(record);
        --gMapCharaSceneSlots.valueC8A756;
        record = next;
    }
}

MapEffectRecord* Sub_46F090(
    const std::int16_t kind, const std::int32_t horizontalSign,
    const std::int32_t verticalSign, const std::int32_t phase,
    const std::int32_t speed, const std::int32_t graphicIndex)
{
    MapEffectRecord* const record = Sub_46E340();
    if (record == nullptr) {
        return record;
    }

    WriteMapEffectHalf(*record, 0x3C, static_cast<std::uint16_t>(kind));
    const std::int32_t offset = verticalSign != 0
        ? phase % (horizontalSign != 0 ? 1920 : 1040)
        : phase % 880 + 1040;

    std::int32_t x = -400;
    std::int32_t y = -400;
    WriteMapEffectWord(*record, 0x00, static_cast<std::uint32_t>(x));
    WriteMapEffectWord(*record, 0x04, static_cast<std::uint32_t>(y));
    if (offset >= 1040) {
        y = offset - 1440;
        WriteMapEffectWord(*record, 0x04, static_cast<std::uint32_t>(y));
    } else {
        x = offset - 400;
        WriteMapEffectWord(*record, 0x00, static_cast<std::uint32_t>(x));
    }
    if (horizontalSign < 0) {
        x = 240 - x;
        WriteMapEffectWord(*record, 0x00, static_cast<std::uint32_t>(x));
    }
    if (verticalSign < 0) {
        y = 80 - y;
        WriteMapEffectWord(*record, 0x04, static_cast<std::uint32_t>(y));
    }
    WriteMapEffectWord(*record, 0x08, static_cast<std::uint32_t>(x) << 16);
    WriteMapEffectWord(*record, 0x0C, static_cast<std::uint32_t>(y) << 16);
    WriteMapEffectWord(*record, 0x10, static_cast<std::uint32_t>(speed));
    WriteMapEffectWord(*record, 0x14, static_cast<std::uint32_t>(graphicIndex));
    ++gMapCharaSceneSlots.valueC8A756;
    return record;
}

std::int32_t __cdecl Sub_46F000(std::int32_t* const x, std::int32_t* const y)
{
    float projectedX;
    float projectedY;
    ProjectWorldToScreen_46CE30(0.0F, 0.0F, &projectedX, &projectedY);
    const std::int32_t currentX = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(static_cast<std::int64_t>(
            static_cast<double>(projectedX) + 0.5)));
    const std::int32_t currentY = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(static_cast<std::int64_t>(
            static_cast<double>(projectedY) + 0.5)));

    if (gProjectedX55DEA4 == -1) {
        gProjectedX55DEA4 = currentX;
        gProjectedY55DEA8 = currentY;
        return currentY;
    }

    const std::uint32_t deltaX =
        static_cast<std::uint32_t>(currentX)
        - static_cast<std::uint32_t>(gProjectedX55DEA4);
    *x = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(*x) + deltaX);
    const std::uint32_t deltaY =
        static_cast<std::uint32_t>(currentY)
        - static_cast<std::uint32_t>(gProjectedY55DEA8);
    *y = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(*y) + deltaY);
    gProjectedX55DEA4 = currentX;
    gProjectedY55DEA8 = currentY;
    return currentY;
}

void Sub_46F1D0()
{
    if (gCurrentSceneType == 10) {
        return;
    }

    double direction[2];
    platform::detail::Cg2dMapEffectDirection(
        gMapCharaSceneSlots.value55DEA0,
        &gMapCharaSceneSlots.value55DE9C,
        &gMapCharaSceneSlots.valueC8A758,
        &gMapCharaSceneSlots.valueC8A75C, direction);
    std::int32_t cameraX = 0;
    std::int32_t cameraY = 0;
    Sub_46F000(&cameraX, &cameraY);
    platform::detail::MapEffectMotion motion;
    platform::detail::Cg2dMapEffectMotion(
        direction, cameraX, cameraY, &motion);

    if (gMapCharaSceneSlots.value55DEA2 == 0) {
        const std::int32_t phase = gMapCharaSceneSlots.valueC8A760;
        Sub_46F090(8, motion.horizontalSign, motion.verticalSign,
            phase, 2000, phase % 3);
        gMapCharaSceneSlots.actionC8A764 =
            login::detail::NewYobiAction_404AE0(3, 0);
        if (gMapCharaSceneSlots.actionC8A764 != nullptr) {
            login::detail::SetYobiActionField2A8(
                gMapCharaSceneSlots.actionC8A764, 110308);
            login::detail::SetYobiActionField2B0(
                gMapCharaSceneSlots.actionC8A764, 3);
            login::detail::SetYobiActionField2B8(
                gMapCharaSceneSlots.actionC8A764,
                gMapCharaSceneSlots.valueC8A762 % 8);
            login::detail::ClearYobiActionCallback(
                gMapCharaSceneSlots.actionC8A764);
            login::detail::SetYobiActionField15(
                gMapCharaSceneSlots.actionC8A764, 26);
        }
    }
    if (gMapCharaSceneSlots.value55DEA2 >= 0) {
        --gMapCharaSceneSlots.value55DEA2;
    }

    const auto spawn = [&motion](
        const std::int16_t kind, const std::uint16_t level,
        const std::int32_t minimumSpeed) {
        if (level == 0) {
            return;
        }
        const float speed = gMapCharaSceneSlots.value55DE9C;
        if (!(speed > 0.0F || speed < 0.0F)) {
            return;
        }
        std::int32_t divisor = platform::detail::Cg2dMapEffectDivisor(
            static_cast<std::int32_t>(std::bit_cast<std::int16_t>(level)) - 1,
            &motion.magnitude);
        if (divisor == 0) {
            divisor = 1;
        }
        if (platform::NextRandom_53B4C9() % divisor != 0) {
            return;
        }
        const std::int32_t graphicIndex = platform::NextRandom_53B4C9() % 3;
        const std::int32_t recordSpeed =
            platform::NextRandom_53B4C9() % 1000 + minimumSpeed;
        const std::int32_t phase = platform::NextRandom_53B4C9();
        Sub_46F090(kind, motion.horizontalSign, motion.verticalSign,
            phase, recordSpeed, graphicIndex);
    };
    spawn(6, gMapCharaSceneSlots.valueC8A752, 1000);
    spawn(7, gMapCharaSceneSlots.valueC8A754, 500);

    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        const std::uint16_t kind = ReadMapEffectHalf(*record, 0x3C);
        if (kind == 6 || kind == 7 || kind == 8) {
            std::int64_t delta[2];
            platform::detail::Cg2dMapEffectStep(
                std::bit_cast<std::int32_t>(ReadMapEffectWord(*record, 0x10)),
                direction, delta);
            std::uint32_t fixedX =
                ReadMapEffectWord(*record, 0x08)
                + static_cast<std::uint32_t>(delta[0]);
            WriteMapEffectWord(*record, 0x08, fixedX);
            std::uint32_t fixedY =
                ReadMapEffectWord(*record, 0x0C)
                + static_cast<std::uint32_t>(delta[1]);
            WriteMapEffectWord(*record, 0x0C, fixedY);
            fixedX += static_cast<std::uint32_t>(cameraX) << 16;
            WriteMapEffectWord(*record, 0x08, fixedX);
            fixedY += static_cast<std::uint32_t>(cameraY) << 16;
            WriteMapEffectWord(*record, 0x0C, fixedY);
            const std::int32_t x = std::bit_cast<std::int32_t>(fixedX) >> 16;
            const std::int32_t y = std::bit_cast<std::int32_t>(fixedY) >> 16;
            WriteMapEffectWord(*record, 0x00, static_cast<std::uint32_t>(x));
            WriteMapEffectWord(*record, 0x04, static_cast<std::uint32_t>(y));
            if (x < -400 || x > 640 || y < -400 || y > 480) {
                if (ReadMapEffectHalf(*record, 0x3C) == 8) {
                    login::detail::MarkDeath(gMapCharaSceneSlots.actionC8A764);
                    gMapCharaSceneSlots.actionC8A764 = nullptr;
                }
                MapEffectRecord* const next = record->next;
                Sub_46E390(record);
                --gMapCharaSceneSlots.valueC8A756;
                record = next;
                continue;
            }

            const std::uint16_t drawKind = ReadMapEffectHalf(*record, 0x3C);
            if (drawKind == 8 && gMapCharaSceneSlots.actionC8A764 != nullptr) {
                login::detail::SetYobiActionField30(
                    gMapCharaSceneSlots.actionC8A764, x + 150);
                const auto actionY = std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 0x04));
                login::detail::SetYobiActionField34(
                    gMapCharaSceneSlots.actionC8A764, actionY + 150);
                login::detail::Sub_517880(
                    gMapCharaSceneSlots.actionC8A764, 100, 0, 0);
                login::YobiAction* const action = gMapCharaSceneSlots.actionC8A764;
                const std::int32_t imageNo = login::detail::GetYobiActionField20(action);
                const std::int32_t drawY = login::detail::GetYobiActionField34(action);
                const std::int32_t drawX = login::detail::GetYobiActionField30(action);
                login::detail::DrawGraphic(drawX, drawY, 0, imageNo, 0, nullptr);
            }
            if (drawKind == 7) {
                const auto index = std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 0x14));
                render::DrawGraphic3_520970(
                    x, y, 1, 14648, 0, 1, 0,
                    gEffectDrawModes55DEB0.data() + 3 + index);
            } else if (drawKind == 6 || drawKind == 8) {
                const auto index = std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 0x14));
                const auto speed = std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 0x10));
                const auto drawX = std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 0x00));
                const auto drawY = std::bit_cast<std::int32_t>(
                    ReadMapEffectWord(*record, 0x04));
                render::DrawGraphic3_520970(
                    drawX, drawY, static_cast<std::uint8_t>(speed / 200 + 15),
                    14648, 0, 1, 0, gEffectDrawModes55DEB0.data() + index);
            }
        }
        record = record->next;
        gMapCharaSceneSlots.valueC8A73C = 1;
    }
}

std::int32_t Sub_440FF0()
{
    const std::int32_t direction = login::detail::ReadMapObservationDirection();
    constexpr std::array<std::int32_t, 8> angles {315, 270, 225, 180, 135, 90, 45, 0};
    const auto angle = static_cast<std::int16_t>(angles[direction]);
    const render::GraphicDescriptor16 descriptor {{
        0xC0, 0xFE, 0x54, 0x01, 0x40, 0x00, 0x30, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00,
    }};
    gMapCharaSceneSlots.valueC8A752 = 2;
    gMapCharaSceneSlots.valueC8A754 = 2;
    gMapCharaSceneSlots.value55DE9C = 6.0F;
    gMapCharaSceneSlots.value55DEA0 = angle;
    Sub_46F1D0();

    std::int32_t result = 0;
    for (std::int32_t y = 0; y < 480; y += 48) {
        for (std::int32_t x = 0; x < 640; x += 64) {
            result = render::DrawGraphic3_520970(
                x, y, 0, 14648, 0, 1, 0, &descriptor);
        }
    }
    return result;
}

void Sub_476680()
{
    login::YobiAction* const action = gPlayerRuntime.action;
    if (action == nullptr) {
        return;
    }

    std::int32_t radius = platform::gValue554640;
    std::int32_t x = login::detail::GetYobiActionField30(action) - radius * 64;
    std::int32_t y = login::detail::GetYobiActionField34(action);
    const auto countdown = std::bit_cast<std::int32_t>(gValue626BFC);
    x -= 31;
    y -= 23;
    if (countdown > 0) {
        std::int32_t sideCount = radius * 2 + 1;
        if (sideCount > 0) {
            std::int32_t rowX = x;
            std::int32_t rowY = y;
            std::int32_t row = 0;
            do {
                std::int32_t column = 0;
                if (sideCount > 0) {
                    do {
                        static_cast<void>(render::Sub_521C80(x, y, 100, 3, 0, nullptr));
                        radius = platform::gValue554640;
                        ++column;
                        x += 32;
                        sideCount = radius * 2 + 1;
                        y -= 24;
                    } while (column < sideCount);
                }
                ++row;
                sideCount = radius * 2 + 1;
                x = rowX + 32;
                y = rowY + 24;
                rowX = x;
                rowY = y;
            } while (row < sideCount);
        }
    }

    // 原尾部先发布递减值，再按符号位决定是否写零。
    const std::uint32_t remaining = gValue626BFC - 1u;
    gValue626BFC = remaining;
    if (std::bit_cast<std::int32_t>(remaining) < 0) {
        gValue626BFC = 0;
    }
}

void Sub_50A3E0(
    const std::uintptr_t /*socket*/, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    const std::uint32_t mask = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    auto& slot = gPetMapCharaSlots[static_cast<std::size_t>(index)];
    if (mask == 0) {
        if (slot.flags != 0 && slot.objectIndex != gPlayerRuntime.objectIndex) {
            login::YobiAction* const action =
                FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(slot.objectIndex));
            if (action != nullptr) {
                Sub_442C70(action);
            }
        }
        std::int32_t x = -1;
        std::int32_t y = -1;
        if (slot.action != nullptr) {
            login::detail::ReadMapActorTarget(slot.action, &x, &y);
        }
        slot.flags = 0;
        slot.action = nullptr;
        std::int32_t count = 0;
        std::int32_t next = -1;
        for (std::int32_t i = 0; i < 5; ++i) {
            if (gPetMapCharaSlots[static_cast<std::size_t>(i)].flags != 0) {
                ++count;
                if (next == -1 && i > index) {
                    next = i;
                }
            }
        }
        if (count <= 1) {
            gValueFF0D36 = 0;
            static_cast<void>(Sub_51B080());
            return;
        }
        if (next >= 0 && (x >= 0 || y >= 0)) {
            Sub_46AFE0(next, x, y);
        }
        return;
    }

    gValueFF0D32 = 0;
    gValueFF0D36 = 1;
    slot.flags = 1;
    std::int32_t field = 2;
    if ((mask & 3u) != 0) {
        slot.objectIndex = static_cast<std::uint32_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field));
        ++field;
    }
    if ((mask & 5u) != 0) {
        slot.level08 = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((mask & 9u) != 0) {
        slot.value10 = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((mask & 0x11u) != 0) {
        slot.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((mask & 0x21u) != 0) {
        slot.value14 = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((mask & 0x41u) != 0) {
        char name[256];
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        std::strcpy(
            slot.name18.data(), std::strlen(name) <= 16 ? name : kUnknownText554590);
    }
    if (slot.objectIndex != gPlayerRuntime.objectIndex) {
        login::YobiAction* const action =
            FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(slot.objectIndex));
        if (action != nullptr) {
            slot.action = action;
            Sub_442C40(action);
            if (index == 0) {
                Sub_442BD0(action);
            }
        } else {
            slot.action = nullptr;
        }
    } else {
        slot.action = gPlayerRuntime.action;
        Sub_51A5E0();
        if (index == 0) {
            Sub_51A5A0();
        }
    }
}

void Sub_50B380(const std::uintptr_t /*socket*/, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (ReadRippleGateOnce() != 0) {
        return;
    }

    char row[2048];
    char field[2048];
    char name[256];
    char titles[256];
    char extraText[256];
    char label[1024];
    for (std::int32_t rowIndex = 1;; ++rowIndex) {
        const std::int32_t status = network::ExtractDelimitedField_52CED0(
            payload, '\t', rowIndex, 2047, row);
        if (std::strlen(row) == 0 && status == 1) {
            return;
        }
        network::ExtractDelimitedField_52CED0(row, '|', 11, 2047, field);
        if (std::strlen(field) != 0) {
            const std::int32_t type =
                network::ParseDelimitedInteger_52CFA0(row, '|', 1);
            network::ExtractDelimitedField_52CED0(row, '|', 2, 2047, field);
            const std::int32_t objectIndex = network::Base62Decode_52D050(field);
            network::ExtractDelimitedField_52CED0(row, '|', 3, 2047, field);
            const std::int32_t x = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 4, 2047, field);
            const std::int32_t y = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 5, 2047, field);
            const std::int32_t direction = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 6, 2047, field);
            const std::int32_t imageNo = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 7, 2047, field);
            const std::int32_t level = std::atoi(field);
            const std::int32_t value40 =
                network::ParseDelimitedInteger_52CFA0(row, '|', 8);
            network::ExtractDelimitedField_52CED0(row, '|', 9, 255, name);
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            network::ExtractDelimitedField_52CED0(row, '|', 10, 255, titles);
            input::UnescapeLegacyText_52D150(titles);
            network::ExtractDelimitedField_52CED0(row, '|', 11, 2047, field);
            const std::int32_t flag1 = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 12, 2047, field);
            const std::int32_t flag2 = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 13, 2047, field);
            static_cast<void>(std::atoi(field));
            network::ExtractDelimitedField_52CED0(row, '|', 14, 255, extraText);
            input::UnescapeLegacyTextAndNotify_52D2F0(extraText);
            network::ExtractDelimitedField_52CED0(row, '|', 15, 2047, field);
            const std::int32_t mode = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 16, 2047, field);
            const std::int32_t attachmentImage = std::atoi(field);

            login::YobiAction* object;
            if (static_cast<std::uint32_t>(objectIndex) == gPlayerRuntime.objectIndex) {
                const std::int32_t mappedImage =
                    character_select::detail::Sub_51B740(imageNo);
                if (gPlayerRuntime.direction < 0) {
                    gPlayerRuntime.direction = direction;
                }
                if (gPlayerRuntime.action == nullptr) {
                    Sub_519CD0(mappedImage, x, y, direction);
                    Sub_51A4B0();
                } else {
                    Sub_519E50(mappedImage, gPlayerRuntime.direction);
                }
                object = gPlayerRuntime.action;
                login::detail::UpdateMapActorMode(
                    gPlayerRuntime.action, mode, attachmentImage);
                if (mode == 2) {
                    if ((gPlayerRuntime.flags18A & 0x200u) == 0) {
                        gPlayerRuntime.flags18A |= 0x100u;
                    }
                } else if (static_cast<std::uint8_t>(gPlayerRuntime.flags18C) == 0) {
                    gPlayerRuntime.flags18A &= 0xFEFFu;
                }
                gValue9A4B94 &= 0xFDu;
                static_cast<void>(Sub_468200());
                Sub_51A090(name, titles, level, value40, flag1, flag2);
                if ((gPlayerRuntime.flags18A & 0x100u) != 0
                    && gPetMapCharaSlots[0].flags != 0) {
                    gPetMapCharaSlots[0].level08 = gPlayerRuntime.level;
                    std::strcpy(
                        gPetMapCharaSlots[0].name18.data(), gPlayerRuntime.name.data());
                }
                for (std::size_t i = 0; i < gPetMapCharaSlots.size(); ++i) {
                    auto& slot = gPetMapCharaSlots[i];
                    if (slot.flags != 0
                        && slot.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
                        slot.action = gPlayerRuntime.action;
                        Sub_51A5E0();
                        if (i == 0) {
                            Sub_51A5A0();
                        }
                        break;
                    }
                }
            } else {
                const std::int32_t mappedImage =
                    character_select::detail::Sub_51B740(imageNo);
                Sub_445350(
                    objectIndex, mappedImage, x, y, direction, name, titles,
                    extraText, level, value40, flag1, flag2, type);
                object = FindObjectYobi_4462B0(objectIndex);
                if (object != nullptr) {
                    login::detail::UpdateMapActorMode(object, mode, attachmentImage);
                    if (mode == 2) {
                        Sub_449C90(object);
                    } else {
                        Sub_449CC0(object);
                    }
                    for (std::size_t i = 0; i < gPetMapCharaSlots.size(); ++i) {
                        auto& slot = gPetMapCharaSlots[i];
                        if (slot.flags != 0
                            && slot.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
                            slot.action = object;
                            Sub_442C40(object);
                            if (i == 0) {
                                Sub_442BD0(object);
                            }
                            break;
                        }
                    }
                }
            }
            login::detail::ApplyMapActorReplyAttachments(object, row);
            continue;
        }

        network::ExtractDelimitedField_52CED0(row, '|', 6, 2047, field);
        if (std::strlen(field) != 0) {
            network::ExtractDelimitedField_52CED0(row, '|', 1, 2047, field);
            const std::int32_t objectIndex = network::Base62Decode_52D050(field);
            network::ExtractDelimitedField_52CED0(row, '|', 2, 2047, field);
            const std::int32_t x = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 3, 2047, field);
            const std::int32_t y = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 4, 2047, field);
            const std::int32_t imageNo = std::atoi(field);
            const std::int32_t value48 =
                network::ParseDelimitedInteger_52CFA0(row, '|', 5);
            network::ExtractDelimitedField_52CED0(row, '|', 6, 1023, label);
            input::UnescapeLegacyTextAndNotify_52D2F0(label);
            const std::int32_t value50 =
                network::ParseDelimitedInteger_52CFA0(row, '|', 7);
            Sub_445D20(objectIndex, imageNo, x, y, 0, value48, label, value50);
            continue;
        }

        network::ExtractDelimitedField_52CED0(row, '|', 4, 2047, field);
        if (std::strlen(field) != 0) {
            network::ExtractDelimitedField_52CED0(row, '|', 1, 2047, field);
            const std::int32_t objectIndex = network::Base62Decode_52D050(field);
            network::ExtractDelimitedField_52CED0(row, '|', 2, 2047, field);
            const std::int32_t x = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 3, 2047, field);
            const std::int32_t y = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 4, 2047, field);
            const std::int32_t count = std::atoi(field);
            std::sprintf(label, "%d \xAA\xF7\xB9\xF4", count);
            std::int32_t imageNo;
            if (count < 50) {
                imageNo = 0x6B08;
            } else if (count < 100) {
                imageNo = 0x6B09;
            } else if (count < 1000) {
                imageNo = 0x6B0A;
            } else if (count < 5000) {
                imageNo = 0x6B0B;
            } else if (count < 10000) {
                imageNo = 0x6B0C;
            } else if (count < 50000) {
                imageNo = 0x6B0D;
            } else if (count < 100000) {
                imageNo = 0x6B0E;
            } else if (count < 500000) {
                imageNo = 0x6B0F;
            } else if (count < 1000000) {
                imageNo = 0x6B10;
            } else {
                imageNo = 0x6B11;
            }
            Sub_445F40(objectIndex, imageNo, x, y, 0, count, label);
        }
    }
}

void Sub_50BCD0(const std::uintptr_t /*socket*/, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (ReadRippleGateOnce() != 0) {
        return;
    }
    char row[2048];
    char field[2048];
    // 原0x1000字节局部区，比较门槛仍为0x1000项；不改容量或模拟邻栈。
    std::array<std::int32_t, 1024> missing;
    std::int32_t missingCount = 0;
    std::int32_t* missingCursor = missing.data();
    for (std::int32_t rowIndex = 1;; ++rowIndex) {
        network::ExtractDelimitedField_52CED0(payload, ',', rowIndex, 2047, row);
        if (std::strlen(row) == 0) {
            return;
        }
        network::ExtractDelimitedField_52CED0(row, '|', 1, 2047, field);
        const std::int32_t objectIndex = network::Base62Decode_52D050(field);
        const std::int32_t x = network::ParseDelimitedInteger_52CFA0(row, '|', 2);
        const std::int32_t y = network::ParseDelimitedInteger_52CFA0(row, '|', 3);
        const std::int32_t characterAction = network::ParseDelimitedInteger_52CFA0(row, '|', 4);
        const std::int32_t direction = network::ParseDelimitedInteger_52CFA0(row, '|', 5);
        if (login::detail::Sub_440920(objectIndex, x, y, direction, characterAction, row) != 0) {
            continue;
        }
        const std::int32_t option1 = network::ParseDelimitedInteger_52CFA0(row, '|', 6);
        const std::int32_t option2 = network::ParseDelimitedInteger_52CFA0(row, '|', 7);
        const std::int32_t option3 = network::ParseDelimitedInteger_52CFA0(row, '|', 8);
        const std::int32_t option4 = network::ParseDelimitedInteger_52CFA0(row, '|', 9);
        if (gPlayerRuntime.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
            login::YobiAction* const player = gPlayerRuntime.action;
            if (player != nullptr && login::detail::GetYobiActionField2A8(player) != 0) {
                Sub_51A840(x, y, direction, characterAction, option1, option2, option3, option4);
            }
            continue;
        }
        login::YobiAction* const action = FindObjectYobi_4462B0(objectIndex);
        if (action != nullptr) {
            Sub_444410(action, x, y, direction, characterAction, option1, option2, option3, option4);
            continue;
        }
        std::int32_t index = 0;
        for (; index < missingCount; ++index) {
            if (missing[index] == objectIndex) {
                break;
            }
        }
        if (index == missingCount && static_cast<std::uint32_t>(missingCount) < 0x1000u) {
            ++missingCount;
            *missingCursor = objectIndex;
            ++missingCursor;
        }
    }
}

namespace {

// 原六个入口共享473060发送尾；选项修改后重新读取，不按发送结果回滚。
std::int32_t TogglePlayerOptionAndSend(const std::uint16_t mask)
{
    const auto options = static_cast<std::uint8_t>(gPlayerRuntime.options194);
    if ((options & mask) != 0) {
        gPlayerRuntime.options194 &= static_cast<std::uint16_t>(~mask);
    } else {
        gPlayerRuntime.options194 |= mask;
    }
    const std::int32_t value = gPlayerRuntime.options194;
    const std::uintptr_t socket = platform::gConnectionSocket;
    return network::Sub_511A00(socket, value);
}

} // namespace

std::int32_t Sub_472F40()
{
    return TogglePlayerOptionAndSend(0x04);
}

std::int32_t Sub_472F70()
{
    return TogglePlayerOptionAndSend(0x08);
}

std::int32_t Sub_472FA0()
{
    return TogglePlayerOptionAndSend(0x01);
}

std::int32_t Sub_472FD0()
{
    return TogglePlayerOptionAndSend(0x10);
}

std::int32_t Sub_473000()
{
    return TogglePlayerOptionAndSend(0x20);
}

std::int32_t Sub_473030()
{
    return TogglePlayerOptionAndSend(0x40);
}

std::uint32_t Sub_50BFA0(
    const std::uintptr_t /*socket*/, const std::int32_t value)
{
    const std::uint32_t gate = gValue9A6E0C;
    if (gate != 0) {
        return gate;
    }
    const std::uint32_t options = static_cast<std::uint32_t>(value) & 0xFFFDu;
    gPlayerRuntime.options194 = static_cast<std::uint16_t>(options);
    return options;
}

std::uint16_t Sub_50BFC0(
    const std::uintptr_t /*socket*/, const std::int32_t value)
{
    const auto low = static_cast<std::uint16_t>(value);
    gPlayerRuntime.value1A0 = low;
    return low;
}

std::int32_t Sub_50CF70(
    const std::uintptr_t /*socket*/, const std::int32_t index,
    const std::int32_t value)
{
    gPetRuntimeF633F8[index].lvFlag6C = value;
    return index;
}

std::uint32_t Sub_50C7A0(
    const std::uintptr_t /*socket*/, const std::int32_t seconds,
    const std::int32_t mode)
{
    const std::uint32_t gate = gValue9A6E0C;
    if (gate != 0) {
        return gate;
    }
    const std::int32_t duration = seconds < 0 ? 0 : seconds;
    gPlayerRuntime.timerMode10 = mode;
    gPlayerRuntime.timerRemaining0C = duration * 1000;
    const std::uint32_t tick = platform::ReadTickCount32();
    gPlayerRuntime.timerTick14 = tick;
    input::gValue626C14 = 0;
    return tick;
}

std::int32_t Sub_50CE10(const std::uintptr_t /*socket*/, const char* const text)
{
    char source[2024];
    char command[512];
    std::strcpy(source, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(source);
    if (std::strncmp(source, "@FIND_LV1", 9) == 0) {
        std::int32_t sound = 0;
        std::strncpy(command, source, sizeof(command));
        std::sscanf(command, "@FIND_LV1@SE:%d", &sound);
        const char* const message = std::strstr(command, "@MSG:");
        std::strncpy(source, message + 5, sizeof(source));
        static_cast<void>(audio::PlaySe_52C450(sound, 320));
    }
    Sub_44B050(source, 4, 3);
    const std::int32_t soundEnabled = platform::gValue554644;
    if (soundEnabled != 0) {
        return audio::PlaySe_52C450(74, 320);
    }
    return soundEnabled;
}

void Sub_50E1A0(
    const std::uintptr_t /*socket*/, const std::int32_t mask,
    const std::int32_t value, const char* const /*unusedText*/)
{
    if (mask == 0) {
        const std::uint16_t zero = 0;
        gMapCharaSceneSlots.valueC8A73E = 0;
        std::memcpy(&gMapCharaSceneSlots.valueC8A744, &zero, sizeof(zero));
        gMapCharaSceneSlots.valueC8A74E = 0;
    } else {
        const auto low = static_cast<std::uint16_t>(value);
        if ((mask & 1) != 0) {
            gMapCharaSceneSlots.valueC8A73E = low;
        }
        if ((mask & 2) != 0) {
            std::memcpy(&gMapCharaSceneSlots.valueC8A744, &low, sizeof(low));
        }
        if ((mask & 4) != 0) {
            gMapCharaSceneSlots.valueC8A74E = low;
        }
    }
    Sub_46E0D0(mask, value);
}

void Sub_50E650(
    const std::uintptr_t /*socket*/, const std::int32_t mode,
    const std::int32_t duration, const std::int32_t floor,
    const std::int32_t mapX, const std::int32_t mapY)
{
    login::detail::Sub_449700(mode, duration, floor, mapX, mapY);
}

std::int32_t Sub_50E210(
    const std::uintptr_t /*socket*/, const std::int32_t value)
{
    const std::int32_t gate = std::bit_cast<std::int32_t>(gValue9A6E0C);
    if (gate != 0) {
        return gate;
    }
    std::int32_t sound;
    switch (value) {
    case 0: sound = 0x47; break;
    case 1: sound = 0x48; break;
    case 2:
    case 4: sound = 0x44; break;
    case 3: sound = 0x45; break;
    case 5: sound = 0x41; break;
    case 6: sound = 0x42; break;
    case 7: sound = 0x43; break;
    case 8: sound = 0x49; break;
    default: return value;
    }
    return audio::PlaySe_52C450(sound, 320);
}

std::int32_t Sub_50C940(
    const input::InputContext* const context, const std::int32_t color)
{
    const std::int32_t sendColor = color == 4 ? 5 : color;
    const std::int32_t gate = gChatGateDA3B54;
    const std::uint8_t disabled = gChatDisabledDA3B4C;
    const bool restricted = disabled != 0
        || (gate == 0 && gValueFF0D36 != 0
            && (static_cast<std::uint8_t>(gPlayerRuntime.options194) & 8u) != 0);
    if (restricted) {
        const std::uint32_t tick = platform::ReadTickCount32();
        if (tick - gChatCurrentTickDA3B40 > 180000u) {
            gChatDisabledDA3B4C = 0;
        } else if (gChatGateDA3B54 != 0 || gValueFF0D36 == 0
                   || (static_cast<std::uint8_t>(gPlayerRuntime.options194) & 8u) == 0) {
            Sub_44B050(kChatDisabledText58AFEC, 4, 3);
            const std::int32_t sound = platform::gValue554644;
            if (sound != 0) {
                return audio::PlaySe_52C450(74, 320);
            }
            return sound;
        }
    } else {
        gChatPreviousTickDA3B3C = gChatCurrentTickDA3B40;
        const std::uint32_t tick = platform::ReadTickCount32();
        const std::uint32_t previous = gChatPreviousTickDA3B3C;
        gChatCurrentTickDA3B40 = tick;
        if (tick - previous < 1000u) {
            Sub_44B050(kChatRepeatedText58B000, 4, 3);
            std::int32_t result = platform::gValue554644;
            if (result != 0) {
                result = audio::PlaySe_52C450(74, 320);
            }
            gChatDisabledDA3B4C = 1;
            return result;
        }
    }
    char escaped[1024];
    char packet[1024];
    input::EscapeInputText_52D2D0(context, escaped, 1024);
    std::sprintf(packet, "P|%s", escaped);
    const std::int32_t mode = platform::gValue73ECBC;
    const std::int32_t range = platform::gValue554640;
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    const std::uintptr_t socket = platform::gConnectionSocket;
    return network::Sub_511900(socket, x, y, packet, sendColor, range, mode);
}

void MarkPlayerChatActive() noexcept
{
    gPlayerRuntime.flags18A |= 0x2000u;
}

void Sub_50CAE0(
    const std::uintptr_t /*socket*/, const std::int32_t objectIndex,
    const char* const payload, const std::int32_t color, const std::int32_t font)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char kind[2];
    network::ExtractDelimitedField_52CED0(payload, '|', 1, 1, kind);
    if (kind[0] != 'P') {
        return;
    }
    char source[2024];
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 2023, source);
    input::UnescapeLegacyTextAndNotify_52D2F0(source);
    const auto length = static_cast<std::int32_t>(std::strlen(source));
    char snapshot[2048];
    if (length > 0) {
        std::memcpy(snapshot, source, static_cast<std::size_t>(length));
    }
    std::int32_t drawColor = color;
    std::int32_t drawFont = font;
    if (input::gValue626C14 == 0) {
        char name[64];
        char stripped[32];
        std::memset(name, 0, sizeof(name));
        const std::size_t initialLength = std::strlen(source);
        gChatGateDA3B54 = 0;
        if (initialLength != 0) {
            std::uint32_t cursor = 0;
            for (;;) {
                if (source[cursor] == ':' && source[cursor + 1] == ' ') {
                    std::strncpy(name, source, cursor);
                    name[cursor] = '\0';
                    break;
                }
                ++cursor;
                if (cursor >= std::strlen(source)) {
                    break;
                }
            }
        }
        const auto stripPrefix = [&](const std::uint32_t prefixLength) {
            std::uint32_t cursor = 0;
            if (std::strlen(name) != prefixLength) {
                do {
                    stripped[cursor] = name[cursor + prefixLength];
                    ++cursor;
                } while (cursor < std::strlen(name) - prefixLength);
            }
            stripped[cursor] = '\0';
            std::memset(name, 0, sizeof(name));
            std::strcpy(name, stripped);
        };
        if (std::strncmp(name, "[MIC]", std::strlen("[MIC]")) == 0) {
            gChatGateDA3B54 = 1;
            stripPrefix(5);
        } else if (std::strncmp(name, "[GP]", std::strlen("[GP]")) == 0) {
            stripPrefix(4);
        }
        if (blacklist::ContainsBlacklistRecord_4013D0(name) != 0) {
            return;
        }
        if (color < 0) {
            drawColor = 0;
            drawFont = 1;
        }
        if (std::strcmp(source, input::LegacyText593850()) != 0) {
            Sub_44B050(source, drawColor, drawFont);
        }
    }
    if (platform::gValue554644 != 0) {
        static_cast<void>(audio::PlaySe_52C450(74, 320));
    }
    if (objectIndex < 0) {
        return;
    }
    if (gPlayerRuntime.action != nullptr
        && gPlayerRuntime.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
        Sub_51A6A0(1000, snapshot, drawColor, drawFont, length);
    } else {
        login::YobiAction* const action = FindObjectYobi_4462B0(objectIndex);
        if (action != nullptr) {
            Sub_442DC0(action, 1000, snapshot, drawColor, drawFont, length);
        }
    }
}

void Sub_50D000(
    const std::uintptr_t /*socket*/, const std::int32_t first, const std::int32_t second)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (first == 1) {
        if (second == 1) {
            Sub_51A5E0();
        }
    } else if (first == 0 && second == 1) {
        Sub_51A5F0();
        gValueFF0D36 = 0;
        static_cast<void>(Sub_51B080());
        const std::int32_t direction = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(gPlayerRuntime.direction));
        const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotY);
        const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotX);
        static_cast<void>(
            network::Sub_512720(platform::gConnectionSocket, x, y, direction));
    }
    gValueFF0D32 = 0;
}

std::int32_t Sub_509650(
    const std::uintptr_t /*socket*/, const char* const payload)
{
    const std::int32_t gate = std::bit_cast<std::int32_t>(gValue9A6E0C);
    if (gate != 0) {
        return gate;
    }
    const auto flags = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    std::int32_t field = 2;
    const auto parseCrypt = [&](platform::LegacyCryptInt* const target,
                                const std::uint32_t mask) {
        if ((flags & mask) != 0) {
            const std::int32_t parsed =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            platform::LegacyCryptInt temporary;
            platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, parsed);
            target->mask = temporary.mask;
            target->encoded = temporary.encoded;
            target->readCount = temporary.readCount;
            platform::legacy_crypt_int::Destroy_401CF0(&temporary);
            ++field;
        }
    };
    parseCrypt(&gPlayerRuntime.hp, 0x3u);
    parseCrypt(&gPlayerRuntime.maxHp, 0x5u);
    parseCrypt(&gPlayerRuntime.fp, 0x9u);
    parseCrypt(&gPlayerRuntime.maxFp, 0x11u);

    std::uint32_t bit = 0x20u;
    for (std::int32_t index = 0; index < 8; ++index) {
        if ((flags & (bit | 1u)) != 0) {
            gPlayerRuntime.values78[index] =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
    parseCrypt(&gPlayerRuntime.value58, 0x2001u);
    parseCrypt(&gPlayerRuntime.value68, 0x4001u);

    const std::array<std::int32_t*, 12> targets {{
        &gPlayerRuntime.level,
        &gPlayerRuntime.values78[8], &gPlayerRuntime.values78[9],
        &gPlayerRuntime.values78[10], &gPlayerRuntime.values78[11],
        &gPlayerRuntime.values78[12], &gPlayerRuntime.values78[13],
        &gPlayerRuntime.valueB8,
        &gPlayerRuntime.valuesE4[0], &gPlayerRuntime.valuesE4[1],
        &gPlayerRuntime.valuesE4[2], &gPlayerRuntime.valuesE4[3],
    }};
    bit = 0x8000u;
    for (std::int32_t* const target : targets) {
        if ((flags & (bit | 1u)) != 0) {
            *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
    const std::uint32_t tailGate = flags & 0x8000001u;
    if (tailGate != 0) {
        gPlayerRuntime.valueFC =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((flags & 0x10000001u) != 0) {
        gPlayerRuntime.titleIndex100 =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((flags & 0x20000001u) != 0) {
        char name[256];
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) <= 16) {
            std::strcpy(gPlayerRuntime.name.data(), name);
        }
        ++field;
    }
    if ((flags & 0x40000001u) != 0) {
        char title[256];
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, title);
        input::UnescapeLegacyTextAndNotify_52D2F0(title);
        if (std::strlen(title) <= 16) {
            std::strcpy(gPlayerRuntime.ownTitle.data(), title);
        }
        ++field;
    }
    if (tailGate != 0) {
        const std::int32_t result =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        gPlayerRuntime.value15BCC = result;
        return result;
    }
    return 0;
}

void Sub_509BA0(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    const auto flags = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    std::int32_t field = 2;
    const std::array<std::int32_t*, 12> firstTargets {{
        &gPlayerRuntime.valuesD4[0], &gPlayerRuntime.valuesD4[1],
        &gPlayerRuntime.valuesD4[2], &gPlayerRuntime.valuesD4[3],
        &gPlayerRuntime.valuesBC[0], &gPlayerRuntime.valuesBC[1],
        &gPlayerRuntime.valuesBC[2], &gPlayerRuntime.valuesBC[3],
        &gPlayerRuntime.valuesBC[4], &gPlayerRuntime.valuesBC[5],
        &gPlayerRuntime.value104, &gPlayerRuntime.valueF4,
    }};
    std::uint32_t bit = 2;
    for (std::int32_t* const target : firstTargets) {
        if ((flags & (bit | 1u)) != 0) {
            *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
    if ((flags & 0x2001u) != 0) {
        gPlayerRuntime.value00 = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field));
        ++field;
    }
    const std::array<std::int32_t*, 4> middleTargets {{
        &gPlayerRuntime.valueF8, &gPlayerRuntime.value08,
        &gPlayerRuntime.value17C, &gPlayerRuntime.value16C,
    }};
    bit = 0x4000u;
    for (std::int32_t* const target : middleTargets) {
        if ((flags & (bit | 1u)) != 0) {
            *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
    if ((flags & 0x40001u) != 0) {
        std::int32_t speed = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        gPlayerRuntime.walkSpeed = speed;
        if (speed <= 0) {
            speed = 100;
            gPlayerRuntime.walkSpeed = speed;
        }
        login::YobiAction* const action = gPlayerRuntime.action;
        if (action != nullptr) {
            login::detail::SetYobiWalkSpeed(action, speed);
        }
        ++field;
    }
    const std::array<std::int32_t*, 5> lastTargets {{
        &gPlayerRuntime.valuesB0[0], &gPlayerRuntime.valuesB0[1],
        &gPlayerRuntime.values15BD0[0], &gPlayerRuntime.values15BD0[1],
        &gPlayerRuntime.values15BD0[2],
    }};
    bit = 0x80000u;
    for (std::int32_t* const target : lastTargets) {
        if ((flags & (bit | 1u)) != 0) {
            *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
}

void Sub_509E30(const std::uintptr_t, const std::int32_t value, char* const text)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    gSkillRuntimeSlots.valueF633E8 =
        (gSkillRuntimeSlots.valueF633E8 & 0xFFFFFF00u) | static_cast<std::uint8_t>(value);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(
        gSkillRuntimeSlots.jobName.data(), std::strlen(text) < 24 ? text : kUnknownText554590);
}

// 技能/索引表借用唯一第四清零块；原零初值及拖放门禁不新增构造/RNG消费。
std::array<SkillRuntimeRecord, 15>& gSkillsF1BAFC = gSkillRuntimeSlots.skills;
std::array<SkillPairRecord, 15>& gSkillPairsF63370 = gSkillRuntimeSlots.pairs;
std::uint16_t gValueDA3B20 = 0;

std::int32_t Sub_504F10(const std::int32_t value, const std::int32_t mode)
{
    const char* const storage = reinterpret_cast<const char*>(&gSkillRuntimeSlots);
    // 原包含末次F633A0读取，别名为pairs[6]前四字节，不是第16条技能。
    for (std::int32_t index = 0; index <= 15; ++index) {
        const char* const record =
            storage + offsetof(SkillRuntimeSlots, skills) + index * sizeof(SkillRuntimeRecord);
        std::int32_t skillId;
        std::memcpy(&skillId, record + offsetof(SkillRuntimeRecord, skillId), sizeof(skillId));
        const bool matches = mode == 1
            ? skillId >= 601 && skillId <= 605
            : (skillId >= 200 && skillId <= 214) || skillId == 2004 || skillId == 3103;
        if (!matches) {
            continue;
        }
        for (std::int32_t recipe = 0; recipe < 56; ++recipe) {
            std::int32_t candidate;
            std::memcpy(&candidate, record + offsetof(SkillRuntimeRecord, recipes8E8)
                + recipe * sizeof(SkillRecipeRecord) + offsetof(SkillRecipeRecord, value10),
                sizeof(candidate));
            if (candidate == value) {
                return 1;
            }
        }
    }
    return 0;
}

std::int32_t Sub_505040(const std::int32_t slot)
{
    const PlayerItemRecord* const item = &gPlayerItemsFDA8A4[slot];
    if (item->hasDescription == 0) {
        return 0;
    }
    const std::int32_t type = item->itemType;
    if (type == 38 || type == 25 || type == 39) {
        return 1;
    }
    if ((item->flags & 0x800u) == 0) {
        return 0;
    }
    return Sub_504F10(item->itemId, 0) != 0 ? 1 : 0;
}

std::int32_t Sub_5050B0(const std::int32_t slot)
{
    const PlayerItemRecord* const item = &gPlayerItemsFDA8A4[slot];
    if (item->hasDescription == 0) {
        return 0;
    }
    if (item->itemType == 80) {
        return 1;
    }
    if ((item->flags & 0x800u) == 0) {
        return 0;
    }
    return Sub_504F10(item->itemId, 1) != 0 ? 1 : 0;
}

std::int32_t Sub_505110(const std::int32_t slot)
{
    const PlayerItemRecord* const item = &gPlayerItemsFDA8A4[slot];
    if (item->hasDescription == 0) {
        return 0;
    }
    const std::int32_t type = item->itemType;
    if (!((type >= 0 && type < 23) || (type >= 56 && type < 66))) {
        return 0;
    }
    return (item->flags & 0x1000u) != 0 ? 1 : 0;
}

void Sub_509EB0(const std::uintptr_t /*socket*/, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char name[256];
    std::int32_t field = 2;
    for (;;) {
        const std::int32_t index =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field - 1);
        const std::int32_t skillId =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::int32_t level =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 2);
        const std::int32_t experience =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 3);
        const std::int32_t maxLevel =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
        const std::int32_t nextExperience =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 5);
        const std::int32_t requiredSlots =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 6);
        const std::int32_t canRebirth =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 7);
        const std::int32_t fpReduce =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 8);
        const std::int32_t sequence =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 9);
        if (index < 0) {
            break;
        }

        SkillRuntimeRecord& record = gSkillsF1BAFC[index];
        if (std::strlen(name) == 0 && level < 0 && experience < 0) {
            record.name[0] = '\0';
            record.level = 0;
            record.experience = 0;
            std::memset(record.techniques3C.data(), 0, sizeof(record.techniques3C));
            Sub_519950(index);
        } else {
            std::strcpy(
                record.name.data(), std::strlen(name) <= 24 ? name : kUnknownText554590);
            record.level = level;
            record.experience = experience;
            record.maxLevel = maxLevel;
            record.nextExperience = nextExperience;
            record.requiredSlots19[0] = static_cast<std::uint8_t>(requiredSlots);
            record.canRebirth = canRebirth;
            record.skillId = skillId;
            record.fpReduce = fpReduce;
            record.sequence = sequence;
            if (canRebirth == 1) {
                const std::uint8_t flag = ui::ReadValue5F9754();
                const std::uintptr_t socket = platform::gConnectionSocket;
                const std::int32_t value = index + (flag != 0 ? 100 : 0);
                static_cast<void>(network::Sub_510F50(socket, value));
            }
        }
        field += 11;
    }
    Sub_518600(gSkillsF1BAFC.data(), gSkillPairsF63370.data(), 15);
    gValueDA3B20 = 0;
}

void Sub_50A120(
    const std::uintptr_t, const std::int32_t skillIndex, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    auto& techniques = gSkillsF1BAFC[skillIndex].techniques3C;
    std::memset(techniques.data(), 0, sizeof(techniques));
    SkillTechniqueRecord* target = techniques.data();
    char name[256];
    char description[256];
    std::int32_t field = 2;
    for (;;) {
        if (network::ExtractDelimitedField_52CED0(
                payload, '|', field - 1, 255, name) == 1) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        const std::int32_t value7C =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 1);
        const std::int32_t value80 =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 2);
        const auto packed = static_cast<std::uint32_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 3));
        const std::int32_t value8C =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
        if (std::strlen(name) == 0) {
            continue;
        }
        target->used88 = 1;
        std::strcpy(target->name.data(), std::strlen(name) <= 24 ? name : kUnknownText554590);
        std::strcpy(
            target->description.data(),
            std::strlen(description) <= 96 ? description : kUnknownText554590);
        target->value7C = value7C;
        target->value80 = value80;
        target->value90 = static_cast<std::int32_t>((packed >> 12) & 0xFu);
        target->value84 = static_cast<std::int32_t>(packed & 0xFFFu);
        target->value8C = value8C;
        field += 6;
        ++target;
    }
}

void Sub_50C580(
    const std::uintptr_t, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    SkillRuntimeRecord& record = gSkillsF1BAFC[index];
    record.rebirthState4C48 = std::bit_cast<std::int8_t>(ui::ReadValue5F9754());
    char name[256];
    char description[256];
    std::int32_t field = 1;
    for (SkillRecipeRecord& recipe : record.recipes8E8) {
        recipe.itemId = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        recipe.level = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        recipe.value10 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        network::ExtractDelimitedField_52CED0(payload, '|', field++, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) <= 28) {
            std::strcpy(recipe.name.data(), name);
        }
        network::ExtractDelimitedField_52CED0(payload, '|', field++, 255, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        if (std::strlen(description) <= 56) {
            std::strcpy(recipe.description.data(), description);
        }
        recipe.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        recipe.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        for (SkillRecipeItem& item : recipe.items6C) {
            item.itemId = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
            network::ExtractDelimitedField_52CED0(payload, '|', field++, 255, name);
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            if (std::strlen(name) <= 28) {
                std::strcpy(item.name.data(), name);
            }
            item.count = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        }
    }
}

void Sub_519950(const std::int32_t index)
{
    SkillRuntimeRecord& record = gSkillsF1BAFC[index];
    record.rebirthState4C48 = 0;
    for (SkillRecipeRecord& recipe : record.recipes8E8) {
        recipe.itemId = -1;
        for (SkillRecipeItem& item : recipe.items6C) {
            item.itemId = -1;
        }
    }
}

std::int32_t Sub_518660(
    const SkillPairRecord* const left, const SkillPairRecord* const right)
{
    if (right->used == 0) {
        return -1;
    }
    if (left->used == 0) {
        return 1;
    }
    return left->sequence <= right->sequence ? -1 : 1;
}

void Sub_518600(
    const SkillRuntimeRecord* source, SkillPairRecord* const destination,
    const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        destination[index].used = source->name[0] != '\0' ? 1 : 0;
        destination[index].index = static_cast<std::uint16_t>(index);
        destination[index].sequence = source->sequence;
        ++source;
    }

    // 原53C112/53C266的8B记录路径；比较器不返回0，不能换现代排序。
    const auto sortCount = static_cast<std::uint32_t>(count);
    if (sortCount < 2) {
        return;
    }
    SkillPairRecord* first = destination;
    SkillPairRecord* last = first + sortCount - 1;
    std::array<SkillPairRecord*, 30> firstStack;
    std::array<SkillPairRecord*, 30> lastStack;
    std::int32_t stackSize = 0;
    for (;;) {
        const auto length = last - first + 1;
        if (length <= 8) {
            while (last > first) {
                SkillPairRecord* maximum = first;
                for (SkillPairRecord* item = first + 1; item <= last; ++item) {
                    if (Sub_518660(item, maximum) > 0) {
                        maximum = item;
                    }
                }
                std::swap(*maximum, *last);
                --last;
            }
        } else {
            std::swap(*(first + length / 2), *first);
            SkillPairRecord* left = first;
            SkillPairRecord* right = last + 1;
            for (;;) {
                do {
                    ++left;
                } while (left <= last && Sub_518660(left, first) <= 0);
                do {
                    --right;
                } while (right > first && Sub_518660(right, first) >= 0);
                if (right < left) {
                    break;
                }
                std::swap(*left, *right);
            }
            std::swap(*first, *right);
            if (right - first > last - left) {
                if (first + 1 < right) {
                    firstStack[stackSize] = first;
                    lastStack[stackSize] = right - 1;
                    ++stackSize;
                }
                if (left < last) {
                    first = left;
                    continue;
                }
            } else {
                if (left < last) {
                    firstStack[stackSize] = left;
                    lastStack[stackSize] = last;
                    ++stackSize;
                }
                if (first + 1 < right) {
                    last = right - 1;
                    continue;
                }
            }
        }
        if (--stackSize < 0) {
            return;
        }
        first = firstStack[stackSize];
        last = lastStack[stackSize];
    }
}

std::array<PlayerItemRecord, 28>& gPlayerItemsFDA8A4 = gPlayerRuntime.items;

namespace {

// 两物品回复仅非空名称的字段尾相同，外层停止、清理及UI同步各自保留。
void ApplyPlayerItemFields(
    const std::int32_t index, const std::int32_t nameField,
    const char* const payload, const char* const name,
    char* const memo, char* const description)
{
    PlayerItemRecord& record = gPlayerItemsFDA8A4[index];
    record.used = 1;
    if (std::strlen(name) <= 28) {
        std::strcpy(record.name.data(), name);
    }
    const std::int32_t color =
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 1);
    record.color = color;
    if (color < 0) {
        record.color = 0;
    }
    network::ExtractDelimitedField_52CED0(payload, '|', nameField + 2, 767, description);
    input::UnescapeLegacyTextAndNotify_52D2F0(description);
    Sub_5196B0(index, description);
    record.hasDescription = std::strlen(record.description[0].data()) != 0 ? 1 : 0;
    record.imageNo = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 3);
    record.canUseInField = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 4));
    record.battleUseFlag = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 5));
    record.target = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 6));
    record.level = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 7);
    record.flags = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 8));
    record.itemId = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 9);
    record.itemType = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 10);
    record.count = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 11);
    network::ExtractDelimitedField_52CED0(payload, '|', nameField + 12, 255, memo);
    input::UnescapeLegacyTextAndNotify_52D2F0(memo);
    if (std::strlen(memo) <= 16) {
        std::strcpy(record.memo.data(), memo);
    }
}

} // namespace

void Sub_50BF20(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    std::int32_t field = 1;
    std::int32_t objectIndex =
        network::ParseDelimitedBase62_52D0D0(payload, ',', field);
    while (objectIndex != -1) {
        Sub_4462E0(objectIndex);
        for (auto& slot : gPetMapCharaSlots) {
            if (slot.flags != 0
                && slot.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
                slot.action = nullptr;
                break;
            }
        }
        ++field;
        objectIndex = network::ParseDelimitedBase62_52D0D0(payload, ',', field);
    }
}

void Sub_5196B0(const std::int32_t index, const char* const text)
{
    PlayerItemRecord& record = gPlayerItemsFDA8A4[index];
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char* const row = record.description[line].data();
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            last = line;
        } else {
            row[0] = '\0';
        }
    }
    while (last > 0 && std::strlen(record.description[last - 1].data()) == 0) {
        --last;
    }
    if (last < 0) {
        record.pages = 0;
    } else {
        const std::int32_t pages = last / 8 + 1;
        record.pages = pages;
        if (pages > 4) {
            record.pages = 4;
        }
    }
}

void Sub_50BFD0(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char name[256];
    char memo[256];
    char description[768];
    for (std::int32_t index = 0; index < 28; ++index) {
        static_cast<void>(ui::detail::Sub_4FB920(index));
        const std::int32_t nameField = 2 + index * 13;
        network::ExtractDelimitedField_52CED0(payload, '|', nameField, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) == 0) {
            gPlayerItemsFDA8A4[index].used = 0;
            gPlayerItemsFDA8A4[index].name[0] = '\0';
        } else {
            ApplyPlayerItemFields(index, nameField, payload, name, memo, description);
        }
        if (index >= 8) {
            ui::detail::Sub_4E54E0(1, index - 8);
        }
    }
}

void Sub_50C260(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char name[256];
    char memo[256];
    char description[768];
    std::int32_t field = 1;
    std::int32_t index = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
    static_cast<void>(ui::detail::Sub_4FB920(index));
    std::int32_t status =
        network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, name);
    while (status != 1) {
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) == 0) {
            gPlayerItemsFDA8A4[index].used = 0;
        } else {
            ApplyPlayerItemFields(index, field + 1, payload, name, memo, description);
        }
        if (index >= 8) {
            ui::detail::Sub_4E54E0(1, index - 8);
            ui::detail::Sub_4E0100(1, index - 8);
        }
        field += 14;
        index = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        static_cast<void>(ui::detail::Sub_4FB920(index));
        status = network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, name);
    }
}

void Sub_51B530(const std::int32_t first, const std::int32_t second)
{
    if (first < 0 || second < 0) {
        return;
    }
    PlayerItemRecord* const left = &gPlayerItemsFDA8A4[first];
    PlayerItemRecord* const right = &gPlayerItemsFDA8A4[second];
    PlayerItemRecord temporary;
    std::memcpy(&temporary, left, sizeof(temporary));
    std::memmove(left, right, sizeof(temporary));
    std::memcpy(right, &temporary, sizeof(temporary));
    if (first < 8) {
        left->flags |= 0x80u;
    }
    if (second < 8) {
        right->flags |= 0x80u;
    }
    ui::detail::Sub_48BA60(first, second);
}

std::int32_t Sub_51B5F0(const std::int32_t itemId)
{
    std::int32_t count = 0;
    if (itemId <= 0) {
        return count;
    }
    for (const PlayerItemRecord* item = gPlayerItemsFDA8A4.data() + 8;
         item != gPlayerItemsFDA8A4.data() + gPlayerItemsFDA8A4.size(); ++item) {
        if (item != nullptr && item->used != 0 && item->itemId == itemId) {
            count += item->count;
        }
    }
    return count;
}

void Sub_50C900(
    const std::uintptr_t, const std::int32_t first, const std::int32_t second)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    Sub_51B530(first, second);
    static_cast<void>(ui::detail::Sub_4FB920(first));
    static_cast<void>(ui::detail::Sub_4FB920(second));
    ui::detail::Sub_4E5DA0(1, first, second);
    ui::detail::Sub_4E1840(1, first, second);
}

void Sub_51B630(
    const std::int32_t firstGroup, const std::int32_t firstIndex,
    const std::int32_t secondGroup, const std::int32_t secondIndex)
{
    if (firstIndex < 0 || secondIndex < 0) {
        return;
    }
    PlayerItemRecord* firstBase = nullptr;
    PlayerItemRecord* secondBase = nullptr;
    if (firstGroup == 0) {
        if (firstIndex >= 28) {
            return;
        }
        firstBase = gPlayerItemsFDA8A4.data();
    } else if (firstGroup >= 1 && firstGroup <= 5) {
        if (firstIndex >= 6) {
            return;
        }
        firstBase = gPetRuntimeF633F8[firstGroup - 1].items6E4.data();
    }
    if (secondGroup == 0) {
        if (secondIndex >= 28) {
            return;
        }
        secondBase = gPlayerItemsFDA8A4.data();
    } else if (secondGroup >= 1 && secondGroup <= 5) {
        if (secondIndex >= 6) {
            return;
        }
        secondBase = gPetRuntimeF633F8[secondGroup - 1].items6E4.data();
    }
    if (firstBase == nullptr || secondBase == nullptr) {
        return;
    }
    PlayerItemRecord* const first = firstBase + firstIndex;
    PlayerItemRecord* const second = secondBase + secondIndex;
    PlayerItemRecord temporary;
    std::memcpy(&temporary, first, sizeof(temporary));
    std::memmove(first, second, sizeof(temporary));
    std::memcpy(second, &temporary, sizeof(temporary));
}

// 原宠物表、索引对和两独立标量均为PE虚拟零区；构造由LegacyMouse按CRT顺序接入。
std::array<PetRuntimeRecord, 5> gPetRuntimeF633F8 {};
std::array<PetPairRecord, 5> gPetPairsDE0B08 {};
std::int32_t gPetCountDE0B00 = 0;
std::uint32_t gValueD67B28 = 0;

// 原F7C950同属PE虚拟零区，独立于宠物表与F7C9FC之后的状态。
ReplyRecordF7C950 gReplyRecordF7C950 {};

std::int32_t Sub_52E180(const std::int32_t index, const std::int32_t techniqueId)
{
    if (index < 0 || index >= 5) {
        return 0;
    }
    const PetRuntimeRecord& pet = gPetRuntimeF633F8[index];
    if (pet.used == 0) {
        return 0;
    }
    for (const PetTechniqueRecord& technique : pet.techniquesD8) {
        if (technique.used != 0 && technique.techniqueId == techniqueId) {
            return 1;
        }
    }
    return 0;
}

void Sub_519770(
    const std::int32_t petIndex, const std::int32_t itemIndex, const char* const text)
{
    PlayerItemRecord& record = gPetRuntimeF633F8[petIndex].items6E4[itemIndex];
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char* const row = record.description[line].data();
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            last = line;
        } else {
            row[0] = '\0';
        }
    }
    while (last > 0 && std::strlen(record.description[last - 1].data()) == 0) {
        --last;
    }
    record.pages = last < 0 ? 0 : last / 8 + 1;
}

void Sub_510150(
    const std::uintptr_t, const std::int32_t petIndex, const char* const payload)
{
    if (gValue9A6E0C != 0 || petIndex < 0 || petIndex >= 5) {
        return;
    }
    char name[256];
    char memo[256];
    char description[768];
    std::int32_t field = 1;
    std::int32_t index = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
    while (index < 6) {
        if (network::ExtractDelimitedField_52CED0(
                payload, '|', field + 1, 255, name) == 1) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        PlayerItemRecord& record = gPetRuntimeF633F8[petIndex].items6E4[index];
        if (std::strlen(name) == 0) {
            record.used = 0;
            record.name[0] = '\0';
        } else {
            record.used = 1;
            if (std::strlen(name) <= 28) {
                std::strcpy(record.name.data(), name);
            }
            const std::int32_t color =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 2);
            record.color = color;
            if (color < 0) {
                record.color = 0;
            }
            network::ExtractDelimitedField_52CED0(
                payload, '|', field + 3, 767, description);
            input::UnescapeLegacyTextAndNotify_52D2F0(description);
            Sub_519770(petIndex, index, description);
            record.hasDescription = std::strlen(record.description[0].data()) != 0 ? 1 : 0;
            record.imageNo = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
            record.canUseInField = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 5));
            record.battleUseFlag = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 6));
            record.target = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 7));
            // 原510362再次传field+4，不能按玩家物品协议改成field+8或复用图号。
            record.level = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
            record.flags = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 9));
            record.itemId = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 10);
            record.itemType = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 11);
            record.count = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 12);
            network::ExtractDelimitedField_52CED0(payload, '|', field + 13, 255, memo);
            input::UnescapeLegacyTextAndNotify_52D2F0(memo);
            if (std::strlen(memo) <= 16) {
                std::strcpy(record.memo.data(), memo);
            }
        }
        field += 14;
        index = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
    }
}

void Sub_50A6B0(
    const std::uintptr_t /*socket*/, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0 || index < 0 || index >= 5) {
        return;
    }
    const auto flags = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    PetRuntimeRecord& record = gPetRuntimeF633F8[index];
    std::int32_t field = 2;
    if (flags == 0) {
        std::memset(&record, 0, sizeof(record));
    } else {
        record.used = 1;
        if ((flags & 0x3u) != 0) {
            const std::int32_t imageNo =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            record.imageNo6A0 = character_select::detail::Sub_51B740(imageNo);
            ++field;
        }
        if ((flags & 0x5u) != 0) {
            record.value04 = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field));
            ++field;
        }
        std::uint32_t bit = 0x8u;
        for (platform::LegacyCryptInt& value : record.values0C) {
            if ((flags & (bit | 1u)) != 0) {
                const std::int32_t parsed =
                    network::ParseDelimitedInteger_52CFA0(payload, '|', field);
                platform::LegacyCryptInt temporary;
                platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, parsed);
                value.mask = temporary.mask;
                value.encoded = temporary.encoded;
                value.readCount = temporary.readCount;
                platform::legacy_crypt_int::Destroy_401CF0(&temporary);
                ++field;
            }
            bit <<= 1;
        }

        // 掩码201..100001按原顺序映射，不将两条协议的数值列混用。
        const std::array<std::int32_t*, 12> targets {{
            &record.value08,
            &record.values70[5], &record.values70[6], &record.values70[7],
            &record.values70[8], &record.values70[9], &record.values70[10],
            &record.values70[21], &record.values70[22],
            &record.values70[23], &record.values70[24],
            &record.value6A4,
        }};
        for (std::int32_t* const target : targets) {
            if ((flags & (bit | 1u)) != 0) {
                *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
                ++field;
            }
            bit <<= 1;
        }
        if ((flags & 0x200001u) != 0) {
            record.value02 = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field));
            ++field;
        }
        if ((flags & 0x400001u) != 0) {
            char name[256];
            network::ExtractDelimitedField_52CED0(payload, '|', field, 255, name);
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            if (std::strlen(name) <= 16) {
                std::strcpy(record.name6AC.data(), name);
            }
            ++field;
        }
        if ((flags & 0x800001u) != 0) {
            char name[256];
            network::ExtractDelimitedField_52CED0(payload, '|', field, 255, name);
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            if (std::strlen(name) <= 16) {
                std::strcpy(record.name6BD.data(), name);
            }
            ++field;
        }
        if ((flags & 0x1000001u) != 0) {
            const auto state = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field));
            if ((state & 0x10u) != 0) {
                record.flags6AA = static_cast<std::uint16_t>(state & ~0x10u);
                record.follow6D4 = 1;
            } else {
                record.flags6AA = state;
                record.follow6D4 = 0;
            }
            ++field;
        }
        if ((flags & 0x2000001u) != 0) {
            record.values70[25] =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        }
    }
    Sub_519920();
    Sub_518580(gPetRuntimeF633F8.data(), gPetPairsDE0B08.data(), 5);
    Sub_492250();
    record.updated510C = 1;
}

void Sub_50AC20(
    const std::uintptr_t /*socket*/, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0 || index < 0 || index >= 5) {
        return;
    }
    const auto flags = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    PetRuntimeRecord& record = gPetRuntimeF633F8[index];
    if (flags == 0) {
        record.used = 0;
        record.name6AC[0] = '\0';
        record.name6BD[0] = '\0';
    } else {
        record.used = 1;
        const std::array<std::int32_t*, 19> targets {{
            &record.values70[0], &record.values70[1], &record.values70[2],
            &record.values70[3], &record.values70[4],
            &record.values70[17], &record.values70[18],
            &record.values70[19], &record.values70[20],
            &record.values70[11], &record.values70[12], &record.values70[13],
            &record.values70[14], &record.values70[15], &record.values70[16],
            &record.value6D0,
            &record.values6D8[1], &record.values6D8[2], &record.values6D8[0],
        }};
        std::uint32_t bit = 2;
        std::int32_t field = 2;
        for (std::int32_t* const target : targets) {
            if ((flags & (bit | 1u)) != 0) {
                *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
                ++field;
            }
            bit <<= 1;
        }
    }
    Sub_519920();
    Sub_518580(gPetRuntimeF633F8.data(), gPetPairsDE0B08.data(), 5);
    Sub_492250();
}

void Sub_50B0D0(
    const std::uintptr_t, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0 || index < 0 || index >= 5) {
        return;
    }
    PetRuntimeRecord& record = gPetRuntimeF633F8[index];
    if (record.used == 0) {
        return;
    }
    std::memset(record.techniquesD8.data(), 0, sizeof(record.techniquesD8));
    char name[256];
    char description[256];
    std::int32_t field = 2;
    for (;;) {
        const std::int32_t techniqueIndex =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field - 1);
        if (techniqueIndex < 0) {
            break;
        }
        const std::int32_t techniqueId =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 2, 255, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        const std::int32_t fp =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 3);
        const std::int32_t techniqueField =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
        const std::int32_t target =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 5);
        const std::int32_t sequence =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 6);
        PetTechniqueRecord& technique = record.techniquesD8[techniqueIndex];
        if (std::strlen(name) == 0) {
            technique.name[0] = '\0';
            technique.description[0] = '\0';
            technique.fp = 0;
            technique.used = 0;
            // 原50B230不推进字段；不把空名称改成跳过本条或结束解析。
            continue;
        }
        technique.used = 1;
        technique.techniqueId = techniqueId;
        std::strcpy(technique.name.data(), std::strlen(name) <= 24 ? name : "???");
        std::strcpy(
            technique.description.data(),
            std::strlen(description) <= 96 ? description : "???");
        field += 8;
        technique.fp = fp;
        technique.field = static_cast<std::uint16_t>(techniqueField);
        technique.target = static_cast<std::uint16_t>(target);
        technique.sequence = sequence;
    }
    Sub_5185C0(record.techniquesD8.data(), record.techniquePairs650.data(), 10);
}

void Sub_5185C0(
    const PetTechniqueRecord* const source, SkillPairRecord* const destination,
    const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        destination[index].used = source[index].used;
        destination[index].index = static_cast<std::uint16_t>(index);
        destination[index].sequence = source[index].sequence;
    }
}

std::int32_t Sub_43C4B0()
{
    for (std::int32_t index = 0; index < 5; ++index) {
        const PetRuntimeRecord& pet = gPetRuntimeF633F8[index];
        if (pet.used == 1 && pet.flags6AA == 2) {
            return index;
        }
    }
    return -1;
}

std::int32_t Sub_519920()
{
    std::int32_t count = 0;
    gPetCountDE0B00 = 0;
    for (const PetRuntimeRecord& record : gPetRuntimeF633F8) {
        if (record.used != 0) {
            ++count;
        }
    }
    gPetCountDE0B00 = count;
    return count;
}

void Sub_518580(
    const PetRuntimeRecord* source, PetPairRecord* destination, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        destination->used = source->used;
        destination->index = static_cast<std::uint16_t>(index);
        destination->value = source->value6D0;
        ++source;
        ++destination;
    }
}

void Sub_492250()
{
    gValueD67B28 = 1;
}

// 原两表均位于PE虚拟零区；5199A0后续只重置原表编号，不能清副本。
std::array<TitleRecord, 64> gTitlesFF0508 {};
std::array<TitleRecord, 64> gSortedTitlesDE03F8 {};

void Sub_50A300(const std::uintptr_t /*socket*/, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char text[256];
    std::int32_t field = 2;
    for (TitleRecord& record : gTitlesFF0508) {
        network::ExtractDelimitedField_52CED0(payload, '|', field - 1, 255, text);
        input::UnescapeLegacyTextAndNotify_52D2F0(text);
        const std::int32_t id = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        if (id >= 0) {
            record.id = id;
            std::strcpy(record.text.data(), text);
            record.value = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 1);
        } else {
            record.id = -1;
            record.text[0] = '\0';
            record.value = -1;
        }
        field += 3;
    }
    Sub_5198A0(
        reinterpret_cast<std::uint8_t*>(gSortedTitlesDE03F8.data()),
        reinterpret_cast<const std::uint8_t*>(gTitlesFF0508.data()), 64);
    Sub_5198D0(gSortedTitlesDE03F8.data(), 64);
}

std::int32_t Sub_519840(const std::int32_t id)
{
    if (id < 0) {
        return -1;
    }
    for (std::int32_t index = 0; index < 64; ++index) {
        if (gTitlesFF0508[index].id == id) {
            return index;
        }
    }
    return -1;
}

std::int32_t Sub_519870()
{
    std::int32_t count = 0;
    for (const TitleRecord& record : gTitlesFF0508) {
        if (record.id >= 0) {
            ++count;
        }
    }
    return count / 8 + 1;
}

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

// 原5198D0固定28B记录，保留53C112/53C266的比较和交换次序。
void Sub_5198D0(TitleRecord* const records, const std::uint32_t count)
{
    if (count < 2) {
        return;
    }
    TitleRecord* first = records;
    TitleRecord* last = first + count - 1;
    std::array<TitleRecord*, 30> firstStack;
    std::array<TitleRecord*, 30> lastStack;
    std::int32_t stackSize = 0;
    for (;;) {
        const auto length = last - first + 1;
        if (length <= 8) {
            while (last > first) {
                TitleRecord* maximum = first;
                for (TitleRecord* item = first + 1; item <= last; ++item) {
                    if (Sub_5198F0(item, maximum) > 0) {
                        maximum = item;
                    }
                }
                std::swap(*maximum, *last);
                --last;
            }
        } else {
            std::swap(*(first + length / 2), *first);
            TitleRecord* left = first;
            TitleRecord* right = last + 1;
            for (;;) {
                do {
                    ++left;
                } while (left <= last && Sub_5198F0(left, first) <= 0);
                do {
                    --right;
                } while (right > first && Sub_5198F0(right, first) >= 0);
                if (right < left) {
                    break;
                }
                std::swap(*left, *right);
            }
            std::swap(*first, *right);
            // 原按字节比较(right-first)-1 >= last-left，整项距离等价为严格大于。
            if (right - first > last - left) {
                if (first + 1 < right) {
                    firstStack[stackSize] = first;
                    lastStack[stackSize] = right - 1;
                    ++stackSize;
                }
                if (left < last) {
                    first = left;
                    continue;
                }
            } else {
                if (left < last) {
                    firstStack[stackSize] = left;
                    lastStack[stackSize] = last;
                    ++stackSize;
                }
                if (first + 1 < right) {
                    last = right - 1;
                    continue;
                }
            }
        }
        if (--stackSize < 0) {
            return;
        }
        first = firstStack[stackSize];
        last = lastStack[stackSize];
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
std::int32_t gDialogRequestCC21E0 = 0;
std::int32_t gDialogTargetD2DE6C = 0;

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
std::uint32_t gValue62B398 = 0;
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

// 原59F9F8；50D120生产，421480后续复用。
std::int32_t gBattleBackgroundImage59F9F8 = 0;
std::int32_t gBattleType5ABFC8 = 0;
std::uint32_t gBattleModeFlag5ABFCC = 0;
std::uint32_t gValue5ABB1C = 0;
std::uint32_t gValue5ABB28 = 0;
std::uint32_t gBattleFirstTurnPending5ABC6C = 0;
std::int32_t gBattleCWriteIndex5AAA6C = 0;
std::int32_t gBattleCReadIndex5AAA44 = 0;
std::int32_t gBattleMWriteIndex5A3A1C = 0;
std::int32_t gBattleMReadIndex5AAA68 = 0;
std::array<char, 0x4000> gBattleCQueue5A4A3C {};
std::array<char, 0x4000> gBattleMQueue59FA18 {};
std::array<char, 0x1000> gBattleCText5A3A20 {};
std::array<char, 0x1000> gBattleMText5AAA7C {};
std::array<login::YobiAction*, 20> gBattleEntries59E980 {};
std::int32_t gBattleCurrentPosition5ABCA4 = 0;
std::uint32_t gBattlePosition5ABCA8 = 0;
std::int32_t gBattlePlayerActionState5ABC74 = 0;
std::array<std::int32_t, 9> gBattleSelectionModes5ABC7C {};
std::int32_t gBattleSelectedSkill5ABB5C = 0;
std::int32_t gBattleSelectedTechnique5ABC78 = 0;
std::int32_t gBattleCommandState5ABC3C = 0;
std::uint8_t gBattleRebirthDisabled5ABC40 = 0;
std::int32_t gBattleSkillActionCount5ABBF4 = 0;
std::int32_t gBattlePetActionCount5ABBBC = 0;
std::int32_t gBattleSelectionMode5ABC70 = 0;
std::int32_t gBattleSkillUsed5ABC68 = 0;
std::int32_t gBattlePetCommandPhase553F88 = 1;
std::int32_t gBattleValue5ABCC0 = 0;
login::YobiAction* gBattleCommandAction5ABC00 = nullptr;
std::uint32_t gBattleTargetFlags5ABC44 = 0;
std::int32_t gBattleSelectedItem5ABC48 = 0;
std::int32_t gBattleItemTargetPending5ABCC4 = 0;
std::int32_t gBattleBombLevelD67B2C = 0;
std::uint32_t gBattlePlayerFlags5ABCAC = 0;
std::uint32_t gBattleSkillFlags5ABC04 = 0;
std::uint32_t gBattlePetSkillFlags5ABB78 = 0;
std::uint32_t gBattleRebirthLevel5ABB7C = 0;
std::uint32_t gBattleRidePet5ABC38 = 0;
std::uint32_t gBattleEndBits5ABBC0 = 0;
std::uint32_t gBattleTurn5ABB74 = 0;
std::uint32_t gBattleTurnSnapshot5ABC08 = 0;
std::uint32_t gValue5ABCB0 = 0;
static_assert(sizeof(gBattleEntries59E980) == 20 * sizeof(login::YobiAction*));
static_assert(sizeof(gBattleCQueue5A4A3C) == 4 * 0x1000);
static_assert(sizeof(gBattleMQueue59FA18) == 4 * 0x1000);
static_assert(sizeof(gBattleCText5A3A20) == 0x1000);
static_assert(sizeof(gBattleMText5AAA7C) == 0x1000);
// 原5ABCD8；43CB10末尾重读图号后发布，DrawBattleScene用于重建判断。
std::int32_t gBattleBackgroundSnapshot5ABCD8 = 0;

// 原 0xD2F788/0xFF0C10：504A30 清零的独立 S32/W16 状态槽。
std::int32_t gValueD2F788 = 0;
std::uint16_t gValueFF0C10 = 0;

// 原 0xFF0D04/0xFF0D08：50D700 写入、展示侧按 S32 读取的独立槽。
std::int32_t gValueFF0D04 = 0;
std::int32_t gValueFF0D08 = 0;

// 原 0xFF0D34：50D3B0 写入、5199A0 清零的独立 W16 槽。
std::uint16_t gValueFF0D34 = 0;
// 原 519A80 公共尾；协议与普通场景后续复用，初值为 PE 零填充。
std::uint16_t gValueFF0D30 = 0;
std::uint16_t gValueFF0D32 = 0;
std::uint16_t gValueF7CA04 = 0;
std::uint16_t gValueFF03D8 = 0;
std::uint16_t gValueFF0D36 = 0;
std::uint32_t gValueFF0D38 = 0;

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
std::uint32_t gValue9EE8F8 = 0;
std::uint32_t gValue9EE8FC = 0;
std::uint32_t gValue9EE900 = 0;
std::uint32_t gValue9DAD78 = 0;
std::uint8_t gValue9A4B94 = 0;
std::uint16_t gValueC82200 = 0;
std::uint16_t gValueC82202 = 0;
std::array<std::int16_t, 2> gMapWindowEndY9A86C4 {};
std::array<std::int16_t, 2> gMapWindowStartY9A86C8 {};
std::array<std::int16_t, 2> gMapWindowEndX9A86CC {};
std::array<std::int16_t, 2> gMapWindowStartX9A86D0 {};
std::array<std::uint16_t, 2> gPendingDirections9A86D4 {};
std::array<std::uint16_t, kMovePathCapacity> gMovePathDirections {};
std::array<std::uint16_t, kMovePathCapacity> gMovePathX {};
std::array<std::uint16_t, kMovePathCapacity> gMovePathY {};
static_assert(sizeof(gMovePathDirections) == 0xC8);
static_assert(sizeof(gMovePathX) == 0xC8);
static_assert(sizeof(gMovePathY) == 0xC8);
static_assert(sizeof(gMapWindowEndY9A86C4) == 4);
static_assert(sizeof(gMapWindowStartY9A86C8) == 4);
static_assert(sizeof(gMapWindowEndX9A86CC) == 4);
static_assert(sizeof(gMapWindowStartX9A86D0) == 4);
static_assert(sizeof(gPendingDirections9A86D4) == 4);
std::uint32_t gValueC821FC = 0;
std::uint32_t gValue9EE8B8 = 0;
std::uint16_t gValue9EE8A4 = 0;
std::int32_t gValue9A7F7C = 0;
std::int32_t gValue9A7F80 = 0;
std::uint32_t gValueAF10C8 = 0;
std::uint16_t gValueC8A79C = 0;
std::uint16_t gValue9A86D8 = 0;
std::uint16_t gValue9FB92E = 0;
std::uint16_t gValueB65A7C = 0;
std::uint16_t gValueC82220 = 0;
std::uint32_t gValueC82238 = 0;
std::uint32_t gValueC8223C = 0; // 原转向发送的独立GetTickCount位槽。
std::uint32_t gValueC8A774 = 0;
std::uint32_t gValueC8A778 = 0;
std::uint32_t gValueC8A77C = 0;
std::uint32_t gValueC8A780 = 0;
std::int32_t gPlayerMoveTargetX = 0; // 原C8A784；46F716发布。
std::int32_t gPlayerMoveTargetY = 0; // 原C8A788。
std::int32_t gValueC8A78C = 0;
std::int32_t gValueC8A790 = 0;
std::uint32_t gValueC8A794 = 0;
std::uint32_t gValueC8A7A0 = 0;
std::uint32_t gValueC8A7A4 = 0;
std::uint32_t gValueC8A7A8 = 0;
std::uint32_t gValueC8A7AC = 0;
std::uint32_t gValue55DDAC = 0xFFFFFFFFu;
std::uint32_t gValueC82208 = 0;
std::uint32_t gValueC8220C = 0;
std::uint32_t gValueC82210 = 0;
std::uint32_t gValueC82214 = 0;
std::uint32_t gValueC82218 = 0;
std::int32_t gValueFF0D4C = 0;
std::int32_t gValue73ED44 = 0;
std::int32_t gValue73ED48 = 0;
std::uint32_t gValueC82224 = 0;
std::uint32_t gValueC8221C = 0;
std::uint16_t gValue9DAD54 = 0;
std::uint16_t gValue9DAD56 = 0;
std::uint16_t gValue9FB92C = 0;
std::uint16_t gValue55D9FC = 0xFFFFu;
std::uint16_t gValueC821D0 = 0;
std::uint16_t gValue9EE910 = 0;
std::array<std::uint16_t, kMapBufferCells> gMapTileArray {};
std::array<std::uint16_t, kMapBufferCells> gMapObjArray {};
std::array<std::uint16_t, kMapBufferCells> gMapHitArray {};
std::array<std::uint16_t, kMapBufferCells> gMapWalkabilityArray {};
std::uint16_t gLastDetectedCode = 0;
std::int32_t gValue9E0434 = 0;

static_assert(sizeof(gMapTileArray) == 5202);
static_assert(sizeof(gMapObjArray) == 5202);
static_assert(sizeof(gMapHitArray) == 5202);
static_assert(sizeof(gMapWalkabilityArray) == 5202);

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

// 原 [0x468570,0x4687A3)：仅缺文件路径尝试建目录，旧头比较后可重写。
std::int32_t __cdecl Sub_468570(
    const std::int32_t a1, const std::int32_t a2,
    const std::int32_t a3, const std::int32_t a4,
    const std::int32_t a5, const std::int32_t a6,
    const std::int32_t a7)
{
    if (a3 == 0) {
        return 0;
    }

    char path[256];
    Sub_468520(path, a2, a1, a3);
    std::FILE* const file = std::fopen(path, kMapReadMode);
    if (file == nullptr) {
        ::_mkdir(kMapPathPrefix);
        char directory[256];
        std::sprintf(directory, kMapDirectoryOne, kMapPathPrefix, a2);
        ::_mkdir(directory);
        if (a2 > 0) {
            std::sprintf(directory, kMapDirectoryTwo, kMapPathPrefix, a2, a1);
            ::_mkdir(directory);
        }
        return Sub_4687B0(path, a6, a7, a4, a5) >= 0 ? 1 : -1;
    }

    std::fseek(file, 0, SEEK_SET);
    char label[4];
    label[0] = static_cast<char>(std::fgetc(file));
    label[1] = static_cast<char>(std::fgetc(file));
    label[2] = static_cast<char>(std::fgetc(file));
    label[3] = '\0';
    const std::uint8_t version = static_cast<std::uint8_t>(std::fgetc(file));
    std::int32_t oldSixth = 0;
    std::int32_t oldSeventh = 0;
    std::int32_t oldFourth = 0;
    std::int32_t oldFifth = 0;
    std::fread(&oldSixth, 4, 1, file);
    std::fread(&oldSeventh, 4, 1, file);
    std::fread(&oldFourth, 4, 1, file);
    std::fread(&oldFifth, 4, 1, file);
    std::fclose(file);

    if (a6 != oldSixth || a7 != oldSeventh
        || a4 != oldFourth || a5 != oldFifth
        || std::strcmp(label, kMapHeaderLabel) != 0
        || version != 1 || a2 == 2) {
        return Sub_4687B0(path, a6, a7, a4, a5) < 0 ? -1 : 0;
    }
    return 0;
}

// 原 [0x4687B0,0x4688A0)：四字段头后，三平面逐 WORD 写零。
std::int32_t __cdecl Sub_4687B0(
    const char* const path,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t height,
    const std::int32_t width)
{
    std::FILE* const file = std::fopen(path, kMapCreateMode);
    if (file == nullptr) {
        return -1;
    }
    std::fputc('M', file);
    std::fputc('A', file);
    std::fputc('P', file);
    std::fputc(1, file);
    std::fwrite(&first, 4, 1, file);
    std::fwrite(&second, 4, 1, file);
    std::fwrite(&height, 4, 1, file);
    std::fwrite(&width, 4, 1, file);
    const std::uint32_t zero = 0;
    for (std::int32_t plane = 0; plane < 3; ++plane) {
        for (std::int32_t row = 0; row < height; ++row) {
            for (std::int32_t column = 0; column < width; ++column) {
                std::fwrite(&zero, 2, 1, file);
            }
        }
    }
    std::fclose(file);
    return 0;
}

// 原 [0x4688A0,0x468BF5)：先写 tile/obj，hit 逐行更新共享缓冲后再写盘。
std::int32_t __cdecl Sub_4688A0(
    const std::int32_t mapId, const std::int32_t floor,
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    const std::uint16_t* const tile, const std::uint16_t* const obj,
    std::uint16_t* const hit)
{
    char path[256];
    Sub_468520(path, mapId, gValue9E0434, floor);
    std::FILE* const file = std::fopen(path, kMapUpdateMode);
    if (file == nullptr) {
        return -1;
    }

    for (int i = 0; i < 4; ++i) {
        std::fgetc(file);
    }
    std::int32_t ignoredFirst = 0;
    std::int32_t ignoredSecond = 0;
    std::int32_t fileWidth = 0;
    std::int32_t fileHeight = 0;
    std::fread(&ignoredFirst, 4, 1, file);
    std::fread(&ignoredSecond, 4, 1, file);
    std::fread(&fileWidth, 4, 1, file);
    std::fread(&fileHeight, 4, 1, file);

    const std::int32_t destinationWidth = xEnd - xStart;
    const std::int32_t sourceX = std::max(xStart, 0);
    const std::int32_t destinationX = std::max(-xStart, 0);
    const std::int32_t sourceY = std::max(yStart, 0);
    const std::int32_t destinationY = std::max(-yStart, 0);
    const std::int32_t copyWidth = std::min(xEnd, fileWidth) - sourceX;
    const std::int32_t copyHeight = std::min(yEnd, fileHeight) - sourceY;
    const std::int32_t sourceStart = sourceX + fileWidth * sourceY;
    const std::int32_t destinationStart =
        destinationX + destinationWidth * destinationY;
    const std::int32_t planeBytes = 2 * fileWidth * fileHeight;
    const std::size_t rowBytes =
        static_cast<std::uint32_t>(2 * copyWidth);

    if (copyHeight > 0) {
        const std::uint16_t* const planes[] {tile, obj};
        for (std::int32_t plane = 0; plane < 2; ++plane) {
            for (std::int32_t row = 0; row < copyHeight; ++row) {
                const std::int32_t fileRow =
                    20 + plane * planeBytes
                    + 2 * (sourceStart + row * fileWidth);
                std::fseek(file, fileRow, SEEK_SET);
                std::fwrite(
                    planes[plane] + destinationStart + row * destinationWidth,
                    rowBytes, 1, file);
            }
        }

        for (std::int32_t row = 0; row < copyHeight; ++row) {
            std::uint16_t* const inputRow =
                hit + destinationStart + row * destinationWidth;
            const std::int32_t y = yStart + row;
            for (std::int32_t column = 0; column < copyWidth; ++column) {
                const std::uint16_t value =
                    static_cast<std::uint16_t>(inputRow[column] | 0xC000u);
                inputRow[column] = value;
                const std::int32_t x = xStart + column;
                if (gMapFloor == floor
                    && gMapXStart <= x && x < gMapXEnd
                    && gMapYStart <= y && y < gMapYEnd) {
                    static_cast<void>(Sub_468C00(x, y, value));
                }
            }
            const std::int32_t fileRow =
                20 + 2 * planeBytes
                + 2 * (sourceStart + row * fileWidth);
            std::fseek(file, fileRow, SEEK_SET);
            std::fwrite(inputRow, rowBytes, 1, file);
        }
    }
    std::fclose(file);
    return 0;
}

// 原 [0x469870,0x469C87)：四段定点 hit 探测，最多发布两项裁剪窗口。
std::int32_t Sub_469870(const std::int32_t direction)
{
    if (gMapSpanX < 51 || gMapSpanY < 51) {
        return 0;
    }
    if (platform::legacy_crypt_int::Greater_401D40(&gPlayerX, 11)) {
        static_cast<void>(
            platform::legacy_crypt_int::Subtract_401E60(&gPlayerX, 11));
    }
    if (platform::legacy_crypt_int::Greater_401D40(&gPlayerY, 11)) {
        static_cast<void>(
            platform::legacy_crypt_int::Subtract_401E60(&gPlayerY, 11));
    }
    if (platform::legacy_crypt_int::IntMinusCrypt_401FE0(
            std::bit_cast<std::int32_t>(gValue9FB930), &gPlayerX) > 11) {
        static_cast<void>(
            platform::legacy_crypt_int::Add_401E10(&gPlayerX, 11));
    }
    if (platform::legacy_crypt_int::IntMinusCrypt_401FE0(
            std::bit_cast<std::int32_t>(gValueC821D4), &gPlayerY) > 11) {
        static_cast<void>(
            platform::legacy_crypt_int::Add_401E10(&gPlayerY, 11));
    }

    gValue9DAD56 = 0;
    std::int32_t found = 0;
    if (direction == 6 || direction == 7 || direction == 5) {
        const std::int32_t x =
            platform::legacy_crypt_int::Subtract_401E60(&gPlayerX, 11);
        std::int32_t y =
            platform::legacy_crypt_int::Subtract_401E60(&gPlayerY, 11);
        const std::int32_t span = gMapSpanX;
        for (std::int32_t step = 0; step < 23; ++step, ++y) {
            if (x >= 0 && x < std::bit_cast<std::int32_t>(gValue9FB930)
                && y >= 0 && y < std::bit_cast<std::int32_t>(gValueC821D4)
                && gMapHitArray[static_cast<std::size_t>(
                    span * (step + 9) + 19)] == 0) {
                const std::size_t index = gValue9DAD56;
                gMapWindowEndX9A86CC[index] = static_cast<std::int16_t>(x + 1);
                gMapWindowStartX9A86D0[index] = static_cast<std::int16_t>(x - 1);
                gMapWindowStartY9A86C8[index] = static_cast<std::int16_t>(y - 1);
                gMapWindowEndY9A86C4[index] =
                    static_cast<std::int16_t>(y - step + 23);
                static_cast<void>(Sub_468290(
                    &gMapWindowStartX9A86D0[index],
                    &gMapWindowStartY9A86C8[index],
                    &gMapWindowEndX9A86CC[index],
                    &gMapWindowEndY9A86C4[index]));
                ++gValue9DAD56;
                found = 1;
                break;
            }
        }
    }

    if (direction == 0 || direction == 7 || direction == 1) {
        std::int32_t x =
            platform::legacy_crypt_int::Subtract_401E60(&gPlayerX, 11);
        const std::int32_t y =
            platform::legacy_crypt_int::Subtract_401E60(&gPlayerY, 11);
        const std::int32_t span = gMapSpanX;
        const std::int32_t xBound = std::bit_cast<std::int32_t>(gValue9FB930);
        for (std::int32_t step = 0; step < 23; ++step, ++x) {
            if (x >= 0 && x < xBound
                && y >= 0 && y < std::bit_cast<std::int32_t>(gValueC821D4)
                && gMapHitArray[static_cast<std::size_t>(
                    9 * span + 19 + step)] == 0) {
                const std::size_t index = gValue9DAD56;
                gMapWindowStartY9A86C8[index] = static_cast<std::int16_t>(y - 1);
                gMapWindowStartX9A86D0[index] = static_cast<std::int16_t>(x - 1);
                gMapWindowEndX9A86CC[index] =
                    static_cast<std::int16_t>(x - step + 23);
                gMapWindowEndY9A86C4[index] = static_cast<std::int16_t>(y + 1);
                static_cast<void>(Sub_468290(
                    &gMapWindowStartX9A86D0[index],
                    &gMapWindowStartY9A86C8[index],
                    &gMapWindowEndX9A86CC[index],
                    &gMapWindowEndY9A86C4[index]));
                ++gValue9DAD56;
                found = 1;
                break;
            }
        }
    }
    if (std::bit_cast<std::int16_t>(gValue9DAD56) >= 2) {
        return found;
    }

    if (direction == 2 || direction == 1 || direction == 3) {
        const std::int32_t x =
            platform::legacy_crypt_int::Add_401E10(&gPlayerX, 11);
        std::int32_t y =
            platform::legacy_crypt_int::Subtract_401E60(&gPlayerY, 11);
        const std::int32_t span = gMapSpanX;
        const std::int32_t xBound = std::bit_cast<std::int32_t>(gValue9FB930);
        for (std::int32_t step = 0; step < 23; ++step, ++y) {
            if (x >= 0 && x < xBound
                && y >= 0 && y < std::bit_cast<std::int32_t>(gValueC821D4)
                && gMapHitArray[static_cast<std::size_t>(
                    span * (step + 9) + 41)] == 0) {
                const std::size_t index = gValue9DAD56;
                gMapWindowStartY9A86C8[index] = static_cast<std::int16_t>(y - 1);
                gMapWindowStartX9A86D0[index] = static_cast<std::int16_t>(x);
                gMapWindowEndX9A86CC[index] = static_cast<std::int16_t>(x + 2);
                gMapWindowEndY9A86C4[index] =
                    static_cast<std::int16_t>(y - step + 23);
                static_cast<void>(Sub_468290(
                    &gMapWindowStartX9A86D0[index],
                    &gMapWindowStartY9A86C8[index],
                    &gMapWindowEndX9A86CC[index],
                    &gMapWindowEndY9A86C4[index]));
                ++gValue9DAD56;
                found = 1;
                break;
            }
        }
    }
    if (std::bit_cast<std::int16_t>(gValue9DAD56) >= 2) {
        return found;
    }

    if (direction == 4 || direction == 3 || direction == 5) {
        std::int32_t x =
            platform::legacy_crypt_int::Subtract_401E60(&gPlayerX, 11);
        const std::int32_t y =
            platform::legacy_crypt_int::Add_401E10(&gPlayerY, 11);
        const std::int32_t span = gMapSpanX;
        const std::int32_t xBound = std::bit_cast<std::int32_t>(gValue9FB930);
        for (std::int32_t step = 0; step < 23; ++step, ++x) {
            if (x >= 0 && x < xBound
                && y >= 0 && y < std::bit_cast<std::int32_t>(gValueC821D4)
                && gMapHitArray[static_cast<std::size_t>(
                    31 * span + 19 + step)] == 0) {
                const std::size_t index = gValue9DAD56;
                gMapWindowStartY9A86C8[index] = static_cast<std::int16_t>(y);
                gMapWindowStartX9A86D0[index] = static_cast<std::int16_t>(x - 1);
                gMapWindowEndX9A86CC[index] =
                    static_cast<std::int16_t>(x - step + 23);
                gMapWindowEndY9A86C4[index] = static_cast<std::int16_t>(y + 2);
                static_cast<void>(Sub_468290(
                    &gMapWindowStartX9A86D0[index],
                    &gMapWindowStartY9A86C8[index],
                    &gMapWindowEndX9A86CC[index],
                    &gMapWindowEndY9A86C4[index]));
                ++gValue9DAD56;
                return 1;
            }
        }
    }
    return found;
}

// 原 [0x46AF30,0x46AFDE)：探测成功后逐项发送 UUN 并发布玩家快照。
std::uint32_t Sub_46AF30()
{
    const std::int32_t direction =
        std::bit_cast<std::int16_t>(gPendingDirections9A86D4[0]);
    if (Sub_469870(direction) == 0) {
        return 0;
    }

    std::int32_t index = 0;
    while (index < std::bit_cast<std::int16_t>(gValue9DAD56)) {
        const std::size_t position = static_cast<std::size_t>(index);
        const std::int32_t endY = gMapWindowEndY9A86C4[position];
        const std::int32_t endX = gMapWindowEndX9A86CC[position];
        const std::int32_t startY = gMapWindowStartY9A86C8[position];
        const std::int32_t startX = gMapWindowStartX9A86D0[position];
        const std::int32_t floor = gMapFloor;
        const std::int32_t mapId = std::bit_cast<std::int32_t>(gValue9A86DC);
        const std::uintptr_t socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_510710(
            socket, mapId, floor, startX, startY, endX, endY));
        ++index;
    }

    gValue9EE8B8 = 1;
    gValue9EE8A4 = static_cast<std::uint16_t>(direction);
    gValue9A7F7C = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    gValue9A7F80 = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::uint32_t tick = platform::ReadTickCount32();
    gValueAF10C8 = tick;
    return tick;
}

// 原 [0x468C40,0x468F25)：成功打开才清三平面，逐行短读保留已清零后缀。
std::int32_t __cdecl Sub_468C40(
    const std::int32_t mapId, const std::int32_t floor,
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    std::uint16_t* const tile, std::uint16_t* const obj,
    std::uint16_t* const hit)
{
    char path[256];
    Sub_468520(path, mapId, gValue9E0434, floor);
    std::FILE* const file = std::fopen(path, kMapReadMode);
    if (file == nullptr) {
        platform::PostLegacyQuitMessage(0);
        return -1;
    }

    std::memset(tile, 0, kMapBufferCells * sizeof(std::uint16_t));
    std::memset(obj, 0, kMapBufferCells * sizeof(std::uint16_t));
    std::memset(hit, 0, kMapBufferCells * sizeof(std::uint16_t));
    for (int i = 0; i < 4; ++i) {
        std::fgetc(file);
    }

    std::int32_t ignoredFirst = 0;
    std::int32_t ignoredSecond = 0;
    std::int32_t fileWidth = 0;
    std::int32_t fileHeight = 0;
    std::fread(&ignoredFirst, 4, 1, file);
    std::fread(&ignoredSecond, 4, 1, file);
    std::fread(&fileWidth, 4, 1, file);
    std::fread(&fileHeight, 4, 1, file);

    const std::int32_t destinationWidth = xEnd - xStart;
    const std::int32_t sourceX = std::max(xStart, 0);
    const std::int32_t destinationX = std::max(-xStart, 0);
    const std::int32_t sourceY = std::max(yStart, 0);
    const std::int32_t destinationY = std::max(-yStart, 0);
    const std::int32_t copyWidth = std::min(xEnd, fileWidth) - sourceX;
    const std::int32_t copyHeight = std::min(yEnd, fileHeight) - sourceY;
    const std::int32_t destinationStart =
        destinationX + destinationWidth * destinationY;
    if (copyHeight > 0) {
        std::uint16_t* const planes[] {tile, obj, hit};
        const std::int32_t planeBytes = 2 * fileWidth * fileHeight;
        for (std::int32_t plane = 0; plane < 3; ++plane) {
            for (std::int32_t row = 0; row < copyHeight; ++row) {
                const std::int32_t fileRow =
                    20 + plane * planeBytes
                    + 2 * ((sourceY + row) * fileWidth + sourceX);
                std::fseek(file, fileRow, SEEK_SET);
                const std::int32_t destinationRow =
                    destinationStart + row * destinationWidth;
                std::fread(
                    planes[plane] + destinationRow,
                    static_cast<std::size_t>(2 * copyWidth), 1, file);
            }
        }
    }
    std::fclose(file);
    return 0;
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

std::uint32_t gValue55DA88 = 1;

namespace {

// 原 55DA90/55DA94：保留可写索引及三组 S32 尺寸，不冻结为当前 54x54。
std::int16_t gAutoMapWindowIndex55DA90 = 1;
struct AutoMapWindowSize {
    std::int32_t width;
    std::int32_t height;
};
AutoMapWindowSize gAutoMapWindowSizes55DA94[3] {{107, 107}, {54, 54}, {27, 27}};

// 原 46C840 的清零字节数；与 51x51 世界地图缓存独立。
constexpr std::size_t kAutoMapWindowCells = 11664;
std::uint32_t gAutoMapColorWindow9CF6EC[kAutoMapWindowCells] {};
std::uint8_t gAutoMapIndexWindow9DAD7C[kAutoMapWindowCells] {};
static_assert(sizeof(AutoMapWindowSize) == 8);
static_assert(sizeof(gAutoMapColorWindow9CF6EC) == 46656);
static_assert(sizeof(gAutoMapIndexWindow9DAD7C) == 11664);

} // namespace

std::int32_t Sub_46C840(
    const std::int32_t floor, const std::int32_t playerX, const std::int32_t playerY)
{
    const std::int32_t mapId = std::bit_cast<std::int32_t>(gValue9A86DC);
    std::memset(gAutoMapIndexWindow9DAD7C, 0, sizeof(gAutoMapIndexWindow9DAD7C));
    std::memset(gAutoMapColorWindow9CF6EC, 0, sizeof(gAutoMapColorWindow9CF6EC));
    char path[256];
    Sub_468520(path, mapId, gValue9E0434, floor);
    std::FILE* const file = std::fopen(path, kMapReadMode);
    if (file == nullptr) {
        return 0;
    }

    std::array<std::uint16_t, kAutoMapWindowCells> tile {};
    std::array<std::uint16_t, kAutoMapWindowCells> object {};
    std::array<std::uint16_t, kAutoMapWindowCells> hit {};
    std::fseek(file, 12, SEEK_SET);
    // 原头读取不检查返回，未写字节不套用其他地图函数的补零特例。
    std::int32_t fileWidth;
    std::int32_t fileHeight;
    std::fread(&fileWidth, 4, 1, file);
    std::fread(&fileHeight, 4, 1, file);

    const std::int32_t sizeIndex = gAutoMapWindowIndex55DA90;
    const std::int32_t windowWidth = gAutoMapWindowSizes55DA94[sizeIndex].width;
    const std::int32_t windowHeight = gAutoMapWindowSizes55DA94[sizeIndex].height;
    const std::int32_t startX = playerX - windowWidth / 2;
    const std::int32_t startY = playerY - windowHeight / 2;
    const std::int32_t endX = startX + windowWidth;
    const std::int32_t endY = startY + windowHeight;
    std::int32_t sourceX = startX;
    std::int32_t sourceY = startY;
    std::int32_t copyWidth = windowWidth;
    std::int32_t copyHeight = windowHeight;
    std::int32_t destinationX = 0;
    std::int32_t destinationY = 0;
    if (startX < 0) {
        copyWidth += startX;
        sourceX = 0;
        destinationX = -startX;
    }
    if (endX > fileWidth) {
        copyWidth += fileWidth - endX;
    }
    if (startY < 0) {
        sourceY = 0;
        copyHeight += startY;
        destinationY = -startY;
    }
    if (endY > fileHeight) {
        copyHeight += fileHeight - endY;
    }
    const std::int32_t destinationStart = destinationX + windowWidth * destinationY;
    std::uint16_t* const planes[] {tile.data(), object.data(), hit.data()};
    std::int32_t planeOffset = 20;
    for (std::int32_t plane = 0; plane < 3; ++plane) {
        std::int32_t sourceCell = sourceX + fileWidth * sourceY;
        if (copyHeight > 0) {
            std::uint16_t* target = planes[plane] + destinationStart;
            for (std::int32_t row = 0; row < copyHeight; ++row) {
                std::fseek(file, planeOffset + 2 * sourceCell, SEEK_SET);
                std::fread(target,
                    static_cast<std::size_t>(static_cast<std::uint32_t>(2 * copyWidth)), 1, file);
                sourceCell += fileWidth;
                target += windowWidth;
            }
        }
        if (plane < 2) {
            planeOffset += 2 * fileWidth * fileHeight;
        }
    }
    std::fclose(file);

    if (gValue9DAD68 == 1) {
        const std::int32_t index = gAutoMapWindowIndex55DA90;
        const std::int32_t height = gAutoMapWindowSizes55DA94[index].height;
        if (height > 0) {
            const std::int32_t width = gAutoMapWindowSizes55DA94[index].width;
            std::int32_t autRow = startX + fileWidth * startY;
            const std::uint16_t* flags = hit.data();
            std::uint32_t* output = gAutoMapColorWindow9CF6EC;
            for (std::int32_t row = 0; row < height; ++row) {
                for (std::int32_t column = 0; column < width; ++column) {
                    if ((flags[column] & 0x4000u) != 0) {
                        const AutRecordAF10D8& color = gAutDataAF10D8[autRow + column];
                        const std::uint32_t blue = color.value02;
                        const std::uint32_t green = color.value01;
                        const std::uint32_t red = color.value00;
                        output[column] = red | (green << 8) | (blue << 16);
                    }
                }
                output += width;
                flags += width;
                autRow += fileWidth;
            }
        }
        return 1;
    }

    const std::uint8_t* const colors = resource::BorrowAutoMapColors();
    std::int16_t nextIndex = gAutoMapWindowIndex55DA90;
    std::int32_t currentIndex = nextIndex;
    const std::int32_t tileHeight = gAutoMapWindowSizes55DA94[currentIndex].height;
    if (tileHeight > 0) {
        const std::int32_t width = gAutoMapWindowSizes55DA94[currentIndex].width;
        std::int32_t cell = 0;
        for (std::int32_t row = 0; row < tileHeight; ++row) {
            for (std::int32_t column = 0; column < width; ++column) {
                if ((hit[cell + column] & 0x4000u) != 0) {
                    gAutoMapIndexWindow9DAD7C[cell + column] = colors[tile[cell + column]];
                }
            }
            cell += width;
        }
        nextIndex = gAutoMapWindowIndex55DA90;
    }

    std::int32_t row = 0;
    std::int32_t height = gAutoMapWindowSizes55DA94[currentIndex].height;
    while (row < height) {
        std::int32_t width = gAutoMapWindowSizes55DA94[currentIndex].width;
        std::int32_t column = 0;
        while (column < width) {
            const std::int32_t cell = row * width + column;
            if ((hit[cell] & 0x4000u) != 0) {
                const std::uint16_t mapNo = object[cell];
                const std::uint8_t color = colors[mapNo];
                if (color != 0) {
                    std::int32_t graphicIndex;
                    resource::GetGraphicMapNoIndex(mapNo, &graphicIndex);
                    std::uint16_t flag;
                    resource::GetGraphicFlag1(std::bit_cast<std::uint32_t>(graphicIndex), &flag);
                    if (flag != 0) {
                        const std::int32_t currentWidth =
                            gAutoMapWindowSizes55DA94[gAutoMapWindowIndex55DA90].width;
                        gAutoMapIndexWindow9DAD7C[row * currentWidth + column] = color;
                    } else {
                        std::uint16_t sx;
                        std::uint16_t sy;
                        resource::GetGraphicSxSy(std::bit_cast<std::uint32_t>(graphicIndex), &sx, &sy);
                        const std::int32_t spanY = std::bit_cast<std::int16_t>(sy);
                        if (spanY > 0) {
                            const std::int32_t spanX = std::bit_cast<std::int16_t>(sx);
                            std::int32_t targetRow = row;
                            for (std::int32_t offsetY = 0; offsetY < spanY; ++offsetY) {
                                for (std::int32_t offsetX = 0; offsetX < spanX; ++offsetX) {
                                    if (targetRow >= 0) {
                                        const std::int32_t currentWidth =
                                            gAutoMapWindowSizes55DA94[gAutoMapWindowIndex55DA90].width;
                                        if (column + offsetX < currentWidth) {
                                            gAutoMapIndexWindow9DAD7C[
                                                targetRow * currentWidth + column + offsetX] = color;
                                        }
                                    }
                                }
                                --targetRow;
                            }
                        }
                    }
                }
            }
            nextIndex = gAutoMapWindowIndex55DA90;
            ++column;
            width = gAutoMapWindowSizes55DA94[nextIndex].width;
        }
        currentIndex = nextIndex;
        ++row;
        height = gAutoMapWindowSizes55DA94[currentIndex].height;
    }
    return 1;
}

std::int32_t Sub_46C760(const std::int32_t x, const std::int32_t y)
{
    if (gValue9DAD68 == 1) {
        gValue55DA88 = 1;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
        static_cast<void>(Sub_46C840(gMapFloor, playerX, playerY));
        const std::int32_t index = gAutoMapWindowIndex55DA90;
        gValue55DA88 = 0;
        const std::int32_t height = gAutoMapWindowSizes55DA94[index].height;
        const std::int32_t width = gAutoMapWindowSizes55DA94[index].width;
        return render::DrawColorAutoMap_44FEB0(x, y, gAutoMapColorWindow9CF6EC, width, height);
    }
    if (gValue55DA88 != 0) {
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
        static_cast<void>(Sub_46C840(gMapFloor, playerX, playerY));
        gValue55DA88 = 0;
    }
    const std::int32_t index = gAutoMapWindowIndex55DA90;
    const std::int32_t height = gAutoMapWindowSizes55DA94[index].height;
    const std::int32_t width = gAutoMapWindowSizes55DA94[index].width;
    return render::DrawIndexedAutoMap_44F5C0(x, y, gAutoMapIndexWindow9DAD7C, width, height);
}

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

// 原 [0x469810,0x469864)：边界外返回 1，边界内比较第四平面的完整 WORD。
std::int32_t __cdecl Sub_469810(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t localX = x - gMapXStart;
    const std::int32_t localY = y - gMapYStart;
    if (localX < 0) {
        return 1;
    }
    const std::int32_t width = gMapSpanX;
    if (localX >= width || localY < 0) {
        return 1;
    }
    const std::int32_t height = gMapSpanY;
    if (localY >= height) {
        return 1;
    }
    const std::int32_t index = localX + width * localY;
    return gMapWalkabilityArray[static_cast<std::size_t>(index)] == 1;
}

// 原 [0x469240,0x469284)：按字节读取，两张原表查值并保留低 W16 状态。
std::int32_t __cdecl Sub_469240(
    const std::uint8_t* const bytes, const std::int32_t length)
{
    std::uint16_t state = 0;
    for (std::int32_t i = 0; i < length; ++i) {
        std::uint16_t next = kMapBitReverse55DAAC[bytes[i]];
        next ^= kMapCrcTable55DBAC[state >> 8];
        next ^= static_cast<std::uint16_t>((state & 0xFFu) << 8);
        state = next;
    }
    return state;
}

// 原 [0x469290,0x4693E9)：装载失败仍校验局部平面并可能发送 UUN。
std::int32_t __cdecl Sub_469290(
    const std::int32_t first, const std::int32_t floor,
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    const std::int32_t expectedTile, const std::int32_t expectedObj,
    const std::int32_t expectedHit)
{
    std::array<std::uint16_t, kMapBufferCells + 1> tile {};
    std::array<std::uint16_t, kMapBufferCells + 1> obj {};
    std::array<std::uint16_t, kMapBufferCells + 1> hit {};
    const std::int32_t width = xEnd - xStart;
    const std::int32_t height = yEnd - yStart;
    static_cast<void>(Sub_468C40(
        std::bit_cast<std::int32_t>(gValue9A86DC),
        floor, xStart, yStart, xEnd, yEnd,
        tile.data(), obj.data(), hit.data()));

    if (height > 0) {
        const std::uint16_t mask = gValue9DAD68 == 1 ? 0x01FFu : 0x0FFFu;
        std::int32_t rowStart = 0;
        for (std::int32_t row = 0; row < height; ++row) {
            for (std::int32_t column = 0; column < width; ++column) {
                hit[static_cast<std::size_t>(rowStart + column)] &= mask;
            }
            rowStart += width;
        }
    }

    constexpr std::int32_t kCheckedBytes = 0x5B2;
    const std::int32_t tileCheck = Sub_469240(
        reinterpret_cast<const std::uint8_t*>(tile.data()), kCheckedBytes);
    const std::int32_t objCheck = Sub_469240(
        reinterpret_cast<const std::uint8_t*>(obj.data()), kCheckedBytes);
    const std::int32_t hitCheck = Sub_469240(
        reinterpret_cast<const std::uint8_t*>(hit.data()), kCheckedBytes);
    if (expectedTile == tileCheck && expectedObj == objCheck
        && expectedHit == hitCheck) {
        return 1;
    }
    static_cast<void>(network::Sub_510710(
        platform::gConnectionSocket,
        first, floor, xStart, yStart, xEnd, yEnd));
    return 0;
}

// 原 [0x4691D0,0x469239)：两个 -1 临时对象各自构造、复制、析构。
void __cdecl Sub_4691D0()
{
    platform::LegacyCryptInt temporary;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, -1);
    gValue9EE8D0.mask = temporary.mask;
    gValue9EE8D0.encoded = temporary.encoded;
    gValue9EE8D0.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, -1);
    gValue9EE8E0.mask = temporary.mask;
    gValue9EE8E0.encoded = temporary.encoded;
    gValue9EE8E0.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
}

void Sub_5093C0(
    const std::uintptr_t, const std::int32_t x, const std::int32_t y,
    const std::int32_t direction)
{
    static_cast<void>(Sub_468200());
    static_cast<void>(Sub_519EC0(x, y));
    Sub_519EE0();
    static_cast<void>(Sub_51A040(direction));
}

void Sub_50D080(
    const std::uintptr_t, const std::int32_t first, const std::int32_t second)
{
    platform::LegacyCryptInt temporary;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, first);
    gValue9FB918.encoded = temporary.encoded;
    gValue9FB918.mask = temporary.mask;
    gValue9FB918.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, second);
    gValueAF0FE8.readCount = temporary.readCount;
    gValueAF0FE8.encoded = temporary.encoded;
    gValueAF0FE8.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    const std::uint32_t encoded = gValue9FB918.encoded;
    const std::uint32_t mask = gValue9FB918.mask;
    const std::uint8_t readCount = gValue9FB918.readCount;
    gValueC821E8.encoded = encoded;
    gValueC821E8.mask = mask;
    gValueC821E8.readCount = readCount;
}

void Sub_455C60()
{
    gValue62B5B0 = 0;
}

void Sub_455FC0()
{
    const std::uint32_t value = gValue62B5B0;
    gValue62B398 = 0;
    if (value == 0) {
        login::YobiAction* const action = gWindow39Action62B5B8;
        if (action != nullptr) {
            static_cast<void>(login::detail::MarkDeath(action));
            gWindow39Action62B5B8 = nullptr;
        }
        return;
    }
    if (gValueCAFEA0 != 0) {
        static_cast<void>(audio::Sub_504180(1));
        gValueCAFEA0 = 0;
        gValue62B5B0 = 0;
        return;
    }
    if (ui::detail::gWindowStates[39].window == nullptr
        && gWindow39Action62B5B8 == nullptr) {
        gWindow39Action62B5B8 = ui::CreateWindowAnimation_475AA0(
            39, 0, 0, &gWindow39Action62B5B8);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
    }
}

void Sub_50D120(
    const std::uintptr_t, const std::int32_t type, const std::int32_t imageNo)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (gValueD67E78 != 0) {
        if (gValueCAFEA0 == 0) {
            gValueCAFEA0 = static_cast<std::uint32_t>(audio::Sub_504410(1));
        }
        static_cast<void>(Sub_519D50());
        Sub_4464D0();
        static_cast<void>(Sub_4681B0());
        Sub_51B0F0();
        gValue62B5B0 = 0;
        Sub_455FC0();
        Sub_455C60();

        const std::uint32_t mask = gValue9FB918.mask;
        const std::uint32_t encoded = gValue9FB918.encoded;
        const std::uint8_t readCount = gValue9FB918.readCount;
        gValueC821E8.mask = mask;
        gValueC821E8.encoded = encoded;
        gValueC821E8.readCount = readCount;

        platform::LegacyCryptInt temporary;
        platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, 0);
        gValueC821D8.mask = temporary.mask;
        gValueC821D8.encoded = temporary.encoded;
        gValueC821D8.readCount = temporary.readCount;
        platform::legacy_crypt_int::Destroy_401CF0(&temporary);

        platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, 1);
        gRippleGate.mask = temporary.mask;
        gRippleGate.encoded = temporary.encoded;
        gRippleGate.readCount = temporary.readCount;
        platform::legacy_crypt_int::Destroy_401CF0(&temporary);

        gValue9FB92E = 0;
        gValueFF03D8 = 0;
        gValueF7CA04 = 0;
        static_cast<void>(SetCurrentSceneTypeAndState(9, 1));
        login::detail::YobiClearUp_404D30();
        Sub_446610();

        platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, 0);
        gRippleGate.mask = temporary.mask;
        gRippleGate.encoded = temporary.encoded;
        gRippleGate.readCount = temporary.readCount;
        platform::legacy_crypt_int::Destroy_401CF0(&temporary);
        static_cast<void>(network::Sub_5108F0(platform::gConnectionSocket, 0));
        return;
    }
    if (type > 0) {
        if (gValueFF0D36 != 0 && (ReadPlayerFlags18A() & 0x100u) == 0
            && gValueCAFEA0 == 0) {
            gValueCAFEA0 = static_cast<std::uint32_t>(audio::Sub_504410(1));
        }
        gValue5ABFC4 = 1;
        gBattleType5ABFC8 = type;
        gBattleModeFlag5ABFCC = type == 2 ? 1u : 0u;
        gValueC821D0 = type == 3 ? 1 : 0;
        gValueB65A7C = type == 2 || type == 4 || type == 5 || type == 6 ? 1 : 0;
        gValue5ABB1C = 0;
        gBattleBackgroundImage59F9F8 = imageNo;
        gValue5ABB28 = 0;
        gBattleCWriteIndex5AAA6C = 0;
        gBattleCReadIndex5AAA44 = 0;
        gBattleMWriteIndex5A3A1C = 0;
        gBattleMReadIndex5AAA68 = 0;
        gBattleCText5A3A20[0] = '\0';
        gBattleMText5AAA7C[0] = '\0';
        for (std::size_t offset = 0; offset < 0x4000; offset += 0x1000) {
            gBattleCQueue5A4A3C[offset] = '\0';
            gBattleMQueue59FA18[offset] = '\0';
        }
        gBattleFirstTurnPending5ABC6C = 1;
        return;
    }
    platform::LegacyCryptInt temporary;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, 0);
    gValueC821D8.readCount = temporary.readCount;
    gValueC821D8.encoded = temporary.encoded;
    gValueC821D8.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    gValueFF03D8 = 0;
    gValueF7CA04 = 0;
}

void Sub_50D3C0(const std::uintptr_t, const char* const text)
{
    const char kind = text[0];
    if (kind == 'C') {
        const std::int32_t index = gBattleCWriteIndex5AAA6C;
        char* const destination = gBattleCQueue5A4A3C.data()
            + static_cast<std::ptrdiff_t>(index) * 0x1000;
        std::memcpy(destination, text, std::strlen(text) + 1);
        const std::int32_t next = index + 1;
        gBattleCWriteIndex5AAA6C = next;
        if (next >= 4) {
            gBattleCWriteIndex5AAA6C = 0;
        }
        return;
    }
    if (kind == 'P') {
        static_cast<void>(std::sscanf(text + 2, "%X|%X|%X|%X|%X|%X",
            &gBattlePosition5ABCA8, &gBattlePlayerFlags5ABCAC,
            &gBattleSkillFlags5ABC04, &gBattlePetSkillFlags5ABB78,
            &gBattleRebirthLevel5ABB7C, &gBattleRidePet5ABC38));
        return;
    }
    if (kind == 'A') {
        static_cast<void>(std::sscanf(text + 2, "%X|%X",
            &gBattleEndBits5ABBC0, &gBattleTurn5ABB74));
        if (gBattleFirstTurnPending5ABC6C == 1) {
            const std::uint32_t turn = gBattleTurn5ABB74;
            gBattleFirstTurnPending5ABC6C = 0;
            gBattleTurnSnapshot5ABC08 = turn;
        }
        login::detail::Sub_43BFF0();
        return;
    }
    if (text[1] == 'U') {
        gValue5ABCB0 = 1;
        return;
    }
    if (kind == 'M') {
        const std::int32_t index = gBattleMWriteIndex5A3A1C;
        char* const destination = gBattleMQueue59FA18.data()
            + static_cast<std::ptrdiff_t>(index) * 0x1000;
        std::memcpy(destination, text, std::strlen(text) + 1);
        const std::int32_t next = index + 1;
        gBattleMWriteIndex5A3A1C = next;
        if (next >= 4) {
            gBattleMWriteIndex5A3A1C = 0;
        }
    }
}

void Sub_50D500(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    gValueFF0C10 = 1;
    char row[2048];
    char entry[2048];
    char name[2048];
    std::int32_t field = 1;
    for (BattleResultRecord& record : gBattleResultsFF0C14) {
        network::ExtractDelimitedField_52CED0(payload, ',', field, 2047, row);
        record.value00 = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(row, '|', 1));
        record.value02 = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(row, '|', 2));
        record.value04 = network::ParseDelimitedBase62_52D0D0(row, '|', 3);
        ++field;
    }

    network::ExtractDelimitedField_52CED0(payload, ',', field, 2047, row);
    ++field;
    for (std::int32_t index = 0; index < 3; ++index) {
        network::ExtractDelimitedField_52CED0(row, '|', index + 1, 2047, entry);
        input::UnescapeLegacyText_52D150(entry);
        gBattleResultGraphicsFF0D0C[index] =
            network::ParseDelimitedInteger_52CFA0(entry, '|', 1);
        network::ExtractDelimitedField_52CED0(entry, '|', 2, 2047, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) <= 28) {
            std::strcpy(gBattleResultNamesFF0C34[index].data(), name);
        } else {
            gBattleResultNamesFF0C34[index][0] = '\0';
        }
    }

    for (BattleSkillResultRecord& record : gBattleSkillResultsFF0C8C) {
        network::ExtractDelimitedField_52CED0(payload, ',', field, 2047, row);
        const auto bits = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(row, '|', 1));
        record.value00 = std::bit_cast<std::int16_t>(bits);
        if (record.value00 < 0) {
            record.value00 = 0;
        }
        record.value04 = network::ParseDelimitedBase62_52D0D0(row, '|', 2);
        if (record.value04 < 0) {
            record.value04 = 0;
        }
        ++field;
    }
}

// 原 [0x50CF90,0x50CFF7)：回复序号按 S16 比较，成功时清地图请求状态。
void __cdecl Sub_50CF90(
    const std::uintptr_t, const std::int32_t sequence,
    const std::int32_t status)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (std::bit_cast<std::int16_t>(gValue9DAD54) == sequence) {
        gValue9A86D8 = 0;
        if (status == 0) {
            Sub_4691D0();
            gValueC82208 = 0;
            gValueC82210 = 0;
        } else if (gValueC82214 == 0) {
            gValueC82210 = 0;
        }
    } else if (std::bit_cast<std::int16_t>(gValue9FB92C) == sequence &&
               status == 0) {
        gValue9FB92E = 0;
    }
}

// 原 [0x509090,0x5090FC)：校验成功才清地图请求及一次性状态位。
void __cdecl Sub_509090(
    const std::uintptr_t, const std::int32_t first,
    const std::int32_t floor, const std::int32_t xStart,
    const std::int32_t yStart, const std::int32_t xEnd,
    const std::int32_t yEnd, const std::int32_t expectedTile,
    const std::int32_t expectedObj, const std::int32_t expectedHit)
{
    if (gValue9A6E0C != 0 ||
        Sub_469290(
            first, floor, xStart, yStart, xEnd, yEnd,
            expectedTile, expectedObj, expectedHit) == 0) {
        return;
    }
    if (gMapFloor == floor) {
        gValueC82208 = 0;
    }
    if ((gValue9A4B94 & 1u) != 0) {
        Sub_4691D0();
        gValue9A4B94 &= 0xFEu;
    }
}

// 原 [0x509100,0x5093B6)：同一局部帧中的 WORD 写入可覆盖后续原文。
std::int32_t __cdecl Sub_509100(
    const std::uintptr_t, const std::int32_t mapId,
    const std::int32_t floor, const std::int32_t xStart,
    const std::int32_t yStart, const std::int32_t xEnd,
    const std::int32_t yEnd, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return std::bit_cast<std::int32_t>(gValue9A6E0C);
    }

    constexpr std::size_t frameBytes = 0x10594;
    constexpr std::size_t tileOffset = 0x64;
    constexpr std::size_t tileTextOffset = 0x1064;
    constexpr std::size_t objOffset = 0x5774;
    constexpr std::size_t objTextOffset = 0x6774;
    constexpr std::size_t hitOffset = 0xAE84;
    constexpr std::size_t hitTextOffset = 0xBE84;
    static_assert(std::endian::native == std::endian::little);
    // 末尾的零 WORD 只为越帧的异常文本扫描提供终止字节。
    std::array<std::uint16_t, frameBytes / 2 + 1> frame {};
    char* const bytes = reinterpret_cast<char*>(frame.data());
    std::uint16_t* const tile = frame.data() + tileOffset / 2;
    std::uint16_t* const obj = frame.data() + objOffset / 2;
    std::uint16_t* const hit = frame.data() + hitOffset / 2;
    char* const tileText = bytes + tileTextOffset;
    char* const objText = bytes + objTextOffset;
    char* const hitText = bytes + hitTextOffset;
    char* const field = bytes;

    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 1, 0x470F, tileText));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 2, 0x470F, objText));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 3, 0x470F, hitText));

    const std::int32_t firstStatus = network::ExtractDelimitedField_52CED0(
        tileText, ',', 1, 99, field);
    tile[0] = static_cast<std::uint16_t>(network::Base62Decode_52D050(field));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        objText, ',', 1, 99, field));
    obj[0] = static_cast<std::uint16_t>(network::Base62Decode_52D050(field));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        hitText, ',', 1, 99, field));
    hit[0] = static_cast<std::uint16_t>(network::Base62Decode_52D050(field));

    if (firstStatus != 1) {
        std::size_t item = 1;
        std::int32_t ordinal = 2;
        std::int32_t status;
        do {
            // 第三段的末尾是整帧边界；越过它不模拟原 x86 栈外写。
            if (item >= (frameBytes - hitOffset) / 2) {
                return -1;
            }
            status = network::ExtractDelimitedField_52CED0(
                tileText, ',', ordinal, 99, field);
            tile[item] = static_cast<std::uint16_t>(
                network::Base62Decode_52D050(field));
            static_cast<void>(network::ExtractDelimitedField_52CED0(
                objText, ',', ordinal, 99, field));
            obj[item] = static_cast<std::uint16_t>(
                network::Base62Decode_52D050(field));
            static_cast<void>(network::ExtractDelimitedField_52CED0(
                hitText, ',', ordinal, 99, field));
            hit[item] = static_cast<std::uint16_t>(
                network::Base62Decode_52D050(field));
            ++item;
            ++ordinal;
        } while (status != 1);
    }

    static_cast<void>(Sub_4688A0(
        mapId, floor, xStart, yStart, xEnd, yEnd, tile, obj, hit));
    if ((gValue9EE8B8 != 0 || gValueC82208 != 0)
        && gMapFloor == floor) {
        Sub_4691D0();
        gValueC82208 = 0;
    }
    gValue9A4B94 &= 0xFEu;

    if (std::bit_cast<std::int32_t>(gValue9A86DC) == mapId
        && gMapFloor == floor
        && platform::legacy_crypt_int::Subtract_401E60(&gPlayerX, 10) <= xStart
        && xEnd <= platform::legacy_crypt_int::Add_401E10(&gPlayerX, 10)
        && platform::legacy_crypt_int::Subtract_401E60(&gPlayerY, 10) <= yStart
        && yEnd <= platform::legacy_crypt_int::Add_401E10(&gPlayerY, 10)) {
        gValue9EE910 = 1;
    }

    const std::uint32_t state = gValueC82214;
    if (state != 1) {
        return std::bit_cast<std::int32_t>(state);
    }
    const std::int32_t currentFloor = gMapFloor;
    if (currentFloor != floor) {
        return currentFloor;
    }
    gValueC82210 = 0;
    gValueC82214 = 0;
    gValueC82218 = 0;
    return 0;
}

// 原 [0x5093F0,0x509548)：保留地图保存、场景清理和加密对象赋值顺序。
std::int32_t __cdecl Sub_5093F0(
    const std::uintptr_t, const std::int32_t a2, const std::int32_t a3,
    const std::int32_t a4, const std::int32_t a5, const std::int32_t a6,
    const std::int32_t a7, const std::int32_t a8, const std::int32_t a9,
    const std::int32_t a10, const std::int32_t a11, const std::int32_t a12,
    const std::int32_t a13)
{
    const std::uint8_t priorFlag = gValue9A4B94;
    gValueC82208 = 1;
    if (priorFlag == 0 && gCurrentSceneType == 9) {
        if (gValueC8220C == 0) {
            if (gValueCAFEA0 == 0) {
                gValueCAFEA0 =
                    static_cast<std::uint32_t>(audio::Sub_504410(1));
            }
            gCurrentSceneState = 200;
            static_cast<void>(Sub_51D5B0());
        }
        static_cast<void>(Sub_519D50());
        gValueC8220C = 0;
    }

    static_cast<void>(Sub_468570(
        gValue9E0434, a2, a3, a4, a5, a8, a9));
    Sub_4682E0(
        static_cast<std::uint32_t>(a2), a3,
        static_cast<std::uint32_t>(a4), static_cast<std::uint32_t>(a5),
        static_cast<std::uint32_t>(a8), static_cast<std::uint32_t>(a9),
        static_cast<std::uint32_t>(a10), a12);
    static_cast<void>(Sub_468330(a6, a7));
    Sub_4464D0();

    const std::uint32_t mask = gValue9FB918.mask;
    const std::uint32_t encoded = gValue9FB918.encoded;
    const std::uint8_t readCount = gValue9FB918.readCount;
    gValueC821E8.mask = mask;
    gValue9EE8B8 = 0;
    gValueC821E8.encoded = encoded;
    gValueC821E8.readCount = readCount;

    platform::LegacyCryptInt temporary;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, 0);
    gValue9E03C0.mask = temporary.mask;
    gValue9E03C0.encoded = temporary.encoded;
    gValue9E03C0.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    static_cast<void>(Sub_4681B0());

    if (a11 > 0) {
        gValueFF0D5C = static_cast<std::uint32_t>(a11);
        gValueFF0D58 = 1;
    }
    gValue9EE8F8 = static_cast<std::uint32_t>(a13);
    return a11 > 0 ? a13 : a11;
}

// 原 [0x509550,0x509642)：原位反转义、两段提取与地图回复状态发布。
std::int32_t __cdecl Sub_509550(
    const std::uintptr_t, char* const payload)
{
    std::array<char, 256> text;
    std::array<char, 32> number;
    input::UnescapeLegacyText_52D150(payload);
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 1, 255, text.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(text.data());

    const char* const selected =
        std::strlen(text.data()) > 24 ? kUnknownText554590 : text.data();
    std::memcpy(gText9DAD30.data(), selected, std::strlen(selected) + 1);
    gValueFF0D4C = -2;

    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 2, 31, number.data()));
    if (number[0] == '\0') {
        gValueFF0D4C = -1;
    } else {
        gValueFF0D4C = std::atoi(number.data());
        if (gValueFF0D4C < 0) {
            gValueFF0D4C = -1;
        }
    }
    if (gValueC82210 == 1) {
        gValueC82218 = 1;
    }
    return 1;
}

// 原 [51D640,51D6E3)：恢复 C82204 后拷贝整面，queueMode 固定写回 1。
std::uint32_t Sub_51D640()
{
    static_cast<void>(Sub_468F70());
    platform::gPendingGraphicCount = 0;
    platform::gPendingGraphicCount2 = 0;
    static_cast<void>(render::ResetGraphicCache_456D30());
    static_cast<void>(Sub_469030());
    render::SortTextures_520790();
    static_cast<void>(render::ClearSurface_4512C0());
    const std::uint8_t previousC82204 = gValueC82204;
    gValueC82204 = 0;
    render::gRenderQueueMode103CF10 = 0;
    render::DrawAllQueued_520DD0();
    gValueC82204 = previousC82204;
    static_cast<void>(render::CopyBackToTemp());
    render::gRenderQueueMode103CF10 = 1;
    platform::gPendingGraphicCount = 0;
    platform::gPendingGraphicCount2 = 0;
    static_cast<void>(render::ResetGraphicCache_456D30());
    const std::uint32_t tick = platform::ReadTickCount32();
    platform::gLastRenderDt = static_cast<double>(tick);
    return tick;
}

// 原 [0x51D750,0x51D781)：复制结果不影响后续随机调用和完整 W32 写入。
std::int8_t Sub_51D750()
{
    static_cast<void>(render::CopyBackToTemp());
    const std::uint32_t bits = static_cast<std::uint32_t>(
        platform::RandomRange_46FAF0(0, 35));
    const std::uint8_t low = static_cast<std::uint8_t>(bits) & 0xFEu;
    render::gValueFF48E4 = (bits & 0xFFFFFF00u) | low;
    return static_cast<std::int8_t>(low);
}

// 原 [0x51D6F0,0x51D743)：只消费 -1 与非负回复，其他负值保持原状态。
void __cdecl Sub_51D6F0()
{
    const std::int32_t value = gValueFF0D4C;
    if (value == -1) {
        static_cast<void>(render::SelectPalette(gValue73ED48, 0));
        gValue73ED44 = 1;
        gValueFF0D4C = -2;
    } else if (value >= 0) {
        static_cast<void>(render::SelectPalette(value, 0));
        gValue73ED44 = 0;
        gValueFF0D4C = -2;
    }
}

void __cdecl Sub_458F40()
{
    const std::int32_t scene = gCurrentSceneType;
    if (scene == 9) {
        if (gPlayerRuntime.action == nullptr) {
            return;
        }
    } else if (scene != 10) {
        return;
    }
    const std::uint32_t tick = platform::ReadTickCount32();
    if (gPlayerRuntime.timerMode10 != 0) {
        const std::int32_t delta =
            std::bit_cast<std::int32_t>(gPlayerRuntime.timerTick14 - tick);
        gPlayerRuntime.timerRemaining0C += delta;
        if (gPlayerRuntime.timerRemaining0C < 0) {
            gPlayerRuntime.timerRemaining0C = 0;
            gPlayerRuntime.timerMode10 = 0;
        }
    }
    gPlayerRuntime.timerTick14 = tick;
}

// 原 [0x458F00,0x458F3A)：类别变化后可选调色板过渡，末尾总是进入 458F40。
void __cdecl Sub_458F00()
{
    const std::int32_t category = GetCate_458EC0(&gCgTime);
    if (gValue73ED48 != category) {
        const std::int32_t mode = gValue73ED44;
        gValue73ED48 = category;
        if (mode == 1) {
            static_cast<void>(render::SelectPalette(category, 3600));
        }
    }
    Sub_458F40();
}

// 原 519EB0：写入玩家首块的唯一 W32 对象号槽并原样返回。
std::int32_t __cdecl Sub_519EB0(const std::int32_t value)
{
    gPlayerRuntime.objectIndex = std::bit_cast<std::uint32_t>(value);
    return value;
}

// 原 [0x50C870,0x50C8C8)：两次独立取时，调色板后才发布路径与调整值。
std::int32_t __cdecl Sub_50C870(
    const std::uintptr_t, const std::int32_t playerValue,
    const std::int32_t timeOffset, const std::int32_t pathValue,
    const std::int32_t adjustment)
{
    static_cast<void>(Sub_519EB0(playerValue));
    gValue73ED4C = std::bit_cast<std::uint32_t>(timeOffset);
    gValue73ED68 = std::bit_cast<std::uint32_t>(
        platform::ReadLegacyTime32_53BA26(nullptr));
    static_cast<void>(UpdateGameTime(&gCgTime));
    gValue73ED48 = GetCate_458EC0(&gCgTime);
    static_cast<void>(render::SelectPalette(gValue73ED48, 0));
    gValue9E0434 = pathValue;
    gValue73ED40 = std::bit_cast<std::uint32_t>(adjustment);
    return adjustment;
}

void* __cdecl Sub_51D5B0()
{
    const std::uint16_t current = gMapCharaSceneSlots.valueC8A73E;
    const std::uint16_t low = static_cast<std::uint16_t>(
        gMapCharaSceneSlots.valueC8A744);
    gMapCharaSceneSlots.valueC8A740 = current;
    const std::uint32_t gate = gValueD67E78;
    gMapCharaSceneSlots.valueC8A744 =
        (gMapCharaSceneSlots.valueC8A744 & 0x0000FFFFu)
        | (static_cast<std::uint32_t>(low) << 16);

    if (gate != 0
        || login::detail::Sub_43F1F0(ReadPlayerObjectIndex_FDA880())
            == nullptr) {
        gValueFF0D74 = 0;
        if (gValueD67E78 != 0) {
            Sub_51BEF0();
        }
    } else if (gValueFF0D74 == 0) {
        gCurrentSceneState = 250;
        static_cast<void>(login::detail::Sub_440F50());
        ++gValueFF0D74;
    } else {
        const std::uint32_t count = gValueFF0D74;
        gCurrentSceneState = 252;
        gValueFF0D74 = count + 1u;
    }
    return login::detail::Sub_43F230();
}

// 原 [0x469030,0x4691C9)：装载后可选重建第四平面，随后斜向扫描 tile。
std::int32_t __cdecl Sub_469030()
{
    const std::int32_t loadYEnd = gMapYEnd;
    const std::int32_t loadXEnd = gMapXEnd;
    const std::int32_t loadYStart = gMapYStart;
    const std::int32_t loadXStart = gMapXStart;
    const std::int32_t loadFloor = gMapFloor;
    const std::int32_t loadMapId = std::bit_cast<std::int32_t>(gValue9A86DC);
    const std::int32_t loadResult = Sub_468C40(
        loadMapId, loadFloor, loadXStart, loadYStart, loadXEnd, loadYEnd,
        gMapTileArray.data(), gMapObjArray.data(), gMapHitArray.data());
    if (loadResult >= 0) {
        const std::int32_t walkYEnd = gMapYEnd;
        const std::int32_t walkXEnd = gMapXEnd;
        const std::int32_t walkYStart = gMapYStart;
        const std::int32_t walkXStart = gMapXStart;
        static_cast<void>(Sub_46D9C0(
            walkXStart, walkYStart, walkXEnd, walkYEnd,
            gMapTileArray.data(), gMapObjArray.data(), gMapHitArray.data(),
            gMapWalkabilityArray.data()));
    }

    const std::int32_t yEnd = gMapYEnd;
    const std::int32_t xStart = gMapXStart;
    std::int32_t row = gMapSpanY - 1;
    const std::int32_t rawX = (xStart + yEnd) * 64 - 64;
    std::int32_t baseX = std::bit_cast<std::int32_t>(gWorldProjectionSourceC)
        + ((rawX - (rawX < 0 ? -1 : 0)) >> 1);
    const std::int32_t rawY = 48 * (yEnd - xStart - 1);
    std::int32_t result = rawY - (rawY < 0 ? -1 : 0);
    std::int32_t drawY =
        std::bit_cast<std::int32_t>(gWorldProjectionSourceD) + (result >> 1);
    std::int32_t column = 0;

    while (row >= 0) {
        std::int32_t scanRow = row;
        std::int32_t drawX = baseX;
        const std::int32_t diagonal = column - row;
        while (scanRow >= 0) {
            const std::int32_t scanColumn = diagonal + scanRow;
            if (scanColumn < 0) {
                break;
            }
            const std::int32_t index =
                scanColumn + scanRow * gMapSpanX;
            result = Sub_46CF00(
                gMapTileArray[static_cast<std::size_t>(index)]);
            if (result > 999 && drawX >= -32 && drawX < 672 &&
                drawY >= -24 && drawY < 504) {
                result = login::detail::DrawGraphic(
                    drawX, drawY, 5, result, 0, nullptr);
            }
            --scanRow;
            drawX -= 64;
        }
        if (column < gMapSpanX - 1) {
            ++column;
            baseX += 32;
        } else {
            --row;
            baseX -= 32;
        }
        drawY -= 24;
    }
    return result;
}

// 原 [0x4693F0,0x4697C9)：tile 扫描、obj 扫描与尺寸向上覆盖共用同一输出。
std::int32_t __cdecl Sub_4693F0(
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk)
{
    std::memset(walk, 0, kMapBufferCells * sizeof(std::uint16_t));
    const std::int32_t width = xEnd - xStart;
    const std::int32_t height = yEnd - yStart;
    if (width < 1 || height < 1) {
        return 0;
    }

    for (std::int32_t y = 0; y < height; ++y) {
        for (std::int32_t x = 0; x < width; ++x) {
            const std::int32_t index = y * width + x;
            const std::int32_t code = Sub_46CF00(tile[index]);
            std::uint16_t& value = walk[index];
            if (code > 999 || (code >= 60 && code <= 79)) {
                std::int32_t graphic = 0;
                std::uint16_t flag = 0;
                resource::GetGraphicMapNoIndex(code, &graphic);
                resource::GetGraphicFlag1(static_cast<std::uint32_t>(graphic), &flag);
                if (flag == 2) {
                    value = 2;
                } else if (flag == 0 && value != 2) {
                    value = 1;
                }
            } else if (code == 4) {
                value = 2;
            } else if (code == 2 || code == 5 || code == 6 ||
                       code == 9 || code == 10 ||
                       (code == 0 && (hit[index] & 0x4000u) != 0)) {
                if (value != 2) {
                    value = 1;
                }
            }
        }
    }

    for (std::int32_t y = 0; y < height; ++y) {
        for (std::int32_t x = 0; x < width; ++x) {
            const std::int32_t index = y * width + x;
            const std::int32_t code = Sub_46CF00(obj[index]);
            std::uint16_t& value = walk[index];
            if (code > 999 || (code >= 60 && code <= 79)) {
                std::int32_t graphic = 0;
                std::uint16_t flag = 0;
                resource::GetGraphicMapNoIndex(code, &graphic);
                resource::GetGraphicFlag1(static_cast<std::uint32_t>(graphic), &flag);
                if (code > 999 && (flag == 0 || flag == 2)) {
                    std::uint16_t sx = 0;
                    std::uint16_t sy = 0;
                    resource::GetGraphicSxSy(
                        static_cast<std::uint32_t>(graphic), &sx, &sy);
                    const std::int32_t graphicWidth =
                        std::bit_cast<std::int16_t>(sx);
                    const std::int32_t graphicHeight =
                        std::bit_cast<std::int16_t>(sy);
                    for (std::int32_t row = 0; row < graphicHeight; ++row) {
                        for (std::int32_t col = 0; col < graphicWidth; ++col) {
                            if (y - row >= 0 && x + col < width) {
                                std::uint16_t& covered =
                                    walk[(y - row) * width + x + col];
                                if (flag == 2 || covered != 2) {
                                    covered = flag == 2 ? 2 : 1;
                                }
                            }
                        }
                    }
                } else if (flag == 2) {
                    value = 2;
                } else if (flag == 0 && value != 2) {
                    value = 1;
                }
            } else if (code == 4) {
                value = 2;
            } else if (code == 2 || code == 5 || code == 6 ||
                       code == 9 || code == 10) {
                if (value != 2) {
                    value = 1;
                }
            }
            if (tile[index] == 0 && obj[index] == 0 && value == 0) {
                value = 1;
            }
            if (static_cast<std::uint8_t>(hit[index]) == 1) {
                value = 1;
            }
        }
    }
    return 2 * width;
}

// 原 [0x46D9C0,0x46DA72)：模式 1 使用 hit 高字节，其余完整委托 4693F0。
std::int32_t __cdecl Sub_46D9C0(
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk)
{
    if (gValue9DAD68 != 1) {
        return Sub_4693F0(xStart, yStart, xEnd, yEnd, tile, obj, hit, walk);
    }
    std::memset(walk, 0, kMapBufferCells * sizeof(std::uint16_t));
    const std::int32_t width = xEnd - xStart;
    const std::int32_t remaining = yEnd - yStart;
    if (width < 1 || remaining < 1) {
        return remaining;
    }
    for (std::int32_t y = 0; y < remaining; ++y) {
        for (std::int32_t x = 0; x < width; ++x) {
            const std::int32_t index = y * width + x;
            if ((hit[index] & 0x0100u) != 0) {
                walk[index] = 1;
            }
        }
    }
    return 0;
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

std::uint16_t Sub_46AC10()
{
    const std::uint16_t count = gValueC82202;
    if (std::bit_cast<std::int16_t>(count) <= 0) {
        return count;
    }
    std::int32_t index = 0;
    for (;;) {
        const std::int16_t direction = std::bit_cast<std::int16_t>(
            gPendingDirections9A86D4[static_cast<std::size_t>(index)]);
        const std::uint32_t encoded = Sub_46BD20(direction, 0);
        const std::int16_t remaining = std::bit_cast<std::int16_t>(gValueC82202);
        gMoveDirectionText9E03D0[static_cast<std::size_t>(index)] =
            std::bit_cast<char>(static_cast<std::uint8_t>(encoded));
        ++index;
        if (index >= remaining) {
            break;
        }
    }
    if (gValueFF0D36 != 0) {
        const std::uint32_t flags = ReadPlayerFlags18A();
        if ((flags & 0x100u) == 0) {
            return static_cast<std::uint16_t>(flags);
        }
    }
    gMoveDirectionText9E03D0[static_cast<std::size_t>(index)] = '\0';
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    return static_cast<std::uint16_t>(
        Sub_50C8D0(x, y, gMoveDirectionText9E03D0.data()));
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

// 原 [0x46ADA0,0x46AE53)：type 1 当次读取探测值，发送后更新请求状态。
void* __cdecl Sub_46ADA0()
{
    if (gValueCAFEA0 == 0) {
        gValueCAFEA0 = static_cast<std::uint32_t>(audio::Sub_504410(1));
    }
    static_cast<void>(Sub_4681B0());

    const std::uint16_t sequenceLow =
        static_cast<std::uint16_t>(gValueC8221C);
    gValue9A86D8 = 1;
    gValueC82220 = 1;
    gValue9DAD54 = sequenceLow;

    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    const std::int32_t detectedCode =
        std::bit_cast<std::int16_t>(gLastDetectedCode);
    const std::uint32_t sequence = gValueC8221C;
    static_cast<void>(network::Sub_510630(
        platform::gConnectionSocket,
        detectedCode,
        std::bit_cast<std::int32_t>(sequence),
        x,
        y,
        -1));

    const std::uint32_t nextSequence = gValueC8221C + 1U;
    gCurrentSceneState = 200;
    gValueC8221C = nextSequence;
    void* const result = Sub_51D5B0();
    gValueC8220C = 1;
    gValueC82210 = 1;
    gValueC82208 = 1;
    gValueC82214 = 0;
    gValueC82218 = 0;
    return result;
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

double Sub_46FB80(
    const float x, const float y, const float targetX, const float targetY)
{
    const double differenceX = static_cast<double>(targetX) - x;
    const double differenceY = static_cast<double>(targetY) - y;
    const double squaredY = differenceY * differenceY;
    const double squaredX = differenceX * differenceX;
    return squaredY + squaredX;
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

// 原 [0x46B5C0,0x46B6F0)：两次阻挡探测后覆写当前路径末项，计数不增长。
std::int32_t __cdecl Sub_46B5C0(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t firstX, const std::int32_t firstY,
    const std::int32_t secondX, const std::int32_t secondY)
{
    if ((secondX == 0 && secondY == 0) ||
        (firstX == 0 && firstY == 0)) {
        return 0;
    }
    const std::int32_t combinedX = firstX + secondX;
    if (combinedX != 1 && combinedX != -1) {
        return 0;
    }
    const std::int32_t combinedY = firstY + secondY;
    if (combinedY != 1 && combinedY != -1) {
        return 0;
    }

    std::uint16_t direction = 0;
    for (; direction < gDirectionDeltas55D9DC.size(); ++direction) {
        if (gDirectionDeltas55D9DC[direction].x == combinedX &&
            gDirectionDeltas55D9DC[direction].y == combinedY) {
            break;
        }
    }
    const std::int32_t plusX =
        gDirectionDeltas55D9DC[(direction + 1U) & 7U].x;
    const std::int32_t plusY =
        gDirectionDeltas55D9DC[(direction + 1U) & 7U].y;
    const std::int32_t minusX =
        gDirectionDeltas55D9DC[(direction - 1U) & 7U].x;
    const std::int32_t minusY =
        gDirectionDeltas55D9DC[(direction - 1U) & 7U].y;
    const std::int32_t firstBlocked =
        Sub_469810(x + plusX - secondX, y + plusY - secondY);
    const std::int32_t secondBlocked =
        Sub_469810(x + minusX - secondX, y + minusY - secondY);
    if (firstBlocked != 0 || secondBlocked != 0) {
        return 0;
    }

    const std::uint16_t last =
        static_cast<std::uint16_t>(gValueC82200 - 1U);
    const std::size_t index = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(last));
    gMovePathX[index] = static_cast<std::uint16_t>(x + firstX);
    gMovePathDirections[index] = direction;
    gMovePathY[index] = static_cast<std::uint16_t>(y + firstY);
    gValueC82200 = static_cast<std::uint16_t>(last + 1U);
    return 1;
}

std::int32_t Sub_46B6F0()
{
    const std::int32_t targetX =
        platform::legacy_crypt_int::Read_401EC0(&gValue9A7F88);
    const std::int32_t targetY =
        platform::legacy_crypt_int::Read_401EC0(&gValue9A7F98);
    std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::array<std::int32_t, 3> offsets {0, 1, -1};
    std::int32_t accumulatedX = 0;
    std::int32_t accumulatedY = 0;
    gValue55D9FC = 0xFFFFu;
    if (x == targetX && y == targetY) {
        return 0;
    }
    const float initialY = static_cast<float>(targetY - y);
    const float initialX = static_cast<float>(targetX - x);
    const std::int32_t initialDirection = static_cast<std::int32_t>(
        Sub_46CDF0(initialX, initialY));
    gValueC82200 = 0;

    while (x != targetX || y != targetY) {
        if (std::bit_cast<std::int16_t>(gValueC82200) >= 100) {
            gValueC82200 = 0;
            return targetY;
        }
        const float differenceY = static_cast<float>(targetY - y);
        const float differenceX = static_cast<float>(targetX - x);
        const std::int32_t desired = static_cast<std::int32_t>(
            Sub_46CDF0(differenceX, differenceY));
        if (desired != initialDirection
            && desired != ((initialDirection + 1) & 7)
            && desired != ((initialDirection - 1) & 7)) {
            break;
        }

        bool advanced = false;
        for (const std::int32_t offset : offsets) {
            std::int32_t direction = (desired + offset) & 7;
            if (direction != initialDirection
                && direction != ((initialDirection + 1) & 7)
                && direction != ((initialDirection - 1) & 7)) {
                continue;
            }
            std::int32_t deltaY =
                gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].y;
            std::int32_t deltaX =
                gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].x;
            const std::int32_t nextY = y + deltaY;
            const std::int32_t nextX = x + deltaX;
            if (Sub_469810(nextX, nextY) != 0) {
                if (nextX == targetX && nextY == targetY && (direction & 1) == 0) {
                    break;
                }
                continue;
            }
            if ((direction & 1) != 0) {
                const std::int32_t plusDirection = (direction + 1) & 7;
                const std::int32_t minusDirection = (direction - 1) & 7;
                const std::int32_t plusY = gDirectionDeltas55D9DC[
                    static_cast<std::size_t>(plusDirection)].y;
                const std::int32_t plusX = gDirectionDeltas55D9DC[
                    static_cast<std::size_t>(plusDirection)].x;
                const std::int32_t minusX = gDirectionDeltas55D9DC[
                    static_cast<std::size_t>(minusDirection)].x;
                const std::int32_t minusY = gDirectionDeltas55D9DC[
                    static_cast<std::size_t>(minusDirection)].y;
                const std::int32_t plusNextX = x + plusX;
                const std::int32_t plusNextY = y + plusY;
                const std::int32_t plusBlocked = Sub_469810(plusNextX, plusNextY);
                const std::int32_t minusNextX = x + minusX;
                const std::int32_t minusNextY = y + minusY;
                const std::int32_t minusBlocked = Sub_469810(minusNextX, minusNextY);
                if (plusBlocked != 0) {
                    if (minusBlocked != 0) {
                        break;
                    }
                    if (Sub_46B5C0(
                            x, y, minusX, minusY, accumulatedX, accumulatedY) != 0) {
                        accumulatedX += minusX;
                        accumulatedY += minusY;
                    } else {
                        const std::uint16_t count = gValueC82200;
                        const std::size_t index = static_cast<std::size_t>(
                            std::bit_cast<std::int16_t>(count));
                        gMovePathDirections[index] =
                            static_cast<std::uint16_t>(minusDirection);
                        gMovePathX[index] = static_cast<std::uint16_t>(x + minusX);
                        gMovePathY[index] = static_cast<std::uint16_t>(y + minusY);
                        gValueC82200 = static_cast<std::uint16_t>(count + 1u);
                        accumulatedX = 0;
                        accumulatedY = 0;
                    }
                    deltaX -= minusX;
                    deltaY -= minusY;
                    direction = static_cast<std::int32_t>(
                        Sub_46B580(0, 0, deltaX, deltaY));
                    x = minusNextX;
                    y = minusNextY;
                } else if (minusBlocked != 0) {
                    if (Sub_46B5C0(
                            x, y, plusX, plusY, accumulatedX, accumulatedY) != 0) {
                        accumulatedX += plusX;
                        accumulatedY += plusY;
                    } else {
                        const std::uint16_t count = gValueC82200;
                        const std::size_t index = static_cast<std::size_t>(
                            std::bit_cast<std::int16_t>(count));
                        // 原+1侧绕行先写X，不能与另一侧的方向优先写序合并。
                        gMovePathX[index] = static_cast<std::uint16_t>(x + plusX);
                        gMovePathDirections[index] =
                            static_cast<std::uint16_t>(plusDirection);
                        gMovePathY[index] = static_cast<std::uint16_t>(y + plusY);
                        gValueC82200 = static_cast<std::uint16_t>(count + 1u);
                        accumulatedX = 0;
                        accumulatedY = 0;
                    }
                    deltaX -= plusX;
                    deltaY -= plusY;
                    direction = static_cast<std::int32_t>(
                        Sub_46B580(0, 0, deltaX, deltaY));
                    x = plusNextX;
                    y = plusNextY;
                }
            }
            if (Sub_46B5C0(x, y, deltaX, deltaY, accumulatedX, accumulatedY) != 0) {
                x += deltaX;
                y += deltaY;
                accumulatedX += deltaX;
                accumulatedY += deltaY;
            } else {
                const std::uint16_t count = gValueC82200;
                const std::size_t index = static_cast<std::size_t>(
                    std::bit_cast<std::int16_t>(count));
                x += deltaX;
                y += deltaY;
                gMovePathDirections[index] = static_cast<std::uint16_t>(direction);
                accumulatedX = deltaX;
                gMovePathX[index] = static_cast<std::uint16_t>(x);
                gMovePathY[index] = static_cast<std::uint16_t>(y);
                gValueC82200 = static_cast<std::uint16_t>(count + 1u);
                accumulatedY = deltaY;
            }
            advanced = true;
            break;
        }
        if (!advanced) {
            break;
        }
    }

    const std::int32_t remainingX = targetX - x;
    if (remainingX != 0) {
        const std::int32_t step = remainingX > 0 ? 1 : -1;
        for (;;) {
            const std::int32_t nextX = x + step;
            if (Sub_469810(nextX, y) != 0 || x == targetX) {
                break;
            }
            x = nextX;
            const std::size_t index = static_cast<std::size_t>(
                std::bit_cast<std::int16_t>(gValueC82200));
            const std::uint16_t direction =
                static_cast<std::uint16_t>(Sub_46B580(0, 0, step, 0));
            // 轴补走先发布递增后的计数，再写三表，且到达后仍探测一次。
            ++gValueC82200;
            gMovePathDirections[index] = direction;
            gMovePathX[index] = static_cast<std::uint16_t>(x);
            gMovePathY[index] = static_cast<std::uint16_t>(y);
        }
    }
    const std::int32_t remainingY = targetY - y;
    if (remainingY != 0) {
        const std::int32_t step = remainingY > 0 ? 1 : -1;
        for (;;) {
            const std::int32_t nextY = y + step;
            if (Sub_469810(x, nextY) != 0 || y == targetY) {
                break;
            }
            y = nextY;
            const std::size_t index = static_cast<std::size_t>(
                std::bit_cast<std::int16_t>(gValueC82200));
            const std::uint16_t direction =
                static_cast<std::uint16_t>(Sub_46B580(0, 0, 0, step));
            ++gValueC82200;
            gMovePathDirections[index] = direction;
            gMovePathX[index] = static_cast<std::uint16_t>(x);
            gMovePathY[index] = static_cast<std::uint16_t>(y);
        }
    }
    if (std::bit_cast<std::int16_t>(gValueC82200) > 0
        && (x != targetX || y != targetY)) {
        const float differenceY = static_cast<float>(targetY - y);
        const float differenceX = static_cast<float>(targetX - x);
        const std::int32_t direction = static_cast<std::int32_t>(
            Sub_46CDF0(differenceX, differenceY));
        gValue55D9FC = static_cast<std::uint16_t>(direction);
        return direction;
    }
    return targetY;
}

// 原 [0x46BD20,0x46BD32)：AL 加法不改变先装入 EAX 的 mode 高 24 位。
std::uint32_t __cdecl Sub_46BD20(
    const std::int32_t value, const std::int32_t mode)
{
    const std::uint8_t low = static_cast<std::uint8_t>(value);
    const std::uint8_t result =
        static_cast<std::uint8_t>(low + (mode == 0 ? 0x61 : 0x41));
    return (static_cast<std::uint32_t>(mode) & 0xFFFFFF00U) | result;
}

// 原 [0x50C8D0,0x50C8F5)：先采独立 W32，再传文本、Y、X 与当前 socket。
std::int32_t __cdecl Sub_50C8D0(
    const std::int32_t x, const std::int32_t y,
    const char* const text)
{
    const std::int32_t fourth = std::bit_cast<std::int32_t>(gValue9EE8F8);
    return network::Sub_510570(
        platform::gConnectionSocket, x, y, text, fourth);
}

// 原 [0x4D3170,0x4D317B)：仅将 0x581800 写为 -1 后返回。
void Sub_4D3170()
{
    gValue581800 = -1;
}

// 原[440B50,440B6C)：只采一次场景值，以U32比较250..252。
std::int32_t Sub_440B50()
{
    const auto scene = static_cast<std::uint32_t>(gCurrentSceneState);
    return scene >= 250u && scene <= 252u ? 1 : 0;
}

// 原 [0x4410E0,0x4410E6)：无状态固定返回叶。
std::int32_t Sub_4410E0()
{
    return 0x11;
}

std::uint32_t ReadPlayerFlags18A() noexcept
{
    return static_cast<std::uint32_t>(gPlayerRuntime.flags18A)
        | (gPlayerRuntime.flags18C << 16);
}

std::int32_t ReadPlayerWalkSpeed() noexcept
{
    return gPlayerRuntime.walkSpeed;
}

std::int32_t ReadPlayerValueFC() noexcept
{
    return gPlayerRuntime.valueFC;
}

std::int32_t ReadPlayerValue16C() noexcept
{
    return gPlayerRuntime.value16C;
}

std::int32_t GetBattlePetPosition_405B40(const std::int32_t position)
{
    if ((position >= 0 && position < 5) || (position >= 10 && position < 15)) {
        return position + 5;
    }
    return position - 5;
}

std::int32_t Sub_518C60()
{
    std::int32_t count = 0;
    for (std::int32_t slot = 8; slot < 28; ++slot) {
        if (gPlayerItemsFDA8A4[slot].used == 0) {
            ++count;
        }
    }
    return count;
}

std::int32_t ReadPlayerValue15BCC() noexcept
{
    return gPlayerRuntime.value15BCC;
}

std::int32_t ReadPlayerLevel() noexcept
{
    return gPlayerRuntime.level;
}

std::int32_t ReadPlayerValue08() noexcept
{
    return gPlayerRuntime.value08;
}

const char* BorrowPlayerJobName() noexcept
{
    return gSkillRuntimeSlots.jobName.data();
}

std::int32_t ReadPlayerTimerRemaining0C() noexcept
{
    return gPlayerRuntime.timerRemaining0C;
}

std::int32_t ReadPlayerInjury() noexcept
{
    return gPlayerRuntime.valueF4;
}

std::int32_t ReadPlayerHpOnce()
{
    return platform::legacy_crypt_int::Read_401EC0(&gPlayerRuntime.hp);
}

std::int32_t ReadPlayerMaxHpOnce()
{
    return platform::legacy_crypt_int::Read_401EC0(&gPlayerRuntime.maxHp);
}

std::int32_t ReadPlayerFpOnce()
{
    return platform::legacy_crypt_int::Read_401EC0(&gPlayerRuntime.fp);
}

std::int32_t ReadPlayerMaxFpOnce()
{
    return platform::legacy_crypt_int::Read_401EC0(&gPlayerRuntime.maxFp);
}

bool IsPlayerFpLessThan(const std::int32_t cost)
{
    return platform::legacy_crypt_int::Less_401DB0(&gPlayerRuntime.fp, cost);
}

std::int32_t ReadPlayerValue15BD0(const std::int32_t index) noexcept
{
    return gPlayerRuntime.values15BD0[index];
}

std::uint16_t ReadPlayerActionFlags() noexcept
{
    return static_cast<std::uint16_t>(gPlayerRuntime.flags18C >> 16);
}

std::int32_t ReadPlayerActionValue190() noexcept
{
    return gPlayerRuntime.value190;
}

void ClearPlayerActionFlags(const std::uint16_t mask) noexcept
{
    gPlayerRuntime.flags18C &= ~(static_cast<std::uint32_t>(mask) << 16);
}

void SetPlayerActionFlags(const std::uint16_t mask) noexcept
{
    gPlayerRuntime.flags18C |= static_cast<std::uint32_t>(mask) << 16;
}

char* BorrowPlayerStallName() noexcept
{
    return gPlayerRuntime.text15BB8.data();
}

void CopyPlayerBarValues(
    platform::LegacyCryptInt* const hp, platform::LegacyCryptInt* const maxHp,
    platform::LegacyCryptInt* const fp, platform::LegacyCryptInt* const maxFp)
{
    platform::legacy_crypt_int::CopyPayload(hp, &gPlayerRuntime.hp);
    platform::legacy_crypt_int::CopyPayload(maxHp, &gPlayerRuntime.maxHp);
    platform::legacy_crypt_int::CopyPayload(fp, &gPlayerRuntime.fp);
    platform::legacy_crypt_int::CopyPayload(maxFp, &gPlayerRuntime.maxFp);
}

const char* BorrowPlayerDisplayText(const std::int16_t mode) noexcept
{
    switch (mode) {
    case 1: return gPlayerRuntime.name.data();
    case 2: return gPlayerRuntime.ownTitle.data();
    case 3: return gTitlesFF0508[gPlayerRuntime.titleIndex100].text.data();
    case 4: return gPlayerRuntime.text12A.data();
    case 5: return gPlayerRuntime.text14B.data();
    case 6: return gPlayerRuntime.text15BB8.data();
    default: return nullptr;
    }
}

std::uint32_t gActorBarsEnabled5AC490 = 0;

std::int32_t ReadPlayerImageNo() noexcept
{
    return gPlayerRuntime.imageNo;
}

std::int32_t ReadPlayerValue17C() noexcept
{
    return gPlayerRuntime.value17C;
}

std::int16_t ReadDirectionDeltaX(const std::int16_t direction) noexcept
{
    return gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].x;
}

std::int16_t ReadDirectionDeltaY(const std::int16_t direction) noexcept
{
    return gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].y;
}

std::int16_t ReadPetFollowIndex(const std::size_t position) noexcept
{
    return std::bit_cast<std::int16_t>(gPetFollowOrder[position]);
}

login::YobiAction* ReadPetMapAction(const std::int16_t index) noexcept
{
    return gPetMapCharaSlots[static_cast<std::size_t>(index)].action;
}

std::uint32_t ReadPetMapObjectIndex(const std::int16_t index) noexcept
{
    return gPetMapCharaSlots[static_cast<std::size_t>(index)].objectIndex;
}

std::uint16_t ReadPetMapFlags(const std::int16_t index) noexcept
{
    return gPetMapCharaSlots[static_cast<std::size_t>(index)].flags;
}

void Sub_46A990()
{
    if (gValueFF0D36 == 0) {
        return;
    }
    const std::uint32_t emptyPair = 0xFFFFFFFFu;
    std::memcpy(gPetFollowOrder.data(), &emptyPair, sizeof(emptyPair));
    std::memcpy(gPetFollowOrder.data() + 2, &emptyPair, sizeof(emptyPair));
    gPetFollowOrder[4] = 0xFFFFu;
    std::size_t output = 0;
    for (std::size_t index = 0; index < gPetMapCharaSlots.size(); ++index) {
        const auto& slot = gPetMapCharaSlots[index];
        if (slot.flags != 0 && slot.action != nullptr) {
            gPetFollowOrder[output] = static_cast<std::uint16_t>(index);
            ++output;
        }
    }
}

void ResetMapFollowersFromPath(login::YobiAction* const leader)
{
    for (std::int32_t i = 1; i < 5; ++i) {
        auto& slot = gPetMapCharaSlots[static_cast<std::size_t>(i)];
        if (slot.flags != 0) {
            const bool isPlayer = slot.objectIndex == gPlayerRuntime.objectIndex;
            login::detail::ResetMapFollowerFromPath(
                leader, i, slot.action, isPlayer, &gPlayerRuntime.direction);
        }
    }
}

void Sub_46AFE0(const std::int32_t index, std::int32_t x, std::int32_t y)
{
    login::YobiAction* const first = gPetMapCharaSlots[0].action;
    if (first == nullptr || !login::detail::IsMapActorIdleAtTarget(first)) {
        return;
    }
    std::int32_t previous = index - 1;
    for (; previous >= 0; --previous) {
        const auto& slot = gPetMapCharaSlots[static_cast<std::size_t>(previous)];
        if (slot.flags != 0 && slot.action != nullptr) {
            break;
        }
    }
    if (previous >= 0 && login::detail::AreMapActorTargetsAdjacent(
            gPetMapCharaSlots[static_cast<std::size_t>(previous)].action,
            gPetMapCharaSlots[static_cast<std::size_t>(index)].action)) {
        return;
    }
    gValue9DAD78 = 0x3F800000u;
    for (std::int32_t i = index; i < 5; ++i) {
        auto& slot = gPetMapCharaSlots[static_cast<std::size_t>(i)];
        if (slot.flags != 0 && slot.action != nullptr) {
            login::YobiAction* const action = slot.action;
            std::int32_t nextX;
            std::int32_t nextY;
            login::detail::ReadMapActorTarget(slot.action, &nextX, &nextY);
            login::detail::Sub_441F60(action, x, y);
            x = nextX;
            y = nextY;
        }
    }
}

void Sub_51B0F0()
{
    for (auto& slot : gPetMapCharaSlots) {
        slot.action = nullptr;
    }
}

login::YobiAction* Sub_51B080()
{
    for (auto& slot : gPetMapCharaSlots) {
        if (slot.flags != 0) {
            if (slot.objectIndex == gPlayerRuntime.objectIndex) {
                if (slot.action != nullptr) {
                    login::detail::ClearMapActorPathCount(slot.action);
                }
                Sub_51A5F0();
            } else {
                if ((gPlayerRuntime.flags18A & 0x100u) != 0 && slot.action != nullptr) {
                    login::detail::ClearMapActorPathCount(slot.action);
                }
                Sub_442C70(slot.action);
            }
        }
        slot.flags = 0;
        slot.objectIndex = 0;
        slot.action = nullptr;
    }
    return Sub_51A5B0();
}

void Sub_46A5A0(const std::int32_t x, const std::int32_t y)
{
    if (gValueFF0D36 == 0) {
        return;
    }
    // 原跨FDA88A的W32读取；不依赖现代成员布局形成别名。
    const std::uint32_t flags = static_cast<std::uint32_t>(gPlayerRuntime.flags18A)
        | (gPlayerRuntime.flags18C << 16);
    if ((flags & 0x100u) == 0) {
        return;
    }
    const std::int16_t index = std::bit_cast<std::int16_t>(gPetFollowOrder[1]);
    if (index < 0) {
        return;
    }
    login::YobiAction* const action =
        gPetMapCharaSlots[static_cast<std::size_t>(index)].action;
    login::detail::Sub_441F60(action, x, y);
}

void Sub_46A940()
{
    if (gValueFF0D36 == 0) {
        return;
    }
    const std::uint32_t flags = static_cast<std::uint32_t>(gPlayerRuntime.flags18A)
        | (gPlayerRuntime.flags18C << 16);
    if ((flags & 0x100u) == 0) {
        return;
    }
    for (std::size_t position = 1; position < gPetFollowOrder.size(); ++position) {
        const std::int16_t index =
            std::bit_cast<std::int16_t>(gPetFollowOrder[position]);
        if (index < 0) {
            break;
        }
        login::YobiAction* const action =
            gPetMapCharaSlots[static_cast<std::size_t>(index)].action;
        login::detail::Sub_442730(action);
    }
}

std::uint8_t Sub_46F680()
{
    const float mouseY = static_cast<float>(
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY));
    const float mouseX = static_cast<float>(
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX));
    float projectedX;
    float projectedY;
    static_cast<void>(
        ProjectScreenToWorld_46CE80(mouseX, mouseY, &projectedX, &projectedY));
    const std::int32_t pixelX = static_cast<std::int32_t>(
        static_cast<std::int64_t>(static_cast<double>(projectedX) + 0.5));
    const double biasedY = static_cast<double>(projectedY) + 0.5;
    gValueC8A78C = pixelX;
    const std::int32_t pixelY =
        static_cast<std::int32_t>(static_cast<std::int64_t>(biasedY));
    const std::int32_t adjustedX = pixelX + 32;
    gValueC8A790 = pixelY;
    const std::int32_t targetY = (pixelY + 32) / 64;
    gPlayerMoveTargetY = targetY;
    const std::int32_t snappedY = targetY * 64;
    const std::int32_t targetX = adjustedX / 64;
    gPlayerMoveTargetX = targetX;
    const float gridY = static_cast<float>(snappedY);
    const float gridX = static_cast<float>(targetX * 64);
    ProjectWorldToScreen_46CE30(gridX, gridY, &projectedX, &projectedY);
    gValueC8A7A0 = 0;
    gValueC8A7A4 = 0;
    gValueC8A7A8 = 0;
    gValueC8A7AC = 0;

    if (platform::legacy_crypt_int::Less_401DB0(
            &platform::gLegacyMouseState.mouseY, 456)
        && ((static_cast<std::uint8_t>(platform::gLegacyMouseState.valueD68DB4) < 212
                && ui::ReadDragItemIndex_584DA4() < 0)
            || gValueC8A780 != 0 || gValue62B308 != 0 || gValue62B398 != 0)
        && gValueC8A79C == 0) {
        render::GraphicDescriptor16 descriptor {};
        const std::uint32_t color = 0xFFFFFFFFu;
        std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
        descriptor.bytes[13] = 4;
        const std::int32_t drawY = static_cast<std::int32_t>(
            static_cast<std::int64_t>(static_cast<double>(projectedY) + 0.5));
        const std::int32_t drawX = static_cast<std::int32_t>(
            static_cast<std::int64_t>(static_cast<double>(projectedX) + 0.5));
        static_cast<void>(
            login::detail::DrawGraphic(drawX, drawY, 100, 0x3BE99, 0, &descriptor));
    }
    const bool mouseBelowLimit = platform::legacy_crypt_int::Less_401DB0(
        &platform::gLegacyMouseState.mouseY, 456);
    if (!mouseBelowLimit
        || !((static_cast<std::uint8_t>(platform::gLegacyMouseState.valueD68DB4) < 212
                && ui::ReadDragItemIndex_584DA4() < 0)
            || gValueC8A77C != 0 || gValue62B308 != 0 || gValue62B398 != 0)) {
        gValueC8A778 = 0;
        gValueC8A794 = 0;
        return static_cast<std::uint8_t>(mouseBelowLimit);
    }

    const auto leftClick = static_cast<std::uint8_t>(
        platform::gLegacyMouseState.edgeButtons);
    if ((leftClick & 1u) != 0 && gValueC8A774 == 0) {
        gValueC8A7A0 = 1;
        gValueC8A778 = 0;
        gValueC8A794 = platform::ReadTickCount32();
    }
    if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.heldButtons) & 1u) != 0
        && gValueC8A774 == 0) {
        const std::uint32_t baseline = gValueC8A794;
        gValueC8A7A4 = 1;
        if (baseline != 0) {
            const std::uint32_t now = platform::ReadTickCount32();
            gValueC8A778 = now - gValueC8A794;
        }
    } else {
        gValueC8A778 = 0;
        gValueC8A794 = 0;
    }
    const auto rightClick = static_cast<std::uint8_t>(
        platform::gLegacyMouseState.edgeButtons);
    if ((rightClick & 2u) != 0 && gValueC8A7A8 == 0) {
        gValueC8A7A8 = 1;
    }
    if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.heldButtons) & 2u) != 0
        && gValueC8A7A8 == 0) {
        gValueC8A7AC = 1;
    }
    return 2;
}

std::uint32_t Sub_51B3E0()
{
    std::int32_t result = gValue581800;
    if (result != -1) {
        return std::bit_cast<std::uint32_t>(result);
    }
    const std::int32_t signX =
        platform::legacy_crypt_int::Subtract_401E60(&gPlayerX, gPlayerMoveTargetX);
    result = platform::legacy_crypt_int::Subtract_401E60(&gPlayerX, gPlayerMoveTargetX);
    if (signX < 0) {
        result = -result;
    }
    if (result > 1) {
        return std::bit_cast<std::uint32_t>(result);
    }
    const std::int32_t signY =
        platform::legacy_crypt_int::Subtract_401E60(&gPlayerY, gPlayerMoveTargetY);
    result = platform::legacy_crypt_int::Subtract_401E60(&gPlayerY, gPlayerMoveTargetY);
    if (signY < 0) {
        result = -result;
    }
    if (result > 1) {
        return std::bit_cast<std::uint32_t>(result);
    }
    const std::int32_t targetY = gPlayerMoveTargetY;
    const std::int32_t targetX = gPlayerMoveTargetX;
    result = Sub_4450D0(targetX, targetY, 7);
    if (result == 0) {
        return 0;
    }
    const std::int32_t deltaX =
        platform::legacy_crypt_int::IntMinusCrypt_401FE0(gPlayerMoveTargetX, &gPlayerX);
    const std::int32_t secondTargetY = gPlayerMoveTargetY;
    const float differenceX = static_cast<float>(deltaX);
    const std::int32_t deltaY =
        platform::legacy_crypt_int::IntMinusCrypt_401FE0(secondTargetY, &gPlayerY);
    const float differenceY = static_cast<float>(deltaY);
    const std::int32_t direction = static_cast<std::int32_t>(
        Sub_46CDF0(differenceX, differenceY));
    const std::uint32_t now = platform::ReadTickCount32();
    if (gValueFF0D38 + 500u >= now) {
        return now;
    }
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    static_cast<void>(network::Sub_510B80(platform::gConnectionSocket, x, y, direction));
    const std::uint32_t sentAt = platform::ReadTickCount32();
    gValueFF0D38 = sentAt;
    return sentAt;
}

void Sub_46B370()
{
    if (gPlayerRuntime.action == nullptr) {
        return;
    }
    const float movementX = std::bit_cast<float>(gValue9EE8FC);
    if (movementX < 0.0F || movementX > 0.0F) {
        return;
    }
    const float movementY = std::bit_cast<float>(gValue9EE900);
    if (movementY < 0.0F || movementY > 0.0F
        || gValue62B5AA != 0 || gValueC82220 != 0) {
        return;
    }
    if (platform::legacy_crypt_int::Equal_401D00(&gPlayerX, gPlayerMoveTargetX)
        && platform::legacy_crypt_int::Equal_401D00(&gPlayerY, gPlayerMoveTargetY)) {
        return;
    }
    const std::int32_t deltaX =
        platform::legacy_crypt_int::IntMinusCrypt_401FE0(gPlayerMoveTargetX, &gPlayerX);
    const std::int32_t targetY = gPlayerMoveTargetY;
    const float differenceX = static_cast<float>(deltaX);
    const std::int32_t deltaY =
        platform::legacy_crypt_int::IntMinusCrypt_401FE0(targetY, &gPlayerY);
    const float differenceY = static_cast<float>(deltaY);
    const std::int32_t direction = static_cast<std::int32_t>(
        Sub_46CDF0(differenceX, differenceY));
    const std::uint32_t now = platform::ReadTickCount32();
    if (gValueC82238 + 500u >= now) {
        return;
    }
    static_cast<void>(Sub_51A040(direction));
    const std::uint32_t encoded = Sub_46BD20(direction, 1);
    const char text[2] {
        std::bit_cast<char>(static_cast<std::uint8_t>(encoded)), '\0',
    };
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    static_cast<void>(Sub_50C8D0(x, y, text));
    gValueC82238 = platform::ReadTickCount32();
}

std::uint8_t Sub_469C90()
{
    const bool pendingIsZero =
        platform::legacy_crypt_int::Equal_401D00(&gValueC821D8, 0);
    if (pendingIsZero && gValue9A86D8 == 0 && gValue9FB92E == 0) {
        gValueC82220 = 0;
    }
    if ((gPlayerRuntime.flags18C & 0x00800000u) != 0) {
        return static_cast<std::uint8_t>(pendingIsZero);
    }
    if (gValueC8A7A8 != 0 && ui::ReadValueD67BE0() == 0) {
        const std::int32_t targetY = gPlayerMoveTargetY;
        const std::int32_t targetX = gPlayerMoveTargetX;
        const std::int32_t objectIndex = Sub_445030(targetX, targetY, 8, 0x1000);
        if (objectIndex >= 0) {
            static_cast<void>(ui::Sub_4CD3E0());
            static_cast<void>(ui::Sub_4CD400());
            static_cast<void>(network::Sub_512190(platform::gConnectionSocket, objectIndex));
        }
        Sub_46B370();
        static_cast<void>(Sub_51B3E0());
        const std::int32_t secondY = gPlayerMoveTargetY;
        const std::int32_t secondX = gPlayerMoveTargetX;
        const std::int32_t secondIndex = Sub_445160(secondX, secondY, 8);
        if (secondIndex != -1) {
            static_cast<void>(network::Sub_511760(
                platform::gConnectionSocket, 3, secondIndex));
        }
    }
    if ((gValueFF0D36 == 0 || (ReadPlayerFlags18A() & 0x100u) != 0)
        && gValue62B5AA == 0 && gValueC82220 == 0 && ui::ReadValueD67BE0() == 0) {
        if (gValueC8A778 >= 500u) {
            gValueC8A79C = 1;
        }
        if (gValueC8A79C == 1) {
            const auto click = static_cast<std::uint8_t>(
                platform::gLegacyMouseState.edgeButtons);
            if ((click & 1u) != 0) {
                gValueC8A79C = 0;
            } else {
                const std::uint32_t now = platform::ReadTickCount32();
                if (gValue55DDAC + 250u <= now) {
                    const std::uint32_t sampledTick = platform::ReadTickCount32();
                    const std::int32_t targetX = gPlayerMoveTargetX;
                    gValue55DDAC = sampledTick;
                    platform::LegacyCryptInt temporary;
                    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, targetX);
                    gValue9A7F88.mask = temporary.mask;
                    gValue9A7F88.encoded = temporary.encoded;
                    gValue9A7F88.readCount = temporary.readCount;
                    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
                    platform::legacy_crypt_int::ConstructFromInt_401CC0(
                        &temporary, gPlayerMoveTargetY);
                    gValue9A7F98.encoded = temporary.encoded;
                    gValue9A7F98.mask = temporary.mask;
                    gValue9A7F98.readCount = temporary.readCount;
                    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
                    gValueC821FC = 1;
                }
                if (platform::legacy_crypt_int::IntMinusCrypt_401FE0(
                        gPlayerMoveTargetX, &gPlayerX) != 0
                    || platform::legacy_crypt_int::IntMinusCrypt_401FE0(
                        gPlayerMoveTargetY, &gPlayerY) != 0) {
                    const std::int32_t deltaY =
                        platform::legacy_crypt_int::IntMinusCrypt_401FE0(
                            gPlayerMoveTargetY, &gPlayerY);
                    const std::int32_t targetX = gPlayerMoveTargetX;
                    const float differenceY = static_cast<float>(deltaY);
                    const std::int32_t deltaX =
                        platform::legacy_crypt_int::IntMinusCrypt_401FE0(targetX, &gPlayerX);
                    const float differenceX = static_cast<float>(deltaX);
                    const std::int32_t direction = static_cast<std::int32_t>(
                        Sub_46CDF0(differenceX, differenceY));
                    static_cast<void>(platform::RequestCursorState_506DE0(direction + 3, 0));
                }
            }
        } else if (gValueC8A7A0 != 0) {
            const std::uint32_t now = platform::ReadTickCount32();
            if (gValue55DDAC + 250u <= now) {
                // 单次路径先发布tick，再采X；两个临时量的载荷写序也不同。
                gValue55DDAC = platform::ReadTickCount32();
                platform::LegacyCryptInt temporary;
                platform::legacy_crypt_int::ConstructFromInt_401CC0(
                    &temporary, gPlayerMoveTargetX);
                gValue9A7F88.encoded = temporary.encoded;
                gValue9A7F88.mask = temporary.mask;
                gValue9A7F88.readCount = temporary.readCount;
                platform::legacy_crypt_int::Destroy_401CF0(&temporary);
                platform::legacy_crypt_int::ConstructFromInt_401CC0(
                    &temporary, gPlayerMoveTargetY);
                gValue9A7F98.readCount = temporary.readCount;
                gValue9A7F98.encoded = temporary.encoded;
                gValue9A7F98.mask = temporary.mask;
                platform::legacy_crypt_int::Destroy_401CF0(&temporary);
                gValueC821FC = 1;
            }
        }
    }
    Sub_46A990();
    if (gValueFF0D36 != 0 && (ReadPlayerFlags18A() & 0x100u) == 0) {
        return static_cast<std::uint8_t>(Sub_46A190());
    }
    return static_cast<std::uint8_t>(Sub_469FC0());
}

std::uint16_t Sub_469FC0()
{
    const std::uint32_t stopped = gValue9EE8B8;
    gValue9DAD78 = 0x3F800000u;
    if (stopped != 0) {
        return static_cast<std::uint16_t>(stopped);
    }
    if (platform::legacy_crypt_int::Equal_401D00(&gValueC821D8, 0)
        && gValue9A86D8 == 0 && gValue9FB92E == 0) {
        const std::int32_t pending = Sub_46AB60();
        if (pending != 0) {
            return static_cast<std::uint16_t>(pending);
        }
        const double targetX = static_cast<double>(
            platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotX)) * 64.0F;
        const std::uint32_t worldXBits = gWorldCurrentXBits;
        const float worldX = std::bit_cast<float>(worldXBits);
        if (!(targetX < worldX || targetX > worldX)) {
            const double targetY = static_cast<double>(
                platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotY)) * 64.0F;
            const std::uint32_t worldYBits = gWorldCurrentYBits;
            const float worldY = std::bit_cast<float>(worldYBits);
            if (!(targetY < worldY || targetY > worldY)) {
                if (gValueC821FC != 0 && gValueC82202 == 0) {
                    gValueC821FC = 0;
                    static_cast<void>(Sub_46B6F0());
                }
                std::int16_t pathCount = std::bit_cast<std::int16_t>(gValueC82200);
                if (pathCount == 0 && gValueC82202 == 0) {
                    Sub_46B4B0(std::bit_cast<std::int16_t>(gValue55D9FC));
                    pathCount = std::bit_cast<std::int16_t>(gValueC82200);
                    gValue55D9FC = 0xFFFFu;
                }
                if (pathCount > 0 && gValueC82202 == 0) {
                    Sub_46ABD0();
                    if (std::bit_cast<std::int16_t>(gValueC82202) > 0) {
                        static_cast<void>(Sub_46AA00());
                    }
                    static_cast<void>(ui::Sub_4CD3E0());
                    static_cast<void>(Sub_46AC10());
                }
                if (std::bit_cast<std::int16_t>(gValueC82202) > 0) {
                    static_cast<void>(Sub_46AF30());
                    const std::int16_t direction =
                        std::bit_cast<std::int16_t>(gPendingDirections9A86D4[0]);
                    Sub_46BCF0();
                    const std::int32_t speed = gPlayerRuntime.walkSpeed;
                    const std::int16_t deltaY =
                        gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].y;
                    // 原401E00忽略实参高W16；复用单次读取的加法与S16扩展。
                    const std::int32_t nextY =
                        platform::legacy_crypt_int::Add_401E10(&gPlayerY, deltaY);
                    const std::int16_t deltaX =
                        gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].x;
                    const std::int32_t nextX =
                        platform::legacy_crypt_int::Add_401E10(&gPlayerX, deltaX);
                    Sub_46A410(nextX, nextY, speed);
                    const std::int32_t y =
                        platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
                    const std::int32_t x =
                        platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
                    Sub_46A5A0(x, y);
                }
            }
        }
    }
    Sub_46A5F0();
    const std::uint32_t stoppedAfterFollow = gValue9EE8B8;
    gValueC8222E = 0;
    if (stoppedAfterFollow != 0) {
        return static_cast<std::uint16_t>(stoppedAfterFollow);
    }
    Sub_46A700();
    Sub_46A940();
    const std::uint16_t result = Sub_468200();
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    gWorldProjectionReferenceXBits = worldXBits;
    gWorldProjectionReferenceYBits = worldYBits;
    return result;
}

void Sub_46A410(
    const std::int32_t x, const std::int32_t y, const std::int32_t speed)
{
    platform::LegacyCryptInt temporary;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    gPlayerSnapshotX.encoded = temporary.encoded;
    gPlayerSnapshotX.mask = temporary.mask;
    gPlayerSnapshotX.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    gPlayerSnapshotY.readCount = temporary.readCount;
    gPlayerSnapshotY.encoded = temporary.encoded;
    gPlayerSnapshotY.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);

    const std::int32_t pixelX =
        platform::legacy_crypt_int::Multiply_401E90(&gPlayerSnapshotX, 64);
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const float differenceX = static_cast<float>(
        static_cast<double>(pixelX) - std::bit_cast<float>(worldXBits));
    const std::int32_t pixelY =
        platform::legacy_crypt_int::Multiply_401E90(&gPlayerSnapshotY, 64);
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    const platform::detail::MapMoveDifference difference {
        static_cast<double>(differenceX),
        static_cast<double>(pixelY) - std::bit_cast<float>(worldYBits),
    };
    platform::detail::MapMoveDirection direction;
    platform::detail::Cg2dMapMoveDirection(&difference, &direction);
    if (gPlayerRuntime.action != nullptr) {
        const double rate = static_cast<double>(speed);
        const float percent = std::bit_cast<float>(0x3C23D70Au);
        gValue9EE8FC = std::bit_cast<std::uint32_t>(
            static_cast<float>(rate * direction.unroundedX * 4.0F * percent));
        gValue9EE900 = std::bit_cast<std::uint32_t>(
            static_cast<float>(rate * direction.y * 4.0F * percent));
    } else {
        gValue9EE8FC = std::bit_cast<std::uint32_t>(
            static_cast<float>(direction.unroundedX * 4.0F));
        gValue9EE900 = std::bit_cast<std::uint32_t>(
            static_cast<float>(static_cast<double>(direction.y) * 4.0F));
    }
    if (direction.unroundedX < 0.0 || direction.unroundedX > 0.0
        || direction.y < 0.0F || direction.y > 0.0F) {
        const auto value = static_cast<std::int32_t>(
            Sub_46CDF0(direction.x, direction.y));
        static_cast<void>(Sub_51A040(value));
        static_cast<void>(Sub_51A640());
    }
    Sub_519EE0();
}

void Sub_46A700()
{
    static_cast<void>(Sub_46CEC0());
    const float movementX = std::bit_cast<float>(gValue9EE8FC);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(gValue9EE900);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (Sub_51A680() == 1) {
            Sub_51A060(0);
            static_cast<void>(Sub_51A660());
        }
        gMapCharaSceneSlots.value55DE98 = 0xFFFFu;
        static_cast<void>(Sub_46CEE0());
        Sub_519EE0();
        return;
    }

    const float targetX = static_cast<float>(static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotX)) * 64.0F);
    const double scaledTargetY = static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotY)) * 64.0F;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    const float targetY = static_cast<float>(scaledTargetY);
    const float rateX = std::bit_cast<float>(gValue9DAD78);
    const float rawStepX = std::bit_cast<float>(gValue9EE8FC);
    const double scaledX = static_cast<double>(rateX) * rawStepX;
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const float stepX = static_cast<float>(scaledX);
    const float rateY = std::bit_cast<float>(gValue9DAD78);
    const float rawStepY = std::bit_cast<float>(gValue9EE900);
    const float stepY = static_cast<float>(static_cast<double>(rateY) * rawStepY);
    const double distance = Sub_46FB80(
        std::bit_cast<float>(worldXBits), std::bit_cast<float>(worldYBits),
        targetX, targetY);
    const double squaredY = static_cast<double>(stepY) * stepY;
    const double squaredX = static_cast<double>(stepX) * stepX;
    const double squaredStep = squaredY + squaredX;
    if (!(squaredStep >= distance)) {
        const std::uint32_t currentXBits = gWorldCurrentXBits;
        gWorldCurrentXBits = std::bit_cast<std::uint32_t>(static_cast<float>(
            static_cast<double>(std::bit_cast<float>(currentXBits)) + stepX));
        const std::uint32_t currentYBits = gWorldCurrentYBits;
        gWorldCurrentYBits = std::bit_cast<std::uint32_t>(static_cast<float>(
            static_cast<double>(std::bit_cast<float>(currentYBits)) + stepY));
    } else {
        // 原先计算X格值，但先发布世界坐标/移动槽/C8222E，再截断并构造临时量。
        const double gridX = static_cast<double>(targetX) * 0.015625F;
        gWorldCurrentXBits = std::bit_cast<std::uint32_t>(targetX);
        gWorldCurrentYBits = std::bit_cast<std::uint32_t>(targetY);
        gValue9EE8FC = 0;
        gValue9EE900 = 0;
        gValueC8222E = 1;
        platform::LegacyCryptInt temporary;
        const auto x = static_cast<std::int32_t>(static_cast<std::int64_t>(gridX));
        platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
        gPlayerX.mask = temporary.mask;
        gPlayerX.encoded = temporary.encoded;
        gPlayerX.readCount = temporary.readCount;
        platform::legacy_crypt_int::Destroy_401CF0(&temporary);

        const std::uint32_t currentYBits = gWorldCurrentYBits;
        const auto y = static_cast<std::int32_t>(static_cast<std::int64_t>(
            static_cast<double>(std::bit_cast<float>(currentYBits)) * 0.015625F));
        platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
        gPlayerY.mask = temporary.mask;
        gPlayerY.encoded = temporary.encoded;
        gPlayerY.readCount = temporary.readCount;
        platform::legacy_crypt_int::Destroy_401CF0(&temporary);

        const std::int32_t count = gPlayerRuntime.value174;
        if (count > 0) {
            const std::int32_t remaining = count - 1;
            gPlayerRuntime.value174 = remaining;
            if (remaining <= 0) {
                login::YobiAction* const action = gPlayerRuntime.action;
                if (action != nullptr) {
                    login::detail::BeginMapActorPhase2(action);
                    const std::int32_t snapshotY =
                        platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotY);
                    const std::int32_t snapshotX =
                        platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotX);
                    static_cast<void>(network::Sub_511DD0(
                        platform::gConnectionSocket, snapshotX, snapshotY, 70));
                }
            }
        }
    }
    Sub_51A060(1);
    static_cast<void>(Sub_51A640());
    login::YobiAction* const action = gPlayerRuntime.action;
    if (action != nullptr) {
        gMapCharaSceneSlots.value55DE98 =
            login::detail::ReadYobiDirectionLow16(action);
    }
    static_cast<void>(Sub_46CEE0());
    Sub_519EE0();
}

void Sub_519A80()
{
    const std::int32_t direction = gPlayerRuntime.direction;
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    Sub_519CD0(gPlayerRuntime.imageNo, x, y, direction);
    const auto flags = static_cast<std::uint8_t>(gPlayerRuntime.flags18A);
    const std::int32_t flag40 = (flags & 0x40u) != 0;
    const std::int32_t flag80 = (flags & 0x80u) != 0;

    char escaped[256];
    char titles[256];
    input::EscapeLegacyTextAndNotify_52D2D0(
        gPlayerRuntime.ownTitle.data(), escaped, 256);
    std::strcpy(titles, escaped);
    std::strcat(titles, "|");
    input::EscapeLegacyTextAndNotify_52D2D0(
        gPlayerRuntime.text12A.data(), escaped, 256);
    std::strcat(titles, escaped);
    std::strcat(titles, "|");
    input::EscapeLegacyTextAndNotify_52D2D0(
        gPlayerRuntime.text14B.data(), escaped, 256);
    std::strcat(titles, escaped);
    const std::int32_t value188 =
        std::bit_cast<std::int16_t>(gPlayerRuntime.value188);
    const std::int32_t level = gPlayerRuntime.level;
    Sub_51A090(
        gPlayerRuntime.name.data(), titles, level, value188, flag40, flag80);

    login::YobiAction* const action = gPlayerRuntime.action;
    if (action != nullptr) {
        login::detail::CopyYobiPlayerHp(action, &gPlayerRuntime.hp);
        login::detail::CopyYobiPlayerMaxHp(
            gPlayerRuntime.action, &gPlayerRuntime.maxHp);
        const std::uint32_t objectIndex = gPlayerRuntime.objectIndex;
        for (auto& slot : gPetMapCharaSlots) {
            if (slot.flags != 0 && slot.objectIndex == objectIndex) {
                slot.action = gPlayerRuntime.action;
                break;
            }
        }
    }
    gValueFF0D30 = 0;
    gValueFF0D32 = 0;
    gValueF7CA04 = 0;
    gValueFF03D8 = 0;
    gValueFF0D34 = 0;
}

login::YobiAction* Sub_51A090(
    const char* const name, const char* const titles, const std::int32_t level,
    const std::int32_t value188, const std::int32_t flag40,
    const std::int32_t flag80)
{
    const auto nameLength = static_cast<std::int32_t>(std::strlen(name));
    if (nameLength <= 16) {
        // 519A80 的源就是本槽；原 rep movs 允许同源复制。
        std::memmove(gPlayerRuntime.name.data(), name, std::strlen(name) + 1);
    }

    char first[256];
    char second[256];
    char third[256];
    char fourth[256];
    network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, first);
    input::UnescapeLegacyTextAndNotify_52D2F0(first);
    if (std::strlen(first) <= 16) {
        std::strcpy(gPlayerRuntime.ownTitle.data(), first);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, second);
    input::UnescapeLegacyTextAndNotify_52D2F0(second);
    if (std::strlen(second) <= 32) {
        std::strcpy(gPlayerRuntime.text12A.data(), second);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, third);
    input::UnescapeLegacyTextAndNotify_52D2F0(third);
    if (std::strlen(third) <= 32) {
        std::strcpy(gPlayerRuntime.text14B.data(), third);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 4, 255, fourth);
    input::UnescapeLegacyTextAndNotify_52D2F0(fourth);
    if (std::strlen(fourth) <= 16) {
        std::strcpy(gPlayerRuntime.text15BB8.data(), fourth);
    }
    gPlayerRuntime.level = level;
    gPlayerRuntime.value188 = static_cast<std::uint16_t>(value188);
    if (flag40 != 0) {
        gPlayerRuntime.flags18A |= 0x40u;
    }
    if (flag80 != 0) {
        gPlayerRuntime.flags18A |= 0x80u;
    }
    return login::detail::PublishPlayerActionIdentity(
        name, titles, nameLength, level, value188);
}

login::YobiAction* ReadPlayerAction() noexcept
{
    return gPlayerRuntime.action;
}

void Sub_51A4B0()
{
    login::detail::CopyInitialPlayerActionIdentity(
        gPlayerRuntime.name.data(), gPlayerRuntime.ownTitle.data(),
        &gPlayerRuntime.level, &gPlayerRuntime.hp, &gPlayerRuntime.maxHp);
}

void Sub_519CD0(
    const std::int32_t imageNo, const std::int32_t x, const std::int32_t y,
    const std::int32_t direction)
{
    login::YobiAction* const oldAction = gPlayerRuntime.action;
    gPlayerRuntime.imageNo = imageNo;
    gPlayerRuntime.direction = direction;
    if (oldAction != nullptr) {
        Sub_519E50(imageNo, direction);
        static_cast<void>(Sub_519EC0(x, y));
        Sub_519EE0();
        return;
    }

    login::YobiAction* const action =
        login::detail::Sub_441DB0(imageNo, x, y, direction);
    gPlayerRuntime.action = action;
    if (action != nullptr) {
        login::detail::SetYobiPlayerFlag(action);
        login::YobiAction* const current = gPlayerRuntime.action;
        login::detail::SetYobiWalkSpeed(current, gPlayerRuntime.walkSpeed);
    }
}

void Sub_519E50(const std::int32_t imageNo, const std::int32_t direction)
{
    login::YobiAction* const action = gPlayerRuntime.action;
    gPlayerRuntime.imageNo = imageNo;
    gPlayerRuntime.direction = direction;
    if (action == nullptr) {
        return;
    }
    login::detail::SetYobiActionField2A8(action, imageNo);
    login::detail::SetYobiActionField2B8(gPlayerRuntime.action, direction);
    login::YobiAction* const current = gPlayerRuntime.action;
    if (login::detail::ReadMapActorAttachment44(current) != nullptr) {
        const std::int32_t mapped =
            Sub_51C0D0(login::detail::GetYobiActionField2A8(current));
        login::detail::SetYobiActionField2A8(gPlayerRuntime.action, mapped);
    }
}

std::uint32_t Sub_519EC0(const std::int32_t x, const std::int32_t y)
{
    return Sub_468330(x, y);
}

std::int32_t Sub_51A040(const std::int32_t direction)
{
    login::YobiAction* const action = gPlayerRuntime.action;
    gPlayerRuntime.direction = direction;
    if (action != nullptr) {
        login::detail::SetYobiActionField2B8(action, direction);
    }
    return direction;
}

void Sub_51A060(const std::int32_t characterAction)
{
    login::YobiAction* const action = gPlayerRuntime.action;
    if (action != nullptr) {
        login::detail::Sub_43EFC0(
            action, characterAction, characterAction == 1 ? 0 : 1);
    }
}

void Sub_51A840(
    const std::int32_t x, const std::int32_t y, const std::int32_t direction,
    const std::int32_t characterAction, const std::int32_t option1,
    const std::int32_t option2, const std::int32_t option3, const std::int32_t option4)
{
    switch (characterAction) {
    case 0: case 2: case 3: case 4: case 6: case 17: case 18: case 19:
        Sub_51A060(characterAction);
        return;
    case 1: {
        const std::int32_t speed = option1 == -1 ? 100 : option1;
        gPlayerRuntime.walkSpeed = speed;
        login::detail::SetYobiWalkSpeed(gPlayerRuntime.action, speed);
        Sub_51A060(1);
        return;
    }
    case 5: case 7: case 8: case 9: case 10: case 11:
    case 12: case 13: case 14: case 15: case 16:
        static_cast<void>(Sub_51A040(direction));
        Sub_51A060(characterAction);
        return;
    case 30: case 31:
        static_cast<void>(Sub_51A040(direction));
        Sub_51A060(characterAction == 30 ? 1 : 0);
        return;
    case 40: case 41: case 42: case 44:
        static_cast<void>(Sub_519EC0(x, y));
        static_cast<void>(Sub_51A040(direction));
        static_cast<void>(Sub_4681B0());
        if (characterAction == 41) {
            if (option1 == 1) {
                Sub_51A5A0();
            } else {
                static_cast<void>(Sub_51A5B0());
            }
        } else if (characterAction == 42) {
            if (option1 == 1) {
                Sub_51A600();
            } else {
                Sub_51A610();
            }
        } else if (characterAction == 44) {
            if (option1 == 1) {
                Sub_51A620();
            } else {
                Sub_51A630();
            }
        }
        return;
    case 45:
        if (option1 == 1) {
            Sub_51BDB0();
        } else {
            Sub_51BDC0();
        }
        static_cast<void>(Sub_51A040(direction));
        return;
    case 46:
        if (option1 != 0) {
            static_cast<void>(Sub_51BDD0(option1));
        } else {
            Sub_51BDF0();
        }
        static_cast<void>(Sub_51A040(direction));
        return;
    case 47:
        Sub_51BE10();
        static_cast<void>(Sub_51A040(direction));
        return;
    case 48:
        if (option1 == 1) {
            Sub_51BE20();
        } else {
            Sub_51BE30();
        }
        static_cast<void>(Sub_51A040(direction));
        return;
    case 49:
        Sub_51BE40();
        static_cast<void>(Sub_51A040(direction));
        return;
    case 50:
        static_cast<void>(Sub_51A040(direction));
        return;
    case 51:
        if (Sub_469810(x, y) == 0) {
            static_cast<void>(Sub_519EC0(x, y));
        }
        Sub_519EE0();
        static_cast<void>(Sub_51A040(direction));
        Sub_51A060(0);
        static_cast<void>(Sub_4681B0());
        return;
    case 60:
        login::detail::ApplyPlayerAppearanceReply(
            option1, option2, option3, option4, &gPlayerRuntime.imageNo);
        return;
    case 63:
        static_cast<void>(Sub_51BDD0(option1));
        static_cast<void>(Sub_51A040(direction));
        return;
    case 64:
        Sub_51BDF0();
        static_cast<void>(Sub_51A040(direction));
        return;
    case 70:
        login::detail::ApplyPlayerRideReply(option1, option2, &gPlayerRuntime.value174);
        return;
    case 80:
        login::detail::ApplyPlayerPhase3Reply(option1);
        return;
    case 200:
        if (option1 != 0) {
            gPlayerRuntime.flags18C |= 0x10000000u;
        } else {
            gPlayerRuntime.flags18C &= 0xEFFFFFFFu;
        }
        return;
    default:
        return;
    }
}

std::int32_t Sub_51C0D0(const std::int32_t imageNo)
{
    // 保留原区段顺序和两种除法，不扩展为通用图号规则。
    struct ImageRange {
        std::int32_t first;
        std::int32_t last;
        std::int32_t base;
    };
    constexpr ImageRange grouped[] {
        {100000, 100173, 114000}, {100250, 100423, 114028},
        {105000, 105173, 114056}, {105250, 105423, 114084},
        {106000, 106173, 114112}, {106250, 106423, 114140},
        {105500, 105548, 112000},
    };
    for (const auto& range : grouped) {
        if (imageNo >= range.first && imageNo <= range.last) {
            const std::int32_t offset = imageNo - range.first;
            return offset % 25 / 6 + 4 * (offset / 25) + range.base;
        }
    }
    constexpr ImageRange linear[] {
        {104400, 104429, 112010}, {104430, 104459, 112015},
        {104460, 104489, 112020}, {104490, 104519, 112025},
        {120100, 120123, 120148}, {120124, 120147, 120152},
    };
    for (const auto& range : linear) {
        if (imageNo >= range.first && imageNo <= range.last) {
            return (imageNo - range.first) / 6 + range.base;
        }
    }
    if (imageNo >= 120156 && imageNo <= 120159) {
        return imageNo - 8;
    }
    if (imageNo >= 120160 && imageNo <= 120163) {
        return imageNo - 8;
    }
    if (imageNo == 100900) {
        return 114169;
    }
    if (imageNo == 100901) {
        return 114168;
    }
    return imageNo;
}

std::int8_t Sub_519D50()
{
    login::YobiAction* const action = gPlayerRuntime.action;
    if (action != nullptr) {
        login::detail::ClearMapActorAttachments(action);
        login::detail::MarkDeath(gPlayerRuntime.action);
        gPlayerRuntime.action = nullptr;
    }
    Sub_51A5B0();
    if (Sub_51BEC0() == 0) {
        return 0;
    }
    return Sub_51BE50(0);
}

void Sub_51A5A0()
{
    gPlayerRuntime.flags18A |= 0x0100u;
    gPlayerRuntime.flags18C = (gPlayerRuntime.flags18C & 0xFFFFFF00u) | 1u;
}

void Sub_51A5E0()
{
    gPlayerRuntime.flags18A |= 0x200u;
}

void Sub_51A5F0()
{
    gPlayerRuntime.flags18A &= 0xFDFFu;
}

void Sub_51A600()
{
    gPlayerRuntime.flags18A |= 0x4000u;
}

void Sub_51A610()
{
    gPlayerRuntime.flags18A &= 0xBFFFu;
}

void Sub_51A620()
{
    gPlayerRuntime.flags18A |= 0x8000u;
}

void Sub_51A630()
{
    gPlayerRuntime.flags18A &= 0x7FFFu;
}

void Sub_51BDB0()
{
    gPlayerRuntime.flags18C |= 0x00010000u;
}

void Sub_51BDC0()
{
    gPlayerRuntime.flags18C &= 0xFFFEFFFFu;
}

void Sub_51BE10()
{
    gPlayerRuntime.flags18C |= 0x00240000u;
}

void Sub_51BE20()
{
    gPlayerRuntime.flags18C |= 0x00080000u;
}

void Sub_51BE30()
{
    gPlayerRuntime.flags18C &= 0xFFF7FFFFu;
}

void Sub_51BE40()
{
    gPlayerRuntime.flags18C |= 0x00500000u;
}

void Sub_51BEE0()
{
    gPlayerRuntime.flags18C |= 0x00800000u;
}

login::YobiAction* Sub_51A5B0()
{
    login::YobiAction* const action = gPlayerRuntime.action;
    gPlayerRuntime.flags18C &= 0xFFFFFF00u;
    if (action == nullptr || login::detail::GetYobiActionField2E4(action) != 2) {
        gPlayerRuntime.flags18A &= 0xFEFFu;
    }
    return action;
}

std::int32_t Sub_51BDD0(const std::int32_t value)
{
    gPlayerRuntime.flags18C |= 0x00020000u;
    gPlayerRuntime.value190 = value;
    return value;
}

void Sub_51BDF0()
{
    gPlayerRuntime.flags18C &= 0xFFFDFFFFu;
    gPlayerRuntime.value190 = 0;
}

std::int32_t Sub_51BEC0()
{
    const auto flags = static_cast<std::uint8_t>(gPlayerRuntime.flags18C >> 16);
    const std::int32_t value = gPlayerRuntime.value190;
    return (flags & 2u) != 0 ? value : 0;
}

std::int8_t Sub_51BE50(const std::int32_t requested)
{
    const auto flags = static_cast<std::uint8_t>(gPlayerRuntime.flags18C >> 16);
    if ((flags & 2u) != 0 && gPlayerRuntime.value190 == requested) {
        return std::bit_cast<std::int8_t>(flags);
    }

    std::int32_t value = requested;
    if (value < 0 || value >= Sub_4410E0()) {
        value = 0;
    }
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotX);
    const std::int32_t result =
        network::Sub_511E60(platform::gConnectionSocket, x, y, value);
    gPlayerRuntime.flags18C |= 0x00020000u;
    if (value == 0) {
        gPlayerRuntime.flags18C &= 0xFFFDFFFFu;
    }
    gPlayerRuntime.value190 = value;
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(result));
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
