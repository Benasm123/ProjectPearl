
#pragma once
#include <map>

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
	VulkanRenderer(const pearl::Window& window, Camera* camera);
	~VulkanRenderer();

	void DrawMesh(StaticMesh& mesh);
	void DestroyMesh(const StaticMesh& mesh);
	bool Update();

private:
	void BuildCommandBufferCommands(uint32_t index);
	bool Render();

	void SubmitGraphicsQueue();

	bool Present(uint32_t next_image_index);

	void OnResize();
	void SetupRenderArea();

private:
	const pearl::Window& window_;

	pearl::RendererInstance instance_;
	pearl::GraphicsUnit graphics_unit_;
	pearl::RenderSurface render_surface_;
	pearl::RenderPass render_pass_;
	pearl::Swapchain swapchain_;

	pearl::PipelineLayout graphics_pipeline_layout_;
	std::unordered_map<std::string, pearl::GraphicsPipeline*> graphics_pipeline_;

	pearl::DescriptorSets descriptor_sets_;

	pearl::CommandPool command_pool_;
	std::vector<pearl::CommandBuffer*> command_buffers_;

	uint32_t current_render_index_ = 0;

	std::vector<StaticMesh*> meshes_;

	Camera* camera_;
}; 
