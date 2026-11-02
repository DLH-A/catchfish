# catchfish

基于 C++17 和 raylib 的 2D 游戏项目，使用 Visual Studio 2022，平台为 x64。

## 工程文件

- `catchfish.sln`：Visual Studio 解决方案入口。
- `catchfish.vcxproj`：编译选项、源码收集和输出目录。
- `catchfish.vcxproj.filters`：VS 中的文件分类。
- `catchfish.vcxproj.user`：本机调试设置。
- `raylib.props`：团队成员的 raylib 路径、头文件和链接库配置，按 Windows 用户名选择路径。
- `assets/`：图片、声音和音乐素材。
- `docs/`：项目说明文档。

## 核心代码：src/core

- `main.cpp`：程序入口；`main()` 初始化窗口和音频，组装服务，执行更新和绘制循环。
- `GAME.h`：定义 `GameState` 和 `Game`，保存游戏状态与资源、音频、事件服务指针。
- `World.h`：管理组件；`addComponent()` 添加并初始化，`update()` 更新，`draw()` 绘制，`destroy()` 销毁。
- `Component.h / Component.cpp`：组件基类；`Init()`、`update()`、`draw()`、`destroy()` 定义生命周期，`setGame()` 设置上下文。`MouseComponent::update()` 发布鼠标点击事件。
- `EventBus.h / EventBus.cpp`：事件定义与同步分发；`Subscribe()` 订阅，`UnSubscribe()` 退订，`Publish()` 发布事件。

## 资源与音频：src/Manager

- `ResourceManager.h / ResourceManager.cpp`：按路径加载并缓存图片和声音；`Init()` 设置根目录，`GetImage()` / `GetTexture()` 获取图片纹理，`GetSound()` 获取声音，`Clear()` / `Shutdown()` 清空缓存并重置，`LastError()` 获取错误信息。
- `SoundResource.h / SoundResource.cpp`：定义声音资源及共享句柄 `SoundHandle`；析构函数负责释放声音。
- `AudioPlayer.h / AudioPlayer.cpp`：管理音效和背景音乐；`PlaySound()` / `StopSound()` 控制音效，`SetSoundVolume()` / `SetSoundPitch()` 调整音量和音调，`PlayMusic()` / `PauseMusic()` / `ResumeMusic()` / `StopMusic()` 控制音乐，`Update()` 每帧维护播放，`Shutdown()` 释放播放资源。
