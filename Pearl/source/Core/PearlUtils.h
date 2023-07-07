#pragma once
#include <filesystem>
#include <vector>
#include "vulkan/vulkan.hpp"
#include "Render/Types/Types2D.h"
#include "Render/Types/TypesRender.h"

namespace utils
{
	[[nodiscard]] bool ValidateLayersAndExtensions(const std::vector<const char*>& layers, const std::vector<const char*>& extensions, vk::PhysicalDevice gpu=nullptr);
	[[nodiscard]] std::vector<char> ReadShaderCode(const std::filesystem::path& filePath);
	[[nodiscard]] pearl::typesRender::MeshData GetSpherePoints(float radius, uint32_t precision);
	[[nodiscard]] pearl::typesRender::MeshData GetPlanePoints(float size);
}

