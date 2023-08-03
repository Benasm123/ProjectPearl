#include "DescriptorSets.h"
#include "PipelineLayout.h"
#include "BDVK/BDVK_internal.h"

PEARL_NAMESPACE::DescriptorSets::DescriptorSets(const PipelineLayout& pipelineLayout)
{
	descriptorSets_ = pipelineLayout.AllocateDescriptorSet();
}

PEARL_NAMESPACE::DescriptorSets::~DescriptorSets()
{

}
