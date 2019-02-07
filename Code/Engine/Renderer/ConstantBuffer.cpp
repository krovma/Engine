#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
////////////////////////////////
ConstantBuffer::ConstantBuffer(RenderContext* renderer)
	: RenderBuffer(renderer)
{
	m_memoryUsage = GPU_MEMORY_USAGE_DYNAMIC;
	m_bufferUsage = RENDER_BUFFER_USAGE_CONSTANT;
}
////////////////////////////////
ConstantBuffer::~ConstantBuffer()
{
	//Do Nothing;
}
////////////////////////////////
bool ConstantBuffer::Buffer(const void* data, int size)
{
	if (size > m_bufferSize || IsImmutable()) {
		return RenderBuffer::Create(
			data, size, size
			, RENDER_BUFFER_USAGE_CONSTANT
			, GPU_MEMORY_USAGE_DYNAMIC
		);
	} else {
		return RenderBuffer::Buffer(data, size);
	}
}
