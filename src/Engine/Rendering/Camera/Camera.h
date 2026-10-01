#pragma once

#include "Engine/Math/Transform.h"

#include <numbers>

class Camera
{
public:
    Camera() = default;

    explicit Camera(
        float fieldOfViewRadians,
        float nearPlane,
        float farPlane);

    // EN: Camera transform defines the position and orientation of the
    //     observer in world space. Camera does not currently use scale.
    //
    // JP: Camera の Transform はワールド空間上での観測者の位置と姿勢を定義する。
    //     現在 Camera では Scale は使用しない。
    Transform& GetTransform();
    const Transform& GetTransform() const;

    float GetFieldOfView() const;
    float GetNearPlane() const;
    float GetFarPlane() const;

    void SetFieldOfView(float fieldOfViewRadians);
    void SetNearPlane(float nearPlane);
    void SetFarPlane(float farPlane);

    // EN: These directions are derived from the camera orientation instead
    //     of being stored separately, preventing duplicated rotation state.
    //
    // JP: これらの方向は個別に保持せず Camera の姿勢から計算する。
    //     これにより回転状態の重複と同期ずれを防ぐ。
    Vector3 GetForward() const;
    Vector3 GetRight() const;
    Vector3 GetUp() const;

private:
    Transform m_transform{};

    // EN: Projection parameters belong to the camera rather than Transform
    //     because they describe how the world is viewed, not where the
    //     camera exists in the world.
    //
    // JP: 投影パラメータは Camera が世界をどのように見るかを表すため、
    //     空間上の位置・姿勢を表す Transform ではなく Camera が保持する。
    float m_fieldOfView = std::numbers::pi_v<float> / 3.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
};