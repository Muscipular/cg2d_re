#include "SdlRendererBackend.hpp"

#include "platform/Window.hpp"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <limits>
#include <vector>

namespace cg::render {
namespace {

[[nodiscard]] SDL_PixelFormat ToSdlPixelFormat(PixelFormat format) noexcept
{
    switch (format) {
    case PixelFormat::Rgba8:
        return SDL_PIXELFORMAT_RGBA8888;
    case PixelFormat::Bgra8:
        return SDL_PIXELFORMAT_BGRA8888;
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

} // namespace

struct SdlRendererBackend::Impl {
    struct TextureSlot {
        SDL_Texture* texture = nullptr;
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
        return TextureHandle{static_cast<std::uint32_t>(slotIndex + 1), slot.generation};
    }

    void DestroyTexture(TextureHandle handle) noexcept
    {
        TextureSlot* slot = FindTextureSlot(handle);
        if (slot == nullptr) {
            return;
        }

        SDL_DestroyTexture(slot->texture);
        slot->texture = nullptr;
        slot->generation = NextGeneration(slot->generation);
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
            slot.generation = NextGeneration(slot.generation);
        }
    }

    static constexpr std::size_t InvalidSlotIndex = std::numeric_limits<std::size_t>::max();

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

} // namespace cg::render
