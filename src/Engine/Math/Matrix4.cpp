#include "Engine/Math/Matrix4.h"

Matrix4 Matrix4::Identity()
{
    Matrix4 result{};

    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;

    return result;
}

Matrix4 Matrix4::Translation(
    const Vector3& translation)
{
    Matrix4 result = Identity();

    result.m[0][3] = translation.x;
    result.m[1][3] = translation.y;
    result.m[2][3] = translation.z;

    return result;
}

Matrix4 Matrix4::Scale(
    const Vector3& scale)
{
    Matrix4 result = Identity();

    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;

    return result;
}

Matrix4 Matrix4::operator*(
    const Matrix4& rhs) const
{
    Matrix4 result{};

    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            for (int k = 0; k < 4; ++k)
            {
                result.m[row][column] +=
                    m[row][k] *
                    rhs.m[k][column];
            }
        }
    }

    return result;
}

Vector3 Matrix4::TransformPoint(
    const Vector3& point) const
{
    return Vector3{
        m[0][0] * point.x +
        m[0][1] * point.y +
        m[0][2] * point.z +
        m[0][3],

        m[1][0] * point.x +
        m[1][1] * point.y +
        m[1][2] * point.z +
        m[1][3],

        m[2][0] * point.x +
        m[2][1] * point.y +
        m[2][2] * point.z +
        m[2][3]
    };
}

Vector3 Matrix4::TransformDirection(
    const Vector3& direction) const
{
    return Vector3{
        m[0][0] * direction.x +
        m[0][1] * direction.y +
        m[0][2] * direction.z,

        m[1][0] * direction.x +
        m[1][1] * direction.y +
        m[1][2] * direction.z,

        m[2][0] * direction.x +
        m[2][1] * direction.y +
        m[2][2] * direction.z
    };
}

Matrix4 Matrix4::Rotation(
    const Quaternion& rotation)
{
    const Quaternion q =
        rotation.Normalized();

    const float xx = q.x * q.x;
    const float yy = q.y * q.y;
    const float zz = q.z * q.z;

    const float xy = q.x * q.y;
    const float xz = q.x * q.z;
    const float yz = q.y * q.z;

    const float wx = q.w * q.x;
    const float wy = q.w * q.y;
    const float wz = q.w * q.z;

    Matrix4 result = Identity();

    result.m[0][0] = 1.0f - 2.0f * (yy + zz);
    result.m[0][1] = 2.0f * (xy - wz);
    result.m[0][2] = 2.0f * (xz + wy);

    result.m[1][0] = 2.0f * (xy + wz);
    result.m[1][1] = 1.0f - 2.0f * (xx + zz);
    result.m[1][2] = 2.0f * (yz - wx);

    result.m[2][0] = 2.0f * (xz - wy);
    result.m[2][1] = 2.0f * (yz + wx);
    result.m[2][2] = 1.0f - 2.0f * (xx + yy);

    return result;
}