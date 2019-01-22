#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "ThirdParty/stb/stb_image.h"
#include <windows.h>
#include <gl/GL.h>
#include <cstring>
#include <vector>
#pragma comment(lib, "opengl32")
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void RenderContext::Startup()
{
}

void RenderContext::BeginFrame()
{
	_loadBlendFunc();
}

void RenderContext::EndFrame()
{
	//SwapBuffers(_displayContext);
}

void RenderContext::Shutdown()
{

}

void RenderContext::ClearScreen(const Rgba &clearColor)
{
	//glClearColor(1, 1, 1, 1);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderContext::BeginCamera(const Camera &camera)
{
	glLoadIdentity();
	const Vec2 &btmLft = camera.GetOrthoBottomLeft();
	const Vec2 &topRt = camera.GetOrthoTopRight();
	glOrtho(btmLft.x, topRt.x, btmLft.y, topRt.y, 0.f, 1.f);
}

void RenderContext::EndCamera(const Camera &camera)
{
	UNUSED(camera);
}

////////////////////////////////
void RenderContext::SetBlendMode(BlendMode mode)
{
	m_blendMode = mode;
	_loadBlendFunc();
}

void RenderContext::DrawVertexArray(int numVertices, const Vertex_PCU vertices[]) const
{
	glBegin(GL_TRIANGLES);
	{
		for (int i = 0; i < numVertices; ++i) {
			const Vertex_PCU &crtVert = vertices[i];
			glColor4f(crtVert.m_color.r, crtVert.m_color.g, crtVert.m_color.b, crtVert.m_color.a);
			glTexCoord2f(crtVert.m_uvTexCoords.x, crtVert.m_uvTexCoords.y);
			glVertex3f(crtVert.m_position.x, crtVert.m_position.y, crtVert.m_position.z);
		}
	}
	glEnd();
}

////////////////////////////////
void RenderContext::DrawVertexArray(size_t numVertices, const std::vector<Vertex_PCU>& vertices) const
{
	glBegin(GL_TRIANGLES);
	{
		for (size_t i = 0; i < numVertices; ++i) {
			const Vertex_PCU &crtVert = vertices[i];
			glColor4f(crtVert.m_color.r, crtVert.m_color.g, crtVert.m_color.b, crtVert.m_color.a);
			glTexCoord2f(crtVert.m_uvTexCoords.x, crtVert.m_uvTexCoords.y);
			glVertex3f(crtVert.m_position.x, crtVert.m_position.y, crtVert.m_position.z);
		}
	}
	glEnd();
}

void RenderContext::DrawDisk(Vec2 center, float radius, const Rgba &color) const
{
	Vec2 verts[33];
	for (int i = 0; i < 32; ++i) {
		verts[i] = center + Vec2::MakeFromPolarDegrees(((float)i / 32.f * 360.f), radius);
	}
	verts[32] = verts[0];
	glBegin(GL_TRIANGLES);
	{
		glColor4f(color.r, color.g, color.b, color.a);
		for (int i = 0; i < 32; ++i) {
			const Vec2 &crt = verts[i];
			const Vec2 &nxt = verts[i + 1];
			glVertex3f(crt.x, crt.y, 0.f);
			glVertex3f(nxt.x, nxt.y, 0.f);
			glVertex3f(center.x, center.y, 0.f);
		}
	}
	glEnd();
}

////////////////////////////////
Texture* RenderContext::CreateTextureFromFile(const char* imageFilePath)
{
	///// Create new texture
	/////
	int imageTexelSizeX = 0;
	int imageTexelSizeY = 0;
	int numComponent = 0;
	int numComponentRequest = 0; // support both 3/4 channels

	stbi_set_flip_vertically_on_load(1);
	unsigned char* textureData = stbi_load(imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponent, numComponentRequest);
	GUARANTEE_OR_DIE(textureData, Stringf("Image %s didn't loaded", imageFilePath));
	GUARANTEE_OR_DIE(numComponent >= 3 && numComponent <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, "Broken Image!");

	Texture* textureCreated = new Texture();
	textureCreated->SetTextureName(imageFilePath);
	unsigned int textureID = 0;
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint *)&textureID);

	textureCreated->SetTextureID(textureID);
	textureCreated->SetTextureSize(imageTexelSizeX, imageTexelSizeY);
	m_LoadedTexture[imageFilePath] = textureCreated;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if (numComponent == 3) {
		bufferFormat = GL_RGB;
	}
	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

										  // Upload the image texel data (raw pixels bytes) to OpenGL under this textureID
	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		imageTexelSizeX,	// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
		imageTexelSizeY,	// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1, recommend 0)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
		textureData);		// Address of the actual pixel data bytes/buffer in system memory

							// Free the image data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free(textureData);

	return textureCreated;
}

////////////////////////////////
Texture* RenderContext::AcquireTextureFromFile(const char* imageFilePath)
{
	///// Search loaded texture
	/////
	if (m_LoadedTexture.find(imageFilePath) == m_LoadedTexture.end()) {
		m_LoadedTexture[imageFilePath] = CreateTextureFromFile(imageFilePath);
	}
	return m_LoadedTexture[imageFilePath];
}

////////////////////////////////
void RenderContext::BindTexture(const Texture *texture) const
{
	if (texture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	} else {
		glDisable(GL_TEXTURE_2D);
		//glBindTexture((GL_TEXTURE_2D, 0);
	}
}

////////////////////////////////
BitmapFont* RenderContext::AcquireBitmapFontFromFile(const char* fontName)
{
	if (m_LoadedFont.find(fontName) == m_LoadedFont.end()) {
		Texture* fontTexture = AcquireTextureFromFile((std::string("Data/Fonts/") + fontName + ".png").c_str());
		m_LoadedFont[fontName] = new BitmapFont(fontName, fontTexture);
	}
	return m_LoadedFont[fontName];
}

////////////////////////////////
void RenderContext::_loadBlendFunc()
{
	glEnable(GL_BLEND);
	switch (m_blendMode) {
	case ALPHA_BLEND:
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	}
	case ADDITIVE_BLEND:
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	}
	case OVERRIDE_BLEND:
	{
		glBlendFunc(GL_ONE, GL_ZERO);
		break;
	}
	case MULTIPLY_BLEND:
	{
		glBlendFunc(GL_DST_ALPHA, GL_ZERO);
		break;
	}
	default:
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	}
	}
}

#endif