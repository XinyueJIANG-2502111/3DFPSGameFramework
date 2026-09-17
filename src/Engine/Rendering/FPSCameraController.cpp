#include "Engine/Rendering/FPSCameraController.h"

#include "Engine/Input/IInput.h"
#include "Engine/Rendering/Camera.h"

#include <algorithm>
#include <numbers>

FPSCameraController::FPSCameraController(IInput& input)
    : m_input(input)
{
}

void FPSCameraController::Update(
    Camera& camera,
    float deltaTime)
{
    const MouseDelta mouseDelta =
        m_input.GetMouseDelta();

    // EN: Mouse delta already represents displacement between frames.
    //     Unlike movement speed, it should not be multiplied by deltaTime.
    //
    // JP: MouseDelta はすでにフレーム間の移動量を表している。
    //     移動速度とは異なり、ここでは deltaTime を掛けない。
    const float horizontalSign =
        m_invertX ? -1.0f : 1.0f;

    const float verticalSign =
        m_invertY ? -1.0f : 1.0f;

    // EN: Apply inversion to the input direction before accumulating
    //     the yaw and pitch control angles.
    //
    // JP: Yaw / Pitch の制御角度を累積する前に、
    //     入力方向へ反転設定を適用する。
    m_yaw +=
        mouseDelta.x *
        m_mouseSensitivity *
        horizontalSign;

    m_pitch +=
        mouseDelta.y *
        m_mouseSensitivity *
        verticalSign;

    // EN: Limit pitch to slightly less than 90 degrees so the FPS camera
    //     cannot flip vertically.
    //
    // JP: FPS Camera が上下方向に反転しないよう、
    //     Pitch を 90 度よりわずかに小さい範囲に制限する。
    constexpr float maxPitch =
        89.0f *
        std::numbers::pi_v<float> /
        180.0f;

    m_pitch = std::clamp(
        m_pitch,
        -maxPitch,
        maxPitch);

    // EN: Yaw rotates around the world's Y axis.
    //
    // JP: Yaw はワールド空間の Y 軸を中心に回転する。
    const Quaternion yawRotation =
        Quaternion::FromAxisAngle(
            Vector3{ 0.0f, 1.0f, 0.0f },
            m_yaw);

    // EN: Pitch is constructed around the local X basis.
    //     Combined as yaw * pitch, the pitch axis follows the yawed
    //     camera orientation and produces FPS-style looking behavior.
    //
    // JP: Pitch はローカル X 基準軸を中心として構成する。
    //     Yaw * Pitch の順で合成することで、Pitch 軸は Yaw 後の
    //     Camera 姿勢に追従し、FPS 形式の視点操作になる。
    const Quaternion pitchRotation =
        Quaternion::FromAxisAngle(
            Vector3{ 1.0f, 0.0f, 0.0f },
            m_pitch);

    // EN: Rebuild the final orientation from yaw and pitch every frame
    //     instead of continuously accumulating quaternion rotations.
    //     This keeps the FPS control state explicit and prevents
    //     unintended roll accumulation.
    //
    // JP: Quaternion の回転を継続的に累積するのではなく、
    //     毎フレーム Yaw と Pitch から最終姿勢を再構築する。
    //     これにより FPS の制御状態を明確に保ち、
    //     意図しない Roll の累積を防ぐ。
    camera.GetTransform().rotation =
        (yawRotation * pitchRotation).Normalized();

    // EN: deltaTime is intentionally unused for raw mouse displacement.
    //
    // JP: 生のマウス移動量には deltaTime を使用しないため、
    //     現段階では意図的に未使用とする。
    (void)deltaTime;
}

void FPSCameraController::SetMouseSensitivity(float sensitivity)
{
    m_mouseSensitivity = sensitivity;
}

void FPSCameraController::SetInvertX(bool invert)
{
    m_invertX = invert;
}

void FPSCameraController::SetInvertY(bool invert)
{
    m_invertY = invert;
}