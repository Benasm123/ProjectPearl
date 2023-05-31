#pragma once
#include "GraphicsUnit.h"
#include "PearlCore.h"


namespace PEARL_NAMESPACE
{
	class Image
	{
	public:
		Image(const GraphicsUnit& graphicsUnit, vk::Image image, vk::Format format, vk::Extent2D size, bool destroy=true);
		~Image();

		[[nodiscard]] vk::Image Get() const { return image_; }
		[[nodiscard]] vk::Format Format() const { return format_; }
		[[nodiscard]] vk::Extent2D Size() const { return size_; }
	private:
		const GraphicsUnit& graphicsUnit_;

		vk::Image image_;
		vk::Format format_;
		vk::Extent2D size_;

		bool destroy_;
	};
}

