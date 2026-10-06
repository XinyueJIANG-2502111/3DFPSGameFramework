你正在继续接手一个 C++20 + DxLib 的 3D 第一人称恐怖游戏框架项目。

上一阶段已经完成 Volumetric Flashlight 的核心工作，包括 Scene Color / Scene Depth、Single-Scattering Raymarch、SpotLight Distance Attenuation 等，并已经得到正常结果。

现在开始一个新的独立阶段：

**Visual Effect Framework V1**

本阶段不是制作完整粒子引擎，而是建立一条结构正确、生命周期明确、能够继续扩展的基础 VFX 管线。

最终目标：

**在 TestScene 中按一次 Space，在 Camera 前方播放一次测试用 Burst / Hit Visual Effect。**

该 Effect 必须：

- 由 Engine 侧 Visual Effect 接口管理生命周期；
- 不直接依赖 DxLib；
- 通过 Renderer facade 绘制；
- 具有多个简单粒子；
- 粒子随时间移动并淡出；
- 生命周期结束后自动销毁；
- 正常接受场景深度测试；
- 不写入 SceneDepth；
- 按一次 Space 只触发一次，而不是每帧连续生成。

本阶段完成后，再继续 Texture Particle、Dust、Blood Mist、Decal 等更高级效果。

---

# 一、总体架构原则

继续遵守当前项目依赖：

Game
→ Engine
→ Platform Abstraction
→ DxLib

Visual Effect 不允许直接：

include DxLib.h

调用：

DrawBillboard3D
DrawPrimitive3D
SetDrawBlendMode
SetUseZBuffer3D
SetWriteZBuffer3D

这些 DxLib API 只能存在于：

Platform/DxLib

或现有 DxRenderer backend 内。

正确方向应为：

TestScene
    ↓
VisualEffectSystem
    ↓
IVisualEffect / ParticleBurstEffect
    ↓
Renderer
    ↓
IRendererBackend
    ↓
DxRenderer
    ↓
DxLib

不要创建新的全局 Singleton EffectManager。

不要建立新的反向依赖。

---

# 二、首先检查现有项目，不要直接写代码

开始修改前，请检查当前实际代码：

TestScene.h / .cpp

Renderer.h / .cpp

IRendererBackend 当前所在位置

DxRenderer.h / .cpp

Input abstraction

DxInput

Application / Scene render 顺序

ResourceSystem

当前 Camera 数据如何进入 Renderer

当前 Z Buffer / Blend State 使用方式

确认实际接口以后再实施。

如果 Prompt 中的函数名与项目当前 API 不完全一致，应保持项目现有命名风格，不要为了机械匹配 Prompt 而重复创建接口。

---

# 三、这一阶段明确不做的内容

当前不要实现：

完整 Particle Emitter System

持续喷射 Emitter

GPU Particle

Compute Shader

Texture Atlas

Animation Sheet

Decal

Blood Stain

Bullet Hole

Ribbon / Trail

Beam

Smoke Volume

Particle Collision

Particle 与 Volumetric Density 交互

Particle Lighting

Shadow Casting Particle

Effect Graph

Node Editor

EventBus

Object Pool

Multi-threaded Particle Update

复杂 Alpha Particle Sorting

本阶段只建立：

Visual Effect 生命周期
+
基础 Billboard Renderer
+
一次性 Particle Burst
+
TestScene Space Trigger

---

# 四、命名必须使用 VisualEffect / VFX

不要建立模糊的：

IEffect

因为项目以后可能还有：

StatusEffect
DamageEffect
AudioEffect
CameraEffect

本阶段视觉特效明确使用：

IVisualEffect

VisualEffectSystem

ParticleBurstEffect

避免以后语义冲突。

---

# 五、推荐目录结构

建议新增：

src/
└─ Engine/
   ├─ Effects/
   │  ├─ IVisualEffect.h
   │  ├─ VisualEffectSystem.h
   │  ├─ VisualEffectSystem.cpp
   │  └─ Particle/
   │     ├─ Particle.h
   │     ├─ ParticleBurstEffect.h
   │     └─ ParticleBurstEffect.cpp
   │
   └─ Rendering/
      └─ Billboard/
         └─ BillboardRenderData.h

如果现有 Rendering 目录结构有更合适的位置，可以小幅调整。

不要为了本阶段建立大量空目录和抽象层。

---

# 六、IVisualEffect

V1 接口保持最小。

推荐概念：

class IVisualEffect
{
public:
    virtual ~IVisualEffect() = default;

    virtual void Update(float deltaTime) = 0;

    virtual void Render(
        Renderer& renderer) const = 0;

    [[nodiscard]]
    virtual bool IsFinished() const = 0;
};

职责：

Update

推进 Effect 自身生命周期。

Render

只通过 Engine Renderer 提交绘制。

IsFinished

让 VisualEffectSystem 判断什么时候销毁 Effect。

IVisualEffect 不拥有 Renderer。

IVisualEffect 不知道 DxRenderer。

IVisualEffect 不知道 DxLib。

关键代码注释必须 English + Japanese。

---

# 七、VisualEffectSystem

VisualEffectSystem V1 只负责：

Ownership
Update
Render
Finished Effect Removal
Clear

推荐：

std::vector<
    std::unique_ptr<IVisualEffect>>

作为内部所有权。

推荐 API：

void Add(
    std::unique_ptr<IVisualEffect> effect);

void Update(
    float deltaTime);

void Render(
    Renderer& renderer) const;

void Clear();

不要第一版就做复杂 Factory / Registry / Reflection。

生命周期：

Create Effect
→ Add
→ Update
→ Render
→ IsFinished == true
→ erase

使用 erase_if 或当前项目已有的等价 C++20 写法均可。

---

# 八、VisualEffectSystem 的 Ownership

V1 直接由：

TestScene

持有：

VisualEffectSystem m_visualEffects;

这是本阶段最合适的生命周期。

Scene Enter
→ system 可以开始使用

Scene Update
→ system.Update(dt)

Scene Render
→ system.Render(renderer)

Scene Exit
→ system.Clear()
→ Scene 销毁

当前不要提升成：

Application-level global VFX service

也不要做 Singleton。

以后确实需要跨 Scene VFX 时再重新评估 ownership。

---

# 九、Renderer 不应该知道 Particle 行为

不要添加这种设计：

Renderer::UpdateParticles()

Renderer 不负责：

velocity
gravity
age
lifetime

这些属于 Effect 层。

Renderer 只负责：

“给我一组已经算好的 Billboard Render Data，我把它们画出来。”

因此建议增加一个纯渲染数据结构。

---

# 十、BillboardRenderData

推荐 V1：

struct BillboardRenderData
{
    Vector3 position{};

    float size = 1.0f;

    Vector3 color{
        1.0f,
        1.0f,
        1.0f
    };

    float alpha = 1.0f;
};

这可以避免为了 V1 特意引入新的 Vector4。

如果项目当前已经存在独立 Color 类型，应优先复用现有类型。

不要重复建立 Color abstraction。

当前不需要：

UV
Texture
Rotation
Animation Frame

这些下一阶段再增加。

---

# 十一、Blend Mode

V1 测试 Effect 推荐首先使用：

Additive

因为：

不需要处理多个透明粒子的严格 back-to-front sorting

适合：

Hit Spark
Energy Burst
Flash

建议建立 Engine 自己的轻量枚举，例如：

enum class BillboardBlendMode
{
    Alpha,
    Additive
};

如果项目已经存在通用 BlendMode abstraction，则必须复用现有 abstraction。

不要重复创建。

---

# 十二、Renderer 接口

Renderer facade 应增加一个非常小的 Billboard Drawing Path。

例如概念上：

void DrawBillboards(
    std::span<const BillboardRenderData> billboards,
    BillboardBlendMode blendMode);

IRendererBackend 对应增加：

virtual void DrawBillboards(
    std::span<const BillboardRenderData> billboards,
    BillboardBlendMode blendMode) = 0;

然后：

Renderer
→ IRendererBackend
→ DxRenderer

注意：

Effect Layer 不应该直接调用 DxRenderer。

---

# 十三、Billboard Rendering V1

本阶段可以采用最简单的 CPU-expanded billboard。

不要求 Instancing。

不要求 Geometry Shader。

不要求 GPU Particle。

DxRenderer 已经拥有 / 接收到 Camera：

Right
Up

因此 DxRenderer 可以利用：

cameraRight
cameraUp

把每个 Billboard 的中心：

P

扩展成 Camera-facing Quad。

概念上：

halfSize = size * 0.5

topLeft =
P
- Right * halfSize
+ Up * halfSize

topRight =
P
+ Right * halfSize
+ Up * halfSize

bottomLeft =
P
- Right * halfSize
- Up * halfSize

bottomRight =
P
+ Right * halfSize
- Up * halfSize

再组成两个 Triangle。

当前 Sample 数量非常少，因此 CPU expansion 是完全可以接受的 V1。

不要提前优化成 GPU instancing。

---

# 十四、Billboard Render State

Particle / Billboard 应当：

Depth Test = ON

Depth Write = OFF

这是非常重要的。

原因：

Particle 应该被：

Wall
Floor
Model

遮挡。

但是 Particle 本身不应该像 Opaque Geometry 那样修改深度并遮挡后续 transparent object。

Blend：

Test Burst 第一版使用 Additive。

Draw 完以后必须恢复之前的 Renderer State。

尤其恢复：

Blend Mode

Depth Write

必要的 Shader / Texture Binding

不要让 Billboard Render State 泄漏到后面的：

Volumetric
UI
其他 Model Draw

---

# 十五、Visual Effect 与 SceneDepth 的关系

本阶段 VFX：

只进入 Scene Color。

不要进入：

SceneDepth Pass。

也就是说：

TestScene::Render()

应该绘制：

Opaque Models
→ Visual Effects

而：

TestScene::RenderDepth()

仍然只绘制：

Opaque Scene Geometry

不要调用：

m_visualEffects.Render(...)

进入 SceneDepth。

原因：

Spark / Dust / Blood Mist 等透明粒子不应该被当作墙壁一样截断 Volumetric Ray。

---

# 十六、与当前 Volumetric Pass 的 Render Order

请检查当前 Application / SceneManager / Renderer 的实际 Pass 顺序，再做最小接入。

目标概念上应满足：

Opaque Scene Color
→ Transparent World VFX
→ Volumetric Fullscreen Composition
→ UI

SceneDepth 仍然由现有独立 depth pass 生成。

具体函数顺序必须以当前项目真实代码为准。

不要为了符合这段文字而破坏已经正常工作的 Volumetric Pipeline。

核心约束只有：

VFX 写入 Scene Color

VFX 不写入 Linear SceneDepth

Volumetric 最终仍能正常 composition

---

# 十七、Particle V1 数据

新增：

Particle

第一版只需要：

struct Particle
{
    Vector3 position{};
    Vector3 velocity{};

    float age = 0.0f;
    float lifetime = 1.0f;

    float startSize = 1.0f;
    float endSize = 0.0f;
};

如果需要颜色：

可以先让整个 ParticleBurstEffect 共享：

startColor
endColor

不要为了第一个测试 Effect 给每个 Particle 塞大量属性。

---

# 十八、Particle 更新逻辑

每帧：

age += deltaTime

normalizedAge = saturate(
    age / lifetime)

position +=
    velocity * deltaTime

size =
lerp(
    startSize,
    endSize,
    normalizedAge)

alpha =
1 - normalizedAge

V1 可以增加一个非常简单的 Gravity：

velocity.y -=
    gravity * deltaTime

但 Gravity 如果不是完成测试所必需，可以暂时不加。

优先保证：

Spawn
Move
Fade
Die

这四件事正确。

---

# 十九、ParticleBurstEffect

实现：

ParticleBurstEffect
    : public IVisualEffect

它内部拥有：

std::vector<Particle>

以及该 Effect 的共享视觉参数。

推荐 constructor 接收一个简单 Description：

ParticleBurstDesc

例如概念上包含：

position

direction

particleCount

speedMin
speedMax

lifetimeMin
lifetimeMax

startSize
endSize

color

spread

不要第一次就加入几十个参数。

如果项目当前没有随机工具，可以使用标准 C++ `<random>` 做简单 V1。

不要为了这个阶段建立完整 RandomService。

如果希望测试稳定，也可以使用固定 seed。

---

# 二十、第一版 Effect 类型

最终测试先不要做 Blood。

推荐：

Additive Hit Burst / Spark Burst

原因：

Additive rendering 简单。

不依赖 Texture。

不需要透明粒子排序。

容易判断：

Spawn 是否正常
Velocity 是否正常
Lifetime 是否正常
Depth Test 是否正常
Camera Billboard 是否正常

视觉可以是：

10～20 个小 Billboard

从一个点向外 Burst

生命周期：

约 0.3～0.8 秒

颜色：

偏暖的白 / 黄 / 橙均可

不要追求最终美术效果。

当前测试的目标是验证 Framework。

---

# 二十一、关于 Texture

本阶段不要因为 Hit Spark 就立刻建立完整：

Texture
TextureLoader
ITextureResource
DxTextureResource

如果当前 Renderer 已经有通用 Texture abstraction，可以使用。

如果没有：

V1 允许使用纯色、无纹理 Billboard。

这意味着它可能看起来像小发光 Quad。

这是可以接受的。

下一阶段再正式建立：

Texture Resource

并让：

Spark
Dust
Blood Mist

使用真正粒子纹理。

不要为了让测试更好看而扩大当前 Scope。

---

# 二十二、TestScene 接入

TestScene 增加：

VisualEffectSystem m_visualEffects;

在：

Update(float deltaTime)

中：

m_visualEffects.Update(
    deltaTime);

在：

Render()

中：

Opaque Model Draw 之后调用：

m_visualEffects.Render(
    m_renderer);

不要在：

RenderDepth()

中调用。

---

# 二十三、Space Trigger

最终验收要求：

按一次 Space
→ Spawn 一次 ParticleBurstEffect

必须首先检查项目当前 Input abstraction。

不要直接：

CheckHitKey(KEY_INPUT_SPACE)

不要从 TestScene include DxLib。

优先使用现有 Engine Input API。

必须使用：

Pressed / Triggered / WentDown

这种“本帧刚按下”的语义。

不能使用：

IsDown(Space)

然后每帧 Spawn。

如果当前 Input abstraction 已经支持：

Pressed

直接使用。

如果只有：

Down / Held

则做最小、正确的 Engine Input abstraction 扩展，增加 edge-trigger semantics。

不要只在 TestScene 偷偷调用 DxLib。

---

# 二十四、测试 Effect Spawn Position

按下 Space 时，TestScene 在 Camera 前方生成 Effect。

建议：

spawnPosition =
cameraPosition
+
cameraForward * 3.0f

这样 Effect 基本肯定能出现在视野中。

如果高度不合适，可以：

cameraPosition
+
cameraForward * 3.0f
+
Vector3{
    0.0f,
    -0.3f,
    0.0f
}

但先从纯 forward * 3 开始。

Burst Direction 可以先大致使用：

-cameraForward

或：

Vector3{
    0.0f,
    1.0f,
    0.0f
}

结合随机 spread。

V1 只需视觉明显。

不要现在实现真正的 weapon hit raycast。

---

# 二十五、TestScene 目标行为

运行 TestScene：

不按键：

没有 VFX。

按一次 Space：

Camera 前方生成一个 Burst。

Burst：

立即出现

多个 Particle 向外移动

尺寸 / Alpha 随 Lifetime 变化

大约不到 1 秒自然消失

然后：

VisualEffectSystem 自动删除 Effect。

连续按 Space：

每次可以独立创建新的 Burst。

多个 Burst 可以同时存在。

不应该 Crash。

不应该出现 dangling pointer。

Scene Exit 后：

所有 Effect 被清理。

---

# 二十六、Depth Test 验证

除了 Camera 前方 Spawn，还要测试墙壁。

可以走到：

Back Wall

附近。

让 Burst Position 位于：

墙后

或让部分 Particle 飞向墙后方。

至少确认：

被 Wall 遮挡的 Billboard 不应该永远透墙显示。

如果全部 Particle 无视墙壁始终显示：

说明 Billboard Draw 的 Depth Test State 不正确。

不要通过手工 CPU raycast 来隐藏 Particle。

应该让 GPU depth test 工作。

---

# 二十七、Blend State 验证

测试 Burst 使用 Additive。

Effect 消失后：

普通 Model Rendering

Volumetric

UI

不能被残留 Additive Blend 影响。

如果后面的 Scene 变亮或 Blend 异常：

说明 DxRenderer 没有正确恢复 Render State。

这是 Major Bug。

必须修复。

---

# 二十八、当前不做真正碰撞

Particle 当前可以穿过：

Floor
Wall
Model

只要 GPU 渲染时正确被深度遮挡即可。

不要本阶段实现：

Particle-vs-World Collision

Bounce

Collision Spawn

Decal Spawn

以后 Blood / Debris 阶段再做。

---

# 二十九、接口粒度要求

不要创建：

EffectManager
ParticleManager
BillboardManager
VFXManager
EffectFactory
EffectRegistry

一堆 Manager。

V1 应控制在：

IVisualEffect

VisualEffectSystem

ParticleBurstEffect

BillboardRenderData

Renderer Billboard API

DxRenderer implementation

足够。

---

# 三十、生命周期与资源要求

VisualEffectSystem：

owns IVisualEffect

ParticleBurstEffect：

owns Particle vector

Renderer：

does not own Effects

DxRenderer：

does not own Particle lifetime

TestScene：

owns VisualEffectSystem

Scene Exit：

Effect lifecycle 结束

这几个 ownership 必须清楚。

---

# 三十一、C++ 安全要求

使用：

std::unique_ptr

std::vector

std::span

RAII

不要：

new / delete 裸管理

不要保存：

Renderer*

到每个 Effect 内部

Effect 的：

Render(Renderer&)

只接收临时 non-owning reference。

不要使用 Global Renderer。

---

# 三十二、代码注释要求

所有关键新增代码注释必须：

English
+
Japanese

注释必须解释：

为什么 Effect 不直接访问 DxLib

为什么 Billboard：

Depth Test ON
Depth Write OFF

为什么 SceneDepth 不绘制 transparent VFX

为什么 Effect 由 Scene-owned system 管理

为什么使用 Additive 作为第一版测试

不要只是逐行翻译代码。

---

# 三十三、实施顺序

必须严格小步执行。

Step 1：

建立：

BillboardRenderData

以及 Renderer / Backend 的 Billboard Drawing API。

编译。

先确认不影响现有 Model / Volumetric。

Step 2：

实现 DxRenderer Billboard Draw。

用临时单个 Billboard 测试：

Camera-facing
颜色
Depth Test
Additive Blend
State Restore

编译运行。

不要此时就加入 EffectSystem。

Step 3：

建立：

IVisualEffect
VisualEffectSystem

先保证：

空 System
Update
Render
Clear

正常编译。

Step 4：

建立：

Particle
ParticleBurstEffect

实现：

Update
Render
IsFinished

编译。

Step 5：

TestScene 持有：

VisualEffectSystem

接入：

Update
Render
OnExit

编译运行。

Step 6：

检查 Input abstraction。

接入：

Space one-shot trigger。

Step 7：

Space 按下时：

Camera 前方 Spawn ParticleBurstEffect。

Step 8：

验证：

Repeated Spawn
Lifetime Cleanup
Depth Test
Blend Restore
Scene Exit

不要一次把所有文件全部写完后再第一次编译。

---

# 三十四、本阶段最终验收

本阶段结束必须满足：

程序正常启动。

Volumetric Lighting 仍正常。

SceneDepth 仍正常。

TestScene 内：

按一次 Space

只生成一次 Effect。

Effect 位于 Camera 前方。

Effect 至少包含多个独立 Particle。

Particle 随时间移动。

Particle 随生命周期淡出或缩小。

Effect 生命周期结束后自动销毁。

连续按 Space 可以生成多个 Effect。

Particle Billboard 面向 Camera。

Particle 接受正常 Depth Test。

Particle 不写入 Linear SceneDepth。

Effect 不直接调用 DxLib。

Effect 不知道 DxRenderer。

Renderer 不负责 Effect Lifetime。

Scene Exit 后 Effect 正确清理。

Draw Effect 后 Blend / Depth State 正确恢复。

无内存泄漏。

无 dangling pointer。

---

# 三十五、本阶段完成后的下一阶段

本阶段通过以后，下一阶段才开始：

Texture Resource abstraction

然后让 Billboard 支持：

Particle Texture

再逐步做：

Spark Texture
Dust Hit
Blood Mist

之后：

Decal System

实现：

Blood Splatter
Bullet Hole
Impact Mark

不要在本阶段提前实现这些内容。

---

# 三十六、最终设计原则

这一阶段真正需要验证的不是：

“粒子看起来够不够漂亮”

而是：

Game 能否只表达：

“这里播放一个 Visual Effect”

Effect 能否自己管理：

行为与 Lifetime

Renderer 能否只负责：

绘制数据

DxRenderer 能否只负责：

DxLib backend implementation

Scene 能否清楚拥有：

EffectSystem

如果最终 TestScene 中：

Space
→ Spawn
→ Update
→ Render
→ Fade
→ Finish
→ Auto Remove

这条链稳定成立，

那么 Visual Effect Framework V1 就完成。

# 其他注意事项
- 在本阶段工作中请称呼我为*咪咪*
- 不要频繁地结束工作然后把半成品丢给我调试。