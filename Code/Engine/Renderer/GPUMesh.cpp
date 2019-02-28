#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>

////////////////////////////////
GPUMesh::GPUMesh(RenderContext* renderer)
{
	m_vertexBuffer = new VertexBuffer(renderer);
	m_indexBuffer = new IndexBuffer(renderer);
}

////////////////////////////////
GPUMesh::~GPUMesh()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
}

////////////////////////////////
void GPUMesh::CreateFromCPUMesh(const CPUMesh& mesh/*, GPUMemoryUsage memoryUsage = GPU_MEMORY_USAGE_IMMUTABLE*/)
{
	std::vector<Vertex_PCU> verts;
	int count = mesh.GetVertexCount();
	verts.reserve(count);
	for (int i = 0; i < count; ++i) {
		Vertex_PCU currentVert;
		VertexMaster meshVert = mesh.GetVertexByIndex(i);
		currentVert.m_position = meshVert.Position;
		currentVert.m_color = meshVert.Color;
		currentVert.m_uvTexCoords = meshVert.UV;
		verts.push_back(currentVert);
	}
	m_vertexBuffer->CreateImmutable(verts.data(), (int)verts.size());
	m_vertexBuffer->Buffer(verts.data(), (int)verts.size());
	m_indexBuffer->CreateImmutable(mesh.GetIndicesDataBuffer(), mesh.GetIndicesCount());
	m_indexBuffer->Buffer(mesh.GetIndicesDataBuffer(), mesh.GetIndicesCount());
	SetDrawCall(mesh.IsUsingIndexBuffer(), mesh.GetElementCount());
}

////////////////////////////////
void GPUMesh::SetDrawCall(bool usingIndexBuffer, int count)
{
	m_usingIndexBuffer = usingIndexBuffer;
	m_count = count;
}

