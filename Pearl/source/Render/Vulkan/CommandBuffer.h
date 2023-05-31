#pragma once
#include "PearlCore.h"


namespace PEARL_NAMESPACE
{
	class CommandBuffer
	{
	public:
		CommandBuffer(vk::CommandBuffer commandBuffer);
		~CommandBuffer() = default;

		void Begin() const;
		void End() const;

		[[nodiscard]] const vk::CommandBuffer& Get() const { return commandBuffer_; }

	private:
		vk::CommandBuffer commandBuffer_;
	};
}

