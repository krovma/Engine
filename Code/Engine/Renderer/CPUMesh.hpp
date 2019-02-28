#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include <vector>

struct VertexMaster
{
	Vec3 Position=Vec3::ZERO;
	Rgba Color=Rgba::WHITE;
	Vec2 UV=Vec2::ZERO;

	VertexMaster() = default;
	VertexMaster(const Vec3 &position, const Rgba& color, const Vec2& uv)
		: Position(position), Color(color), UV(uv)
	{}
};

class CPUMesh
{
public:
public:
	CPUMesh()=default;
	CPUMesh(const CPUMesh& copyFrom);
	void Clear();
	void ClearIndices();

	void ResetBursh();
	void SetBrushColor(const Rgba& color) { m_brush.Color = color; }
	void SetBrushUV(const Vec2& uv) { m_brush.UV = uv; };
	int AddVertex(const VertexMaster& vertex);
	int AddVertex(const Vec3& position);
	void SetVertexUVByIndex(int index, const Vec2& uv);
	void SetVertexColorByIndex(int index, const Rgba& color);

	//void AddTriangle3D(const Vec3& vert0, const Vec3& vert1, const Vec3& vert2);
	void AddAABB2ToMesh(const AABB2& quad);
	void AddCubeToMesh(const AABB3& box);
	void AddUVSphereToMesh(const Vec3& center, float radius, int longitude = 32, int latitude = 16);
	void AddTriangleByIndices(int vert0, int vert1, int vert2);
	void AddQuad3D(const Vec3& topLeft, const Vec3& topRight, const Vec3& bottomLeft, const Vec3& bottomRight);
	void AddQuadByIndices(int topLeft, int topRight, int bottomLeft, int bottomRight);

	int GetVertexCount() const { return (int)m_vertices.size(); }
	int GetIndicesCount() const { return (int)m_indices.size();  }
	VertexMaster GetVertexByIndex(int index) const { return m_vertices[index]; }
	int GetIndexByIndex(int index) const { return m_indices[index]; }
	const int* GetIndicesDataBuffer() const { return m_indices.data(); }
	
	bool IsUsingIndexBuffer() const { return GetIndicesCount() > 0; }
	int GetElementCount() const { return IsUsingIndexBuffer() ? GetIndicesCount() : GetVertexCount(); }
	
private:
	std::vector<VertexMaster> m_vertices;
	std::vector<int> m_indices;
	VertexMaster m_brush;
};