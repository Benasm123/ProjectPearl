#include "PearlUtils.h"

#include <fstream>

#include "Core/Logger.h"
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
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		LOG_ERROR("Failed opening file: {}.", filePath);
		return {};
	}

	const uint32_t fileSize = static_cast<uint32_t>(std::filesystem::file_size(filePath));
	std::vector<char> contents(fileSize);

	file.seekg(0);
	file.read(contents.data(), fileSize);
	file.close();

	return contents;
}
