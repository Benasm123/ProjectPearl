#include "PipelineLayout.h"
#include "PearlCore.h"

using namespace PEARL_NAMESPACE;

PipelineLayout::PipelineLayout(const GraphicsUnit& graphicsUnit)
	: graphicsUnit_{graphicsUnit}
{
	CreateDescriptorSetLayout();
	CreatePipelineLayout();
}


PipelineLayout::~PipelineLayout()
{
	for (const vk::DescriptorSetLayout layout : descriptorSetLayouts_)
	{
		graphicsUnit_.GetLogical().destroyDescriptorSetLayout(layout);
	}
	graphicsUnit_.GetLogical().destroyPipelineLayout(pipelineLayout_);
}


void PipelineLayout::CreateDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding binding = vk::DescriptorSetLayoutBinding()
		.setBinding(0)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(1)
		.setPImmutableSamplers(nullptr)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	const std::vector<vk::DescriptorSetLayoutBinding> bindings;

	const vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
		.setFlags({})
		.setBindingCount(static_cast<uint32_t>(bindings.size()))
		.setPBindings(bindings.data());

	descriptorSetLayouts_.push_back(graphicsUnit_.GetLogical().createDescriptorSetLayout(descriptorSetLayoutInfo));
}


void PipelineLayout::CreatePipelineLayout()
{
	const vk::PipelineLayoutCreateInfo pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
	                                                        .setFlags({})
	                                                        .setSetLayoutCount(static_cast<uint32_t>(descriptorSetLayouts_.size()))
	                                                        .setPSetLayouts(descriptorSetLayouts_.data())
	                                                        .setPushConstantRangeCount(0)
	                                                        .setPPushConstantRanges(nullptr);

	pipelineLayout_ = graphicsUnit_.GetLogical().createPipelineLayout(pipelineLayoutInfo);
}
