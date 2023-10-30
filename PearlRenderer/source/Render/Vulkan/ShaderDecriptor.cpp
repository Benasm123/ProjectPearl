# include "ShaderDescriptor.h"

ShaderDescriptor::ShaderDescriptor(std::filesystem::path shader_path, const ShaderStage stage, const std::vector<ShaderInputs>& inputs)
	: file_(std::move(shader_path))
	, stage_(stage)
	, inputs_(inputs)
{
}
