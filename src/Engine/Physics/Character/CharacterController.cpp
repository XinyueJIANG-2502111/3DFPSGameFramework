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
    // EN: Apply the requested movement first. Collision penetration
    //     is resolved afterward using the MTV returned by
    //     CollisionWorld.
    //
    // JP: まず要求された移動を適用し、その後 CollisionWorld から
    //     得られる MTV を使用してめり込みを解消する。
    transform.position += displacement;

    collider.SetBounds(
        collider.GetBounds().Translated(
            displacement));

    CollisionHit hit{};

    if (!m_collisionWorld.ComputeCollision(
        collider,
        hit))
    {
        return;
    }

    // EN: The minimum translation vector moves the character
    //     out of the overlapping collider along the shortest
    //     separation direction.
    //
    // JP: Minimum Translation Vector により、最短の分離方向へ
    //     Character を移動させ、Collider の重なりを解消する。
    const Vector3 correction =
        hit.normal *
        hit.penetration;

    transform.position += correction;

    collider.SetBounds(
        collider.GetBounds().Translated(
            correction));
}