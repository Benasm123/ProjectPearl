#include "VulkanRenderer.h"

#include <iostream>
#include <Core/Config.h>
#include "Core/Logger.h"

#pragma warning(push, 0)
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Render/Vulkan/BDVK/BDVK_enums.h>
#include <Core/StaticMesh.h>
#pragma warning(pop)

#include "Render/Vulkan/ShaderDescriptor.h"


/**
 * \brief A 2D vulkan renderer allowing rendering to a window.
 * \param window The window to render to.
 * \param camera The camera to use, or nullptr to create one.
 */
VulkanRenderer::VulkanRenderer(const pearl::Window& window, Camera* camera)
	: window_{window}
	, instance_{window_}
	, graphics_unit_{instance_}
	, render_surface_{instance_, window_}
	, render_pass_{graphics_unit_, render_surface_}
	, swapchain_{ graphics_unit_, render_pass_, render_surface_ }
	, graphics_pipeline_layout_{graphics_unit_}
	, descriptor_sets_{graphics_pipeline_layout_}
	, command_pool_{graphics_unit_, graphics_unit_.GetGraphicsQueueIndex()}
	, camera_{ camera }
{
	if (!camera)
	{
		camera_ = new Camera{ 45.0f, swapchain_.GetSize() };
	}

	for (uint32_t i = 0; i < swapchain_.GetImageCount(); i++) {
		command_buffers_.push_back(new pearl::CommandBuffer(command_pool_));
	}

	SetupRenderArea();

	ShaderInputs vertex_input = {
		ShaderInputType::UniformBuffer,
		kVertex,
		{
			InputValueType::kVec3Float32,
			InputValueType::kVec3Float32,
			InputValueType::kVec2Float32
		}
	};

	const ShaderDescriptor default_vertex_shader("Basic.vert.spv", kVertex, { vertex_input });
	const ShaderDescriptor default_fragment_shader("Basic.frag.spv", kFragment, {});
	const ShaderDescriptor debug_vertex_shader("Debug.vert.spv", kVertex, { vertex_input });

	const std::vector<ShaderDescriptor> pipeline_shaders = { default_vertex_shader, default_fragment_shader };
	const std::vector<ShaderDescriptor> debug_pipeline_shaders = { debug_vertex_shader, default_fragment_shader };

	const pearl::GraphicsPipelineInfo default_info = {
		.primitive_type = vk::PrimitiveTopology::eTriangleList,
		.polygon_mode = vk::PolygonMode::eFill,
		.cull_mode = vk::CullModeFlagBits::eBack,
		.depth_enabled = VK_TRUE,
		.shader_infos = pipeline_shaders,
	};

	graphics_pipeline_["Default"] = new pearl::GraphicsPipeline(graphics_unit_, render_surface_, render_pass_, graphics_pipeline_layout_, default_info);

	const pearl::GraphicsPipelineInfo debug_info = {
		.primitive_type = vk::PrimitiveTopology::eTriangleList,
		.polygon_mode = vk::PolygonMode::eLine,
		.cull_mode = vk::CullModeFlagBits::eBack,
		.depth_enabled = VK_FALSE,
		.shader_infos = debug_pipeline_shaders,
	};

	// TODO -> Need to create a GraphicsPipelineInfo struct containing all values that can be modified, with sensible defaults, so only those that need changing need changing. Defaults probably equate to the "Default" renderer.
	graphics_pipeline_["Debug"] = new pearl::GraphicsPipeline(graphics_unit_, render_surface_, render_pass_, graphics_pipeline_layout_, debug_info);
}

VulkanRenderer::~VulkanRenderer()
{
	graphics_unit_.WaitIdle();

	for (auto pipeline = graphics_pipeline_.begin() ; pipeline != graphics_pipeline_.end(); ++pipeline)
	{
		delete pipeline->second;
	}
};

void VulkanRenderer::DrawMesh(StaticMesh& mesh)
{
	// TODO -- this should be in a mesh class. should also look into abstracting all vk types out to reduce dependancy.
	// turn meshes into handle that will then have a look up for all needed renderer types?
	mesh.vertexResource_ = graphics_unit_.CreateBufferResource(mesh.data_.points.size() * sizeof(mesh.data_.points[0]), bdvk::BufferType::Vertex);
	std::memcpy(mesh.vertexResource_.dataPtr, mesh.data_.points.data(), mesh.data_.points.size() * sizeof(mesh.data_.points[0]));


	mesh.indexResource_ = graphics_unit_.CreateBufferResource(mesh.data_.triangles.size() * sizeof(mesh.data_.triangles[0]), bdvk::BufferType::Index);
	std::memcpy(mesh.indexResource_.dataPtr, mesh.data_.triangles.data(), mesh.data_.triangles.size() * sizeof(mesh.data_.triangles[0]));

	meshes_.push_back(&mesh);

	LOG_TRACE("Drawing mesh with {} triangles", mesh.data_.triangles.size());
}

void VulkanRenderer::DestroyMesh(const StaticMesh& mesh)
{
	graphics_unit_.WaitIdle();

	graphics_unit_.DestroyMesh(mesh);
}

void VulkanRenderer::BuildCommandBufferCommands(const uint32_t index)
{
	pearl::CommandBuffer current_buffer = *command_buffers_[index];

	current_buffer.Reset();

	current_buffer.Begin();
	current_buffer.BeginRenderPass(render_pass_, *swapchain_.GetFramebuffers()[index], render_surface_);

	// TODO -> Can add scissor and viewport to renderSurface class and allow commandBuffers to access these with a SetRenderSurface method.
	current_buffer.SetRenderSurface(render_surface_);

	current_buffer.BindPipeline(*graphics_pipeline_["Default"]);

	for (const auto& mesh : meshes_)
	{
		// mesh->mvp_ = {glm::mat4(1.0)};

		current_buffer.PushConstants(graphics_pipeline_layout_, pearl::typesRender::PushConstantInfo{bdvk::ShaderType::Vertex, mesh->mvp_});
	
		current_buffer.DrawIndexed(*mesh);
	}

	current_buffer.BindPipeline(*graphics_pipeline_["Debug"]);

	for (const auto& mesh : meshes_)
	{
		current_buffer.PushConstants(graphics_pipeline_layout_, pearl::typesRender::PushConstantInfo{bdvk::ShaderType::Vertex, mesh->mvp_});

		current_buffer.DrawIndexed(*mesh);
	}

	current_buffer.EndRenderPass();
	current_buffer.End();
}


bool VulkanRenderer::Render()
{
	if (window_.IsMinimised()) return false;

	if (!graphics_unit_.WaitForFences(swapchain_.GetFences()[current_render_index_]))
		return false;

	const uint32_t nextImageIndex = swapchain_.GetNextImageIndex(current_render_index_);
	if (nextImageIndex == ~0u) return false;

	BuildCommandBufferCommands(current_render_index_);

	SubmitGraphicsQueue();

	const bool present_succeeded = Present(nextImageIndex);

	current_render_index_ = (current_render_index_ + 1) % swapchain_.GetImageCount();

	if (!present_succeeded)
	{
		vk::Extent2D size = graphics_unit_.GetSurfaceCapabilities(render_surface_).currentExtent;
		if (size.width == 0 || size.height == 0) return true;
		OnResize();
	}

	return true;
}

void VulkanRenderer::SubmitGraphicsQueue() 
{
	// TODO -> This needs to be moved to a mesh update function once we have a real mesh class and updated in update.
	for (StaticMesh* mesh : meshes_)
	{
		glm::mat4 mvp = camera_->GetPerspective() * camera_->GetView();

		mesh->modelMatrix_ = glm::translate(glm::mat4(1.0f), mesh->position_);
		//mvp * 
		mesh->mvp_.mvp = mvp * mesh->modelMatrix_;
	}

	graphics_unit_.GetGraphicsQueue().Submit(
		bdvk::PipelineStage::ColourOutput
		, *command_buffers_[current_render_index_]
		, { swapchain_.GetImageAcquiredSemaphores()[current_render_index_] }
		, { swapchain_.GetDrawCompletedSemaphores()[current_render_index_] }
		, swapchain_.GetFences()[current_render_index_]
	);
}



bool VulkanRenderer::Present(uint32_t next_image_index)
{
	return graphics_unit_.GetGraphicsQueue().Present(
		swapchain_
		, next_image_index
		, { swapchain_.GetDrawCompletedSemaphores()[current_render_index_] }
	);
}


void VulkanRenderer::OnResize() {
	swapchain_.Recreate();
	current_render_index_ = 0;
	camera_->SetViewArea(swapchain_.GetSize());
	camera_->UpdatePerspective();

	SetupRenderArea();
}


void VulkanRenderer::SetupRenderArea() 
{
	render_surface_.Resize();
}


bool VulkanRenderer::Update()
{
	Render();
	return true;
}
