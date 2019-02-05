#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/AABBCollider2D.hpp"
#include "Engine/Physics/DiskCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <algorithm>
//////////////////////////////////////////////////////////////////////////
using CollideCheck2DFunction = bool(Collision2D&, const Collider2D*, const Collider2D*);

//////////////////////////////////////////////////////////////////////////
bool _Collide_AABB2_AABB2(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	AABBCollider2D* colliderA = (AABBCollider2D*)a;
	AABBCollider2D* colliderB = (AABBCollider2D*)b;
	AABB2 boxA = colliderA->GetWorldShape();
	AABB2 boxB = colliderB->GetWorldShape();
	AABB2 manifold;
	manifold.Min.x = std::max(boxA.Min.x, boxB.Min.x);
	manifold.Min.y = std::max(boxA.Min.y, boxB.Min.y);
	manifold.Max.x = std::min(boxA.Max.x, boxB.Max.x);
	manifold.Max.y = std::min(boxA.Max.y, boxB.Max.y);
	if (manifold.Max.AllComponentGreatThan(manifold.Min)) {
		out_collision.isCollide = true;
	} else {
		out_collision.isCollide = false;
	}
	return out_collision.isCollide;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_AABB2_Disk(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	AABBCollider2D* colliderA = (AABBCollider2D*)a;
	DiskCollider2D* colliderB = (DiskCollider2D*)b;
	AABB2 boxA = colliderA->GetWorldShape();
	Vec2 centerB = colliderB->m_rigidbody->GetPosition();
	float radiusB = colliderB->GetRadius();
	Vec2 nearestPointOnAABB = GetNearestPointOnAABB2(centerB, boxA);
	float distance2 = GetDistanceSquare(centerB, nearestPointOnAABB);
	if (distance2 < radiusB * radiusB) {
		out_collision.isCollide = true;
	} else {
		out_collision.isCollide = false;
	}
	return out_collision.isCollide;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_Disk_AABB2(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	return _Collide_AABB2_Disk(out_collision, b, a);
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_Disk_Disk(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	DiskCollider2D* colliderA = (DiskCollider2D*)a;
	DiskCollider2D* colliderB = (DiskCollider2D*)b;
	Vec2 centerA = colliderA->m_rigidbody->GetPosition();
	float radiusA = colliderA->GetRadius();
	Vec2 centerB = colliderB->m_rigidbody->GetPosition();
	float radiusB = colliderB->GetRadius();
	Vec2 dispAB = centerA - centerB;
	if (dispAB.GetLengthSquare() < (radiusB + radiusA) * (radiusB + radiusA)) {
		out_collision.isCollide = true;
	} else {
		out_collision.isCollide = false;
	}
	return out_collision.isCollide;
}


////////////////////////////////
bool GetCollision(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	static CollideCheck2DFunction* collideFunctions[NUM_COLLIDER_2D_TYPE][NUM_COLLIDER_2D_TYPE] = {
		//				AABB2		, Disk		,
		/*AABB2*/_Collide_AABB2_AABB2, _Collide_AABB2_Disk,
		/*Disk*/_Collide_Disk_AABB2,	_Collide_Disk_Disk,
	};
	CollideCheck2DFunction* doCollide = collideFunctions[(int)(a->m_type)][(int)(b->m_type)];
	return doCollide(out_collision, a, b);
}