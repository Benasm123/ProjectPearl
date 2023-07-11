// ReSharper disable All
#include "pch.h"
#include "../../PearlRenderer/source/Render/Vulkan/Swapchain.h"
#include "../../PearlRenderer/source/Render/Vulkan/GraphicsUnit.h"
#include "../../PearlRenderer/source/Render/Vulkan/RendererInstance.h"
#include "../../PearlRenderer/source/Render/Vulkan/RenderPass.h"

using namespace pearl;

TEST(Swapchains, Creation)
{
	Window window("Test", 100, 100);

	RendererInstance instance(window);

	GraphicsUnit graphicsUnit(instance, instance.FindBestGraphicsUnit());

	RenderSurface renderSurface(instance, window);

	RenderPass renderPass(graphicsUnit, renderSurface);

	Swapchain swapchain(graphicsUnit, renderPass, renderSurface);
	ASSERT_TRUE(swapchain.Get());
}