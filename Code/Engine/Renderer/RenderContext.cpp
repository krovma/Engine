#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "ThirdParty/stb/stb_image.h"
#include <cstring>
#include <vector>

#include "Engine/Renderer/RenderCommon.hpp"

////////////////////////////////
RenderContext::RenderContext(void* hWnd, unsigned int resWidth, unsigned int resHeight)
{
//Creating d3d rendering context
	UINT device_flags = 0U;
#if defined(RENDER_DEBUG)
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
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::Startup()
{
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
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_context);
	DX_SAFE_RELEASE(m_device);
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
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	bool result = m_currentShader->CreateVertexPCULayout(this);
	GUARANTEE_OR_DIE(result, "Can not crate input layout\n");
	m_context->IASetInputLayout(m_currentShader->GetVertexPCULayout());
	m_context->Draw((UINT)vertexCount, byteOffset);
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

void RenderContext::DrawDisk(Vec2 center, float radius, const Rgba &color) const
{
	UNUSED(center);
	UNUSED(radius);
	UNUSED(color);
	ERROR_RECOVERABLE("D3d version unimplemented\n");
// 	Vec2 verts[33];
// 	for (int i = 0; i < 32; ++i) {
// 		verts[i] = center + Vec2::MakeFromPolarDegrees(((float)i / 32.f * 360.f), radius);
// 	}
// 	verts[32] = verts[0];
// 	glBegin(GL_TRIANGLES);
// 	{
// 		glColor4f(color.r, color.g, color.b, color.a);
// 		for (int i = 0; i < 32; ++i) {
// 			const Vec2 &crt = verts[i];
// 			const Vec2 &nxt = verts[i + 1];
// 			glVertex3f(crt.x, crt.y, 0.f);
// 			glVertex3f(nxt.x, nxt.y, 0.f);
// 			glVertex3f(center.x, center.y, 0.f);
// 		}
// 	}
// 	glEnd();
}

////////////////////////////////
Texture* RenderContext::_CreateTextureFromFile(const char* imageFilePath)
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

	ERROR_RECOVERABLE("D3d version unimplemented\n");
//  unsigned int textureID = 0;
// 	glEnable(GL_TEXTURE_2D);
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 	glGenTextures(1, (GLuint *)&textureID);
// 
// 	textureCreated->SetTextureID(textureID);
// 	textureCreated->SetTextureSize(imageTexelSizeX, imageTexelSizeY);
// 	m_LoadedTexture[imageFilePath] = textureCreated;
// 
// 	glBindTexture(GL_TEXTURE_2D, textureID);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
// 
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// 
// 	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
// 	if (numComponent == 3) {
// 		bufferFormat = GL_RGB;
// 	}
// 	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL
// 
// 										  // Upload the image texel data (raw pixels bytes) to OpenGL under this textureID
// 	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
// 		GL_TEXTURE_2D,		// Creating this as a 2d texture
// 		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
// 		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
// 		imageTexelSizeX,	// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
// 		imageTexelSizeY,	// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
// 		0,					// Border size, in texels (must be 0 or 1, recommend 0)
// 		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
// 		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
// 		textureData);		// Address of the actual pixel data bytes/buffer in system memory
// 
// 							// Free the image data now that we've sent a copy of it down to the GPU to be stored in video memory
// 	stbi_image_free(textureData);

	return textureCreated;
}

////////////////////////////////
Texture* RenderContext::AcquireTextureFromFile(const char* imageFilePath)
{
	///// Search loaded texture
	/////
	if (m_LoadedTexture.find(imageFilePath) == m_LoadedTexture.end()) {
		m_LoadedTexture[imageFilePath] = _CreateTextureFromFile(imageFilePath);
	}
	return m_LoadedTexture[imageFilePath];
}

////////////////////////////////
void RenderContext::BindTexture(const Texture *texture) const
{
	UNUSED(texture);
	ERROR_RECOVERABLE("D3d version unimplemented\n");
// 	if (texture) {
// 		glEnable(GL_TEXTURE_2D);
// 		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
// 	} else {
// 		glDisable(GL_TEXTURE_2D);
// 		//glBindTexture((GL_TEXTURE_2D, 0);
// 	}
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
	ERROR_RECOVERABLE("D3d version unimplemented\n");
// 	glEnable(GL_BLEND);
// 	switch (m_blendMode) {
// 	case ALPHA_BLEND:
// 	{
// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 		break;
// 	}
// 	case ADDITIVE_BLEND:
// 	{
// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
// 		break;
// 	}
// 	case OVERRIDE_BLEND:
// 	{
// 		glBlendFunc(GL_ONE, GL_ZERO);
// 		break;
// 	}
// 	case MULTIPLY_BLEND:
// 	{
// 		glBlendFunc(GL_DST_ALPHA, GL_ZERO);
// 		break;
// 	}
// 	default:
// 	{
// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 		break;
// 	}
// 	}
}

#endif