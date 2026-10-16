#include "cg/render/Font.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/render/TextPendingQueue.hpp"
#include "BackTextDrawing.hpp"
#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"

#include <objidl.h>
#include <gdiplus.h>
#include <SDL3/SDL_surface.h>

#include <bit>
#include <cstring>
#include <limits>
#include <map>
#include <memory>
#include <utility>
#include <system_error>

namespace cg::render {
namespace {

// 原0x005590E0：字体仍统一宋体；UTL01使用UTF-16字体名与CreateFontW。
constexpr wchar_t kFaceName5590E0[] = L"宋体";
std::array<const wchar_t*, 4> gFaceFamilies558F08 { // 原全局：0x00558F08。
    kFaceName5590E0, kFaceName5590E0, kFaceName5590E0, kFaceName5590E0,
};

// 原 gFontList 每项 +00 是 GDI 句柄；与 0x14 字节度量分开承载。
std::array<HFONT, kFontTypeCount> gFontHandles558F68 {}; // 原全局：0x00558F68。
// R06：参考128项色表按D3DCOLOR_XRGB的实际宏位序承接，不按颜色名称交换通道。
constexpr std::array<std::uint32_t, 128> kTextColors {{
    0xFFFFFFFFU, 0xFF00FFFFU, 0xFFFF00FFU, 0xFF0000FFU, 0xFFFFFF00U, 0xFF00FF00U, 0xFFFF0000U, 0xFFA0A0A4U,
    0xFFA6CAF0U, 0xFFC0DCC0U, 0xFF000000U, 0xFFCDCDCDU, 0xFF665425U, 0xFF800000U, 0xFF808000U, 0xFF800080U,
    0xFFC0C0C0U, 0xFF00005FU, 0xFF0000AFU, 0xFF005F5FU, 0xFF005FAFU, 0xFF005FFFU, 0xFF00875FU, 0xFF0087AFU,
    0xFF0087FFU, 0xFF00AF5FU, 0xFF00AFAFU, 0xFF00AFFFU, 0xFF00D75FU, 0xFF00D7AFU, 0xFF00D7FFU, 0xFF00FF5FU,
    0xFF00FFAFU, 0xFF5F005FU, 0xFF5F00AFU, 0xFF5F00FFU, 0xFF5F5F5FU, 0xFF5F5FAFU, 0xFF5F5FFFU, 0xFF5F875FU,
    0xFF5F87AFU, 0xFF5F87FFU, 0xFF5FAF5FU, 0xFF5FAFAFU, 0xFF5FAFFFU, 0xFF5FD75FU, 0xFF5FD7AFU, 0xFF5FD7FFU,
    0xFF5FFF5FU, 0xFF5FFFAFU, 0xFF5FFFFFU, 0xFF87005FU, 0xFF8700AFU, 0xFF8700FFU, 0xFF875F5FU, 0xFF875FAFU,
    0xFF875FFFU, 0xFF87875FU, 0xFF8787AFU, 0xFF8787FFU, 0xFF87AF5FU, 0xFF87AFAFU, 0xFF87AFFFU, 0xFF87D75FU,
    0xFF87D7AFU, 0xFF87D7FFU, 0xFF87FF5FU, 0xFF87FFAFU, 0xFF87FFFFU, 0xFFAF005FU, 0xFFAF00AFU, 0xFFAF00FFU,
    0xFFAF5F5FU, 0xFFAF5FAFU, 0xFFAF5FFFU, 0xFFAF875FU, 0xFFAF87AFU, 0xFFAF87FFU, 0xFFAFAF5FU, 0xFFAFAFAFU,
    0xFFAFAFFFU, 0xFFAFD75FU, 0xFFAFD7AFU, 0xFFAFD7FFU, 0xFFAFFF5FU, 0xFFAFFFAFU, 0xFFAFFFFFU, 0xFFD7005FU,
    0xFFD700AFU, 0xFFD700FFU, 0xFFD75F5FU, 0xFFD75FAFU, 0xFFD75FFFU, 0xFFD7875FU, 0xFFD787AFU, 0xFFD787FFU,
    0xFFD7AF5FU, 0xFFD7AFAFU, 0xFFD7AFFFU, 0xFFD7D75FU, 0xFFD7D7AFU, 0xFFD7D7FFU, 0xFFD7FF5FU, 0xFFD7FFAFU,
    0xFFD7FFFFU, 0xFFFF005FU, 0xFFFF00AFU, 0xFFFF5F5FU, 0xFFFF5FAFU, 0xFFFF5FFFU, 0xFFFF875FU, 0xFFFF87AFU,
    0xFFFF87FFU, 0xFFFFAF5FU, 0xFFFFAFAFU, 0xFFFFAFFFU, 0xFFFFD75FU, 0xFFFFD7AFU, 0xFFFFD7FFU, 0xFFFFFF5FU,
    0xFFFFFFAFU, 0xFF121212U, 0xFF262626U, 0xFF3A3A3AU, 0xFF626262U, 0xFF767676U, 0xFF8A8A8AU, 0xFF000000U,
}};
std::array<std::int32_t, 8> gDerived558F30 {}; // 原全局：0x00558F30。
std::array<std::int32_t, 8> gDerived5545AC {}; // 原全局：0x005545AC。
// 原 554618 初值 0x39；随后每项 55461C+i*4 先写钳制值再被商覆盖。
std::array<std::int32_t, 9> gDerived554618 {{0x39}}; // 原全局：0x00554618。

static_assert(sizeof(gDerived558F30) == 0x20);
static_assert(sizeof(gDerived5545AC) == 0x20);

HFONT CreateLegacyFont(
    const std::int32_t height,
    const std::int32_t width,
    const std::int32_t weight,
    const wchar_t* const face)
{
    return ::CreateFontW(
        height, width, 0, 0, weight, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, 0x11, face);
}

const TextPendingRecord* FindTextPendingRecord(const std::uint32_t token)
{
    return token == 0 ? nullptr : &gTextPendingRecords[token - 1];
}

} // namespace

std::array<FontMetrics, kFontTypeCount> gFontMetrics {{ // 原全局：0x00558F68。
    {9, 17, 17, 17, 0, 0, 0, 0},
    {8, 15, 15, 15, 0, 0, 0, 0},
    {11, 21, 21, 21, 0, 0, 0, 0},
    {9, 17, 17, 17, 0, 0, 0, 0},
    {8, 15, 15, 15, 0, 0, 0, 0},
    {11, 21, 21, 21, 0, 0, 0, 0},
    {11, 21, 21, 21, 1, 20, 0, 0},
    {25, 49, 49, 49, -1, 48, 0, 0},
    {5, 11, 10, 11, 0, 11, 0, 0},
    {6, 12, 12, 12, -1, 12, 0, 0},
    {8, 16, 16, 16, -1, 16, 0, 0},
    {10, 20, 20, 20, -1, 20, 0, 0},
    {6, 12, 12, 12, 0, 12, 0, 0},
    {7, 13, 13, 13, 0, 12, 0, 0},
}};

// R06：度量与栅格缓存共用单／双格尺寸，保持完整UTF-16字符。
static SIZE MeasureGlyphSize(const std::wstring_view character, const std::int32_t fontType)
{
    const FontMetrics& metrics = gFontMetrics[static_cast<std::size_t>(fontType)];
    std::int32_t height = metrics.fontHeight != 0 ? metrics.fontHeight : metrics.doubleHeight;
    if (height == 0) { height = metrics.sHeight; }
    const std::int32_t halfWidth = (metrics.doubleWidth + 1) / 2;
    SIZE size {character.front() < 0x80 ? halfWidth : halfWidth * 2, height};
    if (height != 0) { return size; }

    const HDC dc = ::CreateCompatibleDC(nullptr);
    if (dc == nullptr) {
        throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), "创建文字度量DC失败");
    }
    const HGDIOBJ previous = ::SelectObject(dc, gFontHandles558F68[static_cast<std::size_t>(fontType)]);
    const BOOL measured = ::GetTextExtentPoint32W(dc, character.data(), static_cast<int>(character.size()), &size);
    const DWORD error = measured ? ERROR_SUCCESS : ::GetLastError();
    static_cast<void>(::SelectObject(dc, previous));
    static_cast<void>(::DeleteDC(dc));
    if (!measured) {
        throw std::system_error(static_cast<int>(error), std::system_category(), "Unicode文字度量失败");
    }
    return size;
}

// R06：图集独立持有纹理；字符键包含字体类型及最多两个UTF-16码元。
class FontGlyphCache final {
public:
    struct Glyph {
        TextureSurface* surface; // 借用本缓存Atlas拥有的纹理。
        TextureRectangle source; // 图集中的实际字形矩形，不含两像素间隔。
    };

    FontGlyphCache()
    {
        Gdiplus::GdiplusStartupInput startup;
        if (Gdiplus::GdiplusStartup(&token_, &startup, nullptr) != Gdiplus::Ok) {
            token_ = 0;
        }
    }

    ~FontGlyphCache()
    {
        atlases_.clear();
        if (token_ != 0) { Gdiplus::GdiplusShutdown(token_); }
    }

    const Glyph* FindOrCreate(const std::wstring_view character, const std::uint8_t fontType)
    {
        const std::uint32_t units = static_cast<std::uint16_t>(character[0])
            | (character.size() == 2 ? static_cast<std::uint32_t>(character[1]) << 16U : 0U);
        const std::uint64_t key = (static_cast<std::uint64_t>(fontType) << 32U) | units;
        const auto found = glyphs_.find(key);
        if (found != glyphs_.end()) { return &found->second; }
        if (token_ == 0) { return nullptr; }

        const SIZE size = MeasureGlyphSize(character, fontType);
        Gdiplus::Bitmap bitmap(size.cx, size.cy, PixelFormat32bppARGB);
        if (bitmap.GetLastStatus() != Gdiplus::Ok) { return nullptr; }
        const HDC dc = ::CreateCompatibleDC(nullptr);
        if (dc == nullptr) { return nullptr; }
        bool rendered;
        {
            Gdiplus::Font font(dc, gFontHandles558F68[fontType]);
            Gdiplus::Graphics graphics(&bitmap);
            Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
            Gdiplus::StringFormat format;
            format.SetAlignment(Gdiplus::StringAlignmentCenter);
            format.SetLineAlignment(Gdiplus::StringAlignmentNear);
            format.SetTrimming(Gdiplus::StringTrimmingNone);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
            graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSingleBitPerPixelGridFit);
            graphics.SetCompositingQuality(Gdiplus::CompositingQualityAssumeLinear);
            rendered = font.GetLastStatus() == Gdiplus::Ok
                && graphics.Clear(Gdiplus::Color(0, 0, 0, 0)) == Gdiplus::Ok
                && graphics.DrawString(character.data(), static_cast<int>(character.size()), &font,
                    Gdiplus::PointF(static_cast<Gdiplus::REAL>(size.cx / 2), 0), &format, &brush) == Gdiplus::Ok;
        }
        static_cast<void>(::DeleteDC(dc));
        if (!rendered) { return nullptr; }

        Atlas* atlas = nullptr;
        for (const auto& candidate : atlases_) {
            if (candidate->rowHeight != size.cy + 2) { continue; }
            const std::int32_t y = candidate->x + size.cx + 2 <= kAtlasSize
                ? candidate->y : candidate->y + candidate->rowHeight;
            if (size.cx + 2 <= kAtlasSize && y + size.cy + 2 <= kAtlasSize) {
                atlas = candidate.get();
                break;
            }
        }
        if (atlas == nullptr) {
            auto pending = std::make_unique<Atlas>();
            TextureSurface* const surface = CreatePlainTextureSurface_44C8B0(kAtlasSize, kAtlasSize, 0, 0x4000);
            if (surface == nullptr) { return nullptr; }
            pending->surface.Bind(surface);
            ReleaseTextureSurface(surface);
            if (!SDL_FillSurfaceRect(GetTextureSdlSurface(pending->surface.Get()), nullptr, 0)) { return nullptr; }
            pending->rowHeight = size.cy + 2;
            atlas = pending.get();
            atlases_.push_back(std::move(pending));
        }

        const bool newRow = atlas->x + size.cx + 2 > kAtlasSize;
        const std::int32_t x = newRow ? 0 : atlas->x;
        const std::int32_t y = newRow ? atlas->y + atlas->rowHeight : atlas->y;
        if (x + size.cx + 2 > kAtlasSize || y + size.cy + 2 > kAtlasSize) { return nullptr; }
        Gdiplus::BitmapData pixels {};
        const Gdiplus::Rect source(0, 0, size.cx, size.cy);
        if (bitmap.LockBits(&source, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &pixels) != Gdiplus::Ok) {
            return nullptr;
        }
        SDL_Surface* const output = GetTextureSdlSurface(atlas->surface.Get());
        if (!SDL_LockSurface(output)) {
            static_cast<void>(bitmap.UnlockBits(&pixels));
            return nullptr;
        }
        for (std::int32_t row = 0; row < size.cy; ++row) {
            const auto* input = static_cast<const std::uint8_t*>(pixels.Scan0) + row * pixels.Stride;
            auto* destination = static_cast<std::uint8_t*>(output->pixels)
                + (y + row) * output->pitch + x * sizeof(std::uint32_t);
            std::memcpy(destination, input, static_cast<std::size_t>(size.cx) * sizeof(std::uint32_t));
        }
        SDL_UnlockSurface(output);
        MarkTextureSurfaceModified(atlas->surface.Get());
        if (bitmap.UnlockBits(&pixels) != Gdiplus::Ok) { return nullptr; }

        atlas->x = x + size.cx + 2;
        atlas->y = y;
        const Glyph glyph {atlas->surface.Get(), {x, y, x + size.cx, y + size.cy}};
        return &glyphs_.emplace(key, glyph).first->second;
    }

private:
    static constexpr std::int16_t kAtlasSize = 512;
    struct Atlas {
        TextureSurfaceBinding surface;
        std::int32_t x = 0;
        std::int32_t y = 0;
        std::int32_t rowHeight = 0;
    };
    ULONG_PTR token_ = 0;
    std::vector<std::unique_ptr<Atlas>> atlases_;
    std::map<std::uint64_t, Glyph> glyphs_;
};

static std::unique_ptr<FontGlyphCache> gFontGlyphCache;

void ResetFontGlyphCache() noexcept
{
    gFontGlyphCache.reset();
}

// 原 [456190,456528)：旧句柄只删除不清槽，索引 6 不创建新句柄。
std::int32_t InitializeFont_456190(const std::int32_t family)
{
    ResetFontGlyphCache();
    const std::array<const wchar_t*, 3> initialFaces {
        kFaceName5590E0, kFaceName5590E0, kFaceName5590E0,
    };
    for (HFONT handle : gFontHandles558F68) {
        if (handle != nullptr) {
            static_cast<void>(::DeleteObject(handle));
        }
    }

    const wchar_t* const initialFace = initialFaces[static_cast<std::size_t>(family)];
    constexpr std::array<std::int32_t, 3> kBoldHeights {16, 14, 20};
    for (std::size_t index = 0; index < kBoldHeights.size(); ++index) {
        gFontHandles558F68[index] =
            CreateLegacyFont(kBoldHeights[index], 0, FW_BOLD, initialFace);
    }
    for (std::size_t index = 0; index < kBoldHeights.size(); ++index) {
        gFontHandles558F68[index + 3] =
            CreateLegacyFont(kBoldHeights[index], 0, FW_BOLD, kFaceName5590E0);
    }

    for (std::size_t index = 7; index <= 12; ++index) {
        FontMetrics& metrics = gFontMetrics[index];
        std::int8_t familyIndex = metrics.familyIndex;
        if (familyIndex < 0) {
            familyIndex = std::bit_cast<std::int8_t>(
                static_cast<std::uint8_t>(family));
            metrics.familyIndex = familyIndex;
        }
        const std::int32_t width = index == 7 ? 0 : metrics.sWidth;
        const wchar_t* const face = gFaceFamilies558F08[
            static_cast<std::size_t>(familyIndex)];
        const std::int32_t height = metrics.fontHeight;
        const std::int32_t weight = index == 7 ? FW_BOLD : FW_NORMAL;
        gFontHandles558F68[index] =
            CreateLegacyFont(height, width, weight, face);
    }
    gFontHandles558F68[13] = CreateLegacyFont(12, 0, FW_BOLD, kFaceName5590E0);

    for (std::size_t index = 0; index < 8; ++index) {
        gDerived558F30[index] =
            static_cast<std::int32_t>(gFontMetrics[index + 6].fontHeight / 4) - 1;
    }

    std::int32_t result = 0;
    for (std::size_t index = 0; index < 8; ++index) {
        const FontMetrics& metrics = gFontMetrics[index + 6];
        const std::int32_t width = metrics.doubleWidth;
        const std::int32_t height = metrics.fontHeight;
        gDerived558F30[index] = height / 4 - 1;
        gDerived5545AC[index] = 20 - height;
        std::int32_t difference = 25 - width;
        gDerived554618[index + 1] = difference;
        if (std::bit_cast<std::uint32_t>(difference) < 1U) {
            gDerived554618[index + 1] = 1;
        }
        result = 1980 / width;
        gDerived554618[index + 1] = result;
    }
    return result;
}

// 原 [456530,456554)：末项无句柄返回零，否则透传其 DeleteObject 返回。
std::int32_t ReleaseFont_456530()
{
    ResetFontGlyphCache();
    std::int32_t result = 0;
    for (HFONT handle : gFontHandles558F68) {
        result = handle == nullptr ? 0 : ::DeleteObject(handle);
    }
    return result;
}

// 原函数：0x00456D70。R06：按链顺序与每条文字的纹理批次提交缓存字形。
void RenderTextToSurface_456D70(const std::uint32_t headToken)
{
    if (gFontGlyphCache == nullptr) { gFontGlyphCache = std::make_unique<FontGlyphCache>(); }
    struct GlyphDraw {
        const FontGlyphCache::Glyph* glyph;
        std::int32_t x;
    };
    for (const TextPendingRecord* record = FindTextPendingRecord(headToken);
         record != nullptr; record = FindTextPendingRecord(record->nextToken)) {
        std::int32_t x = record->x;
        std::map<TextureSurface*, std::vector<GlyphDraw>> batches;
        for (std::size_t offset = 0; offset < record->text.size();) {
            const std::size_t next = cg::text::Next(record->text, offset);
            const auto* glyph = gFontGlyphCache->FindOrCreate(
                std::wstring_view(record->text).substr(offset, next - offset), record->fontType);
            offset = next;
            if (glyph == nullptr) { continue; }
            batches[glyph->surface].push_back({glyph, x});
            x += glyph->source[2] - glyph->source[0];
        }
        const std::uint32_t color = kTextColors[record->color & 0x7FU];
        const bool shadow = (record->color & 0x80U) == 0 && record->color != 127;
        for (const auto& [surface, draws] : batches) {
            for (const GlyphDraw& draw : draws) {
                const TextureRectangle& source = draw.glyph->source;
                const std::int32_t width = source[2] - source[0];
                const std::int32_t height = source[3] - source[1];
                if (shadow) {
                    const TextureRectangle destination {draw.x + 1, record->y + 1,
                        draw.x + width + 1, record->y + height + 1};
                    static_cast<void>(RenderImageTexture(surface, source, destination, 0, 0x9F000000U));
                }
                const TextureRectangle destination {draw.x, record->y, draw.x + width, record->y + height};
                static_cast<void>(RenderImageTexture(surface, source, destination, 0, color));
            }
        }
    }
}

// 原函数：0x0044B2E0。R06：与字形绘制共用步进，UTF-16预算不截断代理对。
std::int32_t Font::measure(const std::wstring_view text, const std::int32_t fontType, const std::int32_t maxUnits)
{
    if (maxUnits <= 0 || text.empty()) { return 0; }
    const std::size_t length = cg::text::Prefix(text, static_cast<std::size_t>(maxUnits));
    std::int32_t width = 0;
    for (std::size_t offset = 0; offset < length;) {
        const std::size_t next = cg::text::Next(text, offset);
        width += MeasureGlyphSize(text.substr(offset, next - offset), fontType).cx;
        offset = next;
    }
    return width;
}

std::vector<std::wstring> Font::split(const std::wstring_view text, const std::int32_t fontType, const std::int32_t width)
{
    const std::wstring contents(text);
    std::vector<std::wstring> lines;
    std::size_t start = 0;
    do {
        std::size_t end = start;
        while (end < contents.size() && contents[end] != '\r' && contents[end] != '\n') {
            const std::size_t next = cg::text::Next(contents, end);
            if (end > start && width > 0 && measure(contents.c_str() + start, fontType, static_cast<std::int32_t>(next - start)) > width) { break; }
            end = next;
        }
        lines.emplace_back(contents.substr(start, end - start));
        start = end < contents.size() && (contents[end] == '\r' || contents[end] == '\n') ? end + 1 : end;
    } while (start < contents.size());
    if (!contents.empty() && (contents.back() == '\r' || contents.back() == '\n')) { lines.emplace_back(); }
    return lines;
}


// 原0x0044B260／0x0044B2A0：动态UTF-16正文直接度量。
std::int32_t MeasureDefaultText_44B260(const std::wstring_view text)
{
    return Font::measure(text, 0, (std::numeric_limits<std::int32_t>::max)());
}
std::int32_t TextMeasure(const std::wstring_view text, const std::int32_t fontType)
{
    return Font::measure(text, fontType, (std::numeric_limits<std::int32_t>::max)());
}

} // namespace cg::render
