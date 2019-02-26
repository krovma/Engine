#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/RenderTargetView.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Math/Mat4.hpp"
//////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	struct __CameraConstantBuffer
	{
		Mat4 projection;
		//Mat4 camera;
		Mat4 view;
	};

	Camera();
	~Camera();
	explicit Camera(const Vec2 &orthoMin, const Vec2 &orthoMax, bool uiCamera=false);
public:
	void SetOrthoView(const Vec2 &orthoMin, const Vec2 &orthoMax, bool uiView=false);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	
	void Translate2D(const Vec2 &translate);
	
	void SetRenderTarget(RenderTargetView* renderTarget) { m_renderTarget = renderTarget; }
	RenderTargetView* GetRenderTarget() const { return m_renderTarget; }
	void UpdateConstantBuffer(RenderContext* renderer);
	ConstantBuffer* GetConstantBuffer() const { return m_cameraUBO; }

	void SetCameraModel(const Mat4& model);
private:
	Vec2 m_orthoMin;
	Vec2 m_orthoMax;
	Mat4 m_projection = Mat4::Identity;
	Mat4 m_camera = Mat4::Identity;
	Mat4 m_view = Mat4::Identity;
	//Mat4 m_model = Mat4::Identity;
	
	RenderTargetView* m_renderTarget = nullptr;
	ConstantBuffer* m_cameraUBO = nullptr;
};
