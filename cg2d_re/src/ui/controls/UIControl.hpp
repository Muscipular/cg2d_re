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
struct UIClipRect {
    short left = 0;
    short top = 0;
    short right = 0;
    short bottom = 0;
};

// UI只识别事件接口；Lua注册表引用封装在脚本侧实现中。
struct WindowControlRef {
    int winId = -1;
    uint32_t generation = 0;
    uint32_t controlId = 0;
};
struct WindowElementEvents {
    virtual ~WindowElementEvents() = default;
    virtual int mouse(WindowControlRef, int) { return 0; }
    virtual int text(WindowControlRef, bool, std::wstring_view) { return 0; }
    virtual void attach(WindowElement&) {}
    virtual void release(WindowElement&) {}
};
using WindowEventBinding = std::shared_ptr<WindowElementEvents>;

struct UIControl {
    enum TYPE : uint8_t {
        TYPE_NONE,
        TYPE_IMAGE,
        TYPE_TEXT,
        TYPE_PNG_IMAGE,
        TYPE_TEXT_INPUT,
        TYPE_ANIME,
        TYPE_SCROLL_VIEW,
        TYPE_MAP_EX,
        TYPE_CANVAS,
    };
    enum MOUSE_STATE : uint8_t {
        MOUSE_NONE = 0,
        MOUSE_HOVER = 1,
        MOUSE_PRESS = 2, //LeftDown
    };
    struct ImageData : WindowGraphicData {
        int imageHover = 0;
        int imagePress = 0;
        ImageData(int image = 0, int hover = 0, int press = 0, COLORREF tint = 0xffffffff)
            : WindowGraphicData{image, 0, 0, 0, 0, tint}, imageHover(hover), imagePress(press) {}
    };
    struct AnimeData {
        int animeNo;
        int action;
        int dir;
        int revertPlay;
    };
    struct TextData : WindowTextData {
        TextData(uint8_t color = 0, uint8_t font = 0, std::wstring value = {})
            : WindowTextData{{std::byte{color}, std::byte{font}, {}, {}}, std::move(value)} {}
    };
    struct PngImageState {
        std::wstring path;
        RECT16 rect{};
        bool hasRect = false;
    };
    struct PngImageData {
        PngImageState image;
        PngImageState imageHover;
        PngImageState imagePress;
        COLORREF color;
    };
    struct TextInputData {
        uint8_t color;
        uint8_t font;
        int maxLength;
        std::wstring lastText;
        bool focused = false;
        InputContext* previousInput = nullptr;
        InputContext context{};
    };

    int winId;
    uint32_t generation;
    uint32_t id;
    TYPE type;
    uint8_t mouseState = MOUSE_NONE;
    bool hitable = true;
    uint32_t parentId = 0;
    std::wstring name{};

    WindowEventBinding callbacks;
    WindowInstance* windowOwner = nullptr; // 借用所属窗口。
    bool attached = true; // 清空时立即失效，回收由UI安全点执行。
    WindowElement draft{}; // 创建期间暂存；加入窗口后活动属性仅在sortedMap独占的稳定对象中。
    WindowElement* currentElement = &draft;
    WindowElement& element() { return *currentElement; }
    const WindowElement& element() const { return *currentElement; }
    WindowAnimationData animationPayload{};
    WindowInputData inputPayload{};
    virtual void bindPayload(); // 将union绑定到组件实际载荷。
    WindowControlRef ref() const { return {winId, generation, id}; }


    UIControl(int winId, uint32_t generation, uint32_t id, TYPE type, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, WindowEventBinding callbacks);

    virtual ~UIControl();

    virtual bool render(short x, short y);
    virtual bool render(short x, short y, const UIClipRect* clipRect);

    virtual bool process(int index, int hasFocused, int captured);
    virtual bool process(int index, int hasFocused, int captured, const UIClipRect* clipRect, short ox, short oy);

    bool isHitable() const;

    virtual ImageData* imageData();
    virtual const ImageData* imageData() const;
    virtual AnimeData* animeData();
    virtual const AnimeData* animeData() const;
    virtual TextData* textData();
    virtual const TextData* textData() const;
    virtual PngImageData* pngImageData();
    virtual const PngImageData* pngImageData() const;
    virtual TextInputData* textInputData();
    virtual const TextInputData* textInputData() const;

    virtual bool isTextInputFocused();

    virtual void blurTextInput();

    virtual void focusTextInput();

    virtual void setTextInputText(std::wstring_view text);

    virtual void syncTextInputText();
};

bool UIIntersectClipRect(UIClipRect& out, const UIClipRect* a, const UIClipRect& b);
bool UIClipRectValid(const UIClipRect& rect);
bool UIPointInClipRect(int x, int y, const UIClipRect* clipRect);
bool UIRectIntersectsClipRect(int x, int y, int w, int h, const UIClipRect* clipRect);
bool IsControlAttached(const WindowControlRef& ref);
void UIResetControlProcessState(UIControl* control);
void UIEmitControlLeaveIfNeeded(WindowControlRef controlRef, WindowEventBinding callbacks, uint8_t previousMouseState, uint8_t mouseState);
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
