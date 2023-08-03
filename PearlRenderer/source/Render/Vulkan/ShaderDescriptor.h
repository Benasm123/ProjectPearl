#pragma once
#include <filesystem>

class ShaderDescriptor 
{
public:
	ShaderDescriptor(std::filesystem::path shaderPath);
	~ShaderDescriptor();
};