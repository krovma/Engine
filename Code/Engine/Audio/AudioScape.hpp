#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
struct AudioPath2D
{
public:
	int m_numReflection;
	float m_distance;
	IntVec2 m_dir = IntVec2::ZERO;
public:
	float GetFrequencyRolloff() const;
	bool IsAudible() const;
	bool IsBlocked() const;
};

class AudioScape2D
{
	friend class AudioSystem;
public:
	AudioPath2D GetPath(const IntVec2& from, const IntVec2& to) const;
	IntVec2 GetScapeDimension() const;
	int GetIndex(int x, int y) const;
	int GetMapTile(int x, int y) const;

	bool IsCoordValid(int x, int y) const
	{
		return 0 <= x && x < m_size.x && 0 <= y && y < m_size.y;
	}
private:
	AudioScape2D(const char* path);
	~AudioScape2D();
	IntVec2 m_size;
	// some huge data here
	AudioPath2D** m_data = nullptr;
	char* m_map = nullptr;
};