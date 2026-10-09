#include "Game/Scenes/TestScene.h"

#include "Engine/Debug/IDebugText.h"
#include "Engine/Input/IInput.h"
#include "Engine/Effects/Particle/ParticleBurstEffect.h"
#include "Engine/Effects/Screen/LowHealthScreenEffect.h"
#include "Game/Effects/GameEffectRecipes.h"
#include "Engine/Physics/Collision/Intersection.h"
#include "Engine/Rendering/Camera/ICameraBackend.h"

#include "Engine/Resources/ResourceSystem.h"

#include "Engine/Rendering/Model/Model.h"
#include "Engine/Rendering/Shader/Shader.h"
#include "Engine/Rendering/Texture/Texture.h"
#include "Engine/Rendering/Volumetric/ShaderVolumetricData.h"


#include <DxLib.h>

#include <format>
#include <string>
#include <cassert>

namespace
{
    // EN: Temporary conversion helper used only by TestScene debug drawing.
    //     Engine math types themselves must not depend on DxLib.
    //
    // JP: TestScene のデバッグ描画だけで使用する一時的な変換関数。
    //     Engine の Math 型自体は DxLib に依存させない。
    VECTOR ToDxVector(const Vector3& value)
    {
        return VGet(
            value.x,
            value.y,
            value.z);
    }

    // EN: Draws an AABB as twelve debug lines.
    //     This is diagnostic rendering only and is intentionally kept
    //     outside the collision geometry implementation.
    //
    // JP: AABB を12本のデバッグラインとして描画する。
    //     これは診断用描画だけを目的としており、
    //     Collision の幾何実装とは意図的に分離する。
    void DrawAABB(
        const AABB& bounds,
        unsigned int color)
    {
        const Vector3& min = bounds.min;
        const Vector3& max = bounds.max;

        const Vector3 corners[8]
        {
            // Bottom
            { min.x, min.y, min.z },
            { max.x, min.y, min.z },
            { max.x, min.y, max.z },
            { min.x, min.y, max.z },

            // Top
            { min.x, max.y, min.z },
            { max.x, max.y, min.z },
            { max.x, max.y, max.z },
            { min.x, max.y, max.z }
        };

        constexpr int edges[12][2]
        {
            // Bottom
            { 0, 1 },
            { 1, 2 },
            { 2, 3 },
            { 3, 0 },

            // Top
            { 4, 5 },
            { 5, 6 },
            { 6, 7 },
            { 7, 4 },

            // Vertical
            { 0, 4 },
            { 1, 5 },
            { 2, 6 },
            { 3, 7 }
        };

        for (const auto& edge : edges)
        {
            DrawLine3D(
                ToDxVector(corners[edge[0]]),
                ToDxVector(corners[edge[1]]),
                color);
        }
    }
}

TestScene::TestScene(
    IInput& input,
    IDebugText& debugText,
    ICameraBackend& cameraBackend,
    ResourceSystem& resourceSystem,
    IRendererBackend& rendererBackend)
    : m_input(input)
    , m_debugText(debugText)
    , m_cameraBackend(cameraBackend)
    , m_inputMap(input)
    , m_fpsController(m_inputMap)
    , m_cameraController(input)
    , m_characterController(m_collisionWorld)
    , m_resourceSystem(resourceSystem)
    , m_renderer(rendererBackend)
{
}

void TestScene::OnEnter()
{
    // ------------------------------------------------------------
    // Texture resource test (Phase A)
    // ------------------------------------------------------------
    m_testTexture =
        m_resourceSystem.LoadTexture(
            "Assets/test.png");

    assert(m_testTexture != nullptr);
    assert(m_testTexture->IsValid());

    const std::shared_ptr<Texture> cachedTexture =
        m_resourceSystem.LoadTexture(
            "Assets/test.png");

    // EN: Identical normalized paths must reuse one shared resource.
    //
    // JP: ???????????????????????? Path ?????????? Shared Resource ??????????p????????B
    assert(cachedTexture == m_testTexture);

    m_bloodTexture =
        m_resourceSystem.LoadTexture(
            "Assets/Textures/BloodDroplet.png");
    assert(m_bloodTexture != nullptr);
    assert(m_bloodTexture->IsValid());

    // EN: Start below the threshold so the overlay can be verified immediately.
    // JP: Start the test health below the threshold.
    m_testHealthRatio = 0.10f;

    // ------------------------------------------------------------
    // shader test
    // ------------------------------------------------------------
    m_testShader =
        m_resourceSystem.LoadShader(
            "Assets/Shaders/Source/BasicModelVS.vso",
            "Assets/Shaders/Source/BasicModelPS.pso");

    assert(m_testShader != nullptr);
    assert(m_testShader->IsValid());


    m_sceneDepthShader =
        m_resourceSystem.LoadShader(
            "Assets/Shaders/Source/SceneDepthVS.vso",
            "Assets/Shaders/Source/SceneDepthPS.pso");

    assert(m_sceneDepthShader != nullptr);
    assert(m_sceneDepthShader->IsValid());

    m_testModelInstance.SetShader(
        m_testShader);

    // ------------------------------------------------------------
    // Input Mapping
    // ------------------------------------------------------------

    // EN: TestScene currently defines the default gameplay bindings.
    //     InputMap itself does not know which physical keys the game uses.
    //
    // JP: 現段階では TestScene が Gameplay のデフォルト Binding を定義する。
    //     InputMap 自体は Game が使用する物理キーを知らない。
    m_inputMap.Bind(
        InputAction::MoveForward,
        KeyCode::W);

    m_inputMap.Bind(
        InputAction::MoveBackward,
        KeyCode::S);

    m_inputMap.Bind(
        InputAction::MoveLeft,
        KeyCode::A);

    m_inputMap.Bind(
        InputAction::MoveRight,
        KeyCode::D);

    // ------------------------------------------------------------
    // Test Model
    // ------------------------------------------------------------
    /*m_testModel =
        m_resourceSystem.LoadModel(
            "Assets/Models/test/SimpleModel.mqo");*/

    m_testModel =
        m_resourceSystem.LoadModel(
            "Assets/Models/bicycle.mv1");

    assert(m_testModel != nullptr);
    assert(m_testModel->IsValid());

    m_testModelInstance.SetModel(
        m_testModel);

    auto& transform =
        m_testModelInstance.GetTransform();

    constexpr float Pi =
        3.14159265358979323846f;

    transform.position =
        Vector3{
            0.0f,
            2.0f,
            5.0f
    };

    transform.rotation =
        Quaternion{};  // Identity rotation

    transform.rotation =
        Quaternion::FromAxisAngle(
            Vector3{ 0.0f, 1.0f, 0.0f },
            Pi * 0.5f);

    transform.scale =
        Vector3{
            0.02f,
            0.02f,
            0.02f
        };

	// ------------------------------------------------------------
    // Test Room Geometry
    // ------------------------------------------------------------
    
    // EN: Load one unit cube and reuse it for the floor and walls.
    //     Each ModelInstance owns only its transform/state while sharing
    //     the same immutable model resource.
    //
    // JP: Unit Cube を 1 回だけ Load し、Floor と Wall で共有する。
    //     各 ModelInstance は Transform / State のみを保持し、
    //     同一の Model Resource を共有する。
    m_testCubeModel =
        m_resourceSystem.LoadModel(
            "Assets/Models/TestCube.mqo");

    assert(m_testCubeModel != nullptr);
    assert(m_testCubeModel->IsValid());

    m_floorInstance.SetModel(
        m_testCubeModel);

    m_backWallInstance.SetModel(
        m_testCubeModel);

    m_leftWallInstance.SetModel(
        m_testCubeModel);

    m_rightWallInstance.SetModel(
        m_testCubeModel);


    m_floorInstance.SetShader(
        m_testShader);

    m_backWallInstance.SetShader(
        m_testShader);

    m_leftWallInstance.SetShader(
        m_testShader);

    m_rightWallInstance.SetShader(
        m_testShader);

    {
        Transform& transform =
            m_floorInstance.GetTransform();

        // EN: Thin unit cube scaled into a floor covering the
        //     current flashlight test area.
        //
        // JP: Unit Cube を薄く引き伸ばし、現在の Flashlight Test Area を
        //     覆う Floor として使用する。
        transform.position =
            Vector3{
                0.0f,
                -0.05f,
                2.0f
        };

        transform.rotation =
            Quaternion{};

        transform.scale =
            Vector3{
                12.0f,
                0.1f,
                16.0f
        };
    }

    {
        Transform& transform =
            m_backWallInstance.GetTransform();

        transform.position =
            Vector3{
                0.0f,
                2.5f,
                9.95f
        };

        transform.rotation =
            Quaternion{};

        transform.scale =
            Vector3{
                12.0f,
                5.0f,
                0.1f
        };
    }

    {
        Transform& transform =
            m_leftWallInstance.GetTransform();

        transform.position =
            Vector3{
                -5.95f,
                2.5f,
                2.0f
        };

        transform.rotation =
            Quaternion{};

        transform.scale =
            Vector3{
                0.1f,
                5.0f,
                16.0f
        };
    }

    {
        Transform& transform =
            m_rightWallInstance.GetTransform();

        transform.position =
            Vector3{
                5.95f,
                2.5f,
                2.0f
        };

        transform.rotation =
            Quaternion{};

        transform.scale =
            Vector3{
                0.1f,
                5.0f,
                16.0f
        };
    }


    // ------------------------------------------------------------
    // Player
    // ------------------------------------------------------------

    // EN: Player position represents the character's ground-level
    //     reference point, not the eye position.
    //
    // JP: Player の Position は目の位置ではなく、
    //     キャラクターの地面基準位置を表す。
    m_playerTransform.position =
        Vector3{
            0.0f,
            0.0f,
            -5.0f
        };

    m_playerTransform.rotation =
        Quaternion{};

    m_playerTransform.scale =
        Vector3{ 1.0f };


    // ------------------------------------------------------------
    // Camera
    // ------------------------------------------------------------

    // EN: Identity rotation faces the engine's +Z forward direction.
    //
    // JP: 単位回転では Engine の +Z Forward 方向を向く。
    m_camera.GetTransform().rotation =
        Quaternion{};

    constexpr Vector3 eyeOffset{
        0.0f,
        1.7f,
        0.0f
    };

    m_camera.GetTransform().position =
        m_playerTransform.position +
        eyeOffset;


    // ------------------------------------------------------------
    // Test World AABB
    // ------------------------------------------------------------
	// EN: Create a fixed world collider used as a temporary obstacle.
	//
	// JP: 一時的な障害物として使用する
	//     固定 World Collider を作成する。
	/*m_testWorldCollider.SetBounds(
		AABB{
			Vector3{
				-4.0f,
				 0.0f,
				 1.0f
			},
			Vector3{
				 1.0f,
				 2.0f,
				 2.0f
			}
		});

    m_testWorldCollider2.SetBounds(
        AABB{
            Vector3{
                 1.0f,
                 0.0f,
                 1.0f
            },
            Vector3{
                 2.0f,
                 2.0f,
                 6.0f
            }
        });*/

    // EN: Both colliders participate in CollisionWorld queries.
    //
    // JP: 両方の Collider を CollisionWorld の
    //     Query 対象として登録する。
    m_collisionWorld.Register(
        m_playerCollider);
    /*m_collisionWorld.Register(
        m_testWorldCollider);

    m_collisionWorld.Register(
        m_testWorldCollider2);*/


    // ------------------------------------------------------------
    // volumetric data
    // ------------------------------------------------------------
    ShaderVolumetricData volumetric{};

    volumetric.enabled = 1.0f;
    volumetric.intensity = 4.0f;
    volumetric.scattering = 0.35f;

    m_renderer.SetVolumetricSettings(
        volumetric);

    // ------------------------------------------------------------
    // Fog
    // ------------------------------------------------------------
    m_fog.enabled = true;

    m_fog.color =
        Vector3{
            0.25f,
            0.25f,
            0.25f
    };

    m_fog.startDistance = 0.0f;

    m_fog.density = 0.2f;

    m_fog.endDistance = 25.0f;

    m_renderer.SetFog(m_fog);

    m_renderer.SetClearColor(
        m_clearColor);

    // ------------------------------------------------------------
    // ambient light
    // ------------------------------------------------------------
    m_ambientLight.enabled = true;

    m_ambientLight.color =
        Vector3{
            0.04f,
            0.045f,
            0.055f
    };

    m_ambientLight.intensity = 1.0f;

    m_renderer.SetAmbientLight(
        m_ambientLight);

    // ------------------------------------------------------------
    // test spot light
    // ------------------------------------------------------------
    m_flashlight.enabled = true;

    /*m_flashlight.position =
        Vector3{
            0.0f,
            2.0f,
            0.0f
    };

    m_flashlight.direction =
        Vector3{
            0.0f,
            0.0f,
            1.0f
    };*/

    m_flashlight.color =
        Vector3{
            1.00f,
            0.95f,
            0.85f
    };

    m_flashlight.intensity = 1.0f;
    m_flashlight.range = 15.0f;

    m_flashlight.innerAngle = 0.30f;
    m_flashlight.outerAngle = 0.50f;

    m_renderer.SetSpotLight(
        m_flashlight);
}

void TestScene::OnExit()
{
    m_visualEffects.Clear();
    m_collisionWorld.Clear();

    m_renderer.SetClearColor(
        Vector3{
            0.0f,
            0.0f,
            0.0f
        });

    // EN: Releases scene-owned resource references before the
    //     rendering platform is shut down.
    //
    // JP: Rendering Platform が終了する前に、
    //     Scene が保持する Resource Reference を解放する。
    m_testModelInstance.SetModel(nullptr);
    m_testModel.reset();

    m_floorInstance.SetModel(nullptr);
    m_backWallInstance.SetModel(nullptr);
    m_leftWallInstance.SetModel(nullptr);
    m_rightWallInstance.SetModel(nullptr);

    m_floorInstance.SetShader(nullptr);
    m_backWallInstance.SetShader(nullptr);
    m_leftWallInstance.SetShader(nullptr);
    m_rightWallInstance.SetShader(nullptr);

    m_testCubeModel.reset();

    m_flashlight.enabled = false;

    m_renderer.SetSpotLight(
        m_flashlight);

    FogSettings disabledFog;
    disabledFog.enabled = false;

    m_renderer.SetFog(disabledFog);

    m_testModelInstance.SetShader(nullptr);
    m_testShader.reset();
    m_bloodTexture.reset();
    m_sceneDepthShader.reset();

    ShaderVolumetricData volumetric{};
    volumetric.enabled = 0.0f;

    m_renderer.SetVolumetricSettings(
        volumetric);
}

void TestScene::Update(float deltaTime)
{
    m_deltaTime = deltaTime;
    m_visualEffects.Update(deltaTime);

    // EN: H lowers test health and J restores it without adding a health system.
    // JP: H lowers test health and J restores it.
    if (m_input.IsKeyPressed(KeyCode::H))
    {
        m_testHealthRatio = (m_testHealthRatio > 0.10f) ? m_testHealthRatio - 0.10f : 0.0f;
    }
    if (m_input.IsKeyPressed(KeyCode::J))
    {
        m_testHealthRatio = (m_testHealthRatio < 0.90f) ? m_testHealthRatio + 0.10f : 1.0f;
    }
    m_lowHealthEffect.SetHealthRatio(m_testHealthRatio);
    m_lowHealthEffect.Update(deltaTime);


    // ------------------------------------------------------------
    // Camera Look
    // ------------------------------------------------------------

    // EN: Update view orientation before movement so WASD uses
    //     the current frame's camera direction.
    //
    // JP: WASD が現在フレームの Camera 方向を使用できるよう、
    //     移動処理より先に視点姿勢を更新する。
    m_cameraController.Update(
        m_camera,
        deltaTime);


    // ------------------------------------------------------------
    // Player Movement
    // ------------------------------------------------------------

    // EN: Convert FPS input into a desired world-space displacement.
    //     No collision response happens inside FPSController.
    //
    // JP: FPS 入力をワールド空間上の希望移動量へ変換する。
    //     FPSController 内では Collision Response を行わない。
    const Vector3 desiredMovement =
        m_fpsController.ComputeMovement(
            m_camera,
            deltaTime);


    // EN: CharacterController decides how the requested movement
    //     affects the actual character position after collision
    //     resolution.
    //
    // JP: CharacterController が Collision の解決結果を考慮し、
    //     要求された移動を実際の Character 位置へ反映する。
    m_characterController.Move(
        m_playerTransform,
        m_playerCollider,
        desiredMovement);


    // ------------------------------------------------------------
    // Camera Follow
    // ------------------------------------------------------------

    // EN: Camera follows the player's ground reference position
    //     with a fixed eye-height offset.
    //
    // JP: Camera は Player の地面基準位置に固定の目線高さを
    //     加えた位置へ追従する。
    constexpr Vector3 eyeOffset{
        0.0f,
        1.7f,
        0.0f
    };

    m_camera.GetTransform().position =
        m_playerTransform.position +
        eyeOffset;

    // EN: Pressed is the input edge, so holding Space creates only one burst.
    //     Spawn after camera follow so the effect uses this frame's position.
    // JP: Pressed は押下の立ち上がりなので、Space を保持しても Burst は一つだけ。
    //     Camera Follow 後に生成し、今フレームの位置を使用する。
    if (m_input.IsKeyPressed(KeyCode::Space))
    {
        const Vector3 sparkPosition =
            m_camera.GetTransform().position +
            m_camera.GetForward() * 3.0f;
        const Vector3 sparkNormal =
            -m_camera.GetForward();

        // EN: TestScene selects a game recipe; the engine effect stays generic.
        // JP: TestScene ???? Game Recipe ???????I??????????AEngine Effect ????????p?????????????B
        ParticleBurstDesc burst =
            MakeMetalSparkBurst(
                sparkPosition,
                sparkNormal,
                m_testTexture,
                27u);
        m_visualEffects.Add(std::make_unique<ParticleBurstEffect>(burst));
    }

    // EN: B is an edge-triggered gameplay input for the airborne blood spray.
    //     The recipe uses alpha blending and never creates a surface decal.
    if (m_input.IsKeyPressed(KeyCode::B))
    {
        const Vector3 bloodPosition =
            m_camera.GetTransform().position +
            m_camera.GetForward() * 3.0f;
        const Vector3 bloodDirection =
            -m_camera.GetForward();
        ParticleBurstDesc bloodBurst =
            MakeBloodSprayBurst(
                bloodPosition,
                bloodDirection,
                m_bloodTexture,
                73u);
        m_visualEffects.Add(std::make_unique<ParticleBurstEffect>(bloodBurst));
    }

    // ------------------------------------------------------------
    // spotlight update
    // ------------------------------------------------------------
    const Vector3 cameraPosition =
        m_camera.GetTransform().position;
    const Vector3 cameraForward =
        m_camera.GetForward();
    const Vector3 cameraRight =
        m_camera.GetRight();
    const Vector3 cameraUp =
        m_camera.GetUp();

    m_renderer.SetCameraPosition(cameraPosition);
    m_renderer.SetCameraForward(cameraForward);

    // EN: Keep this prototype's flashlight offset in camera-local space.
    // JP: この試作の Flashlight Offset は Camera Local Space で指定する。
    constexpr float kFlashlightOffsetRight = 0.05f;
    constexpr float kFlashlightOffsetUp = -0.02f;
    constexpr float kFlashlightOffsetForward = 0.05f;

    // EN: Rebuild the offset from the camera basis every frame so the light
    //     origin follows camera rotation as well as camera movement.
    // JP: 毎フレーム Camera Basis から Offset を再構築し、Light Origin を
    //     Camera の移動と回転の両方に追従させる。
    m_flashlight.position =
        cameraPosition +
        cameraRight * kFlashlightOffsetRight +
        cameraUp * kFlashlightOffsetUp +
        cameraForward * kFlashlightOffsetForward;

    // EN: Change only the light origin in V1. Keep the beam axis parallel
    //     to Camera Forward to assess the position offset independently.
    // JP: V1 では Light Origin だけを変更する。Position Offset の影響を
    //     単独で確認できるよう、Beam Axis は Camera Forward と平行に保つ。
    m_flashlight.direction =
        cameraForward;

    m_renderer.SetSpotLight(
        m_flashlight);


    // ------------------------------------------------------------
    // Temporary Player AABB
    // ------------------------------------------------------------
    constexpr float playerHalfWidth = 0.3f;
    constexpr float playerHeight = 1.8f;

    const Vector3& playerPosition =
        m_playerTransform.position;

    // EN: Synchronize the temporary player collider with the
    //     player's current world-space position.
    //
    // JP: 一時的な Player Collider を Player の現在の
    //     ワールド位置へ同期する。
    m_playerCollider.SetBounds(
        AABB{
            Vector3{
                playerPosition.x - playerHalfWidth,
                playerPosition.y,
                playerPosition.z - playerHalfWidth
            },
            Vector3{
                playerPosition.x + playerHalfWidth,
                playerPosition.y + playerHeight,
                playerPosition.z + playerHalfWidth
            }
        });

    // ------------------------------------------------------------
    // Collision Detection
    // ------------------------------------------------------------

    // EN: Detect overlap only. Collision response is deliberately
    //     not implemented yet, so the player may pass through the box.
    //
    // JP: 現段階では重なりの検出だけを行う。
    //     Collision Response はまだ実装しないため、
    //     Player は Box を通り抜けることができる。
    /*m_isColliding =
        m_collisionWorld.OverlapsAny(
            m_playerCollider);*/

    m_isColliding =
        m_collisionWorld.ComputeCollision(
            m_playerCollider,
            m_collisionHit);
}

void TestScene::Render()
{
    // ------------------------------------------------------------
    // Camera
    // ------------------------------------------------------------

    // EN: Apply engine Camera data to the active DxLib camera
    //     before issuing any 3D drawing commands.
    //
    // JP: 3D 描画命令を実行する前に、Engine の Camera 情報を
    //     DxLib の有効な Camera へ適用する。
    m_cameraBackend.Apply(m_camera);

    // ------------------------------------------------------------
    // Temporary World Grid
    // ------------------------------------------------------------

    const unsigned int gridColor =
        GetColor(80, 80, 80);

    constexpr int gridHalfSize = 10;

    for (int i = -gridHalfSize;
        i <= gridHalfSize;
        ++i)
    {
        DrawLine3D(
            VGet(
                static_cast<float>(i),
                0.0f,
                -static_cast<float>(gridHalfSize)),
            VGet(
                static_cast<float>(i),
                0.0f,
                static_cast<float>(gridHalfSize)),
            gridColor);

        DrawLine3D(
            VGet(
                -static_cast<float>(gridHalfSize),
                0.0f,
                static_cast<float>(i)),
            VGet(
                static_cast<float>(gridHalfSize),
                0.0f,
                static_cast<float>(i)),
            gridColor);
    }


    // test model
    m_renderer.Draw(
        m_testModelInstance);

    // ------------------------------------------------------------
    // Test room
    // ------------------------------------------------------------

    m_renderer.Draw(
        m_floorInstance);

    m_renderer.Draw(
        m_backWallInstance);

    m_renderer.Draw(
        m_leftWallInstance);

    m_renderer.Draw(
        m_rightWallInstance);

    m_renderer.SetCameraRight(m_camera.GetRight());
    m_renderer.SetCameraUp(m_camera.GetUp());
    m_visualEffects.Render(m_renderer);

    // ------------------------------------------------------------
    // AABB Debug Rendering
    // ------------------------------------------------------------

    const unsigned int worldBoundsColor =
        GetColor(255, 255, 0);

    const unsigned int playerBoundsColor =
        m_isColliding
        ? GetColor(255, 0, 0)
        : GetColor(0, 255, 255);

    DrawAABB(
        m_testWorldCollider.GetBounds(),
        worldBoundsColor);

    DrawAABB(
        m_playerCollider.GetBounds(),
        playerBoundsColor);

    DrawAABB(
        m_testWorldCollider2.GetBounds(),
        worldBoundsColor);


    // ------------------------------------------------------------
    // Debug Text
    // ------------------------------------------------------------
    /*const float fps =
        m_deltaTime > 0.0f
        ? 1.0f / m_deltaTime
        : 0.0f;

    const std::string fpsText =
        std::format(
            "FPS: {:.1f}",
            fps);

    const std::string deltaTimeText =
        std::format(
            "Delta Time: {:.4f}",
            m_deltaTime);

    const std::string playerPositionText =
        std::format(
            "Player Position: ({:.2f}, {:.2f}, {:.2f})",
            m_playerTransform.position.x,
            m_playerTransform.position.y,
            m_playerTransform.position.z);

    const std::string collisionText =
        std::format(
            "Collision: {}",
            m_isColliding ? "YES" : "NO");

    const std::string collisionNormalText =
        std::format(
            "Normal: ({:.1f}, {:.1f}, {:.1f})",
            m_collisionHit.normal.x,
            m_collisionHit.normal.y,
            m_collisionHit.normal.z);

    const std::string penetrationText =
        std::format(
            "Penetration: {:.3f}",
            m_collisionHit.penetration);

    m_debugText.Draw(
        20,
        20,
        fpsText.c_str());

    m_debugText.Draw(
        20,
        40,
        deltaTimeText.c_str());

    m_debugText.Draw(
        20,
        60,
        playerPositionText.c_str());

    m_debugText.Draw(
        20,
        80,
        collisionText.c_str());

    m_debugText.Draw(
        20,
        100,
        collisionNormalText.c_str());

    m_debugText.Draw(
        20,
        120,
        penetrationText.c_str());*/

}

void TestScene::RenderOverlay()
{
    // EN: This pass runs after volumetric composition and owns no world VFX.
    // JP: Overlay pass follows volumetric lighting.
    m_lowHealthEffect.Render(m_renderer);
}

void TestScene::RenderDepth()
{
    // EN: Transparent VFX must not truncate volumetric rays like opaque walls.
    // JP: 透明 VFX は不透明な壁のように Volumetric Ray を遮断してはならない。
    if (!m_sceneDepthShader ||
        !m_sceneDepthShader->IsValid())
    {
        return;
    }


    // EN: SetDrawScreen resets the camera when the depth target is bound.
    //     Reapply the color-pass camera before drawing the same geometry.
    //
    // JP: 深度ターゲットへの切り替えでカメラ設定がリセットされるため、
    //     同じ形状を描く前にカラーパスと同じカメラを再適用する。
    m_cameraBackend.Apply(m_camera);

    // EN: Ensure Renderer has the latest camera position and forward
    //     before the depth pass draw.
    //
    // JP: Depth Pass 描画前に Renderer が最新の Camera Position と
    //     Forward を保持していることを保証する。
    m_renderer.SetCameraPosition(
        m_camera.GetTransform().position);

    m_renderer.SetCameraForward(
        m_camera.GetForward());

    m_renderer.SetCameraRight(
        m_camera.GetRight());

    m_renderer.SetCameraUp(
        m_camera.GetUp());

    m_renderer.SetCameraFieldOfView(
        m_camera.GetFieldOfView());

    // EN: Depth pass draws only opaque geometry that should
    //     block volumetric light.
    //
    // JP: Depth Pass では Volumetric Light を遮る
    //     Opaque Geometry のみを描画する。
    m_renderer.Draw(
        m_testModelInstance,
        *m_sceneDepthShader);

    // EN: The same opaque room geometry rendered in the color pass must
    //     also participate in the scene-depth pass so it can terminate
    //     volumetric rays correctly.
    //
    // JP: Color Pass で描画する同じ Opaque Room Geometry を
    //     Scene Depth Pass にも参加させ、Volumetric Ray を
    //     正しく遮断できるようにする。
    m_renderer.Draw(
        m_floorInstance,
        *m_sceneDepthShader);

    m_renderer.Draw(
        m_backWallInstance,
        *m_sceneDepthShader);

    m_renderer.Draw(
        m_leftWallInstance,
        *m_sceneDepthShader);

    m_renderer.Draw(
        m_rightWallInstance,
        *m_sceneDepthShader);
}