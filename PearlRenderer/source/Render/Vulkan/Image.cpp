#include "Image.h"
#include "BDVK/BDVK_internal.h"

using namespace PEARL_NAMESPACE;


Image::Image(const GraphicsUnit& graphicsUnit, const vk::Image image, const vk::Format format, const vk::Extent2D size, const bool destroy)
	: graphicsUnit_{graphicsUnit}
	, image_{image}
	, format_{format}
	, size_{size}
	, destroy_{destroy}
{
}

Image::Image(const GraphicsUnit& graphicsUnit, const vk::Format format, const vk::Extent2D size, const vk::ImageLayout initialLayout, const vk::ImageUsageFlags usage, const bool destroy)
	: graphicsUnit_{ graphicsUnit }
	, format_{ format }
	, size_{ size }
	, destroy_{ destroy }
{
	const vk::ImageCreateInfo imageInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setExtent(VkExtent3D{ size.width, size.height, 1 })
		.setMipLevels(1)
		.setArrayLayers(1)
		.setFormat(format_)
		.setTiling(vk::ImageTiling::eOptimal)
		.setInitialLayout(initialLayout)
		.setUsage(usage)
		.setSamples(vk::SampleCountFlagBits::e8);

	image_ = graphicsUnit_.logicalUnit_.createImage(imageInfo);
}


Image::~Image()
{
	if (destroy_)
	{
		graphicsUnit_.logicalUnit_.destroyImage(image_);
	}
}
