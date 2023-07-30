#include "Queue.h"
#include "GraphicsUnit.h"
#include "CommandBuffer.h"
#include "Swapchain.h"
#include "Fence.h"
#include "Semaphore.h"
#include "BDVK/BDVK_internal.h"


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

void PEARL_NAMESPACE::Queue::Submit(bdvk::PipelineStage stage, const CommandBuffer& commandBuffer, const std::vector<Semaphore*>& waitSemaphores, const std::vector<Semaphore*>& signalSemaphores, Fence* fence)
{
	std::vector<vk::Semaphore> waitSems{};
	for (const Semaphore* sem : waitSemaphores) {
		waitSems.push_back(sem->_internalSemaphore_);
	}

	std::vector<vk::Semaphore> signalSems{};
	for (const Semaphore* sem : signalSemaphores) {
		signalSems.push_back(sem->_internalSemaphore_);
	}

	std::vector<vk::SubmitInfo> submitInfos = {};

	vk::PipelineStageFlags vkStage = (vk::PipelineStageFlagBits)stage;

	submitInfos.push_back(
		vk::SubmitInfo()
		.setWaitDstStageMask(vkStage)
		.setWaitSemaphoreCount((uint32_t)waitSems.size())
		.setPWaitSemaphores(waitSems.data())
		.setCommandBufferCount(1)
		.setPCommandBuffers(&commandBuffer.commandBuffer_)
		.setSignalSemaphoreCount((uint32_t)signalSems.size())
		.setPSignalSemaphores(signalSems.data())
	);

	NextQueue().submit(submitInfos, fence->_internalFence_);
}

bool PEARL_NAMESPACE::Queue::Present(const Swapchain& swapchain, uint32_t imageIndex, const std::vector<Semaphore*>& waitSemaphores)
{
	std::vector<vk::Semaphore> waitSems{};
	for (const Semaphore* sem : waitSemaphores) {
		waitSems.push_back(sem->_internalSemaphore_);
	}

	const vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
		.setWaitSemaphoreCount((uint32_t)waitSems.size())
		.setPWaitSemaphores(waitSems.data())
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
