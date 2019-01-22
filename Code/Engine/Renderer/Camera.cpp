#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"

Camera::Camera()
{
}

Camera::Camera(const Vec2 &bottomLeft, const Vec2 &topRight)
	: _bottomLeft(bottomLeft)
	, _topRight(topRight)
{
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetOrthoView(const Vec2 &bottomLeft, const Vec2 &topRight)
{
	_bottomLeft = bottomLeft;
	_topRight = topRight;
}

//////////////////////////////////////////////////////////////////////////
Vec2 Camera::GetOrthoBottomLeft() const
{
	return Vec2(_bottomLeft);
}

//////////////////////////////////////////////////////////////////////////
Vec2 Camera::GetOrthoTopRight() const
{
	return Vec2(_topRight);
}

////////////////////////////////
void Camera::Translate2D(const Vec2 &translate)
{
	_bottomLeft += translate;
	_topRight += translate;
}

#endif // 