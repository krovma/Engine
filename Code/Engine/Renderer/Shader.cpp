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
static D3D11_COMPARISON_FUNC GetD3DComparisonFunc(CompareOperator op) {
	switch (op) {
	case COMPARE_NEVER:
		return D3D11_COMPARISON_NEVER;
	case COMPARE_ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	case COMPARE_EQ:
		return D3D11_COMPARISON_EQUAL;
	case COMPARE_NOTEQ:
		return D3D11_COMPARISON_NOT_EQUAL;
	case COMPARE_LESS:
		return D3D11_COMPARISON_LESS;
	case COMPARE_GREATER:
		return D3D11_COMPARISON_GREATER;
	case COMPARE_LESSEQ:
		return D3D11_COMPARISON_LESS_EQUAL;
	case COMPARE_GREATEREQ:
		return D3D11_COMPARISON_GREATER_EQUAL;
	default:
		return D3D11_COMPARISON_LESS_EQUAL;
	}
}
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
	, const std::string& filename
	, const char* entrypoint)
{
	ID3DBlob* bytecode = _CompileHLSL(
		shaderCode.c_str()
		, shaderCode.length()
		, entrypoint
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
	DX_SAFE_RELEASE(m_blendState);
	DX_SAFE_RELEASE(m_depthStencilState);
	DX_SAFE_RELEASE(m_rasterizerState);
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
bool Shader::CreateShaderFromFile(const std::string& filePath, const char* vertEntry, const char* pixelEntry)
{
	std::string sourceCode;
	LoadTextFileToString(filePath, sourceCode);
	m_vertexShader.CreateFromCode(g_theRenderer, sourceCode, SHADER_STAGE_VERTEX_SHADER, filePath, vertEntry);
	m_pixelShader.CreateFromCode(g_theRenderer, sourceCode, SHADER_STAGE_PIXEL_SHADER, filePath, pixelEntry);
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
	} else if (m_blendMode == BLEND_MODE_ADDITIVE) {
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
void Shader::SetDepthStencil(CompareOperator op, bool write)
{
	m_depthStencilOp = op;
	m_writeDepth = write;
}

////////////////////////////////
bool Shader::UpdateDepthStencil(const RenderContext* renderer)
{
	if (m_depthStencilDirty || (m_depthStencilState == nullptr)) {
		D3D11_DEPTH_STENCIL_DESC desc;
		memset(&desc, 0, sizeof(desc));

		desc.DepthEnable = TRUE;  // for simplicity, just set to true (could set to false if write is false and comprae is always)
		desc.DepthWriteMask = m_writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = GetD3DComparisonFunc(m_depthStencilOp);  

		// Stencil - just use defaults for now; 
		desc.StencilEnable = false;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;

		D3D11_DEPTH_STENCILOP_DESC opDesc;
		memset(&opDesc, 0, sizeof(opDesc));
		opDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;      // what to do if stencil fails
		opDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // What to do if stencil succeeds but depth fails
		opDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;      // what to do if the stencil succeeds
		opDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;      // function to test against

		// can have different rules setup for front and backface
		desc.FrontFace = opDesc;
		desc.BackFace = opDesc;

		DX_SAFE_RELEASE(m_depthStencilState);
		renderer->GetDevice()->CreateDepthStencilState(&desc, &m_depthStencilState);
		m_depthStencilDirty = false;
	}
	return m_depthStencilDirty;
}

////////////////////////////////
bool Shader::UpdateShaderStates(const RenderContext* renderer)
{
	UpdateBlendMode(renderer);
	UpdateDepthStencil(renderer);
	UpdateRasterizerStates(renderer);
	return true;
}

////////////////////////////////
bool Shader::UpdateRasterizerStates(const RenderContext* renderer)
{
	if (m_rasterizerDirty || m_rasterizerState == nullptr) {
		D3D11_RASTERIZER_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.CullMode = xml_cullMode;
		desc.FrontCounterClockwise = xml_FrontCCW;
		desc.FillMode = xml_fillMode;
		desc.DepthBias = 0;
		desc.AntialiasedLineEnable = FALSE;
		desc.DepthClipEnable = TRUE;
		renderer->GetDevice()->CreateRasterizerState(&desc, &m_rasterizerState);
		renderer->GetContext()->RSSetState(m_rasterizerState);
		m_rasterizerDirty = false;
	}
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
//////////////////////////////////////////////////////////////////////////
static CompareOperator __GetOperatorFromString(const std::string& str)
{
	if (str == "never") {
		return COMPARE_NEVER;
	} else if (str == "always"){
		return COMPARE_ALWAYS;
	} else if (str == "lt") {
		return COMPARE_LESS;
	} else if (str == "gt") {
		return COMPARE_GREATER;
	} else if (str == "leq") {
		return COMPARE_LESSEQ;
	} else if (str == "geq") {
		return COMPARE_GREATEREQ;
	} else if (str == "eq") {
		return COMPARE_EQ;
	} else if (str == "neq") {
		return COMPARE_NOTEQ;
	}
	return COMPARE_ALWAYS;
}
//////////////////////////////////////////////////////////////////////////
static BlendMode __GetBlendModeFromString(const std::string& str)
{
	if (str == "transparent") {
		return BLEND_MODE_ALPHA;
	} else if (str == "opaque") {
		return BLEND_MODE_OPAQUE;
	} else if (str == "additive") {
		return BLEND_MODE_ADDITIVE;
	}
	return BLEND_MODE_ALPHA;
}
//////////////////////////////////////////////////////////////////////////
static D3D11_FILL_MODE __GetD3DFillModeFromString(const std::string& str)
{
	if (str == "solid") {
		return D3D11_FILL_SOLID;
	} else {
		return D3D11_FILL_WIREFRAME;
	}
}
//////////////////////////////////////////////////////////////////////////
static D3D11_CULL_MODE __GetD3DCullModeFromString(const std::string& str)
{
	if (str == "none") {
		return D3D11_CULL_NONE;
	} else if (str == "front") {
		return D3D11_CULL_FRONT;
	} else {
		return D3D11_CULL_BACK;
	}
}

////////////////////////////////
STATIC Shader* Shader::CreateShaderFromXml(const std::string& xmlPath, RenderContext* renderer)
{
	Shader* createdShader = nullptr;
	XmlElement* xml = nullptr;
	ParseXmlFromFile(xml, xmlPath.c_str());
	XmlElement* pass = xml->FirstChildElement("pass");
	std::string src = ParseXmlAttr(*pass, "src", "");
	std::string xml_vertEntry = ParseXmlAttr(*pass->FirstChildElement("vert"), "entry", "Vert");
	std::string xml_pixelEntry = ParseXmlAttr(*pass->FirstChildElement("pixel"), "entry", "Vert");
	createdShader = renderer->AcquireShaderFromFile(src.c_str(), xml_vertEntry.c_str(), xml_pixelEntry.c_str());
	
	createdShader->m_writeDepth = ParseXmlAttr(*pass->FirstChildElement("depth"), "write", false);
	createdShader->m_depthStencilOp = __GetOperatorFromString(
		ParseXmlAttr(*pass->FirstChildElement("depth"), "test", "geq")
	);
	createdShader->m_blendMode = __GetBlendModeFromString(
		ParseXmlAttr(*pass->FirstChildElement("blend"), "mode", "opaque")
	);
	createdShader->xml_fillMode = __GetD3DFillModeFromString(
		ParseXmlAttr(*pass->FirstChildElement("raster"), "fill", "wire")
	);
	createdShader->xml_cullMode = __GetD3DCullModeFromString(
		ParseXmlAttr(*pass->FirstChildElement("raster"), "cull", "none")
	);
	createdShader->xml_FrontCCW = ParseXmlAttr(*pass->FirstChildElement("raster"), "frontCCW", false);
	createdShader->UpdateShaderStates(renderer);
	return createdShader;
}