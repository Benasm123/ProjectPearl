#include "VulkanRenderer.h"

#include <iostream>
#include <Core/Config.h>
#include "Core/Logger.h"

#pragma warning(push, 0)
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <Render/Vulkan/BDVK/BDVK_enums.h>
#pragma warning(pop)


/**
 * \brief A 2D vulkan renderer allowing rendering to a window.
 * \param window The window to render to.
 */
VulkanRenderer::VulkanRenderer(const pearl::Window& window)
	: window_{window}
	, instance_{window_}
	, graphicsUnit_{instance_}
	, renderSurface_{instance_, window_}
	, renderPass_{graphicsUnit_, renderSurface_}
	, swapchain_{ graphicsUnit_, renderPass_, renderSurface_ }
	, graphicsPipelineLayout_{graphicsUnit_}
	, graphicsPipeline_{graphicsUnit_, renderSurface_, renderPass_, graphicsPipelineLayout_}
	, commandPool_{graphicsUnit_, graphicsUnit_.GetGraphicsQueueIndex()}
	, camera_{45.0f, swapchain_.GetSize()}
	, descriptorSets_{graphicsPipelineLayout_}
{
	for (uint32_t i = 0; i < swapchain_.GetImageCount(); i++) {
		commandBuffers_.push_back(new pearl::CommandBuffer(commandPool_));
	}

	SetupRenderArea();
}

VulkanRenderer::~VulkanRenderer()
{
	graphicsUnit_.WaitIdle();
};

///
/// Prepare a mesh to be rendered, adding it to list of meshes and creating its vertex and index buffers.
/// @param mesh The mesh which is to rendered.
///
void VulkanRenderer::DrawMesh(pearl::typesRender::Mesh& mesh)
{
	// TODO -- this should be in a mesh class. should also look into abstracting all vk types out to reduce dependancy.
	// turn meshes into handle that will then have a look up for all needed renderer types?
	mesh.vertexResource = graphicsUnit_.CreateBufferResource(mesh.data.points.size() * sizeof(mesh.data.points[0]), bdvk::BufferType::Vertex);
	std::memcpy(mesh.vertexResource.dataPtr, mesh.data.points.data(), mesh.data.points.size() * sizeof(mesh.data.points[0]));

	
	mesh.indexResource = graphicsUnit_.CreateBufferResource(mesh.data.triangles.size() * sizeof(mesh.data.triangles[0]), bdvk::BufferType::Index);
	std::memcpy(mesh.indexResource.dataPtr, mesh.data.triangles.data(), mesh.data.triangles.size() * sizeof(mesh.data.triangles[0]));
			
	meshes_.push_back(&mesh);

	LOG_TRACE("Drawing mesh with {} triangles", mesh.data.triangles.size());
}

void VulkanRenderer::DestroyMesh(const pearl::typesRender::Mesh& mesh)
{
	graphicsUnit_.WaitIdle();

	graphicsUnit_.DestroyMesh(mesh);
}


void VulkanRenderer::BuildCommandBufferCommands(const uint32_t index)
{
	pearl::CommandBuffer currentBuffer = *commandBuffers_[index];

	currentBuffer.Reset();

	currentBuffer.Begin();
	currentBuffer.BeginRenderPass(renderPass_, *swapchain_.GetFramebuffers()[index], renderSurface_);

	currentBuffer.BindPipeline(graphicsPipeline_);

	// TODO -> Can add scissor and viewport to renderSurface class and allow commandBuffers to access these with a SetRenderSurface method.
	currentBuffer.SetRenderSurface(renderSurface_);

	currentBuffer.BindDescriptorSets(bdvk::PipelineBindPoint::Graphics, graphicsPipelineLayout_, descriptorSets_);

	for (auto& mesh : meshes_)
	{
		currentBuffer.PushConstants(graphicsPipelineLayout_, pearl::typesRender::PushConstantInfo{bdvk::ShaderType::Vertex, mesh->mvp});

		currentBuffer.DrawIndexed(*mesh);
	}

	currentBuffer.EndRenderPass();
	currentBuffer.End();
}


bool VulkanRenderer::Render()
{
	// TODO -> Fences needs to be wrapped.
	bool res = graphicsUnit_.WaitForFences(swapchain_.GetFences()[currentRenderIndex_]);
	if (!res) return false;

	const uint32_t nextImageIndex = swapchain_.GetNextImageIndex(currentRenderIndex_);
	if (nextImageIndex == ~0u) return false;

	BuildCommandBufferCommands(currentRenderIndex_);

	SubmitGraphicsQueue();

	bool presentSucceded = Present(nextImageIndex);

	currentRenderIndex_ = (currentRenderIndex_ + 1) % swapchain_.GetImageCount();

	if (!presentSucceded)
	{
		OnResize();
	}

	return true;
}

void VulkanRenderer::SubmitGraphicsQueue() 
{

	// TODO -> This needs to be moved to a mesh update function once we have a real mesh class and updated in update.
	for (pearl::typesRender::Mesh* mesh : meshes_)
	{
		glm::mat4 mvp = camera_.GetPerspective() * camera_.GetView();

		mesh->modelMatrix = glm::translate(glm::mat4(1.0f), mesh->position);

		mesh->mvp.mvp = mvp * mesh->modelMatrix;
	}

	graphicsUnit_.GetGraphicsQueue().Submit(
		bdvk::PipelineStage::ColourOutput
		, *commandBuffers_[currentRenderIndex_]
		, { swapchain_.GetImageAcquiredSemaphores()[currentRenderIndex_] }
		, { swapchain_.GetDrawCompletedSemaphores()[currentRenderIndex_] }
		, swapchain_.GetFences()[currentRenderIndex_]
	);
}



bool VulkanRenderer::Present(uint32_t nextImageIndex)
{
	return graphicsUnit_.GetGraphicsQueue().Present(
		swapchain_
		, nextImageIndex
		, { swapchain_.GetDrawCompletedSemaphores()[currentRenderIndex_] }
	);
}


void VulkanRenderer::OnResize() {
	swapchain_.Recreate();
	currentRenderIndex_ = 0;
	camera_.SetViewArea(swapchain_.GetSize());
	camera_.UpdatePerspective();

	SetupRenderArea();
}


void VulkanRenderer::SetupRenderArea() 
{
	renderSurface_.Resize();
}


bool VulkanRenderer::Update()
{
	if (!Render()) LOG_INFO("FAILED");
	return true;
}
