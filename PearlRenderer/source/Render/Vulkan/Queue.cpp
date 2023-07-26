#include "Queue.h"
#include "GraphicsUnit.h"
#include "CommandBuffer.h"
#include "Swapchain.h"


PEARL_NAMESPACE::Queue::Queue(const GraphicsUnit& graphicsUnit, uint32_t familyIndex, uint32_t numberOfQueue)
{
	for (uint32_t i = 0; i < numberOfQueue; i++) 
	{
		queues_.push_back(graphicsUnit.logicalUnit_.getQueue(familyIndex, i));
	}
}

PEARL_NAMESPACE::Queue::~Queue()
{
}

void PEARL_NAMESPACE::Queue::Submit(bdvk::PipelineStage stage, const CommandBuffer& commandBuffer, const std::vector<vk::Semaphore>& waitSemaphores, const std::vector<vk::Semaphore>& signalSemaphores, vk::Fence fence)
{
	std::vector<vk::SubmitInfo> submitInfos = {};

	vk::PipelineStageFlags vkStage = (vk::PipelineStageFlagBits)stage;

	submitInfos.push_back(
		vk::SubmitInfo()
		.setWaitDstStageMask(vkStage)
		.setWaitSemaphoreCount((uint32_t)waitSemaphores.size())
		.setPWaitSemaphores(waitSemaphores.data())
		.setCommandBufferCount(1)
		.setPCommandBuffers(&commandBuffer.commandBuffer_)
		.setSignalSemaphoreCount((uint32_t)signalSemaphores.size())
		.setPSignalSemaphores(signalSemaphores.data())
	);

	NextQueue().submit(submitInfos, fence);
}

bool PEARL_NAMESPACE::Queue::Present(const Swapchain& swapchain, uint32_t imageIndex, const std::vector<vk::Semaphore>& waitSemaphores)
{
	const vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
		.setWaitSemaphoreCount((uint32_t)waitSemaphores.size())
		.setPWaitSemaphores(waitSemaphores.data())
		.setSwapchains(swapchain.swapchain_)
		.setImageIndices(imageIndex);

	vk::Result result = NextQueue().presentKHR(&presentInfo);

	return result == vk::Result::eSuccess;
}

vk::Queue PEARL_NAMESPACE::Queue::NextQueue()
{
	vk::Queue queue = queues_[currentQueueIndex];

	currentQueueIndex++;
	currentQueueIndex %= queues_.size();

	return queue;
}
