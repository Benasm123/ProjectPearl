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
		void CreateDepthResources();
		void CreateMultiSampleResources();

	private:
		const GraphicsUnit& graphicsUnit_;

		std::vector<class vk::ImageView> attachments_;

		Image depthImage_;
		class vk::ImageView depthImageView_;
		class vk::DeviceMemory depthMemory_;

		Image multiSampleImage_;
		class vk::ImageView multiSampleImageView_;
		class vk::DeviceMemory multiSampleMemory_;

		class vk::Framebuffer framebuffer_;

		friend class CommandBuffer;
	};
}

