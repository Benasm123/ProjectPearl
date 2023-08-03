#include "Fence.h"
#include "GraphicsUnit.h"
#include "BDVK/BDVK_internal.h"

PEARL_NAMESPACE::Fence::Fence(const GraphicsUnit& graphicsUnit)
	: owner_(graphicsUnit)
{
	constexpr vk::FenceCreateInfo fenceInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
	_internalFence_ = owner_.logicalUnit_.createFence(fenceInfo);
}

PEARL_NAMESPACE::Fence::~Fence()
{
	owner_.logicalUnit_.destroyFence(_internalFence_);
}
