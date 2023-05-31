#include "Swapchain.h"

using namespace PEARL_NAMESPACE;

Swapchain::Swapchain(const GraphicsUnit& graphicsUnit, const RenderPass& renderPass, const RenderSurface& renderSurface)
	: graphicsUnit_{graphicsUnit}
	, renderPass_{renderPass}
	, renderSurface_{renderSurface}
{
	GetSwapchainSettings();
	Recreate();
}


Swapchain::~Swapchain()
{
	for (const Image* image : swapchainImages_)
	{
		delete image;
	}

	for (const FrameBuffer* frameBuffer : frameBuffers_)
	{
		delete frameBuffer;
	}

	graphicsUnit_.DestroySwapchain(swapchain_);
}


void Swapchain::Recreate()
{
	CreateSwapchain();
	GetSwapchainImages();
	CreateFramebuffers();
}


void Swapchain::CreateSwapchain()
{
	const vk::SwapchainKHR oldSwapchain = swapchain_;

	const auto surfaceCapabilities = graphicsUnit_.GetSurfaceCapabilities(renderSurface_);

	const vk::SwapchainCreateInfoKHR swapchainInfo = vk::SwapchainCreateInfoKHR()
		.setFlags({})
		.setClipped(VK_TRUE)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setMinImageCount(swapchainImageCount_)
		.setImageFormat(swapchainImageFormat_)
		.setImageExtent({ surfaceCapabilities.currentExtent })
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setImageSharingMode(vk::SharingMode::eExclusive) // TODO -> Not sure if need this for concurrency! ---> I assume if compute queue is separate and it needs swapchain it will need concurrency?
		.setQueueFamilyIndexCount(0)
		.setQueueFamilyIndices(nullptr)
		.setPreTransform(swapchainTransform_)
		.setPresentMode(swapchainPresentMode_)
		.setOldSwapchain(oldSwapchain)
		.setSurface(renderSurface_.Get());

	swapchain_ = graphicsUnit_.CreateSwapchain(swapchainInfo);
	graphicsUnit_.DestroySwapchain(oldSwapchain);
}


void Swapchain::GetSwapchainSettings()
{
	const auto surfaceCapabilities = graphicsUnit_.GetSurfaceCapabilities(renderSurface_);

	swapchainImageFormat_ = graphicsUnit_.GetBestSurfaceFormat(renderSurface_).format;

	swapchainImageCount_ = std::max(Config::renderer::numberOfSwapchainImagesToUse, surfaceCapabilities.minImageCount);

	swapchainImageCount_ = std::min(Config::renderer::numberOfSwapchainImagesToUse, surfaceCapabilities.maxImageCount);

	if ( swapchainImageCount_ != Config::renderer::numberOfSwapchainImagesToUse )
	{
		LOG_INFO("Had to use a different number of swapchain images to what was set in config, as this was not supported. Requested: {}, Using: {}", Config::renderer::numberOfSwapchainImagesToUse, swapchainImageCount_);
	}

	swapchainTransform_ = surfaceCapabilities.currentTransform;

	const auto availablePresentModes = graphicsUnit_.GetSurfacePresentModes(renderSurface_);

	swapchainPresentMode_ = availablePresentModes.front();
	for ( const auto presentMode : availablePresentModes )
	{
		if ( presentMode == vk::PresentModeKHR::eMailbox ) swapchainPresentMode_ = presentMode;
	}
}


void Swapchain::GetSwapchainImages()
{
	const auto surfaceCapabilities = graphicsUnit_.GetSurfaceCapabilities(renderSurface_);
	const std::vector<vk::Image> swapchainImages = graphicsUnit_.GetSwapchainImages(swapchain_);
	swapchainImageCount_ = static_cast<uint32_t>(swapchainImages.size()); // Update swapchain Image count to actual count of images.

	swapchainImages_.resize(swapchainImageCount_);

	for ( uint32_t i = 0; i < swapchainImageCount_; i++)
	{
		swapchainImages_[i] = new Image(graphicsUnit_, swapchainImages[i], swapchainImageFormat_, surfaceCapabilities.currentExtent, false);
	}
}


void Swapchain::CreateFramebuffers()
{
	frameBuffers_.resize(swapchainImageCount_);
	for ( uint32_t i = 0; i < swapchainImageCount_; i++ )
	{
		const std::vector<Image> attachments = { *swapchainImages_[i] };
		frameBuffers_[i] = new FrameBuffer{graphicsUnit_, renderPass_, attachments};
	}
}
