#pragma once
#include <vector>
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/Vec2.hpp"
//////////////////////////////////////////////////////////////////////////
class RenderContext;
//////////////////////////////////////////////////////////////////////////
class PhysicsSystem
{
public:
	static Vec2 GRAVATY;
	static const float PHYSICS_TIME_UNIT;
public:
	PhysicsSystem();
	~PhysicsSystem();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void Update(float deltaSeconds);
	void DebugRender(RenderContext* renderer) const;
	void EndFrame();
	
	Rigidbody2D* NewRigidbody2D(
		Collider2DType colliderType
		, NamedStrings& colliderInfo
		, Transform2D* entityTransform
		, PhysicsSimulationType simulation = PHSX_SIM_STATIC);
	void DeleteRigidbody2D(Rigidbody2D* rigidbody);

private:
	void _DoDynamicVsStatic(bool isResolve);
	void _DoDynamicVsDynamic(bool isResolve);
	void _DoStaticVsStatic();
	Vec2 _GetElasticCollidedVelocity(const Collision2D& collision) const;

private:
	float m_accumulatedTime = 0.f;
	std::vector<Rigidbody2D*> m_rigidbodies;
};