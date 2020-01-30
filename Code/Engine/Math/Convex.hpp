#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Core/EngineCommon.hpp"

class ConvexPoly
{
public:
	static ConvexPoly GetRandomPoly(float radius = 1.f);
public:
	std::vector<Vec2> m_points;
};

class ConvexHull2
{
public:
	std::vector<Plane2> m_edges;
};