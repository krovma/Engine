#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
////////////////////////////////
Collider2D::Collider2D(Collider2DType colliderType, const Rigidbody2D* rigidbody)
	:m_type(colliderType), m_rigidbody(rigidbody)
{
}

////////////////////////////////
void Collider2D::DebugRender(RenderContext* renderer, const Rgba& renderColor) const
{
	UNUSED(renderColor);
	UNUSED(renderer);
	// Do nothing
}

////////////////////////////////
Collision2D Collider2D::GetCollisionWith(const Collider2D* other) const
{
	Collision2D collision;
	GetCollision(collision, this, other);
	return collision;
}
