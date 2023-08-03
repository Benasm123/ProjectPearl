#pragma once
#include "PearlCore.h"
#include "RendererInstance.h"
#include "vulkan/vulkan.hpp"
#include "Window/RenderSurface.h"
#include "Queue.h"
#include "BDVK/BDVK_enums.h"

class StaticMesh;

namespace PEARL_NAMESPACE
{

	// Class to manage the GPU, by default select the best gpu.
	class GraphicsUnit
	{
	public:
		// TODO -> Make a way to add a specific graphics card, or pass a graphics card/index.
		GraphicsUnit(const RendererInstance& instance);
		~GraphicsUnit();

		bool WaitForFences(Fence* fences);

		void WaitIdle();

		[[nodiscard]] Queue GetGraphicsQueue();

		[[nodiscard]] uint32_t GetGraphicsQueueIndex() const { return graphicsQueueIndex_; }
		[[nodiscard]] uint32_t GetComputeQueueIndex() const { return computeQueueIndex_; }

		[[nodiscard]] PEARL_NAMESPACE::typesRender::BufferResource CreateBufferResource(size_t size, bdvk::BufferType usage);

		void DestroyMesh(const StaticMesh& mesh);

	private:
		[[nodiscard]] struct vk::PhysicalDeviceLimits GetLimits() const;
		[[nodiscard]] struct vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities(const RenderSurface& surface) const;
		std::vector<struct vk::SurfaceFormatKHR> GetSurfaceFormats(const RenderSurface& surface) const;
		struct vk::SurfaceFormatKHR GetBestSurfaceFormat(const RenderSurface& surface) const;
		std::vector<enum vk::PresentModeKHR> GetSurfacePresentModes(const RenderSurface& surface) const;
		uint32_t GetMemoryIndexOfType(class vk::Flags<vk::MemoryPropertyFlagBits> memoryProperty) const;

		[[nodiscard]] class vk::SwapchainKHR CreateSwapchain(const struct vk::SwapchainCreateInfoKHR& createInfo) const;
		std::vector<class vk::Image> GetSwapchainImages(class vk::SwapchainKHR swapchain) const;
		void DestroySwapchain(const class vk::SwapchainKHR swapchainToDestroy) const;

		[[nodiscard]] class vk::RenderPass CreateRenderPass(const vk::RenderPassCreateInfo& renderPassInfo) const;
		void DestroyRenderPass(const vk::RenderPass renderPass) const;

		[[nodiscard]] class vk::ImageView CreateImageView(class vk::Image image, enum class vk::Format format, enum class vk::ImageAspectFlagBits imageAspect) const;

		[[nodiscard]] class vk::Buffer CreateBuffer(size_t size, bdvk::BufferType usageFlags, enum vk::SharingMode sharingMode = vk::SharingMode::eExclusive);
		[[nodiscard]] class vk::DeviceMemory AllocateMemory(struct vk::MemoryRequirements requirements, class vk::Flags<vk::MemoryPropertyFlagBits> memoryFlags);
		[[nodiscard]] void* BindAndMapBufferMemory(class vk::Buffer buffer, class vk::DeviceMemory memory,  uint64_t offset = 0, uint64_t size = VK_WHOLE_SIZE);

	private:
		std::string name_;

		std::vector<const char*> layers_;
		std::vector<const char*> extensions_;

		struct vk::PhysicalDeviceFeatures requiredFeatures_;

		const RendererInstance& instance_;

		class vk::PhysicalDevice graphicsUnit_;
		class vk::Device logicalUnit_;

		uint32_t graphicsQueueIndex_ = ~0u;
		uint32_t computeQueueIndex_ = ~0u;

		Queue graphicsQueue_;
		Queue computeQueue_;

		friend class Queue;
		friend class FrameBuffer;
		friend class PipelineLayout;
		friend class GraphicsPipeline;
		friend class CommandPool;
		friend class Image;
		friend class Swapchain;
		friend class Fence;
		friend class Semaphore;
		friend class CommandBuffer;
		friend class RenderPass;
	};
}
