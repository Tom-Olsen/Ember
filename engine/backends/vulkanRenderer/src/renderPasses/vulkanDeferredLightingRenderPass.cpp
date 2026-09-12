#include "vulkanDeferredLightingRenderPass.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	DeferredLightingRenderPass::DeferredLightingRenderPass(const RenderTargetResources& renderTargets)
	{
		CreateRenderPass(renderTargets);
		CreateFrameBuffers(renderTargets);
		NAME_VK_OBJECT(m_renderPass, "RenderPass_DeferredLighting");
	}
	DeferredLightingRenderPass::~DeferredLightingRenderPass()
	{

	}



	// Private methods:
	void DeferredLightingRenderPass::CreateRenderPass(const RenderTargetResources& renderTargets)
	{
		// Attachments:
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = renderTargets.GetSceneColorTexturePair().GetRenderTargetTexture(0, 0).GetFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Attachment references:
		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Subpass:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;

		// Deferred geometry pass -> deferred lighting pass:
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::LateFragmentTest::depthStencilAttachmentWrite;
		dependency.dstAccessMask = AccessMasks::FragmentShader::shaderRead | AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;
		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void DeferredLightingRenderPass::CreateFrameBuffers(const RenderTargetResources& renderTargets)
	{
		m_framebuffers.resize(renderTargets.GetFrameCount());
		for (size_t frameIndex = 0; frameIndex < m_framebuffers.size(); frameIndex++)
		{
			const RenderTexture2d& sceneColorTexture = renderTargets.GetSceneColorTexturePair().GetRenderTargetTexture(frameIndex, 0);
			VkImageView attachment = sceneColorTexture.GetVkImageView();

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &attachment;
			framebufferInfo.width = sceneColorTexture.GetWidth();
			framebufferInfo.height = sceneColorTexture.GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[frameIndex]));
			NAME_VK_OBJECT(m_framebuffers[frameIndex], "Framebuffer_DeferredLighting_Frame" + std::to_string(frameIndex));
		}
	}
}