#pragma once
#include "CommandBuffer.h"
#include "GraphicsUnit.h"
#include "PearlCore.h"

namespace PEARL_NAMESPACE
{
	class CommandPool
	{
	public:
		CommandPool(const GraphicsUnit& graphicsUnit, uint32_t queueIndex); // TODO -> Graphics Queue index here is redundant as the GraphicsUnit holds this information!
		~CommandPool();

		[[nodiscard]] std::vector<CommandBuffer> AllocateCommandBuffers(uint32_t number, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) const;

		[[nodiscard]] const vk::CommandPool& Get() const { return commandPool_; }
	private:
		const GraphicsUnit& graphicsUnit_;

		vk::CommandPool commandPool_;
	};
}

