#pragma once

class IInput;
class Camera;

class FPSCameraController
{
public:
    explicit FPSCameraController(IInput& input);

    // EN: Updates the camera orientation from mouse input.
    //     The controller owns yaw and pitch as input-control state,
    //     while Camera stores the resulting spatial orientation.
    //
    // JP: マウス入力から Camera の姿勢を更新する。
    //     Controller は入力制御用の Yaw / Pitch を保持し、
    //     Camera はその結果となる空間上の姿勢を保持する。
    void Update(Camera& camera, float deltaTime);

    // EN: Sets the mouse sensitivity used to convert mouse movement
    //     into angular movement in radians.
    //
    // JP: マウス移動量をラジアン単位の回転角へ変換するための
    //     マウス感度を設定する。
    void SetMouseSensitivity(float sensitivity);

    // EN: Controls whether horizontal mouse look is inverted.
    //
    // JP: 水平方向のマウス視点操作を反転するかどうかを設定する。
    void SetInvertX(bool invert);

    // EN: Controls whether vertical mouse look is inverted.
    //
    // JP: 垂直方向のマウス視点操作を反転するかどうかを設定する。
    void SetInvertY(bool invert);

private:
    IInput& m_input;

    // EN: Horizontal and vertical view angles in radians.
    //
    // JP: 水平・垂直方向の視点角度。単位はラジアン。
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;

    // EN: Converts mouse movement in pixels into angular movement
    //     in radians.
    //
    // JP: マウス移動量（ピクセル）を回転角（ラジアン）へ変換する。
    float m_mouseSensitivity = 0.0025f;

    bool m_invertX = false;
    bool m_invertY = false;
};