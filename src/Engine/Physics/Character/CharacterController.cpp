#include "Engine/Physics/Character/CharacterController.h"

#include "Engine/Math/Transform.h"
#include "Engine/Physics/Collision/Collider.h"
#include "Engine/Physics/Collision/CollisionHit.h"
#include "Engine/Physics/Collision/CollisionWorld.h"

CharacterController::CharacterController(
    CollisionWorld& collisionWorld)
    : m_collisionWorld(collisionWorld)
{
}

void CharacterController::Move(
    Transform& transform,
    Collider& collider,
    const Vector3& displacement)
{
    // EN: Apply the requested movement to both the character Transform
    //     and its world-space collider before resolving penetration.
    //
    // JP: Penetration を解消する前に、要求された移動量を
    //     Character の Transform とワールド空間 Collider の両方へ適用する。
    transform.position += displacement;

    collider.SetBounds(
        collider.GetBounds().Translated(
            displacement));

    // EN: Resolve a limited number of overlapping colliders.
    //     A fixed iteration budget prevents malformed geometry or
    //     floating-point error from causing an infinite loop.
    //
    // JP: 複数 Collider との重なりを有限回だけ解消する。
    //     固定の反復回数を設けることで、不正な Geometry や
    //     浮動小数点誤差による無限ループを防止する。
    constexpr int maxIterations = 4;

    for (int iteration = 0;
        iteration < maxIterations;
        ++iteration)
    {
        CollisionHit hit{};

        if (!m_collisionWorld.ComputeCollision(
            collider,
            hit))
        {
            break;
        }

        // EN: Contact without penetration requires no positional
        //     correction. This can occur because touching AABBs are
        //     intentionally considered intersecting.
        //
        // JP: Penetration を伴わない接触では位置補正は不要。
        //     現在の AABB 判定では境界接触も Intersection として
        //     扱うため、この状態が発生する可能性がある。
        /*if (hit.penetration <= 0.0f)
        {
            break;
        }*/

        const Vector3 correction =
            hit.normal *
            hit.penetration;

        transform.position += correction;

        collider.SetBounds(
            collider.GetBounds().Translated(
                correction));
    }
}

//void CharacterController::Move(
//    Transform& transform,
//    Collider& collider,
//    const Vector3& displacement)
//{
//    // EN: Apply the requested movement first. Collision penetration
//    //     is resolved afterward using the MTV returned by
//    //     CollisionWorld.
//    //
//    // JP: まず要求された移動を適用し、その後 CollisionWorld から
//    //     得られる MTV を使用してめり込みを解消する。
//    transform.position += displacement;
//
//    collider.SetBounds(
//        collider.GetBounds().Translated(
//            displacement));
//
//    CollisionHit hit{};
//
//    if (!m_collisionWorld.ComputeCollision(
//        collider,
//        hit))
//    {
//        return;
//    }
//
//    // EN: The minimum translation vector moves the character
//    //     out of the overlapping collider along the shortest
//    //     separation direction.
//    //
//    // JP: Minimum Translation Vector により、最短の分離方向へ
//    //     Character を移動させ、Collider の重なりを解消する。
//    const Vector3 correction =
//        hit.normal *
//        hit.penetration;
//
//    transform.position += correction;
//
//    collider.SetBounds(
//        collider.GetBounds().Translated(
//            correction));
//}