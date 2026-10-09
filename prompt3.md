你正在继续修改一个 C++20 + DxLib 的 3D 第一人称恐怖游戏框架。

当前项目已经具备：

- Renderer facade
- IRendererBackend
- DxRenderer
- Model / Shader Resource abstraction
- ResourceSystem
- Scene Color Pass
- Linear Scene Depth Pass
- Volumetric Lighting Pass
- VisualEffectSystem
- IVisualEffect
- ParticleBurstEffect
- BillboardRenderData
- Renderer::DrawBillboards(...)
- DxLib billboard backend
- TestScene 中可触发测试 Burst
- Transparent VFX 不进入 SceneDepth
- Billboard Depth Test ON / Depth Write OFF
- Billboard Render State Restore

本阶段目标是：

**把当前 Effect V1 从“测试用纯色 Additive Billboard”升级为真正可用于项目的基础 VFX Framework。**

最终必须在 TestScene 中可以独立测试：

1. 金属打击火花
2. 血液飞溅
3. 低 HP 时屏幕四周周期性明暗的红色边框

不要同时实现：

- Decal
- Blood Stain
- Bullet Hole
- Trail System
- Ribbon
- GPU Particle
- Compute Shader
- Particle Collision
- Volumetric Smoke
- Shadowed Particle
- PostProcess Graph
- ECS 重构
- EventBus 重构

本阶段严格按以下顺序执行：

A. Texture Resource
B. Billboard / Particle Production Upgrade
C. Metal Spark
D. Blood Spray
E. Screen Overlay Pass + Low Health Effect

每个阶段都必须：

修改
→ 编译
→ 测试
→ 确认正常
→ 再进入下一阶段

不要一次把全部代码写完。

---

# 一、总体依赖方向

必须继续保持：

Game
→ Engine
→ Platform Abstraction
→ DxLib

禁止：

ParticleBurstEffect
直接 include DxLib.h

禁止：

VisualEffectSystem
调用 DrawPrimitive3D / LoadGraph / SetDrawBlendMode

禁止：

Game Effect Recipe
知道 DxRenderer

正确方向：

Game Effect Recipe
→ ParticleBurstEffect
→ Renderer
→ IRendererBackend
→ DxRenderer
→ DxLib

Screen Effect 同理。

---

# 二、先检查当前代码

开始修改前，先检查当前实际代码：

src/Engine/Effects/IVisualEffect.h
src/Engine/Effects/VisualEffectSystem.h/.cpp
src/Engine/Effects/Particle/Particle.h
src/Engine/Effects/Particle/ParticleBurstEffect.h/.cpp
src/Engine/Rendering/Billboard/BillboardRenderData.h
src/Engine/Rendering/Renderer.h/.cpp
src/Platform/DxLib/DxRenderer.h/.cpp
src/Platform/DxLib/DxBillboardRenderer.*
src/Engine/Resources/ResourceSystem.h/.cpp
src/Game/Scenes/TestScene.h/.cpp
src/Engine/Scene/*
src/Engine/Core/Application.cpp

如果当前项目文件名稍有不同，请使用实际已有结构。

不要机械创建重复模块。

---

# ============================================================
# 阶段 A：Texture Resource
# ============================================================

# 三、目标

当前 Effect 没有正式 Texture Resource。

不要在：

ParticleBurstEffect

里直接：

LoadGraph()

必须沿用 Model / Shader 已有的资源设计。

建议新增：

src/Engine/Rendering/Texture/
├─ ITextureResource.h
├─ Texture.h
├─ Texture.cpp
├─ TextureImpl.h
├─ TextureLoader.h
├─ TextureLoader.cpp
├─ TextureResourceAccess.h
└─ TextureResourceAccess.cpp

src/Platform/DxLib/
├─ DxTextureResource.h
└─ DxTextureResource.cpp

如果 Model / Shader 当前 PImpl / ResourceAccess 写法已经稳定，请尽量保持一致。

---

# 四、ITextureResource

建立最小 backend-independent interface：

```cpp
class ITextureResource
{
public:
    virtual ~ITextureResource() = default;
};
```

不要把 DxLib handle 暴露出来。

---

# 五、DxTextureResource

DxTextureResource：

- implements ITextureResource
- owns one DxLib graph handle
- LoadGraph()
- destructor DeleteGraph()
- no raw ownership leak
- movable if current Model / Shader resource also movable

失败时必须保持 InvalidHandle。

关键注释 English + Japanese。

---

# 六、Texture facade

Texture 应与当前 Model / Shader 风格一致。

目标：

```cpp
class Texture
{
public:
    [[nodiscard]]
    bool IsValid() const;

private:
    ...
};
```

不要让 Game 层能取得 DxLib handle。

---

# 七、TextureLoader

TextureLoader 应由：

IRendererBackend&

创建 backend resource。

概念上：

```cpp
std::shared_ptr<Texture> Load(
    const std::string& filePath);
```

不要使用 static global loader。

---

# 八、IRendererBackend 增加资源创建接口

概念：

```cpp
virtual std::unique_ptr<ITextureResource>
CreateTextureResource(
    const std::string& filePath) = 0;
```

保持和 Model / Shader 当前模式一致。

DxRenderer 实现：

```cpp
return std::make_unique<DxTextureResource>(
    filePath);
```

具体签名请按项目现有风格调整。

---

# 九、ResourceSystem

新增：

TextureLoader

和 weak cache。

目标 API：

```cpp
std::shared_ptr<Texture> LoadTexture(
    const std::string& filePath);
```

同一路径重复 Load：

应该复用已有 shared resource。

不要重复 LoadGraph。

---

# 十、阶段 A 验收

此阶段先不改 Particle。

只做一个临时 Texture Load 测试。

TestScene OnEnter：

Load 一个测试 PNG。

确认：

Texture valid

重复 Load 得到共享资源

Scene Exit 后无泄漏

Shutdown 正常

阶段 A 完成后再进入 B。

---

# ============================================================
# 阶段 B：Billboard / Particle Production Upgrade
# ============================================================

# 十一、BillboardRenderData 升级

当前如果还是：

```cpp
position
size
color
alpha
```

升级成：

```cpp
struct BillboardRenderData
{
    Vector3 position{};

    float width = 1.0f;
    float height = 1.0f;

    float rotationRadians = 0.0f;

    Vector3 color{
        1.0f,
        1.0f,
        1.0f
    };

    float alpha = 1.0f;
};
```

不要每个 Billboard 持有 Texture。

Texture 属于一个 batch。

---

# 十二、新增 BillboardDrawSettings

建议新增：

```cpp
struct BillboardDrawSettings
{
    BillboardBlendMode blendMode =
        BillboardBlendMode::Alpha;

    const Texture* texture =
        nullptr;
};
```

如果项目 prefer shared_ptr in render submission，优先参考 ModelInstance 的资源引用方式。

但不要让：

BillboardRenderData

每个粒子都复制 shared_ptr<Texture>。

---

# 十三、Renderer API 修改

从当前：

```cpp
DrawBillboards(
    span,
    blendMode)
```

升级为：

```cpp
DrawBillboards(
    std::span<const BillboardRenderData> billboards,
    const BillboardDrawSettings& settings);
```

同步修改：

Renderer
IRendererBackend
DxRenderer
Recording/Test Backend

所有实现必须一致。

---

# 十四、Billboard texture binding

Dx backend：

如果：

settings.texture != nullptr

通过受控的 TextureResourceAccess 获取 DxTextureResource。

不要 dynamic_cast Game object。

Texture backend cast 只允许发生在 Platform/DxLib 边界。

DrawPrimitive3D 的 texture handle 使用：

DxTextureResource 的 graph handle。

没有 texture 时：

允许 handle = -1 / InvalidHandle

作为纯色 fallback。

---

# 十五、Billboard rotation

当前 Camera Facing basis：

cameraRight
cameraUp

加入：

rotationRadians

二维旋转：

```text
rotatedRight =
Right * cos(r)
+
Up * sin(r)

rotatedUp =
-Up * sin(r)
+
Up * cos(r)
```

请注意第二个公式必须数学正确。

标准 2D basis rotation 建议使用：

```cpp
rotatedRight =
    cameraRight * cosR +
    cameraUp * sinR;

rotatedUp =
    cameraUp * cosR -
    cameraRight * sinR;
```

然后：

```text
halfWidth
halfHeight
```

构造 quad。

---

# 十六、Billboard Batch Draw

当前如果：

for each billboard
→ DrawPrimitive3D()

必须改成：

一次 build 全部 vertices。

例如：

```cpp
std::vector<VERTEX3D> vertices;

vertices.reserve(
    billboards.size() * 6);
```

每个 billboard push 六个 triangle-list vertex。

最后一次：

```cpp
DrawPrimitive3D(
    vertices.data(),
    static_cast<int>(vertices.size()),
    DX_PRIMTYPE_TRIANGLELIST,
    textureHandle,
    TRUE);
```

目标：

一个 DrawBillboards call
≈ 一个 DxLib draw call

不要一个粒子一个 draw call。

---

# 十七、Render State

Billboard 继续：

Depth Test = ON
Depth Write = OFF

Alpha：

DX_BLENDMODE_ALPHA

Additive：

DX_BLENDMODE_ADD

Draw 完必须恢复：

Depth Test
Depth Write
Lighting
Blend
Cull
Fog
Texture / Shader state if modified

不能泄漏。

---

# 十八、Particle 数据升级

Particle 最少增加：

```cpp
Vector3 acceleration{};

float rotationRadians = 0.0f;
float angularVelocity = 0.0f;
```

并把：

startSize
endSize

如果需要非正方形粒子，建议改成：

```cpp
float startWidth;
float endWidth;

float startHeight;
float endHeight;
```

如果当前代码改动太大，也可以第一步保留 size，
但 Metal Spark 阶段前必须支持 width/height。

---

# 十九、Particle Update

每帧：

```cpp
age += dt;

velocity +=
    acceleration * dt;

position +=
    velocity * dt;

rotationRadians +=
    angularVelocity * dt;
```

normalizedAge：

```cpp
t =
    Clamp(
        age / lifetime,
        0,
        1);
```

width/height：

lerp start→end

alpha：

```cpp
1 - t
```

V1 继续线性即可。

不要现在做 curve asset。

---

# 二十、ParticleBurstDesc 增加配置

至少增加：

```cpp
BillboardBlendMode blendMode;

std::shared_ptr<Texture> texture;

Vector3 acceleration;

float startWidth;
float endWidth;

float startHeight;
float endHeight;

float angularVelocityMin;
float angularVelocityMax;

std::uint32_t randomSeed;
```

保持已有：

position
direction
particleCount
speedMin/max
lifetimeMin/max
spread
color

如果 color 当前只有单值，可以先继续单值。

不要一次加入几十个参数。

---

# 二十一、修正固定 RNG

如果当前：

```cpp
std::mt19937 random(27);
```

改成：

```cpp
std::mt19937 random(
    desc.randomSeed);
```

不要创建 RandomManager。

---

# 二十二、ParticleBurstEffect Render

不要再写死：

```cpp
BillboardBlendMode::Additive
```

改成使用 desc 保存的：

blendMode

texture

构造：

```cpp
BillboardDrawSettings settings;
settings.blendMode = m_blendMode;
settings.texture = m_texture.get();
```

然后：

```cpp
renderer.DrawBillboards(
    m_billboards,
    settings);
```

---

# 二十三、阶段 B 验收

TestScene 临时仍用 Space Trigger。

测试：

Texture alpha 正常

Billboard rotation 正常

width != height 正常

Alpha / Additive 都正常

Depth Test 正常

Depth Write OFF

多个 particle 只产生一个 batch draw

Effect 自动销毁

阶段 B 正常后进入 C。

---

# ============================================================
# 阶段 C：Metal Spark
# ============================================================

# 二十四、不要新建 SparkEffect 类

不要：

```cpp
class MetalSparkEffect
```

复制 ParticleBurstEffect。

Metal Spark 应当只是：

ParticleBurstDesc preset / recipe。

建议 Game 层新增：

```text
src/Game/Effects/
├─ GameEffectRecipes.h
└─ GameEffectRecipes.cpp
```

---

# 二十五、Metal Spark Recipe

提供类似：

```cpp
ParticleBurstDesc MakeMetalSparkBurst(
    const Vector3& position,
    const Vector3& normal,
    std::shared_ptr<Texture> texture,
    std::uint32_t seed);
```

实际签名按当前 style 调整。

建议初始参数：

```text
particleCount:
12～24

blendMode:
Additive

speed:
3.0～8.0

lifetime:
0.10～0.40

acceleration:
Y = -3 ～ -6

startWidth:
0.01～0.03

startHeight:
0.08～0.20

endWidth:
接近 0

endHeight:
接近 0

color:
white/yellow/orange

spread:
围绕 hit normal
```

不要把这些值硬编码在 ParticleBurstEffect 类里。

它们属于 Game recipe。

---

# 二十六、Spark texture

使用一张简单：

白色 / 黄白细长 streak
透明背景 PNG

如果当前没有素材：

允许临时创建简单测试 texture。

但不要把 texture 二进制生成逻辑放到运行时代码。

---

# 二十七、TestScene Trigger

临时可以继续用 Space 或另一个测试键。

例如：

Space
→ Metal Spark

Spawn 位置：

cameraPosition +
cameraForward * 3

normal：

-cameraForward

或临时固定法线。

当前只是验证。

不要现在做 weapon raycast。

---

# 二十八、Metal Spark 验收

必须看到：

细长而不是正方形

Additive

短生命周期

向外飞散

有 gravity

可被墙遮挡

不会写 SceneDepth

连续触发多个 Burst 正常

---

# ============================================================
# 阶段 D：Blood Spray
# ============================================================

# 二十九、同样不要新建重复 Particle 类

Blood Spray 仍然使用：

ParticleBurstEffect

只改变 Recipe。

---

# 三十、Blood Recipe

新增：

```cpp
ParticleBurstDesc MakeBloodSprayBurst(
    const Vector3& position,
    const Vector3& direction,
    std::shared_ptr<Texture> texture,
    std::uint32_t seed);
```

建议初始参数：

```text
particleCount:
20～40

blend:
Alpha

speed:
1.5～5.0

lifetime:
0.35～1.0

acceleration:
Y ≈ -9.8

startWidth/Height:
0.04～0.12

end:
更小

rotation:
随机

angularVelocity:
小到中等

color:
dark red
```

---

# 三十一、Blood texture

使用：

不规则血滴 / 小片状 alpha PNG。

不要使用一个纯圆点。

第一版不做：

wall stain
floor stain
decal

Blood Spray 只表示：

Airborne Blood.

---

# 三十二、Alpha Blend

Blood 必须：

BillboardBlendMode::Alpha

不要 Additive。

如果出现：

血液像发光粒子

优先检查 blendMode 是否仍被写死 Additive。

---

# 三十三、TestScene

例如：

按 B
→ Blood Spray

或如果 Input abstraction 当前不方便增加键，
允许临时：

Space = Spark
另一个已有 key = Blood

但必须使用 edge-trigger。

不能每帧连续 spawn。

---

# 三十四、Blood 验收

血液：

受 gravity 下坠

Alpha blend

不会发光

粒子有随机 rotation

可被墙遮挡

多个 Burst 可同时存在

生命周期结束后自动删除

SceneDepth 不受影响

Volumetric 不受影响

---

# ============================================================
# 阶段 E：Screen Overlay Pass + Low HP Effect
# ============================================================

# 三十五、不要把 Low HP 红框塞进 VisualEffectSystem

VisualEffectSystem 当前适合：

短生命周期 World VFX

不要让 LowHealth 继承 IVisualEffect 强行复用。

Low HP 是：

state-driven persistent screen effect。

---

# 三十六、增加 Scene Overlay Render Hook

检查当前：

IScene
SceneManager
Application

增加：

```cpp
virtual void RenderOverlay()
{
}
```

SceneManager：

```cpp
void RenderOverlay();
```

内部：

```cpp
if (m_activeScene)
{
    m_activeScene->RenderOverlay();
}
```

Application render 顺序改成：

```text
Scene Color
Scene Depth
Volumetric Lighting
Scene Overlay
EndFrame
```

具体应类似：

```cpp
m_rendererBackend.RenderVolumetricLighting(
    width,
    height);

m_sceneManager.RenderOverlay();

m_platform.EndFrame();
```

Low HP Overlay 必须发生在 Volumetric 之后。

---

# 三十七、Renderer Screen Effect API

V1 不需要通用 PostProcess Graph。

新增具体 API 即可。

例如：

```cpp
struct LowHealthScreenEffectData
{
    float intensity = 0.0f;
};
```

Renderer：

```cpp
void DrawLowHealthOverlay(
    const LowHealthScreenEffectData& data);
```

IRendererBackend 同步。

DxRenderer 实现 fullscreen draw。

---

# 三十八、LowHealth shader

新增：

```text
Assets/Shaders/Source/LowHealthOverlayPS.hlsl
```

使用 fullscreen UV。

不依赖 SceneDepth。

不采样 SceneColor 也可以。

如果使用 Alpha Blend overlay：

shader 只输出：

Red + Alpha

然后 backend 用：

DX_BLENDMODE_ALPHA

覆盖在当前 BackBuffer 上。

这样比再次采 SceneColor 更简单。

---

# 三十九、LowHealth vignette 形状

建议：

```hlsl
float2 centered =
    abs(
        input.TexCoords0 * 2.0f -
        1.0f);

float edge =
    max(
        centered.x,
        centered.y);

float border =
    smoothstep(
        0.55f,
        1.0f,
        edge);
```

为了更自然，可再加：

```hlsl
float radial =
    length(
        input.TexCoords0 * 2.0f -
        1.0f);

border =
    max(
        border,
        smoothstep(
            0.65f,
            1.25f,
            radial));
```

不要这阶段加 noise。

---

# 四十、LowHealthScreenEffect 类

建议新增：

```text
Engine/Effects/Screen/
├─ LowHealthScreenEffect.h
└─ LowHealthScreenEffect.cpp
```

职责：

保存：

healthRatio
elapsedTime

Update(dt)

Render(Renderer&)

API 例如：

```cpp
void SetHealthRatio(
    float ratio);
```

---

# 四十一、HP threshold

V1：

```text
healthRatio >= 0.30
→ no overlay

healthRatio < 0.30
→ effect increases
```

severity：

```cpp
severity =
    Clamp(
        (0.30f - healthRatio) /
        0.30f,
        0,
        1);
```

---

# 四十二、Pulse

不要硬切。

例如：

```cpp
const float pulse =
    0.5f +
    0.5f *
    std::sin(
        m_elapsedTime *
        pulseSpeed);
```

pulseSpeed 可以随 severity 稍微增大。

例如：

```cpp
pulseSpeed =
    Lerp(
        2.5f,
        5.0f,
        severity);
```

final：

```cpp
intensity =
    severity *
    Lerp(
        0.35f,
        1.0f,
        pulse);
```

注意：

具体值只是初始 tuning。

不要把复杂生命系统引入 Effect 类。

---

# 四十三、TestScene 暂时模拟 HP

如果当前没有完整 Player HP system：

TestScene 可以临时保存：

```cpp
float m_testHealthRatio =
    1.0f;
```

测试按键：

例如：

H
→ 减少 healthRatio

J
→ 恢复

或者直接启动时：

0.2f

只要能验证 Overlay。

不要为了 LowHealth Effect 现在设计完整 HealthComponent。

---

# 四十四、RenderOverlay

TestScene：

```cpp
void TestScene::RenderOverlay()
{
    m_lowHealthEffect.Render(
        m_renderer);
}
```

Update：

```cpp
m_lowHealthEffect.SetHealthRatio(
    m_testHealthRatio);

m_lowHealthEffect.Update(
    deltaTime);
```

---

# 四十五、Overlay Render State

LowHealth fullscreen：

Depth Test = OFF

Depth Write = OFF

Blend = Alpha

Draw 后恢复 state。

不能影响 UI / 后续 frame。

---

# 四十六、Low HP 验收

health >= 30%

没有红框。

health < 30%

屏幕边缘开始红。

HP 越低：

强度更高。

红框：

周期性明暗。

中心区域：

尽量保持可见。

Volumetric：

不影响 LowHealth overlay。

Overlay：

在 Volumetric 之后绘制。

---

# ============================================================
# 最终架构边界
# ============================================================

# 四十七、World VFX

World VFX：

Metal Spark
Blood Spray

走：

```text
VisualEffectSystem
→ ParticleBurstEffect
→ Renderer::DrawBillboards
→ DxRenderer
```

---

# 四十八、Screen VFX

Low Health：

不要走：

VisualEffectSystem

而走：

```text
LowHealthScreenEffect
→ Renderer::DrawLowHealthOverlay
→ DxRenderer
→ BackBuffer
```

---

# 四十九、Game Recipe 边界

Engine 提供：

Particle
ParticleBurstEffect
Billboard Rendering
Texture
LowHealth Screen primitive

Game 提供：

Metal Spark 参数
Blood 参数
HP threshold

不要把恐怖游戏具体美术参数硬编码到 Engine。

---

# 五十、不要创建 Manager 海洋

不要增加：

ParticleManager
EffectManager
TextureManager
ScreenEffectManager
VFXManager
SparkManager
BloodManager

当前足够：

ResourceSystem
VisualEffectSystem

以及具体 effect / recipe。

---

# 五十一、代码注释要求

关键新增代码必须：

English
+
Japanese

说明：

Purpose
Ownership
Render-pass reason
Blend reason
Depth reason
Lifecycle

不要只翻译代码。

---

# 五十二、每阶段完成后的报告格式

每完成 A/B/C/D/E 一个阶段后：

停止继续修改。

向用户报告：

1. 修改文件
2. 新增文件
3. 删除 / 替换了哪些旧逻辑
4. 当前依赖方向
5. 编译结果
6. 运行结果
7. 已知限制
8. 下一阶段建议

等用户确认再继续。

---

# 五十三、最终验收标准

全部完成以后必须满足：

Texture Resource：

- 不暴露 DxLib handle
- RAII 正常
- ResourceSystem cache 正常

Billboard：

- Texture
- Alpha
- Additive
- width/height
- rotation
- batch draw
- depth test
- depth write off
- state restore

Particle：

- velocity
- acceleration
- lifetime
- width/height interpolation
- alpha fade
- rotation
- seeded random

Metal Spark：

- Additive
- 细长
- 短生命周期
- 重力
- 可遮挡

Blood Spray：

- Alpha
- 血液 texture
- 重力明显
- 不发光
- 可遮挡

Low Health：

- Volumetric 后绘制
- 红色边框
- 低 HP 才启用
- 周期明暗
- 中心区域可见

架构：

- Game 不直接依赖 DxLib
- Effects 不直接依赖 DxRenderer
- Renderer 不管理 Effect Lifetime
- VisualEffectSystem 不管理 Screen Effect
- Transparent VFX 不进入 SceneDepth

如果全部成立，本阶段可以视为：

**VFX Production Foundation V1 完成。**