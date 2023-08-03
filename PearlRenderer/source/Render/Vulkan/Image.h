#pragma once
#include "GraphicsUnit.h"
#include "PearlCore.h"


namespace PEARL_NAMESPACE
{
	class Image
	{
	public:
		Image(const GraphicsUnit& graphicsUnit, vk::Image image, vk::Format format, vk::Extent2D size, bool destroy=true);
		Image(const GraphicsUnit& graphicsUnit, vk::Format format, vk::Extent2D size, vk::ImageLayout initialLayout, vk::ImageUsageFlags usage, bool destroy=true);
		~Image();

		glm::vec2 Size() const { return { size_.width, size_.height }; }

	private:
		const GraphicsUnit& graphicsUnit_;

		class vk::Image image_;
		enum vk::Format format_;
		struct vk::Extent2D size_;

		bool destroy_;

		friend class FrameBuffer;
	};
}

