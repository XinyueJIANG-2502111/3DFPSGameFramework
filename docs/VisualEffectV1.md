# Visual Effect Framework V1

TestScene 按下 Space 时，在当前 Camera 前方 3 米创建一个 16 粒子的暖色 Burst。按住不重复生成；松开后再次按下可创建独立 Burst。粒子移动、缩小、淡出，寿命为 0.3～0.8 秒。

## 生命周期与绘制

`TestScene → VisualEffectSystem → IVisualEffect / ParticleBurstEffect → Renderer → IRendererBackend → DxRenderer → DxLib`

- Scene 拥有 System；System 用 unique_ptr 拥有 Effect，结束后自动删除，OnExit 清空。
- Effect 拥有粒子和已计算的 Billboard 数据，不保存 Renderer，不依赖 DxLib。
- Renderer 仅转发 span；DxRenderer 使用 Camera Right / Up 展开 Quad。
- Billboard 使用加算合成、GPU 深度测试，关闭深度写入。
- RAII 恢复 Blend、深度、Lighting、Fog 和 Culling。标准无纹理 Primitive 不改变自定义 Shader / Texture Binding。
- DxLib 没有深度和 Lighting Getter，因此 Backend 追踪这些状态，并在 Color Pass 开始时建立状态。后续扩展应继续通过 Backend 的状态方法修改它们。
- VFX 在 Opaque Scene Color 之后绘制，独立 Linear SceneDepth 只绘制不透明模型。Backend 在 Depth Pass 内也拒绝 Billboard 提交。
- 现有 `EndSceneDepthRender → DrawCameraRayDebug` 的 Fullscreen Volumetric Composition 保持原样；本阶段未修改 Shader。

## 构建与自动测试

使用 Visual Studio 2022 Developer PowerShell，或提供 MSBuild.exe 的绝对路径：

```powershell
MSBuild.exe DxLib.vcxproj /p:Configuration=Debug /p:Platform=x64 /m
MSBuild.exe tests/VisualEffectTests.vcxproj /p:Configuration=Debug /p:Platform=x64 /m
& ./x64/VFXTests/VisualEffectTests.exe
```

测试工程使用隔离的 DxLib 输入桩编译真实 DxInput.cpp，不链接 DxLib。覆盖移动、Alpha/Size 变化、多个粒子的独立运动、自动销毁、Clear、System 析构、多个 Burst 并存、结束后停止绘制，以及 Space 按住 120 帧只触发一次、松开再按触发第二次。

若运行环境同时存在 PATH 和 Path，旧版 MSBuild 会报重复环境变量键，可在当前构建会话中保留一个 Path 项后重试。

本次因已有进程占用默认 DxLib.exe，游戏构建使用 `/p:OutDir=x64/VFXValidation/`。最终验证程序位于 `x64/VFXValidation/DxLib.exe`，运行工作目录必须是项目根目录，否则 Assets 相对路径无法加载。

## 验证记录与范围

- 分步编译：Renderer API、Backend Quad、System、Burst、Scene 接入、Space Trigger 均通过 Debug x64 构建。
- 自动测试通过，包括真实 DxInput 的按键边沿语义。
- 实际运行中，临时单 Quad 在 Camera 前方正确显示，并进入体积光合成。
- 临时固定 Camera/粒子验证中，墙前的暖色和绿色 Quad 可见，墙后的红色 Quad 被 GPU 深度测试遮挡；模型与体积光仍正常。
- 所有临时探针、延长寿命和输入日志代码均已移除，正式版本不按键时没有 VFX。
- UI 自动化发送的短促 Space 未被实时轮询采样；随后最终版本检测到用户输入，实际画面中捕捉到多个分散的暖色粒子，体积光仍正常。之后用户按 Escape 停止了 Computer Use，没有继续截图；消失/生命周期和单次触发/保持/再按语义由自动测试验证。

不包含 Texture、粒子碰撞、排序、Emitter、Pool、Decal 或 GPU Particle。现有 TestScene 的 DxLib 调用仍仅用于原有 Grid/AABB 调试，新 VFX 路径完全经过 Renderer。
