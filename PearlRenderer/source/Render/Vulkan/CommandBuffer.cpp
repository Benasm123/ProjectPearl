#include "CommandBuffer.h"
#include "FrameBuffer.h"
#include "GraphicsPipeline.h"
#include "DescriptorSets.h"
#include "CommandPool.h"
#include "BDVK/BDVK_internal.h"

using namespace PEARL_NAMESPACE;

CommandBuffer::CommandBuffer(const CommandPool& commandPool)
{
	// TODO -> This needs changing, currently can only allocate 1 command pool per call. This should be converted to a container of command buffers not a single one.

	const vk::CommandBufferAllocateInfo allocateInfo = vk::CommandBufferAllocateInfo()
		.setCommandPool(commandPool.commandPool_)
		.setLevel(vk::CommandBufferLevel::ePrimary)
		.setCommandBufferCount(1);

	commandBuffer_ = commandPool.graphicsUnit_.logicalUnit_.allocateCommandBuffers(allocateInfo)[0];
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

void PEARL_NAMESPACE::CommandBuffer::Reset() const
{
	commandBuffer_.reset();
}

void PEARL_NAMESPACE::CommandBuffer::BeginRenderPass(const RenderPass& renderPass, const FrameBuffer& framebuffer, const RenderSurface& renderSurface) const
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
		.setRenderPass(renderPass.renderPass_)
		.setFramebuffer(framebuffer.framebuffer_)
		.setRenderArea(renderSurface.scissor_)
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
	commandBuffer_.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline_);
}

void PEARL_NAMESPACE::CommandBuffer::SetRenderSurface(const RenderSurface& renderSurface) 
{
	commandBuffer_.setScissor(0, renderSurface.scissor_);
	commandBuffer_.setViewport(0, renderSurface.viewport_);
}

void PEARL_NAMESPACE::CommandBuffer::BindDescriptorSets(bdvk::PipelineBindPoint bindPoint, const PipelineLayout& pipelineLayout, const DescriptorSets& descriptorSet)
{
	commandBuffer_.bindDescriptorSets((vk::PipelineBindPoint)bindPoint, pipelineLayout.pipelineLayout_, 0, descriptorSet.descriptorSets_, {});
}

void PEARL_NAMESPACE::CommandBuffer::PushConstants(const PipelineLayout& pipelineLayout, const pearl::typesRender::PushConstantInfo& pushConstantInfo)
{
	commandBuffer_.pushConstants(pipelineLayout.pipelineLayout_, (vk::ShaderStageFlagBits)pushConstantInfo.shaderStage, 0, sizeof(pushConstantInfo.data), &pushConstantInfo.data);
}

void PEARL_NAMESPACE::CommandBuffer::DrawIndexed(const typesRender::Mesh& mesh)
{
	constexpr vk::DeviceSize offset[] = { 0 };
	commandBuffer_.bindVertexBuffers(0, 1, &mesh.vertexResource.buffer, offset);

	commandBuffer_.bindIndexBuffer(mesh.indexResource.buffer, 0, vk::IndexType::eUint32);

	commandBuffer_.drawIndexed(((uint32_t)mesh.data.triangles.size() * 3u), 1, 0, 0, 0);
}
