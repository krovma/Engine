#pragma once
#include "Engine/Math/Vec2.hpp"

struct Transform2D
{
public:
	Vec2 Position = Vec2::ZERO;
	float OrientationDegrees = 0.f;
	float UniformScale = 1.f;
};