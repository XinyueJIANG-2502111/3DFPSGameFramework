#include "Platform/DxLib/DxCamera.h"

#include "Engine/Rendering/Camera.h"

#include <DxLib.h>

namespace
{
    // EN: Converts the engine's platform-independent Vector3 into
    //     DxLib's VECTOR type. The conversion stays inside the DxLib
    //     backend so the Math layer does not depend on DxLib.
    //
    // JP: Engine のプラットフォーム非依存な Vector3 を
    //     DxLib の VECTOR 型へ変換する。
    //     Math レイヤーが DxLib に依存しないよう、この変換処理は
    //     DxLib バックエンド内部に閉じ込める。
    VECTOR ToDxVector(const Vector3& vector)
    {
        return VGet(
            vector.x,
            vector.y,
            vector.z);
    }
}

void DxCamera::Apply(const Camera& camera)
{
    const Transform& transform =
        camera.GetTransform();

    const Vector3 position =
        transform.position;

    const Vector3 forward =
        camera.GetForward();

    const Vector3 up =
        camera.GetUp();

    // EN: DxLib expects a camera position and a look-at target.
    //     Engine Camera stores position and orientation instead,
    //     so the target is derived from position + forward.
    //
    // JP: DxLib は Camera の位置と注視点を要求する。
    //     Engine 側の Camera は位置と姿勢を保持しているため、
    //     Position + Forward から注視点を導出する。
    const Vector3 target =
        position + forward;

    const VECTOR dxPosition =
        ToDxVector(position);

    const VECTOR dxTarget =
        ToDxVector(target);

    const VECTOR dxUp =
        ToDxVector(up);

    SetCameraPositionAndTargetAndUpVec(
        dxPosition,
        dxTarget,
        dxUp);

    SetupCamera_Perspective(
        camera.GetFieldOfView());

    SetCameraNearFar(
        camera.GetNearPlane(),
        camera.GetFarPlane());
}