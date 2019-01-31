#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//////////////////////////////////////////////////////////////////////////
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
//////////////////////////////////////////////////////////////////////////

enum ShaderStageType
{
	SHADER_STAGE_VERTEX_SHADER,
	SHADER_STAGE_PIXEL_SHADER,
	NUM_SHADER_STAGE_TYPES
};

class ShaderStage
{
public:
	friend class Shader;
	ShaderStage();
	~ShaderStage();

	void CreateFromCode(
		const RenderContext* renderContext
		, const std::string& shaderCode
		, ShaderStageType stageType
		, const std::string& filename);
	bool IsValid() const { return m_handle != nullptr; }
private:
	union
	{
		ID3D11Resource* m_handle;
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
	};
	ShaderStageType m_stageType;
};

class Shader
{
public:
	static const char* GetShaderStageEntryName(ShaderStageType stageType);
	static const char* GetShaderModel(ShaderStageType stageType);
public:
	bool CreateShaderFromFile(const std::string& filePath);

private:
	ShaderStage m_vertexShader;
	ShaderStage m_pixelShader;
};


