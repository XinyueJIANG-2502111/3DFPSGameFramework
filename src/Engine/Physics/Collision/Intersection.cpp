#include "Engine/Physics/Collision/Intersection.h"

bool Intersects(
    const AABB& a,
    const AABB& b)
{
    const bool overlapsX =
        a.max.x >= b.min.x &&
        a.min.x <= b.max.x;

    const bool overlapsY =
        a.max.y >= b.min.y &&
        a.min.y <= b.max.y;

    const bool overlapsZ =
        a.max.z >= b.min.z &&
        a.min.z <= b.max.z;

    return
        overlapsX &&
        overlapsY &&
        overlapsZ;
}

bool Contains(
    const AABB& bounds,
    const Vector3& point)
{
    return
        point.x >= bounds.min.x &&
        point.x <= bounds.max.x &&

        point.y >= bounds.min.y &&
        point.y <= bounds.max.y &&

        point.z >= bounds.min.z &&
        point.z <= bounds.max.z;
}

bool ComputePenetration(
    const AABB& a,
    const AABB& b,
    Vector3& normal,
    float& penetration)
{
    if (!Intersects(a, b))
    {
        normal = Vector3{};
        penetration = 0.0f;
        return false;
    }

    // EN: Compute the distance required to move A out of B
    //     in both directions of each world axis.
    //
    // JP: 各ワールド軸の正負両方向について、
    //     A を B から分離するために必要な距離を計算する。
    const float pushNegativeX =
        a.max.x - b.min.x;

    const float pushPositiveX =
        b.max.x - a.min.x;

    const float pushNegativeY =
        a.max.y - b.min.y;

    const float pushPositiveY =
        b.max.y - a.min.y;

    const float pushNegativeZ =
        a.max.z - b.min.z;

    const float pushPositiveZ =
        b.max.z - a.min.z;


    // EN: Start with the shortest X-axis correction.
    //
    // JP: まず X 軸上で最短となる補正方向を選択する。
    if (pushNegativeX < pushPositiveX)
    {
        normal = Vector3{ -1.0f, 0.0f, 0.0f };
        penetration = pushNegativeX;
    }
    else
    {
        normal = Vector3{ 1.0f, 0.0f, 0.0f };
        penetration = pushPositiveX;
    }


    // EN: Replace the current result if either Y direction
    //     provides a shorter separation distance.
    //
    // JP: Y 軸方向の分離距離が現在の結果より短い場合、
    //     Normal と Penetration を更新する。
    if (pushNegativeY < penetration)
    {
        normal = Vector3{ 0.0f, -1.0f, 0.0f };
        penetration = pushNegativeY;
    }

    if (pushPositiveY < penetration)
    {
        normal = Vector3{ 0.0f, 1.0f, 0.0f };
        penetration = pushPositiveY;
    }


    // EN: Perform the same minimum-distance comparison for Z.
    //
    // JP: Z 軸についても同様に最小分離距離を比較する。
    if (pushNegativeZ < penetration)
    {
        normal = Vector3{ 0.0f, 0.0f, -1.0f };
        penetration = pushNegativeZ;
    }

    if (pushPositiveZ < penetration)
    {
        normal = Vector3{ 0.0f, 0.0f, 1.0f };
        penetration = pushPositiveZ;
    }

    return true;
}