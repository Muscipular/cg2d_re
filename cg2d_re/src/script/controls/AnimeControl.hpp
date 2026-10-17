#pragma once
#include "UIControl.hpp"

namespace cg::script {
struct UIAnimeControl : UIControl {
    AnimeData data;
    YobiAction* action = nullptr;
    int actionAnimeNo = -1;

    UIAnimeControl(int winId, uint32_t generation, int id, const char* name, short x, short y, short w, short h, uint8_t visible, AnimeData data, LuaCallbackRefs callbacks);
    ~UIAnimeControl() override;

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    AnimeData* animeData() override;
    const AnimeData* animeData() const override;

    void resetAction();
    void releaseAction();
};
} // namespace cg::script
