#pragma once
#include "Engine/Math/Vec3.hpp"

struct Capsule3;
struct Plane3;

struct Ray3
{
	Vec3 start = Vec3::ZERO;
	const Vec3 dir = Vec3(1,0,0);

	Ray3(const Vec3& start, const Vec3& dir);
	static Ray3 FromPoint(const Vec3& start, const Vec3& pointOnRay);

	Vec3 GetPointAt(float k) const;
	Vec3 GetClampedPointAt(float k) const;

	float RaycastToInfCylinder(const Vec3& center, const Vec3& dir, float radius) const;
	float RaycastToSphere(const Vec3& center, float radius) const;
	float RaycastToCapsule3(const Capsule3& capsule) const;
	float RaycastToPlane3(const Plane3& plane) const;
};
