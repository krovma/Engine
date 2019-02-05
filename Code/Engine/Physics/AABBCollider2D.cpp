#include "Engine/Physics/AABBCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderContext.hpp"
////////////////////////////////
AABBCollider2D::AABBCollider2D(const AABB2& localShape, Rigidbody2D* rigidbody)
	:Collider2D(COLLIDER_AABB2, rigidbody), m_localShape(localShape)
{
}

////////////////////////////////
AABB2 AABBCollider2D::GetLocalShape() const
{
	return m_localShape;
}

////////////////////////////////
AABB2 AABBCollider2D::GetWorldShape() const
{
	return m_localShape + m_rigidbody->GetPosition();
}

////////////////////////////////
void AABBCollider2D::DebugRender(RenderContext* renderer, const Rgba& renderColor) const
{
	std::vector<Vertex_PCU> verts;
	AddVerticesOfAABB2D(verts, GetWorldShape(), renderColor);
	renderer->DrawVertexArray(verts.size(), verts);
}
