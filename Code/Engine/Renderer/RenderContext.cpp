#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Image.hpp"
#include "ThirdParty/stb/stb_image.h"
#include <cstring>
#include <vector>

#include "Engine/Renderer/RenderCommon.hpp"
#define RENDER_DEBUG_LEAK
//#define RENDER_DEBUG_REPORT
////////////////////////////////
RenderContext::RenderContext(void* hWnd, unsigned int resWidth, unsigned int resHeight)
{
//Creating d3d rendering context
	UINT device_flags = 0U;
#if defined(RENDER_DEBUG_LEAK)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	// This flag fails unless we' do 11.1 (which we're not), and we query that
	// the adapter support its (which we're not).  Just here to let you know it exists.
	// device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

//Swap chain
	DXGI_SWAP_CHAIN_DESC swap_desc;
	memset(&swap_desc, 0, sizeof(swap_desc));

	// fill the swap chain description struct
	swap_desc.BufferCount = 2;                                    // two buffers (one front, one back?)

	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;
	swap_desc.OutputWindow = (HWND)hWnd;                                // the window to be copied to on present
	swap_desc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)
																  // Default options.
	swap_desc.Windowed = TRUE;                                    // windowed/full-screen mode
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	swap_desc.BufferDesc.Width = resWidth;
	swap_desc.BufferDesc.Height = resHeight;


	// Actually Create
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(nullptr, // Adapter, if nullptr, will use adapter window is primarily on.
		D3D_DRIVER_TYPE_HARDWARE,  // Driver Type - We want to use the GPU (HARDWARE)
		nullptr,                   // Software Module - DLL that implements software mode (we do not use)
		device_flags,              // device creation options
		nullptr,                   // feature level (use default)
		0U,                        // number of feature levels to attempt
		D3D11_SDK_VERSION,         // SDK Version to use
		&swap_desc,                // Description of our swap chain
		&m_swapChain,            // Swap Chain we're creating
		&m_device,               // [out] The device created
		nullptr,                   // [out] Feature Level Acquired
		&m_context);            // Context that can issue commands on this pipe.

								  // SUCCEEDED & FAILED are macros provided by Windows to checking
								  // the results.  Almost every D3D call will return one - be sure to check it.
	GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to create D3D render context\n");

	ID3D11RasterizerState *pstate;
	D3D11_RASTERIZER_DESC rstate;
	memset(&rstate, 0, sizeof(rstate));
	rstate.CullMode = D3D11_CULL_NONE;
	rstate.FillMode = D3D11_FILL_SOLID;
	rstate.DepthBias = 0;
	rstate.AntialiasedLineEnable = FALSE;
	rstate.DepthClipEnable = TRUE;
	m_device->CreateRasterizerState(&rstate, &pstate);
	m_context->RSSetState(pstate);
	DX_SAFE_RELEASE(pstate);
	m_immediateVBO = new VertexBuffer(this);
#if defined(RENDER_DEBUG_REPORT)
	hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debug));
	if (SUCCEEDED(hr)) {
		DebuggerPrintf("Debug context created!!!!!!\n");
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::Startup()
{
	Sampler* point = new Sampler();
	point->SetFilterModes(FILTER_MODE_POINT, FILTER_MODE_POINT);
	m_cachedSamplers[SAMPLER_POINT] = point;
	Sampler* linear = new Sampler();
	linear->SetFilterModes(FILTER_MODE_LINEAR, FILTER_MODE_LINEAR);
	m_cachedSamplers[SAMPLER_LINEAR] = linear;

	Image* whitepx = new Image(1, 1, "White");
	whitepx->SetTexelColor(0, 0, Rgba::WHITE);
	Texture2D* whiteTexture = new Texture2D(this);
	whiteTexture->LoadFromImage(whitepx);
	m_LoadedTexture["White"] = whiteTexture;
	m_cachedTextureView[whiteTexture] = whiteTexture->CreateTextureView();
}

void RenderContext::BeginFrame()
{
	m_backBuffer = nullptr;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_backBuffer);
	m_frameRenderTarget = new RenderTargetView();
	m_frameRenderTarget->_CreateFromInternalTexture(m_device, m_backBuffer);
	DX_SAFE_RELEASE(m_backBuffer); //release my hold on it (does not destroy it!)
}

void RenderContext::EndFrame()
{
	m_swapChain->Present(0, // Sync Interval, set to 1 for VSync
		0);                    // Present flags, see;
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
	delete m_frameRenderTarget;
	m_frameRenderTarget = nullptr;
	// #SD2ToDo: just update the m_rtv (being sure to release the old one)
}

void RenderContext::Shutdown()
{
	for (auto eachShader : m_LoadedShader) {
		delete eachShader.second;
	}
	m_LoadedShader.clear();
	for (auto eachSampler : m_cachedSamplers) {
		delete eachSampler;
	}
	for (auto eachTexture : m_LoadedTexture) {
		delete eachTexture.second;
	}
	m_LoadedTexture.clear();
	for (auto eachTextureView : m_cachedTextureView) {
		delete eachTextureView.second;
	}
	m_cachedTextureView.clear();

	delete m_immediateVBO;
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_context);
	DX_SAFE_RELEASE(m_device);
#if defined(RENDER_DEBUG_REPORT)
	m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	DX_SAFE_RELEASE(m_debug);
#endif
}

////////////////////////////////
RenderTargetView* RenderContext::GetFrameColorTarget() const
{
	return m_frameRenderTarget;
}

void RenderContext::ClearColorTarget(const Rgba &clearColor) const
{
	m_context->ClearRenderTargetView(m_frameRenderTarget->m_renderTargetView, (const FLOAT *)&clearColor);
}

////////////////////////////////
void RenderContext::BindShader(Shader* shader)
{
	m_currentShader = shader;
	m_context->VSSetShader(shader->GetVertexShader(), nullptr, 0u);
	m_context->PSSetShader(shader->GetPixelShader(), nullptr, 0u);
}

////////////////////////////////
void RenderContext::BindConstantBuffer(ConstantBufferSlot slot, ConstantBuffer* buffer)
{
	ID3D11Buffer *buf = (buffer != nullptr) ? (buffer->GetHandle()) : nullptr;
	m_context->VSSetConstantBuffers(slot, 1u, &buf);
	m_context->PSSetConstantBuffers(slot, 1u, &buf);
}

////////////////////////////////
void RenderContext::BindVertexBuffer(VertexBuffer* buffer) const
{
	unsigned int stride = sizeof(Vertex_PCU);
	unsigned int offset = 0;
	ID3D11Buffer* buf = buffer->GetHandle();
	m_context->IASetVertexBuffers(0, 1, &buf, &stride, &offset);
}

////////////////////////////////
void RenderContext::BindIndexBuffer(IndexBuffer* buffer) const
{
	ID3D11Buffer *handle = nullptr;
	if (buffer != nullptr) {
		handle = buffer->GetHandle();
	}
	m_context->IASetIndexBuffer(handle, DXGI_FORMAT_R32_UINT, 0);
}

void RenderContext::BeginCamera(Camera &camera)
{
	// Set Render target
	m_currentCamera = &camera;
	RenderTargetView* renderTarget = m_currentCamera->GetRenderTarget();
	//#SD2ToDo: If view is nullptr, use the frame's backbuffer; 
	m_context->OMSetRenderTargets(1u, &(renderTarget->m_renderTargetView), nullptr);
	
	// Set Viewport
	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.TopLeftX = 0u;
	viewport.TopLeftY = 0u;
	viewport.Width = renderTarget->GetWidth();
	viewport.Height = renderTarget->GetHeight();
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_context->RSSetViewports(1, &viewport);
	m_currentCamera->UpdateConstantBuffer(this);
	BindConstantBuffer(CONSTANT_SLOT_CAMERA, m_currentCamera->GetConstantBuffer());
}

void RenderContext::EndCamera(Camera &camera)
{
	UNUSED(camera);
	m_context->OMSetRenderTargets(0u, nullptr, nullptr);
	m_currentCamera = nullptr;
}

////////////////////////////////
void RenderContext::SetBlendMode(BlendMode mode)
{
	m_blendMode = mode;
	_loadBlendFunc();
}

////////////////////////////////
void RenderContext::Draw(int vertexCount, unsigned int byteOffset/*=0u*/) const
{
	m_currentShader->UpdateBlendMode(this);
	static float black[] = { 0.f,0.f,0.f,1.f };
	m_context->OMSetBlendState(m_currentShader->GetBlendState(), black, 0xffffffff);

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	bool result = m_currentShader->CreateVertexPCULayout(this);
	GUARANTEE_OR_DIE(result, "Can not crate input layout\n");
	m_context->IASetInputLayout(m_currentShader->GetVertexPCULayout());
	m_context->Draw((UINT)vertexCount, byteOffset);
}

////////////////////////////////
void RenderContext::DrawIndexed(int count)
{
	m_currentShader->UpdateBlendMode(this);
	static float black[] = { 0.f,0.f,0.f,1.f };
	m_context->OMSetBlendState(m_currentShader->GetBlendState(), black, 0xffffffff);

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	bool result = m_currentShader->CreateVertexPCULayout(this);
	GUARANTEE_OR_DIE(result, "Can not crate input layout\n");
	m_context->IASetInputLayout(m_currentShader->GetVertexPCULayout());
	m_context->DrawIndexed(count, 0, 0);
}

void RenderContext::DrawVertexArray(int numVertices, const Vertex_PCU vertices[]) const
{
	m_immediateVBO->Buffer(vertices, numVertices);
	BindVertexBuffer(m_immediateVBO);
	Draw(numVertices);
}

////////////////////////////////
void RenderContext::DrawVertexArray(size_t numVertices, const std::vector<Vertex_PCU>& vertices) const
{
	DrawVertexArray((int)numVertices, vertices.data());
}

////////////////////////////////
void RenderContext::DrawMesh(GPUMesh& mesh)
{
	BindVertexBuffer(mesh.GetVertexBuffer());
	BindIndexBuffer(mesh.GetIndexBuffer());

	if (mesh.IsUsingIndexBuffer()) {
		DrawIndexed(mesh.GetElementCount());
	} else {
		Draw(mesh.GetElementCount());
	}
}

////////////////////////////////
Texture2D* RenderContext::_CreateTextureFromFile(const char* imageFilePath)
{
	Texture2D* textureCreated = new Texture2D(this);
	textureCreated->LoadFromFile(imageFilePath);
	return textureCreated;
}

////////////////////////////////
TextureView2D* RenderContext::AcquireTextureViewFromFile(const char* imageFilePath)
{
	if (m_LoadedTexture.find(imageFilePath) == m_LoadedTexture.end()) {
		m_LoadedTexture[imageFilePath] = _CreateTextureFromFile(imageFilePath);
	}
	Texture2D* tex = m_LoadedTexture[imageFilePath];
	if (m_cachedTextureView.find(tex) == m_cachedTextureView.end()) {
		TextureView2D* view = tex->CreateTextureView();
		view->SetSampler(m_cachedSamplers[SAMPLER_DEFAULT]);
		m_cachedTextureView[tex] = view;
	}
	return m_cachedTextureView[tex];
}

////////////////////////////////
void RenderContext::BindTextureViewWithSampler(unsigned int slot, const TextureView2D* texture) const
{
	BindTextureView(slot, texture);
	if (texture != nullptr) {
		BindSampler(slot, texture->GetSampler());
	} else {
		BindSampler(slot, nullptr);
	}
}

////////////////////////////////
void RenderContext::BindTextureView(unsigned int slot, const TextureView2D* texture) const
{
	ID3D11ShaderResourceView *rsView = nullptr;
	if (texture != nullptr) {
		rsView = texture->GetView();
	} else {
		Texture2D* white = m_LoadedTexture.find("White")->second;
		rsView = m_cachedTextureView.find(white)->second->GetView();
	}
	m_context->PSSetShaderResources(slot, 1u, &rsView);
}

////////////////////////////////
void RenderContext::BindSampler(unsigned int slot, Sampler* sampler) const
{
	if (sampler == nullptr) {
		sampler = m_cachedSamplers[SAMPLER_DEFAULT];
	}
	sampler->CreateState(this);
	ID3D11SamplerState *state = sampler->GetSampleState();
	m_context->PSSetSamplers(slot, 1u, &state);

}

////////////////////////////////
void RenderContext::BindSampler(unsigned int slot, PresetSamplers sampler) const
{
	BindSampler(slot, m_cachedSamplers[sampler]);
}

////////////////////////////////
BitmapFont* RenderContext::AcquireBitmapFontFromFile(const char* fontName)
{
	if (m_LoadedFont.find(fontName) == m_LoadedFont.end()) {
		TextureView2D* fontTexture = AcquireTextureViewFromFile((std::string("Data/Fonts/") + fontName + ".png").c_str());
		m_LoadedFont[fontName] = new BitmapFont(fontName, fontTexture);
	}
	return m_LoadedFont[fontName];
}

////////////////////////////////
Shader* RenderContext::_CreateShaderFromFile(const char* sourceFilePath)
{
	Shader* createdShader = new Shader();
	// The shader class assume there is only one render context
	//(g_theRenderer) in the world
	createdShader->CreateShaderFromFile(sourceFilePath);
	if (createdShader != nullptr && createdShader->IsValid()) {
		return createdShader;
	} else {
		delete createdShader;
		ERROR_AND_DIE("Fail to create shader from file\n");
	}
	//return nullptr;
}

////////////////////////////////
Shader* RenderContext::AcquireShaderFromFile(const char* sourceFilePath)
{
	if (m_LoadedShader.find(sourceFilePath) == m_LoadedShader.end()) {
		m_LoadedShader[sourceFilePath] = _CreateShaderFromFile(sourceFilePath);
	}
	return m_LoadedShader[sourceFilePath];
}

////////////////////////////////
void RenderContext::_loadBlendFunc()
{
	m_currentShader->SetBlendMode(m_blendMode);
}

#endif