#pragma once
#include "PearlCore.h"

namespace PEARL_NAMESPACE
{
	class GraphicsUnit;
	class Swapchain;
	class CommandBuffer;

	class Queue
	{
	public:
		Queue() = default;
		Queue(const GraphicsUnit& graphicsUnit, uint32_t familyIndex, uint32_t numberOfQueue=1);
		~Queue();

		void Submit(bdvk::PipelineStage stage, const CommandBuffer& commandBuffer, const std::vector<vk::Semaphore>& waitSemaphores, const std::vector<vk::Semaphore>& signalSemaphores, vk::Fence fence);
		bool Present(const Swapchain& swapchain, uint32_t imageIndex, const std::vector<vk::Semaphore>& waitSemaphores);

	private:
		vk::Queue NextQueue();

	private:
		std::vector<vk::Queue> queues_;

		uint32_t currentQueueIndex{};
	};
}

