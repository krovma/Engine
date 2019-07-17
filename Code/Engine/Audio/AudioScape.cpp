#include "Engine/Audio/AudioScape.hpp"
#include "Engine/Core/EngineCommon.hpp"
////////////////////////////////
AudioScape2D::AudioScape2D(const char* path)
{
	std::string bakedPath = std::string(path) + ".baked";
	FILE* fp = ::fopen(bakedPath.c_str(), "rb");
	//std::ofstream fout(path + ".txt");
	FILE* fmap = ::fopen(path, "r");
	if (fp == NULL) {
		ERROR_AND_DIE(Stringf("Loading Audio map %s failed", path).c_str());
	}

	int w, h;
	::fread(&w, sizeof(int), 1, fp);
	::fread(&h, sizeof(int), 1, fp);
	m_size = IntVec2(w, h);


	m_data = new AudioPath2D*[w * h];
	m_map = new char[w * h];
	for (int x = 0; x < w; ++x) {
		for (int y = 0; y < h; ++y) {
			m_data[GetIndex(x, y)] = new AudioPath2D[w * h];
		}
	}

	int _t;
	fscanf(fmap, "%d %d", &_t, &_t);
	for (int y = h - 1; y >= 0; --y) {
		for (int x = 0; x < w; ++x) {
			fscanf(fmap, "%1d", &_t);
			m_map[GetIndex(x, y)] = char(_t);
		}
	}
	fclose(fmap);

	bool audiable;
	char reflection;
	float dist;
	IntVec2 from;
	for (int x = 0; x < w; ++x) {
		for (int y = 0; y < h; ++y) {
			for (int tx = 0; tx < w; ++tx) {
				for (int ty = 0; ty < h; ++ty) {
					AudioPath2D& info = m_data[GetIndex(x, y)][GetIndex(tx, ty)];
					::fread(&audiable, sizeof(bool), 1, fp);
					if (audiable) {
						::fread(&reflection, sizeof(char), 1, fp);
						::fread(&dist, sizeof(float), 1, fp);
						::fread(&from, sizeof(IntVec2), 1, fp);
						info.m_numReflection = reflection;
						info.m_distance = dist;
						info.m_dir = from;
					} else {
						info.m_distance = -1;
					}
				}
			}
		}
	}
	//::fclose(fmap);
	::fclose(fp);
}

////////////////////////////////
AudioScape2D::~AudioScape2D()
{
	for (int x = 0; x < m_size.x; ++x) {
		for (int y = 0; y < m_size.y; ++y) {
			delete[] m_data[GetIndex(x, y)];
		}
	}
	delete[] m_data;
	delete[] m_map;
}

////////////////////////////////
AudioPath2D AudioScape2D::GetPath(const IntVec2& from, const IntVec2& to) const
{
	return m_data[GetIndex(from.x, from.y)][GetIndex(to.x, to.y)];
}

////////////////////////////////
IntVec2 AudioScape2D::GetScapeDimension() const
{
	return m_size;
}

////////////////////////////////
int AudioScape2D::GetIndex(int x, int y) const
{
	return x * m_size.y + y;
}

////////////////////////////////
int AudioScape2D::GetMapTile(int x, int y) const
{
	return m_map[GetIndex(x, y)];
}

////////////////////////////////
float AudioPath2D::GetFrequencyRolloff() const
{
	float rolloff = 1.f;
	for (int t = 0; t < m_numReflection; ++t) {
		rolloff *= 0.8f;
	}
	return rolloff;
}

////////////////////////////////
bool AudioPath2D::IsAudible() const
{
	return m_distance >= 0;
}

////////////////////////////////
bool AudioPath2D::IsBlocked() const
{
	return m_numReflection != 0;
}
