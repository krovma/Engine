#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
//////////////////////////////////////////////////////////////////////////
STATIC const Mat4 Mat4::I;

////////////////////////////////
STATIC Mat4 Mat4::MakeTranslate2D(const Vec2& translateXY)
{
	Mat4 t;
	t[Tx] = translateXY.x;
	t[Ty] = translateXY.y;
	t[Jx];
	return t;
}

////////////////////////////////
STATIC Mat4 Mat4::MakeUniformScale2D(float scaleXY)
{
	Mat4 s;
	s[Ix] = scaleXY;
	s[Jy] = scaleXY;
	return s;
}

////////////////////////////////
STATIC Mat4 Mat4::MakeRotateDegrees2D(float degrees)
{
	Mat4 r;
	float s = SinDegrees(degrees);
	float c = CosDegrees(degrees);
	r[Ix] = c;
	r[Iy] = s;
	r[Jx] = -s;
	r[Jy] = c;
	return r;
}

////////////////////////////////
Mat4::Mat4()
{
	m_value[Ix] = 1.f;
	m_value[Iy] = 0.f;
	m_value[Iz] = 0.f;
	m_value[Iw] = 0.f;

	m_value[Jx] = 0.f;
	m_value[Jy] = 1.f;
	m_value[Jz] = 0.f;
	m_value[Jw] = 0.f;

	m_value[Kx] = 0.f;
	m_value[Ky] = 0.f;
	m_value[Kz] = 1.f;
	m_value[Kw] = 0.f;

	m_value[Tx] = 0.f;
	m_value[Ty] = 0.f;
	m_value[Tz] = 0.f;
	m_value[Tw] = 1.f;

}

////////////////////////////////
Mat4::Mat4(const Vec2& iBasis, const Vec2& jBasis, const Vec2& transform/*=Vec2(0, 0)*/)
{
	m_value[Ix] = iBasis.x;
	m_value[Iy] = iBasis.y;
	m_value[Iz] = 0.f;
	m_value[Iw] = 0.f;

	m_value[Jx] = jBasis.x;
	m_value[Jy] = jBasis.y;
	m_value[Jz] = 0.f;
	m_value[Jw] = 0.f;

	m_value[Kx] = 0.f;
	m_value[Ky] = 0.f;
	m_value[Kz] = 1.f;
	m_value[Kw] = 0.f;

	m_value[Tx] = transform.x;
	m_value[Ty] = transform.y;
	m_value[Tz] = 0.f;
	m_value[Tw] = 1.f;
}

////////////////////////////////
Mat4::Mat4(const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis, const Vec3& transform/*=Vec3(0, 0, 0)*/)
{
	m_value[Ix] = iBasis.x;
	m_value[Iy] = iBasis.y;
	m_value[Iz] = iBasis.z;
	m_value[Iw] = 0.f;

	m_value[Jx] = jBasis.x;
	m_value[Jy] = jBasis.y;
	m_value[Jz] = jBasis.z;
	m_value[Jw] = 0.f;

	m_value[Kx] = kBasis.x;
	m_value[Ky] = kBasis.y;
	m_value[Kz] = kBasis.z;
	m_value[Kw] = 0.f;

	m_value[Tx] = transform.x;
	m_value[Ty] = transform.y;
	m_value[Tz] = transform.z;
	m_value[Tw] = 1.f;
}

////////////////////////////////
Mat4::Mat4(const Vec4& iBasis, const Vec4& jBasis, const Vec4& kBasis, const Vec4& transform)
{
	m_value[Ix] = iBasis.x;
	m_value[Iy] = iBasis.y;
	m_value[Iz] = iBasis.z;
	m_value[Iw] = iBasis.w;

	m_value[Jx] = jBasis.x;
	m_value[Jy] = jBasis.y;
	m_value[Jz] = jBasis.z;
	m_value[Jw] = jBasis.w;

	m_value[Kx] = kBasis.x;
	m_value[Ky] = kBasis.y;
	m_value[Kz] = kBasis.z;
	m_value[Kw] = kBasis.w;

	m_value[Tx] = transform.x;
	m_value[Ty] = transform.y;
	m_value[Tz] = transform.z;
	m_value[Tw] = transform.w;
}

////////////////////////////////
Mat4::Mat4(const float values[/*16*/])
{
	m_value[Ix] = values[0];
	m_value[Iy] = values[1];
	m_value[Iz] = values[2];
	m_value[Iw] = values[3];
	
	m_value[Jx] = values[4];
	m_value[Jy] = values[5];
	m_value[Jz] = values[6];
	m_value[Jw] = values[7];
	
	m_value[Kx] = values[8];
	m_value[Ky] = values[9];
	m_value[Kz] = values[10];
	m_value[Kw] = values[11];
	m_value[Tx] = values[12];

	m_value[Ty] = values[13];
	m_value[Tz] = values[14];
	m_value[Tw] = values[15];
}

////////////////////////////////
float& Mat4::operator[](int index)
{
	return m_value[index];
}

////////////////////////////////
const float Mat4::operator[](int index) const
{
	return m_value[index];
}

////////////////////////////////
const Mat4& Mat4::operator=(const Mat4& copyFrom)
{
	m_value[Ix] = copyFrom[Ix];
	m_value[Iy] = copyFrom[Iy];
	m_value[Iz] = copyFrom[Iz];
	m_value[Iw] = copyFrom[Iw];

	m_value[Jx] = copyFrom[Jx];
	m_value[Jy] = copyFrom[Jy];
	m_value[Jz] = copyFrom[Jz];
	m_value[Jw] = copyFrom[Jw];

	m_value[Kx] = copyFrom[Kx];
	m_value[Ky] = copyFrom[Ky];
	m_value[Kz] = copyFrom[Kz];
	m_value[Kw] = copyFrom[Kw];

	m_value[Tx] = copyFrom[Tx];
	m_value[Ty] = copyFrom[Ty];
	m_value[Tz] = copyFrom[Tz];
	m_value[Tw] = copyFrom[Tw];

	return *this;
}

////////////////////////////////
const Mat4 Mat4::operator*(const Mat4& rhs) const
{
	Mat4 result;
	result[Ix] = m_value[Ix] * rhs[Ix] + m_value[Jx] * rhs[Iy] + m_value[Kx] * rhs[Iz] + m_value[Tx] * rhs[Iw];
	result[Iy] = m_value[Iy] * rhs[Ix] + m_value[Jy] * rhs[Iy] + m_value[Ky] * rhs[Iz] + m_value[Ty] * rhs[Iw];
	result[Iz] = m_value[Iz] * rhs[Ix] + m_value[Jz] * rhs[Iy] + m_value[Kz] * rhs[Iz] + m_value[Tz] * rhs[Iw];
	result[Iw] = m_value[Iw] * rhs[Ix] + m_value[Jw] * rhs[Iy] + m_value[Kw] * rhs[Iz] + m_value[Tw] * rhs[Iw];

	result[Jx] = m_value[Ix] * rhs[Jx] + m_value[Jx] * rhs[Jy] + m_value[Kx] * rhs[Jz] + m_value[Tx] * rhs[Jw];
	result[Jy] = m_value[Iy] * rhs[Jx] + m_value[Jy] * rhs[Jy] + m_value[Ky] * rhs[Jz] + m_value[Ty] * rhs[Jw];
	result[Jz] = m_value[Iz] * rhs[Jx] + m_value[Jz] * rhs[Jy] + m_value[Kz] * rhs[Jz] + m_value[Tz] * rhs[Jw];
	result[Jw] = m_value[Iw] * rhs[Jx] + m_value[Jw] * rhs[Jy] + m_value[Kw] * rhs[Jz] + m_value[Tw] * rhs[Jw];

	result[Kx] = m_value[Ix] * rhs[Kx] + m_value[Jx] * rhs[Ky] + m_value[Kx] * rhs[Kz] + m_value[Tx] * rhs[Kw];
	result[Ky] = m_value[Iy] * rhs[Kx] + m_value[Jy] * rhs[Ky] + m_value[Ky] * rhs[Kz] + m_value[Ty] * rhs[Kw];
	result[Kz] = m_value[Iz] * rhs[Kx] + m_value[Jz] * rhs[Ky] + m_value[Kz] * rhs[Kz] + m_value[Tz] * rhs[Kw];
	result[Kw] = m_value[Iw] * rhs[Kx] + m_value[Jw] * rhs[Ky] + m_value[Kw] * rhs[Kz] + m_value[Tw] * rhs[Kw];

	result[Tx] = m_value[Ix] * rhs[Tx] + m_value[Jx] * rhs[Ty] + m_value[Kx] * rhs[Tz] + m_value[Tx] * rhs[Tw];
	result[Ty] = m_value[Iy] * rhs[Tx] + m_value[Jy] * rhs[Ty] + m_value[Ky] * rhs[Tz] + m_value[Ty] * rhs[Tw];
	result[Tz] = m_value[Iz] * rhs[Tx] + m_value[Jz] * rhs[Ty] + m_value[Kz] * rhs[Tz] + m_value[Tz] * rhs[Tw];
	result[Tw] = m_value[Iw] * rhs[Tx] + m_value[Jw] * rhs[Ty] + m_value[Kw] * rhs[Tz] + m_value[Tw] * rhs[Tw];

	return result;
}

////////////////////////////////
const Mat4& Mat4::operator*=(const Mat4& rhs)
{
	return (*this = *this * rhs);
}

////////////////////////////////
Vec2 Mat4::GetTransformedVector2D(const Vec2& vector) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(vector, 0, 0));
	Vec2 result(result4.x, result4.y);
	/*
	result.x = m_value[Ix] * vector.x + m_value[Jx] * vector.y;
	result.y = m_value[Iy] * vector.y + m_value[Jy] * vector.y;
	*/
	return result;
}

////////////////////////////////
Vec2 Mat4::GetTransformedPosition2D(const Vec2& position) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(position, 0, 1));
	Vec2 result(result4.x, result4.y);
// 	result.x = m_value[Ix] * position.x + m_value[Jx] * position.y + m_value[Tx];
// 	result.y = m_value[Iy] * position.y + m_value[Jy] * position.y + m_value[Ty];
	return result;
}

////////////////////////////////
Vec3 Mat4::GetTransformedVector3D(const Vec3& vector) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(vector, 0));
	Vec3 result(result4.x, result4.y, result4.z);
// 	result.x = m_value[Ix] * vector.x + m_value[Jx] * vector.y + m_value[Kx] * vector.z;
// 	result.y = m_value[Iy] * vector.y + m_value[Jy] * vector.y + m_value[Ky] * vector.z;
// 	result.z = m_value[Iz] * vector.y + m_value[Jz] * vector.y + m_value[Kz] * vector.z;
	return result;
}

////////////////////////////////
Vec3 Mat4::GetTransformedPosition3D(const Vec3& position) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(position, 1));
	Vec3 result(result4.x, result4.y, result4.z);
// 	result.x = m_value[Ix] * position.x + m_value[Jx] * position.y + m_value[Kx] * position.z + m_value[Tx];
// 	result.y = m_value[Iy] * position.y + m_value[Jy] * position.y + m_value[Ky] * position.z + m_value[Ty];
// 	result.z = m_value[Iz] * position.y + m_value[Jz] * position.y + m_value[Kz] * position.z + m_value[Tz];
	return result;

}

////////////////////////////////
Vec4 Mat4::GetTransformedVec4(const Vec4& v) const
{
	Vec4 result;
	result.x = m_value[Ix] * v.x + m_value[Jx] * v.y + m_value[Kx] * v.z + m_value[Tx] * v.w;
	result.y = m_value[Iy] * v.x + m_value[Jy] * v.y + m_value[Ky] * v.z + m_value[Ty] * v.w;
	result.z = m_value[Iz] * v.x + m_value[Jz] * v.y + m_value[Kz] * v.z + m_value[Tz] * v.w;
	result.w = m_value[Iw] * v.x + m_value[Jw] * v.y + m_value[Kw] * v.z + m_value[Tw] * v.w;
	return result;
}
