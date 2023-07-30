#pragma once
#include "GraphicsUnit.h"
#include "PearlCore.h"

namespace PEARL_NAMESPACE
{
	class RenderPass
	{
	public:
		RenderPass(const GraphicsUnit& graphicsUnit, const RenderSurface& renderSurface);
		~RenderPass();

	private:
		void CreateRenderPass(const RenderSurface& renderSurface);

	private:
		class vk::RenderPass renderPass_{};

		const GraphicsUnit& graphicsUnit_;

		friend class CommandBuffer;
		friend class FrameBuffer;
		friend class GraphicsPipeline;
		friend class RenderPass;
	};
}

