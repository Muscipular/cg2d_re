#include "cg/render/Font.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/core/m_log.hpp"

#include "cg/render/TextPendingQueue.hpp"
#include "BackTextDrawing.hpp"
#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"

#include <objidl.h>
#include <gdiplus.h>
#include <SDL3/SDL_surface.h>
#include <yaml-cpp/yaml.h>

#include <array>
#include <bit>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <system_error>

namespace cg::render {

// 原0x005590E0：FC01保留宋体作为内置默认，配置可按ID覆盖。
static constexpr wchar_t kFaceName5590E0[] = L"宋体";
// R06：参考128项色表按D3DCOLOR_XRGB的实际宏位序承接，不按颜色名称交换通道。
static constexpr std::array<std::uint32_t, 128> kTextColors {{
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
std::uint32_t GetTextColor(const std::uint8_t color) noexcept { return kTextColors[color & 0x7FU]; }

static std::array<std::int32_t, 8> gDerived558F30 {}; // 原全局：0x00558F30。
static std::array<std::int32_t, 8> gDerived5545AC {}; // 原全局：0x005545AC。
// 原 554618 初值 0x39；随后每项 55461C+i*4 先写钳制值再被商覆盖。
static std::array<std::int32_t, 9> gDerived554618 {{0x39}}; // 原全局：0x00554618。

static_assert(sizeof(gDerived558F30) == 0x20);
static_assert(sizeof(gDerived5545AC) == 0x20);

static HFONT CreateLegacyFont(
    const std::int32_t height,
    const std::int32_t width,
    const std::int32_t weight,
    const wchar_t* const face,
    const bool italic = false)
{
    return ::CreateFontW(
        height, width, 0, 0, weight, italic ? TRUE : FALSE, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, 0x11, face);
}

static const TextPendingRecord* FindTextPendingRecord(const std::uint32_t token)
{
    return token == 0 ? nullptr : &gTextPendingRecords[token - 1];
}

// FC02：原字体表仍在font.yml；0号兜底恢复迁移前布局与字形度量。
static constexpr FontMetrics kFallbackFontMetrics {9, 17, 17, 17, 0, 0, 0, 0};

// FC01：会话先于所有GDI+资源创建，并在它们释放后关闭。
class FontSession final {
public:
    FontSession()
    {
        Gdiplus::GdiplusStartupInput startup;
        if (Gdiplus::GdiplusStartup(&token_, &startup, nullptr) != Gdiplus::Ok) {
            throw std::runtime_error("字体系统GDI+初始化失败");
        }
    }
    ~FontSession() { Gdiplus::GdiplusShutdown(token_); }
    FontSession(const FontSession&) = delete;
    FontSession& operator=(const FontSession&) = delete;
private:
    ULONG_PTR token_ = 0;
};

// FC01：每项同时拥有度量与绘制字体；Font必须先于私有集合及HFONT销毁。
struct FontResource final {
    FontMetrics metrics {};
    HFONT handle = nullptr;
    std::unique_ptr<Gdiplus::PrivateFontCollection> collection;
    std::unique_ptr<Gdiplus::Font> font;

    ~FontResource()
    {
        font.reset();
        if (handle != nullptr) { static_cast<void>(::DeleteObject(handle)); }
    }
};

// FC02：系统字体沿迁移前CreateFontW/HFONT路径；DC在成功和异常路径均释放。
static void CreateSystemFont(FontResource& resource, const std::wstring& family,
    const int width, const int height, const int weight, const bool italic)
{
    resource.handle = CreateLegacyFont(height, width, weight, family.c_str(), italic);
    if (resource.handle == nullptr) { throw std::runtime_error("CreateFontW创建字体失败"); }
    const std::unique_ptr<std::remove_pointer_t<HDC>, decltype(&::DeleteDC)> dc(
        ::CreateCompatibleDC(nullptr), &::DeleteDC);
    if (dc == nullptr) { throw std::runtime_error("创建字体DC失败"); }
    resource.font = std::make_unique<Gdiplus::Font>(dc.get(), resource.handle);
    if (resource.font->GetLastStatus() != Gdiplus::Ok) {
        throw std::runtime_error("从HFONT创建GDI+字体失败");
    }
}

static std::unique_ptr<FontSession> gFontSession;
static std::map<std::int32_t, std::unique_ptr<FontResource>> gFonts;

std::int32_t ResolveFontId(const std::int32_t id) noexcept
{
    return gFonts.find(id) == gFonts.end() ? 0 : id;
}

const FontMetrics& GetFontMetrics(const std::int32_t id) noexcept
{
    const auto found = gFonts.find(ResolveFontId(id));
    return found == gFonts.end() ? kFallbackFontMetrics : found->second->metrics;
}

// 配置边界严格按UTF-8解码；路径与族名不经过游戏协议的GBK转换。
static std::wstring DecodeFontUtf8(const std::string& value)
{
    if (value.empty() || value.find('\0') != std::string::npos
        || value.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
        throw std::runtime_error("字体名称或文件路径为空、含NUL或过长");
    }
    const int length = static_cast<int>(value.size());
    const int count = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), length, nullptr, 0);
    if (count == 0) { throw std::runtime_error("字体名称或路径不是有效UTF-8"); }
    std::wstring result(static_cast<std::size_t>(count), L'\0');
    if (::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), length, result.data(), count) != count) {
        throw std::runtime_error("字体名称或路径UTF-8转换失败");
    }
    return result;
}

static std::unique_ptr<FontResource> LoadConfiguredFont(
    const YAML::Node& entry, const std::filesystem::path& directory)
{
    const std::wstring familyName = DecodeFontUtf8(entry["family"].as<std::string>());
    const int width = entry["width"].as<int>();
    const int height = entry["height"].as<int>();
    const std::string styleName = entry["style"].as<std::string>();
    const int weight = entry["weight"].as<int>();
    if (width < 1 || width > 255 || height < 1 || height > 255) {
        throw std::runtime_error("width和height必须在1到255之间");
    }
    const int fullWidth = entry["full_width"] ? entry["full_width"].as<int>() : width * 2;
    const int glyphHeight = entry["glyph_height"] ? entry["glyph_height"].as<int>() : height;
    const int rasterWidth = entry["raster_width"] ? entry["raster_width"].as<int>() : 0;
    const int rasterHeight = entry["raster_height"] ? entry["raster_height"].as<int>() : height;
    if ((fullWidth != width * 2 && fullWidth != width * 2 - 1)
        || glyphHeight < 0 || glyphHeight > 255 || rasterWidth < 0 || rasterWidth > 255
        || rasterHeight < 1 || rasterHeight > 255) {
        throw std::runtime_error("字体布局或栅格尺寸配置无效");
    }
    if (styleName != "normal" && styleName != "italic") {
        throw std::runtime_error("style只支持normal或italic");
    }
    if (weight != 400 && weight != 700) { throw std::runtime_error("weight只支持400或700"); }
    const int style = (weight == 700 ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular)
        | (styleName == "italic" ? Gdiplus::FontStyleItalic : Gdiplus::FontStyleRegular);
    auto resource = std::make_unique<FontResource>();
    if (entry["file"]) {
        if (rasterWidth != 0) { throw std::runtime_error("私有字体文件不支持非零raster_width"); }
        const auto file = directory / std::filesystem::path(DecodeFontUtf8(entry["file"].as<std::string>()));
        resource->collection = std::make_unique<Gdiplus::PrivateFontCollection>();
        if (resource->collection->AddFontFile(file.c_str()) != Gdiplus::Ok) {
            throw std::runtime_error("字体文件无法加载或格式不受GDI+支持");
        }
    }
    const Gdiplus::FontFamily family(familyName.c_str(), resource->collection.get());
    if (family.GetLastStatus() != Gdiplus::Ok) { throw std::runtime_error("找不到指定字体族"); }
    if (!family.IsStyleAvailable(style)) { throw std::runtime_error("字体族不支持指定风格或粗细"); }
    if (resource->collection == nullptr) {
        CreateSystemFont(*resource, familyName, rasterWidth, rasterHeight, weight, styleName == "italic");
    } else {
        const auto emHeight = family.GetEmHeight(style);
        const auto lineSpacing = family.GetLineSpacing(style);
        if (emHeight == 0 || lineSpacing == 0) { throw std::runtime_error("字体行距无效"); }
        const Gdiplus::REAL size = static_cast<Gdiplus::REAL>(rasterHeight) * emHeight / lineSpacing;
        resource->font = std::make_unique<Gdiplus::Font>(&family, size, style, Gdiplus::UnitPixel);
    }
    if (resource->font->GetLastStatus() != Gdiplus::Ok) { throw std::runtime_error("创建配置字体失败"); }
    resource->metrics = {width, height, fullWidth, height, 0, static_cast<std::uint8_t>(glyphHeight), 0, 0};
    return resource;
}

static void LoadFontConfiguration()
{
    try {
        const auto path = std::filesystem::absolute(L"font.yml");
        if (!std::filesystem::exists(path)) { return; }
        std::ifstream stream(path, std::ios::binary);
        if (!stream) { throw std::runtime_error("无法打开font.yml"); }
        const YAML::Node root = YAML::Load(stream);
        if (!root.IsMap() || !root["fonts"].IsSequence()) {
            throw std::runtime_error("font.yml根节点必须包含fonts列表");
        }
        const YAML::Node entries = root["fonts"];
        std::set<int> ids;
        // 先完整核对ID，防止重复/越界配置只发布了前半部分。
        for (const auto& entry : entries) {
            const int id = entry["id"].as<int>();
            if (id < 0 || id > kMaxFontId || !ids.insert(id).second) {
                throw std::runtime_error("字体id必须在0到32之间且不能重复");
            }
        }
        for (const auto& entry : entries) {
            const int id = entry["id"].as<int>();
            try {
                auto font = LoadConfiguredFont(entry, path.parent_path());
                gFonts.insert_or_assign(id, std::move(font));
            } catch (const std::exception& error) {
                if (id != 0) { gFonts.erase(id); }
                static_cast<void>(error);
                m_log_iw(L"font.yml 字体%d加载失败，使用0号字体：%hs\n", id, error.what());
            }
        }
    } catch (const std::exception& error) {
        static_cast<void>(error);
        m_log_iw(L"font.yml读取失败，使用0号默认字体：%hs\n", error.what());
    }
}

// R06：度量与栅格缓存共用单／双格尺寸，保持完整UTF-16字符。
static SIZE MeasureGlyphSize(const std::wstring_view character, const std::int32_t fontType)
{
    const FontMetrics& metrics = GetFontMetrics(fontType);
    std::int32_t height = metrics.fontHeight != 0 ? metrics.fontHeight : metrics.doubleHeight;
    if (height == 0) { height = metrics.sHeight; }
    const std::int32_t halfWidth = (metrics.doubleWidth + 1) / 2;
    return {character.front() < 0x80 ? halfWidth : halfWidth * 2, height};
}

// R06：图集独立持有纹理；字符键包含字体类型及最多两个UTF-16码元。
class FontGlyphCache final {
public:
    struct Glyph {
        TextureSurface* surface; // 借用本缓存Atlas拥有的纹理。
        TextureRectangle source; // 图集中的实际字形矩形，不含两像素间隔。
    };

    const Glyph* FindOrCreate(const std::wstring_view character, const std::uint8_t fontType)
    {
        const std::int32_t fontId = ResolveFontId(fontType);
        const auto registered = gFonts.find(fontId);
        if (registered == gFonts.end()) { return nullptr; }
        const std::uint32_t units = static_cast<std::uint16_t>(character[0])
            | (character.size() == 2 ? static_cast<std::uint32_t>(character[1]) << 16U : 0U);
        const std::uint64_t key = (static_cast<std::uint64_t>(fontId) << 32U) | units;
        const auto found = glyphs_.find(key);
        if (found != glyphs_.end()) { return &found->second; }
        const SIZE size = MeasureGlyphSize(character, fontId);
        Gdiplus::Bitmap bitmap(size.cx, size.cy, PixelFormat32bppARGB);
        if (bitmap.GetLastStatus() != Gdiplus::Ok) { return nullptr; }
        bool rendered;
        {
            const Gdiplus::Font& font = *registered->second->font;
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
        MarkTextureSurfaceModified(atlas->surface.Get(), {x, y, x + size.cx, y + size.cy});
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
    std::vector<std::unique_ptr<Atlas>> atlases_;
    std::map<std::uint64_t, Glyph> glyphs_;
};

static std::unique_ptr<FontGlyphCache> gFontGlyphCache;

void ResetFontGlyphCache() noexcept
{
    gFontGlyphCache.reset();
}

// 原[456190,456528)；FC01先保证0号兜底，再从font.yml注册所有字体。
std::int32_t InitializeFont_456190(const std::int32_t)
{
    static_cast<void>(ReleaseFont_456530());
    gFontSession = std::make_unique<FontSession>();
    auto resource = std::make_unique<FontResource>();
    resource->metrics = kFallbackFontMetrics;
    CreateSystemFont(*resource, kFaceName5590E0, 0, 16, FW_BOLD, false);
    gFonts.emplace(0, std::move(resource));
    LoadFontConfiguration();

    std::int32_t result = 0;
    for (std::size_t index = 0; index < 8; ++index) {
        const FontMetrics& metrics = GetFontMetrics(static_cast<std::int32_t>(index + 6));
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

// 原[456530,456554)；FC01先清图集，再销毁字体和私有集合，最后关闭GDI+。
std::int32_t ReleaseFont_456530()
{
    ResetFontGlyphCache();
    gFonts.clear();
    gFontSession.reset();
    return 0;
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
