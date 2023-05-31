#include "Image.h"

using namespace PEARL_NAMESPACE;


Image::Image(const GraphicsUnit& graphicsUnit, const vk::Image image, const vk::Format format, const vk::Extent2D size, const bool destroy)
	: graphicsUnit_{graphicsUnit}
	, image_{image}
	, format_{format}
	, size_{size}
	, destroy_{destroy}
{
}


Image::~Image()
{
	if (destroy_)
	{
		graphicsUnit_.GetLogical().destroyImage(image_);
	}
}
