#pragma once

#include <vector>

class Collider;
struct CollisionHit;

class CollisionWorld
{
public:
    // EN: Registers a collider in this collision world.
    //     CollisionWorld does not own the collider.
    //
    // JP: Collider を CollisionWorld に登録する。
    //     CollisionWorld は Collider の所有権を持たない。
    void Register(Collider& collider);



    // EN: Removes a previously registered collider.
    //
    // JP: 登録済みの Collider を CollisionWorld から解除する。
    void Unregister(Collider& collider);



    // EN: Returns true when the specified collider overlaps
    //     any other registered collider.
    //
    // JP: 指定した Collider が他の登録済み Collider の
    //     いずれかと重なっている場合 true を返す。
    bool OverlapsAny(const Collider& collider) const;



    // EN: Finds the first registered collider overlapping the query
    //     collider and computes penetration information for it.
    //
    // JP: Query Collider と重なる最初の登録 Collider を検索し、
    //     その Penetration 情報を計算する。
    bool ComputeCollision(
        const Collider& collider,
        CollisionHit& hit) const;



    // EN: Removes all collider registrations without destroying
    //     the collider objects themselves.
    //
    // JP: Collider 自体を破棄せず、
    //     CollisionWorld 内の登録だけをすべて解除する。
    void Clear();

private:
    // EN: Non-owning collider pointers. The objects must outlive
    //     their registration in this world.
    //
    // JP: 所有権を持たない Collider ポインタ。
    //     登録中は Collider オブジェクトの寿命が
    //     CollisionWorld より有効でなければならない。
    std::vector<Collider*> m_colliders;
};