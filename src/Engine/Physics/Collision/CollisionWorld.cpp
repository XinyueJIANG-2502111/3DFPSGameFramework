#include "Engine/Physics/Collision/CollisionWorld.h"

#include "Engine/Physics/Collision/Collider.h"
#include "Engine/Physics/Collision/Intersection.h"
#include "Engine/Physics/Collision/CollisionHit.h"

#include <algorithm>

void CollisionWorld::Register(Collider& collider)
{
    // EN: Ignore duplicate registration of the same collider.
    //
    // JP: 同一 Collider の重複登録は無視する。
    const auto it =
        std::find(
            m_colliders.begin(),
            m_colliders.end(),
            &collider);

    if (it != m_colliders.end())
    {
        return;
    }

    m_colliders.push_back(&collider);
}

void CollisionWorld::Unregister(Collider& collider)
{
    const auto it =
        std::remove(
            m_colliders.begin(),
            m_colliders.end(),
            &collider);

    m_colliders.erase(
        it,
        m_colliders.end());
}

bool CollisionWorld::OverlapsAny(
    const Collider& collider) const
{
    for (const Collider* other : m_colliders)
    {
        // EN: A collider must not be tested against itself.
        //
        // JP: Collider 自身との交差判定は行わない。
        if (other == &collider)
        {
            continue;
        }

        if (Intersects(
            collider.GetBounds(),
            other->GetBounds()))
        {
            return true;
        }
    }

    return false;
}

bool CollisionWorld::ComputeCollision(
    const Collider& collider,
    CollisionHit& hit) const
{
    for (const Collider* other : m_colliders)
    {
        if (other == &collider)
        {
            continue;
        }

        Vector3 normal{};
        float penetration = 0.0f;

        if (ComputePenetration(
            collider.GetBounds(),
            other->GetBounds(),
            normal,
            penetration))
        {
            hit.other = other;
            hit.normal = normal;
            hit.penetration = penetration;

            return true;
        }
    }

    hit = CollisionHit{};
    return false;
}

void CollisionWorld::Clear()
{
    m_colliders.clear();
}