#pragma once
#include "PearlCore.h"
#include "vulkan/vulkan.hpp"
#include "Window/Window.h"


namespace PEARL_NAMESPACE
{
	class RendererInstance
	{
	public:
		RendererInstance(const Window& window, const std::vector<const char*>& additionalLayers = {}, const std::vector<const char*>& additionalExtensions = {});
		~RendererInstance();

		[[nodiscard]] const vk::Instance& Get() const { return instance_; }
		vk::PhysicalDevice FindBestGraphicsUnit(std::vector<const char*> additionalExtensions = {});

		[[nodiscard]] const std::vector<const char*>& GetLayers() const { return requiredInstanceLayers_; }
		[[nodiscard]] const std::vector<const char*>& GetExtensions() const { return requiredInstanceExtension_; }
		[[nodiscard]] const std::vector<const char*>& GetDeviceExtensions() const { return requiredDeviceExtensions_; }

	private:
		vk::Instance instance_;
		const Window& window_;

		std::vector<const char*> requiredInstanceLayers_;

		std::vector<const char*> requiredInstanceExtension_ = {
			"VK_KHR_surface"
		};

		std::vector<const char*> requiredDeviceExtensions_ = {
			"VK_KHR_swapchain"
		};
	};
}

