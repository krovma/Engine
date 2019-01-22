#pragma once
#include "Engine/Math/Vec2.hpp"

class Camera
{
public:
	Camera();
	explicit Camera(const Vec2 &bottomLeft, const Vec2 &topRight);
public:
	void SetOrthoView(const Vec2 &bottomLeft, const Vec2 &topRight);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	
	void Translate2D(const Vec2 &translate);

private:
	Vec2 _bottomLeft;
	Vec2 _topRight;
};
