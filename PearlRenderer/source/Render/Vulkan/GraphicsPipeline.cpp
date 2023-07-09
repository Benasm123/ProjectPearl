#include "GraphicsPipeline.h"

#include "Render/Types/Types2D.h"

using namespace PEARL_NAMESPACE;

GraphicsPipeline::GraphicsPipeline(const GraphicsUnit& graphicsUnit, const RenderSurface& renderSurface, const RenderPass& renderPass, const PipelineLayout& pipelineLayout)
	: graphicsUnit_{graphicsUnit}
	, renderSurface_{renderSurface}
	, renderPass_{renderPass}
	, pipelineLayout_{pipelineLayout}
{
	CreateGraphicsPipeline();
}


GraphicsPipeline::~GraphicsPipeline()
{
	graphicsUnit_.GetLogical().destroyPipeline(pipeline_);
}


void GraphicsPipeline::CreateGraphicsPipeline()
{

#pragma region ShaderLoading
	auto vertexShaderCode = utils::ReadShaderCode("Basic.vert.spv");

	const vk::ShaderModuleCreateInfo vertexModuleInfo = vk::ShaderModuleCreateInfo()
	                                                    .setFlags({})
	                                                    .setCodeSize(vertexShaderCode.size())
	                                                    .setPCode(reinterpret_cast<uint32_t*>(vertexShaderCode.data()));

	const vk::ShaderModule vertexModule = graphicsUnit_.GetLogical().createShaderModule(vertexModuleInfo);

	const vk::PipelineShaderStageCreateInfo vertexStageInfo = vk::PipelineShaderStageCreateInfo()
	                                                          .setFlags({})
	                                                          .setStage(vk::ShaderStageFlagBits::eVertex)
	                                                          .setModule(vertexModule)
	                                                          .setPName("main");

	auto fragmentShaderCode = utils::ReadShaderCode("Basic.frag.spv");

	const vk::ShaderModuleCreateInfo fragmentModuleInfo = vk::ShaderModuleCreateInfo()
		.setFlags({})
		.setCodeSize(fragmentShaderCode.size())
		.setPCode(reinterpret_cast<uint32_t*>(fragmentShaderCode.data()));

	const vk::ShaderModule fragmentModule = graphicsUnit_.GetLogical().createShaderModule(fragmentModuleInfo);

	const vk::PipelineShaderStageCreateInfo fragmentStageInfo = vk::PipelineShaderStageCreateInfo()
		.setFlags({})
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(fragmentModule)
		.setPName("main");

	const std::vector<vk::PipelineShaderStageCreateInfo> stages = { vertexStageInfo, fragmentStageInfo };
	std::vector<vk::ShaderModule> shaderModules = { vertexModule, fragmentModule };
#pragma endregion ShaderLoading

#pragma region VertexInputState

	vk::VertexInputBindingDescription vertexBindingInfo = vk::VertexInputBindingDescription()
		.setBinding(0)
		.setStride(sizeof(PEARL_NAMESPACE::typesRender::VertexInput2D))
		.setInputRate(vk::VertexInputRate::eVertex);

	std::vector<vk::VertexInputBindingDescription> vertexBindings = { vertexBindingInfo };

	vk::VertexInputAttributeDescription vertexPositionInfo = vk::VertexInputAttributeDescription()
		.setLocation(0)
		.setBinding(0)
		.setFormat(vk::Format::eR32G32B32Sfloat)
		.setOffset(0);

	vk::VertexInputAttributeDescription vertexNormalInfo = vk::VertexInputAttributeDescription()
		.setLocation(1)
		.setBinding(0)
		.setFormat(vk::Format::eR32G32B32Sfloat)
		.setOffset(offsetof(PEARL_NAMESPACE::typesRender::VertexInput2D, normal));

	vk::VertexInputAttributeDescription vertexTextureInfo = vk::VertexInputAttributeDescription()
		.setLocation(2)
		.setBinding(0)
		.setFormat(vk::Format::eR32G32Sfloat)
		.setOffset(offsetof(PEARL_NAMESPACE::typesRender::VertexInput2D, textureCoordinate));

	std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes = { vertexPositionInfo, vertexNormalInfo, vertexTextureInfo };

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
		.setFlags({})
		.setVertexBindingDescriptionCount(static_cast<uint32_t>(vertexBindings.size()))
		.setPVertexBindingDescriptions(vertexBindings.data())
		.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexInputAttributes.size()))
		.setPVertexAttributeDescriptions(vertexInputAttributes.data());

#pragma endregion VertexInputState

#pragma region VertexInputAssembly

	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo()
		.setFlags({})
		.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(VK_FALSE);

#pragma endregion VertexInputAssembly

#pragma region TesselationState

	vk::PipelineTessellationStateCreateInfo tessellationInfo = vk::PipelineTessellationStateCreateInfo()
		.setFlags({})
		.setPatchControlPoints(graphicsUnit_.GetPhysical().getProperties().limits.maxTessellationPatchSize);

#pragma endregion TesselationState

#pragma region ViewportState

	vk::Extent2D renderExtent = graphicsUnit_.GetSurfaceCapabilities(renderSurface_).currentExtent;

	viewport_.setWidth(static_cast<float>(renderExtent.width))
		.setHeight(static_cast<float>(renderExtent.height))
		.setX(0)
		.setY(0)
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f);

	std::vector<vk::Viewport> viewports = { viewport_ };

	scissor_.setExtent(renderExtent)
		.setOffset({ 0, 0 });

	std::vector<vk::Rect2D> scissors = { scissor_ };

	vk::PipelineViewportStateCreateInfo viewportInfo = vk::PipelineViewportStateCreateInfo()
		.setFlags({})
		.setViewportCount(static_cast<uint32_t>(viewports.size()))
		.setPViewports(viewports.data())
		.setScissorCount(static_cast<uint32_t>(scissors.size()))
		.setPScissors(scissors.data());

#pragma endregion ViewportState

#pragma region RasterizationState

	vk::PipelineRasterizationStateCreateInfo rasterizationInfo = vk::PipelineRasterizationStateCreateInfo()
		.setFlags({})
		.setDepthClampEnable(VK_FALSE)
		.setRasterizerDiscardEnable(VK_FALSE)
		.setPolygonMode(vk::PolygonMode::eFill)
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setDepthBiasEnable(VK_FALSE)
		.setDepthBiasConstantFactor(0.0f)
		.setDepthBiasClamp(0.0f)
		.setDepthBiasSlopeFactor(0.0f)
		.setLineWidth(1.0f);

#pragma endregion RasterizationState

#pragma region MultisampleState

	vk::PipelineMultisampleStateCreateInfo multisampleInfo = vk::PipelineMultisampleStateCreateInfo()
		.setFlags({})
		.setRasterizationSamples(vk::SampleCountFlagBits::e1)
		.setSampleShadingEnable(VK_FALSE)
		.setMinSampleShading(1.0f)
		.setPSampleMask(nullptr)
		.setAlphaToCoverageEnable(VK_FALSE)
		.setAlphaToOneEnable(VK_FALSE);

#pragma endregion MultisampleState

#pragma region DepthStencilState

	vk::PipelineDepthStencilStateCreateInfo depthStencilInfo = vk::PipelineDepthStencilStateCreateInfo()
		.setFlags({})
		.setDepthTestEnable(VK_TRUE)
		.setDepthWriteEnable(VK_TRUE)
		.setDepthCompareOp(vk::CompareOp::eLess)
		.setDepthBoundsTestEnable(VK_FALSE)
		.setStencilTestEnable(VK_FALSE)
		.setFront(vk::StencilOp::eKeep)
		.setBack(vk::StencilOp::eKeep)
		.setMinDepthBounds(0.0f)
		.setMaxDepthBounds(1.0f);

#pragma endregion DepthStencilState

#pragma region ColourBlendState

	vk::PipelineColorBlendAttachmentState colourAttachmentState = vk::PipelineColorBlendAttachmentState()
		.setBlendEnable(VK_FALSE)
		.setSrcColorBlendFactor(vk::BlendFactor::eSrcColor)
		.setDstColorBlendFactor(vk::BlendFactor::eOneMinusDstColor)
		.setColorBlendOp(vk::BlendOp::eAdd)
		.setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha)
		.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusDstAlpha)
		.setAlphaBlendOp(vk::BlendOp::eAdd)
		.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	std::vector<vk::PipelineColorBlendAttachmentState> colourAttachments = { colourAttachmentState };

	vk::PipelineColorBlendStateCreateInfo colourBlendInfo = vk::PipelineColorBlendStateCreateInfo()
		.setFlags({})
		.setLogicOpEnable(VK_FALSE)
		.setLogicOp(vk::LogicOp::eAnd)
		.setAttachmentCount(static_cast<uint32_t>(colourAttachments.size()))
		.setPAttachments(colourAttachments.data())
		.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

#pragma endregion ColourBlendState

#pragma region DynamicStates

	std::vector<vk::DynamicState> dynamicStates = {};

	vk::PipelineDynamicStateCreateInfo dynamicInfo = vk::PipelineDynamicStateCreateInfo()
		.setFlags({})
		.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()))
		.setPDynamicStates(dynamicStates.data());

#pragma endregion DynamicStates

	vk::GraphicsPipelineCreateInfo graphicsPipelineInfo = vk::GraphicsPipelineCreateInfo()
		.setFlags({})
		.setLayout(pipelineLayout_.Get())
		.setStageCount(static_cast<uint32_t>(stages.size()))
		.setPStages(stages.data())
		.setPVertexInputState(&vertexInputInfo)
		.setPInputAssemblyState(&inputAssemblyInfo)
		.setPTessellationState(&tessellationInfo)
		.setPViewportState(&viewportInfo)
		.setPRasterizationState(&rasterizationInfo)
		.setPMultisampleState(&multisampleInfo)
		.setPDepthStencilState(&depthStencilInfo)
		.setPColorBlendState(&colourBlendInfo)
		.setPDynamicState(&dynamicInfo)
		.setLayout(pipelineLayout_.Get())
		.setRenderPass(renderPass_.Get())
		.setSubpass(0)
		.setBasePipelineHandle(VK_NULL_HANDLE)
		.setBasePipelineIndex(0);

	constexpr vk::PipelineCacheCreateInfo pipelineCacheInfo;
	auto pipelineCache = graphicsUnit_.GetLogical().createPipelineCache(pipelineCacheInfo, nullptr);
	pipeline_ = graphicsUnit_.GetLogical().createGraphicsPipeline(pipelineCache, graphicsPipelineInfo).value;

	for (vk::ShaderModule module : shaderModules)
	{
		graphicsUnit_.GetLogical().destroyShaderModule(module);
	}

	graphicsUnit_.GetLogical().destroyPipelineCache(pipelineCache);
}