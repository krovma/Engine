#pragma once

class RenderContext;
class Rigidbody2D;
struct Collision2D;
struct Rgba;
//////////////////////////////////////////////////////////////////////////
enum Collider2DType
{
	COLLIDER_AABB2,
	COLLIDER_DISK2D,

	NUM_COLLIDER_2D_TYPE
};

class Collider2D
{
public:
	Collider2D(Collider2DType colliderType, const Rigidbody2D* rigidbody);
	virtual void DebugRender(RenderContext* renderer, const Rgba& renderColor) const;
	Collision2D GetCollisionWith(const Collider2D* other) const;

public:
	const Collider2DType m_type;
	const Rigidbody2D* m_rigidbody;
	bool m_inCollision = false;
};