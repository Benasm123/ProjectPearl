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

pearl::typesRender::MeshData utils::GetGridPlanePoints(float size, int grid_points)
{
	std::vector <pearl::typesRender::VertexInput2D> points{};
	std::vector<pearl::typesRender::Triangle> triangles{};

	int vertex_index = 0;

	for (uint32_t y = 0; y < grid_points; y++)
	{
		for (uint32_t x = 0; x < grid_points; x++)
		{

			std::vector<glm::vec3> current_face = {
				glm::vec3(0 + x, 0, 0 + y),
				glm::vec3(1 + x, 0, 0 + y),
				glm::vec3(1 + x, 0, 1 + y),
				glm::vec3(0 + x, 0, 1 + y)
			};

			points.push_back({ current_face[0], {0, 1, 1} });
			points.push_back({current_face[1], {0, 1, 1} });
			points.push_back({current_face[2], {0, 1, 1} });
			points.push_back({current_face[3], {0, 1, 1} });

			triangles.push_back(pearl::typesRender::Triangle{ 0u + vertex_index, 2u + vertex_index, 1u + vertex_index });
			triangles.push_back(pearl::typesRender::Triangle{ 0u + vertex_index, 3u + vertex_index, 2u + vertex_index });

			vertex_index += 4;
		}
	}

	return pearl::typesRender::MeshData{ points, triangles };
}


pearl::typesRender::MeshData utils::GetGridPlanePointsBD(float size, int grid_points)
{
	std::vector <pearl::typesRender::VertexInput2D> points{};
	std::vector<pearl::typesRender::Triangle> triangles{};


	for (uint32_t y = 0; y < grid_points; y++)
	{
		for (uint32_t x = 0; x < grid_points; x++)
		{
			const float random = (std::rand() % 100) / 50.0f;
			points.emplace_back(glm::vec3{ x, random, y }, glm::normalize(glm::vec3{ x, random, y }), glm::vec3{});
		}
	}

	for (uint32_t y = 0; y < grid_points - 1; y++)
	{
		for (uint32_t x = 0; x < grid_points - 1; x++)
		{
			triangles.push_back(pearl::typesRender::Triangle{ x + (grid_points * y), (x + 1) + (grid_points * (y + 1)), (x + 1) + (grid_points * y) });
			triangles.push_back(pearl::typesRender::Triangle{ x + (grid_points * y), (x) + (grid_points * (y + 1)), (x + 1) + (grid_points * (y + 1)) });
		}
	}

	return pearl::typesRender::MeshData{ points, triangles };
}


pearl::typesRender::MeshData utils::GetGridReduced(pearl::typesRender::MeshData original, int grid_points, int reduce_amount)
{
	std::vector <pearl::typesRender::VertexInput2D> points{};
	std::vector<pearl::typesRender::Triangle> triangles{};

	int vertex_index = 0;

	// points on grid x, y == x + (y * grid_points)
	std::vector<int> removed_points;
	int reduce = 0;
	for (uint32_t y = 0; y < grid_points; y++)
	{
		for (uint32_t x = 0; x < grid_points; x++)
		{
			const int index = x + (y * grid_points);
			if (x == 0 or x == (grid_points - 1) or y == 0 or y == (grid_points-1))
			{
				points.push_back(original.points[index]);
				continue;
			}
			if (reduce == 0)
			{
				removed_points.push_back(index);
			}
			else
			{
				points.push_back(original.points[index]);
			}
			reduce++;
			reduce %= reduce_amount;
		}
	}

	std::vector<uint32_t> connected;

	int removed_passed = 0;
	int removed_above = 0;
	int removed_below = 0;
	for (uint32_t y = 0; y < grid_points; y++)
	{
		for (uint32_t x = 0; x < grid_points; x++)
		{
			int index = x + (y * grid_points);
			const bool removed = std::ranges::find(removed_points, index) != removed_points.end();

			const uint32_t left = index - 1 - removed_passed;
			const uint32_t right = index - removed_passed;
			const uint32_t top = index - (grid_points) - (removed_above);
			const uint32_t bottom = index + (grid_points) - (removed_below);

			if (removed)
			{
				triangles.push_back({ top, left, right });
				triangles.push_back({ left, bottom, right });

				connected.push_back(index - 1);
				connected.push_back(index + 1);
				connected.push_back(index - grid_points);
				connected.push_back(index + grid_points);

				removed_passed++;
			}

			if (index - (grid_points) >= 0)
			{
				if (const bool top_removed = std::ranges::find(removed_points, index - (grid_points)) != removed_points.end()) removed_above++;
			}

			if (index + (grid_points)  < original.points.size())
			{
				if (const bool bottom_removed = std::ranges::find(removed_points, index + (grid_points)) != removed_points.end()) removed_below++;
			}
		}
	}

	removed_passed = 0;
	removed_above = 0;
	removed_below = 0;
	for (int y = 0; y < grid_points; y++)
	{
		for (int x = 0; x < grid_points; x++)
		{
			int index = x + (y * grid_points);

			const bool removed = std::ranges::find(removed_points, index) != removed_points.end();

			// if Can FIND index then skip to next
			if (std::ranges::find(connected, index) != connected.end() or removed)
			{
				if (removed)
				{
					removed_passed++;
				}

				if (index - (grid_points) >= 0)
				{
					if (const bool top_removed = std::ranges::find(removed_points, index - (grid_points)) != removed_points.end()) removed_above++;
				}

				if (index + (grid_points) < original.points.size())
				{
					if (const bool bottom_removed = std::ranges::find(removed_points, index + (grid_points)) != removed_points.end()) removed_below++;
				}

				continue;
			}

			const uint32_t left = index - 1 - removed_passed;
			const uint32_t right = index + 1 - removed_passed;
			const uint32_t top = index - (grid_points) - (removed_above);
			const uint32_t bottom = index + (grid_points) - (removed_below);

			if (((y-1) >= 0) and ((x-1) >= 0))
			{
				triangles.push_back({ (uint32_t)(index - removed_passed), (uint32_t)top, (uint32_t)left });
			}

			if (((x-1) >= 0) and ((y+1) < grid_points))
			{
				triangles.push_back({ (uint32_t)(index - removed_passed), (uint32_t)left, (uint32_t)bottom });
			}

			if (((y+1) < grid_points) and ((x+1) < grid_points))
			{
				triangles.push_back({ (uint32_t)(index - removed_passed), (uint32_t)bottom, (uint32_t)right });
			}

			if (((x+1) < grid_points) and ((y-1) >= 0))
			{
				triangles.push_back({ (uint32_t)(index - removed_passed), (uint32_t)right, (uint32_t)top });
			}

			// Create A method that checks if the point is in the removed points list

			//FFFFFFFFFFFFFFFFFFFFFFFFFFFF

			if (removed)
			{
				removed_passed++;
			}

			if (index - (grid_points) >= 0)
			{
				if (const bool top_removed = std::ranges::find(removed_points, index - (grid_points)) != removed_points.end()) removed_above++;
			}

			if (index + (grid_points) < original.points.size())
			{
				if (const bool bottom_removed = std::ranges::find(removed_points, index + (grid_points)) != removed_points.end()) removed_below++;
			}
		}
	}

	LOG_INFO("POINTS :::: {}", points.size());
	LOG_INFO("TRIANGLES :::: {}", triangles.size());

	return pearl::typesRender::MeshData{ points, triangles };
}

// Write a function that prints hello world.

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
