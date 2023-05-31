#pragma once
#include <filesystem>
#include <vector>
#include "vulkan/vulkan.hpp"

namespace utils
{
	[[nodiscard]] bool ValidateLayersAndExtensions(const std::vector<const char*>& layers, const std::vector<const char*>& extensions, vk::PhysicalDevice gpu=nullptr);
	[[nodiscard]] std::vector<char> ReadShaderCode(const std::filesystem::path& filePath);
}

