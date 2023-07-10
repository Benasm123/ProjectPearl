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

		[[nodiscard]] vk::Framebuffer Get() const { return framebuffer_; }

	private:
		void CreateDepthResources();
		void CreateMultiSampleResources();

	private:
		const GraphicsUnit& graphicsUnit_;

		std::vector<vk::ImageView> attachments_;

		Image depthImage_;
		vk::ImageView depthImageView_;
		vk::DeviceMemory depthMemory_;

		Image multiSampleImage_;
		vk::ImageView multiSampleImageView_;
		vk::DeviceMemory multiSampleMemory_;

		vk::Framebuffer framebuffer_;
	};
}

