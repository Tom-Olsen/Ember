#include "shadowRenderPass.h"
#include "macros.h"
#include "texture2d.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



// static members:
uint32_t ShadowRenderPass::s_shadowMapWidth = 1024;
uint32_t ShadowRenderPass::s_shadowMapHeight = 1024;
uint32_t ShadowRenderPass::s_layerCount = MAX_D_LIGHTS + MAX_S_LIGHTS + 6 * MAX_P_LIGHTS;



// Constructor/Destructor:
ShadowRenderPass::ShadowRenderPass(VulkanContext* pContext)
{
	m_pContext = pContext;

	CreateShadowMapTexture();
	CreateRenderpass();
	CreateFramebuffers();
}
ShadowRenderPass::~ShadowRenderPass()
{

}



// Public methods:
Texture2d* const ShadowRenderPass::GetShadowMaps() const
{
	return m_shadowMaps.get();
}



// Private methods:
void ShadowRenderPass::CreateShadowMapTexture()
{
	// Subresource range:
	VkImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = s_layerCount;

	// Image info:
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = s_shadowMapWidth;
	imageInfo.extent.height = s_shadowMapHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = s_layerCount;
	imageInfo.format = m_shadowMapFormat;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	// Allocation info:
	VmaAllocationCreateInfo allocationInfo = {};
	allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	allocationInfo.flags = 0;
	allocationInfo.requiredFlags = 0;
	allocationInfo.preferredFlags = 0;

	VmaImage* image = new VmaImage(m_pContext, imageInfo, allocationInfo, subresourceRange);
	m_shadowMaps = std::make_unique<Texture2d>(m_pContext, image, "shadowMaps");
}
void ShadowRenderPass::CreateRenderpass()
{
	// Attachment description:
	VkAttachmentDescription attachment = {};
	attachment.format = m_shadowMapFormat;
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
	VKA(vkCreateRenderPass(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_renderPass));
}
void ShadowRenderPass::CreateFramebuffers()
{
	// One framebuffer per swapchain image:
	m_framebuffers.resize(m_pContext->framesInFlight);
	for (uint32_t i = 0; i < m_pContext->framesInFlight; i++)
	{
		VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &m_shadowMaps->GetVmaImage()->GetVkImageView();
		framebufferInfo.width = s_shadowMapWidth;
		framebufferInfo.height = s_shadowMapHeight;
		framebufferInfo.layers = s_layerCount;
		vkCreateFramebuffer(m_pContext->GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]);
	}
}