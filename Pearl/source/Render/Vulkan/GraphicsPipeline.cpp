#include "GraphicsPipeline.h"
using namespace PEARL_NAMESPACE;


GraphicsPipeline::GraphicsPipeline(const GraphicsUnit& graphicsUnit)
	: graphicsUnit_{graphicsUnit}
{
	CreateGraphicsPipeline();
}


GraphicsPipeline::~GraphicsPipeline()
{
	graphicsUnit_.GetLogical().destroyPipeline(pipeline_);
}


void GraphicsPipeline::CreateGraphicsPipeline()
{

}
