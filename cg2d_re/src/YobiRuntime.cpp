#include "cg/YobiRuntime.hpp"

#include "LoginMessageDependencies.hpp"
#include "AnimationRuntimeState.hpp"
#include "world/WorldUpdateDependencies.hpp"
#include "platform/MapEffectMath.hpp"

#include "cg/AnimationRuntime.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <tuple>
#include <type_traits>

namespace cg::login {

using YobiCallback = void (*)(YobiAction* action);

// 原 x86 +offset 只作为字段来源证据；原生 x64 对象不复制 0x330 指针布局。
struct YobiAction {
    YobiAction* prev;
    YobiAction* next;
    YobiCallback callback;
    void* payload;
    std::uint8_t priority;
    std::uint8_t displayPriority;
    std::int32_t cacheIndex;
    std::int32_t deathFlag;
    std::int32_t graphicIndex;
    std::uint32_t flags;
    std::int32_t phase;
    std::int32_t x;
    std::int32_t y;
    std::int32_t deltaX; // 原 +38；窗口动画的横向步长。
    std::int32_t deltaY; // 原 +3C；440C50 另以此值排序子项，不与 y 合并。
    std::int32_t field58; // 原 +58，与 +2B8 的 direction 独立。
    float scaleX;
    float scaleY;
    std::array<char, 64> displayName; // 原 +64..+A3；41EB10 按 64B 复制，保留原编码。
    std::int32_t nameX;              // 原 +A4；DrawText2 消费低 S16。
    std::int32_t nameY;              // 原 +A8。
    std::int8_t nameColor;           // 原 +BC。
    std::int8_t nameFontType;        // 原 +BD。
    std::int8_t nameLayer;           // 原 +BE；DrawText2 的 renderMode。
    std::int32_t nameValueC0;        // 原 +C0；DrawText2 仅消费低 S8。
    std::array<char, 17> nameC4; // 原 +C4 原始姓名，不是 +64 绘制文本。
    std::array<char, 17> titleE1; // 原 +E1，51A090 当前最多写16B及NUL。
    std::array<char, 33> text102;
    std::array<char, 33> text123;
    platform::LegacyCryptInt readiness; // 原 +170；地图实体将同一槽用于 HP。
    platform::LegacyCryptInt maxHp; // 原 +180；不与 +170 合并。
    std::int32_t field1D0; // 原 +1D0；弹窗param6、观察子项槽序号或地图实体等级。
    std::int32_t field1D8; // 原 +1D8；51A090 写入完整第四参数。
    platform::LegacyCryptInt mapX1E0; // 原 +1E0/+1F0 当前位置；46DD90 按 X→Y 读取并乘64。
    platform::LegacyCryptInt mapY1F0;
    platform::LegacyCryptInt mapX200; // 原 +200/+210 移动目标；46DE50先赋这组，不合并载荷。
    platform::LegacyCryptInt mapY210;
    std::array<std::int32_t, 10> pathX220; // 原 +220..+247，路径X。
    std::array<std::int32_t, 10> pathY248; // 原 +248..+26F，路径Y。
    std::uint16_t field270; // 原 +270；路径按S16消费，地图创建只清本W16。
    std::uint16_t field272; // 原 +272；移动动作标志，不与路径计数合并。
    float worldX; // 原 +274/+278，地图投影输入，与 CryptInt 两组坐标独立。
    float worldY;
    std::uint32_t field27C;
    std::uint32_t field280;
    std::int32_t walkSpeed;
    std::int32_t framePlayingRate;
    std::int32_t imageNo;
    std::int32_t characterAction;
    std::int32_t direction;
    std::int32_t lastImageNo;       // 原 +2AC：上次播放图号。
    std::int32_t lastCharacterAction; // 原 +2B4：上次动作。
    std::int32_t lastDirection;     // 原 +2BC：上次方向。
    std::uint32_t frameIndex;       // 原 +2C0：以 U32 与帧数比较。
    std::int32_t frameRemain;       // 原 +2C4：非零则递减，本轮不取新帧。
    std::int32_t graphicOffsetX;    // 原 +2C8：图形偏移叠加帧坐标。
    std::int32_t graphicOffsetY;    // 原 +2CC。
    std::int32_t frameEvent;        // 原 +2D0：仅 flag>=10000 写入。
    std::int32_t hidden;            // 原 +2D4：仅严格等于 1 时跳过播放。
    std::uint32_t completedLoops;   // 原 +2DC。
    std::int32_t playRevert;        // 原 +2E0：保留 caller 完整 W32。
    std::int32_t field2E4;          // 原 +2E4；51A5B0 比较完整 W32 是否为2。
    std::int32_t frameX;            // 原 +2F0：帧坐标叠加坐标文件偏移。
    std::int32_t frameY;            // 原 +2F4。
    render::GraphicDescriptor16 drawMode;
    std::uint8_t batchType;
    std::int32_t color2;
    std::uint8_t rebirthValue1;
    std::uint8_t rebirthValue2;
    std::int32_t mapOffsetX; // 原 +314/+318；46CF60 写 .cut 偏移，46DD90 在 FrameMove 后读取。
    std::int32_t mapOffsetY;
    std::array<char, 17> text31C; // 原 +31C；51A090 第四文本的独立槽。
};

static_assert(std::is_standard_layout_v<YobiAction>);
static_assert(std::is_trivially_copyable_v<YobiAction>);

namespace detail {

std::int32_t gYobiUpdateGateMode = 0;
std::int32_t gYobiUpdatePhaseCounter = 0;
volatile std::uint32_t gYobiScreenOffsetRising = 0; // 原 0x59F9F4。
volatile std::uint32_t gYobiScreenOffsetX = 0; // 原 0x59F9FC。
volatile std::uint32_t gYobiScreenOffsetY = 0; // 原 0x59FA00。
volatile std::uint32_t gYobiScreenOffsetAmplitude = 0; // 原 0x59FA04。
volatile std::uint32_t gYobiScreenOffsetMaximumAmplitude = 4; // 原 0x553C58。

namespace {

constexpr std::size_t kYobiRegistryRowCount = 399;
constexpr std::size_t kYobiRegistryColumnCount = 399;
constexpr std::size_t kYobiRegistryEntryCount =
    kYobiRegistryRowCount * kYobiRegistryColumnCount;

// 原 byte_B65A80/dword_B8C864；按行展开，指针不拥有 action。
std::array<std::uint8_t, kYobiRegistryEntryCount> gYobiRegistryFlags {};
std::array<YobiAction*, kYobiRegistryEntryCount> gYobiRegistryActions {};

static_assert(
    std::tuple_size_v<decltype(gYobiRegistryFlags)>
    == std::size_t{399} * std::size_t{399});
static_assert(
    std::tuple_size_v<decltype(gYobiRegistryActions)>
    == std::size_t{399} * std::size_t{399});

constexpr std::uint32_t kFourPhaseFlag = 0x00040000U;
constexpr std::uint8_t kModalPriority = 3;
constexpr std::uint8_t kModalDisplayPriority = 0xD4;
constexpr std::uint8_t kSpriteBatch = 0;
constexpr std::uint8_t kModalLayer = 0xD4;

constexpr char kConfirmCaption[] = "\xBD\x54\xBB\x7B";
constexpr char kActionAllocationText[] = "GetAction Erorr!!";
constexpr char kActionAllocationCaption[] = "GetAction Erorr";
constexpr char kActionPayloadText[] =
    "\xB0\x4F\xBE\xD0\xC5\xE9\xAE\x65\xB6\x71\xA4\xA3\xA8\xAC\xA1\x49" "pAct";
constexpr char kYobiAllocationText[] =
    "\xB0\x4F\xBE\xD0\xC5\xE9\xAE\x65\xB6\x71\xA4\xA3\xA8\xAC\xA1\x49" "yobi";
constexpr char kYobiAllocationCaption[] = "GetYobi Erorr";

struct YobiUIData {
    std::int16_t winId;
    std::int16_t frameCount;
    std::int32_t width;
    std::int32_t height;
    std::int32_t centerX;
    std::int32_t centerY;
    std::int32_t currentHalfWidth;
    std::int32_t currentHalfHeight;
    std::int32_t baseImageNo;
    std::int16_t tileStyle;
    std::uint8_t trailingZero0;
    std::uint8_t trailingZero1;
};

// 原 payload +00..+2F；原生指针扩宽后只供三种地图角色及观察链使用。
struct MapCharaPayload {
    std::array<std::uint32_t, 5> values;
    std::array<YobiAction*, 5> partyActions;
    std::int32_t frame;
    YobiAction* next;
};

static_assert(std::is_standard_layout_v<MapCharaPayload>);
static_assert(std::is_trivially_copyable_v<MapCharaPayload>);
static_assert(offsetof(MapCharaPayload, values) == 0);
static_assert(offsetof(MapCharaPayload, partyActions) == 0x18);
static_assert(offsetof(MapCharaPayload, frame) == 0x40);
static_assert(offsetof(MapCharaPayload, next) == 0x48);
static_assert(sizeof(MapCharaPayload) == 0x50);

// 原 441DB0 的 0x50 载荷，与上方观察链载荷不同；未消费区不猜业务类型。
struct MapActorPayload {
    std::int32_t index;
    YobiAction* action04;
    YobiAction* action08;
    YobiAction* action0C;
    YobiAction* action10;
    std::array<std::byte, 8> bytes14;
    char* text1C;
    std::array<std::byte, 12> bytes20;
    YobiAction* action2C;
    YobiAction* action30;
    YobiAction* action34;
    YobiAction* action38;
    std::array<std::byte, 8> bytes3C;
    YobiAction* action44;
    std::int32_t counter48; // 原+48；43E490有符号比较/递增，46A700清零。
    YobiAction* action4C;
};

static_assert(std::is_standard_layout_v<MapActorPayload>);
static_assert(std::is_trivially_copyable_v<MapActorPayload>);
static_assert(offsetof(MapActorPayload, action04) == 0x08);
static_assert(offsetof(MapActorPayload, action08) == 0x10);
static_assert(offsetof(MapActorPayload, action0C) == 0x18);
static_assert(offsetof(MapActorPayload, action10) == 0x20);
static_assert(offsetof(MapActorPayload, bytes14) == 0x28);
static_assert(offsetof(MapActorPayload, text1C) == 0x30);
static_assert(offsetof(MapActorPayload, bytes20) == 0x38);
static_assert(offsetof(MapActorPayload, action2C) == 0x48);
static_assert(offsetof(MapActorPayload, action30) == 0x50);
static_assert(offsetof(MapActorPayload, action34) == 0x58);
static_assert(offsetof(MapActorPayload, action38) == 0x60);
static_assert(offsetof(MapActorPayload, bytes3C) == 0x68);
static_assert(offsetof(MapActorPayload, action44) == 0x70);
static_assert(offsetof(MapActorPayload, counter48) == 0x78);
static_assert(offsetof(MapActorPayload, action4C) == 0x80);
static_assert(sizeof(MapActorPayload) == 0x88);

static_assert(std::is_standard_layout_v<YobiUIData>);
static_assert(std::is_trivially_copyable_v<YobiUIData>);
static_assert(sizeof(YobiUIData) == 0x24);
static_assert(offsetof(YobiUIData, winId) == 0x00);
static_assert(offsetof(YobiUIData, frameCount) == 0x02);
static_assert(offsetof(YobiUIData, width) == 0x04);
static_assert(offsetof(YobiUIData, height) == 0x08);
static_assert(offsetof(YobiUIData, centerX) == 0x0C);
static_assert(offsetof(YobiUIData, centerY) == 0x10);
static_assert(offsetof(YobiUIData, currentHalfWidth) == 0x14);
static_assert(offsetof(YobiUIData, currentHalfHeight) == 0x18);
static_assert(offsetof(YobiUIData, baseImageNo) == 0x1C);
static_assert(offsetof(YobiUIData, tileStyle) == 0x20);
static_assert(offsetof(YobiUIData, trailingZero0) == 0x22);
static_assert(offsetof(YobiUIData, trailingZero1) == 0x23);

struct NineGridDescriptorFields {
    std::int16_t sourceX;
    std::int16_t sourceY;
    std::int16_t width;
    std::int16_t height;
    std::int32_t color;
    std::uint8_t clip;
    std::uint8_t mixMode;
    std::uint16_t paletteCacheIndex;
};

static_assert(std::is_standard_layout_v<NineGridDescriptorFields>);
static_assert(std::is_trivially_copyable_v<NineGridDescriptorFields>);
static_assert(sizeof(NineGridDescriptorFields) == 0x10);
static_assert(offsetof(NineGridDescriptorFields, color) == 0x08);
static_assert(offsetof(NineGridDescriptorFields, clip) == 0x0C);
static_assert(offsetof(NineGridDescriptorFields, mixMode) == 0x0D);
static_assert(offsetof(NineGridDescriptorFields, paletteCacheIndex) == 0x0E);

YobiAction* gYobiActionStart = nullptr;
YobiAction* gYobiActionEnd = nullptr;
YobiAction* gMapCharaHead = nullptr; // 原 0x5F9750，独立于首尾哨兵。
std::array<std::int32_t, 5> gMapValues5F9720 {};
std::array<std::int32_t, 5> gMapImages5AC4A0 {};
std::int32_t gMapImage5F9734 = 0;
std::int32_t gMapDirection5F9740 = 0;
std::int32_t gMapDuration5F9744 = 0;

struct MapPartyOffset {
    std::int32_t x;
    std::int32_t y;
};

// 原只读 [54D950,54DD10)：group/slot/direction 扁平索引；保留相邻项。
constexpr std::array<MapPartyOffset, 120> kMapPartyOffsets54D950 {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {-41, 15}, {-44, 0}, {-19, -9}, {0, -15}, {28, -12}, {44, 6}, {33, 15}, {0, 20},
    {19, -9}, {44, 0}, {41, 15}, {0, 20}, {-33, 15}, {-44, 6}, {-28, -12}, {0, -15},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {29, 9}, {0, 24}, {-29, 9}, {-45, -9}, {-31, -21}, {0, -18}, {31, -21}, {45, -9},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {34, 15}, {0, 21}, {-34, 15}, {-47, 2}, {-33, -11}, {0, -17}, {33, -11}, {47, 2},
    {65, 27}, {0, 38}, {-64, 27}, {-97, 0}, {-64, -31}, {0, -40}, {64, -31}, {97, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

static_assert(sizeof(MapPartyOffset) == 8);
static_assert(offsetof(MapPartyOffset, y) == 4);
static_assert(sizeof(kMapPartyOffsets54D950) == 0x3C0);

render::GraphicDescriptor16 ToGraphicDescriptor(
    const NineGridDescriptorFields& fields)
{
    render::GraphicDescriptor16 descriptor {};
    std::memcpy(descriptor.bytes.data(), &fields, sizeof(fields));
    return descriptor;
}

void QueueNineGridPiece(
    const std::int32_t baseImageNo,
    const std::int32_t imageOffset,
    const std::int32_t x,
    const std::int32_t y,
    const std::uint8_t layer,
    const NineGridDescriptorFields& fields)
{
    const render::GraphicDescriptor16 descriptor = ToGraphicDescriptor(fields);
    render::DrawGraphic3_520970(
        x,
        y,
        layer,
        baseImageNo + imageOffset,
        0,
        1,
        kSpriteBatch,
        &descriptor);
}

NineGridDescriptorFields MakeNineGridDescriptor(
    const std::int32_t sourceX,
    const std::int32_t sourceY,
    const std::int32_t width,
    const std::int32_t height,
    const std::int32_t color)
{
    return NineGridDescriptorFields {
        static_cast<std::int16_t>(sourceX),
        static_cast<std::int16_t>(sourceY),
        static_cast<std::int16_t>(width),
        static_cast<std::int16_t>(height),
        color,
        std::uint8_t{1},
        std::uint8_t{4},
        std::uint16_t{0},
    };
}

std::int32_t LowSignedByte(const std::int16_t value)
{
    return static_cast<std::int32_t>(
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value)));
}

void DrawNineGridImpl_49BE00(
    const std::int32_t baseImageNo,
    const std::int16_t x16,
    const std::int16_t y16,
    const std::int16_t width16,
    const std::int16_t height16,
    const std::uint8_t layer,
    const std::int32_t borderColor,
    const std::int32_t centerColor,
    const std::int16_t tileWidth16,
    const std::int16_t tileHeight16)
{
    const std::int32_t x = x16;
    const std::int32_t y = y16;
    const std::int32_t width = width16;
    const std::int32_t height = height16;
    const std::int32_t tileWidth = LowSignedByte(tileWidth16);
    const std::int32_t tileHeight = LowSignedByte(tileHeight16);
    const std::int32_t halfWidth = tileWidth >> 1;
    const std::int32_t halfHeight = tileHeight >> 1;

    std::int32_t leftWidth = tileWidth;
    std::int32_t rightWidth = tileWidth;
    if (width < 2 * tileWidth) {
        leftWidth = width >> 1;
        rightWidth = width - leftWidth;
    }

    std::int32_t topHeight = tileHeight;
    std::int32_t bottomHeight = tileHeight;
    if (height < 2 * tileHeight) {
        topHeight = height >> 1;
        bottomHeight = height - topHeight;
    }

    const std::int32_t leftSourceX = -halfWidth;
    const std::int32_t topSourceY = -halfHeight;
    const std::int32_t rightSourceX =
        tileWidth - rightWidth - halfWidth;
    const std::int32_t bottomSourceY =
        tileHeight - bottomHeight - halfHeight;
    const std::int32_t rightX = x + width - rightWidth;
    const std::int32_t bottomY = y + height - bottomHeight;

    QueueNineGridPiece(
        baseImageNo,
        0,
        x,
        y,
        layer,
        MakeNineGridDescriptor(
            leftSourceX,
            topSourceY,
            leftWidth,
            topHeight,
            borderColor));
    QueueNineGridPiece(
        baseImageNo,
        2,
        rightX,
        y,
        layer,
        MakeNineGridDescriptor(
            rightSourceX,
            topSourceY,
            rightWidth,
            topHeight,
            borderColor));
    QueueNineGridPiece(
        baseImageNo,
        6,
        x,
        bottomY,
        layer,
        MakeNineGridDescriptor(
            leftSourceX,
            bottomSourceY,
            leftWidth,
            bottomHeight,
            borderColor));
    QueueNineGridPiece(
        baseImageNo,
        8,
        rightX,
        bottomY,
        layer,
        MakeNineGridDescriptor(
            rightSourceX,
            bottomSourceY,
            rightWidth,
            bottomHeight,
            borderColor));

    const std::int32_t verticalEnd = height - bottomHeight;
    const std::int32_t verticalFullEnd = verticalEnd - tileHeight;
    std::int32_t verticalOffset = topHeight;
    while (verticalOffset < verticalFullEnd) {
        QueueNineGridPiece(
            baseImageNo,
            3,
            x,
            y + verticalOffset,
            layer,
            MakeNineGridDescriptor(
                leftSourceX,
                topSourceY,
                leftWidth,
                tileHeight,
                borderColor));
        QueueNineGridPiece(
            baseImageNo,
            5,
            rightX,
            y + verticalOffset,
            layer,
            MakeNineGridDescriptor(
                rightSourceX,
                topSourceY,
                rightWidth,
                tileHeight,
                borderColor));
        verticalOffset += tileHeight;
    }
    if (verticalOffset < verticalEnd) {
        const std::int32_t tailHeight = verticalEnd - verticalOffset;
        QueueNineGridPiece(
            baseImageNo,
            3,
            x,
            y + verticalOffset,
            layer,
            MakeNineGridDescriptor(
                leftSourceX,
                topSourceY,
                leftWidth,
                tailHeight,
                borderColor));
        QueueNineGridPiece(
            baseImageNo,
            5,
            rightX,
            y + verticalOffset,
            layer,
            MakeNineGridDescriptor(
                rightSourceX,
                topSourceY,
                rightWidth,
                tailHeight,
                borderColor));
    }

    const std::int32_t horizontalEnd = width - rightWidth;
    const std::int32_t horizontalFullEnd = horizontalEnd - tileWidth;
    std::int32_t horizontalOffset = leftWidth;
    while (horizontalOffset < horizontalFullEnd) {
        QueueNineGridPiece(
            baseImageNo,
            1,
            x + horizontalOffset,
            y,
            layer,
            MakeNineGridDescriptor(
                leftSourceX,
                topSourceY,
                tileWidth,
                topHeight,
                borderColor));
        QueueNineGridPiece(
            baseImageNo,
            7,
            x + horizontalOffset,
            bottomY,
            layer,
            MakeNineGridDescriptor(
                leftSourceX,
                bottomSourceY,
                tileWidth,
                bottomHeight,
                borderColor));
        horizontalOffset += tileWidth;
    }
    if (horizontalOffset < horizontalEnd) {
        const std::int32_t tailWidth = horizontalEnd - horizontalOffset;
        QueueNineGridPiece(
            baseImageNo,
            1,
            x + horizontalOffset,
            y,
            layer,
            MakeNineGridDescriptor(
                leftSourceX,
                topSourceY,
                tailWidth,
                topHeight,
                borderColor));
        QueueNineGridPiece(
            baseImageNo,
            7,
            x + horizontalOffset,
            bottomY,
            layer,
            MakeNineGridDescriptor(
                leftSourceX,
                bottomSourceY,
                tailWidth,
                bottomHeight,
                borderColor));
    }

    auto drawCenterRow = [&](
                             const std::int32_t rowOffset,
                             const std::int32_t rowHeight) {
        const std::int32_t centerFullEnd = width - tileWidth;
        std::int32_t centerOffset = 0;
        while (centerOffset < centerFullEnd) {
            QueueNineGridPiece(
                baseImageNo,
                4,
                x + centerOffset,
                y + rowOffset,
                layer,
                MakeNineGridDescriptor(
                    leftSourceX,
                    topSourceY,
                    tileWidth,
                    rowHeight,
                    centerColor));
            centerOffset += tileWidth;
        }
        if (centerOffset < width) {
            QueueNineGridPiece(
                baseImageNo,
                4,
                x + centerOffset,
                y + rowOffset,
                layer,
                MakeNineGridDescriptor(
                    leftSourceX,
                    topSourceY,
                    width - centerOffset,
                    rowHeight,
                    centerColor));
        }
    };

    const std::int32_t centerFullEnd = height - tileHeight;
    std::int32_t centerRowOffset = 0;
    while (centerRowOffset < centerFullEnd) {
        drawCenterRow(centerRowOffset, tileHeight);
        centerRowOffset += tileHeight;
    }
    if (centerRowOffset < height) {
        drawCenterRow(centerRowOffset, height - centerRowOffset);
    }
}

void AssignReady(YobiAction* const action)
{
    platform::legacy_crypt_int::AssignFromInt(&action->readiness, 1);
}

void UpdateModalAction_5046D0(YobiAction* const action)
{
    auto* const payload = static_cast<YobiUIData*>(action->payload);

    if (action->phase == 0) {
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(
                payload->centerX - payload->currentHalfWidth),
            static_cast<std::uint16_t>(
                payload->centerY - payload->currentHalfHeight),
            static_cast<std::uint16_t>(
                payload->centerX + payload->currentHalfWidth),
            static_cast<std::uint16_t>(
                payload->centerY + payload->currentHalfHeight),
            kModalLayer,
            0,
            0);

        payload->currentHalfWidth += action->deltaX;
        payload->currentHalfHeight += action->deltaY;
        if (payload->frameCount >= 10) {
            if (payload->winId == -1 || payload->winId == -2) {
                action->phase = 1;
            } else if (payload->winId >= 0) {
                action->phase = 2;
                ++payload->frameCount;
                return;
            }
        }
        ++payload->frameCount;
        return;
    }

    if (action->phase == 1) {
        AssignReady(action);
        return;
    }
    if (action->phase != 2) {
        return;
    }

    if (payload->winId < 4 && action->field1D0 != 0) {
        const std::int32_t rowCount = payload->height / 48;
        const std::int32_t columnCount = payload->width / 64;
        std::int32_t drawY = action->y + 24;
        for (std::int32_t row = 0; row < rowCount; ++row) {
            std::int32_t drawX = action->x + 32;
            for (std::int32_t column = 0;
                 column < columnCount;
                 ++column) {
                std::int32_t imageOffset;
                if (row == 0) {
                    if (column == 0) {
                        imageOffset = 0;
                    } else {
                        imageOffset = column == columnCount - 1 ? 2 : 1;
                    }
                } else if (row == rowCount - 1) {
                    if (column == 0) {
                        imageOffset = 6;
                    } else {
                        imageOffset = column == columnCount - 1 ? 8 : 7;
                    }
                } else if (column == 0) {
                    imageOffset = 3;
                } else {
                    imageOffset = column == columnCount - 1 ? 5 : 4;
                }

                login::detail::DrawGraphic(
                    drawX,
                    drawY,
                    kModalLayer,
                    payload->baseImageNo + imageOffset,
                    payload->tileStyle,
                    nullptr);
                drawX += 64;
            }
            drawY += 48;
        }
    } else {
        const bool wideBorder = payload->winId >= 4;
        DrawNineGridImpl_49BE00(
            payload->baseImageNo,
            static_cast<std::int16_t>(action->x),
            static_cast<std::int16_t>(action->y),
            static_cast<std::int16_t>(payload->width),
            static_cast<std::int16_t>(payload->height),
            kModalLayer,
            -1,
            std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}),
            static_cast<std::int16_t>(wideBorder ? 74 : 64),
            static_cast<std::int16_t>(wideBorder ? 54 : 48));
    }

    AssignReady(action);
}

void DeleteYobiAction_404D10(YobiAction* const action)
{
    if (action->payload != nullptr) {
        std::free(action->payload);
    }
    std::free(action);
}

bool ShouldDeferForFourPhaseGate(const YobiAction* const action)
{
    return (action->flags & kFourPhaseFlag) != 0
        && gYobiUpdateGateMode == 1
        && gYobiUpdatePhaseCounter != 0;
}

// 原两播放入口的同源翻转段；只有真实图形加载非零后才按需读取各个 DX 尺寸。
void ApplyYobiGraphicFlip(YobiAction* const action)
{
    if ((action->drawMode.bytes[0x0D] & 3u) == 0
        || render::LoadGraphic_522570(action->graphicIndex) == 0) {
        return;
    }
    const std::uint8_t mode = action->drawMode.bytes[0x0D];
    if ((mode & 1u) != 0) {
        action->graphicOffsetX =
            -(action->graphicOffsetX + render::ReadGraphicDxWidth(action->graphicIndex));
    }
    if ((mode & 2u) != 0) {
        action->graphicOffsetY =
            -(action->graphicOffsetY + render::ReadGraphicDxHeight(action->graphicIndex));
    }
}

// 原 517A8C..517C85 / 518098..518291 的相同阶段；不合并两入口的图号或速率门禁。
void ApplyYobiAnimeFrame(
    YobiAction* const action,
    const std::int32_t animeIndex,
    const resource::animation::detail::ActionEntry* const selected,
    const resource::animation::detail::FrameEntry* const frames)
{
    const std::uint16_t flag = frames[action->frameIndex].flag;
    if (flag != 0) {
        if (flag >= 10000) {
            action->frameEvent = flag;
        } else if (world::gCurrentSceneType != 9 || (action->flags & 0x80200u) == 0) {
            static_cast<void>(audio::PlaySe_52C450(flag, action->x));
        }
    }
    action->drawMode.bytes[0x0D] &= 0xFCu;
    action->graphicIndex = static_cast<std::int32_t>(frames[action->frameIndex].graphicIndex);
    std::int16_t offsetX;
    std::int16_t offsetY;
    resource::GetGraphicXY(frames[action->frameIndex].graphicIndex, &offsetX, &offsetY);
    action->graphicOffsetX = offsetX + frames[action->frameIndex].x;
    action->graphicOffsetY = offsetY + frames[action->frameIndex].y;
    std::memcpy(action->drawMode.bytes.data() + 0x0E,
        &selected->paletteIndex, sizeof(selected->paletteIndex));
    action->drawMode.bytes[0x0D] = static_cast<std::uint8_t>(selected->revert);
    std::memcpy(action->drawMode.bytes.data() + 8,
        &selected->value14, sizeof(selected->value14));
    ApplyYobiGraphicFlip(action);

    const auto* const coordinates = static_cast<resource::animation::detail::CoordinateEntry*>(
        resource::animation::gAnimateRecords9A4A58[animeIndex].coordinates);
    std::int32_t coordinateIndex = 0;
    if (coordinates != nullptr) {
        const auto count = resource::animation::gAnimateRecords9A4A58[animeIndex].coordinateCount;
        for (; coordinateIndex < count; ++coordinateIndex) {
            if (coordinates[coordinateIndex].graphicIndex
                == static_cast<std::uint32_t>(action->graphicIndex)) {
                break;
            }
        }
    }
    const std::uint32_t frameIndex = action->frameIndex;
    action->frameX = frames[frameIndex].x;
    action->frameY = frames[frameIndex].y;
    if (coordinates != nullptr
        && coordinateIndex < resource::animation::gAnimateRecords9A4A58[animeIndex].coordinateCount) {
        action->frameX += coordinates[coordinateIndex].x;
        action->frameY += coordinates[coordinateIndex].y;
    }
    const std::uint8_t mode = action->drawMode.bytes[0x0D];
    if ((mode & 1u) != 0) {
        action->frameX = -action->frameX;
    }
    if ((mode & 2u) != 0) {
        action->frameY = -action->frameY;
    }
    const std::int32_t remaining = action->frameRemain - 1;
    action->frameIndex = frameIndex + 1u;
    action->frameRemain = remaining;
}

} // namespace

void DrawNineGrid_49BE00(
    const std::int32_t baseImageNo,
    const std::int16_t x,
    const std::int16_t y,
    const std::int16_t width,
    const std::int16_t height,
    const std::uint8_t layer,
    const std::int32_t borderColor,
    const std::int32_t centerColor,
    const std::int16_t tileWidth,
    const std::int16_t tileHeight)
{
    DrawNineGridImpl_49BE00(
        baseImageNo, x, y, width, height, layer,
        borderColor, centerColor, tileWidth, tileHeight);
}

void Sub_46DD40()
{
    for (std::size_t row = 0; row < kYobiRegistryRowCount; ++row) {
        const std::size_t rowOffset = row * kYobiRegistryColumnCount;
        for (std::size_t column = 0;
             column < kYobiRegistryColumnCount;
             ++column) {
            const std::size_t index = rowOffset + column;
            if (gYobiRegistryFlags[index] == std::uint8_t{1}) {
                gYobiRegistryFlags[index] = std::uint8_t{0};
                YobiAction* const action = gYobiRegistryActions[index];
                MarkDeath(action);
            }
        }
    }
}

std::uint8_t& MapYobiRegistryFlagAt(const std::int32_t index) noexcept
{
    return gYobiRegistryFlags.data()[index];
}

YobiAction*& MapYobiRegistryActionAt(const std::int32_t index) noexcept
{
    return gYobiRegistryActions.data()[index];
}

void SetYobiMapOffsetX(YobiAction* const action, const std::int32_t value) noexcept
{
    action->mapOffsetX = value;
}

void SetYobiMapOffsetY(YobiAction* const action, const std::int32_t value) noexcept
{
    action->mapOffsetY = value;
}

YobiAction* Sub_404A40()
{
    gYobiActionStart = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));
    gYobiActionEnd = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));

    gYobiActionStart->prev = nullptr;
    gYobiActionStart->next = gYobiActionEnd;
    gYobiActionStart->callback = nullptr;
    gYobiActionStart->priority = 0;
    gYobiActionStart->graphicIndex = -1;

    gYobiActionEnd->prev = gYobiActionStart;
    gYobiActionEnd->next = nullptr;
    gYobiActionEnd->callback = nullptr;
    gYobiActionEnd->priority = 0xFF;
    gYobiActionEnd->graphicIndex = -1;
    return gYobiActionEnd;
}

YobiAction* NewYobiAction_404AE0(
    const std::uint8_t priority,
    const std::uint32_t payloadSize)
{
    auto* const action = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));
    if (action == nullptr) {
        platform::ShowLegacyMessage(
            kActionAllocationText,
            kConfirmCaption,
            0);
        platform::ShowLegacyMessage(
            kActionPayloadText,
            kActionAllocationCaption,
            0);
        return nullptr;
    }

    if (payloadSize != 0) {
        action->payload = std::calloc(std::size_t{1}, payloadSize);
        if (action->payload == nullptr) {
            std::free(action);
            platform::ShowLegacyMessage(
                kYobiAllocationText,
                kYobiAllocationCaption,
                0);
            return nullptr;
        }
        std::memset(action->payload, 0, payloadSize);
    }

    action->callback = nullptr;
    action->priority = priority;
    action->graphicIndex = -1;
    action->cacheIndex = -2;
    action->framePlayingRate = 100;
    action->scaleX = 1.0F;
    action->scaleY = 1.0F;
    action->drawMode.bytes[8] = 0xFF;
    action->drawMode.bytes[9] = 0xFF;
    action->drawMode.bytes[10] = 0xFF;
    action->drawMode.bytes[11] = 0xFF;
    action->batchType = 0;
    action->color2 = -1;
    action->rebirthValue1 = 0;
    action->rebirthValue2 = 0;
    action->walkSpeed = 100;

    YobiAction* insertionPoint = gYobiActionStart->next;
    YobiAction* const stop = gYobiActionEnd->next;
    if (insertionPoint != stop) {
        while (insertionPoint->priority <= priority) {
            insertionPoint = insertionPoint->next;
            if (insertionPoint == stop) {
                return action;
            }
        }

        YobiAction* const previous = insertionPoint->prev;
        action->next = insertionPoint;
        action->prev = previous;
        previous->next = action;
        insertionPoint->prev = action;
    }
    return action;
}

YobiAction* NewMapCharaAction()
{
    return NewYobiAction_404AE0(2, sizeof(MapCharaPayload));
}

// 原 441DB0 设置的真实目标；世界对象表及此回调完整实现仍待后续落位。
void Sub_4417E0(YobiAction* action);

YobiAction* Sub_441DB0(
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction)
{
    YobiAction* const action = NewYobiAction_404AE0(2, sizeof(MapActorPayload));
    if (action == nullptr) {
        return nullptr;
    }
    action->direction = direction;
    action->callback = Sub_4417E0;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->displayPriority = 10;
    action->flags = 0x34;

    // 本入口的 count/encoded/mask 写序与通用 CopyPayload 不同。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        platform::LegacyCryptInt temporary;
        platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, value);
        target.readCount = temporary.readCount;
        target.encoded = temporary.encoded;
        target.mask = temporary.mask;
        platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    };
    assignCoordinate(action->mapX200, mapX);
    assignCoordinate(action->mapY210, mapY);
    action->field270 = 0;
    assignCoordinate(action->mapX1E0, mapX);
    assignCoordinate(action->mapY1F0, mapY);

    const double scaledX = static_cast<double>(mapX) * 64.0F;
    action->field27C = 0;
    action->field280 = 0;
    action->phase = 0;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(static_cast<double>(mapY) * 64.0F);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(static_cast<double>(screenX) + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(static_cast<double>(screenY) + 0.5));
    return action;
}

void SetYobiPlayerFlag(YobiAction* const action) noexcept
{
    action->flags |= 0x80u;
}

void SetYobiWalkSpeed(YobiAction* const action, const std::int32_t speed) noexcept
{
    action->walkSpeed = speed;
}

YobiAction* ReadMapActorAttachment44(const YobiAction* const action) noexcept
{
    return static_cast<const MapActorPayload*>(action->payload)->action44;
}

void BeginMapActorPhase2(YobiAction* const action)
{
    auto* const payload = static_cast<MapActorPayload*>(action->payload);
    action->phase = 2;
    payload->counter48 = 0;
}

std::uint16_t ReadYobiDirectionLow16(const YobiAction* const action) noexcept
{
    return static_cast<std::uint16_t>(action->direction);
}

YobiAction* PublishPlayerActionIdentity(
    const char* const name, const char* const titles,
    const std::int32_t originalNameLength, const std::int32_t level,
    const std::int32_t value188)
{
    YobiAction* action = world::detail::ReadPlayerAction();
    if (action == nullptr) {
        return nullptr;
    }
    if (originalNameLength <= 16) {
        std::memmove(action->nameC4.data(), name, std::strlen(name) + 1);
    }

    // 原对象阶段重新解析四段；不能使用上一轮已写回的共享称号。
    char first[256];
    char second[256];
    char third[256];
    char fourth[256];
    network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, first);
    input::UnescapeLegacyTextAndNotify_52D2F0(first);
    if (std::strlen(first) <= 16) {
        action = world::detail::ReadPlayerAction();
        std::strcpy(action->titleE1.data(), first);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, second);
    input::UnescapeLegacyTextAndNotify_52D2F0(second);
    if (std::strlen(second) <= 32) {
        action = world::detail::ReadPlayerAction();
        std::strcpy(action->text102.data(), second);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, third);
    input::UnescapeLegacyTextAndNotify_52D2F0(third);
    if (std::strlen(third) <= 32) {
        action = world::detail::ReadPlayerAction();
        std::strcpy(action->text123.data(), third);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 4, 255, fourth);
    input::UnescapeLegacyTextAndNotify_52D2F0(fourth);
    if (std::strlen(fourth) <= 16) {
        action = world::detail::ReadPlayerAction();
        std::strcpy(action->text31C.data(), fourth);
    }
    action = world::detail::ReadPlayerAction();
    action->field1D0 = level;
    action = world::detail::ReadPlayerAction();
    action->field1D8 = value188;
    return action;
}

void CopyYobiPlayerHp(
    YobiAction* const action, const platform::LegacyCryptInt* const source)
{
    platform::legacy_crypt_int::CopyPayload(&action->readiness, source);
}

void CopyYobiPlayerMaxHp(
    YobiAction* const action, const platform::LegacyCryptInt* const source)
{
    platform::legacy_crypt_int::CopyPayload(&action->maxHp, source);
}

void Sub_442AB0(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY)
{
    if (action == nullptr) {
        return;
    }
    // 本入口以mask开头，既不同于通用赋值，也不同于441DB0的count优先顺序。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        platform::LegacyCryptInt temporary;
        platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, value);
        target.mask = temporary.mask;
        target.encoded = temporary.encoded;
        target.readCount = temporary.readCount;
        platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    };
    assignCoordinate(action->mapX1E0, targetX);
    assignCoordinate(action->mapY1F0, targetY);
    assignCoordinate(action->mapX200, targetX);
    assignCoordinate(action->mapY210, targetY);
    const double worldX = static_cast<double>(targetX);
    action->field27C = 0;
    action->field280 = 0;
    action->worldX = static_cast<float>(worldX * 64.0F);
    action->worldY = static_cast<float>(static_cast<double>(targetY) * 64.0F);
}

void Sub_441F60(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY)
{
    if (action == nullptr) {
        return;
    }
    const std::uint16_t count = action->field270;
    if (count >= 10u) {
        action->field270 = 0;
        Sub_442AB0(action, targetX, targetY);
        return;
    }
    const auto xSlot = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(count));
    action->pathX220[xSlot] = targetX;
    const auto ySlot = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(action->field270));
    action->pathY248[ySlot] = targetY;
    action->field270 = static_cast<std::uint16_t>(action->field270 + 1u);
}

void Sub_441FC0(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY)
{
    if (action == nullptr) {
        return;
    }
    std::int32_t startX;
    std::int32_t startY;
    const std::int16_t count = std::bit_cast<std::int16_t>(action->field270);
    if (count > 0) {
        const auto last = static_cast<std::size_t>(count - 1);
        startX = action->pathX220[last];
        startY = action->pathY248[last];
    } else {
        startX = platform::legacy_crypt_int::Read_401EC0(&action->mapX200);
        startY = platform::legacy_crypt_int::Read_401EC0(&action->mapY210);
    }
    const std::int32_t differenceX = targetX - startX;
    const std::int32_t differenceY = targetY - startY;
    const std::int32_t absoluteX = differenceX < 0 ? -differenceX : differenceX;
    const std::int32_t absoluteY = differenceY < 0 ? -differenceY : differenceY;
    std::array<std::int32_t, 2> pendingX;
    std::array<std::int32_t, 2> pendingY;
    std::int32_t pointCount = 0;
    if (absoluteX == 2 && absoluteY == 2) {
        pendingX[0] = startX + differenceX / 2;
        pendingY[0] = startY + differenceY / 2;
        pointCount = 1;
    } else if (absoluteX == 2) {
        pendingX[0] = startX + differenceX / 2;
        pendingY[0] = targetY;
        pointCount = 1;
    } else if (absoluteY == 2) {
        pendingX[0] = targetX;
        pendingY[0] = startY + differenceY / 2;
        pointCount = 1;
    }
    const std::int32_t currentCount =
        std::bit_cast<std::int16_t>(action->field270);
    pendingX[static_cast<std::size_t>(pointCount)] = targetX;
    pendingY[static_cast<std::size_t>(pointCount)] = targetY;
    ++pointCount;
    if (static_cast<std::uint32_t>(currentCount + pointCount) > 10u) {
        const auto last = static_cast<std::size_t>(pointCount - 1);
        const std::int32_t finalY = pendingY[last];
        const std::int32_t finalX = pendingX[last];
        action->field270 = 0;
        Sub_442AB0(action, finalX, finalY);
        return;
    }
    for (std::int32_t point = 0; point < pointCount; ++point) {
        const auto xSlot = static_cast<std::size_t>(
            std::bit_cast<std::int16_t>(action->field270));
        const std::int32_t x = pendingX[static_cast<std::size_t>(point)];
        action->pathX220[xSlot] = x;
        const std::int32_t y = pendingY[static_cast<std::size_t>(point)];
        const auto ySlot = static_cast<std::size_t>(
            std::bit_cast<std::int16_t>(action->field270));
        action->pathY248[ySlot] = y;
        action->field270 = static_cast<std::uint16_t>(action->field270 + 1u);
    }
}

double Sub_442110(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY, const std::int32_t speed,
    const float overrideRate)
{
    if (action == nullptr) {
        return 0.0;
    }
    std::int16_t pathCount = 0;
    if (!(overrideRate < 0.0F || overrideRate > 0.0F)) {
        pathCount = std::bit_cast<std::int16_t>(action->field270);
    }
    const platform::detail::MapActorVelocityInput input {
        targetX, targetY, speed, pathCount, 0, overrideRate,
        action->worldX, action->worldY,
    };
    platform::detail::MapActorVelocity velocity;
    platform::detail::Cg2dMapActorVelocity(&input, &velocity);
    action->field27C = std::bit_cast<std::uint32_t>(velocity.stepX);
    action->field280 = std::bit_cast<std::uint32_t>(velocity.stepY);
    platform::legacy_crypt_int::AssignFromInt(&action->mapX200, targetX);
    platform::legacy_crypt_int::AssignFromInt(&action->mapY210, targetY);
    if (velocity.directionX < 0.0F || velocity.directionX > 0.0F
        || velocity.directionY < 0.0F || velocity.directionY > 0.0F) {
        action->direction = static_cast<std::int32_t>(
            world::detail::Sub_46CDF0(velocity.directionX, velocity.directionY));
        action->field272 = 1;
    }
    return static_cast<double>(velocity.rate);
}

void Sub_4422F0(
    YobiAction* const action, const std::int32_t targetX,
    const std::int32_t targetY, const std::int32_t speed)
{
    if (action == nullptr) {
        return;
    }
    const std::int32_t pixelX = targetX * 64;
    const std::int32_t pixelY = targetY * 64;
    const platform::detail::MapMoveDifference difference {
        static_cast<double>(pixelX) - action->worldX,
        static_cast<double>(pixelY) - action->worldY,
    };
    platform::detail::MapMoveDirection direction;
    platform::detail::Cg2dMapMoveDirection(&difference, &direction);
    const double rate = static_cast<double>(speed);
    const float percent = std::bit_cast<float>(0x3C23D70Au);
    action->field27C = std::bit_cast<std::uint32_t>(
        static_cast<float>(rate * direction.x * 4.0F * percent));
    action->field280 = std::bit_cast<std::uint32_t>(
        static_cast<float>(rate * direction.y * 4.0F * percent));

    platform::LegacyCryptInt temporary;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, targetX);
    action->mapX200.mask = temporary.mask;
    action->mapX200.encoded = temporary.encoded;
    action->mapX200.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, targetY);
    action->mapY210.mask = temporary.mask;
    action->mapY210.encoded = temporary.encoded;
    action->mapY210.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    if (direction.x < 0.0F || direction.x > 0.0F
        || direction.y < 0.0F || direction.y > 0.0F) {
        action->direction = static_cast<std::int32_t>(
            world::detail::Sub_46CDF0(direction.x, direction.y));
        action->field272 = 1;
    }
}

void Sub_442460(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const std::int16_t count = std::bit_cast<std::int16_t>(action->field270);
    if (count > 0) {
        action->field270 = static_cast<std::uint16_t>(count - 1);
    }
    for (std::int32_t index = 0;
         index < std::bit_cast<std::int16_t>(action->field270); ++index) {
        const auto slot = static_cast<std::size_t>(index);
        action->pathX220[slot] = action->pathX220[slot + 1];
        action->pathY248[slot] = action->pathY248[slot + 1];
    }
}

void Sub_43EFC0(
    YobiAction* const action, const std::int32_t characterAction,
    const std::int32_t restart)
{
    auto* const payload = static_cast<MapActorPayload*>(action->payload);
    YobiAction* const ride = payload->action44;
    if (ride != nullptr) {
        ride->characterAction = characterAction;
        const std::int32_t imageNo = action->imageNo;
        if ((imageNo >= 114000 && imageNo <= 114169)
            || (imageNo >= 112000 && imageNo <= 112007)
            || (imageNo >= 112010 && imageNo <= 112028)
            || (imageNo >= 120148 && imageNo <= 120155)) {
            switch (payload->action44->characterAction) {
            case 0: action->characterAction = 0; break;
            case 1: action->characterAction = 1; break;
            case 2:
            case 3:
            case 4: action->characterAction = 3; break;
            case 5:
            case 6: action->characterAction = 5; break;
            case 8:
            case 10: action->characterAction = 8; break;
            case 9: action->characterAction = 9; break;
            default:
                action->characterAction = 0;
                payload->action44->characterAction = 0;
                break;
            }
        } else if ((imageNo >= 100000 && imageNo <= 100999)
            || (imageNo >= 105000 && imageNo <= 106800)
            || (imageNo >= 104400 && imageNo <= 104520)
            || (imageNo >= 120100 && imageNo <= 120147)
            || imageNo == 100900 || imageNo == 100901) {
            action->characterAction = 11;
        } else {
            action->characterAction = 0;
        }
    } else {
        action->characterAction = characterAction;
    }

    if (restart != 0) {
        action->lastCharacterAction = -1;
        YobiAction* attachment = payload->action44;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
        attachment = payload->action34;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
        attachment = payload->action38;
        if (attachment != nullptr) {
            attachment->lastCharacterAction = -1;
        }
    }
}

void Sub_4424B0(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float firstTargetX = static_cast<float>(static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&action->mapX200)) * 64.0F);
    const double firstTargetY = static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&action->mapY210)) * 64.0F;
    const float firstWorldX = action->worldX;
    // 原 FCOMP/C3；Y在首轮不落F32，只有X比较通过才读取世界Y。
    if (!(firstTargetX < firstWorldX || firstTargetX > firstWorldX)) {
        const float firstWorldY = action->worldY;
        if (!(firstTargetY < firstWorldY || firstTargetY > firstWorldY)
            && std::bit_cast<std::int16_t>(action->field270) > 0) {
            const std::int32_t previousX =
                platform::legacy_crypt_int::Read_401EC0(&action->mapX1E0);
            const std::int32_t previousY =
                platform::legacy_crypt_int::Read_401EC0(&action->mapY1F0);
            const std::int32_t nextY = action->pathY248[0];
            const std::int32_t nextX = action->pathX220[0];
            const std::int32_t speed = action->walkSpeed;
            const float rate =
                static_cast<float>(Sub_442110(action, nextX, nextY, speed, 0.0F));
            Sub_442460(action);
            if ((action->flags & 0x200000u) == 0) {
                auto* const payload = static_cast<MapActorPayload*>(action->payload);
                YobiAction* const child = payload->action2C;
                if (child != nullptr) {
                    Sub_442110(child, previousX, previousY, child->walkSpeed, rate);
                }
            }
        }
    }

    const float targetX = static_cast<float>(static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&action->mapX200)) * 64.0F);
    const float targetY = static_cast<float>(static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&action->mapY210)) * 64.0F);
    const float movementX = std::bit_cast<float>(action->field27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->field280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->field272 != 0) {
            Sub_43EFC0(action, 0, 0);
        }
        action->field272 = 0;
    } else {
        const float stepY = std::bit_cast<float>(action->field280);
        const float stepX = std::bit_cast<float>(action->field27C);
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const double distance = world::detail::Sub_46FB80(
            worldX, worldY, targetX, targetY);
        const double squaredY = static_cast<double>(stepY) * stepY;
        const double squaredX = static_cast<double>(stepX) * stepX;
        const double squaredStep = squaredY + squaredX;
        if (!(squaredStep >= distance)) {
            action->worldX = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->field27C))
                + action->worldX);
            action->worldY = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->field280))
                + action->worldY);
        } else {
            action->worldX = targetX;
            action->worldY = targetY;
            action->field27C = 0;
            action->field280 = 0;
        }
        Sub_43EFC0(action, 1, 0);
    }

    // 此入口先写count，不使用encoded优先的通用AssignFromInt。
    platform::LegacyCryptInt temporary;
    const auto mapX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(action->worldX) * 0.015625F));
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, mapX);
    action->mapX1E0.readCount = temporary.readCount;
    action->mapX1E0.encoded = temporary.encoded;
    action->mapX1E0.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    const auto mapY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(action->worldY) * 0.015625F));
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, mapY);
    action->mapY1F0.readCount = temporary.readCount;
    action->mapY1F0.encoded = temporary.encoded;
    action->mapY1F0.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
}

void Sub_442730(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float targetX = static_cast<float>(static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&action->mapX200)) * 64.0F);
    const float targetY = static_cast<float>(static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&action->mapY210)) * 64.0F);
    const float movementX = std::bit_cast<float>(action->field27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->field280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->field272 != 0) {
            Sub_43EFC0(action, 0, 0);
        }
        action->field272 = 0;
    } else {
        const float stepY = std::bit_cast<float>(action->field280);
        const float stepX = std::bit_cast<float>(action->field27C);
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const double distance = world::detail::Sub_46FB80(
            worldX, worldY, targetX, targetY);
        const double squaredY = static_cast<double>(stepY) * stepY;
        const double squaredX = static_cast<double>(stepX) * stepX;
        const double squaredStep = squaredY + squaredX;
        if (!(squaredStep >= distance)) {
            action->worldX = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->field27C))
                + action->worldX);
            action->worldY = static_cast<float>(
                static_cast<double>(std::bit_cast<float>(action->field280))
                + action->worldY);
        } else {
            action->worldX = targetX;
            action->worldY = targetY;
            action->field27C = 0;
            action->field280 = 0;
        }
        Sub_43EFC0(action, 1, 0);
    }

    platform::LegacyCryptInt temporary;
    const auto mapX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(action->worldX) * 0.015625F));
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, mapX);
    action->mapX1E0.readCount = temporary.readCount;
    action->mapX1E0.encoded = temporary.encoded;
    action->mapX1E0.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    const auto mapY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(action->worldY) * 0.015625F));
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, mapY);
    action->mapY1F0.readCount = temporary.readCount;
    action->mapY1F0.encoded = temporary.encoded;
    action->mapY1F0.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
}

void Sub_4428F0(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const float targetX = static_cast<float>(static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&action->mapX200)) * 64.0F);
    const float targetY = static_cast<float>(static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&action->mapY210)) * 64.0F);
    const float movementX = std::bit_cast<float>(action->field27C);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(action->field280);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (action->field272 != 0) {
            Sub_43EFC0(action, 0, 0);
        }
        action->field272 = 0;
    } else {
        const float rateX = std::bit_cast<float>(world::detail::gValue9DAD78);
        const float rawStepX = std::bit_cast<float>(action->field27C);
        const double scaledX = static_cast<double>(rateX) * rawStepX;
        const float worldY = action->worldY;
        const float worldX = action->worldX;
        const float stepX = static_cast<float>(scaledX);
        const float rateY = std::bit_cast<float>(world::detail::gValue9DAD78);
        const float rawStepY = std::bit_cast<float>(action->field280);
        const float stepY = static_cast<float>(static_cast<double>(rateY) * rawStepY);
        const double distance = world::detail::Sub_46FB80(
            worldX, worldY, targetX, targetY);
        const double squaredY = static_cast<double>(stepY) * stepY;
        const double squaredX = static_cast<double>(stepX) * stepX;
        const double squaredStep = squaredY + squaredX;
        if (!(squaredStep >= distance)) {
            action->worldX =
                static_cast<float>(static_cast<double>(stepX) + action->worldX);
            action->worldY =
                static_cast<float>(static_cast<double>(stepY) + action->worldY);
        } else {
            action->worldX = targetX;
            action->worldY = targetY;
            action->field27C = 0;
            action->field280 = 0;
        }
        Sub_43EFC0(action, 1, 0);
    }

    // 倍率入口与442730不同：mask→encoded→count，不提前复制目标载荷。
    platform::LegacyCryptInt temporary;
    const auto mapX = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(action->worldX) * 0.015625F));
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, mapX);
    action->mapX1E0.mask = temporary.mask;
    action->mapX1E0.encoded = temporary.encoded;
    action->mapX1E0.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    const auto mapY = static_cast<std::int32_t>(static_cast<std::int64_t>(
        static_cast<double>(action->worldY) * 0.015625F));
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, mapY);
    action->mapY1F0.mask = temporary.mask;
    action->mapY1F0.encoded = temporary.encoded;
    action->mapY1F0.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
}

void ClearMapActorAttachments(YobiAction* const action)
{
    auto* const payload = static_cast<MapActorPayload*>(action->payload);
    if (payload == nullptr) {
        return;
    }

    // 原清理不包含 +34/+38；只保存槽地址，每轮调用前才读取对象指针。
    YobiAction** const slots[] {
        &payload->action04, &payload->action08, &payload->action0C,
        &payload->action10, &payload->action2C, &payload->action4C,
        &payload->action30, &payload->action44,
    };
    for (YobiAction** const slot : slots) {
        YobiAction* const child = *slot;
        if (child != nullptr) {
            MarkDeath(child);
            *slot = nullptr;
        }
    }
    char* const text = payload->text1C;
    if (text != nullptr) {
        std::free(text);
        payload->text1C = nullptr;
    }
}

YobiAction* Sub_46DE50(
    const std::int32_t imageNo, const std::int32_t mapX, const std::int32_t mapY)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->direction = 0;
    action->displayPriority = 10;
    action->flags = 0x30;
    platform::legacy_crypt_int::AssignFromInt(&action->mapX200, mapX);
    platform::legacy_crypt_int::AssignFromInt(&action->mapY210, mapY);
    action->field270 = 0;
    platform::legacy_crypt_int::AssignFromInt(&action->mapX1E0, mapX);
    platform::legacy_crypt_int::AssignFromInt(&action->mapY1F0, mapY);
    const double scaledX = static_cast<double>(mapX) * 64.0f;
    action->field27C = 0;
    action->field280 = 0;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(static_cast<double>(mapY) * 64.0f);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(static_cast<double>(screenX) + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(static_cast<double>(screenY) + 0.5));
    return action;
}

void Sub_46DD90(YobiAction* const action)
{
    const float sceneY = action->worldY;
    const float sceneX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(sceneX, sceneY, &screenX, &screenY);
    const auto x = static_cast<std::int64_t>(static_cast<double>(screenX) + 0.5);
    const double y = static_cast<double>(screenY) + 0.5;
    action->x = static_cast<std::int32_t>(x);
    action->y = static_cast<std::int32_t>(static_cast<std::int64_t>(y));
    const auto worldX = static_cast<float>(
        platform::legacy_crypt_int::Multiply_401E90(&action->mapX1E0, 64));
    const auto worldY = static_cast<float>(
        platform::legacy_crypt_int::Multiply_401E90(&action->mapY1F0, 64));
    static_cast<void>(Sub_517880(action, 100, 0, 0));
    const std::int32_t offsetY = action->mapOffsetY;
    const std::int32_t offsetX = action->mapOffsetX;
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t drawX = offsetX + action->x;
    const std::int32_t imageNo = action->graphicIndex;
    world::detail::Sub_46DFF0(
        imageNo, std::bit_cast<std::uint32_t>(worldX), std::bit_cast<std::uint32_t>(worldY),
        static_cast<std::int16_t>(drawX), static_cast<std::int16_t>(drawY), &action->drawMode);
}

YobiAction* Sub_43F180(YobiAction* const action)
{
    YobiAction* current = gMapCharaHead;
    if (current == nullptr) {
        gMapCharaHead = action;
        return nullptr;
    }
    for (;;) {
        auto* const payload = static_cast<MapCharaPayload*>(current->payload);
        if (payload->next == nullptr) {
            payload->next = action;
            return action;
        }
        current = payload->next;
    }
}

void* Sub_43F1B0(YobiAction* const action)
{
    YobiAction* current = gMapCharaHead;
    if (current == action) {
        auto* const payload = static_cast<MapCharaPayload*>(action->payload);
        gMapCharaHead = payload->next;
        return payload;
    }
    while (current != nullptr) {
        auto* const payload = static_cast<MapCharaPayload*>(current->payload);
        YobiAction* const next = payload->next;
        if (next == action) {
            auto* const removed = static_cast<MapCharaPayload*>(next->payload);
            payload->next = removed->next;
            return removed;
        }
        current = next;
    }
    return nullptr;
}

YobiAction* Sub_43F1F0(const std::uint32_t value)
{
    YobiAction* current = gMapCharaHead;
    while (current != nullptr) {
        const auto* const payload =
            static_cast<const MapCharaPayload*>(current->payload);
        for (const std::uint32_t entry : payload->values) {
            if (entry == value) {
                return current;
            }
        }
        current = payload->next;
    }
    return nullptr;
}

void Sub_43F220()
{
    gMapCharaHead = nullptr;
}

void* Sub_43F230()
{
    YobiAction* current = gMapCharaHead;
    YobiAction* last = nullptr;
    while (current != nullptr) {
        const auto* const payload =
            static_cast<const MapCharaPayload*>(current->payload);
        YobiAction* const next = payload->next;
        for (YobiAction* const child : payload->partyActions) {
            if (child != nullptr) {
                MarkDeath(child);
            }
        }
        last = MarkDeath(current);
        current = next;
    }
    gMapCharaHead = nullptr;
    // 空链时原 EAX 未定义；新端返回空值，不作为原端等价结果验收。
    return last;
}

std::int32_t* Sub_43FD90(
    const std::int32_t imageNo, const std::int32_t slot,
    const std::int32_t direction, std::int32_t* const x, std::int32_t* const y)
{
    std::int32_t group = 0;
    if (imageNo >= 110782 && imageNo <= 110787) {
        group = 1;
    } else if (imageNo >= 110788 && imageNo <= 110793) {
        group = 2;
    }
    const std::int32_t index = (group * 5 + slot) * 8 + direction;
    const std::int32_t oldX = *x;
    *x = oldX + kMapPartyOffsets54D950[index].x;
    const std::int32_t offsetY = kMapPartyOffsets54D950[index].y;
    *y += offsetY;
    return y;
}

std::int32_t Sub_5182B0(YobiAction* const action, const std::uint8_t axis)
{
    std::int16_t x;
    std::int16_t y;
    resource::GetGraphicXY(
        static_cast<std::uint32_t>(action->graphicIndex), &x, &y);
    if (axis == 0) {
        return action->graphicOffsetX - x;
    }
    if (axis == 1) {
        return action->graphicOffsetY - y;
    }
    return 0;
}

// 原 440C50；唯一 callback 写者为 440B70，见 Yobi 观察回调 SPEC。
void Sub_440C50(YobiAction* const action)
{
    auto* const payload = static_cast<MapCharaPayload*>(action->payload);
    platform::detail::MapObserverProjection projection;
    platform::detail::Cg2dMapObserverProjection(
        payload->frame, &gMapDuration5F9744, action->direction, &projection);
    Sub_517880(action, 100, 0, 0);
    action->x = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(projection.x)) + 320;
    const auto arc = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        platform::detail::Cg2dMapObserverArc(&projection.progress)));
    action->y = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(projection.y)) - arc + 290;

    std::array<std::int32_t, 5> drawX;
    std::array<std::int32_t, 5> drawY;
    std::array<std::int8_t, 5> order;
    std::size_t count = 0;
    for (std::size_t index = 0; index < payload->partyActions.size(); ++index) {
        if (payload->partyActions[index] == nullptr) {
            continue;
        }
        std::int32_t offsetX = 0;
        std::int32_t offsetY = 0;
        Sub_517880(payload->partyActions[index], 100, 0, 0);
        YobiAction* const child = payload->partyActions[index];
        const std::int32_t direction = child->direction;
        const std::int32_t slot = child->field1D0;
        Sub_43FD90(action->imageNo, slot, direction, &offsetX, &offsetY);
        const std::int32_t graphicX = Sub_5182B0(payload->partyActions[index], 0);
        drawX[index] = graphicX + action->frameX + action->x + offsetX;
        const std::int32_t graphicY = Sub_5182B0(payload->partyActions[index], 1);
        const std::int32_t y = graphicY + action->frameY + action->y + offsetY;
        order[count] = static_cast<std::int8_t>(index);
        drawY[index] = y;
        ++count;
    }
    for (std::size_t index = count; index < order.size(); ++index) {
        order[index] = -1;
    }

    // 原 440E41–440EB9 按子对象 +3C 降序换索引，不按 drawY 排序。
    for (std::size_t index = 0; index < order.size(); ++index) {
        if (order[index] < 0) {
            break;
        }
        for (std::size_t next = index + 1; next < order.size(); ++next) {
            if (order[next] < 0) {
                break;
            }
            if (payload->partyActions[order[index]]->deltaY
                < payload->partyActions[order[next]]->deltaY) {
                const std::int8_t previous = order[index];
                order[index] = order[next];
                order[next] = previous;
            }
        }
    }
    for (const std::int8_t index : order) {
        if (index < 0) {
            break;
        }
        YobiAction* const child = payload->partyActions[index];
        render::DrawGraphic2_521DD0(
            drawX[index], drawY[index], 10, child->graphicIndex, 0, 0,
            &child->drawMode);
    }
    const std::int32_t graphicIndex = action->graphicIndex;
    const render::GraphicDescriptor16* const mode = &action->drawMode;
    const std::int32_t offsetY = Sub_5182B0(action, 1);
    const std::int32_t y = offsetY + action->y;
    const std::int32_t offsetX = Sub_5182B0(action, 0);
    const std::int32_t x = offsetX + action->x;
    render::DrawGraphic2_521DD0(x, y, 10, graphicIndex, 0, 0, mode);
    ++payload->frame;
}

YobiAction* Sub_440B70()
{
    YobiAction* const action = NewMapCharaAction();
    if (action == nullptr) {
        return nullptr;
    }

    action->callback = Sub_440C50;
    action->imageNo = gMapImage5F9734;
    action->characterAction = 3;
    action->direction = gMapDirection5F9740;
    action->displayPriority = 10;
    action->flags = 0x20;
    action->x = -100;
    action->y = -100;
    static_cast<void>(Sub_43F180(action));

    auto* const payload = static_cast<MapCharaPayload*>(action->payload);
    for (std::size_t index = 0; index < payload->values.size(); ++index) {
        payload->values[index] = 0;
        payload->partyActions[index] = nullptr;
        const std::int32_t value = gMapValues5F9720[index];
        if (value <= 0) {
            continue;
        }

        payload->values[index] = static_cast<std::uint32_t>(value);
        YobiAction* const child = NewYobiAction_404AE0(2, 0);
        payload->partyActions[index] = child;
        if (child != nullptr) {
            child->imageNo = gMapImages5AC4A0[index];
            child->direction = gMapDirection5F9740;
            child->characterAction = 3;
            child->field1D0 = static_cast<std::int32_t>(index);
        }
    }
    return action;
}

void* Sub_440F50()
{
    YobiAction* const action =
        Sub_43F1F0(world::ReadPlayerObjectIndex_FDA880());
    if (action == nullptr) {
        return nullptr;
    }

    auto* const payload = static_cast<MapCharaPayload*>(action->payload);
    gMapImage5F9734 = action->imageNo;
    gMapDirection5F9740 = action->field58;
    gMapDuration5F9744 =
        platform::legacy_crypt_int::Multiply_401E90(&action->readiness, 60);
    for (std::size_t index = 0; index < payload->values.size(); ++index) {
        gMapValues5F9720[index] = 0;
        gMapImages5AC4A0[index] = 0;
        const std::int32_t value =
            std::bit_cast<std::int32_t>(payload->values[index]);
        if (value > 0) {
            gMapValues5F9720[index] = value;
            YobiAction* const child = payload->partyActions[index];
            if (child != nullptr) {
                gMapImages5AC4A0[index] = child->imageNo;
            }
        }
    }
    return payload->partyActions.data();
}

std::int32_t ReadMapObservationDirection() noexcept
{
    return gMapDirection5F9740;
}

std::int32_t Sub_440FE0()
{
    static_cast<void>(Sub_440B70());
    static_cast<void>(audio::Sub_52B990());
    return 0x25;
}

YobiAction* NewYobiGear_4410F0(const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }

    action->callback = nullptr;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->displayPriority = 10;
    action->flags = 0x34;
    return action;
}

std::int32_t YobiUpdateAll_404C50()
{
    YobiAction* current = gYobiActionStart->next;
    while (current != gYobiActionEnd) {
        if (current->deathFlag != 0) {
            if (ShouldDeferForFourPhaseGate(current)) {
                current = current->next;
                continue;
            }

            current->prev->next = current->next;
            current->next->prev = current->prev;
            YobiAction* const next = current->next;
            DeleteYobiAction_404D10(current);
            current = next;
            continue;
        }

        const YobiCallback callback = current->callback;
        if (callback != nullptr && !ShouldDeferForFourPhaseGate(current)) {
            callback(current);
        }
        current = current->next;
    }

    const std::int32_t result = ++gYobiUpdatePhaseCounter;
    if (gYobiUpdatePhaseCounter >= 4) {
        gYobiUpdatePhaseCounter = 0;
    }
    return result;
}

YobiAction* MarkDeath(YobiAction* const action)
{
    if (action != nullptr) {
        action->deathFlag = 1;
    }
    return action;
}

void YobiClearUp_404D30()
{
    YobiAction* current = gYobiActionStart->next;
    Sub_46DD40();
    while (current != gYobiActionEnd) {
        current->deathFlag = 1;
        current = current->next;
    }
}

void Sub_404D60()
{
    YobiClearUp_404D30();
    YobiUpdateAll_404C50();
    std::free(gYobiActionStart);
    std::free(gYobiActionEnd);
}

YobiAction* Sub_448370(void (*callback)(YobiAction*))
{
    // 原 0x448370/0x448379/0x448380：先读首槽、缓存尾槽，再读 start->next。
    YobiAction* const start = gYobiActionStart;
    YobiAction* const end = gYobiActionEnd;
    YobiAction* current = start->next;
    while (current != end) {
        if (current->callback == callback && current->deathFlag == 0) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// 原 [0x49C5C0,0x49C5DB)：S16 宽度先减 48，再按有符号商加 2。
std::int32_t Sub_49C5C0(const std::int16_t width)
{
    return (static_cast<std::int32_t>(width) - 48) / 9 + 2;
}

// 原 [0x49C5E0,0x49C5FD)：非零标志扣 24，再扣 32 并用 idiv 求商。
std::int32_t Sub_49C5E0(
    const std::int16_t height,
    const std::int32_t marginFlag,
    const std::int32_t divisor)
{
    return (static_cast<std::int32_t>(height)
            - (marginFlag != 0 ? 24 : 0) - 32)
        / divisor;
}

YobiAction* Sub_5044E0(
    const std::int32_t x,
    const std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    const std::int32_t winId,
    const std::int32_t param6)
{
    YobiAction* const action = NewYobiAction_404AE0(
        kModalPriority,
        static_cast<std::uint32_t>(sizeof(YobiUIData)));
    if (action == nullptr) {
        return nullptr;
    }

    auto* const payload = static_cast<YobiUIData*>(action->payload);
    action->flags |= 6U;
    action->callback = UpdateModalAction_5046D0;
    action->displayPriority = kModalDisplayPriority;

    if (winId < 0) {
        payload->tileStyle = 0;
        payload->baseImageNo = 0;
    } else {
        action->field1D0 = param6;
        if (param6 != 0) {
            if (width % 64 != 0) {
                width = (width / 64 + 1) * 64;
            }
            if (height % 48 != 0) {
                height = (height / 48 + 1) * 48;
            }
        }

        payload->tileStyle =
            winId != 2 && winId != 3 && winId != 5
                && winId != 7 && winId != 9
            ? std::int16_t{1}
            : std::int16_t{0};

        if (winId == 0 || winId == 2) {
            payload->baseImageNo = 22620;
        } else if (winId == 1 || winId == 3) {
            payload->baseImageNo = 22630;
        }
        switch (winId >> 1) {
        case 2:
            payload->baseImageNo = 245300;
            break;
        case 3:
            payload->baseImageNo = 245310;
            break;
        case 4:
            payload->baseImageNo = 245380;
            break;
        default:
            break;
        }
    }

    action->x = x;
    action->y = y;
    payload->width = width;
    payload->height = height;
    payload->winId = static_cast<std::int16_t>(winId);
    payload->frameCount = 0;

    if (winId == -1) {
        payload->centerX = x + width / 2;
        payload->centerY = y + height / 2;
        action->deltaX = (payload->centerX - x) / 10;
        action->deltaY = (payload->centerY - y) / 10;
    } else if (winId == -2 || winId >= 0) {
        payload->centerX = x + width / 2;
        payload->centerY = y + height / 2;
        action->deltaX = 0;
        action->deltaY = (payload->centerY - y) / 10;
        payload->currentHalfWidth = payload->centerX - x;
    }
    return action;
}

bool IsYobiActionReady(
    YobiAction* const action,
    const std::int32_t threshold)
{
    return platform::legacy_crypt_int::GreaterEqual_401D70(
        &action->readiness,
        threshold);
}

bool IsYobiActionStateEqual(
    YobiAction* const action,
    const std::int32_t expected)
{
    return platform::legacy_crypt_int::Equal_401D00(
        &action->readiness,
        expected);
}

bool IsYobiActionStateLessOrEqual(
    YobiAction* const action,
    const std::int32_t maximum)
{
    return platform::legacy_crypt_int::LessEqual_401DA0(
        &action->readiness,
        maximum);
}

std::int32_t Sub_517880(
    YobiAction* const action,
    const std::int32_t rate,
    const std::int32_t stopAtEnd,
    const std::uint32_t flipStatic)
{
    if (action == nullptr || action->hidden == 1) {
        return 0;
    }
    const std::int32_t characterAction = action->characterAction;
    action->playRevert = stopAtEnd;
    std::int32_t imageNo;
    if (action->lastCharacterAction != characterAction
        || action->direction != action->lastDirection
        || (imageNo = action->imageNo) != action->lastImageNo) {
        imageNo = action->imageNo;
        action->lastCharacterAction = characterAction;
        action->lastImageNo = imageNo;
        action->lastDirection = action->direction;
        action->frameRemain = 0;
        action->frameIndex = 0;
        action->completedLoops = 0;
    }
    if (action->frameRemain != 0) {
        --action->frameRemain;
        return 0;
    }
    if (imageNo <= 999) {
        action->graphicIndex = 0;
        return 0;
    }
    if (imageNo < 100000 || imageNo >= 200000) {
        std::int32_t graphicIndex;
        resource::GetGraphicMapNoIndex(imageNo, &graphicIndex);
        std::int16_t offsetX;
        std::int16_t offsetY;
        resource::GetGraphicXY(static_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
        action->graphicOffsetY = offsetY;
        action->graphicIndex = graphicIndex;
        action->graphicOffsetX = offsetX;
        action->frameRemain = 0x7FFFFFFF;
        const std::int16_t paletteIndex = 0;
        std::memcpy(action->drawMode.bytes.data() + 0x0E, &paletteIndex, sizeof(paletteIndex));
        action->drawMode.bytes[0x0D] = 0;
        const std::uint32_t color = 0xFFFFFFFFu;
        std::memcpy(action->drawMode.bytes.data() + 8, &color, sizeof(color));
        if (static_cast<std::uint8_t>(flipStatic) != 0) {
            action->drawMode.bytes[0x0D] = 0;
            if (action->direction >= 2 && action->direction < 5) {
                action->drawMode.bytes[0x0D] = 1;
            }
            ApplyYobiGraphicFlip(action);
        }
        action->frameX = 0;
        action->frameY = 0;
        return 0;
    }
    if (imageNo > resource::animation::gMaxAnimateCount9A4A4C + 100000) {
        return 1;
    }
    const std::int32_t animeIndex =
        resource::animation::GetIndex_45B400(static_cast<std::uint32_t>(imageNo - 100000));
    if (animeIndex < 0) {
        return 1;
    }
    if (resource::animation::AnimeLoadAction_45B760(animeIndex) == 0) {
        return -1;
    }
    const auto* const list = static_cast<resource::animation::detail::ActionEntry*>(
        resource::animation::gAnimateRecords9A4A58[animeIndex].actionList);
    const std::uint16_t count = resource::animation::gAnimateRecords9A4A58[animeIndex].count;
    if (count == 0) {
        return 1;
    }
    std::int32_t selectedIndex = 0;
    const std::int32_t direction = action->direction;
    for (; selectedIndex < count; ++selectedIndex) {
        if (direction == static_cast<std::uint16_t>(list[selectedIndex].direction)
            && static_cast<std::uint16_t>(list[selectedIndex].action) == action->characterAction) {
            break;
        }
    }
    if (selectedIndex >= count) {
        return 1;
    }
    const auto* const selected = &list[selectedIndex];
    const std::uint32_t product = selected->duration * static_cast<std::uint32_t>(rate);
    const auto* const frames =
        static_cast<resource::animation::detail::FrameEntry*>(selected->frames);
    action->frameRemain = static_cast<std::int32_t>(product / 100u);
    if (action->frameRemain < 1) {
        action->frameRemain = 1;
    }
    if (action->frameIndex >= selected->frameCount) {
        if (stopAtEnd == 1) {
            action->frameRemain = 255;
            action->completedLoops = 1;
            return 1;
        }
        action->frameIndex = 0;
        ++action->completedLoops;
    }
    ApplyYobiAnimeFrame(action, animeIndex, selected, frames);
    return 0;
}

std::int32_t PlayFrame_517DA0(YobiAction* const action, const std::int32_t stopAtEnd)
{
    if (action == nullptr || action->hidden == 1) {
        return 0;
    }
    const std::int32_t characterAction = action->characterAction;
    action->playRevert = stopAtEnd;
    std::int32_t imageNo;
    if (action->lastCharacterAction != characterAction
        || action->direction != action->lastDirection
        || (imageNo = action->imageNo) != action->lastImageNo) {
        imageNo = action->imageNo;
        action->lastCharacterAction = characterAction;
        action->lastImageNo = imageNo;
        action->lastDirection = action->direction;
        action->frameRemain = 0;
        action->frameIndex = 0;
        action->completedLoops = 0;
    }
    if (action->frameRemain != 0) {
        --action->frameRemain;
        return 0;
    }
    if (imageNo <= 999) {
        action->graphicIndex = 0;
        return 0;
    }
    if (imageNo < 100000) {
        std::int32_t graphicIndex;
        resource::GetGraphicMapNoIndex(imageNo, &graphicIndex);
        std::int16_t offsetX;
        std::int16_t offsetY;
        resource::GetGraphicXY(static_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
        action->graphicOffsetX = offsetX;
        const std::int32_t direction = action->direction;
        action->graphicIndex = graphicIndex;
        action->graphicOffsetY = offsetY;
        action->frameRemain = 0x7FFFFFFF;
        const std::int16_t paletteIndex = 0;
        std::memcpy(action->drawMode.bytes.data() + 0x0E, &paletteIndex, sizeof(paletteIndex));
        const std::uint32_t color = 0xFFFFFFFFu;
        std::memcpy(action->drawMode.bytes.data() + 8, &color, sizeof(color));
        action->drawMode.bytes[0x0D] = 0;
        if (direction >= 2 && direction < 5) {
            action->drawMode.bytes[0x0D] = 1;
        }
        ApplyYobiGraphicFlip(action);
        action->frameX = 0;
        action->frameY = 0;
        return 0;
    }
    if (imageNo > resource::animation::gMaxAnimateCount9A4A4C + 100000) {
        return 1;
    }
    const std::int32_t animeIndex =
        resource::animation::GetIndex_45B400(static_cast<std::uint32_t>(imageNo - 100000));
    if (animeIndex < 0) {
        return 1;
    }
    if (resource::animation::AnimeLoadAction_45B760(animeIndex) == 0) {
        return -1;
    }
    const auto* const list = static_cast<resource::animation::detail::ActionEntry*>(
        resource::animation::gAnimateRecords9A4A58[animeIndex].actionList);
    const std::uint16_t count = resource::animation::gAnimateRecords9A4A58[animeIndex].count;
    if (count == 0) {
        return 1;
    }
    std::int32_t selectedIndex = 0;
    const std::int32_t direction = action->direction;
    for (; selectedIndex < count; ++selectedIndex) {
        if (direction == static_cast<std::uint16_t>(list[selectedIndex].direction)
            && static_cast<std::uint16_t>(list[selectedIndex].action) == action->characterAction) {
            break;
        }
    }
    if (selectedIndex >= count) {
        return 1;
    }
    const auto* const selected = &list[selectedIndex];
    const std::uint32_t product =
        selected->duration * static_cast<std::uint32_t>(action->framePlayingRate);
    const auto* const frames =
        static_cast<resource::animation::detail::FrameEntry*>(selected->frames);
    action->frameRemain = static_cast<std::int32_t>(product / 100u);
    if (action->frameRemain < 1) {
        action->frameRemain = 1;
    }
    if (action->frameIndex >= selected->frameCount) {
        if (stopAtEnd == 1) {
            action->frameRemain = 255;
            action->completedLoops = 1;
            return 1;
        }
        action->frameIndex = 0;
        ++action->completedLoops;
    }
    ApplyYobiAnimeFrame(action, animeIndex, selected, frames);
    return 0;
}

float GetYobiActionScaleX(const YobiAction* const action) noexcept
{
    return action->scaleX;
}

float GetYobiActionScaleY(const YobiAction* const action) noexcept
{
    return action->scaleY;
}

void ClearYobiActionCallback(YobiAction* const action)
{
    action->callback = nullptr;
}

std::int32_t GetYobiActionField20(YobiAction* const action)
{
    return action->graphicIndex;
}

std::int32_t GetYobiActionField2E4(const YobiAction* const action) noexcept
{
    return action->field2E4;
}

std::int32_t GetYobiActionField2A8(YobiAction* const action)
{
    return action->imageNo;
}

void SetYobiActionField2A8(
    YobiAction* const action,
    const std::int32_t value)
{
    action->imageNo = value;
}

void SetYobiActionField2B0(
    YobiAction* const action,
    const std::int32_t value)
{
    action->characterAction = value;
}

void SetYobiActionField15(
    YobiAction* const action,
    const std::uint8_t value)
{
    action->displayPriority = value;
}

std::int32_t GetYobiActionField30(YobiAction* const action)
{
    return action->x;
}

void SetYobiActionField30(
    YobiAction* const action,
    const std::int32_t value)
{
    action->x = value;
}

std::int32_t GetYobiActionField34(YobiAction* const action)
{
    return action->y;
}

void SetYobiActionField34(
    YobiAction* const action,
    const std::int32_t value)
{
    action->y = value;
}

void SetYobiActionField2B8(
    YobiAction* const action,
    const std::int32_t value)
{
    action->direction = value;
}

void SetYobiActionField308(
    YobiAction* const action,
    const std::uint8_t value)
{
    action->batchType = value;
}

void SetYobiActionField30C(
    YobiAction* const action,
    const std::int32_t value)
{
    action->color2 = value;
}

std::uint8_t GetYobiActionField30F(YobiAction* const action)
{
    const std::uint32_t bits = std::bit_cast<std::uint32_t>(action->color2);
    return static_cast<std::uint8_t>(bits >> 24U);
}

void SetYobiActionField30F(
    YobiAction* const action,
    const std::uint8_t value)
{
    const std::uint32_t bits =
        std::bit_cast<std::uint32_t>(action->color2);
    const std::uint32_t updated =
        (bits & 0x00FFFFFFU) | (std::uint32_t{value} << 24U);
    action->color2 = std::bit_cast<std::int32_t>(updated);
}

} // namespace detail
} // namespace cg::login

namespace cg::world::detail {

void Sub_46B4B0(const std::int32_t direction)
{
    if (direction < 0 || direction > 7) {
        return;
    }
    login::YobiAction* const action = ReadPlayerAction();
    if (action == nullptr) {
        return;
    }
    const float movementX = std::bit_cast<float>(gValue9EE8FC);
    if (movementX < 0.0F || movementX > 0.0F) {
        return;
    }
    const float movementY = std::bit_cast<float>(gValue9EE900);
    if (movementY < 0.0F || movementY > 0.0F
        || gValue62B5AA != 0 || gValueC82220 != 0
        || action->direction == direction) {
        return;
    }
    const std::uint32_t now = platform::ReadTickCount32();
    if (gValueC8223C + 500u >= now) {
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
    gValueC8223C = platform::ReadTickCount32();
}

std::uint32_t Sub_46A190()
{
    for (std::size_t position = 0; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        const std::uint32_t playerIndex = ReadPlayerObjectIndex_FDA880();
        login::YobiAction* const action = ReadPetMapAction(index);
        const bool isPlayer = ReadPetMapObjectIndex(index) == playerIndex;
        if (isPlayer) {
            const double targetX = static_cast<double>(
                platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotX)) * 64.0F;
            const std::uint32_t worldXBits = gWorldCurrentXBits;
            const float worldX = std::bit_cast<float>(worldXBits);
            if (targetX < worldX || targetX > worldX) {
                continue;
            }
            const double targetY = static_cast<double>(
                platform::legacy_crypt_int::Read_401EC0(&gPlayerSnapshotY)) * 64.0F;
            const std::uint32_t worldYBits = gWorldCurrentYBits;
            const float worldY = std::bit_cast<float>(worldYBits);
            if (targetY < worldY || targetY > worldY
                || std::bit_cast<std::int16_t>(action->field270) <= 0) {
                continue;
            }
        } else {
            const double targetX = static_cast<double>(
                platform::legacy_crypt_int::Read_401EC0(&action->mapX200)) * 64.0F;
            const float worldX = action->worldX;
            if (targetX < worldX || targetX > worldX) {
                continue;
            }
            const double targetY = static_cast<double>(
                platform::legacy_crypt_int::Read_401EC0(&action->mapY210)) * 64.0F;
            const float worldY = action->worldY;
            if (targetY < worldY || targetY > worldY
                || std::bit_cast<std::int16_t>(action->field270) <= 0) {
                continue;
            }
            if (position == 0) {
                gValue9DAD78 = 0x3F800000u;
                const std::int16_t count =
                    std::bit_cast<std::int16_t>(action->field270);
                if (count > 5) {
                    gValue9DAD78 = 0x40000000u;
                } else if (count >= 4) {
                    gValue9DAD78 = 0x3FCCCCCDu;
                } else if (count >= 2) {
                    gValue9DAD78 = 0x3F99999Au;
                }
            }
        }

        login::YobiAction* const first = ReadPetMapAction(ReadPetFollowIndex(0));
        const std::int32_t nextY = action->pathY248[0];
        const std::int32_t speed = first->walkSpeed;
        const std::int32_t nextX = action->pathX220[0];
        if (isPlayer) {
            Sub_46A410(nextX, nextY, speed);
        } else {
            login::detail::Sub_4422F0(action, nextX, nextY, speed);
        }
        login::detail::Sub_442460(action);
        // 两路末项前视均在消费前丢弃，不扩展原五槽顺序表。
        if (position + 1 >= 5) {
            continue;
        }
        const std::int16_t nextIndex = ReadPetFollowIndex(position + 1);
        if (nextIndex < 0) {
            continue;
        }
        login::YobiAction* const next = ReadPetMapAction(nextIndex);
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&action->mapY1F0);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&action->mapX1E0);
        login::detail::Sub_441F60(next, x, y);
    }

    for (std::size_t position = 0; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        const std::uint32_t playerIndex = ReadPlayerObjectIndex_FDA880();
        if (ReadPetMapObjectIndex(index) == playerIndex) {
            Sub_46A700();
        } else {
            login::detail::Sub_4428F0(ReadPetMapAction(index));
        }
    }
    static_cast<void>(Sub_468200());
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    gWorldProjectionReferenceXBits = worldXBits;
    gWorldProjectionReferenceYBits = worldYBits;
    return worldXBits;
}

void Sub_46A5F0()
{
    if (gValueFF0D36 == 0 || (ReadPlayerFlags18A() & 0x100u) == 0) {
        return;
    }
    for (std::size_t position = 1; position < 5; ++position) {
        const std::int16_t index = ReadPetFollowIndex(position);
        if (index < 0) {
            break;
        }
        login::YobiAction* const action = ReadPetMapAction(index);
        const double targetX = static_cast<double>(
            platform::legacy_crypt_int::Read_401EC0(&action->mapX200)) * 64.0F;
        const float worldX = action->worldX;
        if (targetX < worldX || targetX > worldX) {
            continue;
        }
        const double targetY = static_cast<double>(
            platform::legacy_crypt_int::Read_401EC0(&action->mapY210)) * 64.0F;
        const float worldY = action->worldY;
        if (targetY < worldY || targetY > worldY
            || std::bit_cast<std::int16_t>(action->field270) <= 0) {
            continue;
        }

        const std::int32_t speed = ReadPlayerWalkSpeed();
        const std::int32_t nextY = action->pathY248[0];
        const std::int32_t nextX = action->pathX220[0];
        static_cast<void>(
            login::detail::Sub_442110(action, nextX, nextY, speed, 0.0F));
        login::detail::Sub_442460(action);
        // 原末位置的9FB942前视值无论正负都被丢弃；不虚构第六槽。
        if (position + 1 >= 5) {
            continue;
        }
        const std::int16_t nextIndex = ReadPetFollowIndex(position + 1);
        if (nextIndex < 0) {
            continue;
        }
        login::YobiAction* const nextAction = ReadPetMapAction(nextIndex);
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&action->mapY1F0);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&action->mapX1E0);
        login::detail::Sub_441F60(nextAction, x, y);
    }
}

login::YobiAction* Sub_51A640()
{
    login::YobiAction* const action = ReadPlayerAction();
    if (action != nullptr) {
        action->field272 = 1;
    }
    return action;
}

login::YobiAction* Sub_51A660()
{
    login::YobiAction* const action = ReadPlayerAction();
    if (action != nullptr) {
        action->field272 = 0;
    }
    return action;
}

std::int32_t Sub_51A680()
{
    login::YobiAction* const action = ReadPlayerAction();
    return action == nullptr ? 0 : std::bit_cast<std::int16_t>(action->field272);
}

void Sub_519EE0()
{
    if (ReadPlayerAction() == nullptr) {
        return;
    }
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&gPlayerX);
    const std::int32_t blocked = Sub_469810(x, y);
    login::YobiAction* action = ReadPlayerAction();
    if (blocked != 0) {
        platform::legacy_crypt_int::CopyPayload(&gPlayerX, &action->mapX1E0);
        platform::legacy_crypt_int::CopyPayload(&gPlayerY, &action->mapY1F0);
    }

    const std::uint32_t worldXBits = gWorldCurrentXBits;
    action->worldX = std::bit_cast<float>(worldXBits);
    action = ReadPlayerAction();
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    action->worldY = std::bit_cast<float>(worldYBits);
    action = ReadPlayerAction();
    platform::legacy_crypt_int::CopyPayload(&action->mapX1E0, &gPlayerX);
    action = ReadPlayerAction();
    platform::legacy_crypt_int::CopyPayload(&action->mapY1F0, &gPlayerY);
    action = ReadPlayerAction();
    action->field27C = gValue9EE8FC;
    const std::uint32_t movementY = gValue9EE900;
    action = ReadPlayerAction();
    action->field280 = movementY;
    action = ReadPlayerAction();
    platform::legacy_crypt_int::CopyPayload(&action->mapX200, &gPlayerSnapshotX);
    action = ReadPlayerAction();
    platform::legacy_crypt_int::CopyPayload(&action->mapY210, &gPlayerSnapshotY);
}

void RenderChara_521F20()
{
    using login::detail::gYobiActionStart;
    using login::detail::gYobiActionEnd;

    for (login::YobiAction* action = gYobiActionStart->next;
         action != gYobiActionEnd;
         action = action->next) {
        const std::uint32_t flags = action->flags;
        if (((flags & 0x8000u) != 0 && render::gFrameParity628FC8 == 0)
            || ((flags & 0x10000u) != 0 && render::gFrameParity628FC8 == 1)) {
            action->cacheIndex = -2;
            continue;
        }

        // 原 521F90 测内嵌地址，不是首字符；空名也交给既有文本入口。
        static_cast<void>(render::DrawText2_456670(
            static_cast<std::int16_t>(action->nameX),
            static_cast<std::int16_t>(action->nameY),
            action->nameLayer,
            action->nameFontType,
            action->nameColor,
            action->displayName.data(),
            static_cast<std::int8_t>(action->nameValueC0)));
        const std::int16_t count =
            std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
        if (count >= MAX_QUEUE_SIZE) {
            return;
        }
        if (action->graphicIndex == -1
            || action->deathFlag == 1
            || (action->flags & 2u) != 0) {
            continue;
        }

        const auto queueIndex = static_cast<std::size_t>(count);
        render::GraphicCache3& order = render::gLegacyPendingStorage.cache3[queueIndex];
        render::GraphicCacheItem32& record =
            render::gLegacyPendingStorage.records[queueIndex];
        order.layer = action->displayPriority;
        order.cacheIndex = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);

        // 原 CRT 设置 x87 为 53 位；__ftol 先转 S64，caller 只取 EAX。见 DX x64 SPEC §16。
        const double scaledX =
            static_cast<double>(action->graphicOffsetX) * static_cast<double>(action->scaleX);
        record.x = static_cast<std::int32_t>(static_cast<std::int64_t>(scaledX)) + action->x;
        const double scaledY =
            static_cast<double>(action->graphicOffsetY) * static_cast<double>(action->scaleY);
        record.y = static_cast<std::int32_t>(static_cast<std::int64_t>(scaledY)) + action->y;
        record.imageNo = action->graphicIndex;
        record.descriptor = action->drawMode.bytes;
        if (action->batchType != 0) {
            if ((action->batchType & 1u) != 0) {
                record.descriptor[10] = static_cast<std::uint8_t>(
                    std::bit_cast<std::uint32_t>(action->color2) >> 16);
            }
            if ((action->batchType & 2u) != 0) {
                record.descriptor[9] = static_cast<std::uint8_t>(
                    std::bit_cast<std::uint32_t>(action->color2) >> 8);
            }
            if ((action->batchType & 4u) != 0) {
                record.descriptor[8] = static_cast<std::uint8_t>(action->color2);
            }
            if ((action->batchType & 8u) != 0) {
                record.descriptor[11] = static_cast<std::uint8_t>(
                    std::bit_cast<std::uint32_t>(action->color2) >> 24);
            }
        }
        if (action->rebirthValue1 != 0) {
            record.descriptor[13] &= static_cast<std::uint8_t>(~action->rebirthValue1);
            record.descriptor[13] |= action->rebirthValue1 & action->rebirthValue2;
        }
        if ((action->flags & 0x100000u) != 0) {
            record.descriptor[11] = 0x80;
        }

        record.reference.action = action;
        const std::uint32_t publishedFlags = action->flags;
        if ((publishedFlags & 8u) != 0) {
            record.value4 = 2;
        } else if ((publishedFlags & 4u) != 0) {
            record.value4 = 1;
        } else {
            record.value4 = 0;
        }
        action->cacheIndex =
            std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
        record.batchType = 0;
        if (record.descriptor[12] == 1) {
            record.batchType = 1;
        }
        if ((record.reference.action->flags & 0x20u) != 0) {
            record.batchType = 0xFF;
        }
        if (record.imageNo >= 3096 && record.imageNo <= 3105) {
            render::gSpecialGraphicPending = 1;
        }
        ++platform::gPendingGraphicCount;
    }
}

} // namespace cg::world::detail
