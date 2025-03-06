#include "shadowRenderPass.h"
#include "depthTexture2dArray.h"
#include "lighting.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// static members:
	VkFormat ShadowRenderPass::s_shadowMapFormat = VK_FORMAT_D32_SFLOAT;
	uint32_t ShadowRenderPass::s_layerCount = Lighting::maxDirectionalLights + Lighting::maxPositionalLights;



	// Constructor/Destructor:
	ShadowRenderPass::ShadowRenderPass()
	{
		// Create shadow map texture:
		m_shadowMaps = std::make_unique<DepthTexture2dArray>("shadowMaps", s_shadowMapFormat, Lighting::shadowMapResolution, Lighting::shadowMapResolution, s_layerCount);
		CreateRenderpass();
		CreateFramebuffers();
	}
	ShadowRenderPass::~ShadowRenderPass()
	{

	}



	// Public methods:
	DepthTexture2dArray* const ShadowRenderPass::GetShadowMaps() const
	{
		return m_shadowMaps.get();
	}



	// Private methods:
	void ShadowRenderPass::CreateRenderpass()
	{
		// Attachment description:
		VkAttachmentDescription attachment = {};
		attachment.format = s_shadowMapFormat;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;					// clear framebuffer to black before rendering
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;					// store for later render passes
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;			// do not use stencils
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		// do not use stencils
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;				// we don't care about initial layout of the image
		attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;	// will be read in a shader

		// Attachment reference:
		VkAttachmentReference shadowMapAttachmentReference = {};
		shadowMapAttachmentReference.attachment = 0;
		shadowMapAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Subpass description:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.pDepthStencilAttachment = &shadowMapAttachmentReference;

		// Create render pass:
		VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &attachment;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpass;
		VKA(vkCreateRenderPass(VulkanContext::GetVkDevice(), &createInfo, nullptr, &m_renderPass));
	}
	void ShadowRenderPass::CreateFramebuffers()
	{
		// One framebuffer per swapchain image:
		m_framebuffers.resize(VulkanContext::framesInFlight);
		for (uint32_t i = 0; i < VulkanContext::framesInFlight; i++)
		{
			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &m_shadowMaps->GetVmaImage()->GetVkImageView();
			framebufferInfo.width = Lighting::shadowMapResolution;
			framebufferInfo.height = Lighting::shadowMapResolution;
			framebufferInfo.layers = s_layerCount;
			vkCreateFramebuffer(VulkanContext::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]);
		}
	}
}