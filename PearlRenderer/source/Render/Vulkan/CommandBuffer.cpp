#include "CommandBuffer.h"

using namespace PEARL_NAMESPACE;


CommandBuffer::CommandBuffer(const vk::CommandBuffer commandBuffer)
	:commandBuffer_{commandBuffer}
{
}


void CommandBuffer::Begin() const
{
	constexpr vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
	                                             .setFlags({})
	                                             .setPInheritanceInfo({});

	commandBuffer_.begin(beginInfo);
}


void CommandBuffer::End() const
{
	commandBuffer_.end();
}
