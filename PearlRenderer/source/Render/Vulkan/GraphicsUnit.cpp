#include "GraphicsUnit.h"

#include "Window/RenderSurface.h"

using namespace PEARL_NAMESPACE;

GraphicsUnit::GraphicsUnit(const RendererInstance& instance, const vk::PhysicalDevice device)
	: instance_(instance)
	, graphicsUnit_(device)
{
	requiredFeatures_.setFillModeNonSolid(VK_TRUE);

	if ( !graphicsUnit_ ) throw std::exception("Invalid graphics unit");

	layers_ = instance_.GetLayers();
	extensions_ = instance_.GetDeviceExtensions();

	const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = graphicsUnit_.getQueueFamilyProperties();

	uint32_t index = 0;
	for (vk::QueueFamilyProperties queueFamilyProperty : queueFamilyProperties )
	{
		if ( queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics
			&& graphicsQueueIndex_ == ~0u )
		{
			graphicsQueueIndex_ = index;

			if ( queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute )
			{
				computeQueueIndex_ = index;
			}
		}

		if ( Config::renderer::useSeparateComputeQueue )
		{
			if ( queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute
				&& !(queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics) )
			{
				computeQueueIndex_ = index;
			}
		}

		index++;
	}

	const std::vector graphicsPriorities(Config::renderer::numberOfGraphicsQueuesToUse, 1.0f);
	const std::vector computePriorities(Config::renderer::numberOfComputeQueuesToUse, 1.0f);

	std::vector<vk::DeviceQueueCreateInfo> queueInfos;

	if ( Config::renderer::numberOfGraphicsQueuesToUse > queueFamilyProperties[graphicsQueueIndex_].queueCount )
	{
		LOG_ERROR("Requesting more graphics queues than are available!");
	}

	const vk::DeviceQueueCreateInfo graphicsQueueInfo = vk::DeviceQueueCreateInfo()
		.setFlags({})
		.setQueueCount(Config::renderer::numberOfGraphicsQueuesToUse)
		.setQueueFamilyIndex(graphicsQueueIndex_)
		.setPQueuePriorities(graphicsPriorities.data());

	queueInfos.push_back(graphicsQueueInfo);

	if ( Config::renderer::numberOfComputeQueuesToUse > queueFamilyProperties[computeQueueIndex_].queueCount )
	{
		LOG_ERROR("Requesting more compute queues than are available!");
	}

	const vk::DeviceQueueCreateInfo computeQueueInfo = vk::DeviceQueueCreateInfo()
		.setFlags({})
		.setQueueCount(Config::renderer::numberOfComputeQueuesToUse)
		.setQueueFamilyIndex(computeQueueIndex_)
		.setPQueuePriorities(computePriorities.data());

	if ( Config::renderer::useSeparateComputeQueue )
	{
		queueInfos.push_back(computeQueueInfo);
	}

	const vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo()
		.setFlags({})
		.setEnabledLayerCount(static_cast<uint32_t>(layers_.size()))
		.setPpEnabledLayerNames(layers_.data())
		.setEnabledExtensionCount(static_cast<uint32_t>(extensions_.size()))
		.setPpEnabledExtensionNames(extensions_.data())
		.setPEnabledFeatures(&requiredFeatures_)
		.setQueueCreateInfos(queueInfos);

	logicalUnit_ = graphicsUnit_.createDevice(deviceInfo);
	LOG_TRACE("Graphics Queue index: {}", graphicsQueueIndex_);
	LOG_TRACE("Compute Queue index: {}", computeQueueIndex_);
	LOG_TRACE("Created Logical Device");

	name_ = std::string(graphicsUnit_.getProperties().deviceName.data());

	for (int i = 0; i < Config::renderer::numberOfGraphicsQueuesToUse; i++)
	{
		graphicsQueues_.push_back(logicalUnit_.getQueue(graphicsQueueIndex_, i));
	}
}


GraphicsUnit::~GraphicsUnit()
{
	logicalUnit_.destroy();
}


vk::PhysicalDeviceLimits GraphicsUnit::GetLimits() const
{
	return graphicsUnit_.getProperties().limits;
}


vk::SurfaceCapabilitiesKHR GraphicsUnit::GetSurfaceCapabilities(const RenderSurface& surface) const
{
	return graphicsUnit_.getSurfaceCapabilitiesKHR(surface.Get());
}


std::vector<vk::SurfaceFormatKHR> GraphicsUnit::GetSurfaceFormats(const RenderSurface& surface) const
{
	return graphicsUnit_.getSurfaceFormatsKHR(surface.Get());
}


vk::SurfaceFormatKHR GraphicsUnit::GetBestSurfaceFormat(const RenderSurface& surface) const
{
	const std::vector<vk::SurfaceFormatKHR> surfaceFormats = GetSurfaceFormats(surface);

	vk::SurfaceFormatKHR imageFormat = surfaceFormats.front().format;
	for ( const auto format : surfaceFormats )
	{
		if ( format.format == vk::Format::eB8G8R8A8Unorm ) imageFormat = format.format;
	}

	return imageFormat;
}


std::vector<vk::PresentModeKHR> GraphicsUnit::GetSurfacePresentModes(const RenderSurface& surface) const
{
	return graphicsUnit_.getSurfacePresentModesKHR(surface.Get());
}


uint32_t GraphicsUnit::GetMemoryIndexOfType(const vk::MemoryPropertyFlags memoryProperty) const
{
	const vk::PhysicalDeviceMemoryProperties props = graphicsUnit_.getMemoryProperties();
	for (uint32_t i = 0; i < props.memoryTypeCount; i++)
	{
		if ((props.memoryTypes[i].propertyFlags & memoryProperty) == memoryProperty )
		{
			return i;
		}
	}

	LOG_ERROR("Failed to find memory with property");

	return -1;
}


vk::SwapchainKHR GraphicsUnit::CreateSwapchain(const vk::SwapchainCreateInfoKHR& createInfo) const
{
	return logicalUnit_.createSwapchainKHR(createInfo);
}


std::vector<vk::Image> GraphicsUnit::GetSwapchainImages(const vk::SwapchainKHR swapchain) const
{
	return logicalUnit_.getSwapchainImagesKHR(swapchain);
}


void GraphicsUnit::DestroyRenderPass(const vk::RenderPass renderPass) const
{
	logicalUnit_.destroyRenderPass(renderPass);
}


vk::ImageView GraphicsUnit::CreateImageView(const vk::Image image, const vk::Format format, vk::ImageAspectFlagBits imageAspect) const
{
	const vk::ImageViewCreateInfo imageViewInfo = vk::ImageViewCreateInfo()
	                                              .setFlags({})
	                                              .setImage(image)
	                                              .setComponents(vk::ComponentMapping().setA(vk::ComponentSwizzle::eIdentity).setB(vk::ComponentSwizzle::eIdentity).setG(vk::ComponentSwizzle::eIdentity).setR(vk::ComponentSwizzle::eIdentity))
	                                              .setFormat(format)
	                                              .setSubresourceRange({ imageAspect, 0, 1, 0, 1 })
	                                              .setViewType(vk::ImageViewType::e2D);

	return logicalUnit_.createImageView(imageViewInfo);
}


vk::Queue GraphicsUnit::GetGraphicsQueue()
{
	// graphicsQueuePos_++;
	// graphicsQueuePos_ %= Config::renderer::numberOfGraphicsQueuesToUse;
	return graphicsQueues_[graphicsQueuePos_];
}


void GraphicsUnit::DestroySwapchain(const vk::SwapchainKHR swapchainToDestroy) const
{
	logicalUnit_.destroySwapchainKHR(swapchainToDestroy);
}


vk::RenderPass GraphicsUnit::CreateRenderPass(const vk::RenderPassCreateInfo& renderPassInfo) const
{
	return logicalUnit_.createRenderPass(renderPassInfo);
}
