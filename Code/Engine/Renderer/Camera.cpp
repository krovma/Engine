#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/AABB2.hpp"
Camera::Camera()
{
}

Camera::Camera(const Vec2 &orthoMin, const Vec2 &orthomax)
	: m_orthoMin(orthoMin)
	, m_orthoMax(orthomax)
{
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetOrthoView(const Vec2 &orthoMin, const Vec2 &orthoMax)
{
	m_orthoMin = orthoMin;
	m_orthoMax = orthoMax;
}

//////////////////////////////////////////////////////////////////////////
Vec2 Camera::GetOrthoBottomLeft() const
{
	return Vec2(m_orthoMin);
}

//////////////////////////////////////////////////////////////////////////
Vec2 Camera::GetOrthoTopRight() const
{
	return Vec2(m_orthoMax);
}

////////////////////////////////
void Camera::Translate2D(const Vec2 &translate)
{
	m_orthoMin += translate;
	m_orthoMax += translate;
}

////////////////////////////////
void Camera::UpdateConstantBuffer(RenderContext* renderer)
{
	if (m_cameraUBO == nullptr) {
		m_cameraUBO = new ConstantBuffer(renderer);
	}
	AABB2 cameraData;
	cameraData.Min = m_orthoMin;
	cameraData.Max = m_orthoMax;
	m_cameraUBO->Buffer(&cameraData, sizeof(cameraData));
}

#endif // 