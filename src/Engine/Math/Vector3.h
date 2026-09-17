#pragma once

#include <cmath>

struct Vector3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr Vector3() = default;

    constexpr Vector3(float x, float y, float z)
        : x(x), y(y), z(z){}

    explicit constexpr Vector3(float value)
        : x(value), y(value), z(value){}

    constexpr Vector3 operator+(const Vector3& rhs) const
    {
        return {
            x + rhs.x,
            y + rhs.y,
            z + rhs.z
        };
    }

    constexpr Vector3 operator-(const Vector3& rhs) const
    {
        return {
            x - rhs.x,
            y - rhs.y,
            z - rhs.z
        };
    }

    constexpr Vector3 operator-() const
    {
        return { -x, -y, -z };
    }

    constexpr Vector3 operator*(float scalar) const
    {
        return {
            x * scalar,
            y * scalar,
            z * scalar
        };
    }

    constexpr Vector3 operator/(float scalar) const
    {
        return {
            x / scalar,
            y / scalar,
            z / scalar
        };
    }

    constexpr Vector3& operator+=(const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;

        return *this;
    }

    constexpr Vector3& operator-=(const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;

        return *this;
    }

    float Length() const
    {
        return std::sqrt(LengthSquared());
    }

    constexpr float LengthSquared() const
    {
        return x * x + y * y + z * z;
    }
};

constexpr Vector3 operator*(float scalar, const Vector3& v)
{
    return v * scalar;
}

// EN: Dot product measures how strongly two vectors point in the
//     same direction. It is commonly used for angle and projection tests.
//
// JP: 内積は二つのベクトルがどの程度同じ方向を向いているかを表す。
//     主に角度判定や射影計算などで使用する。
constexpr float Dot(const Vector3& a, const Vector3& b)
{
    return
        a.x * b.x +
        a.y * b.y +
        a.z * b.z;
}

// EN: Cross product returns a vector perpendicular to both inputs.
//     Camera and coordinate-system calculations use this to derive
//     directions such as the right vector.
//
// JP: 外積は二つの入力ベクトルの両方に垂直なベクトルを返す。
//     Camera や座標系の計算では Right 方向などを求めるために使用する。
constexpr Vector3 Cross(const Vector3& a, const Vector3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline Vector3 Normalize(const Vector3& v)
{
    const float lengthSquared = v.LengthSquared();

    if (lengthSquared <= 0.0f)
    {
        return {};
    }

    const float inverseLength =
        1.0f / std::sqrt(lengthSquared);

    return v * inverseLength;
}

constexpr Vector3 Lerp(
    const Vector3& a,
    const Vector3& b,
    float t)
{
    return a + (b - a) * t;
}