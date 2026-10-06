#include "Engine/Effects/VisualEffectSystem.h"
#include "Engine/Effects/Particle/ParticleBurstEffect.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/Model/IModelResource.h"
#include "Engine/Rendering/Shader/IShaderResource.h"
#include "Platform/DxLib/DxInput.h"
#include "DxLib.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>

bool spaceDown = false;
int CheckHitKey(int key) { return key == KEY_INPUT_SPACE && spaceDown; }
int GetMouseInput() { return 0; }
int GetMousePoint(int* x, int* y) { *x = 0; *y = 0; return 0; }

class RecordingBackend final : public IRendererBackend
{
public:
    std::vector<BillboardRenderData> data;
    int submissions = 0;
    BillboardBlendMode mode{};
    std::unique_ptr<IModelResource> CreateModelResource(const char*) override { return {}; }
    std::unique_ptr<IShaderResource> CreateShaderResource(const char*, const char*) override { return {}; }
    void BeginSceneRender(int, int) override {}
    void EndSceneRender(int, int) override {}
    void BeginSceneDepthRender(int, int) override {}
    void EndSceneDepthRender() override {}
    void RenderVolumetricLighting(int, int) override {}
    void SetCameraPosition(const Vector3&) override {}
    void SetCameraForward(const Vector3&) override {}
    void SetCameraRight(const Vector3&) override {}
    void SetCameraUp(const Vector3&) override {}
    void SetCameraFieldOfView(float) override {}
    void SetClearColor(const Vector3&) override {}
    void SetFog(const FogSettings&) override {}
    void SetAmbientLight(const AmbientLight&) override {}
    void SetPointLight(const PointLight&) override {}
    void SetSpotLight(const SpotLight&) override {}
    void Draw(const ModelInstance&) override {}
    void Draw(const ModelInstance&, const Shader&) override {}
    void SetVolumetricSettings(const ShaderVolumetricData&) override {}
    void DrawVolumetricCone(const VolumetricCone&, const Shader&) override {}
    void Shutdown() override {}
    void DrawBillboards(std::span<const BillboardRenderData> items,
        BillboardBlendMode blend) override
    {
        ++submissions;
        data.assign(items.begin(), items.end());
        mode = blend;
    }
};

class LifetimeProbe final : public IVisualEffect
{
public:
    explicit LifetimeProbe(int& destroyed) : destroyed(destroyed) {}
    ~LifetimeProbe() override { ++destroyed; }
    void Update(float) override { finished = true; }
    void Render(Renderer&) const override {}
    bool IsFinished() const override { return finished; }
private:
    int& destroyed;
    bool finished = false;
};

int main()
{
    RecordingBackend backend;
    Renderer renderer(backend);
    VisualEffectSystem effects;
    effects.Add(nullptr);
    effects.Update(0.0f);
    effects.Render(renderer);
    effects.Clear();
    assert(backend.submissions == 0);

    ParticleBurstDesc desc;
    desc.position = Vector3{ 1.0f, 2.0f, 3.0f };
    desc.direction = Vector3{ 0.0f, 0.0f, -1.0f };
    desc.spread = 0.0f;
    desc.speedMin = desc.speedMax = 2.0f;
    desc.lifetimeMin = desc.lifetimeMax = 0.8f;
    ParticleBurstEffect burst(desc);
    burst.Render(renderer);
    assert(backend.data.size() == 16);
    assert(backend.mode == BillboardBlendMode::Additive);
    assert(backend.data.front().position.z == 3.0f);
    assert(backend.data.front().alpha == 1.0f);
    burst.Update(0.2f);
    burst.Render(renderer);
    assert(std::abs(backend.data.front().position.z - 2.6f) < 0.0001f);
    assert(std::abs(backend.data.front().alpha - 0.75f) < 0.0001f);
    assert(backend.data.front().size < desc.startSize);
    burst.Update(-1.0f);
    burst.Update(std::numeric_limits<float>::quiet_NaN());
    burst.Render(renderer);
    assert(std::abs(backend.data.front().alpha - 0.75f) < 0.0001f);
    burst.Update(0.8f);
    assert(burst.IsFinished());
    burst.Render(renderer);
    assert(backend.data.empty());

    ParticleBurstEffect spreadBurst(ParticleBurstDesc{});
    spreadBurst.Update(0.1f);
    spreadBurst.Render(renderer);
    bool independentMovement = false;
    for (const auto& item : backend.data)
    {
        independentMovement |= (item.position - backend.data.front().position).LengthSquared() > 0.0f;
    }
    assert(independentMovement);

    int destroyed = 0;
    effects.Add(std::make_unique<LifetimeProbe>(destroyed));
    effects.Update(0.1f);
    assert(destroyed == 1);
    effects.Add(std::make_unique<LifetimeProbe>(destroyed));
    effects.Clear();
    assert(destroyed == 2);
    {
        VisualEffectSystem temporary;
        temporary.Add(std::make_unique<LifetimeProbe>(destroyed));
    }
    assert(destroyed == 3);

    DxInput input;
    int spawns = 0;
    const auto tick = [&] {
        input.Update();
        if (input.IsKeyPressed(KeyCode::Space))
        {
            ++spawns;
            effects.Add(std::make_unique<ParticleBurstEffect>(ParticleBurstDesc{}));
        }
    };
    tick();
    assert(spawns == 0);
    spaceDown = true;
    for (int i = 0; i < 120; ++i) tick();
    assert(spawns == 1);
    effects.Render(renderer);
    const int oneBurstSubmissions = backend.submissions;
    spaceDown = false;
    tick();
    assert(input.IsKeyReleased(KeyCode::Space));
    spaceDown = true;
    tick();
    assert(spawns == 2);
    effects.Render(renderer);
    assert(backend.submissions == oneBurstSubmissions + 2);
    effects.Update(1.0f);
    const int finishedSubmissions = backend.submissions;
    effects.Render(renderer);
    assert(backend.submissions == finishedSubmissions);
    effects.Clear();
    std::cout << "VFX tests passed: movement, fade, ownership, cleanup, repeated spawn, Space edge.\n";
}
