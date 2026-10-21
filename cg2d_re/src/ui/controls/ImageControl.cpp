#include "../WindowComponents.hpp"
namespace cg::ui {
void WindowElement_UpdateImageState(WindowElement& element) {
    auto* data = element.data.graphic;
    if (!data || (!data->imageHover && !data->imagePress)) return;
    data->imageNo = element.mouseState == WindowMouseState_Press && data->imagePress > 0 ? data->imagePress
        : element.mouseState == WindowMouseState_Hover && data->imageHover > 0 ? data->imageHover : data->imageNormal;
}
}
