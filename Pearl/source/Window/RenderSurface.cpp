#include "RenderSurface.h"

#include "SDL2/SDL_vulkan.h"

using namespace PEARL_NAMESPACE;

RenderSurface::RenderSurface(const RendererInstance& instance, const Window& window)
	:instance_(instance.Get())
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(window.Get(), instance.Get(), &surface);
	surface_ = vk::SurfaceKHR{ surface };
}

RenderSurface::~RenderSurface()
{
	instance_.destroySurfaceKHR(surface_);
}
