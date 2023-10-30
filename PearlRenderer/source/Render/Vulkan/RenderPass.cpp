#include "RenderPass.h"

#include "GraphicsUnit.h"
#include "BDVK/BDVK_internal.h"

using namespace PEARL_NAMESPACE;

RenderPass::RenderPass(const GraphicsUnit& graphicsUnit, const RenderSurface& renderSurface)
	:graphicsUnit_{graphicsUnit}
{
	CreateRenderPass(renderSurface);
}


RenderPass::~RenderPass()
{
	graphicsUnit_.DestroyRenderPass(renderPass_);
}


void RenderPass::CreateRenderPass(const RenderSurface& renderSurface)
{
	std::vector<vk::AttachmentDescription> attachmentDescriptions;

	const vk::AttachmentDescription multiSampleAttachment = vk::AttachmentDescription()
		.setFormat(graphicsUnit_.GetBestSurfaceFormat(renderSurface).format)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setSamples(vk::SampleCountFlagBits::e8);

	attachmentDescriptions.push_back(multiSampleAttachment);

	constexpr vk::AttachmentDescription depthAttachment = vk::AttachmentDescription()
		.setFlags({})
		.setFormat(vk::Format::eD32Sfloat) //Should get this from the depth image not hard code
		.setSamples(vk::SampleCountFlagBits::e8)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	attachmentDescriptions.push_back(depthAttachment);

	const vk::AttachmentDescription colourAttachment = vk::AttachmentDescription()
	                                             .setFlags({})
	                                             .setFormat(graphicsUnit_.GetBestSurfaceFormat(renderSurface).format)
	                                             .setLoadOp(vk::AttachmentLoadOp::eClear)
	                                             .setStoreOp(vk::AttachmentStoreOp::eStore)
	                                             .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
	                                             .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
	                                             .setInitialLayout(vk::ImageLayout::eUndefined)
	                                             .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
	                                             .setSamples(vk::SampleCountFlagBits::e1);

	attachmentDescriptions.push_back(colourAttachment);

	std::vector<vk::SubpassDependency> dependencies;

	const vk::SubpassDependency dependency = vk::SubpassDependency()
	                                         .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
	                                         .setSrcAccessMask({})
	                                         .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
	                                         .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
	                                         .setSrcSubpass(VK_SUBPASS_EXTERNAL)
	                                         .setDstSubpass(0);

	dependencies.push_back(dependency);

	constexpr vk::AttachmentReference multiSampleReference = vk::AttachmentReference()
		.setAttachment(0)
		.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	constexpr vk::AttachmentReference colourReference = vk::AttachmentReference()
	                                                .setAttachment(2)
	                                                .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	std::vector<vk::AttachmentReference> references = { multiSampleReference };

	constexpr vk::AttachmentReference depthReference = vk::AttachmentReference()
		.setAttachment(1)
		.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	const vk::SubpassDescription subpass = vk::SubpassDescription()
	                                       .setFlags({})
	                                       .setColorAttachmentCount(1)
	                                       .setPColorAttachments(references.data())
	                                       .setInputAttachmentCount(0)
	                                       .setPInputAttachments(nullptr)
	                                       .setPDepthStencilAttachment(&depthReference)
	                                       .setPPreserveAttachments(nullptr)
	                                       .setPResolveAttachments(&colourReference)
	                                       .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);


	std::vector<vk::SubpassDescription> subpassDescriptions;
	subpassDescriptions.push_back(subpass);

	const vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo()
	                                                .setFlags({})
	                                                .setAttachmentCount(static_cast<uint32_t>(attachmentDescriptions.size()))
	                                                .setPAttachments(attachmentDescriptions.data())
													.setDependencyCount(static_cast<uint32_t>(dependencies.size()))
													.setPDependencies(dependencies.data())
													.setSubpassCount(static_cast<uint32_t>(subpassDescriptions.size()))
													.setPSubpasses(subpassDescriptions.data());

	renderPass_ = graphicsUnit_.CreateRenderPass(renderPassInfo);
}
