#include "RenderSurface.h"

#include "SDL2/SDL_vulkan.h"

using namespace PEARL_NAMESPACE;

RenderSurface::RenderSurface(const RendererInstance& instance, const Window& window)
	:instance_(instance.instance_),
	owningWindow_(window)
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(window.Get(), instance.instance_, &surface);
	surface_ = vk::SurfaceKHR{ surface };

	Resize();
}

RenderSurface::~RenderSurface()
{
	instance_.destroySurfaceKHR(surface_);
}


void RenderSurface::Resize() 
{
	const glm::uvec2 renderExtent = owningWindow_.GetSize();

	viewport_
		.setWidth(static_cast<float>(renderExtent.x))
		.setHeight(static_cast<float>(renderExtent.y))
		.setX(0)
		.setY(0)
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f);


	scissor_
		.setExtent({ renderExtent.x, renderExtent.y })
		.setOffset({ 0, 0 });
}