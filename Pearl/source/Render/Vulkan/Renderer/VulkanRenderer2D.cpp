#include "VulkanRenderer2D.h"

#include <Core/Config.h>
#include "Core/Logger.h"


/**
 * \brief A 2D vulkan renderer allowing rendering to a window.
 * \param window The window to render to.
 */
VulkanRenderer2D::VulkanRenderer2D(const pearl::Window& window)
	: window_{window}
	, instance_{window_}
	, graphicsUnit_{instance_, instance_.FindBestGraphicsUnit()}
	, renderSurface_{instance_, window_}
	, renderPass_{graphicsUnit_, renderSurface_}
	, swapchain_{ graphicsUnit_, renderPass_, renderSurface_ }
{
}

VulkanRenderer2D::~VulkanRenderer2D() = default;


void VulkanRenderer2D::DrawLine(Types2D::Point2D start, Types2D::Point2D end)
{
}

void VulkanRenderer2D::DrawLines(std::vector<Types2D::Point2D> points)
{
}

void VulkanRenderer2D::DrawRect(Types2D::Rect2D rect)
{
}

void VulkanRenderer2D::DrawRects(std::vector<Types2D::Rect2D> rects)
{
}

bool VulkanRenderer2D::Render()
{
	return true;
}

bool VulkanRenderer2D::Update()
{
	Render();
	return true;
}