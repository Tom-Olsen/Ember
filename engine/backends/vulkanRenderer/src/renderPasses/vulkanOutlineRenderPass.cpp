#include "vulkanOutlineRenderPass.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanRenderTexture2d.h"
#include <array>



namespace vulkanRendererBackend
{
	// Public methods:
    // Constructor/Destructor:
	OutlineRenderPass::OutlineRenderPass(const RenderTargetResources& renderTargets)
	{
		CreateRenderPass(renderTargets);
		CreateFrameBuffers(renderTargets);
		NAME_VK_OBJECT(m_renderPass, "RenderPass_Outline");
	}
	OutlineRenderPass::~OutlineRenderPass()
	{

	}



	// Private methods:
	void OutlineRenderPass::CreateRenderPass(const RenderTargetResources& renderTargets)
	{
        // Color attachment description:
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = renderTargets.GetOutlineTexture(0).GetFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;               // clear framebuffer to black before rendering.
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;             // stored for later mid-render compute consumption.
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
		// MidRenderCompute will consume render textures of this render pass:
		dependencies[1].srcSubpass = 0;																// index of source subpass, where dependency originates.
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;											// index of destination subpass, where dependency ends. VK_SUBPASS_EXTERNAL = after renderpass.
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;				// color attachment output must complete before the resolved gizmo texture is consumed.
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;						// expansion happens in the mid-render compute shader pass.
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
	void OutlineRenderPass::CreateFrameBuffers(const RenderTargetResources& renderTargets)
	{
		size_t imageCount = renderTargets.GetFrameCount();
		m_framebuffers.resize(imageCount);

		for (size_t i = 0; i < imageCount; i++)
		{
			// order of attachments is important!
			const RenderTexture2d& outlineTexture = renderTargets.GetOutlineTexture(i);
			VkImageView attachment = outlineTexture.GetVmaImage()->GetVkImageView();

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &attachment;
			framebufferInfo.width = outlineTexture.GetWidth();
			framebufferInfo.height = outlineTexture.GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]));
			NAME_VK_OBJECT(m_framebuffers[i], "Framebuffer_Outline_Frame" + std::to_string(i));
		}
	}
}