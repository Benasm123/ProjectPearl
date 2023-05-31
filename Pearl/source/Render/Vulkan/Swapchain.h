#pragma once
#include "FrameBuffer.h"
#include "GraphicsUnit.h"
#include "Image.h"
#include "PearlCore.h"
#include "vulkan/vulkan.hpp"
#include "Window/RenderSurface.h"


namespace PEARL_NAMESPACE
{
	class Swapchain
	{
	public:
		Swapchain(const GraphicsUnit& graphicsUnit, const RenderPass& renderPass, const RenderSurface& renderSurface);
		~Swapchain();

		[[nodiscard]] const vk::SwapchainKHR& Get() const { return swapchain_; }

		void Recreate();

	private:
		void CreateSwapchain();
		void GetSwapchainSettings();
		void GetSwapchainImages();
		void CreateFramebuffers();

	private:
		vk::SwapchainKHR swapchain_{};

		std::vector<Image*> swapchainImages_{};
		std::vector<vk::ImageView> swapchainImageViews_{};

		const GraphicsUnit& graphicsUnit_;
		const RenderPass& renderPass_;
		const RenderSurface& renderSurface_;

		std::vector<FrameBuffer*> frameBuffers_{};

		// Swapchain settings
		uint32_t swapchainImageCount_ = 3;
		vk::Format swapchainImageFormat_{};
		vk::SurfaceTransformFlagBitsKHR swapchainTransform_{};
		vk::PresentModeKHR swapchainPresentMode_{};

	};
}

