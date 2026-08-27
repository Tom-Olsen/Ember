#include "vulkanDeferredGeometryRenderPass.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanGBufferTexture2d.h"
#include "vulkanMacros.h"
#include <array>
#include <stdexcept>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	DeferredGeometryRenderPass::DeferredGeometryRenderPass(uint32_t renderWidth, uint32_t renderHeight, const std::vector<std::unique_ptr<DepthTexture2d>>& pDepthTextures)
	{
		if (pDepthTextures.empty())
			throw std::invalid_argument("DeferredGeometryRenderPass::DeferredGeometryRenderPass(...) failed. Depth texture count must not be zero.");

		m_pDepthTextures.reserve(pDepthTextures.size());
		for (const std::unique_ptr<DepthTexture2d>& pDepthTexture : pDepthTextures)
			m_pDepthTextures.push_back(pDepthTexture.get());

		CreateGBufferTextures(renderWidth, renderHeight);
		CreateRenderPass();
		CreateFrameBuffers();
		NAME_VK_OBJECT(m_renderPass, "RenderPass_DeferredGeometry");
	}
	DeferredGeometryRenderPass::~DeferredGeometryRenderPass()
	{

	}



	// Getters:
	GBufferTexture2d* DeferredGeometryRenderPass::GetAlbedoTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pAlbedoTextures.size())
			throw std::out_of_range("DeferredGeometryRenderPass::GetAlbedoTexture(...) failed. Frame index out of range.");
		return m_pAlbedoTextures[frameIndex].get();
	}
	GBufferTexture2d* DeferredGeometryRenderPass::GetNormalTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pNormalTextures.size())
			throw std::out_of_range("DeferredGeometryRenderPass::GetNormalTexture(...) failed. Frame index out of range.");
		return m_pNormalTextures[frameIndex].get();
	}
	GBufferTexture2d* DeferredGeometryRenderPass::GetMaterialTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pMaterialTextures.size())
			throw std::out_of_range("DeferredGeometryRenderPass::GetMaterialTexture(...) failed. Frame index out of range.");
		return m_pMaterialTextures[frameIndex].get();
	}
	DepthTexture2d* DeferredGeometryRenderPass::GetDepthTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pDepthTextures.size())
			throw std::out_of_range("DeferredGeometryRenderPass::GetDepthTexture(...) failed. Frame index out of range.");
		return m_pDepthTextures[frameIndex];
	}



	// Private methods:
	void DeferredGeometryRenderPass::CreateGBufferTextures(uint32_t renderWidth, uint32_t renderHeight)
	{
		m_pAlbedoTextures.reserve(m_pDepthTextures.size());
		m_pNormalTextures.reserve(m_pDepthTextures.size());
		m_pMaterialTextures.reserve(m_pDepthTextures.size());
		for (size_t frameIndex = 0; frameIndex < m_pDepthTextures.size(); frameIndex++)
		{
			m_pAlbedoTextures.push_back(std::make_unique<GBufferTexture2d>(deferredRenderingContract::albedoFormat, renderWidth, renderHeight));
			m_pNormalTextures.push_back(std::make_unique<GBufferTexture2d>(deferredRenderingContract::normalFormat, renderWidth, renderHeight));
			m_pMaterialTextures.push_back(std::make_unique<GBufferTexture2d>(deferredRenderingContract::materialFormat, renderWidth, renderHeight));

			m_pAlbedoTextures[frameIndex]->SetDebugName("GBufferAlbedoTexture_Frame" + std::to_string(frameIndex));
			m_pNormalTextures[frameIndex]->SetDebugName("GBufferNormalTexture_Frame" + std::to_string(frameIndex));
			m_pMaterialTextures[frameIndex]->SetDebugName("GBufferMaterialTexture_Frame" + std::to_string(frameIndex));

			VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::BottomOfPipe::none;
			m_pAlbedoTextures[frameIndex]->GetVmaImage()->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pNormalTextures[frameIndex]->GetVmaImage()->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pMaterialTextures[frameIndex]->GetVmaImage()->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
	void DeferredGeometryRenderPass::CreateRenderPass()
	{
		// Attachments:
		std::array<VkAttachmentDescription, deferredRenderingContract::attachmentCount> attachments{};
		{
			const std::array<VkFormat, deferredRenderingContract::attachmentCount> attachmentFormats =
			{
				m_pAlbedoTextures[0]->GetFormat(),
				m_pNormalTextures[0]->GetFormat(),
				m_pMaterialTextures[0]->GetFormat(),
				m_pDepthTextures[0]->GetFormat()
			};

			for (size_t attachmentIndex = 0; attachmentIndex < attachmentFormats.size(); attachmentIndex++)
			{
				attachments[attachmentIndex].format = attachmentFormats[attachmentIndex];
				attachments[attachmentIndex].samples = VK_SAMPLE_COUNT_1_BIT;
				attachments[attachmentIndex].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				attachments[attachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				attachments[attachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachments[attachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				// Albedo/Normal/Material:
				if (attachmentIndex < deferredRenderingContract::colorAttachmentCount)
				{
					attachments[attachmentIndex].initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					attachments[attachmentIndex].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				}
				// Depth:
				else
				{
					attachments[attachmentIndex].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
					attachments[attachmentIndex].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				}
			}
		}

		// Attachment references:
		std::array<VkAttachmentReference, deferredRenderingContract::colorAttachmentCount> colorAttachmentReferences{};
		for (uint32_t attachmentIndex = 0; attachmentIndex < colorAttachmentReferences.size(); attachmentIndex++)
		{
			colorAttachmentReferences[attachmentIndex].attachment = attachmentIndex;
			colorAttachmentReferences[attachmentIndex].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		VkAttachmentReference depthAttachmentReference = {};
		depthAttachmentReference.attachment = deferredRenderingContract::depthAttachmentIndex;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Subpass:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
		subpass.pColorAttachments = colorAttachmentReferences.data();
		subpass.pDepthStencilAttachment = &depthAttachmentReference;

		// Pre render compute -> deferred geometry pass:
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = AccessMasks::TopOfPipe::none;
		dependency.dstAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::EarlyFragmentTest::depthStencilAttachmentWrite;
		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void DeferredGeometryRenderPass::CreateFrameBuffers()
	{
		m_framebuffers.resize(m_pDepthTextures.size());
		for (size_t frameIndex = 0; frameIndex < m_framebuffers.size(); frameIndex++)
		{
			std::array<VkImageView, deferredRenderingContract::attachmentCount> attachments =
			{
				m_pAlbedoTextures[frameIndex]->GetVkImageView(),
				m_pNormalTextures[frameIndex]->GetVkImageView(),
				m_pMaterialTextures[frameIndex]->GetVkImageView(),
				m_pDepthTextures[frameIndex]->GetVkImageView()
			};

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_pAlbedoTextures[frameIndex]->GetWidth();
			framebufferInfo.height = m_pAlbedoTextures[frameIndex]->GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[frameIndex]));
			NAME_VK_OBJECT(m_framebuffers[frameIndex], "Framebuffer_DeferredGeometry_Frame" + std::to_string(frameIndex));
		}
	}
}