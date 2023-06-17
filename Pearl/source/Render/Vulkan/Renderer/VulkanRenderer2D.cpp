#include "VulkanRenderer2D.h"

#include <Core/Config.h>
#include "Core/Logger.h"


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
{
	commandBuffers_ = commandPool_.AllocateCommandBuffers(swapchain_.GetImageCount());
	for (int i = 0 ; i < swapchain_.GetImageCount(); i++)
	{
		descriptorSets_.push_back(graphicsPipelineLayout_.AllocateDescriptorSet(1)[0]);
	}

	currentRenderIndex_ = swapchain_.GetNextImageIndex(currentRenderIndex_);
}

VulkanRenderer2D::~VulkanRenderer2D() = default;


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


void VulkanRenderer2D::DrawMesh(pearl::typesRender::Mesh* mesh)
{
	// Make Graphics Unit functions for getting memory buffers and memory.
	const vk::BufferCreateInfo vertexBufferInfo = vk::BufferCreateInfo()
	                                              .setFlags({})
	                                              .setSize(mesh->data.points.size() * sizeof(mesh->data.points[0]))
	                                              .setSharingMode(vk::SharingMode::eExclusive)
	                                              .setUsage(vk::BufferUsageFlagBits::eVertexBuffer);

	mesh->vertexBuffer = graphicsUnit_.GetLogical().createBuffer(vertexBufferInfo);

	const vk::MemoryRequirements vertexRequirements = graphicsUnit_.GetLogical().getBufferMemoryRequirements(mesh->vertexBuffer);

	const vk::MemoryAllocateInfo vertexAllocateInfo = vk::MemoryAllocateInfo()
	                                                  .setAllocationSize(vertexRequirements.size)
	                                                  .setMemoryTypeIndex(2);

	mesh->vertexMemory = graphicsUnit_.GetLogical().allocateMemory(vertexAllocateInfo);
	graphicsUnit_.GetLogical().bindBufferMemory(mesh->vertexBuffer, mesh->vertexMemory, 0);
	mesh->vertexData = graphicsUnit_.GetLogical().mapMemory(mesh->vertexMemory, 0, vertexRequirements.size, {});
	std::memcpy(mesh->vertexData, mesh->data.points.data(), mesh->data.points.size());


	//


	const vk::BufferCreateInfo indexBufferInfo = vk::BufferCreateInfo()
		.setFlags({})
		.setSize(mesh->data.triangles.size() * sizeof(mesh->data.triangles[0]))
		.setSharingMode(vk::SharingMode::eExclusive)
		.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);

	mesh->indexBuffer = graphicsUnit_.GetLogical().createBuffer(indexBufferInfo);

	const vk::MemoryRequirements indexRequirements = graphicsUnit_.GetLogical().getBufferMemoryRequirements(mesh->indexBuffer);

	const vk::MemoryAllocateInfo indexAllocateInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(indexRequirements.size)
		.setMemoryTypeIndex(2);

	mesh->indexMemory = graphicsUnit_.GetLogical().allocateMemory(indexAllocateInfo);
	graphicsUnit_.GetLogical().bindBufferMemory(mesh->indexBuffer, mesh->indexMemory, 0);
	mesh->indexData = graphicsUnit_.GetLogical().mapMemory(mesh->indexMemory, 0, indexRequirements.size, {});
	std::memcpy(mesh->indexData, mesh->data.triangles.data(), mesh->data.triangles.size());

	vertexCount_ = mesh->data.points.size();
	meshes_.push_back(mesh);

	for (int i = 0 ; i < swapchain_.GetImageCount(); i++)
	{
		BuildCommandBufferCommands(i);
	}
}


void VulkanRenderer2D::BuildCommandBufferCommands(const uint32_t index)
{
	constexpr vk::ClearValue clearValues[1] = {
		vk::ClearColorValue{std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})}
	};

	commandBuffers_[index].Get().begin(vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse));

	commandBuffers_[index].Get().beginRenderPass(vk::RenderPassBeginInfo()
												 .setRenderPass(renderPass_.Get())
												 .setFramebuffer(swapchain_.GetFramebuffers()[index]->Get())
												 .setRenderArea(vk::Rect2D(vk::Offset2D{}, vk::Extent2D(1400, 1400)))
												 .setClearValueCount(1)
												 .setPClearValues(clearValues),
												 vk::SubpassContents::eInline);

	commandBuffers_[index].Get().bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline_.Get());

	commandBuffers_[index].Get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipelineLayout_.Get(), 0, descriptorSets_[index], {});

	vk::DeviceSize offset[] = {0};
	commandBuffers_[index].Get().bindVertexBuffers(0, 1, &meshes_[0]->vertexBuffer, offset);

	commandBuffers_[index].Get().draw(vertexCount_ * 3, 1, 0, 0);

	commandBuffers_[index].Get().endRenderPass();

	commandBuffers_[index].Get().end();
}


bool VulkanRenderer2D::Render()
{
	vk::Result result = graphicsUnit_.GetLogical().waitForFences(swapchain_.GetFences()[currentRenderIndex_], VK_TRUE, UINT64_MAX);
	graphicsUnit_.GetLogical().resetFences({ swapchain_.GetFences()[currentRenderIndex_] });

	vk::PipelineStageFlags pipeStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	graphicsUnit_.GetGraphicsQueue().submit(
		vk::SubmitInfo()
		.setWaitDstStageMask(pipeStageFlags)
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&swapchain_.GetImageAcquiredSemaphores()[currentRenderIndex_])
		.setCommandBuffers(commandBuffers_[currentRenderIndex_].Get())
		.setSignalSemaphoreCount(1)
		.setPSignalSemaphores(&swapchain_.GetDrawCompletedSemaphores()[currentRenderIndex_])
		, swapchain_.GetFences()[currentRenderIndex_]);

	const auto presentInfo = vk::PresentInfoKHR()
		.setWaitSemaphoreCount(1)
		.setPWaitSemaphores(&swapchain_.GetDrawCompletedSemaphores()[currentRenderIndex_])
		.setSwapchains(swapchain_.Get())
		.setImageIndices(currentRenderIndex_);
	
	result = graphicsUnit_.GetGraphicsQueue().presentKHR(&presentInfo);

	currentRenderIndex_ = swapchain_.GetNextImageIndex(currentRenderIndex_);

	return true;
}

bool VulkanRenderer2D::Update()
{
	Render();
	return true;
}