#pragma once
#include "GraphicsUnit.h"
#include "PearlCore.h"

namespace PEARL_NAMESPACE
{
	class PipelineLayout
	{
	public:
		PipelineLayout(const GraphicsUnit& graphicsUnit);
		~PipelineLayout();

		[[nodiscard]] const vk::PipelineLayout& Get() const { return pipelineLayout_; }

	private:
		void CreateDescriptorSetLayout();
		void CreatePipelineLayout();

	private:
		const GraphicsUnit& graphicsUnit_;

		vk::PipelineLayout pipelineLayout_;
		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts_;
		vk::DescriptorPool descriptorPool_;
	};
}

