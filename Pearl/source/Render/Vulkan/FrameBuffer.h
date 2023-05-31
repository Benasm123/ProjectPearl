#pragma once
#include "PearlCore.h"
#include "GraphicsUnit.h"
#include "Image.h"
#include "RenderPass.h"

namespace PEARL_NAMESPACE
{
	class FrameBuffer
	{
	public:
		FrameBuffer(const GraphicsUnit& graphicsUnit, const RenderPass& renderpass, const std::vector<Image>& images);
		~FrameBuffer();

	private:
		const GraphicsUnit& graphicsUnit_;

		std::vector<vk::ImageView> attachments_;
		vk::Framebuffer framebuffer_;
	};
}

