#include "../LuaUI.hpp"

namespace cg::script {
static constexpr uint8_t ANIME_ACTION_PRIO = 2;
static constexpr char ANIME_DISP_PRIO = -33;

UIAnimeControl::UIAnimeControl(int winId, uint32_t generation, int id, const char* name, short x, short y, short w, short h, uint8_t visible, AnimeData data, LuaCallbackRefs callbacks)
    : UIControl(winId, generation, id, TYPE_ANIME, name, x, y, w, h, visible, callbacks), data(data) {
    resetAction();
}

UIAnimeControl::~UIAnimeControl() {
    releaseAction();
}

void UIAnimeControl::resetAction() {
    releaseAction();
    action = yobi::detail::NewYobiAction_404AE0(ANIME_ACTION_PRIO, 0);
    if (!action) {
        return;
    }

    action->flags |= 2u;
    action->callback = nullptr;
    action->imageNo = data.animeNo;
    action->characterAction = data.action;
    action->direction = data.dir;
    action->displayPriority = ANIME_DISP_PRIO;
    actionAnimeNo = data.animeNo;
}

void UIAnimeControl::releaseAction() {
    if (!action) {
        return;
    }

    YobiMarkDeath(action);
    action = nullptr;
    actionAnimeNo = -1;
}

bool UIAnimeControl::render(short ox, short oy) {
    if (visible != 1) {
        return true;
    }
    if (!UIRectIntersectsClipRect(ox + x, oy + y, w, h, nullptr)) {
        return true;
    }
    if (!action || actionAnimeNo != data.animeNo) {
        resetAction();
    }
    if (!action) {
        return true;
    }

    action->characterAction = data.action;
    action->direction = data.dir;
    yobi::detail::Sub_517880(action, 100, data.revertPlay != 0, 0);
    int graphicNo = action->graphicIndex;
    if (!graphicNo) {
        return true;
    }

    auto* mode = &action->drawMode;
    if (action->rebirthValue1) {
        auto mask = static_cast<uint8_t>(action->rebirthValue1);
        mode->mixFlags = static_cast<char>((mask & static_cast<uint8_t>(action->rebirthValue2)) | (static_cast<uint8_t>(mode->mixFlags) & ~mask));
    }

    auto batchType = static_cast<uint8_t>(action->batchType);
    if (batchType) {
        auto* color = reinterpret_cast<uint8_t*>(&mode->color);
        auto* color2 = reinterpret_cast<uint8_t*>(&action->color2);
        if (batchType & 1) {
            color[2] = color2[2];
        }
        if (batchType & 2) {
            color[1] = color2[1];
        }
        if (batchType & 4) {
            color[0] = color2[0];
        }
        if (batchType & 8) {
            color[3] = color2[3];
        }
    }

    // int frameOffsetX = YobiGetFrameOffset(action, 0);
    // int frameOffsetY = YobiGetFrameOffset(action, 1);
    // int graphicOffsetX = action->w - frameOffsetX;
    // int graphicOffsetY = action->h - frameOffsetY;

    render::DrawGraphic2_521DD0(
        ox + x /*- action->w / 2*/ + w / 2,
        oy + y /*- action->h / 2*/ + h / 2,
        LAYER_UI,
        graphicNo,
        0,
        BATCH_SPRITE,
        mode);
    return true;
}

bool UIAnimeControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (!UIRectIntersectsClipRect(ox + x, oy + y, w, h, clipRect)) {
        return true;
    }
    return render(ox, oy);
}

UIControl::AnimeData* UIAnimeControl::animeData() {
    return &data;
}

const UIControl::AnimeData* UIAnimeControl::animeData() const {
    return &data;
}
} // namespace cg::script
