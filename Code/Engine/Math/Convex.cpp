#include "Engine/Math/Convex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/RNG.hpp"

ConvexPoly ConvexPoly::GetRandomPoly(float radius)
{
	ConvexPoly result;
	const float start_angle = g_rng.GetFloatInRange(0, 180.f);
	float additional = 0.f;
	const size_t max_points = g_rng.GetIntInRange(3, 12);
	size_t n_points = 0;

	while (n_points <= max_points && additional < 360.f) {
		const float current_angle = start_angle + additional;
		result.m_points.push_back(
			Vec2(
				CosDegrees(current_angle) * radius,
				SinDegrees(current_angle) * radius
			)
		);
		additional += g_rng.GetFloatInRange(5.f, 120.f);
		++n_points;
	}
	
	return result;
}
