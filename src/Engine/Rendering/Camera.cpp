#include "Engine/Rendering/Camera.h"

Camera::Camera(
    float fieldOfViewRadians,
    float nearPlane,
    float farPlane)
    : m_fieldOfView(fieldOfViewRadians)
    , m_nearPlane(nearPlane)
    , m_farPlane(farPlane)
{
}

Transform& Camera::GetTransform()
{
    return m_transform;
}

const Transform& Camera::GetTransform() const
{
    return m_transform;
}

float Camera::GetFieldOfView() const
{
    return m_fieldOfView;
}

float Camera::GetNearPlane() const
{
    return m_nearPlane;
}

float Camera::GetFarPlane() const
{
    return m_farPlane;
}

void Camera::SetFieldOfView(float fieldOfViewRadians)
{
    m_fieldOfView = fieldOfViewRadians;
}

void Camera::SetNearPlane(float nearPlane)
{
    m_nearPlane = nearPlane;
}

void Camera::SetFarPlane(float farPlane)
{
    m_farPlane = farPlane;
}

Vector3 Camera::GetForward() const
{
    // EN: The engine convention defines local +Z as forward.
    //
    // JP: Engine の座標規約ではローカル +Z を前方向として定義する。
    return m_transform.rotation.Rotate(
        Vector3{ 0.0f, 0.0f, 1.0f });
}

Vector3 Camera::GetRight() const
{
    // EN: The engine convention defines local +X as right.
    //
    // JP: Engine の座標規約ではローカル +X を右方向として定義する。
    return m_transform.rotation.Rotate(
        Vector3{ 1.0f, 0.0f, 0.0f });
}

Vector3 Camera::GetUp() const
{
    // EN: The engine convention defines local +Y as up.
    //
    // JP: Engine の座標規約ではローカル +Y を上方向として定義する。
    return m_transform.rotation.Rotate(
        Vector3{ 0.0f, 1.0f, 0.0f });
}