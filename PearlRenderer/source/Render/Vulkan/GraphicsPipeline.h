#pragma once
#include "GraphicsUnit.h"
#include "PearlCore.h"
#include "PipelineLayout.h"
#include "RenderPass.h"


namespace PEARL_NAMESPACE
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const GraphicsUnit& graphicsUnit, const RenderSurface& renderSurface, const RenderPass& renderPass, const PipelineLayout& pipelineLayout);
		~GraphicsPipeline();

	private:
		void CreateGraphicsPipeline();

	private:
		class vk::Pipeline pipeline_;

		const GraphicsUnit& graphicsUnit_;
		const RenderSurface& renderSurface_;
		const RenderPass& renderPass_;
		const PipelineLayout& pipelineLayout_;

		struct vk::Viewport viewport_;
		struct vk::Rect2D scissor_;

		friend class CommandBuffer;
	};
}

