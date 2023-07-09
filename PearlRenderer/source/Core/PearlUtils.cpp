#include "PearlUtils.h"

#include <fstream>

#include "Core/Logger.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"

bool utils::ValidateLayersAndExtensions(const std::vector<const char*>& layers, const std::vector<const char*>& extensions, const vk::PhysicalDevice gpu)
{
	const std::vector<vk::LayerProperties> availableLayers = !gpu ? vk::enumerateInstanceLayerProperties() : gpu.enumerateDeviceLayerProperties();
	std::vector<vk::ExtensionProperties> availableExtensions = !gpu ? vk::enumerateInstanceExtensionProperties() : gpu.enumerateDeviceExtensionProperties();

	for ( const auto layer : layers )
	{
		auto layersExtensions = !gpu ? vk::enumerateInstanceExtensionProperties(std::string(layer)) : gpu.enumerateDeviceExtensionProperties(std::string(layer));
		availableExtensions.insert(availableExtensions.end(), layersExtensions.begin(), layersExtensions.end());

		bool layerFound = false;
		for ( auto availableLayer : availableLayers )
		{
			if ( std::strcmp(availableLayer.layerName.data(), layer) == 0 )
			{
				layerFound = true;
				break;
			}
		}
		if ( !layerFound )
		{
			LOG_ERROR("Layer is not supported: {}", layer);
			return false;
		}
	}

	for ( const auto extension : extensions )
	{
		bool extensionFound = false;
		for ( auto availableExtension : availableExtensions )
		{
			if ( std::strcmp(availableExtension.extensionName.data(), extension) == 0 )
			{
				extensionFound = true;
				break;
			}
		}
		if ( !extensionFound )
		{
			LOG_ERROR("Extension is not supported: {}", extension);
			return false;
		}
	}

	return true;
}


std::vector<char> utils::ReadShaderCode(const std::filesystem::path& filePath)
{
	std::filesystem::path shaderFolder = std::filesystem::current_path();

	while ( shaderFolder.filename() != "ProjectPearl")
	{
		shaderFolder = shaderFolder.parent_path();
	}

	if (shaderFolder.parent_path().filename() == "ProjectPearl")
	{
		shaderFolder = shaderFolder.parent_path();
	}

	shaderFolder = shaderFolder / "ProjectPearl" / "Shaders" / "Compiled";

	std::ifstream file(shaderFolder / filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		LOG_ERROR("Failed opening file: {}.", (shaderFolder / filePath).string().c_str());
		return {};
	}

	const uint32_t fileSize = static_cast<uint32_t>(std::filesystem::file_size(shaderFolder / filePath));
	std::vector<char> contents(fileSize);

	file.seekg(0);
	file.read(contents.data(), fileSize);
	file.close();

	return contents;
}


pearl::types2D::Point3D operator*(const float lhs, const pearl::types2D::Point3D& rhs)
{
	return pearl::types2D::Point3D{rhs.x * lhs, rhs.y * lhs, rhs.z * lhs};
}


pearl::typesRender::MeshData utils::GetPlanePoints(float size)
{
	std::vector <pearl::typesRender::VertexInput2D> points{};
	std::vector<pearl::typesRender::Triangle> triangles{};

	points.emplace_back(glm::vec3{-1 * size, 0, -1 * size}, glm::vec3{0, 1, 0}, glm::vec3{});
	points.emplace_back(glm::vec3{1 * size, 0, -1 * size}, glm::vec3{0, 1, 0}, glm::vec3{});
	points.emplace_back(glm::vec3{-1 * size, 0, 1 * size}, glm::vec3{0, 1, 0}, glm::vec3{});
	points.emplace_back(glm::vec3{1 * size, 0, 1 * size}, glm::vec3{0, 1, 0}, glm::vec3{});

	triangles.push_back({ 1, 0, 2 });
	triangles.push_back({ 1, 2, 3 });

	return pearl::typesRender::MeshData{points, triangles};
}


pearl::typesRender::MeshData utils::GetSpherePoints(float radius, const uint32_t precision)
{
	std::vector <pearl::typesRender::VertexInput2D> points{};
	std::vector<pearl::typesRender::Triangle> triangles{};

	const auto phi = (1.0f + sqrt(5.0f)) * 0.5f;
	float a = 1.0f;
	float b = 1.0f / phi;

	// add vertices
	points.emplace_back(glm::vec3{0, b, -a}, glm::vec3{0, b, -a}, glm::vec2{});
	points.emplace_back(glm::vec3{b, a, 0}, glm::vec3{b, a, 0}, glm::vec2{});
	points.emplace_back(glm::vec3{-b, a, 0}, glm::vec3{-b, a, 0}, glm::vec2{});
	points.emplace_back(glm::vec3{0, b, a}, glm::vec3{0, b, a}, glm::vec2{});
	points.emplace_back(glm::vec3{0, -b, a}, glm::vec3{0, -b, a}, glm::vec2{});
	points.emplace_back(glm::vec3{-a, 0, b}, glm::vec3{-a, 0, b}, glm::vec2{});
	points.emplace_back(glm::vec3{0, -b, -a}, glm::vec3{0, -b, -a}, glm::vec2{});
	points.emplace_back(glm::vec3{a, 0, -b}, glm::vec3{a, 0, -b}, glm::vec2{});
	points.emplace_back(glm::vec3{a, 0, b}, glm::vec3{a, 0, b}, glm::vec2{});
	points.emplace_back(glm::vec3{-a, 0, -b}, glm::vec3{-a, 0, -b}, glm::vec2{});
	points.emplace_back(glm::vec3{b, -a, 0}, glm::vec3{b, -a, 0}, glm::vec2{});
	points.emplace_back(glm::vec3{-b, -a, 0}, glm::vec3{-b, -a, 0}, glm::vec2{});

	for ( pearl::typesRender::VertexInput2D& point : points )
	{
		point.point = glm::normalize(point.point) * radius;
	}

	// add triangles
	triangles.push_back({2, 1, 0});
	triangles.push_back({ 1, 2, 3 });
	triangles.push_back({5, 4,  3});
	triangles.push_back({ 4,  8,  3});
	triangles.push_back({ 7,  6,  0});
	triangles.push_back({ 6,  9,  0});
	triangles.push_back({ 11,  10,  4});
	triangles.push_back({ 10,  11,  6});
	triangles.push_back({ 9, 5, 2});
	triangles.push_back({5, 9, 11});
	triangles.push_back({8, 7, 1});
	triangles.push_back({7, 8, 10});
	triangles.push_back({2, 5, 3});
	triangles.push_back({8, 1, 3});
	triangles.push_back({9, 2, 0});
	triangles.push_back({1, 7, 0});
	triangles.push_back({11, 9, 6});
	triangles.push_back({7, 10, 6});
	triangles.push_back({5, 11, 4});
	triangles.push_back({10, 8, 4});

	for (uint32_t i = 0; i < precision; i++)
	{
		std::vector<pearl::typesRender::Triangle> trianglesToAdd{};

		for (pearl::typesRender::Triangle triangle : triangles)
		{
			glm::vec3 v0 = points[triangle.f1].point;
			glm::vec3 v1 = points[triangle.f2].point;
			glm::vec3 v2 = points[triangle.f3].point;

			glm::vec3 v3 = glm::normalize(0.5f * (v0 + v1));
			glm::vec3 v4 = glm::normalize(0.5f * (v1 + v2));
			glm::vec3 v5 = glm::normalize(0.5f * (v2 + v0));

			uint32_t v3Index = static_cast<uint32_t>(points.size());
			points.push_back({glm::vec3{v3 * radius}, glm::vec3{v3}, glm::vec2{}});
			uint32_t v4Index = static_cast<uint32_t>(points.size());
			points.push_back({ glm::vec3{v4 * radius}, glm::vec3{v4}, glm::vec2{} });
			uint32_t v5Index = static_cast<uint32_t>(points.size());
			points.push_back({ glm::vec3{v5 * radius}, glm::vec3{v5}, glm::vec2{} });

			trianglesToAdd.push_back({ triangle.f1, v3Index, v5Index });
			trianglesToAdd.push_back({ v3Index, triangle.f2, v4Index });
			trianglesToAdd.push_back({ v4Index, triangle.f3, v5Index });
			trianglesToAdd.push_back({ v3Index, v4Index, v5Index });
		}

		triangles = trianglesToAdd;
	}

	return pearl::typesRender::MeshData{points, triangles};
}
