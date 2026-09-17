#include "Game/Player/FPSController.h"

#include "Engine/Input/InputAction.h"
#include "Engine/Input/InputMap.h"
#include "Engine/Rendering/Camera.h"

FPSController::FPSController(InputMap& inputMap)
    : m_inputMap(inputMap)
{
}

Vector3 FPSController::ComputeMovement(
    const Camera& camera,
    float deltaTime) const
{
    // EN: Project the camera forward direction onto the horizontal
    //     XZ plane so looking up or down does not move the player
    //     vertically.
    //
    // JP: Camera の Forward を水平な XZ 平面へ投影し、
    //     上下を向いても Player が垂直方向へ移動しないようにする。
    Vector3 forward = camera.GetForward();

    forward.y = 0.0f;
    forward = Normalize(forward);

    constexpr Vector3 worldUp{
        0.0f,
        1.0f,
        0.0f
    };

    const Vector3 right =
        Normalize(
            Cross(worldUp, forward));

    Vector3 moveDirection{};

    if (m_inputMap.IsDown(
        InputAction::MoveForward))
    {
        moveDirection += forward;
    }

    if (m_inputMap.IsDown(
        InputAction::MoveBackward))
    {
        moveDirection -= forward;
    }

    if (m_inputMap.IsDown(
        InputAction::MoveRight))
    {
        moveDirection += right;
    }

    if (m_inputMap.IsDown(
        InputAction::MoveLeft))
    {
        moveDirection -= right;
    }

    // EN: Normalize combined input so diagonal movement is not
    //     faster than movement along a single axis.
    //
    // JP: 複数方向の入力を正規化し、斜め移動が単一軸方向より
    //     速くならないようにする。
    if (moveDirection.LengthSquared() > 0.0f)
    {
        moveDirection =
            Normalize(moveDirection);
    }

    return
        moveDirection *
        m_moveSpeed *
        deltaTime;
}

void FPSController::SetMoveSpeed(float speed)
{
    m_moveSpeed = speed;
}