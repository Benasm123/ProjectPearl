#include "RenderPass.h"

#include "GraphicsUnit.h"

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

	const vk::AttachmentDescription colourAttachment = vk::AttachmentDescription()
	                                             .setFlags({})
	                                             .setFormat(graphicsUnit_.GetBestSurfaceFormat(renderSurface).format)
	                                             .setLoadOp(vk::AttachmentLoadOp::eClear)
	                                             .setStoreOp(vk::AttachmentStoreOp::eStore)
	                                             .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
	                                             .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
	                                             .setInitialLayout(vk::ImageLayout::eUndefined)
	                                             .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
	                                             .setSamples(vk::SampleCountFlagBits::e1); // TODO-> SAMPLE COUNT NEEDS TO BE SET SOMEWHERE AND PASSED EVERYWHERE ITS NEEDED TO KEEP THE SAME!

	attachmentDescriptions.push_back(colourAttachment);

	const std::vector<vk::SubpassDependency> dependencies;

	std::vector<vk::SubpassDescription> subpassDescriptions;

	constexpr vk::AttachmentReference colourReference = vk::AttachmentReference()
	                                                .setAttachment(0)
	                                                .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	constexpr vk::SubpassDescription subpass = vk::SubpassDescription()
	                                       .setFlags({})
	                                       .setColorAttachmentCount(1)
	                                       .setPColorAttachments(&colourReference)
	                                       .setInputAttachmentCount(0)
	                                       .setPInputAttachments(nullptr)
	                                       .setPDepthStencilAttachment(nullptr)
	                                       .setPPreserveAttachments(nullptr)
	                                       .setPResolveAttachments(nullptr)
	                                       .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

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
