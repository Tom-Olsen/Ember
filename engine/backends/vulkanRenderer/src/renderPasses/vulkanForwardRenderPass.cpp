#include "vulkanForwardRenderPass.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanMacros.h"
#include "vulkanRenderTexture2d.h"
#include <array>
#include <stdexcept>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ForwardRenderPass::ForwardRenderPass(
		const std::vector<std::unique_ptr<RenderTexture2d>>& pRenderTextures,
		const std::vector<std::unique_ptr<RenderTexture2d>>& pSecondaryRenderTextures,
		const std::vector<std::unique_ptr<DepthTexture2d>>& pDepthTextures)
	{
		if (pRenderTextures.empty() || pRenderTextures.size() != pSecondaryRenderTextures.size() || pRenderTextures.size() != pDepthTextures.size())
			throw std::invalid_argument("ForwardRenderPass::ForwardRenderPass(...) failed. Scene texture counts must match and must not be zero.");

		m_pRenderTextures.reserve(pRenderTextures.size());
		m_pSecondaryRenderTextures.reserve(pSecondaryRenderTextures.size());
		m_pDepthTextures.reserve(pDepthTextures.size());
		for (size_t frameIndex = 0; frameIndex < pRenderTextures.size(); frameIndex++)
		{
			m_pRenderTextures.push_back(pRenderTextures[frameIndex].get());
			m_pSecondaryRenderTextures.push_back(pSecondaryRenderTextures[frameIndex].get());
			m_pDepthTextures.push_back(pDepthTextures[frameIndex].get());
		}

		CreateRenderPass();
		CreateFrameBuffers();
		NAME_VK_OBJECT(m_renderPass, "RenderPass_Forward");
	}
	ForwardRenderPass::~ForwardRenderPass()
	{

	}



	// Getters:
	RenderTexture2d* ForwardRenderPass::GetRenderTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pRenderTextures.size())
			throw std::out_of_range("ForwardRenderPass::GetRenderTexture(...) failed. Frame index out of range.");
		return m_pRenderTextures[frameIndex];
	}
	RenderTexture2d* ForwardRenderPass::GetSecondaryRenderTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pSecondaryRenderTextures.size())
			throw std::out_of_range("ForwardRenderPass::GetSecondaryRenderTexture(...) failed. Frame index out of range.");
		return m_pSecondaryRenderTextures[frameIndex];
	}
	DepthTexture2d* ForwardRenderPass::GetDepthTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pDepthTextures.size())
			throw std::out_of_range("ForwardRenderPass::GetDepthTexture(...) failed. Frame index out of range.");
		return m_pDepthTextures[frameIndex];
	}



	// Private methods:
	void ForwardRenderPass::CreateRenderPass()
	{
		// Attachments:
		std::array<VkAttachmentDescription, 2> attachments{};
		{
			// Color attachment description:
			attachments[0].format = m_pRenderTextures[0]->GetFormat();
			attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;							// load results from deferred rendering.
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// store final image for post processing + presenting.
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;				// we do not use stencils.
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;			// we do not use stencils.
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	// final layout of deferred render pass.
			attachments[0].finalLayout = VK_IMAGE_LAYOUT_GENERAL;						// layout for post processing compute shaders.

			// Depth attachment description:
			attachments[1].format = m_pDepthTextures[0]->GetFormat();
			attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;							// due to deferred + forward rendering we handle msaa manually.
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;						// load depth of previous deferred render pass.
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;					// store depth for further post processing if desired.
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;			// we do not use stencils.
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		// we do not use stencils.
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
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
		// Deferred lighting pass -> forward pass:
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;	// index of source subpass, where dependency originates. VK_SUBPASS_EXTERNAL = before renderpass.
		dependency.dstSubpass = 0;						// index of destination subpass, where dependency ends.  VK_SUBPASS_EXTERNAL = after renderpass.
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;	// deferred color and depth producer stages.
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;	// forward color and depth consumer stages.
		dependency.srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::LateFragmentTest::depthStencilAttachmentWrite;	// deferred attachment writes that forward consumes.
		dependency.dstAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentRead | AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead | AccessMasks::EarlyFragmentTest::depthStencilAttachmentWrite;	// forward loads, tests, and attachment writes.
		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;	// specify special behaviors.
		// Forward pass -> post processing is handled by barrier + semaphore.

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void ForwardRenderPass::CreateFrameBuffers()
	{
		m_framebuffers.resize(m_pRenderTextures.size());
		for (size_t frameIndex = 0; frameIndex < m_framebuffers.size(); frameIndex++)
		{
			std::array<VkImageView, 2> attachments =
			{
				m_pRenderTextures[frameIndex]->GetVkImageView(),
				m_pDepthTextures[frameIndex]->GetVkImageView()
			};

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_pRenderTextures[frameIndex]->GetWidth();
			framebufferInfo.height = m_pRenderTextures[frameIndex]->GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[frameIndex]));
			NAME_VK_OBJECT(m_framebuffers[frameIndex], "Framebuffer_Forward_Frame" + std::to_string(frameIndex));
		}
	}
}