#include "SdlRendererBackend.hpp"

#include "platform/Window.hpp"
#include "SpriteCmd.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <algorithm>
#include <limits>
#include <vector>

namespace cg::render {
namespace {

[[nodiscard]] SDL_PixelFormat ToSdlPixelFormat(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Rgba8:
        return SDL_PIXELFORMAT_RGBA32;
    case PixelFormat::Bgra8:
        return SDL_PIXELFORMAT_BGRA32;
    case PixelFormat::Unknown:
        break;
    }

    return SDL_PIXELFORMAT_UNKNOWN;
}

[[nodiscard]] std::uint32_t NextGeneration(std::uint32_t generation) noexcept
{
    ++generation;
    if (generation == 0) {
        ++generation;
    }

    return generation;
}

[[nodiscard]] SDL_FColor ToSdlColor(ColorF color) noexcept
{
    return SDL_FColor{color.r, color.g, color.b, color.a};
}

[[nodiscard]] SDL_Vertex MakeVertex(float x, float y, float u, float v, SDL_FColor color) noexcept
{
    return SDL_Vertex{SDL_FPoint{x, y}, color, SDL_FPoint{u, v}};
}

void AppendSpriteGeometry(const SpriteCmd& sprite,
    float textureWidth,
    float textureHeight,
    std::vector<SDL_Vertex>& vertices,
    std::vector<int>& indices)
{
    const int baseIndex = static_cast<int>(vertices.size());
    const float x0 = sprite.destination.x;
    const float y0 = sprite.destination.y;
    const float x1 = sprite.destination.x + sprite.destination.width;
    const float y1 = sprite.destination.y + sprite.destination.height;
    const float u0 = sprite.source.x / textureWidth;
    const float v0 = sprite.source.y / textureHeight;
    const float u1 = (sprite.source.x + sprite.source.width) / textureWidth;
    const float v1 = (sprite.source.y + sprite.source.height) / textureHeight;
    const SDL_FColor color = ToSdlColor(sprite.color);

    vertices.push_back(MakeVertex(x0, y0, u0, v0, color));
    vertices.push_back(MakeVertex(x1, y0, u1, v0, color));
    vertices.push_back(MakeVertex(x1, y1, u1, v1, color));
    vertices.push_back(MakeVertex(x0, y1, u0, v1, color));

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
}

} // namespace

struct SdlRendererBackend::Impl {
    struct TextureSlot {
        SDL_Texture* texture = nullptr;
        float width = 0.0F;
        float height = 0.0F;
        std::uint32_t generation = 1;
    };

    ~Impl()
    {
        Reset();
    }

    [[nodiscard]] bool Initialize(WindowId windowId) noexcept
    {
        Reset();

        if (windowId == 0) {
            return false;
        }

        SDL_Window* window = SDL_GetWindowFromID(static_cast<SDL_WindowID>(windowId));
        if (window == nullptr) {
            return false;
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        return renderer != nullptr;
    }

    void Reset() noexcept
    {
        DestroyTextures();

        if (renderer != nullptr) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
    }

    [[nodiscard]] TextureHandle CreateTexture(const TextureDesc& desc)
    {
        if (renderer == nullptr || desc.width <= 0 || desc.height <= 0) {
            return {};
        }

        const SDL_PixelFormat format = ToSdlPixelFormat(desc.format);
        if (format == SDL_PIXELFORMAT_UNKNOWN) {
            return {};
        }

        const std::size_t slotIndex = AcquireTextureSlot();
        if (slotIndex == InvalidSlotIndex) {
            return {};
        }

        SDL_Texture* texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STATIC, desc.width, desc.height);
        if (texture == nullptr) {
            return {};
        }

        TextureSlot& slot = textureSlots[slotIndex];
        slot.texture = texture;
        slot.width = static_cast<float>(desc.width);
        slot.height = static_cast<float>(desc.height);
        return TextureHandle{static_cast<std::uint32_t>(slotIndex + 1), slot.generation};
    }

    [[nodiscard]] bool UpdateTexture(TextureHandle handle, const void* pixels, int pitch) noexcept
    {
        if (pixels == nullptr || pitch <= 0) {
            return false;
        }

        SDL_Texture* texture = ResolveTexture(handle);
        if (texture == nullptr) {
            return false;
        }

        return SDL_UpdateTexture(texture, nullptr, pixels, pitch);
    }

    void DestroyTexture(TextureHandle handle) noexcept
    {
        TextureSlot* slot = FindTextureSlot(handle);
        if (slot == nullptr) {
            return;
        }

        SDL_DestroyTexture(slot->texture);
        slot->texture = nullptr;
        slot->width = 0.0F;
        slot->height = 0.0F;
        slot->generation = NextGeneration(slot->generation);
    }

    [[nodiscard]] bool DrawSprites(const SpriteCmd* sprites, std::size_t count)
    {
        if (count == 0) {
            return true;
        }
        if (renderer == nullptr || sprites == nullptr) {
            return false;
        }

        std::size_t batchStart = 0;
        while (batchStart < count) {
            TextureSlot* slot = FindTextureSlot(sprites[batchStart].texture);
            if (slot == nullptr || slot->width <= 0.0F || slot->height <= 0.0F) {
                return false;
            }

            std::size_t batchEnd = batchStart + 1;
            while (batchEnd < count && sprites[batchEnd].texture == sprites[batchStart].texture) {
                ++batchEnd;
            }

            while (batchStart < batchEnd) {
                const std::size_t batchCount = std::min(batchEnd - batchStart, MaxSpritesPerGeometry);
                if (!RenderGeometryBatch(*slot, sprites + batchStart, batchCount)) {
                    return false;
                }
                batchStart += batchCount;
            }
        }

        return true;
    }

    [[nodiscard]] SDL_Texture* ResolveTexture(TextureHandle handle) noexcept
    {
        TextureSlot* slot = FindTextureSlot(handle);
        if (slot == nullptr) {
            return nullptr;
        }

        return slot->texture;
    }

    [[nodiscard]] std::size_t AcquireTextureSlot()
    {
        for (std::size_t index = 0; index < textureSlots.size(); ++index) {
            if (textureSlots[index].texture == nullptr) {
                return index;
            }
        }

        if (textureSlots.size() >= static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())) {
            return InvalidSlotIndex;
        }

        textureSlots.push_back(TextureSlot{});
        return textureSlots.size() - 1;
    }

    [[nodiscard]] TextureSlot* FindTextureSlot(TextureHandle handle) noexcept
    {
        if (!handle.IsValid()) {
            return nullptr;
        }

        const std::size_t index = static_cast<std::size_t>(handle.id - 1);
        if (index >= textureSlots.size()) {
            return nullptr;
        }

        TextureSlot& slot = textureSlots[index];
        if (slot.texture == nullptr || slot.generation != handle.generation) {
            return nullptr;
        }

        return &slot;
    }

    void DestroyTextures() noexcept
    {
        for (TextureSlot& slot : textureSlots) {
            if (slot.texture == nullptr) {
                continue;
            }

            SDL_DestroyTexture(slot.texture);
            slot.texture = nullptr;
            slot.width = 0.0F;
            slot.height = 0.0F;
            slot.generation = NextGeneration(slot.generation);
        }
    }

    [[nodiscard]] bool RenderGeometryBatch(TextureSlot& slot, const SpriteCmd* sprites, std::size_t count)
    {
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;
        vertices.reserve(count * VerticesPerSprite);
        indices.reserve(count * IndicesPerSprite);

        for (std::size_t index = 0; index < count; ++index) {
            AppendSpriteGeometry(sprites[index], slot.width, slot.height, vertices, indices);
        }

        return SDL_RenderGeometry(renderer,
            slot.texture,
            vertices.data(),
            static_cast<int>(vertices.size()),
            indices.data(),
            static_cast<int>(indices.size()));
    }

    static constexpr std::size_t InvalidSlotIndex = std::numeric_limits<std::size_t>::max();
    static constexpr std::size_t VerticesPerSprite = 4;
    static constexpr std::size_t IndicesPerSprite = 6;
    static constexpr std::size_t MaxSpritesPerGeometry =
        static_cast<std::size_t>(std::numeric_limits<int>::max()) / IndicesPerSprite;

    SDL_Renderer* renderer = nullptr;
    std::vector<TextureSlot> textureSlots;
};

SdlRendererBackend::SdlRendererBackend()
    : impl_(std::make_unique<Impl>())
{
}

SdlRendererBackend::SdlRendererBackend(WindowId windowId)
    : SdlRendererBackend()
{
    (void)Initialize(windowId);
}

SdlRendererBackend::SdlRendererBackend(const platform::Window& window)
    : SdlRendererBackend()
{
    (void)Initialize(window);
}

SdlRendererBackend::~SdlRendererBackend() = default;

bool SdlRendererBackend::Initialize(WindowId windowId) noexcept
{
    return impl_->Initialize(windowId);
}

bool SdlRendererBackend::Initialize(const platform::Window& window) noexcept
{
    return Initialize(static_cast<WindowId>(window.GetId()));
}

void SdlRendererBackend::Shutdown() noexcept
{
    impl_->Reset();
}

bool SdlRendererBackend::BeginFrame(ClearColor clearColor)
{
    if (impl_->renderer == nullptr) {
        return false;
    }

    return SDL_SetRenderDrawColorFloat(impl_->renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a)
        && SDL_RenderClear(impl_->renderer);
}

bool SdlRendererBackend::EndFrame()
{
    if (impl_->renderer == nullptr) {
        return false;
    }

    return SDL_RenderPresent(impl_->renderer);
}

TextureHandle SdlRendererBackend::CreateTexture(const TextureDesc& desc)
{
    return impl_->CreateTexture(desc);
}

bool SdlRendererBackend::UpdateTexture(TextureHandle texture, const void* pixels, int pitch)
{
    return impl_->UpdateTexture(texture, pixels, pitch);
}

void SdlRendererBackend::DestroyTexture(TextureHandle texture) noexcept
{
    impl_->DestroyTexture(texture);
}

bool SdlRendererBackend::DrawTexture(TextureHandle texture, const RectF& source, const RectF& destination)
{
    if (impl_->renderer == nullptr) {
        return false;
    }

    SDL_Texture* sdlTexture = impl_->ResolveTexture(texture);
    if (sdlTexture == nullptr) {
        return false;
    }

    const SDL_FRect sourceRect = {source.x, source.y, source.width, source.height};
    const SDL_FRect destinationRect = {destination.x, destination.y, destination.width, destination.height};
    return SDL_RenderTexture(impl_->renderer, sdlTexture, &sourceRect, &destinationRect);
}

bool SdlRendererBackend::DrawSprites(const SpriteCmd* sprites, std::size_t count)
{
    return impl_->DrawSprites(sprites, count);
}

} // namespace cg::render
