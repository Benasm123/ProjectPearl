#pragma once
#include <cstdint>

namespace Config::renderer
{
	extern bool useValidation;
	extern bool useSeparateComputeQueue;

	extern uint32_t numberOfGraphicsQueuesToUse;
	extern uint32_t numberOfComputeQueuesToUse; // Ignored if useSeparateComputeQueue is false.
	extern uint32_t numberOfSwapchainImagesToUse;
}
