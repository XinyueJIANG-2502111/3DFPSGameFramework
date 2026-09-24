#include "Game/Scenes/TestScene.h"

#include "Engine/Debug/IDebugText.h"
#include "Engine/Input/IInput.h"
#include "Engine/Physics/Collision/Intersection.h"
#include "Engine/Rendering/ICameraBackend.h"

#include "Engine/Resources/ResourceSystem.h"

#include "Engine/Rendering/Model.h"

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
    m_testModel =
        m_resourceSystem.LoadModel(
            "Assets/Models/Bicycle.mv1");

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
            0.0f,
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
	m_testWorldCollider.SetBounds(
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
        });

    // EN: Both colliders participate in CollisionWorld queries.
    //
    // JP: 両方の Collider を CollisionWorld の
    //     Query 対象として登録する。
    m_collisionWorld.Register(
        m_playerCollider);

    m_collisionWorld.Register(
        m_testWorldCollider);

    m_collisionWorld.Register(
        m_testWorldCollider2);
}

void TestScene::OnExit()
{
    // EN: TestScene currently owns no external resource that requires
    //     explicit release.
    //
    // JP: 現段階の TestScene には明示的な解放が必要な
    //     外部 Resource は存在しない。

    m_collisionWorld.Clear();

    // EN: Releases scene-owned resource references before the
    //     rendering platform is shut down.
    //
    // JP: Rendering Platform が終了する前に、
    //     Scene が保持する Resource Reference を解放する。
    m_testModelInstance.SetModel(nullptr);
    m_testModel.reset();
}

void TestScene::Update(float deltaTime)
{
    m_deltaTime = deltaTime;


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


    // test model
    m_renderer.Draw(
        m_testModelInstance);


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


    // ------------------------------------------------------------
    // World Axes
    // ------------------------------------------------------------

    // EN: Engine coordinate convention:
    //     +X = Right, +Y = Up, +Z = Forward.
    //
    // JP: Engine の座標規約：
    //     +X = Right, +Y = Up, +Z = Forward。
    /*DrawLine3D(
        VGet(0.0f, 0.0f, 0.0f),
        VGet(3.0f, 0.0f, 0.0f),
        GetColor(255, 0, 0));

    DrawLine3D(
        VGet(0.0f, 0.0f, 0.0f),
        VGet(0.0f, 3.0f, 0.0f),
        GetColor(0, 255, 0));

    DrawLine3D(
        VGet(0.0f, 0.0f, 0.0f),
        VGet(0.0f, 0.0f, 3.0f),
        GetColor(0, 0, 255));*/


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

    const float fps =
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
        penetrationText.c_str());
}