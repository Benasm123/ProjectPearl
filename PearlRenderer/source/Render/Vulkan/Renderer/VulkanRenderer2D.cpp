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
	// projectionMatrix_ = glm::perspective(glm::radians(45.0f), {static_cast<float>(swapchain_.GetSize().width) / static_cast<float>(swapchain_.GetSize().height)}, 0.1f, 1000.0f);
	// projectionMatrix_[1][1] *= -1;
	//
	// viewMatrix_ = glm::lookAt(cameraPosition_, origin_, up_);

	// const glm::mat4 mvp = projectionMatrix_ * viewMatrix_;
	const glm::mat4 mvp = camera_.GetPerspective() * camera_.GetView();

	auto descriptorBufferInfo = vk::DescriptorBufferInfo().setOffset(0).setRange(sizeof(glm::mat4));
	
	constexpr auto bufferInfo = vk::BufferCreateInfo().setSize(sizeof(glm::mat4)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
	
	vk::WriteDescriptorSet write = vk::WriteDescriptorSet()
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setPBufferInfo(&descriptorBufferInfo);

	commandBuffers_ = commandPool_.AllocateCommandBuffers(swapchain_.GetImageCount());
	for (uint32_t i = 0 ; i < swapchain_.GetImageCount(); i++)
	{
		descriptorSets_.push_back(graphicsPipelineLayout_.AllocateDescriptorSet(1)[0]);

		uniformBuffers_.push_back(graphicsUnit_.GetLogical().createBuffer(bufferInfo, nullptr));

		const vk::MemoryRequirements memReq = graphicsUnit_.GetLogical().getBufferMemoryRequirements(uniformBuffers_[i]);
		
		vk::MemoryAllocateInfo allocateInfo = vk::MemoryAllocateInfo().setAllocationSize(memReq.size).setMemoryTypeIndex(2);
		
		uniformMemories_.push_back(graphicsUnit_.GetLogical().allocateMemory(allocateInfo));
		
		uniformMemoryPtrs_.push_back(graphicsUnit_.GetLogical().mapMemory(uniformMemories_[i], 0, VK_WHOLE_SIZE, vk::MemoryMapFlags()));

		memcpy(uniformMemoryPtrs_[i], &mvp, sizeof(glm::mat4));

		graphicsUnit_.GetLogical().bindBufferMemory(uniformBuffers_[i], uniformMemories_[i], 0);

		descriptorBufferInfo.setBuffer(uniformBuffers_[i]);
		write.setDstSet(descriptorSets_[i]);
		graphicsUnit_.GetLogical().updateDescriptorSets(write, {});
	}
}

VulkanRenderer2D::~VulkanRenderer2D()
{
	WaitFinishRender();

	for (const auto& memory : uniformMemories_)
	{
		graphicsUnit_.GetLogical().unmapMemory(memory);
		graphicsUnit_.GetLogical().freeMemory(memory);
	}

	for (const auto& buffer : uniformBuffers_)
	{
		graphicsUnit_.GetLogical().destroyBuffer(buffer);
	}
};


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


void VulkanRenderer2D::DrawMesh(pearl::typesRender::Mesh& mesh)
{
	// Make Graphics Unit functions for getting memory buffers and memory.
	const vk::BufferCreateInfo vertexBufferInfo = vk::BufferCreateInfo()
	                                              .setFlags({})
	                                              .setSize(mesh.data.points.size() * sizeof(pearl::typesRender::VertexInput2D))
	                                              .setSharingMode(vk::SharingMode::eExclusive)
	                                              .setUsage(vk::BufferUsageFlagBits::eVertexBuffer);

	mesh.vertexBuffer = graphicsUnit_.GetLogical().createBuffer(vertexBufferInfo);

	const vk::MemoryRequirements vertexRequirements = graphicsUnit_.GetLogical().getBufferMemoryRequirements(mesh.vertexBuffer);

	const vk::MemoryAllocateInfo vertexAllocateInfo = vk::MemoryAllocateInfo()
	                                                  .setAllocationSize(vertexRequirements.size)
	                                                  .setMemoryTypeIndex(2);

	mesh.vertexMemory = graphicsUnit_.GetLogical().allocateMemory(vertexAllocateInfo);
	graphicsUnit_.GetLogical().bindBufferMemory(mesh.vertexBuffer, mesh.vertexMemory, 0);
	mesh.vertexData = graphicsUnit_.GetLogical().mapMemory(mesh.vertexMemory, 0, vertexRequirements.size, {});
	std::memcpy(mesh.vertexData, mesh.data.points.data(), mesh.data.points.size() * sizeof(mesh.data.points[0]));


	//


	const vk::BufferCreateInfo indexBufferInfo = vk::BufferCreateInfo()
		.setFlags({})
		.setSize(mesh.data.triangles.size() * sizeof(pearl::typesRender::Triangle))
		.setSharingMode(vk::SharingMode::eExclusive)
		.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);

	mesh.indexBuffer = graphicsUnit_.GetLogical().createBuffer(indexBufferInfo);

	const vk::MemoryRequirements indexRequirements = graphicsUnit_.GetLogical().getBufferMemoryRequirements(mesh.indexBuffer);

	const vk::MemoryAllocateInfo indexAllocateInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(indexRequirements.size)
		.setMemoryTypeIndex(2);

	mesh.indexMemory = graphicsUnit_.GetLogical().allocateMemory(indexAllocateInfo);
	graphicsUnit_.GetLogical().bindBufferMemory(mesh.indexBuffer, mesh.indexMemory, 0);
	mesh.indexData = graphicsUnit_.GetLogical().mapMemory(mesh.indexMemory, 0, indexRequirements.size, {});
	std::memcpy(mesh.indexData, mesh.data.triangles.data(), mesh.data.triangles.size() * sizeof(mesh.data.triangles[0]));
	
	meshes_.push_back(&mesh);

	LOG_TRACE("Drawing mesh with {} triangles", mesh.data.triangles.size());
}

void VulkanRenderer2D::DestroyMesh(const pearl::typesRender::Mesh& mesh)
{
	WaitFinishRender();

	graphicsUnit_.GetLogical().unmapMemory(mesh.vertexMemory);
	graphicsUnit_.GetLogical().freeMemory(mesh.vertexMemory);
	graphicsUnit_.GetLogical().destroyBuffer(mesh.vertexBuffer);

	graphicsUnit_.GetLogical().unmapMemory(mesh.indexMemory);
	graphicsUnit_.GetLogical().freeMemory(mesh.indexMemory);
	graphicsUnit_.GetLogical().destroyBuffer(mesh.indexBuffer);
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

	vk::Extent2D renderExtent = graphicsUnit_.GetSurfaceCapabilities(renderSurface_).currentExtent;

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

	for (int i = 0; i < meshes_.size(); i++)
	{
		commandBuffers_[index].Get().pushConstants(graphicsPipelineLayout_.Get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(pearl::typesRender::PushConstant), &meshes_[i]->mvp);

		constexpr vk::DeviceSize offset[] = { 0 };
		commandBuffers_[index].Get().bindVertexBuffers(0, 1, &meshes_[i]->vertexBuffer, offset);

		commandBuffers_[index].Get().bindIndexBuffer(meshes_[i]->indexBuffer, 0, vk::IndexType::eUint32);

		commandBuffers_[index].Get().drawIndexed((uint32_t)(meshes_[i]->data.triangles.size() * 3ull), 1, 0, 0, 0);

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
