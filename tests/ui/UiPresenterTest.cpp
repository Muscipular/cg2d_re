#include "ui/GraphicalUiPresenter.hpp"
#include "ui/SdlTextUiPresenter.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#ifndef CG_UI_TEST_SOURCE_DIR
#define CG_UI_TEST_SOURCE_DIR "."
#endif

namespace {

using cg::ui::GraphicalUiPresenter;
using cg::ui::SdlTextUiPresenter;
using cg::ui::TuiFrame;
using cg::ui::UiInputEvent;
using cg::ui::UiInputEventType;
using cg::ui::UiIntent;
using cg::ui::UiIntentType;
using cg::ui::UiKey;
using cg::ui::UiMenuItem;
using cg::ui::UiScreenType;
using cg::ui::UiTextDraw;
using cg::ui::UiViewModel;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

std::string Join(const char* left, const char* right)
{
    std::string text(left);
    text += right;
    return text;
}

bool ReadFile(const std::filesystem::path& path, std::string& out)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return false;
    }

    out.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    return true;
}

bool FrameHasText(const TuiFrame& frame, const std::string& text)
{
    for (const auto& run : frame.runs) {
        if (run.text.find(text) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool DrawsHaveText(const std::vector<UiTextDraw>& draws, const std::string& text)
{
    for (const UiTextDraw& draw : draws) {
        if (draw.text.find(text) != std::string::npos) {
            return true;
        }
    }
    return false;
}

UiViewModel MakeMenuModel(UiScreenType screen)
{
    UiViewModel model;
    model.screen = screen;
    model.menuItems = {UiMenuItem{"First", true}, UiMenuItem{"Second", true}};
    model.selectedIndex = 1U;
    return model;
}

int TestPublicHeaderSurface()
{
    int failures = 0;
    const std::filesystem::path root(CG_UI_TEST_SOURCE_DIR);
    const std::array<std::filesystem::path, 8> headers{{
        root / "src/ui/UiIntent.hpp",
        root / "src/ui/UiViewModel.hpp",
        root / "src/ui/IUiPresenter.hpp",
        root / "src/ui/TuiFrame.hpp",
        root / "src/ui/UiTextDraw.hpp",
        root / "src/ui/UiTextRenderer.hpp",
        root / "src/ui/SdlTextUiPresenter.hpp",
        root / "src/ui/GraphicalUiPresenter.hpp",
    }};
    const std::array<std::string, 6> forbidden{{
        Join("S", "DL"),
        Join("T", "TF"),
        Join("Win", "32"),
        Join("platform", "/Event"),
        Join("dynamic", "_cast"),
        Join("type", "id"),
    }};

    for (const std::filesystem::path& header : headers) {
        std::string text;
        failures += Expect(ReadFile(header, text), "ui 公开头可读取");
        for (const std::string& token : forbidden) {
            failures += Expect(text.find(token) == std::string::npos, "ui 公开头无禁止边界");
        }
    }

    return failures;
}

int TestFrames()
{
    int failures = 0;
    SdlTextUiPresenter presenter;

    UiViewModel login;
    login.screen = UiScreenType::Login;
    login.inputText = "user";
    presenter.SetViewModel(login);
    failures += Expect(FrameHasText(presenter.BuildFrame(), "Login"), "Login frame");

    presenter.SetViewModel(MakeMenuModel(UiScreenType::CharacterSelect));
    failures += Expect(FrameHasText(presenter.BuildFrame(), "CharacterSelect"), "CharacterSelect frame");

    UiViewModel world;
    world.screen = UiScreenType::World;
    world.chatLines = {"hello"};
    world.inputText = "你好";
    presenter.SetViewModel(world);
    failures += Expect(FrameHasText(presenter.BuildFrame(), "World"), "World frame");
    failures += Expect(FrameHasText(presenter.BuildFrame(), "你好"), "World frame 保留 UTF-8");

    presenter.SetViewModel(MakeMenuModel(UiScreenType::Battle));
    failures += Expect(FrameHasText(presenter.BuildFrame(), "Battle"), "Battle frame");
    return failures;
}

int TestDrawableTextInputs()
{
    int failures = 0;
    SdlTextUiPresenter tui;
    GraphicalUiPresenter graphical;

    UiViewModel world;
    world.screen = UiScreenType::World;
    world.title = "Scene";
    world.chatLines = {"hello", "你好"};
    world.inputText = "say";
    tui.SetViewModel(world);
    graphical.SetViewModel(world);

    const auto tuiDraws = tui.BuildTextDraws({10.0F, 20.0F, 8.0F, 16.0F, 5U, 9U, {1.0F, 1.0F, 1.0F, 1.0F}});
    const auto graphicalDraws = graphical.BuildTextDraws({200.0F, 30.0F, 18.0F, 6U, 30U, {1.0F, 1.0F, 1.0F, 1.0F}, {0.8F, 0.8F, 0.8F, 1.0F}});

    failures += Expect(!tuiDraws.empty(), "TUI 生成文本绘制输入");
    failures += Expect(!graphicalDraws.empty(), "Graphical 生成文本绘制输入");
    failures += Expect(DrawsHaveText(tuiDraws, "你好"), "TUI 绘制输入保留 UTF-8");
    failures += Expect(DrawsHaveText(graphicalDraws, "你好"), "Graphical 绘制输入保留 UTF-8");
    failures += Expect(tuiDraws[0].destination.x == 10.0F && tuiDraws[0].destination.y == 20.0F, "TUI 固定网格映射坐标");
    failures += Expect(graphicalDraws[0].layer == 6U && graphicalDraws[0].order == 30U, "Graphical 绘制输入 layer/order");
    return failures;
}

int TestTextInputAndEnterIntent()
{
    int failures = 0;
    SdlTextUiPresenter tui;
    GraphicalUiPresenter graphical;

    UiViewModel login;
    login.screen = UiScreenType::Login;
    tui.SetViewModel(login);
    graphical.SetViewModel(login);

    const UiInputEvent textInput{UiInputEventType::TextInput, UiKey::None, "你好"};
    UiIntent tuiIntent = tui.HandleInput(textInput);
    UiIntent graphicalIntent = graphical.HandleInput(textInput);
    failures += Expect(tuiIntent.type == UiIntentType::EditText && tuiIntent.text == "你好", "TUI TextInput 产生 EditText");
    failures += Expect(graphicalIntent.type == UiIntentType::EditText && graphicalIntent.text == "你好", "Graphical TextInput 产生 EditText");
    failures += Expect(tui.ViewModel().inputText == "你好", "TUI UTF-8 原样保存");
    failures += Expect(graphical.ViewModel().inputText == "你好", "Graphical UTF-8 原样保存");

    const UiInputEvent enter{UiInputEventType::Key, UiKey::Enter, {}};
    tuiIntent = tui.HandleInput(enter);
    graphicalIntent = graphical.HandleInput(enter);
    failures += Expect(tuiIntent.type == UiIntentType::SubmitLogin, "Login Enter SubmitLogin");
    failures += Expect(graphicalIntent.type == tuiIntent.type, "Graphical/TUI 共用 Login intent");

    UiViewModel character = MakeMenuModel(UiScreenType::CharacterSelect);
    tui.SetViewModel(character);
    graphical.SetViewModel(character);
    tuiIntent = tui.HandleInput(enter);
    graphicalIntent = graphical.HandleInput(enter);
    failures += Expect(tuiIntent.type == UiIntentType::SelectCharacter && tuiIntent.index == 1U, "CharacterSelect Enter SelectCharacter");
    failures += Expect(graphicalIntent.type == tuiIntent.type && graphicalIntent.index == tuiIntent.index, "Graphical/TUI 共用角色 intent");

    UiViewModel world;
    world.screen = UiScreenType::World;
    world.inputText = "hello";
    tui.SetViewModel(world);
    graphical.SetViewModel(world);
    tuiIntent = tui.HandleInput(enter);
    graphicalIntent = graphical.HandleInput(enter);
    failures += Expect(tuiIntent.type == UiIntentType::SubmitChat && tuiIntent.text == "hello", "World Enter SubmitChat");
    failures += Expect(graphicalIntent.type == tuiIntent.type && graphicalIntent.text == tuiIntent.text, "Graphical/TUI 共用聊天 intent");

    UiViewModel battle = MakeMenuModel(UiScreenType::Battle);
    tui.SetViewModel(battle);
    graphical.SetViewModel(battle);
    tuiIntent = tui.HandleInput(enter);
    graphicalIntent = graphical.HandleInput(enter);
    failures += Expect(tuiIntent.type == UiIntentType::SelectMenuItem && tuiIntent.index == 1U, "Battle Enter SelectMenuItem");
    failures += Expect(graphicalIntent.type == tuiIntent.type && graphicalIntent.index == tuiIntent.index, "Graphical/TUI 共用菜单 intent");
    return failures;
}

int TestDisabledMenuItem()
{
    int failures = 0;
    SdlTextUiPresenter presenter;
    UiViewModel battle;
    battle.screen = UiScreenType::Battle;
    battle.menuItems = {UiMenuItem{"Disabled", false}};
    battle.selectedIndex = 0U;
    presenter.SetViewModel(battle);

    const UiIntent intent = presenter.HandleInput({UiInputEventType::Key, UiKey::Enter, {}});
    failures += Expect(intent.type == UiIntentType::None, "禁用菜单项不产生 SelectMenuItem");
    return failures;
}

int TestUtf8Backspace()
{
    int failures = 0;
    SdlTextUiPresenter presenter;
    UiViewModel world;
    world.screen = UiScreenType::World;
    world.inputText = "你好a";
    presenter.SetViewModel(world);

    UiIntent intent = presenter.HandleInput({UiInputEventType::Key, UiKey::Backspace, {}});
    failures += Expect(intent.type == UiIntentType::EditText, "Backspace 产生 EditText");
    failures += Expect(presenter.ViewModel().inputText == "你好", "Backspace 删除 ASCII codepoint");

    intent = presenter.HandleInput({UiInputEventType::Key, UiKey::Backspace, {}});
    failures += Expect(intent.type == UiIntentType::EditText, "中文 Backspace 产生 EditText");
    failures += Expect(presenter.ViewModel().inputText == "你", "Backspace 按 UTF-8 codepoint 删除中文");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestPublicHeaderSurface();
    failures += TestFrames();
    failures += TestDrawableTextInputs();
    failures += TestTextInputAndEnterIntent();
    failures += TestDisabledMenuItem();
    failures += TestUtf8Backspace();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
