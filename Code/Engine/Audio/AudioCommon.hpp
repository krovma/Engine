#pragma once

#include "Engine/Math/Vec3.hpp"
using FmodAssetID = size_t;
using FmodChannelID = size_t;
struct Vec3;
struct FMOD_VECTOR;

inline FMOD_VECTOR Vec3ToFMOD(const Vec3& vec);
