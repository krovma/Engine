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
/// All manifold generating function assume the Colliders do collide ///
//////////////////////////////////////////////////////////////////////////
bool _SetManifold(Manifold2D& out_manifold, const AABB2& a, const AABB2& b)
{
	AABB2 manifold;
	manifold.Min.x = std::max(a.Min.x, b.Min.x);
	manifold.Min.y = std::max(a.Min.y, b.Min.y);
	manifold.Max.x = std::min(a.Max.x, b.Max.x);
	manifold.Max.y = std::min(a.Max.y, b.Max.y);
	
	if (manifold.Max.AllComponentGreatThan(manifold.Min)) {
		float width = manifold.GetWidth();
		float height = manifold.GetHeight();
		Vec2 displacement = a.GetCenter() - b.GetCenter();
		if (width < height) {
			float dir = (float)Sgn(Vec2(1.f, 0.f).DotProduct(displacement));
			out_manifold.normal = Vec2(dir, 0.f).GetNormalized();
			out_manifold.penetration = width;
		} else {
			float dir = (float)Sgn(Vec2(0.f, 1.f).DotProduct(displacement));
			out_manifold.normal = Vec2(0.f, dir).GetNormalized();
			out_manifold.penetration = height;
		}
		return true;
	} else {
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////
bool _SetManifold(Manifold2D& out_manifold, const AABB2& a, const Vec2& center, float radius)
{
	Vec2 nearestPointOnAABB = GetNearestPointOnAABB2(center, a);
	float distance2 = GetDistanceSquare(center, nearestPointOnAABB);
	if (distance2 < radius * radius) {
		out_manifold.normal = (nearestPointOnAABB - center).GetNormalized();
		out_manifold.penetration = radius - sqrtf(distance2);
		return true;
	} else {
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////
bool _SetManifold(Manifold2D& out_manifold, const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB)
{
	Vec2 dispAB = centerA - centerB;
	float distance2 = dispAB.GetLengthSquare();
	if (distance2 < (radiusB + radiusA) * (radiusB + radiusA)) {
		out_manifold.normal = dispAB.GetNormalized();
		out_manifold.penetration = (radiusB + radiusA) - sqrtf(distance2);
		return true;
	} else {
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////
bool _Collide_AABB2_AABB2(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	AABBCollider2D* colliderA = (AABBCollider2D*)a;
	AABBCollider2D* colliderB = (AABBCollider2D*)b;
	AABB2 boxA = colliderA->GetWorldShape();
	AABB2 boxB = colliderB->GetWorldShape();

	out_collision.isCollide = _SetManifold(out_collision.manifold, boxA, boxB);
	out_collision.which = a;
	out_collision.collideWith = b;
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
	out_collision.isCollide = _SetManifold(out_collision.manifold, boxA, centerB, radiusB);
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_Disk_AABB2(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	DiskCollider2D* colliderA = (DiskCollider2D*)a;
	AABBCollider2D* colliderB = (AABBCollider2D*)b;
	Vec2 centerA = colliderA->m_rigidbody->GetPosition();
	float radiusA = colliderA->GetRadius();
	AABB2 boxB = colliderB->GetWorldShape();
	out_collision.isCollide = _SetManifold(out_collision.manifold, boxB, centerA, radiusA);
	out_collision.manifold.normal *= -1;
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
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
	out_collision.isCollide = _SetManifold(out_collision.manifold, centerA, radiusA, centerB, radiusB);
	out_collision.which = a;
	out_collision.collideWith = b;
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