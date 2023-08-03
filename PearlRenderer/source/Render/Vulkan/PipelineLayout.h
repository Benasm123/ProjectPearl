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

		std::vector<vk::DescriptorSet> AllocateDescriptorSet() const;

	private:
		void CreateDescriptorSetLayout();
		void CreateDescriptorSetPool();
		void CreatePushConstantRanges();
		void CreatePipelineLayout();

	private:
		const GraphicsUnit& graphicsUnit_;

		class vk::PipelineLayout pipelineLayout_;
		std::vector<class vk::DescriptorSetLayout> descriptorSetLayouts_;
		std::vector<struct vk::PushConstantRange> pushConstantRanges_;
		class vk::DescriptorPool descriptorPool_;

		friend class CommandBuffer;
		friend class GraphicsPipeline;
	};
}

