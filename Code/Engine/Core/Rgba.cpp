#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
STATIC const Rgba Rgba::WHITE		(1.0f, 1.0f, 1.0f);
STATIC const Rgba Rgba::BLACK		(0.0f, 0.0f, 0.0f);
STATIC const Rgba Rgba::RED		(1.0f, 0.0f, 0.0f);
STATIC const Rgba Rgba::LIME		(0.0f, 1.0f, 0.0f);
STATIC const Rgba Rgba::BLUE		(0.0f, 0.0f, 1.0f);
STATIC const Rgba Rgba::CYAN		(0.0f, 1.0f, 1.0f);
STATIC const Rgba Rgba::MAGENTA	(1.0f, 0.0f, 1.0f);
STATIC const Rgba Rgba::YELLOW	(1.0f, 1.0f, 0.0f);
STATIC const Rgba Rgba::SILVER	(.75f, .75f, .75f);
STATIC const Rgba Rgba::GRAY		(0.5f, 0.5f, 0.5f);
STATIC const Rgba Rgba::MAROON	(0.5f, 0.0f, 0.0f);
STATIC const Rgba Rgba::OLIVE		(0.5f, 0.5f, 0.0f);
STATIC const Rgba Rgba::GREEN		(0.0f, 0.5f, 0.0f);
STATIC const Rgba Rgba::PURPLE	(0.5f, 0.0f, 0.5f);
STATIC const Rgba Rgba::TEAL		(0.0f, 0.5f, 0.5f);
STATIC const Rgba Rgba::NAVY		(0.0f, 0.0f, 0.5f);

STATIC const Rgba Rgba::TRANSPARENT_BLACK(0.0f, 0.0f, 0.0f, 0.0f);
STATIC const Rgba Rgba::TRANSPARENT_WHITE(1.0f, 1.0f, 1.0f, 1.0f);
//////////////////////////////////////////////////////////////////////////
Rgba::Rgba()
{
}


//////////////////////////////////////////////////////////////////////////
Rgba::Rgba(float r, float g, float b, float a/*=1.f*/)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{
}

////////////////////////////////
Rgba::Rgba(unsigned int r, unsigned int g, unsigned b, unsigned int a/*=0xFFu*/)
	: r((float)r / 255.f)
	, g((float)g / 255.f)
	, b((float)b / 255.f)
	, a((float)a / 255.f)
{
}

////////////////////////////////
void Rgba::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, ',');
	float tr = 255.f;
	float tg = 255.f;
	float tb = 255.f;
	float ta = 255.f;
	if (splited.size() == 3) {
		tr = (float) std::atof(splited[0].c_str());
		tg = (float) std::atof(splited[1].c_str());
		tb = (float) std::atof(splited[2].c_str());
	} else if (splited.size() == 4) {
		tr = (float) std::atof(splited[0].c_str());
		tg = (float) std::atof(splited[1].c_str());
		tb = (float) std::atof(splited[2].c_str());
		ta = (float) std::atof(splited[3].c_str());
	} else {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to Rgba", FUNCTION, text));
		return;
	}
	r = FloatMap(tr, 0.f, 255.f, 0.f, 1.f);
	g = FloatMap(tg, 0.f, 255.f, 0.f, 1.f);
	b = FloatMap(tb, 0.f, 255.f, 0.f, 1.f);
	a = FloatMap(ta, 0.f, 255.f, 0.f, 1.f);
}

////////////////////////////////
bool Rgba::operator<(const Rgba& rhs) const
{
	return 
		r!=rhs.r ? r < rhs.r
		:(g != rhs.g ? g < rhs.g
		:(b != rhs.b ? b < rhs.b
		:(a != rhs.a ? a < rhs.a
		: false)));
}
