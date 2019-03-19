#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

float ConvertRadiansToDegrees(float radians)
{
	return radians / (float)M_PI * 180.f;
}

float ConvertDegreesToRadians(float degrees)
{
	return degrees / 180.f * (float)M_PI;
}

float SinDegrees(float degrees)
{
	return sinf(degrees / 180.f * (float)M_PI);
}

float CosDegrees(float degrees)
{
	return cosf(degrees / 180.f * (float)M_PI);
}

////////////////////////////////
float TanDegrees(float degrees)
{
	return tanf(degrees / 180.f * (float)M_PI);
}

float Atan2Degrees(float y, float x)
{
	return ConvertRadiansToDegrees(atan2f(y, x));
}

bool FloatEq(float lhs, float rhs, float epsilon /*= 1e-6f*/)
{
	return fabsf(lhs - rhs) < epsilon;
}

bool FloatGt(float lhs, float rhs, float epsilon /*= 1e-6f*/)
{
	return lhs - rhs > epsilon;
}

bool FloatLt(float lhs, float rhs, float epsilon /*= 1e-6f*/)
{
	return lhs - rhs < -epsilon;
}

float GetDistance(const Vec2& positionA, const Vec2& positionB)
{
	return sqrtf(
		(positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y));
}

float GetDistance(const Vec3& positionA, const Vec3& positionB)
{
	return sqrtf(
		(positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y)
		+ (positionA.z - positionB.z) * (positionA.z - positionB.z));
}

float GetDistanceXY(const Vec3& positionA, const Vec3& positionB)
{
	return sqrtf(
		(positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y));
}

float GetDistanceSquare(const Vec2& positionA, const Vec2& positionB)
{
	return
		(positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y);
}

float GetDistanceSquare(const Vec3& positionA, const Vec3& positionB)
{
	return
		(positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y)
		+ (positionA.z - positionB.z) * (positionA.z - positionB.z);
}

float GetDistanceXYSquare(const Vec3& positionA, const Vec3& positionB)
{
	return
		(positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y);
}

////////////////////////////////
float GetAngleDisplacementDegrees(float startDegrees, float endDegrees)
{
	float displacement = endDegrees - startDegrees;
	while (displacement > 180.f) {
		displacement -= 360.f;
	}
	while (displacement < -180.f) {
		displacement += 360.f;
	}
	return displacement;
}

////////////////////////////////
float GetTurnedAngleDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{
	float angleDisp = GetAngleDisplacementDegrees(currentDegrees, goalDegrees);
	if (fabsf(angleDisp) < maxDeltaDegrees) {
		return angleDisp;
	} else {
		return Sgn(angleDisp) * maxDeltaDegrees;
	}
}

////////////////////////////////
Mat4 GetRotationXYZFromAToB(const Vec3& a, const Vec3& b)
{
	Vec3 axis = a.CrossProduct(b).GetNormalized();
	float thetaRad = acosf(a.GetNormalized().DotProduct(b.GetNormalized()));
	float s = sinf(thetaRad);
	float c = cosf(thetaRad);
	float rc = (1.f - c);
	Mat4 rot;
	rot[Ix] = axis.x * axis.x * rc + c;
	rot[Iy] = axis.x * axis.y * rc + s * axis.z;
	rot[Iz] = axis.x * axis.z * rc - s * axis.y;

	rot[Jx] = axis.x * axis.y * rc - s * axis.z;
	rot[Jy] = axis.y * axis.y * rc + c;
	rot[Jz] = axis.y * axis.z * rc + s * axis.x;

	rot[Kx] = axis.x * axis.z * rc + s * axis.y;
	rot[Ky] = axis.y * axis.z * rc - s * axis.x;
	rot[Kz] = axis.z * axis.z * rc + c;

	return rot;
}

bool DoDiskOverlap(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB)
{
	float dab2 = GetDistanceSquare(centerA, centerB);
	float distance = radiusA + radiusB;

	return FloatLt(dab2, distance * distance);
}

bool DoSphereOverlap(const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB)
{
	float dab2 = GetDistanceSquare(centerA, centerB);
	float distance = radiusA + radiusB;
	return FloatLt(dab2, distance * distance);
}

Vec2 TransformedPosition_Legacy(const Vec2& position, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	Vec2 scaled = position * uniformScaleXY;
	Vec2 rotated = scaled.GetRotatedDegreesAboutOrigin(rotationDegreesAboutZ);
	Vec2 translated = rotated + translationXY;
	return translated;
}

Vec3 TransformedPosition_Legacy(const Vec3& position, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	Vec2 trunced = Vec2(position.x, position.y);
	Vec2 scaled = trunced * uniformScaleXY;
	Vec2 rotated = scaled.GetRotatedDegreesAboutOrigin(rotationDegreesAboutZ);
	Vec2 translated = rotated + translationXY;
	return Vec3(translated, position.z);
}

void TransformVertex_Legacy(Vertex_PCU& vertex, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	Vec3 newPos = TransformedPosition_Legacy(vertex.m_position, uniformScale, rotationDegreesAboutZ, translationXY);
	vertex.m_position = newPos;
}

void TransformVertexArray_Legacy(int numVertexes, Vertex_PCU vertexes[], float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	for (int i = 0; i < numVertexes; ++i) {
		TransformVertex_Legacy(vertexes[i], uniformScale, rotationDegreesAboutZ, translationXY);
	}
}

////////////////////////////////
Vec2 GetNearestPointOnAABB2(const Vec2& point, const AABB2& box)
{
	float nearestX = Clamp(point.x, box.Min.x, box.Max.x);
	float nearestY = Clamp(point.y, box.Min.y, box.Max.y);
	return Vec2(nearestX, nearestY);
}

////////////////////////////////
bool DoDiskAABB2Overlap(const Vec2& center, float radius, const AABB2& box)
{
	Vec2 nearest = GetNearestPointOnAABB2(center, box);
	if ((center - nearest).GetLengthSquare() < radius * radius)
		return true;
	else
		return false;
}

////////////////////////////////
Vec2 PushDiskOutFromAABB2(const Vec2& center, float radius, const AABB2& box)
{
	Vec2 nearest = GetNearestPointOnAABB2(center, box);
	Vec2 shift = center - nearest;
	float distance = radius - shift.GetLength();
	shift.SetLength(distance);
	return center + shift;
}

////////////////////////////////
Vec2 PushDiskOutFromDisk(const Vec2& centerPush, float radiusPush, const Vec2& centerFrom, float radiusFrom)
{
	Vec2 pushVec = centerPush - centerFrom;
	float pushDist = Clamp(radiusFrom + radiusPush - GetDistance(centerPush, centerFrom), 0.f, radiusFrom + radiusPush);
	pushVec.SetLength(pushDist);
	return centerPush + pushVec;
}

////////////////////////////////
Vec2 GetReflectedByDisk(const Vec2& incident, const Vec2& center, const Vec2& hitPointOnDisk, float bounce/*=1.f*/, float smoothness/*=1.f*/)
{
	Vec2 norm = (center - hitPointOnDisk).GetNormalized();
	Vec2 reflectVector = incident.DotProduct(norm) * norm;
	Vec2 reserveVector = incident - reflectVector;
	return (-1.f * bounce * reflectVector) + (smoothness * reserveVector);
}

////////////////////////////////
float SmoothStep3(float x)
{
	return
		x * x * (3 - 2.f * x);
}

////////////////////////////////
float SmoothStep5(float x)
{
	return
		x * x * x * (x * (6.f * x - 15.f) + 10.f);
}

////////////////////////////////
float SmoothStart2(float x)
{
	return x * x;
}

////////////////////////////////
float SmoothStart3(float x)
{
	return x * x * x;
}

////////////////////////////////
float SmoothStart4(float x)
{
	return x * x * x * x;
}

////////////////////////////////
float SmoothStart5(float x)
{
	return x * x * x * x * x;
}

////////////////////////////////
float SmoothEnd2(float x)
{
	float g = 1.f - x;
	return 1.f - (g * g);
}

////////////////////////////////
float SmoothEnd3(float x)
{
	float g = 1.f - x;
	return 1.f - (g * g * g);
}

////////////////////////////////
float SmoothEnd4(float x)
{
	float g = 1.f - x;
	return 1.f - (g * g * g * g);
}

////////////////////////////////
float SmoothEnd5(float x)
{
	float g = 1.f - x;
	return 1.f - (g * g * g * g * g);
}

////////////////////////////////
float FloatMap(float input, float inputMin, float inputMax, float outputMin, float outputMax)
{
	float inputRange = inputMax - inputMin;
	if (FloatEq(inputRange, 0.f)) {
		return (outputMax + outputMin) / 2.f;
	}
	float normedInput = (input - inputMin) / inputRange;
	float outputRange = outputMax - outputMin;
	return outputRange * normedInput + outputMin;
}
