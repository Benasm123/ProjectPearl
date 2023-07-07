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
		[[nodiscard]] uint32_t GetImageCount() const { return swapchainImageCount_; }
		[[nodiscard]] uint32_t GetNextImageIndex(uint32_t currentIndex) const;

		[[nodiscard]] std::vector<FrameBuffer*> GetFramebuffers() { return frameBuffers_; }

		[[nodiscard]] std::vector<vk::Semaphore>& GetImageAcquiredSemaphores() { return imageAcquiredSemaphore_; }
		[[nodiscard]] std::vector<vk::Semaphore>& GetDrawCompletedSemaphores() { return drawCompleteSemaphore_; }
		[[nodiscard]] std::vector<vk::Fence>& GetFences() { return fences_; }

		void Recreate();

	private:
		void CreateSwapchain();
		void GetSwapchainSettings();
		void GetSwapchainImages();
		void CreateFramebuffers();
		void CreateSynchronization();

	private:
		vk::SwapchainKHR swapchain_{};

		const GraphicsUnit& graphicsUnit_;
		const RenderPass& renderPass_;
		const RenderSurface& renderSurface_;

		std::vector<Image*> swapchainImages_{};
		std::vector<vk::ImageView> swapchainImageViews_{};
		std::vector<FrameBuffer*> frameBuffers_{};
		std::vector<vk::Fence> fences_;
		std::vector<vk::Semaphore> imageAcquiredSemaphore_;
		std::vector<vk::Semaphore> drawCompleteSemaphore_;

		// Swapchain settings
		uint32_t swapchainImageCount_ = 3;
		vk::Format swapchainImageFormat_{};
		vk::SurfaceTransformFlagBitsKHR swapchainTransform_{};
		vk::PresentModeKHR swapchainPresentMode_{};
	};
}

