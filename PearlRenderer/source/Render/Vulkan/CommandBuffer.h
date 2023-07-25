#pragma once
#include "PearlCore.h"

struct vk::Rect2D;

namespace PEARL_NAMESPACE
{
	class RenderPass;
	class FrameBuffer;
	class GraphicsPipeline;

	class CommandBuffer
	{
	public:
		CommandBuffer(vk::CommandBuffer commandBuffer);
		~CommandBuffer() = default;

		void Begin() const;
		void End() const;

		void BeginRenderPass(const RenderPass& renderPass, const FrameBuffer& framebuffer, vk::Rect2D renderArea) const;
		void EndRenderPass() const;

		void BindPipeline(const GraphicsPipeline& pipeline);

		void SetScissor(const vk::Rect2D& scissor);
		void SetViewport(const vk::Viewport& viewport);

		[[nodiscard]] const vk::CommandBuffer& Get() const { return commandBuffer_; }

	private:
		vk::CommandBuffer commandBuffer_;
	};
}

