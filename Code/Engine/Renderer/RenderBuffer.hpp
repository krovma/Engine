#pragma once
//////////////////////////////////////////////////////////////////////////
class RenderContext;
struct ID3D11Buffer;
struct Vertex_PCU;
enum D3D11_USAGE : int;
//////////////////////////////////////////////////////////////////////////

enum RenderBufferUsage : unsigned int
{
	RENDER_BUFFER_USAGE_VERTEX = 0b1u,
	RENDER_BUFFER_USAGE_INDEX = 0b10u,
	RENDER_BUFFER_USAGE_CONSTANT = 0b100u,
};

enum GPUMemoryUsage
{
	GPU_MEMORY_USAGE_GPU,
	GPU_MEMORY_USAGE_IMMUTABLE,
	GPU_MEMORY_USAGE_DYNAMIC,
	GPU_MEMORY_USAGE_STAGING,
};

D3D11_USAGE GetD3DUsageFromGPUMemoryUsage(GPUMemoryUsage usage);
unsigned int GetD3DBufferUsageFromRenderBufferUsage(RenderBufferUsage usage);

//////////////////////////////////////////////////////////////////////////
class RenderBuffer
{
public:
	RenderBuffer(RenderContext* renderer);
	~RenderBuffer();

	int GetSize() const { return m_bufferSize; }
	bool IsImmutable() const { return (m_memoryUsage == GPU_MEMORY_USAGE_IMMUTABLE); }
	bool IsDynamic() const { return (m_memoryUsage == GPU_MEMORY_USAGE_DYNAMIC); }
	ID3D11Buffer* GetHandle() { return m_handle; }
protected:
	bool Create(
		const void* initialData
		, int bufferSize
		, int elementSize
		, RenderBufferUsage bufferUsage
		, GPUMemoryUsage memoryUsage);
	virtual bool Buffer(const void* data, int size);

protected:
	RenderContext* m_renderer = nullptr;
	RenderBufferUsage m_bufferUsage;
	GPUMemoryUsage m_memoryUsage;
	int m_bufferSize = 0;
	int m_elementSize = 0;
	ID3D11Buffer* m_handle = nullptr;
};

//////////////////////////////////////////////////////////////////////////
class ConstantBuffer : public RenderBuffer
{
public:
	ConstantBuffer(RenderContext* renderer);
	~ConstantBuffer();
	virtual bool Buffer(const void* data, int size) override;
};

//////////////////////////////////////////////////////////////////////////
class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer(RenderContext* renderer);
	~VertexBuffer();
	bool CreateImmutable(const Vertex_PCU* data, int count);
	virtual bool Buffer(const Vertex_PCU* data, int count);
	int GetNumVertices() const { return m_numVertices; }
private:
	int m_numVertices;
};
//////////////////////////////////////////////////////////////////////////