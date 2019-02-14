#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include <map>
#include <string>
#include <vector>
#define RENDER_DEBUG
//////////////////////////////////////////////////////////////////////////
class Camera;
class RenderTargetView;
class Texture2D;
class TextureView2D;
class Shader;
class RenderBuffer;
class ConstantBuffer;
class VertexBuffer;
class Sampler;
//////////////////////////////////////////////////////////////////////////
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Debug;
//////////////////////////////////////////////////////////////////////////
#include "Engine/Renderer/RenderTypes.hpp"

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

	TextureView2D* AcquireTextureViewFromFile(const char* imageFilePath);
	void BindTextureViewWithSampler(unsigned int slot, const TextureView2D* texture) const;
	void BindTextureView(unsigned int slot, const TextureView2D* texture) const;
	void BindSampler(unsigned int slot, Sampler* sampler) const;
	void BindSampler(unsigned int slot, PresetSamplers sampler) const;

	BitmapFont* AcquireBitmapFontFromFile(const char* fontName);

	Shader* AcquireShaderFromFile(const char* sourceFilePath);

private:
	void _loadBlendFunc();
	Texture2D* _CreateTextureFromFile(const char* imageFilePath);
	Shader* _CreateShaderFromFile(const char* sourceFilePath);
	std::map<std::string, Texture2D*> m_LoadedTexture;
	std::map<Texture2D*, TextureView2D*> m_cachedTextureView;
	std::map<std::string, BitmapFont*> m_LoadedFont;
	std::map<std::string, Shader*> m_LoadedShader;
	BlendMode m_blendMode = BLEND_MODE_ALPHA;
	Camera* m_currentCamera = nullptr;
	VertexBuffer* m_immediateVBO;
	Shader* m_currentShader = nullptr;
	Sampler* m_cachedSamplers[NUM_PRESET_SAMPLERS];
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	RenderTargetView* m_frameRenderTarget;
	ID3D11Texture2D* m_backBuffer = nullptr;
#if defined(RENDER_DEBUG)
	ID3D11Debug* m_debug = nullptr;
#endif
};

extern RenderContext* g_theRenderer;