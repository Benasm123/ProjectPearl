#pragma once
#include "FrameBuffer.h"
#include "GraphicsUnit.h"
#include "Image.h"
#include "PearlCore.h"
#include "vulkan/vulkan.hpp"
#include "Window/RenderSurface.h"


namespace PEARL_NAMESPACE
{
	class Fence;
	class Semaphore;

	class Swapchain
	{
	public:
		Swapchain(const GraphicsUnit& graphicsUnit, const RenderPass& renderPass, const RenderSurface& renderSurface);
		~Swapchain();

		[[nodiscard]] uint32_t GetImageCount() const { return swapchainImageCount_; }
		[[nodiscard]] uint32_t GetNextImageIndex(uint32_t currentIndex) const;

		[[nodiscard]] std::vector<FrameBuffer*> GetFramebuffers() { return frameBuffers_; }

		[[nodiscard]] std::vector<Semaphore*>& GetImageAcquiredSemaphores() { return imageAcquiredSemaphore_; }
		[[nodiscard]] std::vector<Semaphore*>& GetDrawCompletedSemaphores() { return drawCompleteSemaphore_; }
		[[nodiscard]] std::vector<Fence*>& GetFences() { return fences_; }

		[[nodiscard]] glm::vec2 GetSize() const { return { size_.width, size_.height }; }

		void Recreate();

	private:
		void CreateSwapchain();
		void GetSwapchainSettings();
		void GetSwapchainImages();
		void CreateFramebuffers();
		void CreateSynchronization();

	private:
		class vk::SwapchainKHR swapchain_{};

		const GraphicsUnit& graphicsUnit_;
		const RenderPass& renderPass_;
		const RenderSurface& renderSurface_;

		std::vector<Image*> swapchainImages_{};
		std::vector<FrameBuffer*> frameBuffers_{};

		// Syncro object currently raw pointers to allow storing unknown amount. Should look into setting this through constexpr or something to allow initialisation earlier.
		// TODO -> Move getting numbere of images to use outside of swapchain (However swapchain can choose a different amount anyway... Then pass to a semaphore manager class that will create the amount needed.
		std::vector<Fence*> fences_{};
		std::vector<Semaphore*> imageAcquiredSemaphore_{};
		std::vector<Semaphore*> drawCompleteSemaphore_{};

		// Swapchain settings
		uint32_t swapchainImageCount_ = 3;
		vk::Format swapchainImageFormat_{};
		vk::SurfaceTransformFlagBitsKHR swapchainTransform_{};
		vk::PresentModeKHR swapchainPresentMode_{};

		vk::Extent2D size_;

		friend class Queue;
	};
}

