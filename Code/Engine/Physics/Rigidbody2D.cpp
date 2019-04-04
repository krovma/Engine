#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/OBBCollider2D.hpp"
#include "Engine/Physics/CapsuleCollider2D.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
////////////////////////////////
Rigidbody2D::Rigidbody2D(Transform2D* transform)
	: m_entityTransform(transform)
{
	UpdateFromTransform();
}

////////////////////////////////
Rigidbody2D::~Rigidbody2D()
{
	delete m_collider;
}

////////////////////////////////
void Rigidbody2D::Update(float deltaSeconds)
{
	if (m_simulationType == PHSX_SIM_DYNAMIC) {
		m_velocity += m_acceleration * deltaSeconds;
		m_position += m_velocity * deltaSeconds;
		m_angularVelocity += m_angularAcceleration * deltaSeconds;
		m_rotationDegrees += m_angularVelocity * deltaSeconds;
	}
}

////////////////////////////////
void Rigidbody2D::SetSimulationType(PhysicsSimulationType type)
{
	m_simulationType = type;
}

////////////////////////////////
void Rigidbody2D::SetCollider(Collider2D* collider)
{
	m_collider = collider;
	_UpdateInertia();
}

////////////////////////////////
void Rigidbody2D::UpdateToTransform() const
{
	m_entityTransform->Position = m_position;
	m_entityTransform->RotationDegrees = m_rotationDegrees;
}

////////////////////////////////
void Rigidbody2D::UpdateFromTransform()
{
	m_position = m_entityTransform->Position;
	m_rotationDegrees = m_entityTransform->RotationDegrees;
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

////////////////////////////////
void Rigidbody2D::SetMassKg(float massKg)
{
	m_massKg = massKg;
	_UpdateInertia();
}

////////////////////////////////
void Rigidbody2D::AddLinearForce(const Vec2& forceN)
{
	m_acceleration += forceN / m_massKg;
}

////////////////////////////////
void Rigidbody2D::AddTorque(float torque)
{
	m_angularAcceleration += ConvertRadiansToDegrees(torque / m_rotaionalInertia);
}

////////////////////////////////
void Rigidbody2D::AddForceAt(const Vec2& forceN, const Vec2& position)
{
	Vec2 disp = position - GetPosition();
	disp.Rotate90Degrees();
	float angularImpulse = forceN.DotProduct(disp);
	AddLinearForce(forceN);
	AddTorque(angularImpulse);
}

void Rigidbody2D::AddImpulseAt(const Vec2& impulse, const Vec2& position)
{
	Vec2 disp = position - GetPosition();
	disp.Rotate90Degrees();
	float angularImpulse = impulse.DotProduct(disp);
	AddImpulse(impulse, angularImpulse);
}

////////////////////////////////
void Rigidbody2D::AddImpulse(const Vec2& linearImpulse, float angularImpulse)
{
	//AddLinearForce(linearImpulse);
	m_velocity += linearImpulse / m_massKg;
	//AddTorque(angularImpulse);
	m_angularVelocity += ConvertRadiansToDegrees(angularImpulse / m_rotaionalInertia);
}

////////////////////////////////
void Rigidbody2D::_UpdateInertia()
{
	if (m_collider->m_type == COLLIDER_OBB2) {
		Vec2 shape = ((OBBCollider2D*)m_collider)->GetLocalShape().GetSize();
		m_rotaionalInertia = (1.f / 12.f) * m_massKg * (shape.x * shape.x + shape.y * shape.y);
	}
	else if (m_collider->m_type == COLLIDER_CAPSULE2) {
		CapsuleCollider2D* collider = (CapsuleCollider2D*)m_collider;
		Capsule2 shape = collider->GetLocalShape();
		float radius = shape.Radius;
		float boxLength = GetDistance(shape.End, shape.Start);
		float boxMass = m_massKg * (radius * boxLength) / (float(M_PI) * radius * radius + radius * boxLength);
		float diskMass = m_massKg - boxMass;
		float iFromDisk = diskMass * radius * radius * 0.5f + diskMass * (0.5f * boxLength) * (0.5f * boxLength);
		float iFromBox = (1.f / 12.f) * boxMass * (radius * radius + boxLength * boxLength);
		m_rotaionalInertia = iFromBox + iFromDisk;
	} else {
		m_rotaionalInertia = m_massKg;
	}
}
