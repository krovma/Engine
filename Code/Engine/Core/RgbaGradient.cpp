#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/RgbaGradient.hpp"
#include "Engine/Math/MathUtils.hpp"

STATIC RgbaGradient RgbaGradient::WHITE_NOGRADIENT(Rgba(1.f, 1.f, 1.f, 1.f));

////////////////////////////////
RgbaGradient::RgbaGradient(const Rgba& start, const Rgba& end)
	: m_start(start)
	, m_end(end)
{
}

////////////////////////////////
RgbaGradient::RgbaGradient(const Rgba& noGradientColor)
	: m_start(noGradientColor)
	, m_end(noGradientColor)
{
}

////////////////////////////////
RgbaGradient::RgbaGradient(const RgbaGradient& copyFrom)
	: m_start(copyFrom.m_start)
	, m_end(copyFrom.m_end)
{
}

////////////////////////////////
const Rgba RgbaGradient::GetColorAt(float position /*= 0.5f*/) const
{
	return Lerp(m_start, m_end, position);
}

