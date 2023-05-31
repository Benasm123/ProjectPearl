#include "pch.h"
#include "../../Pearl/source/Render/Vulkan/RendererInstance.h"
#include "../../Pearl/source/Render/Vulkan/GraphicsUnit.h"
#include "../../Pearl/source/Render/Vulkan/RenderPass.h"

using namespace pearl;

TEST(RenderPass, Creation)
{
	const Window window("Test", 100, 100);

	RendererInstance instance(window);

	const GraphicsUnit graphicsUnit(instance, instance.FindBestGraphicsUnit());

	const RenderSurface renderSurface(instance, window);

	const RenderPass renderPass(graphicsUnit, renderSurface);
	ASSERT_TRUE(renderPass.Get());
}