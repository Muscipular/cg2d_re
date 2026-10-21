#include "../WindowComponents.hpp"
namespace cg::ui {
void WindowElement_PrepareAnimation(WindowAnimationData& data) {
    auto* options = data.options;
    if (!options) return;
    if (!data.action || options->actionAnimeNo != options->animeNo) {
        if (data.action) yobi::MarkDeathUnchecked(data.action);
        data.action = yobi::NewYobiAction_404AE0(2, 0);
        if (!data.action) return;
        yobi::ConfigureWindowElementAction(data.action, data.action, &options->animeNo);
        options->actionAnimeNo = options->animeNo;
    }
    data.action->characterAction = options->action;
    data.action->direction = options->dir;
    data.value04 = options->revertPlay != 0;
}
}
