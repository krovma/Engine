#pragma once
//////////////////////////////////////////////////////////////////////////
struct IntVec2;
//-----------------------------------------------------------------------------------------------
struct Vec2
{
public:
	// static
	static Vec2 MakeFromPolarDegrees(float thetaDegrees, float length = 1.f);
	static Vec2 MakeFromPolarRadians(float thetaRadians, float length = 1.f);
	static const Vec2 ZERO;
	static const Vec2 ONE;

	// Construction/Destruction
	~Vec2() {}												// destructor: do nothing (for speed)
	Vec2() {}												// default constructor: do nothing (for speed)
	Vec2( const Vec2& copyFrom );							// copy constructor (from another vec2)
	explicit Vec2(const IntVec2& convertFrom);
	explicit Vec2(float initialX, float initialY);		// explicit constructor (from x, y)

	/// Format: float, float
	void SetFromText(const char* text);

	// Operators
	const Vec2 operator+( const Vec2& vecToAdd ) const;			// vec2 + vec2
	const Vec2 operator-( const Vec2& vecToSubtract ) const;	// vec2 - vec2
	const Vec2 operator*( float uniformScale ) const;			// vec2 * float
	const Vec2 operator/( float inverseScale ) const;			// vec2 / float
	const Vec2 operator-() const;
	void operator+=( const Vec2& vecToAdd );					// vec2 += vec2
	void operator-=( const Vec2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const float uniformScale );				// vec2 *= float
	void operator/=( const float uniformDivisor );				// vec2 /= float
	void operator=( const Vec2& copyFrom );						// vec2 = vec2
	bool operator==( const Vec2& compare ) const;				// vec2 == vec2
	bool operator!=( const Vec2& compare ) const;				// vec2 != vec2

	float DotProduct(const Vec2& dotProductWith) const;
	//float operator *(const Vec2& dotProductWith) const;

	friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );	// float * vec2

	// Getter
	float GetLength() const;
	float GetLengthSquare() const;
	float GetAngleDegrees() const;
	float GetAngleRadians() const;
	const Vec2 GetRotated90Degrees() const;
	const Vec2 GetRotatedMinus90Degrees() const;
	const Vec2 GetClamped(float maxLength) const;
	const Vec2 GetNormalized() const;

	const Vec2 GetRotatedDegreesAboutOrigin(float degrees) const;
	const Vec2 GetRotatedRadiansAboutOrigin(float radians) const;

	// Modifier
	void SetLength(float length);
	void ClampLength(float maxLength);
	void SetAngleDegrees(float angleDegrees);
	void SetAngleRadians(float angleRadians);
	void SetPolarDegrees(float angleDegrees, float length);
	void SetPolarRadians(float angleRadians, float length);
	void RotateDegreesAboutOrigin(float degrees);
	void RotateRadiansAboutOrigin(float radians);
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void Normalize();
	float GetLengthThenNormalize();


public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
};


