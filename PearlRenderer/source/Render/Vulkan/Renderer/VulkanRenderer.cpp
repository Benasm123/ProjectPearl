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
	, graphicsUnit_{instance_, instance_.FindBestGraphicsUnit()} // TODO -> This can maybe be defaulted and take out the vk::Device requirement, instead provide some way to select a gpu, through gpu struct.
	, renderSurface_{instance_, window_}
	, renderPass_{graphicsUnit_, renderSurface_}
	, swapchain_{ graphicsUnit_, renderPass_, renderSurface_ }
	, graphicsPipelineLayout_{graphicsUnit_}
	, graphicsPipeline_{graphicsUnit_, renderSurface_, renderPass_, graphicsPipelineLayout_}
	, commandPool_{graphicsUnit_, graphicsUnit_.GetGraphicsQueueIndex()}
	, camera_{45.0f, {swapchain_.GetSize().width, swapchain_.GetSize().height}}
	, descriptorSets_{graphicsPipelineLayout_}
{
	commandBuffers_ = commandPool_.AllocateCommandBuffers(swapchain_.GetImageCount());

	SetupRenderArea();
}

VulkanRenderer::~VulkanRenderer()
{
	WaitFinishRender();
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
	WaitFinishRender();

	graphicsUnit_.GetLogical().unmapMemory(mesh.vertexResource.memory);
	graphicsUnit_.GetLogical().freeMemory(mesh.vertexResource.memory);
	graphicsUnit_.GetLogical().destroyBuffer(mesh.vertexResource.buffer);

	graphicsUnit_.GetLogical().unmapMemory(mesh.indexResource.memory);
	graphicsUnit_.GetLogical().freeMemory(mesh.indexResource.memory);
	graphicsUnit_.GetLogical().destroyBuffer(mesh.indexResource.buffer);
}


void VulkanRenderer::BuildCommandBufferCommands(const uint32_t index)
{
	pearl::CommandBuffer currentBuffer = commandBuffers_[index];

	currentBuffer.Reset();

	currentBuffer.Begin();
	currentBuffer.BeginRenderPass(renderPass_, *swapchain_.GetFramebuffers()[index], scissor_);

	currentBuffer.BindPipeline(graphicsPipeline_);

	// TODO -> Can add scissor and viewport to renderSurface class and allow commandBuffers to access these with a SetRenderSurface method.
	currentBuffer.SetScissor(scissor_);
	currentBuffer.SetViewport(viewport_);

	currentBuffer.BindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipelineLayout_, descriptorSets_);

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
	graphicsUnit_.WaitForFences({ swapchain_.GetFences()[currentRenderIndex_] });

	const uint32_t nextImageIndex = swapchain_.GetNextImageIndex(currentRenderIndex_);

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
		, commandBuffers_[currentRenderIndex_]
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
	camera_.SetViewArea({ swapchain_.GetSize().width, swapchain_.GetSize().height });
	camera_.UpdatePerspective();

	SetupRenderArea();
}


void VulkanRenderer::SetupRenderArea() {
	const vk::Extent2D renderExtent = graphicsUnit_.GetSurfaceCapabilities(renderSurface_).currentExtent;

	viewport_
		.setWidth(static_cast<float>(renderExtent.width))
		.setHeight(static_cast<float>(renderExtent.height))
		.setX(0)
		.setY(0)
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f);


	scissor_
		.setExtent(renderExtent)
		.setOffset({ 0, 0 });
}


bool VulkanRenderer::Update()
{
	Render();
	return true;
}

void VulkanRenderer::WaitFinishRender() const
{
	graphicsUnit_.GetLogical().waitIdle();
}
