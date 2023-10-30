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
	kVertex = 1 << 0,
	kFragment = 1 << 1,
	kCompute = 1 << 2,
	kGeometry = 1 << 3,
	kTessellation = 1 << 4
};

// TODO -> Add int types up to Vec 4 with unsigned variants.
enum class InputValueType
{
	kFloat32,
	kVec2Float32,
	kVec3Float32
};

struct ShaderInputs 
{
	ShaderInputType input_type;
	ShaderStageMask shader_stage_usage_mask;
	std::vector<InputValueType> input_value_types;
};

// Consider doing a Vulkan type Info struct to pass all needed values, and this class acts as a handle for the shader.
class ShaderDescriptor 
{
public:
	ShaderDescriptor(std::filesystem::path shader_path, const ShaderStage stage, const std::vector<ShaderInputs>& inputs);
	~ShaderDescriptor() = default;

	std::filesystem::path file_;
	ShaderStage stage_;
	std::vector<ShaderInputs> inputs_;
};


// // TEST
// std::filesystem::path path = "Basic.vert";
//
// ShaderStage stage = ShaderStage::Vertex;
//
// std::vector<ShaderInputs> input
// {
// 	{
// 		ShaderInputType::UniformBuffer,
// 			ShaderStage::Vertex | ShaderStage::Compute,
// 		{
// 			InputValueType::VEC3FLOAT32,
// 			InputValueType::VEC3FLOAT32,
// 			InputValueType::VEC2FLOAT32
// 		}
// 	}
// 	,
// 	{
// 		ShaderInputType::UniformBuffer,
// 		ShaderStage::Vertex,
// 		{
// 			InputValueType::VEC3FLOAT32,
// 			InputValueType::VEC3FLOAT32,
// 			InputValueType::VEC2FLOAT32
// 		}
// 	}
// };
//
// ShaderDescriptor sd(path, stage, input);