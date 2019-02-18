#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
////////////////////////////////
Rigidbody2D::Rigidbody2D(Transform2D* transform)
	: m_entityTransform(transform)
{
	UpdateFromTransform();
}

////////////////////////////////
void Rigidbody2D::Update(float deltaSeconds)
{
	if (m_simulationType == PHSX_SIM_DYNAMIC) {
		m_velocity += m_acceleration * deltaSeconds;
		m_position += m_velocity * deltaSeconds;
	}
}

////////////////////////////////
void Rigidbody2D::SetSimulationType(PhysicsSimulationType type)
{
	m_simulationType = type;
}

////////////////////////////////
void Rigidbody2D::UpdateToTransform() const
{
	m_entityTransform->Position = m_position;
	m_entityTransform->OrientationDegrees = m_orientationDegrees;
}

////////////////////////////////
void Rigidbody2D::UpdateFromTransform()
{
	m_position = m_entityTransform->Position;
	m_orientationDegrees = m_entityTransform->OrientationDegrees;
}

////////////////////////////////
bool Rigidbody2D::IsColliding() const
{
	return m_collider->m_inCollision;
}

////////////////////////////////
void Rigidbody2D::SetColliding(bool isColliding)
{
	m_collider->m_inCollision = isColliding;
}
