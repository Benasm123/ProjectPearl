#pragma once
#include "PearlCore.h"

namespace PEARL_NAMESPACE {
	class GraphicsUnit;

	class Semaphore
	{
	public:
		Semaphore(const GraphicsUnit& graphicsUnit);
		~Semaphore();

	private:
		const GraphicsUnit& owner_;

		class vk::Semaphore _internalSemaphore_;

		friend class Swapchain;
		friend class Queue;
	};
}

