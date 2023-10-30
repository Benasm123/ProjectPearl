#pragma once
#include <filesystem>
#include <vector>

#include "Render/Types/Types2D.h"
#include "Render/Types/TypesRender.h"

namespace utils
{
	[[nodiscard]] bool ValidateLayersAndExtensions(const std::vector<const char*>& layers, const std::vector<const char*>& extensions, vk::PhysicalDevice gpu=nullptr);
	[[nodiscard]] std::vector<char> ReadShaderCode(const std::filesystem::path& filePath);
	[[nodiscard]] pearl::typesRender::MeshData GetSpherePoints(float radius, uint32_t precision);
	[[nodiscard]] pearl::typesRender::MeshData GetPlanePoints(float size);
	[[nodiscard]] pearl::typesRender::MeshData GetGridPlanePoints(float size, int grid_points);
	pearl::typesRender::MeshData GetGridPlanePointsBD(float size, int grid_points);
	pearl::typesRender::MeshData GetGridReduced(pearl::typesRender::MeshData original, int grid_points, int reduce_amount);
}

