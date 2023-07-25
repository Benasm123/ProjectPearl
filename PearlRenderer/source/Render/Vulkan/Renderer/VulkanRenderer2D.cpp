#include "VulkanRenderer2D.h"

#include <iostream>
#include <Core/Config.h>
#include "Core/Logger.h"

#pragma warning(push, 0)
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#pragma warning(pop)


/**
 * \brief A 2D vulkan renderer allowing rendering to a window.
 * \param window The window to render to.
 */
VulkanRenderer2D::VulkanRenderer2D(const pearl::Window& window)
	: window_{window}
	, instance_{window_}
	, graphicsUnit_{instance_, instance_.FindBestGraphicsUnit()}
	, renderSurface_{instance_, window_}
	, renderPass_{graphicsUnit_, renderSurface_}
	, swapchain_{ graphicsUnit_, renderPass_, renderSurface_ }
	, graphicsPipelineLayout_{graphicsUnit_}
	, graphicsPipeline_{graphicsUnit_, renderSurface_, renderPass_, graphicsPipelineLayout_}
	, commandPool_{graphicsUnit_, graphicsUnit_.GetGraphicsQueueIndex()}
	, camera_{45.0f, {swapchain_.GetSize().width, swapchain_.GetSize().height}}
{
	commandBuffers_ = commandPool_.AllocateCommandBuffers(swapchain_.GetImageCount());
	for (uint32_t i = 0 ; i < swapchain_.GetImageCount(); i++)
	{
		descriptorSets_.push_back(graphicsPipelineLayout_.AllocateDescriptorSet(1)[0]);
	}

	SetupRenderArea();
}

VulkanRenderer2D::~VulkanRenderer2D()
{
	WaitFinishRender();
};

///
/// Draw a line
/// @param start The point in the world space the drawn line will start.
/// @param end The point in the world space the drawn line will end.
///
void VulkanRenderer2D::DrawLine(PEARL_NAMESPACE::types2D::Point2D start, PEARL_NAMESPACE::types2D::Point2D end)
{
}

void VulkanRenderer2D::DrawLines(std::vector<PEARL_NAMESPACE::types2D::Point2D> points)
{
}

void VulkanRenderer2D::DrawRect(PEARL_NAMESPACE::types2D::Rect2D rect)
{
}

void VulkanRenderer2D::DrawRects(std::vector<PEARL_NAMESPACE::types2D::Rect2D> rects)
{
}

///
/// Prepare a mesh to be rendered, adding it to list of meshes and creating its vertex and index buffers.
/// @param mesh The mesh which is to rendered.
///
void VulkanRenderer2D::DrawMesh(pearl::typesRender::Mesh& mesh)
{
	// TODO -- this should be in a mesh class. should also look into abstracting all vk types out to reduce dependancy.
	// turn meshes into handle that will then have a look up for all needed renderer types?
	mesh.vertexResource = graphicsUnit_.CreateBufferResource(mesh.data.points.size() * sizeof(mesh.data.points[0]), vk::BufferUsageFlagBits::eVertexBuffer);
	std::memcpy(mesh.vertexResource.dataPtr, mesh.data.points.data(), mesh.data.points.size() * sizeof(mesh.data.points[0]));


	mesh.indexResource = graphicsUnit_.CreateBufferResource(mesh.data.triangles.size() * sizeof(mesh.data.triangles[0]), vk::BufferUsageFlagBits::eIndexBuffer);
	std::memcpy(mesh.indexResource.dataPtr, mesh.data.triangles.data(), mesh.data.triangles.size() * sizeof(mesh.data.triangles[0]));
			
	meshes_.push_back(&mesh);

	LOG_TRACE("Drawing mesh with {} triangles", mesh.data.triangles.size());
}

void VulkanRenderer2D::DestroyMesh(const pearl::typesRender::Mesh& mesh)
{
	WaitFinishRender();

	graphicsUnit_.GetLogical().unmapMemory(mesh.vertexResource.memory);
	graphicsUnit_.GetLogical().freeMemory(mesh.vertexResource.memory);
	graphicsUnit_.GetLogical().destroyBuffer(mesh.vertexResource.buffer);

	graphicsUnit_.GetLogical().unmapMemory(mesh.indexResource.memory);
	graphicsUnit_.GetLogical().freeMemory(mesh.indexResource.memory);
	graphicsUnit_.GetLogical().destroyBuffer(mesh.indexResource.buffer);
}


void VulkanRenderer2D::BuildCommandBufferCommands(const uint32_t index)
{
	pearl::CommandBuffer currentBuffer = commandBuffers_[index];

	currentBuffer.Get().reset();

	currentBuffer.Begin();
	currentBuffer.BeginRenderPass(renderPass_, *swapchain_.GetFramebuffers()[index], scissor_);

	currentBuffer.BindPipeline(graphicsPipeline_);


	// TODO -> Create wrapper class for descriptor sets, and make this into a CommandBuffer function to not use vulkan.
	currentBuffer.Get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipelineLayout_.Get(), 0, descriptorSets_[index], {});

	// TODO -> Can add scissor and viewport to renderSurface class and allow commandBuffers to access these with a SetRenderSurface method.
	currentBuffer.SetScissor(scissor_);
	currentBuffer.SetViewport(viewport_);

	for (auto& mesh : meshes_)
	{
		currentBuffer.PushConstants(graphicsPipelineLayout_, pearl::typesRender::PushConstantInfo{vk::ShaderStageFlagBits::eVertex, mesh->mvp});

		currentBuffer.DrawIndexed(*mesh);

	}

	currentBuffer.EndRenderPass();
	currentBuffer.End();
}


bool VulkanRenderer2D::Render()
{
	vk::Result result = graphicsUnit_.GetLogical().waitForFences(swapchain_.GetFences()[currentRenderIndex_], VK_TRUE, UINT64_MAX);
	graphicsUnit_.GetLogical().resetFences({ swapchain_.GetFences()[currentRenderIndex_] });

	const uint32_t nextImageIndex = swapchain_.GetNextImageIndex(currentRenderIndex_);

	BuildCommandBufferCommands(currentRenderIndex_);

	std::vector<vk::SubmitInfo> submitInfos = {};

	for (pearl::typesRender::Mesh* mesh : meshes_)
	{
		glm::mat4 mvp = camera_.GetPerspective() * camera_.GetView();
		
		mesh->modelMatrix = glm::translate(glm::mat4(1.0f), mesh->position);

		mesh->mvp.mvp = mvp * mesh->modelMatrix;
	}

	vk::PipelineStageFlags pipeStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	submitInfos.push_back(vk::SubmitInfo()
						  .setWaitDstStageMask(pipeStageFlags)
						  .setWaitSemaphoreCount(1)
						  .setPWaitSemaphores(&swapchain_.GetImageAcquiredSemaphores()[currentRenderIndex_])
						  .setCommandBufferCount(1)
						  .setPCommandBuffers(&commandBuffers_[currentRenderIndex_].Get())
						  .setSignalSemaphoreCount(1)
						  .setPSignalSemaphores(&swapchain_.GetDrawCompletedSemaphores()[currentRenderIndex_]));

	graphicsUnit_.GetGraphicsQueue().submit(submitInfos, swapchain_.GetFences()[currentRenderIndex_]);

	const vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&swapchain_.GetDrawCompletedSemaphores()[currentRenderIndex_])
		.setSwapchains(swapchain_.Get())
		.setImageIndices(nextImageIndex);

	result = graphicsUnit_.GetGraphicsQueue().presentKHR(&presentInfo);

	currentRenderIndex_ = (currentRenderIndex_ + 1) % 3;

	if ( result != vk::Result::eSuccess )
	{
		OnResize();
	}

	return true;
}


void VulkanRenderer2D::OnResize() {
	swapchain_.Recreate();
	currentRenderIndex_ = 0;
	camera_.SetViewArea({ swapchain_.GetSize().width, swapchain_.GetSize().height });
	camera_.UpdatePerspective();

	SetupRenderArea();
}


void VulkanRenderer2D::SetupRenderArea() {
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


bool VulkanRenderer2D::Update()
{
	Render();
	return true;
}

void VulkanRenderer2D::WaitFinishRender() const
{
	graphicsUnit_.GetLogical().waitIdle();
}
