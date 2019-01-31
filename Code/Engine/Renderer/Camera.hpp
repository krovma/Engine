#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/RenderTargetView.hpp"
//////////////////////////////////////////////////////////////////////////
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
	
	void SetRenderTarget(RenderTargetView* renderTarget) { m_renderTarget = renderTarget; }
	RenderTargetView* GetRenderTarget() const { return m_renderTarget; }
private:
	Vec2 _bottomLeft;
	Vec2 _topRight;
	
	RenderTargetView* m_renderTarget;
};
