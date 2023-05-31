#pragma once
#include "PearlCore.h"
#include "Window.h"
#include "..\Render\Vulkan\RendererInstance.h"

namespace PEARL_NAMESPACE
{
	class RenderSurface
	{
	public:
		RenderSurface(const RendererInstance& instance, const Window& window);
		~RenderSurface();

		[[nodiscard]] const vk::SurfaceKHR& Get() const { return surface_; }

	private:
		const vk::Instance& instance_;
		vk::SurfaceKHR surface_{};
	};
}

