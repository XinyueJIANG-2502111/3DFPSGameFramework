#pragma once

#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Matrix4.h"

struct Transform
{
    // EN: World-space position of the object.
    //
    // JP: オブジェクトのワールド空間上の位置。
    Vector3 position{};

    // EN: World-space orientation stored as a unit quaternion.
    //     Identity represents an object with no rotation.
    //
    // JP: 単位四元数として保持するオブジェクトのワールド空間上の姿勢。
    //     単位四元数は回転していない状態を表す。
    Quaternion rotation{};

    // EN: Scale of the object along its local axes.
    //     A scale of (1, 1, 1) preserves the original size.
    //
    // JP: オブジェクトのローカル軸方向のスケール。
    //     (1, 1, 1) は元の大きさを維持する。
    Vector3 scale{ 1.0f };


    // EN: Builds the world matrix using the engine's TRS convention.
    //     With column-vector semantics, T * R * S applies
    //     Scale -> Rotation -> Translation.
    //
    // JP: Engine の TRS 規約に従って World Matrix を生成する。
    //     Column Vector 規約では T * R * S により、
    //     Scale -> Rotation -> Translation の順で適用される。
    Matrix4 ToMatrix() const
    {
        return
            Matrix4::Translation(position) *
            Matrix4::Rotation(rotation) *
            Matrix4::Scale(scale);
    }
};