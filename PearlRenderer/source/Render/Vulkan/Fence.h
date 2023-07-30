#pragma once
#include "PearlCore.h"

namespace PEARL_NAMESPACE
{
	class GraphicsUnit;

	class Fence
	{
	public:
		Fence(const GraphicsUnit& graphicsUnit);
		~Fence();

	private:
		const GraphicsUnit& owner_;

		class vk::Fence _internalFence_;

		friend class Queue;
		friend class GraphicsUnit;
	};
}

