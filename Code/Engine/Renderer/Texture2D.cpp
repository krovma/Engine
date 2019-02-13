#include "Game/EngineBuildPreferences.hpp"
//////////////////////////////////////////////////////////////////////////
#include "Engine/Core/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include <cstring>

////////////////////////////////
unsigned int GetD3DBind(unsigned int usage)
{
	unsigned int bind = 0u
		| ((usage & TEXTURE_USAGE_TEXTURE) ? D3D11_BIND_SHADER_RESOURCE : 0)
		| ((usage & TEXTURE_USAGE_COLOR_TARGET) ? D3D11_BIND_RENDER_TARGET : 0)
		| ((usage & TEXTURE_USAGE_DEPTH_STENCIL) ? D3D11_BIND_DEPTH_STENCIL : 0)
		;
	return bind;
}
////////////////////////////////
Texture::Texture(RenderContext* renderer)
	:m_renderer(renderer)
{
}

////////////////////////////////
Texture::~Texture()
{
	//Not a virtual function
	DX_SAFE_RELEASE(m_handle);
}

////////////////////////////////
Texture2D::Texture2D(RenderContext* renderer)
	:Texture(renderer)
{
}

////////////////////////////////
Texture2D::~Texture2D()
{
	//Do Nothing now
}

////////////////////////////////
bool Texture2D::LoadFromFile(const std::string& path)
{
	Image* image = Image::AcquireImage(path.c_str());
	return LoadFromImage(image);
}

////////////////////////////////
bool Texture2D::LoadFromImage(Image* image)
{
	DX_SAFE_RELEASE(m_handle);
	ID3D11Device* device = m_renderer->GetDevice();
	m_textureUsage = TEXTURE_USAGE_TEXTURE;
	m_memoryUsage = GPU_MEMORY_USAGE_GPU;
	
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	IntVec2 imageSize = image->GetImageSize();
	desc.Width = (unsigned int)imageSize.x;
	desc.Height = (unsigned int)imageSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Usage = GetD3DUsageFromGPUMemoryUsage(m_memoryUsage);
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = GetD3DBind(m_textureUsage);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	unsigned int pitch = (unsigned int)imageSize.x * 4;
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = image->GetRawImageData();
	data.SysMemPitch = pitch;

	ID3D11Texture2D *createdTexture = nullptr;
	HRESULT hr = device->CreateTexture2D(&desc, &data, &createdTexture);
	if (SUCCEEDED(hr)) {
		m_textureSize = imageSize;
		m_handle = createdTexture;
		return true;
	} else {
		ERROR_AND_DIE("Failed when creating texture\n");
		return false;
	}
}

////////////////////////////////
TextureView2D* Texture2D::CreateTextureView() const
{
	GUARANTEE_OR_DIE((m_handle != nullptr), "Null handle for texture on creating texture view\n");
	ID3D11Device* device = m_renderer->GetDevice();
	ID3D11ShaderResourceView* rsView = nullptr;
	HRESULT hrcheck = device->CreateShaderResourceView(m_handle, nullptr, nullptr);

	HRESULT hr = device->CreateShaderResourceView(m_handle, nullptr, &rsView);
	if (SUCCEEDED(hr)) {
		TextureView2D* createdTextureView = new TextureView2D();
		createdTextureView->m_view = rsView;
		m_handle->AddRef();
		createdTextureView->m_resource = m_handle;
		createdTextureView->m_size = m_textureSize;
		return createdTextureView;
	} else {
		ERROR_AND_DIE("Failed to created texture view\n");
	}
}

