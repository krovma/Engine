#include "Engine/Physics/CapsuleCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
////////////////////////////////
CapsuleCollider2D::CapsuleCollider2D(const Capsule2& localShape, Rigidbody2D* rigidbody)
	:Collider2D(COLLIDER_CAPSULE2, rigidbody), m_localShape(localShape)
{
}

////////////////////////////////
Capsule2 CapsuleCollider2D::GetLocalShape() const
{
	return m_localShape;
}

////////////////////////////////
Capsule2 CapsuleCollider2D::GetWorldShape() const
{
	return m_localShape + m_rigidbody->GetPosition();
}

////////////////////////////////
void CapsuleCollider2D::DebugRender(RenderContext* renderer, const Rgba& renderColor) const
{
	std::vector<Vertex_PCU> verts;
	Capsule2 worldShape = GetWorldShape();
	CPUMesh mesh(RenderBufferLayout::AcquireLayoutFor<Vertex_PCU>());
	mesh.SetBrushColor(renderColor);
	mesh.AddDiskToMesh(worldShape.Start, worldShape.Radius, 32);
	mesh.AddDiskToMesh(worldShape.End, worldShape.Radius, 32);
	mesh.AddLine2DToMesh(worldShape.Start, worldShape.End, worldShape.Radius * 2.f);

	GPUMesh gpuMesh(renderer);
	gpuMesh.CreateFromCPUMesh(mesh);
	renderer->BindTextureViewWithSampler(0, nullptr);
	renderer->DrawMesh(gpuMesh);
}