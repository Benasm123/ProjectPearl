#pragma once
#include <filesystem>

#define MASKABLE uint32_t

enum class ShaderInputType 
{
	UniformBuffer
};

typedef uint32_t ShaderStageMask;

enum ShaderStage : MASKABLE
{
	Vertex = 1 << 0,
	Fragment = 1 << 1,
	Compute = 1 << 2,
	Geometry = 1 << 3,
	Tesselation = 1 << 4
};

// TODO -> Add int types up to Vec 4 with unsighned variants.
enum class InputValueType
{
	FLOAT32,
	VEC2FLOAT32,
	VEC3FLOAT32
};

struct ShaderInputs 
{
	ShaderInputType inputType;
	ShaderStageMask shaderStageUsageMask;
	std::vector<InputValueType> inputValueTypes;
};

std::filesystem::path path = "Basic.vert";

ShaderStage stage = ShaderStage::Vertex;

std::vector<ShaderInputs> input
{
	{
		ShaderInputType::UniformBuffer,
		ShaderStage::Vertex | ShaderStage::Compute,
		{
			InputValueType::VEC3FLOAT32,
			InputValueType::VEC3FLOAT32,
			InputValueType::VEC2FLOAT32
		}
	}
	,
	{
		ShaderInputType::UniformBuffer,
		ShaderStage::Vertex,
		{
			InputValueType::VEC3FLOAT32,
			InputValueType::VEC3FLOAT32,
			InputValueType::VEC2FLOAT32
		}
	}
};

// Consider doing a Vulkan type Info struct to pass all needed values, and this class acts as a handle for the shader.
class ShaderDescriptor 
{
public:
	ShaderDescriptor(std::filesystem::path shaderPath, ShaderStage stage, std::vector<ShaderInputs> inputs) : File(shaderPath), stage(stage), Inputs(inputs) {}
	~ShaderDescriptor() = default;

private:
	std::filesystem::path File;
	ShaderStage stage;
	std::vector<ShaderInputs> Inputs;

};

ShaderDescriptor sd(path, stage, input);