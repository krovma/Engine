#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/XmlUtils.hpp"
//////////////////////////////////////////////////////////////////////////
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D10Blob;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
//////////////////////////////////////////////////////////////////////////

#include "Engine/Renderer/RenderTypes.hpp"

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
	static Shader* CreateShaderFromXml(XmlElement* xml);
public:
	bool CreateShaderFromFile(const std::string& filePath);
	ID3D11VertexShader* GetVertexShader() const;
	ID3D11PixelShader* GetPixelShader() const;
	ID3D11InputLayout* GetVertexPCULayout() const;
	bool CreateVertexPCULayout(const RenderContext* renderer);
	bool IsValid() const;
	void SetBlendMode(BlendMode blendMode);
	bool UpdateBlendMode(const RenderContext* renderer);
	ID3D11BlendState* GetBlendState() const { return m_blendState; }
	ID3D11DepthStencilState* GetDepthStencilState() const { return m_depthStencilState; }
	void SetDepthStencil(CompareOperator op, bool write);
	bool UpdateDepthStencil(const RenderContext* renderer);
	bool UpdateShaderStates(const RenderContext* renderer);
private:
	ShaderStage m_vertexShader;
	ShaderStage m_pixelShader;
	ID3D11InputLayout* m_inputLayout = nullptr;
	BlendMode m_blendMode = BLEND_MODE_ALPHA;
	bool m_blendModeDirty = true;
	bool m_depthStencilDirty = true;
	ID3D11BlendState* m_blendState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	CompareOperator m_depthStencilOp = COMPARE_LESSEQ;
	bool m_writeDepth = false;
};


