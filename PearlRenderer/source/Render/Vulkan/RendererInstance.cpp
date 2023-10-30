#include "RendererInstance.h"
#include "BDVK/BDVK_internal.h"

using namespace PEARL_NAMESPACE;

RendererInstance::RendererInstance(const Window& window, const std::vector<const char*>& additionalLayers, const std::vector<const char*>& additionalExtensions)
	:window_{window}
{
	if ( Config::renderer::useValidation )
	{
		requiredInstanceLayers_.push_back("VK_LAYER_KHRONOS_validation");
		requiredInstanceLayers_.push_back("VK_LAYER_LUNARG_monitor");
	}
#ifdef THIS
#endif

	const std::vector<const char*> requiredWindowExtensions = window_.GetRequiredVulkanExtensions();

	for ( const char* extension : requiredWindowExtensions )
	{
		if ( std::ranges::find(requiredInstanceExtension_, extension) == requiredInstanceExtension_.end() ) requiredInstanceExtension_.push_back(extension);
	}

	if ( !utils::ValidateLayersAndExtensions(requiredInstanceLayers_, requiredInstanceExtension_) )
	{
		LOG_CRITICAL("Required Layers and Extensions are not supported!");
	}

	for ( const char* layer : additionalLayers )
	{
		if ( std::ranges::find(requiredInstanceLayers_, layer) == requiredInstanceLayers_.end() ) requiredInstanceLayers_.push_back(layer);
	}

	for ( const char* extension : additionalExtensions )
	{
		if ( std::ranges::find(requiredInstanceExtension_, extension) == requiredInstanceExtension_.end() ) requiredInstanceExtension_.push_back(extension);
	}

	if ( !utils::ValidateLayersAndExtensions(additionalLayers, additionalExtensions) )
	{
		LOG_CRITICAL("Additional Features Layers and Extensions are not supported!");
	}

	constexpr auto applicationInfo = vk::ApplicationInfo()
		.setPApplicationName("Pearl Engine")
		.setApplicationVersion(1)
		.setPEngineName("Pearl Engine")
		.setEngineVersion(1)
		.setApiVersion(VK_VERSION_1_3);

	const auto& instanceInfo = vk::InstanceCreateInfo()
		.setFlags({})
		.setPApplicationInfo(&applicationInfo)
		.setEnabledLayerCount(static_cast<uint32_t>(requiredInstanceLayers_.size()))
		.setPpEnabledLayerNames(requiredInstanceLayers_.data())
		.setEnabledExtensionCount(static_cast<uint32_t>(requiredInstanceExtension_.size()))
		.setPpEnabledExtensionNames(requiredInstanceExtension_.data());

	instance_ = vk::createInstance(instanceInfo, nullptr);
}


RendererInstance::~RendererInstance()
{
	instance_.destroy();
}


// TODO -> This additional extensions doesnt actually add extensions right now. Otherwise i can make const.
vk::PhysicalDevice RendererInstance::FindBestGraphicsUnit(std::vector<const char*> additionalExtensions) const
{
	const std::vector<vk::PhysicalDevice> devices = instance_.enumeratePhysicalDevices();

	vk::PhysicalDevice bestGPU;
	vk::DeviceSize bestMemoryAvailable = 0;

	for ( const vk::PhysicalDevice device : devices )
	{
		vk::PhysicalDeviceProperties properties = device.getProperties();
		vk::PhysicalDeviceMemoryProperties memoryProperties = device.getMemoryProperties();

		LOG_TRACE("Checking GPU: {}", properties.deviceName);

		if ( !utils::ValidateLayersAndExtensions(requiredInstanceLayers_, requiredDeviceExtensions_, device) ) continue;

		if ( !utils::ValidateLayersAndExtensions(requiredInstanceLayers_, additionalExtensions, device) ) continue;

		if ( !bestGPU )
		{
			bestGPU = device;
			continue;
		}

		if ( properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu )
		{
			if ( bestGPU.getProperties().deviceType != vk::PhysicalDeviceType::eDiscreteGpu )
			{
				bestGPU = device;
				continue;
			}

			vk::DeviceSize totalMemoryAvailable = 0;

			for ( const vk::MemoryHeap memoryHeap : memoryProperties.memoryHeaps )
			{
				totalMemoryAvailable += memoryHeap.size;
			}

			if ( totalMemoryAvailable > bestMemoryAvailable ) bestGPU = device;

			bestMemoryAvailable = std::max(totalMemoryAvailable, bestMemoryAvailable);
		}
	}

	if ( !bestGPU )
	{
		LOG_CRITICAL("Could not find suitable device!\n");
		return nullptr;
	}

	LOG_INFO("Best GPU found: {}", bestGPU.getProperties().deviceName);
	return bestGPU;

}
