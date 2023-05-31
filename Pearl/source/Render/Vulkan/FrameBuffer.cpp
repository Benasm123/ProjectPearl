#include "FrameBuffer.h"

using namespace PEARL_NAMESPACE;

FrameBuffer::FrameBuffer(const GraphicsUnit& graphicsUnit, const RenderPass& renderpass, const std::vector<Image>& images)
	:graphicsUnit_{graphicsUnit}
{
	for (const Image& image : images)
	{
		attachments_.push_back(graphicsUnit_.CreateImageView(image.Get(), image.Format()));
	}

	const vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo()
		.setAttachmentCount(static_cast<uint32_t>(attachments_.size()))
		.setPAttachments(attachments_.data())
		.setHeight(images.front().Size().height)
		.setWidth(images.front().Size().width)
		.setLayers(1)
		.setRenderPass(renderpass.Get());// No renderpass yet
	
	framebuffer_ = graphicsUnit_.GetLogical().createFramebuffer(framebufferInfo);
}


FrameBuffer::~FrameBuffer()
{
	for (const vk::ImageView view : attachments_)
	{
		graphicsUnit_.GetLogical().destroyImageView(view);
	}

	graphicsUnit_.GetLogical().destroyFramebuffer(framebuffer_);
}
