#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCUN.hpp"
#include "Engine/Renderer/CPUMesh.hpp"

STATIC BufferAttribute Vertex_PCUN::Layout[] = {
	BufferAttribute("POSITION", RBD_FLOAT3, offsetof(Vertex_PCUN, m_position)),
	BufferAttribute("COLOR", RBD_RGBA, offsetof(Vertex_PCUN, m_color)),
	BufferAttribute("TEXCOORD", RBD_FLOAT2, offsetof(Vertex_PCUN, m_uvTexCoords)),
	BufferAttribute("NORMAL", RBD_FLOAT3, offsetof(Vertex_PCUN, m_normal)),
	BufferAttribute::END
};

////////////////////////////////
STATIC void Vertex_PCUN::VertexMasterCopyProc(void* dst, const VertexMaster* src, int count)
{
	Vertex_PCUN* buffer = (Vertex_PCUN*)dst;
	for (int i = 0; i < count; ++i) {
		buffer[i].m_position = src[i].Position;
		buffer[i].m_color = src[i].Color;
		buffer[i].m_uvTexCoords = src[i].UV;
		buffer[i].m_normal = src[i].Normal;
	}
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCUN::Vertex_PCUN()
{
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCUN::Vertex_PCUN(const Vec3 &position, const Rgba &color, const Vec2 &uvTexCoords, const Vec3& normal)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
	, m_normal(normal)
{
}

const Vertex_PCUN & Vertex_PCUN::operator=(const Vertex_PCUN &copyFrom)
{
	m_position = copyFrom.m_position;
	m_color = copyFrom.m_color;
	m_uvTexCoords = copyFrom.m_uvTexCoords;
	m_normal = copyFrom.m_normal;
	return *this;
}

