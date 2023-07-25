
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
#include "Window/RenderSurface.h"
#include "Window/Window.h"


class VulkanRenderer
{
public:
	VulkanRenderer(const pearl::Window& window);
	~VulkanRenderer();

	virtual void DrawMesh(pearl::typesRender::Mesh& mesh);
	void DestroyMesh(const pearl::typesRender::Mesh& mesh);
	bool Update();
	void WaitFinishRender() const;

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

	pearl::CommandPool commandPool_;
	std::vector<pearl::CommandBuffer> commandBuffers_;
	std::vector<vk::DescriptorSet> descriptorSets_;

	uint32_t currentRenderIndex_ = 0;

	std::vector<pearl::typesRender::Mesh*> meshes_;
	
	//TODO -- Consider moving this
	vk::Rect2D scissor_;
	vk::Viewport viewport_;

	Camera camera_;
}; 
