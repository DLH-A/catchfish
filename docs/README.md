# catchfish

基于 C++17、raylib 6.0 和 Visual Studio 2022 的轻量 2D 游戏项目。说明文档统一存放于 `docs/`。

## 当前状态

已移除动画管理器及全部源码调用，资源管理器已移除 reanim 的类型、解析、缓存、扫描和专用路径规则。音频播放器通过独立 SoundResource 使用声音资源，不再包含 ResourceManager.h。详情见 [framework-review.md](framework-review.md)。

ResourceManager 已进一步简化为按路径加载、缓存和释放图片/声音，直接调用 raylib。已删除 XML 清单、分组、自定义 JPEG 解码、透明遮罩、资源名前缀和特殊目录映射，不再依赖 ImageDecoder 或 tinyxml2。

本轮按要求只做源码引用检查，没有执行构建或运行验证。

## 项目结构与文件职责

```text
catchfish/
├─ catchfish.sln                 VS 解决方案
├─ catchfish.vcxproj             C++17、raylib 链接、源码收集和输出路径
├─ catchfish.vcxproj.filters     VS 源文件/头文件显示分类
├─ catchfish.vcxproj.user        调试工作目录设置
├─ raylib.props                 团队 raylib 路径及头文件/链接库配置
├─ src/
│  ├─ core/
│  │  ├─ main.cpp               窗口/音频初始化、服务组装、游戏循环
│  │  ├─ GAME.h                 服务指针和游戏状态，仍有旧项目业务字段
│  │  ├─ World.h                组件所有权、服务注入、更新/绘制/销毁
│  │  ├─ Component.h/.cpp       组件生命周期接口和鼠标点击事件发布
│  │  └─ EventBus.h/.cpp        同步订阅、退订、事件发布及事件载荷
│  └─ Manager/
│     ├─ ResourceManager.h/.cpp 按路径加载图片/声音、缓存和释放
│     ├─ SoundResource.h/.cpp   独立声音资源、共享句柄和声音释放
│     └─ AudioPlayer.h/.cpp     多路音效、播放句柄、音量/音调和背景音乐流
├─ assets/                     游戏资源
└─ docs/                       项目说明和检查记录
```

`.vs/` 是 VS 缓存；`x64/`、`catchfish/x64/` 是旧构建目录；新构建输出位于 `build/`。

## 构建配置

团队成员只需编辑根目录的 `raylib.props`：在 `TeamRaylibPaths` 中复制示例配置，填写自己的 Windows 用户名及 raylib 根目录。工程会按当前 `USERNAME` 自动选择对应路径；当前成员 `29645` 使用 `C:\raylib`，其他成员的用户名和路径由本人填写，也可修改文件中的默认路径。用户名可在 PowerShell 中运行 `$env:USERNAME` 查看。

配置路径应包含 `include/raylib.h` 和 `lib/raylib.lib`，使用 MSVC x64 版本。包含目录、库目录、链接依赖和路径缺失提示均集中在 `raylib.props`，不用逐个修改 Debug/Release 配置。该文件已显示在 VS 项目中；修改后如 VS 提示重新加载工程，选择重新加载。

用 VS 2022 打开根目录的 `catchfish.sln`，选择 Debug x64 或 Release x64。本机 `C:\raylib` 的库为 x64，Win32 配置会被工程明确阻止。

如需自行构建，可在项目根目录的 VS 开发者命令行执行：

```powershell
MSBuild.exe catchfish.sln /t:Build /p:Configuration=Debug /p:Platform=x64
```

- raylib 默认根目录：`C:\raylib`，可用 `/p:RaylibDir=其他目录` 覆盖。
- 使用 C++17 和 UTF-8。
- 自动纳入 `src`、`third_party` 下的 C/C++ 源码与头文件，包括新增的 SoundResource.cpp。
- 包含目录：raylib 的 include、项目根目录、src、src/core、src/Manager。其他子目录可使用相对 src 的路径引用，或补充包含目录。
- 链接：raylib.lib、winmm.lib、gdi32.lib、opengl32.lib、user32.lib、shell32.lib。
- 程序输出：`build/bin/x64/配置名/catchfish.exe`；中间文件：`build/obj/x64/配置名/`。
- VS 调试工作目录为项目根目录，便于读取 assets。

## 资源使用

Init 只校验并记录资源根目录，不扫描目录，不预加载文件。GetImage/GetSound 接收包含扩展名的相对路径或绝对路径；相对路径从 Init 指定目录解析。

```cpp
resourceManager.Init("assets");
auto fish = resourceManager.GetImage("images/fish.png");
auto shot = resourceManager.GetSound("sounds/shot.wav");
if (fish)
    DrawTexture(fish->texture, 100, 100, WHITE); // 在 BeginDrawing/EndDrawing 之间
if (shot)
    audioPlayer.PlaySound(shot);
```

加载纹理前需要 InitWindow，加载声音前需要 InitAudioDevice。失败返回空句柄，可读取 LastError；支持的格式由当前 raylib 库决定。GetTexture 返回借用纹理，不应手动 UnloadTexture；需要跨 Clear 持有纹理时应保存 GetImage 返回的句柄。

Clear/Shutdown 清空缓存并重置根目录；外部共享句柄仍可持有资源。应在窗口、音频设备关闭前释放这些句柄及播放实例。

## 后续注意

当前入口只设置 assets 根目录，没有挂载场景组件。组件业务、旧游戏状态字段、事件生命周期和退出时的资源释放顺序仍可继续整理。
