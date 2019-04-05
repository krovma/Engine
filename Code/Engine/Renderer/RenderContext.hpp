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
class IndexBuffer;
class Sampler;
class GPUMesh;
class DepthStencilTargetView;
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
	struct Light
	{
		Vec3 color=Vec3::ONE; float intensity=0.f;
		Vec3 position=Vec3::ZERO; float isDirectional=0.f;
		Vec3 direction=Vec3::ONE; float _;
		Vec3 diffuseAttenuatio = Vec3::ZERO; float __;
		Vec3 specularAttenuation = Vec3::ZERO; float ___;
	};
	struct LightBufferContent
	{
		Rgba ambient=Rgba::TRANSPARENT_WHITE; //A as intensity
		float specular_factor=0.f; float specular_power=17.32f;  Vec2 _;
		Light lights[8];
	};
public:
	RenderContext(void* hWnd, unsigned int resWidth, unsigned int resHeight);

	// Getters
	ID3D11Device* GetDevice() const { return m_device; }
	ID3D11DeviceContext* GetContext() const { return m_context; }
	IntVec2 GetResolution() const { return m_resolution; }
	// Render stuff
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	RenderTargetView* GetFrameColorTarget() const;
	DepthStencilTargetView* GetFrameDepthStencilTarget() const;
	void ClearColorTarget(const Rgba &clearColor) const;
	void ClearDepthStencilTarget(float depth = 1.0f, unsigned char stencil = 0u);
	void BindShader(Shader* shader);
	ConstantBuffer* GetModelBuffer() const { return m_modelBuffer; }
	ConstantBuffer* GetLightBuffer() const { return m_lightBuffer; }
	void BindConstantBuffer(ConstantBufferSlot slot, ConstantBuffer* buffer);
	void BindVertexBuffer(VertexBuffer* buffer) const;
	void BindIndexBuffer(IndexBuffer* buffer) const;
	void BeginCamera(Camera &camera);
	void EndCamera(Camera &camera);
	void SetBlendMode(BlendMode mode);

	void Draw(int vertexCount, unsigned int byteOffset = 0u) const;
	void DrawIndexed(int count);
	

	void DrawVertexArray(int numVertices, const Vertex_PCU vertices[]) const;
	void DrawVertexArray(size_t numVertices, const std::vector<Vertex_PCU>& vertices) const;
	void DrawMesh(GPUMesh& mesh);

	TextureView2D* AcquireTextureViewFromFile(const char* imageFilePath);
	void BindTextureViewWithSampler(unsigned int slot, const TextureView2D* texture) const;
	void BindTextureView(unsigned int slot, const TextureView2D* texture) const;
	void BindSampler(unsigned int slot, Sampler* sampler) const;
	void BindSampler(unsigned int slot, PresetSamplers sampler) const;

	BitmapFont* AcquireBitmapFontFromFile(const char* fontName);

	Shader* AcquireShaderFromFile(const char* sourceFilePath, const char* vertEntry, const char* pixelEntry);

	//Lighting
	void SetAmbientLight(const Rgba& color, float intensity);
	void SetSpecularFactors(float factor, float power);
	void EnableLight(int lightSlot, const Light& lightInfo);
	void DisableLight(int lightSlot);

private:
	void _loadBlendFunc();
	Texture2D* _CreateTextureFromFile(const char* imageFilePath);
	Shader* _CreateShaderFromFile(const char* sourceFilePath, const char* vertEntry, const char* pixelEntry);
	std::map<std::string, Texture2D*> m_LoadedTexture;
	std::map<Texture2D*, TextureView2D*> m_cachedTextureView;
	std::map<std::string, BitmapFont*> m_LoadedFont;
	std::map<std::string, Shader*> m_LoadedShader;
	BlendMode m_blendMode = BLEND_MODE_ALPHA;
	Camera* m_currentCamera = nullptr;
	ConstantBuffer* m_modelBuffer = nullptr;
	ConstantBuffer* m_lightBuffer = nullptr;
	LightBufferContent m_cpuLightBuffer;
	bool m_lightDirty=true;
	VertexBuffer* m_immediateVBO = nullptr;
	GPUMesh* m_immediateMesh = nullptr;
	Shader* m_currentShader = nullptr;
	Sampler* m_cachedSamplers[NUM_PRESET_SAMPLERS];
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	RenderTargetView* m_frameRenderTarget;
	ID3D11Texture2D* m_backBuffer = nullptr;
	Texture2D* m_defaultDepthStencilTexture = nullptr;
	DepthStencilTargetView* m_defaultDepthSencilTargetView = nullptr;
	IntVec2 m_resolution;

#if defined(RENDER_DEBUG)
	ID3D11Debug* m_debug = nullptr;
#endif
};

extern RenderContext* g_theRenderer;