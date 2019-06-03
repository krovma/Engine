#include "Engine/Audio/AudioCommon.hpp"
#include "ThirdParty/fmod/fmod.hpp"

////////////////////////////////
inline FMOD_VECTOR Vec3ToFMOD(const Vec3& vec)
{
	return { vec.x, vec.y, vec.z };
}
