#pragma once
#include "Render/Vulkan/RendererInstance.h"
#include "Render/Generic/Renderer2D.h"
#include "Render/Vulkan/GraphicsUnit.h"
#include "Render/Vulkan/RenderPass.h"
#include "Render/Vulkan/Swapchain.h"
#include "Window/RenderSurface.h"
#include "Window/Window.h"


class VulkanRenderer2D :
    public Renderer2D
{
public:
	VulkanRenderer2D(const pearl::Window& window);
	~VulkanRenderer2D() override;

	virtual void DrawLine(Types2D::Point2D start, Types2D::Point2D end) override;
	virtual void DrawLines(std::vector<Types2D::Point2D> points) override;
	virtual void DrawRect(Types2D::Rect2D rect) override;
	virtual void DrawRects(std::vector<Types2D::Rect2D> rects) override;
	virtual bool Render() override;
	virtual bool Update() override;

private:

private:
	const pearl::Window& window_;
	pearl::RendererInstance instance_;
	pearl::GraphicsUnit graphicsUnit_;
	pearl::RenderSurface renderSurface_;
	pearl::RenderPass renderPass_;
	pearl::Swapchain swapchain_;

	// To remove with wrapper classes
	std::vector<vk::CommandBuffer> commandBuffers_{};
	vk::Pipeline graphicsPipeline_{};
	vk::Pipeline computePipeline_{};

	// Swapchain settings
	uint32_t swapchainImageCount_ = 3;
	vk::Format swapchainImageFormat_{};
	vk::SurfaceTransformFlagBitsKHR swapchainTransform_{};
	vk::PresentModeKHR swapchainPresentMode_{};
};

