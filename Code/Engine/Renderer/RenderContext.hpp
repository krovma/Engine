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
class RenderTargetView;
class Texture;
class Shader;
class RenderBuffer;
class ConstantBuffer;
class VertexBuffer;
//////////////////////////////////////////////////////////////////////////
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
//////////////////////////////////////////////////////////////////////////
enum ConstantBufferSlot : unsigned int
{
	CONSTANT_SLOT_FRAME = 1u,
	CONSTANT_SLOT_CAMERA = 2u,
};

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
	RenderContext(void* hWnd, unsigned int resWidth, unsigned int resHeight);

	// Getters
	ID3D11Device* GetDevice() const { return m_device; }
	ID3D11DeviceContext* GetContext() const { return m_context; }
	// Render stuff
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	RenderTargetView* GetFrameColorTarget() const;
	void ClearColorTarget(const Rgba &clearColor) const;
	void BindShader(Shader* shader);
	void BindConstantBuffer(ConstantBufferSlot slot, ConstantBuffer* buffer);
	void BindVertexBuffer(VertexBuffer* buffer) const;
	void BeginCamera(Camera &camera);
	void EndCamera(Camera &camera);
	void SetBlendMode(BlendMode mode);

	void Draw(int vertexCount, unsigned int byteOffset = 0u) const;

	void DrawVertexArray(int numVertices, const Vertex_PCU vertices[]) const;
	void DrawVertexArray(size_t numVertices, const std::vector<Vertex_PCU>& vertices) const;

	void DrawDisk(Vec2 center, float radius, const Rgba &color) const;

	Texture* AcquireTextureFromFile(const char* imageFilePath);
	void BindTexture(const Texture* texture) const;

	BitmapFont* AcquireBitmapFontFromFile(const char* fontName);

	Shader* AcquireShaderFromFile(const char* sourceFilePath);

private:
	void _loadBlendFunc();
	Texture* _CreateTextureFromFile(const char* imageFilePath);
	Shader* _CreateShaderFromFile(const char* sourceFilePath);
	std::map<std::string, Texture*> m_LoadedTexture;
	std::map<std::string, BitmapFont*> m_LoadedFont;
	std::map<std::string, Shader*> m_LoadedShader;
	BlendMode m_blendMode = ALPHA_BLEND;
	Camera* m_currentCamera = nullptr;
	VertexBuffer* m_immediateVBO;
	Shader* m_currentShader = nullptr;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	RenderTargetView* m_frameRenderTarget;
	ID3D11Texture2D* m_backBuffer = nullptr;
};

extern RenderContext* g_theRenderer;