#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Quaternion.h"

struct Matrix4
{
    float m[4][4]{};

    // EN: Creates an identity matrix that performs no transformation.
    //
    // JP: 変換を行わない単位行列を生成する。
    static Matrix4 Identity();

    // EN: Creates a translation matrix from a world-space offset.
    //
    // JP: World Space の移動量から平行移動行列を生成する。
    static Matrix4 Translation(const Vector3& translation);

    // EN: Creates a non-uniform scale matrix.
    //
    // JP: 各軸に異なる倍率を適用できる Scale 行列を生成する。
    static Matrix4 Scale(const Vector3& scale);

    // EN: Creates a rotation matrix equivalent to the given quaternion.
    //
    // JP: 指定された Quaternion と同じ回転を表す行列を生成する。
    static Matrix4 Rotation(const Quaternion& rotation);

    Matrix4 operator*(const Matrix4& rhs) const;

    // EN: Transforms a position using this matrix, including translation.
    //
    // JP: 平行移動を含め、この行列を使用して Position を変換する。
    Vector3 TransformPoint(const Vector3& point) const;

    // EN: Transforms a direction using this matrix without translation.
    //
    // JP: 平行移動を適用せず、この行列を使用して Direction を変換する。
    Vector3 TransformDirection(const Vector3& direction) const;
};