#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include <stddef.h>
// NEEDED FOR COMPILING
// Note:  This is not allowed for Windows Store Apps.
// Shaders must be compiled to byte-code offline. 
// but for development - great if the program knows how to compile it.
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )
//////////////////////////////////////////////////////////////////////////
//// Used in this file ("Shader.cpp") only
ID3DBlob* _CompileHLSL(const void* code, size_t codeSize, const char* entryPoint, const char* shaderModel, const char* codeFileName);
//////////////////////////////////////////////////////////////////////////

////////////////////////////////
ShaderStage::ShaderStage()
	: m_handle(nullptr)
{
}

////////////////////////////////
ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE(m_bytecode);
	DX_SAFE_RELEASE(m_handle);
}

////////////////////////////////
void ShaderStage::CreateFromCode(
	const RenderContext* renderContext
	, const std::string& shaderCode
	, ShaderStageType stageType
	, const std::string& filename)
{
	ID3DBlob* bytecode = _CompileHLSL(
		shaderCode.c_str()
		, shaderCode.length()
		, Shader::GetShaderStageEntryName(stageType)
		, Shader::GetShaderModel(stageType)
		, filename.c_str()
	);
	if (bytecode == nullptr) {
		ERROR_AND_DIE(Stringf("Error on create shader from %s", filename.c_str()));
	}
	m_bytecode = bytecode;
	m_stageType = stageType;
	HRESULT hr;
	switch (stageType) {
	case SHADER_STAGE_VERTEX_SHADER: {
		hr = renderContext->GetDevice()->CreateVertexShader(
			bytecode->GetBufferPointer()
			, bytecode->GetBufferSize()
			, nullptr
			, &m_vertexShader
		);
		break;
	}
	case SHADER_STAGE_PIXEL_SHADER: {
		hr = renderContext->GetDevice()->CreatePixelShader(
			bytecode->GetBufferPointer()
			, bytecode->GetBufferSize()
			, nullptr
			, &m_pixelShader
		);
		break;
	}
	default: {
		ERROR_AND_DIE("Creation for certain shader stage not implemented");
	}
	}
	if (FAILED(hr)) {
		ERROR_AND_DIE("Failed to create shader stage from bytecode\n");
	}
}

////////////////////////////////
Shader::~Shader()
{
	DX_SAFE_RELEASE(m_inputLayout);
}

////////////////////////////////
STATIC const char* Shader::GetShaderStageEntryName(ShaderStageType stageType)
{
	switch (stageType) {
	case SHADER_STAGE_VERTEX_SHADER: {
		return "Vert";
	}
	case SHADER_STAGE_PIXEL_SHADER: {
		return "Pixel";
	}
	default: {
		ERROR_AND_DIE("Unimplemented shader stage");
	}
	}
}

////////////////////////////////
STATIC const char* Shader::GetShaderModel(ShaderStageType stageType)
{
	switch (stageType) {
	case SHADER_STAGE_VERTEX_SHADER: {
		return "vs_5_0";
	}
	case SHADER_STAGE_PIXEL_SHADER: {
		return "ps_5_0";
	}
	default: {
		ERROR_AND_DIE("Unimplemented shader stage");
	}
	}
}

////////////////////////////////
bool Shader::CreateShaderFromFile(const std::string& filePath)
{
	std::string sourceCode;
	LoadTextFileToString(filePath, sourceCode);
	m_vertexShader.CreateFromCode(g_theRenderer, sourceCode, SHADER_STAGE_VERTEX_SHADER, filePath);
	m_pixelShader.CreateFromCode(g_theRenderer, sourceCode, SHADER_STAGE_PIXEL_SHADER, filePath);
	return IsValid();
}

////////////////////////////////
ID3D11VertexShader* Shader::GetVertexShader() const
{
	return m_vertexShader.m_vertexShader;
}

////////////////////////////////
ID3D11PixelShader* Shader::GetPixelShader() const
{
	return m_pixelShader.m_pixelShader;
}

////////////////////////////////
ID3D11InputLayout* Shader::GetVertexPCULayout() const
{
	return m_inputLayout;
}

////////////////////////////////
bool Shader::CreateVertexPCULayout(const RenderContext* renderer)
{
	if (m_inputLayout != nullptr) {
		return true;
	}
	D3D11_INPUT_ELEMENT_DESC desc[3];
	memset(desc, 0, sizeof(desc));

	desc[0].SemanticName = "POSITION";
	desc[0].SemanticIndex = 0;
	desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[0].InputSlot = 0u;
	desc[0].AlignedByteOffset = offsetof(Vertex_PCU, m_position);
	desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[0].InstanceDataStepRate = 0u;

	desc[1].SemanticName = "COLOR";
	desc[1].SemanticIndex = 0;
	desc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc[1].InputSlot = 0u;
	desc[1].AlignedByteOffset = offsetof(Vertex_PCU, m_color);
	desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[1].InstanceDataStepRate = 0u;

	desc[2].SemanticName = "TEXCOORD";
	desc[2].SemanticIndex = 0;
	desc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	desc[2].InputSlot = 0u;
	desc[2].AlignedByteOffset = offsetof(Vertex_PCU, m_uvTexCoords);
	desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[2].InstanceDataStepRate = 0u;

	ID3DBlob* shaderByteCode = m_vertexShader.GetBytecode();
	HRESULT hr = renderer->GetDevice()->CreateInputLayout(
		desc
		, ARRAYSIZE(desc)
		, shaderByteCode->GetBufferPointer()
		, shaderByteCode->GetBufferSize()
		, &m_inputLayout
	);
	return SUCCEEDED(hr);
}

////////////////////////////////
bool Shader::IsValid() const
{
	return (m_vertexShader.IsValid() && m_pixelShader.IsValid());
}

////////////////////////////////
void Shader::SetBlendMode(BlendMode blendMode)
{
	m_blendMode = blendMode;
	m_blendModeDirty = true;
}

////////////////////////////////
bool Shader::UpdateBlendMode(const RenderContext* renderer)
{
	if (!m_blendModeDirty) {
		return true;
	}
	DX_SAFE_RELEASE(m_blendState);

	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0].BlendEnable = true;
	if (m_blendMode == BLEND_MODE_ALPHA) {
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	} else if (m_blendMode == BLEND_MODE_OPAQUE) {
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	} else if (m_blendMode == BLEND_MODE_ADDTIVE) {
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	} else {
		ERROR_AND_DIE("Blend mode unimplemented\n");
	}
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT hr = renderer->GetDevice()->CreateBlendState(&desc, &m_blendState);
	GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to create blend state\n");
	m_blendModeDirty = false;
	return true;
}

////////////////////////////////
/**/
ID3DBlob* _CompileHLSL(const void* code, size_t codeSize, const char* entryPoint, const char* shaderModel, const char* codeFileName)
{
	DWORD compileFlags = 0U;
#if defined(ENGINE_DEBUG_SHADERS)
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	//compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else 
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
#endif

	ID3DBlob *bytecode = nullptr;
	ID3DBlob *errors = nullptr;

	HRESULT hr = D3DCompile(
		code
		, codeSize
		, codeFileName
		, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, entryPoint
		, shaderModel
		, compileFlags
		, 0u
		, &bytecode
		, &errors
	);

	if (FAILED(hr) || errors != nullptr) {
		if (errors != nullptr) {
			char* what = (char*)errors->GetBufferPointer();
			DebuggerPrintf("Failed to compile [%s].  Compiler gave the following output;\n%s",
				codeFileName,
				what);
			DX_SAFE_RELEASE(errors);
		} else {
			DebuggerPrintf("Failed to compile [%s] HRESULT= %u\n", codeFileName, hr);
		}
	}
	return bytecode;
}
