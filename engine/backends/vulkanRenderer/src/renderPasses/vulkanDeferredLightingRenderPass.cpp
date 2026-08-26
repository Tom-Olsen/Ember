#include "vulkanDeferredLightingRenderPass.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDeferredGeometryRenderPass.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanGBufferTexture2d.h"
#include "vulkanMacros.h"
#include "vulkanRenderTexture2d.h"
#include <stdexcept>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	DeferredLightingRenderPass::DeferredLightingRenderPass(
		const std::vector<std::unique_ptr<RenderTexture2d>>& pSceneColorTextures,
		const std::vector<std::unique_ptr<DepthTexture2d>>& pDepthTextures,
		const DeferredGeometryRenderPass& deferredGeometryRenderPass)
	{
		if (pSceneColorTextures.empty() || pSceneColorTextures.size() != pDepthTextures.size())
			throw std::invalid_argument("DeferredLightingRenderPass::DeferredLightingRenderPass(...) failed. Scene texture counts must match and must not be zero.");

		m_pSceneColorTextures.reserve(pSceneColorTextures.size());
		m_pAlbedoTextures.reserve(pSceneColorTextures.size());
		m_pNormalTextures.reserve(pSceneColorTextures.size());
		m_pMaterialTextures.reserve(pSceneColorTextures.size());
		m_pDepthTextures.reserve(pSceneColorTextures.size());
		for (uint32_t frameIndex = 0; frameIndex < pSceneColorTextures.size(); frameIndex++)
		{
			m_pSceneColorTextures.push_back(pSceneColorTextures[frameIndex].get());
			m_pAlbedoTextures.push_back(deferredGeometryRenderPass.GetAlbedoTexture(frameIndex));
			m_pNormalTextures.push_back(deferredGeometryRenderPass.GetNormalTexture(frameIndex));
			m_pMaterialTextures.push_back(deferredGeometryRenderPass.GetMaterialTexture(frameIndex));
			m_pDepthTextures.push_back(pDepthTextures[frameIndex].get());
		}

		CreateRenderPass();
		CreateFrameBuffers();
		NAME_VK_OBJECT(m_renderPass, "RenderPass_DeferredLighting");
	}
	DeferredLightingRenderPass::~DeferredLightingRenderPass()
	{

	}



	// Getters:
	RenderTexture2d* DeferredLightingRenderPass::GetSceneColorTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pSceneColorTextures.size())
			throw std::out_of_range("DeferredLightingRenderPass::GetSceneColorTexture(...) failed. Frame index out of range.");
		return m_pSceneColorTextures[frameIndex];
	}
	GBufferTexture2d* DeferredLightingRenderPass::GetAlbedoTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pAlbedoTextures.size())
			throw std::out_of_range("DeferredLightingRenderPass::GetAlbedoTexture(...) failed. Frame index out of range.");
		return m_pAlbedoTextures[frameIndex];
	}
	GBufferTexture2d* DeferredLightingRenderPass::GetNormalTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pNormalTextures.size())
			throw std::out_of_range("DeferredLightingRenderPass::GetNormalTexture(...) failed. Frame index out of range.");
		return m_pNormalTextures[frameIndex];
	}
	GBufferTexture2d* DeferredLightingRenderPass::GetMaterialTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pMaterialTextures.size())
			throw std::out_of_range("DeferredLightingRenderPass::GetMaterialTexture(...) failed. Frame index out of range.");
		return m_pMaterialTextures[frameIndex];
	}
	DepthTexture2d* DeferredLightingRenderPass::GetDepthTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pDepthTextures.size())
			throw std::out_of_range("DeferredLightingRenderPass::GetDepthTexture(...) failed. Frame index out of range.");
		return m_pDepthTextures[frameIndex];
	}



	// Private methods:
	void DeferredLightingRenderPass::CreateRenderPass()
	{
		// Attachments:
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = m_pSceneColorTextures[0]->GetFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
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
	void DeferredLightingRenderPass::CreateFrameBuffers()
	{
		m_framebuffers.resize(m_pSceneColorTextures.size());
		for (size_t frameIndex = 0; frameIndex < m_framebuffers.size(); frameIndex++)
		{
			VkImageView attachment = m_pSceneColorTextures[frameIndex]->GetVkImageView();

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &attachment;
			framebufferInfo.width = m_pSceneColorTextures[frameIndex]->GetWidth();
			framebufferInfo.height = m_pSceneColorTextures[frameIndex]->GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[frameIndex]));
			NAME_VK_OBJECT(m_framebuffers[frameIndex], "Framebuffer_DeferredLighting_Frame" + std::to_string(frameIndex));
		}
	}
}