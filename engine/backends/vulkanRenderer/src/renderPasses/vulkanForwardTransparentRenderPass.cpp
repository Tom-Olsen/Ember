#include "vulkanForwardTransparentRenderPass.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanMacros.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include <array>
#include <stdexcept>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ForwardTransparentRenderPass::ForwardTransparentRenderPass(const RenderTargetResources& renderTargets)
	{
		m_frameCount = renderTargets.GetFrameCount();
		CreateRenderPass(renderTargets);
		CreateFrameBuffers(renderTargets);
		NAME_VK_OBJECT(m_renderPass, "RenderPass_ForwardTransparent");
	}
	ForwardTransparentRenderPass::~ForwardTransparentRenderPass()
	{

	}



	// Getters:
	const VkFramebuffer& ForwardTransparentRenderPass::GetFramebuffer(uint32_t frameIndex, uint32_t sceneColorIndex) const
	{
		if (frameIndex >= m_frameCount || sceneColorIndex >= 2)
			throw std::out_of_range("ForwardTransparentRenderPass: frame or scene color texture index out of range.");
		uint32_t framebufferIndex = 2 * frameIndex + sceneColorIndex;
		return m_framebuffers[framebufferIndex];
	}



	// Private methods:
	void ForwardTransparentRenderPass::CreateRenderPass(const RenderTargetResources& renderTargets)
	{
		// Attachments:
		std::array<VkAttachmentDescription, 2> attachments{};
		{
			// Color attachment description:
			attachments[0].format = renderTargets.GetSceneColorTexturePair().GetRenderTargetTexture(0, 0).GetFormat();
			attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;						// load results after screen space compute.
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;					// store final image for post processing + presenting.
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;			// we do not use stencils.
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		// we do not use stencils.
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_GENERAL;					// layout used by screen space compute shaders.
			attachments[0].finalLayout = VK_IMAGE_LAYOUT_GENERAL;					// layout for post processing compute shaders.

			// Depth attachment description (transparent materials only read scene depth):
			attachments[1].format = renderTargets.GetSceneDepthTexture(0).GetFormat();
			attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;									// due to deferred + forward rendering we handle msaa manually.
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;								// load depth produced by deferred geometry and forward opaque rendering.
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;							// store depth for further post processing if desired.
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					// we do not use stencils.
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				// we do not use stencils.
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;	// final layout of forward opaque render pass.
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;	// preserve read-only depth for later shader access.
		}

		// Attachment references:
		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentReference = {};
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		// Subpass:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;
		subpass.pDepthStencilAttachment = &depthAttachmentReference;

		// Synchronization dependencies of individual subpasses:
		VkSubpassDependency screenSpaceComputeToForwardTransparentDependency = {};
		screenSpaceComputeToForwardTransparentDependency.srcSubpass = VK_SUBPASS_EXTERNAL; // index of source subpass, where dependency originates. VK_SUBPASS_EXTERNAL = before renderpass.
		screenSpaceComputeToForwardTransparentDependency.dstSubpass = 0;                   // index of destination subpass, where dependency ends. VK_SUBPASS_EXTERNAL = after renderpass.
		screenSpaceComputeToForwardTransparentDependency.srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; // screen space compute and forward opaque producer stages.
		screenSpaceComputeToForwardTransparentDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; // forward transparent shader, color, and depth consumer stages.
		screenSpaceComputeToForwardTransparentDependency.srcAccessMask = AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite | AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::LateFragmentTest::depthStencilAttachmentWrite; // complete compute reads/writes and preserve opaque attachment writes.
		screenSpaceComputeToForwardTransparentDependency.dstAccessMask = AccessMasks::FragmentShader::shaderRead | AccessMasks::ColorAttachmentOutput::colorAttachmentRead | AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead; // forward transparent shader reads, color loads/writes, and depth tests.
		screenSpaceComputeToForwardTransparentDependency.dependencyFlags = 0; 			// screen space compute may access arbitrary pixels.

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &screenSpaceComputeToForwardTransparentDependency;

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void ForwardTransparentRenderPass::CreateFrameBuffers(const RenderTargetResources& renderTargets)
	{
		const SceneColorTexture2dPair& sceneColorTexturePair = renderTargets.GetSceneColorTexturePair();
		m_framebuffers.resize(2 * m_frameCount);
		for (size_t framebufferIndex = 0; framebufferIndex < m_framebuffers.size(); framebufferIndex++)
		{
			const uint32_t frameIndex = static_cast<uint32_t>(framebufferIndex / 2);
			const uint32_t sceneColorIndex = static_cast<uint32_t>(framebufferIndex % 2);
			const RenderTexture2d& renderTexture = sceneColorTexturePair.GetRenderTargetTexture(frameIndex, sceneColorIndex);
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
			NAME_VK_OBJECT(m_framebuffers[framebufferIndex], "Framebuffer_ForwardTransparent_Frame" + std::to_string(frameIndex) + "_Color" + std::to_string(sceneColorIndex));
		}
	}
}