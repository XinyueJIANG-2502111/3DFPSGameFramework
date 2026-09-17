#pragma once

#include "Engine/Math/Vector3.h"

#include <cmath>

struct Quaternion
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    // EN: The default quaternion represents the identity rotation.
    //     Unlike a zero vector, a zero quaternion is not a valid
    //     rotation quaternion.
    //
    // JP: デフォルトの四元数は単位回転を表す。
    //     ゼロベクトルとは異なり、ゼロ四元数は有効な
    //     回転四元数ではない。
    constexpr Quaternion() = default;

    constexpr Quaternion(
        float x,
        float y,
        float z,
        float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    // EN: Returns the squared magnitude of this quaternion.
    //     This avoids a square root when only a magnitude comparison
    //     is required.
    //
    // JP: この四元数の長さの二乗を返す。
    //     長さの比較だけが必要な場合、平方根の計算を避けられる。
    constexpr float LengthSquared() const
    {
        return
            x * x +
            y * y +
            z * z +
            w * w;
    }

    float Length() const
    {
        return std::sqrt(LengthSquared());
    }

    // EN: Returns a unit-length copy of this quaternion.
    //     A near-zero quaternion cannot represent a valid rotation,
    //     so identity is returned as a safe fallback.
    //
    // JP: この四元数を単位長に正規化したコピーを返す。
    //     ほぼゼロの四元数は有効な回転を表現できないため、
    //     安全なフォールバックとして単位四元数を返す。
    Quaternion Normalized() const
    {
        constexpr float epsilon = 1.0e-6f;

        const float lengthSquared = LengthSquared();

        if (lengthSquared <= epsilon * epsilon)
        {
            return {};
        }

        const float inverseLength =
            1.0f / std::sqrt(lengthSquared);

        return {
            x * inverseLength,
            y * inverseLength,
            z * inverseLength,
            w * inverseLength
        };
    }

    // EN: Returns the quaternion conjugate by negating the vector part
    //     while preserving the scalar part.
    //     For a unit quaternion, the conjugate is also its inverse.
    //
    // JP: ベクトル部の符号を反転し、スカラー部を維持した
    //     共役四元数を返す。
    //     単位四元数の場合、共役は逆四元数と等しくなる。
    constexpr Quaternion Conjugate() const
    {
        return {
            -x,
            -y,
            -z,
             w
        };
    }

    // EN: Creates a rotation quaternion from an axis and an angle.
    //     The axis is normalized internally so callers do not need
    //     to provide a unit vector.
    //
    // JP: 回転軸と角度から回転四元数を生成する。
    //     回転軸は内部で正規化するため、呼び出し側が
    //     単位ベクトルを渡す必要はない。
    static Quaternion FromAxisAngle(
        const Vector3& axis,
        float angleRadians)
    {
        const Vector3 normalizedAxis = Normalize(axis);

        // EN: A zero-length axis does not define a rotation axis.
        //     Return identity instead of constructing an invalid rotation.
        //
        // JP: 長さ 0 の軸では回転軸を定義できない。
        //     無効な回転を生成せず、単位四元数を返す。
        if (normalizedAxis.LengthSquared() <= 0.0f)
        {
            return {};
        }

        const float halfAngle =
            angleRadians * 0.5f;

        const float sinHalf =
            std::sin(halfAngle);

        const float cosHalf =
            std::cos(halfAngle);

        return {
            normalizedAxis.x * sinHalf,
            normalizedAxis.y * sinHalf,
            normalizedAxis.z * sinHalf,
            cosHalf
        };
    }

    // EN: Hamilton product used to compose quaternion rotations.
    //     Quaternion multiplication is not commutative, so changing
    //     the operand order can produce a different orientation.
    //
    // JP: 四元数の回転合成に使用する Hamilton 積。
    //     四元数の乗算には交換法則が成立しないため、
    //     オペランドの順序を変えると異なる姿勢になる場合がある。
    constexpr Quaternion operator*(
        const Quaternion& rhs) const
    {
        return {
            w * rhs.x + x * rhs.w
                + y * rhs.z - z * rhs.y,

            w * rhs.y - x * rhs.z
                + y * rhs.w + z * rhs.x,

            w * rhs.z + x * rhs.y
                - y * rhs.x + z * rhs.w,

            w * rhs.w - x * rhs.x
                - y * rhs.y - z * rhs.z
        };
    }

    // EN: Rotates a 3D vector using q * v * q^-1.
    //     Rotation quaternions are expected to remain normalized,
    //     allowing the conjugate to be used as the inverse.
    //
    // JP: q * v * q^-1 により 3D ベクトルを回転させる。
    //     回転四元数は正規化された状態を維持する前提のため、
    //     共役四元数を逆四元数として使用する。
    Vector3 Rotate(const Vector3& vector) const
    {
        const Quaternion pureVector{
            vector.x,
            vector.y,
            vector.z,
            0.0f
        };

        const Quaternion result =
            (*this) * pureVector * Conjugate();

        return {
            result.x,
            result.y,
            result.z
        };
    }
};