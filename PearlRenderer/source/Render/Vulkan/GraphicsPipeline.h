#pragma once
#include "GraphicsUnit.h"
#include "PearlCore.h"
#include "PipelineLayout.h"
#include "RenderPass.h"


class ShaderDescriptor;

namespace PEARL_NAMESPACE
{
	struct GraphicsPipelineInfo
	{
		vk::PrimitiveTopology primitive_type;
		vk::PolygonMode polygon_mode;
		vk::CullModeFlagBits cull_mode;
		vk::Bool32 depth_enabled;
		const std::vector<ShaderDescriptor>& shader_infos;
	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const GraphicsUnit& graphicsUnit, const RenderSurface& renderSurface, const RenderPass& renderPass, const PipelineLayout& pipelineLayout, const GraphicsPipelineInfo& info);
		~GraphicsPipeline();

	private:
		void CreateGraphicsPipeline(const GraphicsPipelineInfo& info);

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

