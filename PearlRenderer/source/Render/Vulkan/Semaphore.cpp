#include "Semaphore.h"
#include "GraphicsUnit.h"
#include "BDVK/BDVK_internal.h"

PEARL_NAMESPACE::Semaphore::Semaphore(const GraphicsUnit& graphicsUnit)
	:owner_{ graphicsUnit }
{
	constexpr vk::SemaphoreCreateInfo semaphoreInfo = vk::SemaphoreCreateInfo();
	_internalSemaphore_ = owner_.logicalUnit_.createSemaphore(semaphoreInfo);
}

PEARL_NAMESPACE::Semaphore::~Semaphore()
{
	owner_.logicalUnit_.destroySemaphore(_internalSemaphore_);
}
