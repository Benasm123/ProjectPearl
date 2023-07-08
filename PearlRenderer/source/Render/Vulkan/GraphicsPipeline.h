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

		[[nodiscard]] const vk::Pipeline& Get() const { return pipeline_; }

	private:
		void CreateGraphicsPipeline();

	private:
		vk::Pipeline pipeline_;

		const GraphicsUnit& graphicsUnit_;
		const RenderSurface& renderSurface_;
		const RenderPass& renderPass_;
		const PipelineLayout& pipelineLayout_;

		vk::Viewport viewport_;
		vk::Rect2D scissor_;


	};
}

