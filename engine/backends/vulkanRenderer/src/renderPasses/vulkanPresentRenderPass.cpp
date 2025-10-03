#include "vulkanPresentRenderPass.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStage.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	PresentRenderPass::PresentRenderPass()
	{
		CreateRenderPass();
		CreateFrameBuffers();
		NAME_VK_RENDER_PASS(m_renderPass, "presentRenderPass");
	}
	PresentRenderPass::~PresentRenderPass()
	{

	}



	// Private methods:
	void PresentRenderPass::CreateRenderPass()
	{
		// Attachments:
		VkAttachmentDescription attachment = {};
		attachment.format = static_cast<VkFormat>(Context::GetSurface()->GetSurfaceFormat().format);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Attachment references:
		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Subpass:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = PipelineStages::colorAttachmentOutput;
		dependency.dstStageMask = PipelineStages::colorAttachmentOutput;
		dependency.srcAccessMask = AccessMasks::ColorAttachmentOutput::none;
		dependency.dstAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &attachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void PresentRenderPass::CreateFrameBuffers()
	{
		size_t imageCount = Context::GetSwapchain()->GetImages().size();
		Uint2 surfaceExtend = Context::GetSurface()->GetCurrentExtent();
		m_framebuffers.resize(imageCount);

		for (size_t i = 0; i < imageCount; i++)
		{
			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &Context::GetSwapchain()->GetImageViews()[i];
			framebufferInfo.width = surfaceExtend.x;
			framebufferInfo.height = surfaceExtend.y;
			framebufferInfo.layers = 1;
			vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]);
		}
	}
}