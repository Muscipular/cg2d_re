#pragma once
#include "UIControl.hpp"

namespace cg::script {
inline constexpr int TEXT_INPUT_MAX_LENGTH = 287;

int ClampTextInputMaxLength(int maxLength);

struct UITextInputControl : UIControl {
    TextInputData data;

    UITextInputControl(int winId, uint32_t generation, int id, const char* name, short x, short y, short w, short h, uint8_t visible, TextInputData data, LuaCallbackRefs callbacks);

    ~UITextInputControl() override;

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    TextInputData* textInputData() override;

    const TextInputData* textInputData() const override;

    bool isTextInputFocused() override;

    void blurTextInput() override;

    void focusTextInput() override;

    void setTextInputText(const char* text) override;

    void syncTextInputText() override;
};
} // namespace cg::script
