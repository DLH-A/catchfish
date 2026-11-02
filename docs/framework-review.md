# 动画移除与音频解耦记录

更新日期：2026-11-02。

## 已完成

- 删除 src/core/AnimationManager.h 和 AnimationManager.cpp。
- 移除 main、Component、Game、World 中动画管理器的包含、前向声明、成员、初始化、更新和绘制调用。
- 删除 ResourceManager 中的 ReanimHandle、LoadReanimation、GetReanim、ListReanimNames、ReanimCount、PreloadAllUsedTextures 和旧双目录 Init 接口。
- 删除相应的动画数据缓存、文件扫描、名称处理和专用资源路径规则。
- 新增 src/Manager/SoundResource.h/.cpp，将 SoundResource、SoundHandle 和声音析构释放独立出来。
- AudioPlayer.h 仅通过 SoundResource.h 使用声音资源，不再包含 ResourceManager.h；补齐自身使用的标准库头文件。
- ResourceManager 复用同一份 SoundResource 类型，现有 GetSound → PlaySound 的调用方式保持兼容。
- ResourceManager 已删除 XML 资源清单/分组、自定义 JPEG 解码、透明遮罩合成、资源名前缀处理、文件名查找和旧目录映射；删除 ImageDecoder/tinyxml2 依赖和原来的精灵网格行列元数据。
- Init 仅校验并记录根目录；GetImage/GetSound 按完整文件路径调用 raylib 的 LoadTexture/LoadSound，使用规范化的绝对路径缓存。相对路径从根目录解析，不再自动补扩展名或按资源 ID 查找。

依赖方向为：AudioPlayer → SoundResource → raylib；ResourceManager → SoundResource → raylib。声音资源析构不再要求链接 ResourceManager 的实现。

## 音频保留的功能

PlaySound 用 LoadSoundAlias 实现同一源音效的多路播放，Voice 持有源共享句柄；通过递增 ID 停止、查询、调节音量和音调。默认最多 32 路音效，满额时淘汰最早保留的播放通道。

背景音乐采用 Music 流，支持暂停、恢复、停止和音量控制。Update 每帧回收结束的音效、重播循环音效并更新音乐流。音效循环仍是按帧重播，不能保证采样级无缝；音乐格式白名单仍为 OGG/WAV。

## 后续动画使用方式

当前不提供集中式动画管理器，也未新增替代播放器。

本机 raylib.h 提供 LoadImageAnim/LoadImageAnimFromMemory 加载图像帧序列，UpdateTexture 更新纹理，以及 DrawTextureRec/DrawTexturePro 绘制纹理区域。精灵图动画可以在具体组件中计时、选择源矩形并调用绘制接口；这些绘制接口本身不会自动推进播放帧。

## 框架当前定位

资源缓存、音频、组件容器和事件总线可以作为轻量 2D 游戏基础。移除专用动画后，基础模块不再依赖旧动画格式，但不代表所有通用化工作都已完成。

仍需关注：

- GAME.h 中服务指针与旧项目业务字段的分离。
- World 更新/绘制未检查 isActive，组件增删需避免在遍历中修改 vector。
- EventBus 回调中订阅/退订可能修改当前遍历的列表，组件销毁前应退订。
- 当前 main 在资源管理器析构前关闭窗口，且未调用 CloseAudioDevice。应在设备和窗口仍有效时先销毁组件、停止声音、释放所有资源句柄，再关闭设备和窗口。

## 检查范围

已通过文本搜索确认 src 和工程配置中没有残留的动画管理器、旧动画解析器或 SpritePlayer 引用。按用户要求，本轮没有执行构建、测试或运行验证；上述结论不表示工程已经编译通过。
