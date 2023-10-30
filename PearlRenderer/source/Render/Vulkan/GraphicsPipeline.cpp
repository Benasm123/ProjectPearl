#include "GraphicsPipeline.h"

#include "Render/Types/Types2D.h"
#include "BDVK/BDVK_internal.h"
#include "ShaderDescriptor.h"

using namespace PEARL_NAMESPACE;

GraphicsPipeline::GraphicsPipeline(const GraphicsUnit& graphicsUnit, const RenderSurface& renderSurface, const RenderPass& renderPass, const PipelineLayout& pipelineLayout, const GraphicsPipelineInfo& info)
	: graphicsUnit_{graphicsUnit}
	, renderSurface_{renderSurface}
	, renderPass_{renderPass}
	, pipelineLayout_{pipelineLayout}
{
	CreateGraphicsPipeline(info);
}


GraphicsPipeline::~GraphicsPipeline()
{
	graphicsUnit_.logicalUnit_.destroyPipeline(pipeline_);
}


void GraphicsPipeline::CreateGraphicsPipeline(const GraphicsPipelineInfo& info)
{

#pragma region ShaderLoading

	std::vector<vk::PipelineShaderStageCreateInfo> stages;
	std::vector<vk::ShaderModule> shader_modules;

	for (const ShaderDescriptor& shader_info : info.shader_infos)
	{
		std::vector<char> shader_code = utils::ReadShaderCode(shader_info.file_);

		vk::ShaderModuleCreateInfo shader_module_info = vk::ShaderModuleCreateInfo()
			.setCodeSize(shader_code.size())
			.setPCode(reinterpret_cast<uint32_t*>(shader_code.data()));

		const vk::ShaderModule module = graphicsUnit_.logicalUnit_.createShaderModule(shader_module_info);

		vk::ShaderStageFlagBits stage;

		switch (shader_info.stage_)
		{
		case kVertex:
			stage = vk::ShaderStageFlagBits::eVertex;
			break;
		case kFragment:
			stage = vk::ShaderStageFlagBits::eFragment;
			break;
		case kCompute:
			stage = vk::ShaderStageFlagBits::eCompute;
			break;
		case kGeometry:
			stage = vk::ShaderStageFlagBits::eGeometry;
			break;
		case kTessellation:
			stage = vk::ShaderStageFlagBits::eTessellationControl;
			break;
		default:
			LOG_ERROR("Unhandled shader stage case! Pipeline creation can not complete.");
			assert(true);
		}

		const vk::PipelineShaderStageCreateInfo shader_stage_info = vk::PipelineShaderStageCreateInfo()
			.setStage(stage)
			.setModule(module)
			.setPName("main");

		// Making a copy each time. This can be converted to pointer holding, or handles (module i assume is already one so guess this wouldn't have too much impact).
		stages.push_back(shader_stage_info);
		shader_modules.push_back(module);
	}

#pragma endregion ShaderLoading

#pragma region VertexInputState

	vk::VertexInputBindingDescription vertexBindingInfo = vk::VertexInputBindingDescription()
		.setBinding(0)
		.setStride(sizeof(PEARL_NAMESPACE::typesRender::VertexInput2D))
		.setInputRate(vk::VertexInputRate::eVertex);

	vk::VertexInputBindingDescription vertexBindingInfo2 = vk::VertexInputBindingDescription()
		.setBinding(1)
		.setStride(sizeof(glm::vec2))
		.setInputRate(vk::VertexInputRate::eVertex);

	std::vector<vk::VertexInputBindingDescription> vertexBindings = { vertexBindingInfo, vertexBindingInfo2 };

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
		.setBinding(1)
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
		.setTopology(info.primitive_type)
		.setPrimitiveRestartEnable(VK_FALSE);

#pragma endregion VertexInputAssembly

#pragma region TessellationState

	vk::PipelineTessellationStateCreateInfo tessellation_info = vk::PipelineTessellationStateCreateInfo()
		.setFlags({})
		.setPatchControlPoints(0);

#pragma endregion TessellationState

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
		.setPolygonMode(info.polygon_mode)
		.setCullMode(info.cull_mode)
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
		.setRasterizationSamples(vk::SampleCountFlagBits::e8)
		.setSampleShadingEnable(VK_FALSE)
		.setMinSampleShading(1.0f)
		.setPSampleMask(nullptr)
		.setAlphaToCoverageEnable(VK_FALSE)
		.setAlphaToOneEnable(VK_FALSE);

#pragma endregion MultisampleState

#pragma region DepthStencilState

	vk::PipelineDepthStencilStateCreateInfo depthStencilInfo = vk::PipelineDepthStencilStateCreateInfo()
		.setFlags({})
		.setDepthTestEnable(info.depth_enabled)
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

	std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eScissor, vk::DynamicState::eViewport};

	vk::PipelineDynamicStateCreateInfo dynamicInfo = vk::PipelineDynamicStateCreateInfo()
		.setFlags({})
		.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()))
		.setPDynamicStates(dynamicStates.data());

#pragma endregion DynamicStates

	vk::GraphicsPipelineCreateInfo graphicsPipelineInfo = vk::GraphicsPipelineCreateInfo()
		.setFlags({})
		.setLayout(pipelineLayout_.pipelineLayout_)
		.setStageCount(static_cast<uint32_t>(stages.size()))
		.setPStages(stages.data())
		.setPVertexInputState(&vertexInputInfo)
		.setPInputAssemblyState(&inputAssemblyInfo)
		.setPTessellationState(&tessellation_info)
		.setPViewportState(&viewportInfo)
		.setPRasterizationState(&rasterizationInfo)
		.setPMultisampleState(&multisampleInfo)
		.setPDepthStencilState(&depthStencilInfo)
		.setPColorBlendState(&colourBlendInfo)
		.setPDynamicState(&dynamicInfo)
		.setRenderPass(renderPass_.renderPass_)
		.setSubpass(0)
		.setBasePipelineHandle(VK_NULL_HANDLE)
		.setBasePipelineIndex(0);

	constexpr vk::PipelineCacheCreateInfo pipelineCacheInfo;
	auto pipelineCache = graphicsUnit_.logicalUnit_.createPipelineCache(pipelineCacheInfo, nullptr);
	pipeline_ = graphicsUnit_.logicalUnit_.createGraphicsPipeline(pipelineCache, graphicsPipelineInfo).value;

	for (vk::ShaderModule module : shader_modules)
	{
		graphicsUnit_.logicalUnit_.destroyShaderModule(module);
	}

	graphicsUnit_.logicalUnit_.destroyPipelineCache(pipelineCache);
}