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

		[[nodiscard]] const vk::RenderPass& Get() const { return renderPass_; }

	private:
		void CreateRenderPass(const RenderSurface& renderSurface);

	private:
		vk::RenderPass renderPass_{};

		const GraphicsUnit& graphicsUnit_;
	};
}

