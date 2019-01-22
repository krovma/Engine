#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


//////////////////////////////////////////////////////////////////////////
struct Mat4
{
public:
	static const Mat4 I;
	float m_value[16];
	enum Mat4Index
	{
		Ix, Iy, Iz, Iw, Jx, Jy, Jz, Jw, Kx, Ky, Kz, Kw,
		Tx, Ty, Tz, Tw,
	};
public:
	static Mat4 MakeTranslate2D(const Vec2& translateXY);
	static Mat4 MakeUniformScale2D(float scaleXY);
	static Mat4 MakeRotateDegrees2D(float degrees);
	
	Mat4();
	explicit Mat4(const Vec2& iBasis, const Vec2& jBasis, const Vec2& transform=Vec2(0, 0));
	explicit Mat4(const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis, const Vec3& transform=Vec3(0, 0, 0));
	explicit Mat4(const Vec4& iBasis, const Vec4& jBasis, const Vec4& kBasis, const Vec4& transform);
	explicit Mat4(const float values[/*16*/]);
	float& operator[](int index);
	const float operator[](int index) const;
	
	const Mat4& operator=(const Mat4& copyFrom);
	const Mat4 operator*(const Mat4& rhs) const;
	const Mat4& operator*=(const Mat4& rhs);

	//////////////////////////////////////////////////////////////////////////
	//
	//[Ix Jx Kx Tx]   [x]
	//[Iy Jy Ky Ty] * [y]
	//[Iz Jz Kz Tz]   [z]
	//[Iw Jw Kw Tx]   [w]
	//    Mat4       Homo3D
	//
	//
	//
	//
	//////////////////////////////////////////////////////////////////////////
	Vec2 GetTransformedVector2D(const Vec2& vector) const;
	Vec2 GetTransformedPosition2D(const Vec2& position) const;
	Vec3 GetTransformedVector3D(const Vec3& vector) const;
	Vec3 GetTransformedPosition3D(const Vec3& position) const;
	Vec4 GetTransformedVec4(const Vec4& v) const;

};
