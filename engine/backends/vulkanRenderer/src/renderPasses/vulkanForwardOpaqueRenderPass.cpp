#include "vulkanForwardOpaqueRenderPass.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanMacros.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include <array>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ForwardOpaqueRenderPass::ForwardOpaqueRenderPass(const RenderTargetResources& renderTargets)
	{
		CreateRenderPass(renderTargets);
		CreateFrameBuffers(renderTargets);
		NAME_VK_OBJECT(m_renderPass, "RenderPass_ForwardOpaque");
	}
	ForwardOpaqueRenderPass::~ForwardOpaqueRenderPass()
	{

	}



	// Private methods:
	void ForwardOpaqueRenderPass::CreateRenderPass(const RenderTargetResources& renderTargets)
	{
		// Attachments:
		std::array<VkAttachmentDescription, 2> attachments{};
		{
			// Color attachment description:
			attachments[0].format = renderTargets.GetSceneColorTexturePair().GetRenderTargetTexture(0, 0).GetFormat();
			attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;							// load screen-space compute results.
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// store opaque scene color for forward transparent rendering.
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;				// we do not use stencils.
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;			// we do not use stencils.
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_GENERAL;						// layout used by screen-space compute.
			attachments[0].finalLayout = VK_IMAGE_LAYOUT_GENERAL;						// preserve layout for forward transparent rendering.

			// Depth attachment description:
			attachments[1].format = renderTargets.GetSceneDepthTexture(0).GetFormat();
			attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;									// due to deferred + forward rendering we handle msaa manually.
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;								// load depth of previous deferred geometry render pass.
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;							// store depth for forward transparent rendering and post processing.
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					// we do not use stencils.
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				// we do not use stencils.
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;	// final layout of deferred geometry render pass.
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;	// read-only layout required by forward transparent rendering.
		}

		// Attachment references:
		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentReference = {};
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Subpass:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;
		subpass.pDepthStencilAttachment = &depthAttachmentReference;

		// Synchronization dependencies of individual subpasses:
		VkSubpassDependency screenSpaceComputeToForwardOpaqueDependency = {};
		screenSpaceComputeToForwardOpaqueDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		screenSpaceComputeToForwardOpaqueDependency.dstSubpass = 0;
		screenSpaceComputeToForwardOpaqueDependency.srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		screenSpaceComputeToForwardOpaqueDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		screenSpaceComputeToForwardOpaqueDependency.srcAccessMask = AccessMasks::ComputeShader::shaderWrite | AccessMasks::LateFragmentTest::depthStencilAttachmentWrite;
		screenSpaceComputeToForwardOpaqueDependency.dstAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentRead | AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead | AccessMasks::EarlyFragmentTest::depthStencilAttachmentWrite;
		screenSpaceComputeToForwardOpaqueDependency.dependencyFlags = 0;
		VkSubpassDependency forwardOpaqueToForwardTransparentDependency = {};
		forwardOpaqueToForwardTransparentDependency.srcSubpass = 0;
		forwardOpaqueToForwardTransparentDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		forwardOpaqueToForwardTransparentDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		forwardOpaqueToForwardTransparentDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		forwardOpaqueToForwardTransparentDependency.srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::LateFragmentTest::depthStencilAttachmentWrite;
		forwardOpaqueToForwardTransparentDependency.dstAccessMask = AccessMasks::FragmentShader::shaderRead | AccessMasks::ColorAttachmentOutput::colorAttachmentRead | AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead;
		std::array<VkSubpassDependency, 2> dependencies = { screenSpaceComputeToForwardOpaqueDependency, forwardOpaqueToForwardTransparentDependency };

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void ForwardOpaqueRenderPass::CreateFrameBuffers(const RenderTargetResources& renderTargets)
	{
		m_framebuffers.resize(2 * renderTargets.GetFrameCount());
		for (size_t framebufferIndex = 0; framebufferIndex < m_framebuffers.size(); framebufferIndex++)
		{
			const uint32_t frameIndex = static_cast<uint32_t>(framebufferIndex / 2);
			const uint32_t sceneColorIndex = static_cast<uint32_t>(framebufferIndex % 2);
			const RenderTexture2d& renderTexture = renderTargets.GetSceneColorTexturePair().GetRenderTargetTexture(frameIndex, sceneColorIndex);
			std::array<VkImageView, 2> attachments =
			{
				renderTexture.GetVkImageView(),
				renderTargets.GetSceneDepthTexture(frameIndex).GetVkImageView()
			};

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = renderTexture.GetWidth();
			framebufferInfo.height = renderTexture.GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[framebufferIndex]));
			NAME_VK_OBJECT(m_framebuffers[framebufferIndex], "Framebuffer_ForwardOpaque_Frame" + std::to_string(frameIndex) + "_Color" + std::to_string(sceneColorIndex));
		}
	}
}