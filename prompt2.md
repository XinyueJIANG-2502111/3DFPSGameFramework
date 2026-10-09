你正在继续修改一个 C++20 + DxLib 的 3D 第一人称恐怖游戏框架。

当前 Rendering 已经完成：

- Scene Color Pass
- Linear Scene Depth Pass
- 独立 Volumetric Lighting Pass
- Fullscreen volumetric raymarch
- SceneDepth clamp
- SpotLight surface / volumetric Range 统一为 Euclidean distance
- Surface / Volumetric distance attenuation 基本统一
- Single-scattering raymarch
- Henyey-Greenstein phase approximation
- VisualEffect Framework V1

现在开始下一小阶段：

# Flashlight Camera-Local Offset V1

当前主要视觉问题：

SpotLight Position 基本等于 Camera Position，
SpotLight Direction 基本等于 Camera Forward。

这会让 flashlight 看起来像：

“固定贴在屏幕中心的规整亮斑”

本阶段目标：

让 flashlight 在 Camera Local Space 中具有一个固定位置偏移，例如：

Right   * 0.25
Up      * -0.20
Forward * 0.15

但本阶段：

**SpotLight Direction 仍然保持 Camera Forward。**

不要同时实现：

- Focus point aiming
- Flashlight sway
- Head bob
- Cookie
- Shadow Map
- Texture profile
- Noise
- 新 Phase Function
- 新 Attenuation
- Renderer 大规模重构

一次只验证“Light Position 与 Camera Position 分离”这一变量。

---

# 一、开始前先检查当前实际代码

请先检查：

src/Game/Scenes/TestScene.h
src/Game/Scenes/TestScene.cpp

重点找到当前 flashlight 每帧同步 Camera 的代码。

预计现在类似：

```cpp
m_flashlight.position =
    m_camera.GetTransform().position;

m_flashlight.direction =
    m_camera.GetForward();
```

也检查当前：

```cpp
m_renderer.SetSpotLight(...)
```

或等价调用。

不要创建新的 Spotlight。

继续使用现有：

```cpp
m_flashlight
```

---

# 二、Camera 已经具有需要的 basis

当前 Camera 已经提供：

```cpp
m_camera.GetForward()
m_camera.GetRight()
m_camera.GetUp()
```

不要重新从 Quaternion 手工算 Right / Up。

不要在 TestScene 中使用 DxLib VECTOR。

全部使用 Engine：

```cpp
Vector3
```

---

# 三、先修改 TestScene 的 flashlight position

文件：

```text
src/Game/Scenes/TestScene.cpp
```

找到当前每帧更新 flashlight 的位置。

先取得 Camera basis：

```cpp
const Vector3 cameraPosition =
    m_camera.GetTransform().position;

const Vector3 cameraForward =
    m_camera.GetForward();

const Vector3 cameraRight =
    m_camera.GetRight();

const Vector3 cameraUp =
    m_camera.GetUp();
```

如果当前代码已经取得其中部分变量，请复用，不要重复声明。

---

然后加入三个 V1 offset 常量。

当前阶段允许直接在 `TestScene.cpp` 的 Update 附近使用 `constexpr`：

```cpp
// EN: Camera-local flashlight offset used by the first
//     non-head-mounted flashlight prototype.
//
// JP: Camera と完全に同位置ではない Flashlight を検証するための
//     Camera-Local Position Offset。
constexpr float kFlashlightOffsetRight =
    0.25f;

constexpr float kFlashlightOffsetUp =
    -0.20f;

constexpr float kFlashlightOffsetForward =
    0.15f;
```

如果 `TestScene.cpp` 已经有匿名 namespace 放常量，可以把它们放到那里。

不要为了三个 float 新建配置类。

---

将原来的：

```cpp
m_flashlight.position =
    cameraPosition;
```

替换成：

```cpp
// EN: Place the flashlight slightly to the side and below the camera
//     using the camera's local orthonormal basis.
//
//     The offset follows camera rotation automatically because it is
//     reconstructed from Right / Up / Forward every frame.
//
// JP: Camera の Local Orthonormal Basis を使用して、Flashlight を
//     Camera より少し横・下・前方へ配置する。
//
//     Right / Up / Forward から毎 Frame 再構築するため、
//     Camera Rotation に自然に追従する。
m_flashlight.position =
    cameraPosition +
    cameraRight *
        kFlashlightOffsetRight +
    cameraUp *
        kFlashlightOffsetUp +
    cameraForward *
        kFlashlightOffsetForward;
```

---

# 四、Direction はまだ Camera Forward のまま

本阶段不要做 focus target。

继续：

```cpp
// EN: V1 changes only the flashlight origin.
//     Keep the beam direction parallel to the camera forward axis
//     so position-offset behavior can be evaluated independently.
//
// JP: V1 では Flashlight Origin のみを変更する。
//     Position Offset の影響を単独で検証するため、Beam Direction は
//     Camera Forward と平行なまま維持する。
m_flashlight.direction =
    cameraForward;
```

不要改成：

```cpp
normalize(target - flashlightPosition)
```

那属于下一阶段的 aiming behavior。

---

# 五、保持 Renderer 上传逻辑不变

当前 SpotLight 数据已经包含：

```cpp
Position
Range
Direction
Enabled
Color
InnerCos
OuterCos
```

因此本阶段不应该新增 Constant Buffer 字段。

不要修改：

```text
ShaderSpotLightData
LightingBuffer b4
DxRenderer::SetSpotLight
UpdateLightingConstantBuffer
```

除非检查发现它们当前根本没有上传 Position。

如果 Position 已正常上传，只修改 TestScene 的值即可。

---

# 六、重要：当前 Volumetric shader 有一个旧 Apex 假设必须删除

文件：

```text
Assets/Shaders/Source/VolumetricLightingPS.hlsl
```

当前 shader 前半段大概率还存在：

```hlsl
const float rayConeCos =
    dot(
        worldRay,
        coneDirection);
```

以及：

```hlsl
const bool rayInsideCone =
    rayConeCos >=
    g_SpotLight.OuterCos;

const bool rayMovesForward =
    rayConeCos >
    0.0001f;
```

然后：

```hlsl
if (!spotlightEnabled ||
    !rayInsideCone ||
    !rayMovesForward)
{
    return sceneColor;
}
```

这个逻辑在：

```text
CameraPosition ≈ LightPosition
```

时成立。

但是 Light Position 现在与 Camera Position 分离以后：

```text
camera ray direction
```

是否与：

```text
light cone axis
```

夹角在 OuterCos 内，

**并不能判断这个 camera ray 后面是否会进入 spotlight cone。**

例如：

```text
Camera ray 起点在 cone 旁边
但 ray 后面穿过 cone
```

旧 early reject 会错误地把整个 pixel 丢掉。

因此必须删除这层 pixel-level cone reject。

---

# 七、具体删除 VolumetricLightingPS 里的哪些代码

保留：

```hlsl
const float3 coneDirection =
    normalize(
        g_SpotLight.Direction);
```

这个后面的 per-sample cone test 仍然需要。

删除：

```hlsl
const float rayConeCos =
    dot(
        worldRay,
        coneDirection);
```

如果后面已经没有其他用途，彻底删除变量。

---

保留：

```hlsl
const float coneFeatherRange =
    max(
        g_SpotLight.InnerCos -
            g_SpotLight.OuterCos,
        0.0001f);
```

因为 raymarch sample 内仍然使用它。

---

把下面整段：

```hlsl
const bool spotlightEnabled =
    g_SpotLight.Enabled >
    0.5f;

const bool rayInsideCone =
    rayConeCos >=
    g_SpotLight.OuterCos;

const bool rayMovesForward =
    rayConeCos >
    0.0001f;

if (!spotlightEnabled ||
    !rayInsideCone ||
    !rayMovesForward)
{
    return sceneColor;
}
```

修改成只检查：

```hlsl
// EN: Only the spotlight enabled state can be rejected before
//     ray marching once the camera and light origins are separated.
//
//     Cone membership must be evaluated per sample because a camera
//     ray may enter the light cone later even if its direction is not
//     initially aligned with the cone axis.
//
// JP: Camera Origin と Light Origin が分離した後、Raymarch 前に
//     安全に Early Reject できるのは SpotLight Enabled のみ。
//
//     Camera Ray は途中から Light Cone に入る可能性があるため、
//     Cone Membership は Sample 単位で評価する必要がある。
const bool spotlightEnabled =
    g_SpotLight.Enabled >
    0.5f;

if (!spotlightEnabled)
{
    return sceneColor;
}
```

---

# 八、Per-sample cone test は绝对不要删

Raymarch loop 内应该已经有类似：

```hlsl
const float3 lightToSample =
    samplePosition -
    g_SpotLight.Position;

const float sampleLightDistance =
    length(
        lightToSample);

const float3 sampleLightDirection =
    lightToSample /
    sampleLightDistance;

const float sampleConeCos =
    dot(
        sampleLightDirection,
        coneDirection);

if (sampleConeCos <
    g_SpotLight.OuterCos)
{
    continue;
}
```

这一段必须保留。

这是 offset 后真正正确的 cone membership：

```text
Light Position
→ Sample Position
```

而不是：

```text
Camera ray direction
→ Light Direction
```

---

# 九、Range Sphere intersection 继续保留

当前 volumetric 已经把 SpotLight Range 定义为：

```text
Light Origin 出发的最大 Euclidean distance
```

并使用 Ray / Sphere intersection 得到：

```hlsl
rangeExitDistance
```

这一段不要改回 cone end plane。

当前：

```hlsl
lightToRayOrigin =
    CameraPosition -
    SpotLightPosition;
```

正是为了支持：

```text
CameraPosition != SpotLightPosition
```

所以 flashlight offset 后，这部分反而开始真正发挥作用。

确认保持：

```hlsl
const float3 lightToRayOrigin =
    g_Camera.Position -
    g_SpotLight.Position;
```

以及：

```hlsl
rangeExitDistance
```

计算。

---

# 十、检查这个 early range 条件

当前可能存在：

```hlsl
if (rayOriginDistanceSquared >=
    rangeSquared)
{
    return sceneColor;
}
```

本阶段 offset 只有约：

```text
sqrt(
    0.25² +
    0.20² +
    0.15²
)
≈ 0.35
```

而 flashlight Range 通常约：

```text
15
```

所以 Camera 仍然稳定处于 Range Sphere 内。

当前可以继续保留这个判断。

不要本阶段扩展到：

Camera outside range sphere

的一般 ray/sphere entry/exit 情况。

---

# 十一、SceneDepth clamp 不改

继续：

```hlsl
const float raySegmentEnd =
    hasSceneSurface
    ? min(
        rangeExitDistance,
        sceneRayDistance)
    : rangeExitDistance;
```

不要重新引入：

```hlsl
coneExitDistance
```

---

# 十二、Surface Spotlight shader 本阶段原则上不需要修改

`BasicModelPS.hlsl` 的 surface lighting 本身是：

```text
Light Position
→ Surface Position
```

计算：

```hlsl
surfaceToLight
distanceToLight
lightDirection
coneCos
```

因此 Light Position offset 后，它天然会正确变化。

本阶段不要修改：

```text
BasicModelPS attenuation
InnerCos
OuterCos
Range
```

前提是上一阶段已经完成 surface / volumetric attenuation 统一。

---

# 十三、不要改 Camera 数据

不要修改：

```text
ShaderCameraData
CameraBuffer b6
Camera::GetForward
Camera::GetRight
Camera::GetUp
```

Camera 仍然表示观察者。

Flashlight 现在开始成为独立的 Light。

这是正确的概念分离：

```text
Camera
=
View

SpotLight
=
Light
```

不要因为 flashlight offset 又把 Camera 数据一起移动。

---

# 十四、编译顺序

先修改 C++：

```text
TestScene.cpp
```

编译主工程。

如果 C++ 编译正常，再修改：

```text
VolumetricLightingPS.hlsl
```

然后重新执行：

```text
ShaderCompile.bat
```

确认生成：

```text
VolumetricLightingPS.pso
```

再重新运行主程序。

不要一次修改几十个文件后第一次测试。

---

# 十五、第一轮视觉测试

站在平整的 Back Wall 前。

Camera 正对墙。

预期：

以前：

```text
Screen Center
    ↓
Spot Center
```

几乎严格重合。

现在 Spotlight Position 偏向：

```text
Camera Right
+
Camera Down
+
Camera Forward
```

所以墙上的亮斑应该发生轻微空间偏移。

不要期待巨大变化。

当前 offset 只有：

```text
Right   = +0.25
Up      = -0.20
Forward = +0.15
```

目标是确认：

Light 确实脱离 Camera Origin。

---

# 十六、第二轮测试：贴近墙壁

这是本阶段最重要的测试。

站到离墙较近的位置。

左右转动 Camera。

因为：

```text
Camera Origin
!=
Light Origin
```

墙上的投影应该明显比过去更具有：

parallax

不再像严格钉在屏幕中心。

尤其在：

斜着看墙

墙角

地板与墙交界处

观察差异。

---

# 十七、第三轮测试：Volumetric

观察空气中的 volumetric beam。

确认没有出现：

光锥突然缺一块

屏幕某一侧突然全黑

Camera 旋转时 cone 被错误裁掉

这种情况。

如果出现：

“Light offset 后，屏幕边缘本来应该有 beam，但整个 pixel 被提前裁掉”

优先检查：

旧的：

```hlsl
rayInsideCone
rayMovesForward
```

pixel-level early reject

是否真的已经删除。

---

# 十八、当前可接受的现象

因为 Direction 仍然：

```cpp
cameraForward
```

所以 Spotlight Axis 和 Camera Axis 仍然平行。

因此：

亮斑仍可能比较规则。

这是正常的。

本阶段只解决：

```text
Light Origin
!=
Camera Origin
```

还没有解决：

```text
Flashlight optical profile
Shadow
Cookie
Aim convergence
```

不要因为亮斑仍然圆，就认为本阶段失败。

---

# 十九、失败判断

以下情况属于 bug：

1.

Camera 不动，只旋转时：

Flashlight offset 没有跟着 Camera Local Basis 旋转。

说明用了 world-space offset：

```cpp
position += Vector3{0.25, -0.2, 0.15}
```

而不是：

```text
Right / Up / Forward
```

组合。

2.

Offset 后 volumetric 大面积消失。

优先检查旧 pixel-level cone early reject。

3.

Surface light 正常，但 volumetric 仍像从 Camera Origin 发出。

检查：

```text
LightingBuffer b4
```

中的：

```text
SpotLight.Position
```

是否每帧上传。

4.

转动 Camera 后 flashlight Position 不跟随。

检查 TestScene 是否每帧重新计算：

```cpp
cameraRight
cameraUp
cameraForward
```

---

# 二十、不要现在加入 Aim Target

本阶段不要做：

```cpp
const Vector3 target =
    cameraPosition +
    cameraForward *
    focusDistance;

flashlightDirection =
    Normalize(
        target -
        flashlightPosition);
```

虽然这通常会让手持 flashlight 更自然，

但它同时改变：

```text
Position
和
Direction
```

会让本轮测试失去单变量意义。

等本阶段确认正常后，下一阶段再单独讨论：

Flashlight Aim Convergence。

---

# 二十一、本阶段成功标准

必须满足：

1.

Flashlight Position 不再等于 Camera Position。

2.

Offset 使用：

Camera Right
Camera Up
Camera Forward

而不是 world-space 常量。

3.

Camera 旋转时 offset 正确跟随。

4.

Flashlight Direction 暂时仍等于 Camera Forward。

5.

Surface Spotlight 正常。

6.

Volumetric 正常。

7.

SceneDepth clamp 正常。

8.

没有因为 camera/light 分离出现 volumetric pixel 错误 early reject。

9.

没有新增 Constant Buffer 字段。

10.

没有修改 attenuation / phase / shadow / cookie。

11.

项目正常编译。

12.

VolumetricLightingPS 正常编译。

---

# 二十二、本阶段结束后不要自动继续修改

完成以后请向用户报告：

具体修改了哪些文件。

删除了哪些旧 Camera≈Light Apex 假设。

当前 flashlight local offset 数值。

运行结果。

是否观察到 wall projection parallax。

是否发现其他依赖 Camera≈Light Position 的代码。

不要直接继续实现下一阶段。

下一阶段预计是二选一：

A.

Flashlight Aim Convergence

让 Offset Flashlight 稍微瞄向 Camera Forward 某个 focus distance。

或者：

B.

Flashlight Cookie / Light Profile

开始打破完美均匀的圆形光斑。

由用户确认后再继续。