#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include <map>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
class Camera;
class Texture;
//////////////////////////////////////////////////////////////////////////

enum BlendMode
{
	ALPHA_BLEND,
	ADDITIVE_BLEND,
	OVERRIDE_BLEND,
	MULTIPLY_BLEND
};

class RenderContext
{
public:

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void ClearScreen(const Rgba &clearColor);
	void BeginCamera(const Camera &camera);
	void EndCamera(const Camera &camera);
	void SetBlendMode(BlendMode mode);

	void DrawVertexArray(int numVertices, const Vertex_PCU vertices[]) const;
	void DrawVertexArray(size_t numVertices, const std::vector<Vertex_PCU>& vertices) const;

	void DrawDisk(Vec2 center, float radius, const Rgba &color) const;

	Texture* CreateTextureFromFile(const char* imageFilePath);
	Texture* AcquireTextureFromFile(const char* imageFilePath);
	void BindTexture(const Texture* texture) const;

	BitmapFont* AcquireBitmapFontFromFile(const char* fontName);

private:
	void _loadBlendFunc();
	std::map<std::string, Texture*> m_LoadedTexture;
	std::map<std::string, BitmapFont*> m_LoadedFont;
	BlendMode m_blendMode = ALPHA_BLEND;
};