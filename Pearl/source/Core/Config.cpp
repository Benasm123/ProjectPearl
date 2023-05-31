#include "Config.h"

namespace Config::renderer
{
	bool useValidation = true;
	bool useSeparateComputeQueue = true;

	uint32_t numberOfGraphicsQueuesToUse = 16;
	uint32_t numberOfComputeQueuesToUse = 8;
	uint32_t numberOfSwapchainImagesToUse = 3;
}
