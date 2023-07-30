#pragma once
#include "PearlCore.h"

struct vk::Rect2D;

namespace PEARL_NAMESPACE
{
	class RenderPass;
	class FrameBuffer;
	class GraphicsPipeline;
	class PipelineLayout;
	class DescriptorSets;
	class RenderSurface;
	class CommandPool;

	class CommandBuffer
	{
	public:
		CommandBuffer(const CommandPool& commandPool);
		~CommandBuffer() = default;

		void Begin() const;
		void End() const;
		void Reset() const;

		void BeginRenderPass(const RenderPass& renderPass, const FrameBuffer& framebuffer, const RenderSurface& renderSurface) const;
		void EndRenderPass() const;

		void BindPipeline(const GraphicsPipeline& pipeline);

		void SetRenderSurface(const RenderSurface& renderSurface);

		void BindDescriptorSets(bdvk::PipelineBindPoint bindPoint, const PipelineLayout& pipelineLayout, const DescriptorSets& descriptorSet);

		void PushConstants(const PipelineLayout& pipelineLayout, const pearl::typesRender::PushConstantInfo& pushConstantInfo);

		void DrawIndexed(const typesRender::Mesh& mesh);

	private:
		class vk::CommandBuffer commandBuffer_;

		friend class Queue;
	};
}

