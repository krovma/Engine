#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//////////////////////////////////////////////////////////////////////////
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D10Blob;
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
	ID3D10Blob* GetBytecode() const { return m_bytecode; }
private:
	union
	{
		ID3D11Resource* m_handle;
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
	};
	ShaderStageType m_stageType;
	ID3D10Blob* m_bytecode;
};

class Shader
{
public:
	Shader() = default;
	~Shader();
	static const char* GetShaderStageEntryName(ShaderStageType stageType);
	static const char* GetShaderModel(ShaderStageType stageType);
public:
	bool CreateShaderFromFile(const std::string& filePath);
	ID3D11VertexShader* GetVertexShader() const;
	ID3D11PixelShader* GetPixelShader() const;
	ID3D11InputLayout* GetVertexPCULayout() const;
	bool CreateVertexPCULayout(const RenderContext* renderer);
	bool IsValid() const;
private:
	ShaderStage m_vertexShader;
	ShaderStage m_pixelShader;
	ID3D11InputLayout* m_inputLayout = nullptr;
};


