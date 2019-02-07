#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/RenderTargetView.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
//////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	Camera();
	~Camera();
	explicit Camera(const Vec2 &orthoMin, const Vec2 &orthoMax);
public:
	void SetOrthoView(const Vec2 &orthoMin, const Vec2 &orthoMax);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	
	void Translate2D(const Vec2 &translate);
	
	void SetRenderTarget(RenderTargetView* renderTarget) { m_renderTarget = renderTarget; }
	RenderTargetView* GetRenderTarget() const { return m_renderTarget; }
	void UpdateConstantBuffer(RenderContext* renderer);
	ConstantBuffer* GetConstantBuffer() const { return m_cameraUBO; }
private:
	Vec2 m_orthoMin;
	Vec2 m_orthoMax;
	
	RenderTargetView* m_renderTarget = nullptr;
	ConstantBuffer* m_cameraUBO = nullptr;
};
