#include "window.hpp"
#include "base.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "cg/LegacyAudio.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace cg::login {

namespace detail {

// 原475950/48AC60共用布局的24H载荷；函数指针与完成槽留在action各自家族字段。
struct WindowOpeningPayload {
    std::uint16_t id; // 原成员偏移：+0x00。 通用展开为窗口编号，存放41/38族为kind，回调按S16读取。
    std::uint16_t counter; // 原成员偏移：+0x02。
    std::int32_t width; // 原成员偏移：+0x04。
    std::int32_t height; // 原成员偏移：+0x08。
    std::int32_t centerX; // 原成员偏移：+0x0C。
    std::int32_t centerY; // 原成员偏移：+0x10。
    std::int32_t halfWidth; // 原成员偏移：+0x14。
    std::int32_t halfHeight; // 原成员偏移：+0x18。
    std::int32_t value1C; // 原成员偏移：+0x1C。
    std::uint16_t flag20; // 原成员偏移：+0x20。
    std::array<std::byte, 2> bytes22; // 原成员偏移：+0x22。
};

static_assert(offsetof(WindowOpeningPayload, counter) == 2);

static_assert(offsetof(WindowOpeningPayload, width) == 4);

static_assert(offsetof(WindowOpeningPayload, height) == 8);

static_assert(offsetof(WindowOpeningPayload, centerX) == 0x0C);

static_assert(offsetof(WindowOpeningPayload, centerY) == 0x10);

static_assert(offsetof(WindowOpeningPayload, halfWidth) == 0x14);

static_assert(offsetof(WindowOpeningPayload, halfHeight) == 0x18);

static_assert(offsetof(WindowOpeningPayload, value1C) == 0x1C);

static_assert(offsetof(WindowOpeningPayload, flag20) == 0x20);

static_assert(sizeof(WindowOpeningPayload) == 0x24);

struct YobiUIData {
    std::int16_t winId; // 原成员偏移：+0x00。
    std::int16_t frameCount; // 原成员偏移：+0x02。
    std::int32_t width; // 原成员偏移：+0x04。
    std::int32_t height; // 原成员偏移：+0x08。
    std::int32_t centerX; // 原成员偏移：+0x0C。
    std::int32_t centerY; // 原成员偏移：+0x10。
    std::int32_t currentHalfWidth; // 原成员偏移：+0x14。
    std::int32_t currentHalfHeight; // 原成员偏移：+0x18。
    std::int32_t baseImageNo; // 原成员偏移：+0x1C。
    std::int16_t tileStyle; // 原成员偏移：+0x20。
    std::uint8_t trailingZero0; // 原成员偏移：+0x22。
    std::uint8_t trailingZero1; // 原成员偏移：+0x23。
};

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

namespace {

constexpr std::uint8_t kModalPriority = 3;

constexpr std::uint8_t kModalDisplayPriority = 0xD4;

constexpr std::uint8_t kModalLayer = 0xD4;

} // namespace

render::GraphicDescriptor16 ToGraphicDescriptor(
    const NineGridDescriptorFields& fields)
{
    return render::GraphicDescriptor16 {
        fields.sourceX, fields.sourceY, fields.width, fields.height,
        static_cast<std::uint32_t>(fields.color), fields.clip, fields.mixFlags,
        std::bit_cast<std::int16_t>(fields.paletteCacheIndex),
    };
}

namespace {

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
        render::GraphicMixFlag_OriginalPalette,
        std::uint16_t{0},
    };
}

// 原函数：0x0049BE00。
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
    const std::int32_t tileWidth = static_cast<std::int8_t>(tileWidth16);
    const std::int32_t tileHeight = static_cast<std::int8_t>(tileHeight16);
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
    (action->readiness = 1);
}

// 原函数：0x005046D0。
void UpdateModalAction_5046D0(YobiAction* const action)
{
    auto* const payload = action->payload.windowUi;

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

} // namespace

// 原函数：0x0049BE00。
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

// 原函数：0x004758D0。
void Sub_4758D0(YobiAction* const action)
{
    auto* const payload = action->payload.windowOpening;
    const std::int32_t halfHeight = payload->halfHeight;
    const std::int32_t centerY = payload->centerY;
    const std::int32_t halfWidth = payload->halfWidth;
    const std::int32_t centerX = payload->centerX;
    DrawGraphicHover(
        static_cast<std::uint16_t>(centerX - halfWidth),
        static_cast<std::uint16_t>(centerY - halfHeight),
        static_cast<std::uint16_t>(centerX + halfWidth),
        static_cast<std::uint16_t>(centerY + halfHeight), 0xD4, 0, 0);
    const std::int32_t deltaX = action->deltaX;
    const std::int32_t previousWidth = payload->halfWidth;
    const std::int32_t previousHeight = payload->halfHeight;
    payload->halfWidth = previousWidth + deltaX;
    const std::int32_t deltaY = action->deltaY;
    const bool complete = std::bit_cast<std::int16_t>(payload->counter) >= 10;
    payload->halfHeight = previousHeight + deltaY;
    if (complete) {
        static_cast<void>(ui::WindowRuntime_Open(std::bit_cast<std::int16_t>(payload->id)));
        static_cast<void>(MarkDeath(action));
        YobiAction** const completionSlot = action->windowCompletionSlot1D0;
        if (completionSlot != nullptr) {
            *completionSlot = nullptr;
        }
    }
    ++payload->counter;
}

// 原函数：0x00475950。
YobiAction* Sub_475950(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::int32_t id, const std::uint32_t flags, const std::uint32_t mode,
    YobiAction** const completionSlot)
{
    static_cast<void>(audio::PlaySe_52C450(56, 320));
    YobiAction* const action = NewYobiAction_404AE0(3, sizeof(WindowOpeningPayload), YobiPayloadKind::WindowOpening);
    if (action == nullptr) {
        if (completionSlot != nullptr) {
            *completionSlot = nullptr;
        }
        static_cast<void>(ui::WindowRuntime_Open(id));
        return nullptr;
    }
    auto* const payload = action->payload.windowOpening;
    action->callback = &Sub_4758D0;
    action->displayPriority = 0xDF;
    action->flags |= 6u;
    payload->value1C = 0;
    payload->flag20 = 0;
    if ((static_cast<std::uint8_t>(flags) & 1u) != 0) {
        payload->flag20 = 1;
    }
    const std::int32_t lowX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const std::int32_t lowY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t lowHeight =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(height));
    const std::int32_t lowWidth =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(width));
    action->x = lowX;
    action->y = lowY;
    payload->height = lowHeight;
    payload->id = static_cast<std::uint16_t>(id);
    payload->counter = 0;
    payload->width = lowWidth;
    action->windowCompletionSlot1D0 = completionSlot;
    const auto lowMode = static_cast<std::uint8_t>(mode);
    if (lowMode == 0) {
        payload->centerX = lowWidth / 2 + lowX;
        payload->centerY = lowHeight / 2 + lowY;
        action->deltaX = 0;
        action->deltaY = (payload->centerY - lowY) / 10;
        payload->halfWidth = payload->centerX - lowX;
    } else if (lowMode == 1) {
        payload->centerX = lowWidth / 2 + lowX;
        payload->centerY = lowHeight / 2 + lowY;
        action->deltaX = (payload->centerX - lowX) / 10;
        action->deltaY = (payload->centerY - lowY) / 10;
    }
    return action;
}

// QIP01：原48ABC0完整59条；描边后更新半宽/半高，S16计数>=10发布/标死，再无条件W16自增。
void Sub_48ABC0(YobiAction* const action)
{
    auto* const payload = action->payload.windowOpening;
    const std::int32_t halfHeight = payload->halfHeight;
    const std::int32_t centerY = payload->centerY;
    const std::int32_t halfWidth = payload->halfWidth;
    const std::int32_t centerX = payload->centerX;
    DrawGraphicHover(
        static_cast<std::uint16_t>(centerX - halfWidth),
        static_cast<std::uint16_t>(centerY - halfHeight),
        static_cast<std::uint16_t>(centerX + halfWidth),
        static_cast<std::uint16_t>(centerY + halfHeight), 0xD4, 0, 0);
    const std::int32_t deltaX = action->deltaX;
    const std::int32_t previousWidth = payload->halfWidth;
    const std::int32_t previousHeight = payload->halfHeight;
    payload->halfWidth = previousWidth + deltaX;
    const std::int32_t deltaY = action->deltaY;
    const bool complete = std::bit_cast<std::int16_t>(payload->counter) >= 10;
    payload->halfHeight = previousHeight + deltaY;
    if (complete) {
        const ui::WindowValueCallback callback = action->numberWindowCallback2C;
        const std::int32_t initialValue =
            action->mapY1F0.value();
        const std::int32_t value14 =
            action->mapX1E0.value();
        const std::int32_t imageNo = action->graphicIndex;
        const std::int32_t sourceElement = action->nameY;
        const std::int32_t maximum = action->field1D0;
        const std::int32_t sourceWindow = action->nameX;
        static_cast<void>(ui::detail::WindowQuantity_Open(
            sourceWindow, sourceElement, imageNo, maximum, value14, initialValue, callback));
        static_cast<void>(MarkDeath(action));
    }
    ++payload->counter;
}

// QIP01：原48AC60完整186条；actionFlags先发布，旧窗关闭与失败直发布保，低BYTE flags/两CryptInt原序。
YobiAction* Sub_48AC60(
    const std::int32_t sourceWindow, const std::int32_t sourceElement,
    const std::int32_t imageNo, const std::int32_t maximum, const std::int32_t value14,
    const std::int32_t initialValue, const ui::WindowValueCallback callback,
    const std::uint32_t flags, const std::uint32_t mode, const std::uint32_t actionFlags)
{
    ui::detail::gWindowStates[cg::ui::WindowId_Quantity].actionFlags1C = actionFlags;
    static_cast<void>(audio::PlaySe_52C450(56, 320));
    YobiAction* const action = NewYobiAction_404AE0(3, sizeof(WindowOpeningPayload), YobiPayloadKind::WindowOpening);
    if (ui::detail::gWindowStates[cg::ui::WindowId_Quantity].window != nullptr) {
        ui::detail::gWindowStates[cg::ui::WindowId_Quantity].window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (action == nullptr) {
        static_cast<void>(ui::detail::WindowQuantity_Open(
            sourceWindow, sourceElement, imageNo, maximum, value14, initialValue, callback));
        return nullptr;
    }
    const ui::detail::WindowPrototype* const prototype = ui::detail::gWindowPrototypes57C130[cg::ui::WindowId_Quantity];
    const auto windowFlags = reinterpret_cast<const unsigned char*>(
        &ui::detail::gWindowStates[cg::ui::WindowId_Quantity].flags)[0];
    std::int32_t x = prototype->defaultX;
    std::int32_t y = prototype->defaultY;
    const std::int32_t width = prototype->width;
    const std::int32_t height = prototype->height;
    if ((windowFlags & 1u) != 0) {
        x = ui::detail::gWindowStates[cg::ui::WindowId_Quantity].x;
        y = ui::detail::gWindowStates[cg::ui::WindowId_Quantity].y;
    }
    auto* const payload = action->payload.windowOpening;
    action->callback = Sub_48ABC0;
    action->flags |= 6u;
    action->graphicIndex = imageNo;
    action->displayPriority = 0xDF;
    payload->flag20 = 0;
    if ((static_cast<std::uint8_t>(flags) & 1u) != 0) {
        payload->flag20 = 1;
    }
    action->x = x;
    action->y = y;
    payload->width = width;
    payload->height = height;
    action->numberWindowCallback2C = callback;
    action->field1D0 = maximum;
    action->mapX1E0 = static_cast<std::int32_t>(value14);
    action->mapY1F0 = static_cast<std::int32_t>(initialValue);
    action->nameX = sourceWindow;
    action->nameY = sourceElement;
    payload->counter = 0;
    const auto lowMode = static_cast<std::uint8_t>(mode);
    if (lowMode == 0) {
        payload->centerX = width / 2 + x;
        payload->centerY = height / 2 + y;
        action->deltaX = 0;
        action->deltaY = (payload->centerY - y) / 10;
        payload->halfWidth = payload->centerX - x;
    } else if (lowMode == 1) {
        payload->centerX = width / 2 + x;
        payload->centerY = height / 2 + y;
        action->deltaX = (payload->centerX - x) / 10;
        action->deltaY = (payload->centerY - y) / 10;
    }
    return action;
}

// SOA01：原4ABE60完整51条；半尺寸原取样、S16计数完成时Y-X读值开41/标死，末W16递增保持。
void Sub_4ABE60(YobiAction* const action)
{
    auto* const payload = action->payload.windowOpening;
    const std::int32_t halfHeight = payload->halfHeight;
    const std::int32_t centerY = payload->centerY;
    const std::int32_t halfWidth = payload->halfWidth;
    const std::int32_t centerX = payload->centerX;
    DrawGraphicHover(
        static_cast<std::uint16_t>(centerX - halfWidth),
        static_cast<std::uint16_t>(centerY - halfHeight),
        static_cast<std::uint16_t>(centerX + halfWidth),
        static_cast<std::uint16_t>(centerY + halfHeight), 0xD4, 0, 0);
    const std::int32_t deltaX = action->deltaX;
    const std::int32_t previousWidth = payload->halfWidth;
    const std::int32_t previousHeight = payload->halfHeight;
    payload->halfWidth = previousWidth + deltaX;
    const std::int32_t deltaY = action->deltaY;
    const bool complete = std::bit_cast<std::int16_t>(payload->counter) >= 10;
    payload->halfHeight = previousHeight + deltaY;
    if (complete) {
        const std::int32_t userState =
            action->mapY1F0.value();
        const std::int32_t value14 =
            action->mapX1E0.value();
        const std::int32_t kind = std::bit_cast<std::int16_t>(payload->id);
        static_cast<void>(ui::detail::WindowBank_Open(kind, value14, userState));
        static_cast<void>(MarkDeath(action));
    }
    ++payload->counter;
}

// SOA01：原4ABEF0完整154条；保存位只读BYTE，两CryptInt先取后发布/原mode1持中心X，失败同步开41保持。
YobiAction* Sub_4ABEF0(
    const std::int32_t kind, const std::int32_t value14, const std::int32_t userState,
    const std::uint32_t flags, const std::uint32_t mode)
{
    static_cast<void>(audio::PlaySe_52C450(56, 320));
    YobiAction* const action = NewYobiAction_404AE0(3, sizeof(WindowOpeningPayload), YobiPayloadKind::WindowOpening);
    if (action == nullptr) {
        static_cast<void>(ui::detail::WindowBank_Open(kind, value14, userState));
        return nullptr;
    }
    const ui::detail::WindowPrototype* const prototype = ui::detail::gWindowPrototypes57C130[cg::ui::WindowId_Bank];
    const bool savedPosition = (*reinterpret_cast<const unsigned char*>(
        &ui::detail::gWindowStates[cg::ui::WindowId_Bank].flags) & 1u) != 0;
    std::int32_t y = prototype->defaultY;
    const std::int32_t width = prototype->width;
    std::int32_t x = prototype->defaultX;
    const std::int32_t height = prototype->height;
    if (savedPosition) {
        x = ui::detail::gWindowStates[cg::ui::WindowId_Bank].x;
        y = ui::detail::gWindowStates[cg::ui::WindowId_Bank].y;
    }
    auto* const payload = action->payload.windowOpening;
    action->flags |= 6u;
    action->callback = Sub_4ABE60;
    action->displayPriority = 0xDF;
    payload->value1C = 0;
    payload->flag20 = 0;
    if ((static_cast<std::uint8_t>(flags) & 1u) != 0) {
        payload->flag20 = 1;
    }
    action->x = x;
    action->y = y;
    payload->width = width;
    payload->height = height;
    payload->id = static_cast<std::uint16_t>(kind);
    action->mapX1E0 = static_cast<std::int32_t>(value14);
    action->mapY1F0 = static_cast<std::int32_t>(userState);
    payload->counter = 0;
    const auto lowMode = static_cast<std::uint8_t>(mode);
    if (lowMode == 0) {
        payload->centerX = width / 2 + x;
        payload->centerY = height / 2 + y;
        action->deltaX = 0;
        action->deltaY = (payload->centerY - y) / 10;
        payload->halfWidth = payload->centerX - x;
    } else if (lowMode == 1) {
        const std::int32_t centerX = width / 2 + x;
        payload->centerX = centerX;
        payload->centerY = height / 2 + y;
        action->deltaX = (centerX - x) / 10;
        action->deltaY = (payload->centerY - y) / 10;
    }
    return action;
}

// SOA01：原4ACDF0完整53条；半尺寸原取样、phase先持值再Y-X读值开38/标死，末W16递增保持。
void Sub_4ACDF0(YobiAction* const action)
{
    auto* const payload = action->payload.windowOpening;
    const std::int32_t halfHeight = payload->halfHeight;
    const std::int32_t centerY = payload->centerY;
    const std::int32_t halfWidth = payload->halfWidth;
    const std::int32_t centerX = payload->centerX;
    DrawGraphicHover(
        static_cast<std::uint16_t>(centerX - halfWidth),
        static_cast<std::uint16_t>(centerY - halfHeight),
        static_cast<std::uint16_t>(centerX + halfWidth),
        static_cast<std::uint16_t>(centerY + halfHeight), 0xD4, 0, 0);
    const std::int32_t deltaX = action->deltaX;
    const std::int32_t previousWidth = payload->halfWidth;
    const std::int32_t previousHeight = payload->halfHeight;
    payload->halfWidth = previousWidth + deltaX;
    const std::int32_t deltaY = action->deltaY;
    const bool complete = std::bit_cast<std::int16_t>(payload->counter) >= 10;
    payload->halfHeight = previousHeight + deltaY;
    if (complete) {
        const std::int32_t phase = action->phase;
        const std::int32_t userState =
            action->mapY1F0.value();
        const std::int32_t value14 =
            action->mapX1E0.value();
        const std::int32_t kind = std::bit_cast<std::int16_t>(payload->id);
        static_cast<void>(ui::detail::WindowItemStorage_Open(kind, value14, userState, phase));
        static_cast<void>(MarkDeath(action));
    }
    ++payload->counter;
}

// SOA01：原4ACE80完整158条；保存位只读BYTE，两CryptInt先取后发布/原mode1持中心X，phase及失败同步开38保持。
YobiAction* Sub_4ACE80(
    const std::int32_t kind, const std::int32_t value14, const std::int32_t userState,
    const std::int32_t phase, const std::uint32_t flags, const std::uint32_t mode)
{
    static_cast<void>(audio::PlaySe_52C450(56, 320));
    YobiAction* const action = NewYobiAction_404AE0(3, sizeof(WindowOpeningPayload), YobiPayloadKind::WindowOpening);
    if (action == nullptr) {
        static_cast<void>(ui::detail::WindowItemStorage_Open(kind, value14, userState, phase));
        return nullptr;
    }
    const ui::detail::WindowPrototype* const prototype = ui::detail::gWindowPrototypes57C130[cg::ui::WindowId_ItemStorage];
    const bool savedPosition = (*reinterpret_cast<const unsigned char*>(
        &ui::detail::gWindowStates[cg::ui::WindowId_ItemStorage].flags) & 1u) != 0;
    std::int32_t y = prototype->defaultY;
    const std::int32_t width = prototype->width;
    std::int32_t x = prototype->defaultX;
    const std::int32_t height = prototype->height;
    if (savedPosition) {
        x = ui::detail::gWindowStates[cg::ui::WindowId_ItemStorage].x;
        y = ui::detail::gWindowStates[cg::ui::WindowId_ItemStorage].y;
    }
    auto* const payload = action->payload.windowOpening;
    action->flags |= 6u;
    action->callback = Sub_4ACDF0;
    action->displayPriority = 0xDF;
    payload->value1C = 0;
    payload->flag20 = 0;
    if ((static_cast<std::uint8_t>(flags) & 1u) != 0) {
        payload->flag20 = 1;
    }
    action->x = x;
    action->y = y;
    payload->width = width;
    payload->height = height;
    payload->id = static_cast<std::uint16_t>(kind);
    action->phase = phase;
    action->mapX1E0 = static_cast<std::int32_t>(value14);
    action->mapY1F0 = static_cast<std::int32_t>(userState);
    payload->counter = 0;
    const auto lowMode = static_cast<std::uint8_t>(mode);
    if (lowMode == 0) {
        payload->centerX = width / 2 + x;
        payload->centerY = height / 2 + y;
        action->deltaX = 0;
        action->deltaY = (payload->centerY - y) / 10;
        payload->halfWidth = payload->centerX - x;
    } else if (lowMode == 1) {
        const std::int32_t centerX = width / 2 + x;
        payload->centerX = centerX;
        payload->centerY = height / 2 + y;
        action->deltaX = (centerX - x) / 10;
        action->deltaY = (payload->centerY - y) / 10;
    }
    return action;
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

// 原函数：0x005044E0。
YobiAction* CreateModalAction_5044E0(
    const std::int32_t x,
    const std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    const std::int32_t winId,
    const std::int32_t param6)
{
    YobiAction* const action = NewYobiAction_404AE0(
        kModalPriority,
        static_cast<std::uint32_t>(sizeof(YobiUIData)), YobiPayloadKind::WindowUi);
    if (action == nullptr) {
        return nullptr;
    }

    auto* const payload = action->payload.windowUi;
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

} // namespace detail

} // namespace cg::login
