#include "../WindowComponents.hpp"
#include "cg/resource/GraphicInfo.hpp"

namespace cg::ui {
using namespace component_detail;

bool UIClipRectValid(const UIClipRect& rect) {
    return rect.right > rect.left && rect.bottom > rect.top;
}

bool UIIntersectClipRect(UIClipRect& out, const UIClipRect* a, const UIClipRect& b) {
    if (a) {
        out.left = std::max(a->left, b.left);
        out.top = std::max(a->top, b.top);
        out.right = std::min(a->right, b.right);
        out.bottom = std::min(a->bottom, b.bottom);
    } else {
        out = b;
    }
    return UIClipRectValid(out);
}

bool UIPointInClipRect(int x, int y, const UIClipRect* clipRect) {
    return !clipRect || (x >= clipRect->left && x < clipRect->right && y >= clipRect->top && y < clipRect->bottom);
}

bool UIRectIntersectsClipRect(int x, int y, int w, int h, const UIClipRect* clipRect) {
    if (!clipRect) {
        return w > 0 && h > 0;
    }
    return x < clipRect->right && x + w > clipRect->left && y < clipRect->bottom && y + h > clipRect->top;
}

static bool UIClipDestRect(int& x, int& y, int& w, int& h, const UIClipRect* clipRect, int* dx = nullptr, int* dy = nullptr) {
    if (w <= 0 || h <= 0) {
        return false;
    }
    if (!clipRect) {
        if (dx) {
            *dx = 0;
        }
        if (dy) {
            *dy = 0;
        }
        return true;
    }

    int left = std::max(x, static_cast<int>(clipRect->left));
    int top = std::max(y, static_cast<int>(clipRect->top));
    int right = std::min(x + w, static_cast<int>(clipRect->right));
    int bottom = std::min(y + h, static_cast<int>(clipRect->bottom));
    if (right <= left || bottom <= top) {
        return false;
    }
    if (dx) {
        *dx = left - x;
    }
    if (dy) {
        *dy = top - y;
    }
    x = left;
    y = top;
    w = right - left;
    h = bottom - top;
    return true;
}

int RendererDrawRectClipped(int x, int y, int w, int h, uint8_t layer, COLORREF color, const UIClipRect* clipRect) {
    int drawX = x;
    int drawY = y;
    int drawW = w;
    int drawH = h;
    if (!UIClipDestRect(drawX, drawY, drawW, drawH, clipRect)) {
        return -1;
    }
    return RendererDrawRect(static_cast<int16_t>(drawX), static_cast<int16_t>(drawY), static_cast<int16_t>(drawW), static_cast<int16_t>(drawH), layer, color);
}

int RendererDrawGraphicClipped(int x, int y, uint8_t layer, int imageNo, int w, int h, COLORREF color, const UIClipRect* clipRect) {
    if (clipRect && (x < clipRect->left || y < clipRect->top || x + w > clipRect->right || y + h > clipRect->bottom)) {
        return -1;
    }

    KRECT krect{0, 0, 0, 0, color, 0, 4, 0};
    return render::DrawGraphic3_520970(x, y, layer, imageNo, 0, 1, BATCH_SPRITE, &krect);
}



// 与字体测宽共用UTF-16步进；裁剪仅保留完整字符，不拆开代理对。
void RendererDrawTextClipped(session::ClientSession& session, int x, int y, unsigned __int8 layer, char fontType, uint8_t colorType, std::wstring_view msg, char hoverMode, char hoverFrame, const UIClipRect* clipRect) {
    if (msg.empty()) {
        return;
    }
    if (!clipRect) {
        InfoRenderText(session, static_cast<int16_t>(x), static_cast<int16_t>(y), layer, fontType, colorType, msg, hoverMode, hoverFrame);
        return;
    }

    const auto& font = render::GetFontMetrics(fontType);
    int fontHeight = font.fontHeight > 0 ? font.fontHeight : font.sHeight;
    if (fontHeight <= 0) {
        fontHeight = 16;
    }
    if (y < clipRect->top || y + fontHeight > clipRect->bottom) {
        return;
    }

    std::wstring clippedText;
    int drawX = x;
    int currentX = x;
    for (std::size_t offset = 0; offset < msg.size();) {
        const std::size_t next = text::Next(msg, offset);
        const auto glyph = msg.substr(offset, next - offset);
        int charWidth = FontMeasure(glyph, fontType);
        if (charWidth <= 0) {
            // 保留原半角/全角度量回退，不把UTF-16码元数当作GBK字节数。
            charWidth = font.sWidth * (msg[offset] < 0x80 ? 1 : 2);
        }
        const int nextX = currentX + charWidth;
        if (currentX >= clipRect->left && nextX <= clipRect->right) {
            if (clippedText.empty()) drawX = currentX;
            clippedText.append(glyph);
        }
        currentX = nextX;
        offset = next;
    }

    if (!clippedText.empty()) {
        InfoRenderText(session, static_cast<int16_t>(drawX), static_cast<int16_t>(y), layer, fontType, colorType, clippedText, hoverMode, hoverFrame);
    }
}

int RendererDrawPngClipped(const wchar_t* path, RECT16* rect, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clip)
{
    render::PngRectangle source{}, target{};
    if (rect) source = {rect->left, rect->top, rect->right, rect->bottom};
    if (clip) target = {clip->left, clip->top, clip->right, clip->bottom};
    return render::DrawPngImage(path, rect ? &source : nullptr,
        x, y, w, h, color, static_cast<uint8_t>(layer), clip ? &target : nullptr);
}
PngTexture* RendererCreateSurfaceTexture(int w, int h) { return render::CreatePixelImage(w, h); }
bool RendererUpdateSurfaceTexture(PngTexture* texture, const uint32_t* pixels, int w, int h) { return render::UpdatePixelImage(texture, pixels, w, h); }
void RendererReleaseSurfaceTexture(PngTexture* texture) { render::ReleasePixelImage(texture); }
int RendererDrawSurfaceTextureClipped(PngTexture* texture, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clip)
{
    render::PngRectangle target{};
    if (clip) target = {clip->left, clip->top, clip->right, clip->bottom};
    return render::DrawPixelImage(texture, x, y, w, h, color, static_cast<uint8_t>(layer), clip ? &target : nullptr);
}
int RendererDrawCanvasClipped(int x, int y, int w, int h, uint8_t layer, const std::vector<CanvasShape>& shapes, const UIClipRect* clip)
{
    render::CanvasRectangle target{};
    if (clip) target = {clip->left, clip->top, clip->right, clip->bottom};
    return render::DrawCanvasClipped(x, y, w, h, layer, shapes, clip ? &target : nullptr);
}
// 对原生入队结果裁剪，不改变图层、调色板、翻转及原生锚点。
void WindowRuntime_ClipGraphics(std::uint16_t first, const UIClipRect& clip) {
    for (std::uint16_t i = first; i < platform::gPendingGraphicCount; ++i) {
        auto& item = render::gLegacyPendingStorage.records[i];
        if ((item.batchType != 0 && item.batchType != 1) || item.imageNo < 0) continue;
        std::int16_t imageWidth = 0, imageHeight = 0;
        resource::GetGraphicWH(static_cast<std::uint32_t>(item.imageNo), &imageWidth, &imageHeight);
        int sx = item.batchType == 0 ? 0 : item.descriptor.sourceX;
        int sy = item.batchType == 0 ? 0 : item.descriptor.sourceY;
        int width = item.batchType == 0 ? imageWidth : item.descriptor.sourceWidth;
        int height = item.batchType == 0 ? imageHeight : item.descriptor.sourceHeight;
        if (sx < 0) { width += sx; item.x -= sx; sx = 0; }
        if (sy < 0) { height += sy; item.y -= sy; sy = 0; }
        width = std::min(width, imageWidth - sx); height = std::min(height, imageHeight - sy);
        const bool flipX = (item.descriptor.mixFlags & render::GraphicMixFlag_FlipX) != 0;
        const bool flipY = (item.descriptor.mixFlags & render::GraphicMixFlag_FlipY) != 0;
        const int x = item.x + (flipX ? imageWidth - width : 0), y = item.y + (flipY ? imageHeight - height : 0);
        const int left = std::max(x, int(clip.left)), top = std::max(y, int(clip.top));
        const int right = std::min(x + width, int(clip.right)), bottom = std::min(y + height, int(clip.bottom));
        const int clippedWidth = std::max(0, right - left), clippedHeight = std::max(0, bottom - top);
        sx += flipX ? x + width - right : left - x; sy += flipY ? y + height - bottom : top - y;
        item.x = left - (flipX ? imageWidth - clippedWidth : 0); item.y = top - (flipY ? imageHeight - clippedHeight : 0);
        item.descriptor.sourceX = static_cast<std::int16_t>(sx); item.descriptor.sourceY = static_cast<std::int16_t>(sy);
        item.descriptor.sourceWidth = static_cast<std::int16_t>(clippedWidth); item.descriptor.sourceHeight = static_cast<std::int16_t>(clippedHeight);
        item.descriptor.clipMode = 1; item.batchType = 1;
    }
}
} // namespace cg::ui
