#include "Engine/Audio/AudioCommon.hpp"
#include "ThirdParty/fmod/fmod.hpp"
#include <cmath>
////////////////////////////////
inline FMOD_VECTOR Vec3ToFMOD(const Vec3& vec)
{
	return { vec.x, vec.y, vec.z };
}

////////////////////////////////
inline float DBToVolume(float dB)
{
	// Volume = 10^(dB/20);
	return powf(10.f, dB / 20.f);
}

////////////////////////////////
inline float VolumeToDB(float volume)
{
	return 20.f * log10f(volume);
}
