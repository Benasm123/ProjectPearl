#include "FrameBuffer.h"

using namespace PEARL_NAMESPACE;

FrameBuffer::FrameBuffer(const GraphicsUnit& graphicsUnit, const RenderPass& renderpass, const std::vector<Image>& images)
	: graphicsUnit_{graphicsUnit}
	, depthImage_{ graphicsUnit_, vk::Format::eD32Sfloat, images.front().Size(), vk::ImageLayout::eUndefined, vk::ImageUsageFlagBits::eDepthStencilAttachment }
	, multiSampleImage_{ graphicsUnit, images[0].Format(), images[0].Size(), vk::ImageLayout::eUndefined, vk::ImageUsageFlags{vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment} }
{
	CreateDepthResources();
	CreateMultiSampleResources();

	attachments_.push_back(multiSampleImageView_);
	attachments_.push_back(depthImageView_);

	for (const Image& image : images)
	{
		attachments_.push_back(graphicsUnit_.CreateImageView(image.Get(), image.Format(), vk::ImageAspectFlagBits::eColor));
	}


	const vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo()
		.setAttachmentCount(static_cast<uint32_t>(attachments_.size()))
		.setPAttachments(attachments_.data())
		.setHeight(images.front().Size().height)
		.setWidth(images.front().Size().width)
		.setLayers(1)
		.setRenderPass(renderpass.Get());
	
	framebuffer_ = graphicsUnit_.GetLogical().createFramebuffer(framebufferInfo);
}


FrameBuffer::~FrameBuffer()
{

	// graphicsUnit_.GetLogical().destroyImageView(depthImageView_);
	// graphicsUnit_.GetLogical().destroyImageView(multiSampleImageView_);

	graphicsUnit_.GetLogical().freeMemory(multiSampleMemory_);
	graphicsUnit_.GetLogical().freeMemory(depthMemory_);

	for (const vk::ImageView view : attachments_)
	{
		graphicsUnit_.GetLogical().destroyImageView(view);
	}

	graphicsUnit_.GetLogical().destroyFramebuffer(framebuffer_);
}


void FrameBuffer::CreateDepthResources()
{
	const vk::MemoryRequirements requirements = graphicsUnit_.GetLogical().getImageMemoryRequirements(depthImage_.Get());

	const vk::MemoryAllocateInfo imageMemoryInfo = vk::MemoryAllocateInfo()
	                                               .setAllocationSize(requirements.size)
	                                               .setMemoryTypeIndex(1);

	depthMemory_ = graphicsUnit_.GetLogical().allocateMemory(imageMemoryInfo);

	graphicsUnit_.GetLogical().bindImageMemory(depthImage_.Get(), depthMemory_, 0);

	depthImageView_ = graphicsUnit_.CreateImageView(depthImage_.Get(), depthImage_.Format(), vk::ImageAspectFlagBits::eDepth);
}


void FrameBuffer::CreateMultiSampleResources()
{
	const vk::MemoryRequirements requirements = graphicsUnit_.GetLogical().getImageMemoryRequirements(multiSampleImage_.Get());

	const vk::MemoryAllocateInfo imageMemoryInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(requirements.size)
		.setMemoryTypeIndex(1);

	multiSampleMemory_ = graphicsUnit_.GetLogical().allocateMemory(imageMemoryInfo);

	graphicsUnit_.GetLogical().bindImageMemory(multiSampleImage_.Get(), multiSampleMemory_, 0);

	multiSampleImageView_ = graphicsUnit_.CreateImageView(multiSampleImage_.Get(), multiSampleImage_.Format(), vk::ImageAspectFlagBits::eColor);
}
