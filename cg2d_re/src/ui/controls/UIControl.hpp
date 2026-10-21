#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include "../WindowComponentHost.hpp"

namespace cg::ui {
using component_detail::InputContext;
using component_detail::COLORREF;
using component_detail::RECT16;
using component_detail::PngTexture;
using component_detail::CanvasShape;
using component_detail::YobiAction;
struct UIClipRect { short left = 0; short top = 0; short right = 0; short bottom = 0; };
// 跨回调仅借身份；创建序号防止同ID新对象被当作旧对象。
struct WindowElementRef {
    int winId = -1;
    std::uint32_t generation = 0;
    ElementId elementId = 0;
    std::uint64_t creationSerial = 0;
};
WindowElementRef WindowRuntime_ElementRef(const WindowElement& element);
WindowElement* WindowRuntime_ResolveElement(const WindowElementRef& ref);
// 动画配置仅保存资源参数，真实action仍由WindowAnimationData拥有。
struct WindowAnimationOptions {
    int animeNo = 0;
    int action = 0;
    int dir = 0;
    int revertPlay = 0;
    int actionAnimeNo = -1;
};
// 仅需要自有输入上下文的元素分配；原生输入仍借用既有context。
struct WindowTextInputState {
    uint8_t color = 0;
    uint8_t font = 0;
    int maxLength = 0;
    std::wstring lastText;
    bool focused = false;
    InputContext* previousInput = nullptr;
    void* previousElement = nullptr;
    InputContext context{};
};
struct WindowPngImageState { std::wstring path; RECT16 rect{}; bool hasRect = false; };
struct WindowPngImageData {
    WindowPngImageState image;
    WindowPngImageState imageHover;
    WindowPngImageState imagePress;
    COLORREF color = 0xffffffff;
};
bool UIIntersectClipRect(UIClipRect& out, const UIClipRect* a, const UIClipRect& b);
bool UIClipRectValid(const UIClipRect& rect);
void WindowRuntime_ClipGraphics(std::uint16_t first, const UIClipRect& clip);
bool UIPointInClipRect(int x, int y, const UIClipRect* clipRect);
bool UIRectIntersectsClipRect(int x, int y, int w, int h, const UIClipRect* clipRect);
int RendererDrawRectClipped(int x, int y, int w, int h, uint8_t layer, COLORREF color, const UIClipRect* clipRect);
int RendererDrawPngClipped(const wchar_t* path, RECT16* loadPngRect, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clipRect);
PngTexture* RendererCreateSurfaceTexture(int width, int height);
bool RendererUpdateSurfaceTexture(PngTexture* texture, const uint32_t* pixels, int width, int height);
void RendererReleaseSurfaceTexture(PngTexture* texture);
int RendererDrawSurfaceTextureClipped(PngTexture* texture, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clipRect);
int RendererDrawGraphicClipped(int x, int y, uint8_t layer, int imageNo, int w, int h, COLORREF color, const UIClipRect* clipRect);
int RendererDrawCanvasClipped(int x, int y, int w, int h, uint8_t layer, const std::vector<CanvasShape>& shapes, const UIClipRect* clipRect);
void RendererDrawTextClipped(int x, int y, unsigned __int8 layer, char fontType, uint8_t colorType, std::wstring_view msg, char hoverMode, char hoverFrame, const UIClipRect* clipRect);
} // namespace cg::ui
