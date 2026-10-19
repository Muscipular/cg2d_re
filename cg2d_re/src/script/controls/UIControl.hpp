#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include "../LuaHost.hpp"

namespace cg::script {
struct UIClipRect {
    short left = 0;
    short top = 0;
    short right = 0;
    short bottom = 0;
};

struct LuaWindowRef {
    int winId = -1;
    uint32_t generation = 0;
};

struct LuaControlRef {
    int winId = -1;
    uint32_t generation = 0;
    int controlId = 0;
};

struct LuaCallbackRefs {
    lua_State* owner = nullptr; // registry引用所属状态。
    int onEvent = -1;
    int onClick = -1;
    int onPress = -1;
    int onHover = -1;
    int onLeave = -1;
    int onDrag = -1;
    int onDrop = -1;
    int onChange = -1;
    int onEnter = -1;

    void clear(lua_State* L);
};

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
    struct ImageData {
        int imageNo;
        int imageHover;
        int imagePress;
        COLORREF color;
    };
    struct AnimeData {
        int animeNo;
        int action;
        int dir;
        int revertPlay;
    };
    struct TextData {
        uint8_t color;
        uint8_t font;
        std::wstring text; // 内部宽文本；仅Lua读写属性时转换GBK。
    };
    struct PngImageState {
        std::string path;
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
        std::string lastText;
        bool focused = false;
        InputContext* previousInput = nullptr;
        InputContext context{};
    };

    int winId;
    uint32_t generation;
    int id;
    TYPE type;
    uint8_t visible;
    uint8_t mouseState = MOUSE_NONE;
    bool hitable = true;
    int parentId = 0;
    std::string name{};

    short x, y, w, h;
    LuaCallbackRefs callbacks;

    UIControl(int winId, uint32_t generation, int id, TYPE type, const char* name, short x, short y, short w, short h, uint8_t visible, LuaCallbackRefs callbacks);

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

    virtual void setTextInputText(const char* text);

    virtual void syncTextInputText();
};

bool UIIntersectClipRect(UIClipRect& out, const UIClipRect* a, const UIClipRect& b);
bool UIClipRectValid(const UIClipRect& rect);
bool UIPointInClipRect(int x, int y, const UIClipRect* clipRect);
bool UIRectIntersectsClipRect(int x, int y, int w, int h, const UIClipRect* clipRect);
void UIEmitControlLeaveIfNeeded(LuaControlRef controlRef, LuaCallbackRefs callbacks, uint8_t previousMouseState, uint8_t mouseState);
int RendererDrawRectClipped(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t layer, COLORREF color, const UIClipRect* clipRect);
int RendererDrawPngClipped(const char* path, RECT16* loadPngRect, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clipRect);
PngTexture* RendererCreateSurfaceTexture(int width, int height);
bool RendererUpdateSurfaceTexture(PngTexture* texture, const uint32_t* pixels, int width, int height);
void RendererReleaseSurfaceTexture(PngTexture* texture);
int RendererDrawSurfaceTextureClipped(PngTexture* texture, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clipRect);
int RendererDrawGraphicClipped(int x, int y, uint8_t layer, int imageNo, int w, int h, COLORREF color, const UIClipRect* clipRect);
int RendererDrawCanvasClipped(int x, int y, int w, int h, uint8_t layer, const std::vector<CanvasShape>& shapes, const UIClipRect* clipRect);
void RendererBreakWindowTextBatch();
void RendererDrawTextClipped(__int16 x, __int16 y, unsigned __int8 layer, char fontType, uint8_t colorType, std::wstring_view msg, char hoverMode, char hoverFrame, const UIClipRect* clipRect);
} // namespace cg::script
