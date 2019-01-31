#include "Engine/Renderer/Shader.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <d3d11.h>  
#include <DXGI.h>    
// D3D DEBUG 
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )
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
	
	m_stageType = stageType;

	switch (stageType) {
	case SHADER_STAGE_VERTEX_SHADER: {
		renderContext->GetDevice()->CreateVertexShader(
			bytecode
			, bytecode->GetBufferSize()
			, nullptr
			, &m_vertexShader
		);
		break;
	}
	case SHADER_STAGE_PIXEL_SHADER: {
		renderContext->GetDevice()->CreatePixelShader(
			bytecode
			, bytecode->GetBufferSize()
			, nullptr
			, &m_pixelShader
		);
		break;
	}
	default: {
		DX_SAFE_RELEASE(bytecode);
		ERROR_AND_DIE("Creation for certain shader stage not implemented");
	}
	}
	DX_SAFE_RELEASE(bytecode);
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
	int bufferSize = LoadTextFileToString(filePath, sourceCode);
	m_vertexShader.CreateFromCode(g_theRenderer, sourceCode, SHADER_STAGE_VERTEX_SHADER, filePath);
	m_pixelShader.CreateFromCode(g_theRenderer, sourceCode, SHADER_STAGE_PIXEL_SHADER, filePath);
	return (m_vertexShader.IsValid() && m_pixelShader.IsValid());
}

////////////////////////////////
/**/
ID3DBlob* _CompileHLSL(const void* code, size_t codeSize, const char* entryPoint, const char* shaderModel, const char* codeFileName)
{
	DWORD compileFlags = 0U;
#if defined(ENGINE_DEBUG_SHADERS)
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
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
