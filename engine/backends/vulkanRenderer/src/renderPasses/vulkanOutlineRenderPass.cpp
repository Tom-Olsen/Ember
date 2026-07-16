#include "vulkanOutlineRenderPass.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanRenderTexture2d.h"
#include <array>



namespace vulkanRendererBackend
{
	// Public methods:
    // Constructor/Destructor:
	OutlineRenderPass::OutlineRenderPass(uint32_t renderWidth, uint32_t renderHeight)
	{
		CreateRenderTextures(renderWidth, renderHeight);
		CreateRenderPass();
		CreateFrameBuffers();
		NAME_VK_OBJECT(m_renderPass, "RenderPass_Outline");
	}
	OutlineRenderPass::~OutlineRenderPass()
	{

	}



	// Getters:
	RenderTexture2d* OutlineRenderPass::GetRenderTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pRenderTextures.size())
			return nullptr;
		return m_pRenderTextures[frameIndex].get();
	}



	// Private methods:
	void OutlineRenderPass::CreateRenderTextures(uint32_t renderWidth, uint32_t renderHeight)
	{
		const uint32_t framesInFlight = Context::GetFramesInFlight();
		VkFormat renderTextureFormat = VK_FORMAT_R8_UNORM;

		m_pRenderTextures.reserve(framesInFlight);
		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			m_pRenderTextures.push_back(std::make_unique<RenderTexture2d>(renderTextureFormat, renderWidth, renderHeight));
			m_pRenderTextures[frameIndex]->SetDebugName("RenderTexture_Outline_Frame" + std::to_string(frameIndex));

			// Initialize the layout tracked by VmaImage before the first render pass use:
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_GENERAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::BottomOfPipe::none;
			m_pRenderTextures[frameIndex]->GetVmaImage()->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
	void OutlineRenderPass::CreateRenderPass()
	{
        // Color attachment description:
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_pRenderTextures[0]->GetFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;               // clear framebuffer to black before rendering.
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;             // stored for later post render compute pass comsumption.
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;    // do not use stencils.
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;  // do not use stencils.
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;          // we don't care about initial layout of the image.
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;              // image used for compute.

		// Attachment references:
		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Subpass:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;
		subpass.pDepthStencilAttachment = nullptr;
		subpass.pResolveAttachments = nullptr;

		// Synchronization dependencies of individual subpasses:
        // No input dependency:
		std::array<VkSubpassDependency, 2> dependencies{};
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;											// index of source subpass, where dependency originates. VK_SUBPASS_EXTERNAL = before renderpass.
		dependencies[0].dstSubpass = 0;																// index of destination subpass, where dependency ends.
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;							// no previous rendering/compute work must finish before this pass clears the gizmo image.
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = AccessMasks::TopOfPipe::none;								// no previous memory access must be made visible, because the attachments are cleared.
		dependencies[0].dstAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;                              // specify special behaviors.
        // PostRenderCompute will consume render textures of this render pass:
		dependencies[1].srcSubpass = 0;																// index of source subpass, where dependency originates.
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;											// index of destination subpass, where dependency ends. VK_SUBPASS_EXTERNAL = after renderpass.
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;				// color attachment output must complete before the resolved gizmo texture is consumed.
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;						// compositing happens in the post render compute shader pass.
		dependencies[1].srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;	// gizmo color writes must be made visible.
		dependencies[1].dstAccessMask = AccessMasks::ComputeShader::shaderRead;						// compute shader reads must wait on mask writes.
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;								// specify special behaviors.

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void OutlineRenderPass::CreateFrameBuffers()
	{
		size_t imageCount = Context::GetFramesInFlight();
		m_framebuffers.resize(imageCount);

		for (size_t i = 0; i < imageCount; i++)
		{
			// order of attachments is important!
			VkImageView attachment = m_pRenderTextures[i]->GetVmaImage()->GetVkImageView();

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &attachment;
			framebufferInfo.width = m_pRenderTextures[i]->GetWidth();
			framebufferInfo.height = m_pRenderTextures[i]->GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]));
			NAME_VK_OBJECT(m_framebuffers[i], "Framebuffer_Outline_Frame" + std::to_string(i));
		}
	}
}