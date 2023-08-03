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

		void Resize();

	private:
		const vk::Instance& instance_;
		const Window& owningWindow_;
		vk::SurfaceKHR surface_{};

		vk::Rect2D scissor_{};
		vk::Viewport viewport_{};

		friend class CommandBuffer;
		friend class GraphicsUnit;
		friend class Swapchain;
	};
}

