#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
////////////////////////////////

////////////////////////////////
void CPUMesh::AddAABB2ToMesh(const AABB2& quad)
{
	SetBrushUV(Vec2(0.f, 0.f));
	int tl = AddVertex(Vec3(quad.GetTopLeft()));
	SetBrushUV(Vec2(1.f, 0.f));
	int tr = AddVertex(Vec3(quad.GetTopRight()));
	SetBrushUV(Vec2(0.f, 1.f));
	int bl = AddVertex(Vec3(quad.GetBottomLeft()));
	SetBrushUV(Vec2(1.f, 1.f));
	int br = AddVertex(Vec3(quad.GetBottomRight()));
	AddQuadByIndices(tl, tr, bl, br);
}

////////////////////////////////
void CPUMesh::AddCubeToMesh(const AABB3& box)
{
	Vec3 position[8];
	//int tL, tR, bL, bR;
	//  y3   2
	//  |0__x1
	//7/  6
	//4z  5
	// Winding order = CCW
	position[0].x = box.Min.x; position[0].y = box.Min.y; position[0].z = box.Min.z;
	position[1].x = box.Max.x; position[1].y = box.Min.y; position[1].z = box.Min.z;
	position[2].x = box.Max.x; position[2].y = box.Max.y; position[2].z = box.Min.z;
	position[3].x = box.Min.x; position[3].y = box.Max.y; position[3].z = box.Min.z;
	position[4].x = box.Min.x; position[4].y = box.Min.y; position[4].z = box.Max.z;
	position[5].x = box.Max.x; position[5].y = box.Min.y; position[5].z = box.Max.z;
	position[6].x = box.Max.x; position[6].y = box.Max.y; position[6].z = box.Max.z;
	position[7].x = box.Min.x; position[7].y = box.Max.y; position[7].z = box.Max.z;

	AddQuad3D(position[3], position[2], position[0], position[1]);// back
	AddQuad3D(position[3], position[7], position[0], position[4]);// left
	AddQuad3D(position[6], position[2], position[5], position[1]);// right
	AddQuad3D(position[3], position[2], position[7], position[6]);// top
	AddQuad3D(position[1], position[0], position[5], position[4]);// down
	AddQuad3D(position[7], position[6], position[4], position[5]);// front
}

////////////////////////////////
void CPUMesh::AddUVSphereToMesh(const Vec3& center, float radius, int longitude /*= 32*/, int latitude /*= 16*/)
{
	int uStep = longitude + 1;
	int vStep = latitude + 1;
	int* indices = new int[uStep * vStep];
	ResetBursh();
	for (int v = 0; v < vStep; ++v) {
		for (int u = 0; u < uStep; ++u) {
			Vec2 UV;
			UV.x = (float)u / (float)longitude;
			UV.y = (float)v / (float)latitude;
			float theta = 360.f * UV.x;
			float phi = FloatMap(180.f * UV.y, 0.f, 180.f, 90.f, -90.f);
			Vec3 position(
				radius * CosDegrees(phi) * CosDegrees(theta),
				radius * SinDegrees(phi),
				radius * CosDegrees(phi) * SinDegrees(theta)
			);
			SetBrushUV(UV);
			indices[v * uStep + u] = AddVertex(position + center);
		}
	}

	for (int v = 0; v < latitude; ++v) {
		for (int u = 0; u < longitude; ++u) {
			int tl = v * uStep + u;
			AddQuadByIndices(
				indices[tl],
				indices[tl + 1],
				indices[tl + uStep],
				indices[tl + uStep + 1]
			);
		}
	}

	delete[] indices;
}

////////////////////////////////
CPUMesh::CPUMesh(const CPUMesh& copyFrom)
	: m_vertices(copyFrom.m_vertices)
	, m_indices(copyFrom.m_indices)
	, m_brush(copyFrom.m_brush)
{
}

////////////////////////////////
void CPUMesh::Clear()
{
	m_vertices.clear();
	m_indices.clear();
	m_brush = VertexMaster();
}

////////////////////////////////
void CPUMesh::ClearIndices()
{
	m_indices.clear();
}

////////////////////////////////
void CPUMesh::ResetBursh()
{
	m_brush = VertexMaster();
}

////////////////////////////////
int CPUMesh::AddVertex(const Vec3& position)
{
	m_brush.Position = position;
	return AddVertex(m_brush);
}

////////////////////////////////
int CPUMesh::AddVertex(const VertexMaster& vertex)
{
	m_vertices.push_back(vertex);
	return (int)m_vertices.size() - 1;
}

////////////////////////////////
void CPUMesh::AddTriangleByIndices(int vert0, int vert1, int vert2)
{
	int size = (int)m_vertices.size();
	GUARANTEE_OR_DIE(
		(size > vert0) && (size > vert1) && (size > vert2),
		"Vertex index too large! [CPUMesh::AddIndexedTriangle]"
	);
	m_indices.push_back(vert0);
	m_indices.push_back(vert1);
	m_indices.push_back(vert2);
}

////////////////////////////////
void CPUMesh::AddQuad3D(const Vec3& topLeft, const Vec3& topRight, const Vec3& bottomLeft, const Vec3& bottomRight)
{
	SetBrushColor(Rgba::WHITE);
	SetBrushUV(Vec2(0.f, 0.f));
	int tl = AddVertex(topLeft);
	SetBrushUV(Vec2(1.f, 0.f));
	int tr = AddVertex(topRight);
	SetBrushUV(Vec2(0.f, 1.f));
	int bl = AddVertex(bottomLeft);
	SetBrushUV(Vec2(1.f, 1.f));
	int br = AddVertex(bottomRight);
	AddQuadByIndices(tl, tr, bl, br);
}

////////////////////////////////
void CPUMesh::AddQuadByIndices(int topLeft, int topRight, int bottomLeft, int bottomRight)
{
	AddTriangleByIndices(bottomLeft, topRight, topLeft);
	AddTriangleByIndices(bottomLeft, bottomRight, topRight);
}

////////////////////////////////
void CPUMesh::SetVertexUVByIndex(int index, const Vec2& uv)
{
	if (index >= m_vertices.size()) {
		ERROR_AND_DIE("index too larget [CPUMesh::SetVertexUV]");
	}
	m_vertices[index].UV = uv;
}

////////////////////////////////
void CPUMesh::SetVertexColorByIndex(int index, const Rgba& color)
{
	if (index >= m_vertices.size()) {
		ERROR_AND_DIE("index too larget [CPUMesh::SetVertexUV]");
	}
	m_vertices[index].Color = color;
}

