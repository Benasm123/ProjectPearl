#include "CommandPool.h"
#include "BDVK/BDVK_internal.h"

using namespace PEARL_NAMESPACE;


CommandPool::CommandPool(const GraphicsUnit& graphicsUnit, const uint32_t queueIndex)
	:graphicsUnit_{graphicsUnit}
{
	const vk::CommandPoolCreateInfo commandPoolInfo = vk::CommandPoolCreateInfo()
	                                                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
	                                                  .setQueueFamilyIndex(queueIndex);

	commandPool_ = graphicsUnit_.logicalUnit_.createCommandPool(commandPoolInfo);
}


CommandPool::~CommandPool()
{
	graphicsUnit_.logicalUnit_.destroyCommandPool(commandPool_);
}
