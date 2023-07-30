#pragma once
#include "CommandBuffer.h"
#include "GraphicsUnit.h"
#include "PearlCore.h"

namespace PEARL_NAMESPACE
{
	class CommandPool
	{
	public:
		CommandPool(const GraphicsUnit& graphicsUnit, uint32_t queueIndex);
		~CommandPool();

	private:
		const GraphicsUnit& graphicsUnit_;

		class vk::CommandPool commandPool_;

		friend class CommandBuffer;
	};
}

