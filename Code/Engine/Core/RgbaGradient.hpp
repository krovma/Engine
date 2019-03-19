#pragma once
#include "Engine/Core/Rgba.hpp"

class RgbaGradient
{
public:
	static RgbaGradient WHITE_NOGRADIENT;
public:
	RgbaGradient() = default;
	RgbaGradient(const RgbaGradient& copyFrom);
	explicit RgbaGradient(const Rgba& start, const Rgba& end);
	/*IMPLICIT CONSTRUCTOR*/ RgbaGradient(const Rgba& noGradientColor);

	const Rgba GetColorAt(float position = 0.5f) const;
private:
	Rgba m_start = Rgba::BLACK;
	Rgba m_end = Rgba::WHITE;
	// std::vector<std::pair<float,Rgba>> m_keyPoints;
};