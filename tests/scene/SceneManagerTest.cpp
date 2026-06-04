#include "render/RenderQueue.hpp"
#include "scene/SceneManager.hpp"
#include "scene/WorldScene.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#ifndef CG_SCENE_TEST_SOURCE_DIR
#define CG_SCENE_TEST_SOURCE_DIR "."
#endif

namespace {

using cg::render::RenderQueue;
using cg::scene::IScene;
using cg::scene::ISubScene;
using cg::scene::RootSceneId;
using cg::scene::SceneInputEvent;
using cg::scene::SceneInputEventType;
using cg::scene::SceneManager;
using cg::scene::SubSceneId;
using cg::scene::SubScenePolicy;
using cg::scene::WorldScene;

struct SceneCounters {
    int enter = 0;
    int leave = 0;
    int update = 0;
    int input = 0;
    std::vector<std::string> renders;
};

class RecordingScene final : public IScene {
public:
    RecordingScene(RootSceneId id, SceneCounters& counters, std::string renderName)
        : id_(id)
        , counters_(counters)
        , renderName_(std::move(renderName))
    {
    }

    [[nodiscard]] RootSceneId Id() const noexcept override
    {
        return id_;
    }

    void Enter() override
    {
        ++counters_.enter;
    }

    void Leave() override
    {
        ++counters_.leave;
    }

    void Update(float) override
    {
        ++counters_.update;
    }

    bool HandleInput(const SceneInputEvent&) override
    {
        ++counters_.input;
        return true;
    }

    void Render(RenderQueue&) const override
    {
        counters_.renders.push_back(renderName_);
    }

private:
    RootSceneId id_;
    SceneCounters& counters_;
    std::string renderName_;
};

class RecordingSubScene final : public ISubScene {
public:
    RecordingSubScene(SubSceneId id, SceneCounters& counters, std::string renderName, SubScenePolicy policy, bool consumesInput)
        : id_(id)
        , counters_(counters)
        , renderName_(std::move(renderName))
        , policy_(policy)
        , consumesInput_(consumesInput)
    {
    }

    [[nodiscard]] SubSceneId Id() const noexcept override
    {
        return id_;
    }

    [[nodiscard]] SubScenePolicy Policy() const noexcept override
    {
        return policy_;
    }

    void Enter() override
    {
        ++counters_.enter;
    }

    void Leave() override
    {
        ++counters_.leave;
    }

    void Update(float) override
    {
        ++counters_.update;
    }

    bool HandleInput(const SceneInputEvent&) override
    {
        ++counters_.input;
        return consumesInput_;
    }

    void Render(RenderQueue&) const override
    {
        counters_.renders.push_back(renderName_);
    }

private:
    SubSceneId id_;
    SceneCounters& counters_;
    std::string renderName_;
    SubScenePolicy policy_;
    bool consumesInput_ = false;
};

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

int TestPublicHeaderSurface()
{
    int failures = 0;
    const std::filesystem::path root(CG_SCENE_TEST_SOURCE_DIR);
    const std::array<std::filesystem::path, 4> headers{{
        root / "src/scene/SceneTypes.hpp",
        root / "src/scene/Scene.hpp",
        root / "src/scene/SceneManager.hpp",
        root / "src/scene/WorldScene.hpp",
    }};
    const std::array<std::string, 6> forbidden{{
        Join("S", "DL"),
        Join("T", "TF"),
        Join("Win", "32"),
        Join("dynamic", "_cast"),
        Join("type", "id"),
        Join("platform", "/Event"),
    }};

    for (const std::filesystem::path& header : headers) {
        std::string text;
        failures += Expect(ReadFile(header, text), "scene 公开头可读取");
        for (const std::string& token : forbidden) {
            failures += Expect(text.find(token) == std::string::npos, "scene 公开头无禁止边界");
        }
    }

    return failures;
}

int TestStartAndDelayedSwitch()
{
    int failures = 0;
    SceneCounters boot;
    SceneCounters login;
    SceneManager manager;
    manager.RegisterRootSceneFactory(RootSceneId::Boot, [&boot]() {
        return std::make_unique<RecordingScene>(RootSceneId::Boot, boot, "boot");
    });
    manager.RegisterRootSceneFactory(RootSceneId::Login, [&login]() {
        return std::make_unique<RecordingScene>(RootSceneId::Login, login, "login");
    });

    failures += Expect(manager.Start(RootSceneId::Boot), "Start Boot 成功");
    failures += Expect(!manager.Start(RootSceneId::Boot), "Start 只 Enter 一次");
    failures += Expect(boot.enter == 1 && boot.leave == 0, "Boot enter 一次");

    manager.ReplaceRootScene(RootSceneId::Login);
    failures += Expect(manager.CurrentRootSceneId() == RootSceneId::Boot, "ReplaceRootScene 只入队");
    manager.Update(0.016F);
    failures += Expect(manager.CurrentRootSceneId() == RootSceneId::Login, "Update 后切 Login");
    failures += Expect(boot.leave == 1 && login.enter == 1, "切换调用 Leave/Enter");
    return failures;
}

int TestOneSwitchPerFrameAndSameTargetIgnored()
{
    int failures = 0;
    SceneCounters boot;
    SceneCounters login;
    SceneCounters world;
    SceneManager manager;
    manager.RegisterRootSceneFactory(RootSceneId::Boot, [&boot]() {
        return std::make_unique<RecordingScene>(RootSceneId::Boot, boot, "boot");
    });
    manager.RegisterRootSceneFactory(RootSceneId::Login, [&login]() {
        return std::make_unique<RecordingScene>(RootSceneId::Login, login, "login");
    });
    manager.RegisterRootSceneFactory(RootSceneId::World, [&world]() {
        return std::make_unique<RecordingScene>(RootSceneId::World, world, "world");
    });

    (void)manager.Start(RootSceneId::Boot);
    manager.ReplaceRootScene(RootSceneId::Boot);
    failures += Expect(manager.PendingRootSceneCount() == 0U, "同当前目标切换忽略");

    manager.ReplaceRootScene(RootSceneId::Login);
    manager.ReplaceRootScene(RootSceneId::World);
    manager.Update(0.016F);
    failures += Expect(manager.CurrentRootSceneId() == RootSceneId::Login, "每帧最多切一个到 Login");
    failures += Expect(manager.PendingRootSceneCount() == 1U, "仍保留第二个切换请求");
    manager.Update(0.016F);
    failures += Expect(manager.CurrentRootSceneId() == RootSceneId::World, "第二帧切 World");
    return failures;
}

int TestFactoryFailureKeepsCurrent()
{
    int failures = 0;
    SceneCounters boot;
    SceneManager manager;
    manager.RegisterRootSceneFactory(RootSceneId::Boot, [&boot]() {
        return std::make_unique<RecordingScene>(RootSceneId::Boot, boot, "boot");
    });
    manager.RegisterRootSceneFactory(RootSceneId::Login, []() -> std::unique_ptr<IScene> {
        return {};
    });

    (void)manager.Start(RootSceneId::Boot);
    manager.ReplaceRootScene(RootSceneId::Login);
    manager.Update(0.016F);
    failures += Expect(manager.CurrentRootSceneId() == RootSceneId::Boot, "工厂失败保留当前");
    failures += Expect(boot.leave == 0, "工厂失败不 Leave 当前");
    return failures;
}

int TestModalInputAndTransparentRender()
{
    int failures = 0;
    SceneCounters root;
    SceneCounters overlay;
    SceneCounters modal;
    SceneCounters opaque;
    SceneManager manager;
    manager.RegisterRootSceneFactory(RootSceneId::World, [&root]() {
        return std::make_unique<RecordingScene>(RootSceneId::World, root, "root");
    });
    (void)manager.Start(RootSceneId::World);

    SubScenePolicy transparent;
    transparent.transparentRender = true;
    manager.PushSubScene(std::make_unique<RecordingSubScene>(SubSceneId::Chat, overlay, "overlay", transparent, false));

    RenderQueue queue;
    manager.Render(queue);
    failures += Expect(root.renders.size() == 1U && overlay.renders.size() == 1U, "非 modal transparent 可叠加显示");
    failures += Expect(root.renders[0] == "root" && overlay.renders[0] == "overlay", "render 顺序 root 到 overlay");

    const SceneInputEvent event{SceneInputEventType::Confirm, "ok"};
    failures += Expect(manager.HandleInput(event), "非 modal 未消费时下层 root 接收输入");
    failures += Expect(root.input == 1 && overlay.input == 1, "非 modal 输入透传到 root");

    SubScenePolicy modalPolicy;
    modalPolicy.modal = true;
    modalPolicy.transparentRender = true;
    manager.PushSubScene(std::make_unique<RecordingSubScene>(SubSceneId::ConfirmDialog, modal, "modal", modalPolicy, false));
    failures += Expect(manager.HandleInput(event), "modal 阻断输入");
    failures += Expect(root.input == 1 && overlay.input == 1 && modal.input == 1, "modal 不透传下层输入");

    SubScenePolicy opaquePolicy;
    opaquePolicy.transparentRender = false;
    manager.PushSubScene(std::make_unique<RecordingSubScene>(SubSceneId::Inventory, opaque, "opaque", opaquePolicy, true));
    root.renders.clear();
    overlay.renders.clear();
    modal.renders.clear();
    opaque.renders.clear();
    manager.Render(queue);
    failures += Expect(root.renders.size() == 1U, "非透明 SubScene 仍保留 Root 渲染");
    failures += Expect(overlay.renders.empty() && modal.renders.empty(), "非透明 SubScene 遮挡下层 SubScene");
    failures += Expect(opaque.renders.size() == 1U, "非透明 SubScene 自身渲染");
    return failures;
}

int TestPausesBelowStopsRootAndLowerSubScenes()
{
    int failures = 0;
    SceneCounters root;
    SceneCounters lower;
    SceneCounters pause;
    SceneCounters upper;
    SceneManager manager;
    manager.RegisterRootSceneFactory(RootSceneId::World, [&root]() {
        return std::make_unique<RecordingScene>(RootSceneId::World, root, "root");
    });
    (void)manager.Start(RootSceneId::World);

    SubScenePolicy transparent;
    manager.PushSubScene(std::make_unique<RecordingSubScene>(SubSceneId::Chat, lower, "lower", transparent, false));

    SubScenePolicy pausePolicy;
    pausePolicy.pausesBelow = true;
    manager.PushSubScene(std::make_unique<RecordingSubScene>(SubSceneId::ConfirmDialog, pause, "pause", pausePolicy, true));
    manager.PushSubScene(std::make_unique<RecordingSubScene>(SubSceneId::Inventory, upper, "upper", transparent, true));

    manager.Update(0.016F);
    failures += Expect(root.update == 0, "pausesBelow 暂停 Root update");
    failures += Expect(lower.update == 0, "pausesBelow 暂停下层 SubScene update");
    failures += Expect(pause.update == 1 && upper.update == 1, "pausesBelow 自身和上层继续 update");
    return failures;
}

int TestWorldSceneBridge()
{
    int failures = 0;
    WorldScene scene;
    RenderQueue queue;
    failures += Expect(scene.Id() == RootSceneId::World, "WorldScene id");
    failures += Expect(scene.ActiveEntityCount() == 0U, "WorldScene 初始无实体");
    const cg::world::EntityId id = scene.CreateDemoEntity();
    failures += Expect(id.IsValid(), "WorldScene 可创建 demo entity");
    failures += Expect(scene.ActiveEntityCount() == 1U, "WorldScene 统计实体");

    const cg::render::SpriteCmd sprite{
        cg::render::TextureHandle{1U, 1U},
        {0.0F, 0.0F, 16.0F, 16.0F},
        {8.0F, 8.0F, 16.0F, 16.0F},
        {1.0F, 1.0F, 1.0F, 1.0F},
        1U,
        0,
        7U,
    };
    const std::array<cg::render::SpriteCmd, 1> sprites{{sprite}};
    scene.SetRenderSprites(sprites);
    scene.Render(queue);
    failures += Expect(queue.Size() == 1U, "WorldScene 将 world render sprite 推入 RenderQueue");

    scene.ClearWorld();
    failures += Expect(scene.ActiveEntityCount() == 0U, "WorldScene 可清空");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestPublicHeaderSurface();
    failures += TestStartAndDelayedSwitch();
    failures += TestOneSwitchPerFrameAndSameTargetIgnored();
    failures += TestFactoryFailureKeepsCurrent();
    failures += TestModalInputAndTransparentRender();
    failures += TestPausesBelowStopsRootAndLowerSubScenes();
    failures += TestWorldSceneBridge();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
