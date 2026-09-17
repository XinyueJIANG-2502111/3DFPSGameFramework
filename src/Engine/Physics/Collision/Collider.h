#pragma once

#include "Engine/Physics/Collision/AABB.h"

class Collider
{
public:
    Collider() = default;

    explicit Collider(const AABB& bounds)
        : m_bounds(bounds)
    {
    }

    // EN: Returns the world-space bounds currently represented
    //     by this collider.
    //
    // JP: この Collider が現在表している
    //     ワールド空間上の Bounds を返す。
    const AABB& GetBounds() const
    {
        return m_bounds;
    }

    // EN: Updates the world-space bounds of this collider.
    //     Transform synchronization is intentionally handled outside
    //     the collider at this stage.
    //
    // JP: Collider のワールド空間上の Bounds を更新する。
    //     現段階では Transform との同期処理を
    //     Collider 自身には持たせない。
    void SetBounds(const AABB& bounds)
    {
        m_bounds = bounds;
    }

private:
    AABB m_bounds{};
};