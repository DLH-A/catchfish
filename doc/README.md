# catchfish 开发上手指南

**开发原则：一切玩法皆组件。新增功能就写组件，再交给 World 管理。**

## 1. 先运行

1. 安装 Visual Studio 2022 的“使用 C++ 的桌面开发”（MSVC v143、Windows SDK）。
2. 准备配套的 MSVC x64 版 raylib，在根目录 `raylib.props` 中按注释添加自己的用户名和安装路径。目录下应有 `include/raylib.h` 和 `lib/raylib.lib`，不要覆盖其他成员的配置。
3. 打开 `catchfish.sln`，选择 **Debug / x64**。
4. 将“项目属性 → 调试 → 工作目录”设为 `$(MSBuildProjectDirectory)`，确认项目根目录下有 `assets` 文件夹，没有就创建。
5. 按 F5。目前没有注册玩法组件，出现浅色空白窗口是正常的。

## 2. 代码放哪里

- `src/core/main.cpp`：启动程序、创建服务、注册组件、执行主循环和退出清理。
- `src/core/Component.h`：组件基类，新增玩法从继承它开始。
- `src/core/World.h`：管理组件，统一调用初始化、更新、绘制和销毁。
- `src/core/GAME.h`：共享状态和服务入口，组件通过 `game` 访问。
- `src/core/EventBus.h/.cpp`：组件之间传递事件，例如鼠标点击、捕获通知。
- `src/Manager/`：资源加载与音频播放，组件直接使用现有服务。
- `assets/`：图片、音效、音乐。建议分为 `images/`、`sounds/`、`music/`。

新玩法可以放在 `src/gameplay/` 下，例如 `FishComponent`、`CannonComponent`、`ScoreComponent`。这些是建议的命名，当前还未实现。

## 3. 新功能怎么写

组件的四个入口记住就够了：

- `Init()`：准备数据、加载资源、订阅事件。此时可以使用 `game`。
- `update(float dt)`：处理逻辑，例如移动、发射、碰撞。`dt` 是两帧之间的秒数。
- `draw()`：绘制图片、文字和界面。
- `destroy()`：退订事件、停止组件持有的播放、释放资源句柄。

例如，创建 `src/gameplay/FishComponent.h`，先用圆形代替鱼：

```cpp
#pragma once
#include "Component.h"

class FishComponent : public Component {
    float x_ = 0.0f;

public:
    void update(float dt) override {
        if (!isActive) return;
        x_ += 100.0f * dt; // 每秒向右移动 100 像素
        if (x_ > GetScreenWidth() + 20.0f) x_ = -20.0f;
    }

    void draw() override {
        if (!isActive) return;
        DrawCircleV(Vector2{x_, 300.0f}, 20.0f, SKYBLUE);
    }
};
```

在 `main.cpp` 顶部添加头文件：

```cpp
#include "gameplay/FishComponent.h"
#include <memory>
```

在 `resourceManager.Init("assets");` 之后、主循环之前添加一行注册：

```cpp
world.addComponent(std::make_unique<FishComponent>());
```

按 F5，就能看到圆形向右移动。`World` 会自动设置 `game`、调用 `Init()`，并逐帧更新和绘制，**不要在 main 中再手动调用组件的 update 或 draw**。

## 4. 团队开发约定

1. **main 只负责组装和运行框架**.允许添加头文件和组件注册；鱼的移动、开火判断、碰撞、计分、界面等业务逻辑全部写进组件，不往主循环里追加玩法代码。
2. **一个组件负责一类功能**.例如鱼组件管理鱼的生成和移动，炮台组件处理瞄准和发射，分数组件处理计分和显示。鱼、子弹的数据可以放在所属组件的容器里，不要求每条鱼都注册成一个 World 组件。
3. **自己的数据放在自己的组件中**.不要把鱼的位置、子弹列表、临时计时器都塞进 `Game`，也不要为了共享数据随意增加全局变量。
4. **组件间的通知优先走 EventBus**.例如发布“鱼被捕获”事件，由分数组件处理奖励。新增事件需要定义类型、事件类和订阅处理；仅发布事件不会自动执行玩法。
5. **使用现有服务**.通过 `game->resourceManager` 加载图片和音效，通过 `game->audioPlayer` 播放音频，通过 `game->eventBus` 传递事件，不在每个组件中重复创建管理器。
6. **初始化、逻辑、绘制分开**.资源放在 `Init()` 中加载，移动使用 `速度 * dt`，绘制放在 `draw()`；不要每帧加载素材或无条件播放音效。
7. **组件要负责收尾**.订阅事件后，在 `destroy()` 中用相同事件类型和 `this` 退订；管理器返回的资源用共享句柄保存，不手动卸载纹理或源音效。
8. **基础框架改动先与团队沟通**.普通玩法开发优先新增自己的组件文件，减少多人同时修改 `main`、`World` 和管理器。

## 5. 使用现有模块的小提示

- **图片：**`game->resourceManager->GetImage("images/fish.png")`，保存返回的 `ImageHandle`，检查非空后绘制 `handle->texture`。
- **音效**:先用 `GetSound("sounds/click.wav")` 获取句柄，再传给 `game->audioPlayer->PlaySound(handle)`。
- **音乐：**`game->audioPlayer->PlayMusic("assets/music/bgm.ogg")`。音乐路径要带 `assets/`，目前支持 OGG / WAV；主循环已有音频 `Update()`，组件无需重复调用。
- **鼠标**:使用 `EVENT_MOUSE_CLICK` 前，要在主循环前注册 `MouseComponent`，在组件的 `Init()` 中订阅，在 `destroy()` 中退订。
- **加载失败**:检查工作目录、文件路径和扩展名，查看对应服务的 `LastError()`。

## 6. 当前框架要注意

- `World` 不会自动判断 `isActive`，需要组件自己检查，如上面的示例。
- 不要在 `World` 更新或绘制的过程中直接增删其组件，也不要在事件回调中修改当前事件的订阅列表；事件同步执行，不要保存传入的事件指针。
- 接入素材时需要补齐入口的退出清理：销毁组件并释放外部句柄 → `audioPlayer.Shutdown()` → `resourceManager.Shutdown()` → `CloseAudioDevice()` → `CloseWindow()`。当前入口尚未完整实现该顺序。

更多框架细节见 [框架记录](../docs/framework-review.md)。新增文件后若 VS 中没显示，可重新加载项目，并按需在 `.vcxproj.filters` 中添加分组。
