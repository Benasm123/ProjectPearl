#include "CommandPool.h"

using namespace PEARL_NAMESPACE;


CommandPool::CommandPool(const GraphicsUnit& graphicsUnit, const uint32_t queueIndex)
	:graphicsUnit_{graphicsUnit}
{
	const vk::CommandPoolCreateInfo commandPoolInfo = vk::CommandPoolCreateInfo()
	                                                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
	                                                  .setQueueFamilyIndex(queueIndex);

	commandPool_ = graphicsUnit_.GetLogical().createCommandPool(commandPoolInfo);
}


CommandPool::~CommandPool()
{
	graphicsUnit_.GetLogical().destroyCommandPool(commandPool_);
}


std::vector<CommandBuffer> CommandPool::AllocateCommandBuffers(const uint32_t number, const vk::CommandBufferLevel level) const
{
	const vk::CommandBufferAllocateInfo allocateInfo = vk::CommandBufferAllocateInfo()
	                                                   .setCommandPool(commandPool_)
	                                                   .setLevel(level)
	                                                   .setCommandBufferCount(number);

	const std::vector<vk::CommandBuffer> vulkanCommandBuffers = graphicsUnit_.GetLogical().allocateCommandBuffers(allocateInfo);

	std::vector<CommandBuffer> commandBuffers;

	commandBuffers.reserve(vulkanCommandBuffers.size());
	for (const vk::CommandBuffer commandBuffer : vulkanCommandBuffers)
	{
		commandBuffers.emplace_back(commandBuffer);
	}

	return commandBuffers;
}
