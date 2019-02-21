#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Transform2D.hpp"
class Collider2D;

enum PhysicsSimulationType
{
	PHSX_SIM_STATIC,
	PHSX_SIM_DYNAMIC,

	NUM_PHSX_SIM_TYPES
};
/*
class Rigidbody2D : public Entity
{
public:
	friend class PhysicsSystem;
	Rigidbody2D(Game* theGame);
	virtual void Update(float deltaSeconds);
	virtual void Render() const;
	const Collider2D* GetCollider() const { return m_collider; }

	void SetSimulationType(PhysicsSimulationType type);
	PhysicsSimulationType GetSimulationType() const { return m_simulationType; }
private:
	Collider2D* m_collider = nullptr;
	PhysicsSimulationType m_simulationType = PHSX_SIM_STATIC;
};
*/
class Rigidbody2D
{
public:
	friend class PhysicsSystem;
	Rigidbody2D(Transform2D* transform);
	~Rigidbody2D();
	void Update(float deltaSeconds);

	const Collider2D* GetCollider() const { return m_collider; }
	PhysicsSimulationType GetSimulationType() const { return m_simulationType; }
	void SetSimulationType(PhysicsSimulationType type);
	void SetCollider(Collider2D* collider) { m_collider = collider; }
	void UpdateToTransform() const;
	void UpdateFromTransform();
	Vec2 GetPosition() const { return m_position; }
	Vec2 GetVelocity() const { return m_velocity; }
	Vec2 GetAcceleration() const { return m_acceleration; }
	bool IsColliding() const;
	void SetColliding(bool isColliding);

	float GetMassKg() const { return m_massKg; }
	float GetBounciness() const { return m_bounciness; }
	float GetSmoothness() const { return m_smoothness; }

	void SetMassKg(float massKg) { m_massKg = massKg; }
	void SetBounciness(float bounce) { m_bounciness = bounce; }
	void SetSmoothness(float smooth) { m_smoothness = smooth; }


private:
	// These setters can only be called from PhysicsSystem
	void SetPosition(const Vec2& position) { m_position = position; }
	void SetVelocity(const Vec2& velocity) { m_velocity = velocity; }
	void Move(const Vec2& displacement) { m_position += displacement; }
	void SetAcceleration(const Vec2& acceleration) { m_acceleration = acceleration; }

private:
	Collider2D* m_collider = nullptr;
	Transform2D* m_entityTransform;

	Vec2 m_position = Vec2::ZERO;
	float m_orientationDegrees = 0.f;
	Vec2 m_velocity = Vec2::ZERO;
	Vec2 m_acceleration = Vec2::ZERO;
	float m_angularVelocity = 0.f;
	float m_angularAcceleration = 0.f;
	PhysicsSimulationType m_simulationType = PHSX_SIM_STATIC;

	float m_massKg = 1.f;
	float m_bounciness = 1.f;
	float m_smoothness = 1.f;
};