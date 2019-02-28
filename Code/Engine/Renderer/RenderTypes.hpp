#pragma once
enum ConstantBufferSlot : unsigned int
{
	CONSTANT_SLOT_FRAME = 1u,
	CONSTANT_SLOT_CAMERA = 2u,
	CONSTANT_SLOT_MODEL = 3u
};

enum BlendMode
{
	BLEND_MODE_OPAQUE,
	BLEND_MODE_ALPHA,
	BLEND_MODE_ADDITIVE
};

enum RenderBufferUsage : unsigned int
{
	RENDER_BUFFER_USAGE_VERTEX = 0b1u,
	RENDER_BUFFER_USAGE_INDEX = 0b10u,
	RENDER_BUFFER_USAGE_CONSTANT = 0b100u,
};

enum GPUMemoryUsage
{
	GPU_MEMORY_USAGE_GPU,
	GPU_MEMORY_USAGE_IMMUTABLE,
	GPU_MEMORY_USAGE_DYNAMIC,
	GPU_MEMORY_USAGE_STAGING,
};

enum ShaderStageType
{
	SHADER_STAGE_VERTEX_SHADER,
	SHADER_STAGE_PIXEL_SHADER,
	NUM_SHADER_STAGE_TYPES
};

enum TextureUsage : unsigned int
{
	TEXTURE_USAGE_TEXTURE = 0b001u,
	TEXTURE_USAGE_COLOR_TARGET = 0b010u,
	TEXTURE_USAGE_DEPTH_STENCIL = 0b100u
};

enum FilterMode : int
{
	FILTER_MODE_POINT = 0,
	FILTER_MODE_LINEAR,

	NUM_FILTER_MODES
};

enum PresetSamplers
{
	SAMPLER_POINT = 0,
	SAMPLER_LINEAR,
	NUM_PRESET_SAMPLERS,
	SAMPLER_DEFAULT = SAMPLER_POINT,
};

enum CompareOperator
{
	COMPARE_NEVER,
	COMPARE_ALWAYS,
	COMPARE_EQ,
	COMPARE_NOTEQ,
	COMPARE_LESS,
	COMPARE_LESSEQ,
	COMPARE_GREATER,
	COMPARE_GREATEREQ
};