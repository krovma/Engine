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
	const RenderBufferLayout* layout = mesh.GetLayout();
	GUARANTEE_OR_DIE(layout != nullptr, "No Layout desinated");
	int count = mesh.GetVertexCount();
	int sizeOfBuffer = layout->GetStride() * count;
	void* buffer = new char[sizeOfBuffer];

	layout->CopyFromVertexMaster(buffer, mesh.GetVertexBuffer(), count);

	m_vertexBuffer->CreateImmutable(buffer, count, layout);
	m_vertexBuffer->Buffer(buffer, count);
	m_indexBuffer->CreateImmutable(mesh.GetIndicesDataBuffer(), mesh.GetIndicesCount());
	m_indexBuffer->Buffer(mesh.GetIndicesDataBuffer(), mesh.GetIndicesCount());
	SetDrawCall(mesh.IsUsingIndexBuffer(), mesh.GetElementCount());
	m_layout = layout;

	delete[](char*)buffer;
}

////////////////////////////////
void GPUMesh::CopyFromCPUMesh(const CPUMesh& mesh, GPUMemoryUsage memoryUsage /*= GPU_MEMORY_USAGE_DYNAMIC*/)
{
	UNUSED(memoryUsage);
	const RenderBufferLayout* layout = mesh.GetLayout();
	GUARANTEE_OR_DIE(layout != nullptr, "No Layout desinated");
	int count = mesh.GetVertexCount();
	int sizeOfBuffer = layout->GetStride() * count;
	void* buffer = new char[sizeOfBuffer];

	m_vertexBuffer->SetLayout(layout);
	layout->CopyFromVertexMaster(buffer, mesh.GetVertexBuffer(), count);
	m_vertexBuffer->Buffer(buffer, count);
	m_indexBuffer->Buffer(mesh.GetIndicesDataBuffer(), mesh.GetIndicesCount());
	SetDrawCall(mesh.IsUsingIndexBuffer(), mesh.GetElementCount());

	delete[](char*)buffer;
}

////////////////////////////////
void GPUMesh::CopyVertexPCUArray(const Vertex_PCU* vertices, size_t count)
{
	m_vertexBuffer->Buffer(vertices, (int)count);
}

////////////////////////////////
void GPUMesh::SetDrawCall(bool usingIndexBuffer, int count)
{
	m_usingIndexBuffer = usingIndexBuffer;
	m_count = count;
}

