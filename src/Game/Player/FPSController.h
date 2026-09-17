#pragma once

#include "Engine/Math/Vector3.h"

class InputMap;
class Camera;

class FPSController
{
public:
    explicit FPSController(InputMap& inputMap);

    // EN: Computes the displacement requested by FPS movement input
    //     for the current frame. This controller does not modify
    //     the player's Transform directly.
    //
    // JP: 現在フレームの FPS 移動入力から要求される移動量を計算する。
    //     この Controller は Player の Transform を直接変更しない。
    Vector3 ComputeMovement(
        const Camera& camera,
        float deltaTime) const;

    void SetMoveSpeed(float speed);

private:
    InputMap& m_inputMap;

    float m_moveSpeed = 3.0f;
};