#pragma once
#include "Game/Entity.hpp"

class Game;
class Collider2D;

enum PhysicsSimulationType
{
	PHSX_SIM_STATIC,
	PHSX_SIM_DYNAMIC,

	NUM_PHSX_SIM_TYPES
};

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