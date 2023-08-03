#pragma once
#include "PearlCore.h"

namespace PEARL_NAMESPACE {
	class PipelineLayout;

	class DescriptorSets
	{
	public:
		DescriptorSets(const PipelineLayout& pipelineLayout);
		~DescriptorSets();

	private:
		std::vector<class vk::DescriptorSet> descriptorSets_;
		// Consider moving this to a map and having descriptors keys being the shader which it describes.
		// This allows shader to be specified on a mesh and the renderer can set up thee correct descriptor by using it as a key.

		friend class CommandBuffer;
	};
}

