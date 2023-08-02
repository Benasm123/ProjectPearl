
#pragma once
#include "Core/Camera.h"
#include "Render/Vulkan/RendererInstance.h"
#include "Render/Vulkan/CommandBuffer.h"
#include "Render/Vulkan/CommandPool.h"
#include "Render/Vulkan/GraphicsPipeline.h"
#include "Render/Vulkan/GraphicsUnit.h"
#include "Render/Vulkan/PipelineLayout.h"
#include "Render/Vulkan/RenderPass.h"
#include "Render/Vulkan/Swapchain.h"
#include "Render/Vulkan/DescriptorSets.h"
#include "Window/RenderSurface.h"
#include "Window/Window.h"

class StaticMesh;

class VulkanRenderer
{
public:
	VulkanRenderer(const pearl::Window& window);
	~VulkanRenderer();

	void DrawMesh(StaticMesh& mesh);
	void DestroyMesh(const StaticMesh& mesh);
	bool Update();

private:
	void BuildCommandBufferCommands(uint32_t index);
	bool Render();

	void SubmitGraphicsQueue();

	bool Present(uint32_t nextImageIndex);

	void OnResize();
	void SetupRenderArea();

private:
	const pearl::Window& window_;

	pearl::RendererInstance instance_;
	pearl::GraphicsUnit graphicsUnit_;
	pearl::RenderSurface renderSurface_;
	pearl::RenderPass renderPass_;
	pearl::Swapchain swapchain_;

	pearl::PipelineLayout graphicsPipelineLayout_;
	pearl::GraphicsPipeline graphicsPipeline_;

	pearl::DescriptorSets descriptorSets_;

	pearl::CommandPool commandPool_;
	std::vector<pearl::CommandBuffer*> commandBuffers_;

	uint32_t currentRenderIndex_ = 0;

	std::vector<StaticMesh*> meshes_;

	Camera camera_;
}; 
