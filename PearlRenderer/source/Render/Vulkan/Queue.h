#pragma once
#include "PearlCore.h"

namespace PEARL_NAMESPACE
{
	class GraphicsUnit;
	class Swapchain;
	class CommandBuffer;
	class Fence;
	class Semaphore;

	class Queue
	{
	public:
		Queue() = default;
		Queue(const GraphicsUnit& graphicsUnit, uint32_t familyIndex, uint32_t numberOfQueue=1);
		~Queue();

		void Submit(bdvk::PipelineStage stage, const CommandBuffer& commandBuffer, const std::vector<Semaphore*>& waitSemaphores, const std::vector<Semaphore*>& signalSemaphores, Fence* fence);
		bool Present(const Swapchain& swapchain, uint32_t imageIndex, const std::vector<Semaphore*>& waitSemaphores);

	private:
		class vk::Queue NextQueue();

	private:
		std::vector<class vk::Queue> queues_;

		uint32_t currentQueueIndex{};
	};
}

