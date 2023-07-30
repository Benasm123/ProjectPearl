#include "FrameBuffer.h"
#include "BDVK/BDVK_internal.h"

using namespace PEARL_NAMESPACE;

FrameBuffer::FrameBuffer(const GraphicsUnit& graphicsUnit, const RenderPass& renderpass, const std::vector<Image>& images)
	: graphicsUnit_{graphicsUnit}
	, depthImage_{ graphicsUnit_, vk::Format::eD32Sfloat, vk::Extent2D{(uint32_t)images.front().Size().x, (uint32_t)images.front().Size().y}, vk::ImageLayout::eUndefined, vk::ImageUsageFlagBits::eDepthStencilAttachment}
	, multiSampleImage_{ graphicsUnit_, images[0].format_, vk::Extent2D{(uint32_t)images.front().Size().x, (uint32_t)images.front().Size().y}, vk::ImageLayout::eUndefined, vk::ImageUsageFlags{vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment} }
{
	CreateDepthResources();
	CreateMultiSampleResources();

	attachments_.push_back(multiSampleImageView_);
	attachments_.push_back(depthImageView_);

	for (const Image& image : images)
	{
		attachments_.push_back(graphicsUnit_.CreateImageView(image.image_, image.format_, vk::ImageAspectFlagBits::eColor));
	}


	const vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo()
		.setAttachmentCount(static_cast<uint32_t>(attachments_.size()))
		.setPAttachments(attachments_.data())
		.setWidth((uint32_t)images.front().Size().x)
		.setHeight((uint32_t)images.front().Size().y)
		.setLayers(1)
		.setRenderPass(renderpass.renderPass_);
	
	framebuffer_ = graphicsUnit_.logicalUnit_.createFramebuffer(framebufferInfo);
}


FrameBuffer::~FrameBuffer()
{
	graphicsUnit_.logicalUnit_.freeMemory(multiSampleMemory_);
	graphicsUnit_.logicalUnit_.freeMemory(depthMemory_);

	for (const vk::ImageView view : attachments_)
	{
		graphicsUnit_.logicalUnit_.destroyImageView(view);
	}

	graphicsUnit_.logicalUnit_.destroyFramebuffer(framebuffer_);
}


void FrameBuffer::CreateDepthResources()
{
	const vk::MemoryRequirements requirements = graphicsUnit_.logicalUnit_.getImageMemoryRequirements(depthImage_.image_);

	const vk::MemoryAllocateInfo imageMemoryInfo = vk::MemoryAllocateInfo()
	                                               .setAllocationSize(requirements.size)
	                                               .setMemoryTypeIndex(graphicsUnit_.GetMemoryIndexOfType(vk::MemoryPropertyFlagBits::eDeviceLocal));

	depthMemory_ = graphicsUnit_.logicalUnit_.allocateMemory(imageMemoryInfo);

	graphicsUnit_.logicalUnit_.bindImageMemory(depthImage_.image_, depthMemory_, 0);

	depthImageView_ = graphicsUnit_.CreateImageView(depthImage_.image_, depthImage_.format_, vk::ImageAspectFlagBits::eDepth);
}


void FrameBuffer::CreateMultiSampleResources()
{
	const vk::MemoryRequirements requirements = graphicsUnit_.logicalUnit_.getImageMemoryRequirements(multiSampleImage_.image_);

	const vk::MemoryAllocateInfo imageMemoryInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(requirements.size)
		.setMemoryTypeIndex(graphicsUnit_.GetMemoryIndexOfType(vk::MemoryPropertyFlagBits::eDeviceLocal));

	multiSampleMemory_ = graphicsUnit_.logicalUnit_.allocateMemory(imageMemoryInfo);

	graphicsUnit_.logicalUnit_.bindImageMemory(multiSampleImage_.image_, multiSampleMemory_, 0);

	multiSampleImageView_ = graphicsUnit_.CreateImageView(multiSampleImage_.image_, multiSampleImage_.format_, vk::ImageAspectFlagBits::eColor);
}
