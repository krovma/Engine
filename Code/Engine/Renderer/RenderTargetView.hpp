#pragma once
#include "Engine/Math/Vec2.hpp"
//////////////////////////////////////////////////////////////////////////
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11Device;
//////////////////////////////////////////////////////////////////////////
class RenderTargetView
{
public:
	friend class RenderContext;
	RenderTargetView() {/*Do Nothing*/}
	~RenderTargetView();

	float GetHeight() const;
	float GetWidth() const;

private:
	void _CreateFromInternalTexture(ID3D11Device* device, ID3D11Texture2D* texture);
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	Vec2 m_size = Vec2::ZERO;
};