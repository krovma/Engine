#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
//////////////////////////////////////////////////////////////////////////
STATIC const Mat4 Mat4::Identity;

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
	_ix = 1.f;
	_iy = 0.f;
	_iz = 0.f;
	_iw = 0.f;

	_jx = 0.f;
	_jy = 1.f;
	_jz = 0.f;
	_jw = 0.f;

	_kx = 0.f;
	_ky = 0.f;
	_kz = 1.f;
	_kw = 0.f;

	_tx = 0.f;
	_ty = 0.f;
	_tz = 0.f;
	_tw = 1.f;

}

////////////////////////////////
Mat4::Mat4(const Vec2& iBasis, const Vec2& jBasis, const Vec2& transform/*=Vec2(0, 0)*/)
{
	_ix = iBasis.x;
	_iy = iBasis.y;
	_iz = 0.f;
	_iw = 0.f;

	_jx = jBasis.x;
	_jy = jBasis.y;
	_jz = 0.f;
	_jw = 0.f;

	_kx = 0.f;
	_ky = 0.f;
	_kz = 1.f;
	_kw = 0.f;

	_tx = transform.x;
	_ty = transform.y;
	_tz = 0.f;
	_tw = 1.f;
}

////////////////////////////////
Mat4::Mat4(const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis, const Vec3& transform/*=Vec3(0, 0, 0)*/)
{
	_ix = iBasis.x;
	_iy = iBasis.y;
	_iz = iBasis.z;
	_iw = 0.f;

	_jx = jBasis.x;
	_jy = jBasis.y;
	_jz = jBasis.z;
	_jw = 0.f;

	_kx = kBasis.x;
	_ky = kBasis.y;
	_kz = kBasis.z;
	_kw = 0.f;

	_tx = transform.x;
	_ty = transform.y;
	_tz = transform.z;
	_tw = 1.f;
}

////////////////////////////////
Mat4::Mat4(const Vec4& iBasis, const Vec4& jBasis, const Vec4& kBasis, const Vec4& transform)
{
	_ix = iBasis.x;
	_iy = iBasis.y;
	_iz = iBasis.z;
	_iw = iBasis.w;

	_jx = jBasis.x;
	_jy = jBasis.y;
	_jz = jBasis.z;
	_jw = jBasis.w;

	_kx = kBasis.x;
	_ky = kBasis.y;
	_kz = kBasis.z;
	_kw = kBasis.w;

	_tx = transform.x;
	_ty = transform.y;
	_tz = transform.z;
	_tw = transform.w;
}

////////////////////////////////
Mat4::Mat4(const float values[/*16*/])
{
	_ix = values[0];
	_iy = values[1];
	_iz = values[2];
	_iw = values[3];
	
	_jx = values[4];
	_jy = values[5];
	_jz = values[6];
	_jw = values[7];
	
	_kx = values[8];
	_ky = values[9];
	_kz = values[10];
	_kw = values[11];
	
	_tx = values[12];
	_ty = values[13];
	_tz = values[14];
	_tw = values[15];
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
	_ix = copyFrom[Ix];
	_iy = copyFrom[Iy];
	_iz = copyFrom[Iz];
	_iw = copyFrom[Iw];

	_jx = copyFrom[Jx];
	_jy = copyFrom[Jy];
	_jz = copyFrom[Jz];
	_jw = copyFrom[Jw];

	_kx = copyFrom[Kx];
	_ky = copyFrom[Ky];
	_kz = copyFrom[Kz];
	_kw = copyFrom[Kw];

	_tx = copyFrom[Tx];
	_ty = copyFrom[Ty];
	_tz = copyFrom[Tz];
	_tw = copyFrom[Tw];

	return *this;
}

////////////////////////////////
const Mat4 Mat4::operator*(const Mat4& rhs) const
{
	Mat4 result;
	result[Ix] = _ix * rhs[Ix] + _jx * rhs[Iy] + _kx * rhs[Iz] + _tx * rhs[Iw];
	result[Iy] = _iy * rhs[Ix] + _jy * rhs[Iy] + _ky * rhs[Iz] + _ty * rhs[Iw];
	result[Iz] = _iz * rhs[Ix] + _jz * rhs[Iy] + _kz * rhs[Iz] + _tz * rhs[Iw];
	result[Iw] = _iw * rhs[Ix] + _jw * rhs[Iy] + _kw * rhs[Iz] + _tw * rhs[Iw];

	result[Jx] = _ix * rhs[Jx] + _jx * rhs[Jy] + _kx * rhs[Jz] + _tx * rhs[Jw];
	result[Jy] = _iy * rhs[Jx] + _jy * rhs[Jy] + _ky * rhs[Jz] + _ty * rhs[Jw];
	result[Jz] = _iz * rhs[Jx] + _jz * rhs[Jy] + _kz * rhs[Jz] + _tz * rhs[Jw];
	result[Jw] = _iw * rhs[Jx] + _jw * rhs[Jy] + _kw * rhs[Jz] + _tw * rhs[Jw];

	result[Kx] = _ix * rhs[Kx] + _jx * rhs[Ky] + _kx * rhs[Kz] + _tx * rhs[Kw];
	result[Ky] = _iy * rhs[Kx] + _jy * rhs[Ky] + _ky * rhs[Kz] + _ty * rhs[Kw];
	result[Kz] = _iz * rhs[Kx] + _jz * rhs[Ky] + _kz * rhs[Kz] + _tz * rhs[Kw];
	result[Kw] = _iw * rhs[Kx] + _jw * rhs[Ky] + _kw * rhs[Kz] + _tw * rhs[Kw];

	result[Tx] = _ix * rhs[Tx] + _jx * rhs[Ty] + _kx * rhs[Tz] + _tx * rhs[Tw];
	result[Ty] = _iy * rhs[Tx] + _jy * rhs[Ty] + _ky * rhs[Tz] + _ty * rhs[Tw];
	result[Tz] = _iz * rhs[Tx] + _jz * rhs[Ty] + _kz * rhs[Tz] + _tz * rhs[Tw];
	result[Tw] = _iw * rhs[Tx] + _jw * rhs[Ty] + _kw * rhs[Tz] + _tw * rhs[Tw];

	return result;
}

////////////////////////////////
const Mat4& Mat4::operator*=(const Mat4& rhs)
{
	return (*this = *this * rhs);
}

////////////////////////////////
const Vec4 Mat4::operator*(const Vec4& rhs) const
{
	Vec4 result;
	result.x = Vec4(_ix, _jx, _kx, _tx).DotProduct(rhs);
	result.y = Vec4(_iy, _jy, _ky, _ty).DotProduct(rhs);
	result.z = Vec4(_iz, _jz, _kz, _tz).DotProduct(rhs);
	result.w = Vec4(_iw, _jw, _kw, _tw).DotProduct(rhs);
	return result;
}

////////////////////////////////
Vec2 Mat4::GetTransformedVector2D(const Vec2& vector) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(vector, 0, 0));
	Vec2 result(result4.x, result4.y);
	/*
	result.x = ix * vector.x + jx * vector.y;
	result.y = iy * vector.y + jy * vector.y;
	*/
	return result;
}

////////////////////////////////
Vec2 Mat4::GetTransformedPosition2D(const Vec2& position) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(position, 0, 1));
	Vec2 result(result4.x, result4.y);
// 	result.x = ix * position.x + jx * position.y + tx;
// 	result.y = iy * position.y + jy * position.y + ty;
	return result;
}

////////////////////////////////
Vec3 Mat4::GetTransformedVector3D(const Vec3& vector) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(vector, 0));
	Vec3 result(result4.x, result4.y, result4.z);
// 	result.x = ix * vector.x + jx * vector.y + kx * vector.z;
// 	result.y = iy * vector.y + jy * vector.y + ky * vector.z;
// 	result.z = iz * vector.y + jz * vector.y + kz * vector.z;
	return result;
}

////////////////////////////////
Vec3 Mat4::GetTransformedPosition3D(const Vec3& position) const
{
	Vec4 result4 = GetTransformedVec4(Vec4(position, 1));
	Vec3 result(result4.x, result4.y, result4.z);
// 	result.x = ix * position.x + jx * position.y + kx * position.z + tx;
// 	result.y = iy * position.y + jy * position.y + ky * position.z + ty;
// 	result.z = iz * position.y + jz * position.y + kz * position.z + tz;
	return result;

}

////////////////////////////////
Vec4 Mat4::GetTransformedVec4(const Vec4& v) const
{
	Vec4 result;
	result.x = _ix * v.x + _jx * v.y + _kx * v.z + _tx * v.w;
	result.y = _iy * v.x + _jy * v.y + _ky * v.z + _ty * v.w;
	result.z = _iz * v.x + _jz * v.y + _kz * v.z + _tz * v.w;
	result.w = _iw * v.x + _jw * v.y + _kw * v.z + _tw * v.w;
	return result;
}
