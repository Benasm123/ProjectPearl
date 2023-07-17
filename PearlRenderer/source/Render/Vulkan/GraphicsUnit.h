#pragma once
#include "PearlCore.h"
#include "RendererInstance.h"
#include "vulkan/vulkan.hpp"
#include "Window/RenderSurface.h"


namespace PEARL_NAMESPACE
{
	// Class to manage the GPU, by default select the best gpu.
	class GraphicsUnit
	{
	public:
		GraphicsUnit(const RendererInstance& instance, vk::PhysicalDevice device);
		~GraphicsUnit();

		[[nodiscard]] vk::PhysicalDeviceLimits GetLimits() const;
		[[nodiscard]] vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities(const RenderSurface& surface) const;
		std::vector<vk::SurfaceFormatKHR> GetSurfaceFormats(const RenderSurface& surface) const;
		vk::SurfaceFormatKHR GetBestSurfaceFormat(const RenderSurface& surface) const;
		std::vector<vk::PresentModeKHR> GetSurfacePresentModes(const RenderSurface& surface) const;
		uint32_t GetMemoryIndexOfType(vk::MemoryPropertyFlags memoryProperty) const;

		[[nodiscard]] const vk::PhysicalDevice& GetPhysical() const { return graphicsUnit_; }
		[[nodiscard]] const vk::Device& GetLogical() const { return logicalUnit_; }

		[[nodiscard]] vk::SwapchainKHR CreateSwapchain(const vk::SwapchainCreateInfoKHR& createInfo) const;
		std::vector<vk::Image> GetSwapchainImages(vk::SwapchainKHR swapchain) const;
		void DestroySwapchain(const vk::SwapchainKHR swapchainToDestroy) const;

		[[nodiscard]] vk::RenderPass CreateRenderPass(const vk::RenderPassCreateInfo& renderPassInfo) const;
		void DestroyRenderPass(const vk::RenderPass renderPass) const;

		[[nodiscard]] vk::ImageView CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlagBits imageAspect) const;

		[[nodiscard]] uint32_t GetGraphicsQueueIndex() const { return graphicsQueueIndex_; }
		[[nodiscard]] uint32_t GetComputeQueueIndex() const { return computeQueueIndex_; }

		[[nodiscard]] vk::Queue GetGraphicsQueue();

		[[nodiscard]] vk::Buffer CreateBuffer(size_t size, vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode=vk::SharingMode::eExclusive);
		[[nodiscard]] vk::DeviceMemory AllocateMemory(vk::MemoryRequirements requirements, vk::MemoryPropertyFlags memoryFlags);
		[[nodiscard]] void* BindAndMapBufferMemory(vk::Buffer buffer, vk::DeviceMemory memory, vk::DeviceSize offset=0, vk::DeviceSize size=VK_WHOLE_SIZE);
		[[nodiscard]] PEARL_NAMESPACE::typesRender::BufferResource CreateBufferResource(size_t size, vk::BufferUsageFlags usage);
	private:
		std::string name_;

		std::vector<const char*> layers_;
		std::vector<const char*> extensions_;

		vk::PhysicalDeviceFeatures requiredFeatures_;

		const RendererInstance& instance_;

		vk::PhysicalDevice graphicsUnit_;
		vk::Device logicalUnit_;

		uint32_t graphicsQueueIndex_ = ~0u;
		uint32_t computeQueueIndex_ = ~0u;

		uint32_t graphicsQueuePos_ = 0;
		std::vector<vk::Queue> graphicsQueues_;
		uint32_t computeQueuePos_ = 0;
		std::vector<vk::Queue> computeQueues_;
	};
}
