#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/AABBCollider2D.hpp"
#include "Engine/Physics/DiskCollider2D.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Math/MathUtils.hpp"
//////////////////////////////////////////////////////////////////////////
STATIC Vec2 PhysicsSystem::GRAVATY(0, -9.8f);

const float PhysicsSystem::PHYSICS_TIME_UNIT = 1.f / 60.f;

////////////////////////////////
PhysicsSystem::PhysicsSystem()
{

}

////////////////////////////////
PhysicsSystem::~PhysicsSystem()
{
	Shutdown();
}

////////////////////////////////
void PhysicsSystem::Startup()
{

}

////////////////////////////////
void PhysicsSystem::Shutdown()
{
	for (auto eachRigidbody : m_rigidbodies) {
		delete eachRigidbody;
	}
	m_rigidbodies.clear();
}

////////////////////////////////
void PhysicsSystem::BeginFrame()
{
	for (auto eachRigidbody : m_rigidbodies) {
		eachRigidbody->m_collider->m_inCollision = false;
	}
}

////////////////////////////////
void PhysicsSystem::Update(float deltaSeconds)
{
	// Pre update
	for (auto eachRigidbody : m_rigidbodies) {
		eachRigidbody->UpdateFromTransform();
	}
	m_accumulatedTime += deltaSeconds;
	if (m_accumulatedTime < PHYSICS_TIME_UNIT) {
		return;
	}
	// Begin update

	// Move every one
	for (auto eachRigidbody : m_rigidbodies) {
		if (eachRigidbody->GetSimulationType() == PHSX_SIM_DYNAMIC) {
			eachRigidbody->m_acceleration = GRAVATY;
		} else {
			eachRigidbody->m_acceleration = Vec2::ZERO;
		}
		eachRigidbody->Update(m_accumulatedTime);
	}

	//
	_DoDynamicVsStatic(true);
	//_DoDynamicVsDynamic(false);
	_DoDynamicVsDynamic(true);
	_DoDynamicVsStatic(false);
	_DoStaticVsStatic();
// 	int size = (int)m_rigidbodies.size();
// 	for (int i = 0; i < size; ++i) {
// 		for (int j = i + 1; j < size; ++j) {
// 			Collider2D* colliderI = m_rigidbodies[i]->m_collider;
// 			Collider2D* colliderJ = m_rigidbodies[j]->m_collider;
// 
// 			Collision2D result =
// 				colliderI->GetCollisionWith(colliderJ);
// 			if (result.isCollide) {
// 				colliderI->m_inCollision = true;
// 				colliderJ->m_inCollision = true;
// 			}
// 		}
// 	}
	// After update
	for (auto eachRigidbody : m_rigidbodies) {
		eachRigidbody->UpdateToTransform();
	}
	m_accumulatedTime = 0.f;
}

////////////////////////////////
void PhysicsSystem::DebugRender(RenderContext* renderer) const
{
	UNUSED(renderer);
}

////////////////////////////////
void PhysicsSystem::EndFrame()
{

}

////////////////////////////////
Rigidbody2D* PhysicsSystem::NewRigidbody2D(
	Collider2DType colliderType
	, NamedStrings& colliderInfo
	, Transform2D* entityTransform
	, PhysicsSimulationType simulation/*=PHSX_SIM_STATIC*/)
{
	Rigidbody2D* createdRigidbody2D = new Rigidbody2D(entityTransform);

	if (colliderType == Collider2DType::COLLIDER_AABB2) {
		AABB2 colliderLocalShape = colliderInfo.GetAABB2("localShape", AABB2::UNIT);
		createdRigidbody2D->m_collider = new AABBCollider2D(colliderLocalShape, createdRigidbody2D);

	} else if (colliderType == Collider2DType::COLLIDER_DISK2D) {
		float colliderRadius = colliderInfo.GetFloat("radius", 0.f);
		createdRigidbody2D->m_collider = new DiskCollider2D(colliderRadius, createdRigidbody2D);
	}
	createdRigidbody2D->SetSimulationType(simulation);
	m_rigidbodies.push_back(createdRigidbody2D);
	return createdRigidbody2D;
}

////////////////////////////////
void PhysicsSystem::DeleteRigidbody2D(Rigidbody2D* rigidbody)
{
	for (auto each = m_rigidbodies.begin(); each != m_rigidbodies.end();) {
		if (*each == rigidbody) {
			delete rigidbody;
			m_rigidbodies.erase(each);
			return;
		}
		++each;
	}
}

////////////////////////////////
void PhysicsSystem::_DoDynamicVsStatic(bool isResolve)
{
	for (auto eachDynamic : m_rigidbodies) {
		if (eachDynamic->GetSimulationType() == PHSX_SIM_DYNAMIC) {
			for (auto eachStatic : m_rigidbodies) {
				if (eachStatic->GetSimulationType() == PHSX_SIM_STATIC) {
					const Collider2D* colliderA = eachDynamic->GetCollider();
					const Collider2D* colliderB = eachStatic->GetCollider();
					Collision2D result = colliderA->GetCollisionWith(colliderB);
					if (result.isCollide) {
						eachDynamic->SetColliding(true);
						eachStatic->SetColliding(true);
						if (isResolve) {
							eachDynamic->Move(result.manifold.normal * result.manifold.penetration);
							Vec2 newVelocity = _GetElasticCollidedVelocity(result);
							eachDynamic->SetVelocity(newVelocity);
						}
					}
				}
			}
		}
	}
}

////////////////////////////////
void PhysicsSystem::_DoDynamicVsDynamic(bool isResolve)
{
	size_t size = m_rigidbodies.size();

	for (size_t i = 0; i < size; ++i) {
		Rigidbody2D* dynamicA = m_rigidbodies[i];
		if (dynamicA->GetSimulationType() == PHSX_SIM_DYNAMIC) {
			for (size_t j = i + 1; j < size; ++j) {
				Rigidbody2D* dynamicB = m_rigidbodies[j];
				if (dynamicB->GetSimulationType() == PHSX_SIM_DYNAMIC) {
					const Collider2D* colliderA = dynamicA->GetCollider();
					const Collider2D* colliderB = dynamicB->GetCollider();
					Collision2D result = colliderA->GetCollisionWith(colliderB);
					if (result.isCollide) {
						dynamicA->SetColliding(true);
						dynamicB->SetColliding(true);
						if (isResolve) {
							Vec2 fullMove = result.manifold.normal * result.manifold.penetration;
							Vec2 movingA = fullMove * (
									dynamicA->m_massKg / (dynamicA->m_massKg + dynamicB->m_massKg)
								);
							Vec2 movingB = -fullMove * (
									dynamicB->m_massKg / (dynamicA->m_massKg + dynamicB->m_massKg)
								);
							dynamicA->Move(movingA);
							dynamicB->Move(movingB);
							Vec2 newVelocityA = _GetElasticCollidedVelocity(result);
							Collision2D reversed = result;
							reversed.manifold.normal *= -1;
							reversed.which = result.collideWith;
							reversed.collideWith = result.which;
							Vec2 newVelocityB = _GetElasticCollidedVelocity(reversed);

							dynamicA->SetVelocity(newVelocityA);
							dynamicB->SetVelocity(newVelocityB);
						}
					}
				}
			}
		}
	}
}

////////////////////////////////
void PhysicsSystem::_DoStaticVsStatic()
{
	for (auto staticA : m_rigidbodies) {
		if (staticA->GetSimulationType() == PHSX_SIM_STATIC) {
			for (auto staticB : m_rigidbodies) {
				if (staticA == staticB) {
					continue;
				}
				if (staticB->GetSimulationType() == PHSX_SIM_STATIC) {
					const Collider2D* colliderA = staticA->GetCollider();
					const Collider2D* colliderB = staticB->GetCollider();
					Collision2D result = colliderA->GetCollisionWith(colliderB);
					if (result.isCollide) {
						staticA->SetColliding(true);
						staticB->SetColliding(true);
						break;
					}
				}
			}
		}
	}
}

////////////////////////////////
Vec2 PhysicsSystem::_GetElasticCollidedVelocity(const Collision2D& collision) const
{
	const Rigidbody2D* which = collision.which->m_rigidbody;
	const Rigidbody2D* with = collision.collideWith->m_rigidbody;
	float restitution = which->m_bounciness * with->m_bounciness;
	float smoothness = which->m_smoothness * with->m_smoothness;
	const Vec2& normal = collision.manifold.normal;
	if (with->m_simulationType == PHSX_SIM_STATIC) {
		Vec2 velocity = which->m_velocity;
		Vec2 normalVelocity = normal * velocity.DotProduct(normal);
		Vec2 tangentVelocity = velocity - normalVelocity;
		return (-normalVelocity * restitution + tangentVelocity * smoothness);
	} else {
		Vec2 vA = which->m_velocity;
		Vec2 vB = with->m_velocity;
		Vec2 vNormalA = normal * vA.DotProduct(normal);
		Vec2 vNormalB = normal * vB.DotProduct(normal);
		Vec2 vTangentA = vA - vNormalA;
		//Vec2 vTangentB = vB - vNormalB;
		float u1 = vNormalA.DotProduct(normal);//vNormalA.GetLength() * Sgn(vA.DotProduct(normal));
		float u2 = vNormalB.DotProduct(normal);//vNormalB.GetLength() * Sgn(vB.DotProduct(normal));
		float m1 = which->m_massKg;
		float m2 = with->m_massKg;
		float v1 = (restitution * m2 * (u2 - u1) + m1 * u1 + m2 * u2) / (m1 + m2);
		//float v2 = (2.f * m1 * u1 + (m2 - m1) * u2) / (m1 + m2);
		vNormalA = v1 * normal;
		
		return (vNormalA + vTangentA * smoothness);
	}
}
