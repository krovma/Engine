#include "Engine/Physics/Rigidbody2D.hpp"

////////////////////////////////
Rigidbody2D::Rigidbody2D(Game* theGame)
	:Entity(theGame)
{

}

////////////////////////////////
void Rigidbody2D::Update(float deltaSeconds)
{
	m_velocity += m_acceleration * deltaSeconds;
	m_position += m_velocity * deltaSeconds;
}

////////////////////////////////
void Rigidbody2D::Render() const
{
	//Do nothing
}

////////////////////////////////
void Rigidbody2D::SetSimulationType(PhysicsSimulationType type)
{
	m_simulationType = type;
}
