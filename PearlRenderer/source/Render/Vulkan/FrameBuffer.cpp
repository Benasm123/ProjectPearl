#include "FrameBuffer.h"

using namespace PEARL_NAMESPACE;

FrameBuffer::FrameBuffer(const GraphicsUnit& graphicsUnit, const RenderPass& renderpass, const std::vector<Image>& images)
	: graphicsUnit_{graphicsUnit}
	// , depthImage_{ graphicsUnit_, vk::Format::eD32Sfloat, images.front().Size(), vk::ImageLayout::eUndefined, vk::ImageUsageFlagBits::eDepthStencilAttachment }
{
	CreateDepthResources();

	for (const Image& image : images)
	{
		colourAttachments_.push_back(graphicsUnit_.CreateImageView(image.Get(), image.Format(), vk::ImageAspectFlagBits::eColor));
	}

	const vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo()
		.setAttachmentCount(static_cast<uint32_t>(colourAttachments_.size()))
		.setPAttachments(colourAttachments_.data())
		.setHeight(images.front().Size().height)
		.setWidth(images.front().Size().width)
		.setLayers(1)
		.setRenderPass(renderpass.Get());
	
	framebuffer_ = graphicsUnit_.GetLogical().createFramebuffer(framebufferInfo);
}


FrameBuffer::~FrameBuffer()
{
	for (const vk::ImageView view : colourAttachments_)
	{
		graphicsUnit_.GetLogical().destroyImageView(view);
	}

	graphicsUnit_.GetLogical().destroyFramebuffer(framebuffer_);
}


void FrameBuffer::CreateDepthResources()
{
	// depthImageView_ = graphicsUnit_.CreateImageView(depthImage_.Get(), depthImage_.Format(), vk::ImageAspectFlagBits::eDepth);
}
