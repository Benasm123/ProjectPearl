#include "VulkanRenderer2D.h"

#include <iostream>
#include <Core/Config.h>
#include "Core/Logger.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>


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
	commandBuffers_[index].Get().reset();

	const std::array<vk::ClearValue, 3> clearValues = {
		vk::ClearColorValue{std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})},
		vk::ClearDepthStencilValue{1.0f, 0},
		vk::ClearColorValue{std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})}
	};

	commandBuffers_[index].Get().begin(vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse));

	commandBuffers_[index].Get().beginRenderPass(vk::RenderPassBeginInfo()
												 .setRenderPass(renderPass_.Get())
												 .setFramebuffer(swapchain_.GetFramebuffers()[index]->Get())
												 .setRenderArea(vk::Rect2D(vk::Offset2D{0, 0}, swapchain_.GetSize()))
												 .setClearValueCount((uint32_t)clearValues.size())
												 .setPClearValues(clearValues.data()),
												 vk::SubpassContents::eInline);

	commandBuffers_[index].Get().bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline_.Get());

	commandBuffers_[index].Get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipelineLayout_.Get(), 0, descriptorSets_[index], {});

	const vk::Extent2D renderExtent = graphicsUnit_.GetSurfaceCapabilities(renderSurface_).currentExtent;

	viewport_.setWidth(static_cast<float>(renderExtent.width))
		.setHeight(static_cast<float>(renderExtent.height))
		.setX(0)
		.setY(0)
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f);

	const std::vector<vk::Viewport> viewports = { viewport_ };

	scissor_.setExtent(renderExtent)
		.setOffset({ 0, 0 });

	const std::vector<vk::Rect2D> scissors = { scissor_ };

	commandBuffers_[index].Get().setScissor(0, scissors);
	commandBuffers_[index].Get().setViewport(0, viewports);

	for (auto& mesh : meshes_)
	{
		commandBuffers_[index].Get().pushConstants(graphicsPipelineLayout_.Get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(pearl::typesRender::PushConstant), &mesh->mvp);

		constexpr vk::DeviceSize offset[] = { 0 };
		commandBuffers_[index].Get().bindVertexBuffers(0, 1, &mesh->vertexResource.buffer, offset);

		commandBuffers_[index].Get().bindIndexBuffer(mesh->indexResource.buffer, 0, vk::IndexType::eUint32);

		commandBuffers_[index].Get().drawIndexed((uint32_t)(mesh->data.triangles.size() * 3ull), 1, 0, 0, 0);

	}

	commandBuffers_[index].Get().endRenderPass();

	commandBuffers_[index].Get().end();
}


bool VulkanRenderer2D::Render()
{
	vk::Result result = graphicsUnit_.GetLogical().waitForFences(swapchain_.GetFences()[currentRenderIndex_], VK_TRUE, UINT64_MAX);
	graphicsUnit_.GetLogical().resetFences({ swapchain_.GetFences()[currentRenderIndex_] });

	const uint32_t nextImageIndex = swapchain_.GetNextImageIndex(currentRenderIndex_);

	BuildCommandBufferCommands(currentRenderIndex_);

	std::vector<vk::SubmitInfo> submitInfos = {};

	for (int i = 0; i < meshes_.size(); i++)
	{
		// glm::mat4 mvp = projectionMatrix_ * viewMatrix_;
		glm::mat4 mvp = camera_.GetPerspective() * camera_.GetView();
		
		meshes_[i]->modelMatrix = glm::translate(glm::mat4(1.0f), meshes_[i]->position);

		meshes_[i]->mvp.mvp = mvp * meshes_[i]->modelMatrix;
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

	const auto presentInfo = vk::PresentInfoKHR()
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&swapchain_.GetDrawCompletedSemaphores()[currentRenderIndex_])
		.setSwapchains(swapchain_.Get())
		.setImageIndices(nextImageIndex);

	result = graphicsUnit_.GetGraphicsQueue().presentKHR(&presentInfo);

	currentRenderIndex_ = (currentRenderIndex_ + 1) % 3;

	if ( result != vk::Result::eSuccess )
	{
		swapchain_.Recreate();
		currentRenderIndex_ = 0;
		camera_.SetViewArea({ swapchain_.GetSize().width, swapchain_.GetSize().height });
		camera_.UpdatePerspective();
	}

	return true;
}


void VulkanRenderer2D::Build()
{
	for ( uint32_t i = 0; i < swapchain_.GetImageCount(); i++ )
	{
		BuildCommandBufferCommands(i);
	}
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
