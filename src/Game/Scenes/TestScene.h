#pragma once

#include "Engine/Scene/IScene.h"

#include "Engine/Rendering/ModelInstance.h"
#include "Engine/Rendering/Renderer.h"

#include "Engine/Rendering/Camera.h"
#include "Engine/Rendering/FPSCameraController.h"

#include "Engine/Math/Transform.h"
#include "Game/Player/FPSController.h"

#include "Engine/Input/InputMap.h"

#include "Engine/Physics/Collision/AABB.h"

#include "Engine/Physics/Collision/Collider.h"
#include "Engine/Physics/Collision/CollisionWorld.h"
#include "Engine/Physics/Collision/CollisionHit.h"

#include "Engine/Physics/Character/CharacterController.h"

#include <memory>

class Model;

class IInput;
class IDebugText;
class ICameraBackend;
class IRendererBackend;

class ResourceSystem;

class TestScene final : public IScene
{
public:
    explicit TestScene(
        IInput& input,
        IDebugText& debugText,
        ICameraBackend& cameraBackend,
        ResourceSystem& resourceSystem,
        IRendererBackend& rendererBackend);
    //~TestScene() override = default;

    void OnEnter() override;
    void OnExit() override;

    void Update(float deltaTime) override;
    void Render() override;

private:
    // EN: TestScene observes input through the engine abstraction rather
    //     than depending on the DxLib implementation. This keeps gameplay
    //     code independent from the platform backend.
    //
    // JP: TestScene は DxLib の実装ではなく Engine の入力抽象を利用する。
    //     これにより Gameplay 側をプラットフォーム実装から分離する。
    IInput& m_input;
    IDebugText& m_debugText;
    ICameraBackend& m_cameraBackend;

    Camera m_camera;

    FPSCameraController m_cameraController;

    Transform m_playerTransform;

    FPSController m_fpsController;

    float m_elapsedTime = 0.0f;
    float m_deltaTime = 0.0f;

    InputMap m_inputMap;

    // collider test
    CollisionWorld m_collisionWorld;

    Collider m_playerCollider;
    Collider m_testWorldCollider;
    Collider m_testWorldCollider2;

    CharacterController m_characterController;



    bool m_isColliding = false;

    CollisionHit m_collisionHit{};

private:
    ResourceSystem& m_resourceSystem;
    std::shared_ptr<Model> m_testModel;

    Renderer m_renderer;
    ModelInstance m_testModelInstance;
};