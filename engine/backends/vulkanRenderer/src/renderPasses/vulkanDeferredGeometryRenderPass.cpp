#include "vulkanDeferredGeometryRenderPass.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanDeferredRenderingContract.h"
#include "vulkanDepthTexture2d.h"
#include "vulkanGBufferTexture2d.h"
#include "vulkanMacros.h"
#include "vulkanRenderTargetResources.h"
#include <array>
#include <string>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	DeferredGeometryRenderPass::DeferredGeometryRenderPass(const RenderTargetResources& renderTargets)
	{
		CreateRenderPass(renderTargets);
		CreateFrameBuffers(renderTargets);
		NAME_VK_OBJECT(m_renderPass, "RenderPass_DeferredGeometry");
	}
	DeferredGeometryRenderPass::~DeferredGeometryRenderPass()
	{

	}



	// Private methods:
	void DeferredGeometryRenderPass::CreateRenderPass(const RenderTargetResources& renderTargets)
	{
		// Attachments:
		std::array<VkAttachmentDescription, deferredRenderingContract::attachmentCount> attachments{};
		{
			const std::array<VkFormat, deferredRenderingContract::attachmentCount> attachmentFormats =
			{
				renderTargets.GetAlbedoTexture(0).GetFormat(),
				renderTargets.GetNormalTexture(0).GetFormat(),
				renderTargets.GetSurfacePropertiesTexture(0).GetFormat(),
				renderTargets.GetSceneDepthTexture(0).GetFormat()
			};

			for (size_t attachmentIndex = 0; attachmentIndex < attachmentFormats.size(); attachmentIndex++)
			{
				attachments[attachmentIndex].format = attachmentFormats[attachmentIndex];
				attachments[attachmentIndex].samples = VK_SAMPLE_COUNT_1_BIT;
				attachments[attachmentIndex].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				attachments[attachmentIndex].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				attachments[attachmentIndex].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachments[attachmentIndex].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				// Albedo/Normal/Surface properties:
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
	void DeferredGeometryRenderPass::CreateFrameBuffers(const RenderTargetResources& renderTargets)
	{
		m_framebuffers.resize(renderTargets.GetFrameCount());
		for (size_t frameIndex = 0; frameIndex < m_framebuffers.size(); frameIndex++)
		{
			std::array<VkImageView, deferredRenderingContract::attachmentCount> attachments =
			{
				renderTargets.GetAlbedoTexture(frameIndex).GetVkImageView(),
				renderTargets.GetNormalTexture(frameIndex).GetVkImageView(),
				renderTargets.GetSurfacePropertiesTexture(frameIndex).GetVkImageView(),
				renderTargets.GetSceneDepthTexture(frameIndex).GetVkImageView()
			};

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = renderTargets.GetAlbedoTexture(frameIndex).GetWidth();
			framebufferInfo.height = renderTargets.GetAlbedoTexture(frameIndex).GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[frameIndex]));
			NAME_VK_OBJECT(m_framebuffers[frameIndex], "Framebuffer_DeferredGeometry_Frame" + std::to_string(frameIndex));
		}
	}
}