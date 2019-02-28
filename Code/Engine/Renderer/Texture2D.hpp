#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

//////////////////////////////////////////////////////////////////////////
struct ID3D11Texture2D;
class RenderContext;
class Image;
class TextureView2D;
class DepthStencilTargetView;
//////////////////////////////////////////////////////////////////////////
#include "Engine/Renderer/RenderTypes.hpp"
//////////////////////////////////////////////////////////////////////////
class Texture
{
public:
	Texture(RenderContext* renderer);
	~Texture();
	std::string GetTextureName() const { return m_textureName; }
	void SetTextureName(const char* name) { m_textureName = name; }
	void SetTextureID(unsigned int textureID) { m_textureID = textureID; }
protected:
	RenderContext* m_renderer = nullptr;
	ID3D11Texture2D* m_handle = nullptr;
	GPUMemoryUsage m_memoryUsage = GPU_MEMORY_USAGE_IMMUTABLE;
	unsigned int m_textureUsage = TEXTURE_USAGE_TEXTURE;
	std::string m_textureName;
	unsigned int m_textureID;
};

class Texture2D : public Texture
{
public:
	static Texture2D* CreateDepthStencilTarget(RenderContext* renderer, int width, int height);
	static Texture2D* CreateDepthStencilTargetFor(Texture2D* colorTarget);
public:
	Texture2D(RenderContext* renderer);
	~Texture2D();

	bool LoadFromFile(const std::string& path);
	bool LoadFromImage(Image* image);
	IntVec2 GetTextureSize() const { return m_textureSize; }

	TextureView2D* CreateTextureView() const;
	bool CreateDepthStencilTarget(int width, int height);
	DepthStencilTargetView* CreateDepthStencilTargetView() const;
private:
	IntVec2 m_textureSize;
};
