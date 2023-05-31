#pragma once
#include "GraphicsUnit.h"
#include "PearlCore.h"


namespace PEARL_NAMESPACE
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const GraphicsUnit& graphicsUnit);
		~GraphicsPipeline();

		void CreateGraphicsPipeline();

		[[nodiscard]] const vk::Pipeline& Get() const { return pipeline_; }

	private:
		const GraphicsUnit& graphicsUnit_;

		vk::Pipeline pipeline_;

	};
}

