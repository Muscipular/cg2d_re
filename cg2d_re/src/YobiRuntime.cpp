#include "cg/YobiRuntime.hpp"

#include "LoginMessageDependencies.hpp"

#include "cg/LegacyAudio.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
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
    std::int32_t deltaX;
    std::int32_t deltaY;
    std::int32_t field58; // 原 +58，与 +2B8 的 direction 独立。
    float scaleX;
    float scaleY;
    platform::LegacyCryptInt readiness;
    std::int32_t creatorParam6;
    std::int32_t field1D0; // 原 +1D0；440B70 写子 action 的槽序号。
    std::int32_t walkSpeed;
    std::int32_t framePlayingRate;
    std::int32_t imageNo;
    std::int32_t characterAction;
    std::int32_t direction;
    render::GraphicDescriptor16 drawMode;
    std::uint8_t batchType;
    std::int32_t color2;
    std::uint8_t rebirthValue1;
    std::uint8_t rebirthValue2;
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

    if (payload->winId < 4 && action->creatorParam6 != 0) {
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

// 原 440C50 回调后继尚未落位，保持真实目标而非空回调。
void Sub_440C50(YobiAction* action);

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
        action->creatorParam6 = param6;
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
