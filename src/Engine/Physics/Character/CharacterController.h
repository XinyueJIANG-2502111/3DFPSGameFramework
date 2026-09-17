#pragma once

#include "Engine/Math/Vector3.h"

class Collider;
class CollisionWorld;
struct Transform;

class CharacterController
{
public:
    explicit CharacterController(
        CollisionWorld& collisionWorld);

    // EN: Moves the character by the requested displacement and
    //     resolves AABB penetration against the collision world.
    //
    // JP: —v‹‚³‚ê‚½ˆÚ“®—Ê‚¾‚¯ Character ‚ğˆÚ“®‚µA
    //     CollisionWorld ‚É‘Î‚·‚é AABB ‚Ì‚ß‚è‚İ‚ğ‰ğÁ‚·‚éB
    void Move(
        Transform& transform,
        Collider& collider,
        const Vector3& displacement);

private:
    CollisionWorld& m_collisionWorld;
};