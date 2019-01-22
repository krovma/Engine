#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

//////////////////////////////////////////////////////////////////////////
Vertex_PCU::Vertex_PCU()
{
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCU::Vertex_PCU(const Vec3 &position, const Rgba &color, const Vec2 &uvTexCoords)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
{
}

const Vertex_PCU & Vertex_PCU::operator=(const Vertex_PCU &copyFrom)
{
	m_position = copyFrom.m_position;
	m_color = copyFrom.m_color;
	m_uvTexCoords = copyFrom.m_uvTexCoords;
	return *this;
}

