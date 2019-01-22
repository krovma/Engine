#pragma once

//-----------------------------------------------------------------------------------------------
struct IntVec2
{
public:

	static const IntVec2 LOOK_AROUND_4[];
	static const IntVec2 LOOK_AROUND_8[];
	static const IntVec2 ZERO;

	IntVec2() = default;
	IntVec2(const IntVec2& copyFrom);
	explicit IntVec2(int x, int y);

	~IntVec2() {
		//Do nothing
	}

	void SetFromText(const char* text);

	const IntVec2 operator= (const IntVec2& copyFrom);
	const IntVec2 operator+= (const IntVec2& vecToAdd);
	const IntVec2 operator-= (const IntVec2& vecToSubtract);
	const IntVec2 operator*= (int scale);

	const IntVec2 operator+ (const IntVec2& rhs) const;
	const IntVec2 operator- (const IntVec2& rhs) const;
	const IntVec2 operator* (int rhs) const;
	friend const IntVec2 operator* (int lhs, const IntVec2& rhs);

	bool operator== (const IntVec2& rhs) const;
	bool operator!= (const IntVec2& rhs) const;
	

public:
	int x = 0;
	int y = 0;
};
