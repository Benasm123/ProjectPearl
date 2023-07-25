#include "CommandBuffer.h"
#include "FrameBuffer.h"
#include "GraphicsPipeline.h"

using namespace PEARL_NAMESPACE;


CommandBuffer::CommandBuffer(const vk::CommandBuffer commandBuffer)
	:commandBuffer_{commandBuffer}
{
}


void CommandBuffer::Begin() const
{
	constexpr vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
	                                             .setFlags({})
	                                             .setPInheritanceInfo({});

	commandBuffer_.begin(beginInfo);
}


void CommandBuffer::End() const
{
	commandBuffer_.end();
}

void PEARL_NAMESPACE::CommandBuffer::BeginRenderPass(const RenderPass& renderPass, const FrameBuffer& framebuffer, vk::Rect2D renderArea) const
{
	// TODO -> Make these variables with methods to adjust if user wants different clear values.
	const std::array<vk::ClearValue, 3> clearValues = {
		vk::ClearColorValue{std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})},
		vk::ClearDepthStencilValue{1.0f, 0},
		vk::ClearColorValue{std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})}
	};

	// TODO -> Convert .Get() methods to just private access through friends, to remove this access to vulkan.
	commandBuffer_.beginRenderPass(
		vk::RenderPassBeginInfo()
		.setRenderPass(renderPass.Get())
		.setFramebuffer(framebuffer.Get())
		.setRenderArea(renderArea)
		.setClearValueCount((uint32_t)clearValues.size())
		.setPClearValues(clearValues.data()),
		vk::SubpassContents::eInline);
}

void PEARL_NAMESPACE::CommandBuffer::EndRenderPass() const
{
	commandBuffer_.endRenderPass();
}

void PEARL_NAMESPACE::CommandBuffer::BindPipeline(const GraphicsPipeline& pipeline)
{
	commandBuffer_.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.Get());
}

void PEARL_NAMESPACE::CommandBuffer::SetScissor(const vk::Rect2D& scissor)
{
	commandBuffer_.setScissor(0, scissor);
}

void PEARL_NAMESPACE::CommandBuffer::SetViewport(const vk::Viewport& viewport)
{
	commandBuffer_.setViewport(0, viewport);
}
