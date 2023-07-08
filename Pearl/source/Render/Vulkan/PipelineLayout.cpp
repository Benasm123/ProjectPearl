#include "PipelineLayout.h"
#include "PearlCore.h"
#include "Render/Types/TypesRender.h"

using namespace PEARL_NAMESPACE;

PipelineLayout::PipelineLayout(const GraphicsUnit& graphicsUnit)
	: graphicsUnit_{graphicsUnit}
{
	CreatePushConstantRanges();
	CreateDescriptorSetLayout();
	CreateDescriptorSetPool();
	CreatePipelineLayout();
}


PipelineLayout::~PipelineLayout()
{
	graphicsUnit_.GetLogical().destroyDescriptorPool(descriptorPool_);

	for (const vk::DescriptorSetLayout layout : descriptorSetLayouts_)
	{
		graphicsUnit_.GetLogical().destroyDescriptorSetLayout(layout);
	}

	graphicsUnit_.GetLogical().destroyPipelineLayout(pipelineLayout_);
}


void PipelineLayout::CreateDescriptorSetLayout()
{
	const vk::DescriptorSetLayoutBinding binding = vk::DescriptorSetLayoutBinding()
	                                               .setBinding(0)
	                                               .setDescriptorType(vk::DescriptorType::eUniformBuffer)
	                                               .setDescriptorCount(2)
	                                               .setPImmutableSamplers(nullptr)
	                                               .setStageFlags(vk::ShaderStageFlagBits::eVertex);

	const vk::DescriptorSetLayoutBinding tbinding = vk::DescriptorSetLayoutBinding()
		.setBinding(1)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(1)
		.setPImmutableSamplers(nullptr)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	const vk::DescriptorSetLayoutBinding nbinding = vk::DescriptorSetLayoutBinding()
		.setBinding(2)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(1)
		.setPImmutableSamplers(nullptr)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	// const vk::DescriptorSetLayoutBinding mvpBinding = vk::DescriptorSetLayoutBinding()
	// 	.setBinding(3)
	// 	.setDescriptorType(vk::DescriptorType::eUniformBuffer)
	// 	.setDescriptorCount(1)
	// 	.setPImmutableSamplers(nullptr)
	// 	.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	const std::vector<vk::DescriptorSetLayoutBinding> bindings = {binding, tbinding, nbinding};

	const vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
		.setFlags({})
		.setBindingCount(static_cast<uint32_t>(bindings.size()))
		.setPBindings(bindings.data());

	descriptorSetLayouts_.push_back(graphicsUnit_.GetLogical().createDescriptorSetLayout(descriptorSetLayoutInfo));
}


void PipelineLayout::CreateDescriptorSetPool()
{
	const vk::DescriptorPoolSize poolSize = vk::DescriptorPoolSize()
	                                        .setDescriptorCount(3)
	                                        .setType(vk::DescriptorType::eUniformBuffer);

	const std::vector<vk::DescriptorPoolSize> poolSizes = { poolSize };

	const vk::DescriptorPoolCreateInfo descriptorPoolInfo = vk::DescriptorPoolCreateInfo()
	                                                        .setFlags({})
	                                                        .setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()))
	                                                        .setPPoolSizes(poolSizes.data())
	                                                        .setMaxSets(10);

	descriptorPool_ = graphicsUnit_.GetLogical().createDescriptorPool(descriptorPoolInfo);
}


void PipelineLayout::CreatePushConstantRanges()
{
	vk::PushConstantRange pushConstantRange = vk::PushConstantRange()
		.setStageFlags(vk::ShaderStageFlagBits::eVertex)
		.setSize(sizeof(PEARL_NAMESPACE::typesRender::PushConstant))
		.setOffset(0);

	pushConstantRanges_ = { pushConstantRange };
}


void PipelineLayout::CreatePipelineLayout()
{
	const vk::PipelineLayoutCreateInfo pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
	                                                        .setFlags({})
	                                                        .setSetLayoutCount(static_cast<uint32_t>(descriptorSetLayouts_.size()))
	                                                        .setPSetLayouts(descriptorSetLayouts_.data())
	                                                        .setPushConstantRangeCount(static_cast<uint32_t>(pushConstantRanges_.size()))
	                                                        .setPPushConstantRanges(pushConstantRanges_.data());

	pipelineLayout_ = graphicsUnit_.GetLogical().createPipelineLayout(pipelineLayoutInfo);
}


std::vector<vk::DescriptorSet> PipelineLayout::AllocateDescriptorSet(const uint32_t count) const
{
	const vk::DescriptorSetAllocateInfo setAllocateInfo = vk::DescriptorSetAllocateInfo()
	                                                      .setDescriptorPool(descriptorPool_)
	                                                      .setPSetLayouts(descriptorSetLayouts_.data())
	                                                      .setDescriptorSetCount(count);

	return graphicsUnit_.GetLogical().allocateDescriptorSets(setAllocateInfo);
}
