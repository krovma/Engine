#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderBufferLayout.hpp"

struct Vertex_PCUN
{
public:
	static BufferAttribute Layout[];
	static void VertexMasterCopyProc(void* dst, const VertexMaster* src, int count);
public:
	Vertex_PCUN();
	explicit Vertex_PCUN(const Vec3 &position, const Rgba &color, const Vec2 &uvTexCoords, const Vec3 &normal);
	const Vertex_PCUN &operator=(const Vertex_PCUN &copyFrom);
public:
	Vec3 m_position;
	Rgba m_color;
	Vec2 m_uvTexCoords;
	Vec3 m_normal=Vec3(0,0,1);
};