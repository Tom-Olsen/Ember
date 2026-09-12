#include "vulkanGizmoRenderPass.h"
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
	GizmoRenderPass::GizmoRenderPass(const RenderTargetResources& renderTargets)
	{
		CreateRenderPass(renderTargets);
		CreateFrameBuffers(renderTargets);
		NAME_VK_OBJECT(m_renderPass, "RenderPass_Gizmo");
	}
	GizmoRenderPass::~GizmoRenderPass()
	{

	}



	// Private methods:
	void GizmoRenderPass::CreateRenderPass(const RenderTargetResources& renderTargets)
	{
		const bool useMsaa = Context::GetMsaaSamples() != VK_SAMPLE_COUNT_1_BIT;

		// Attachments: [0]=color, [1]=depth, [2]=color resolve (msaa only)
		std::array<VkAttachmentDescription, 3> attachments{};
		{
			// Color attachment description:
			attachments[0].format = renderTargets.GetGizmoTexture(0).GetFormat();
			attachments[0].samples = Context::GetMsaaSamples();
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;										                                // clear framebuffer to black before rendering.
			attachments[0].storeOp = useMsaa ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;                         // msaa on: no need to store multisamples after render, msaa off: must store as this will become the final render.
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;								                                // do not use stencils.
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;							                                // do not use stencils.
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;									                                // we don't care about initial layout of the image.
			attachments[0].finalLayout = useMsaa ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // msaa on: color is resolved into the sampled image, msaa off: image is sampled directly.

			// Depth attachment description:
			attachments[1].format = renderTargets.GetGizmoDepthImage(0).GetFormat();
			attachments[1].samples = Context::GetMsaaSamples();
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;               // clear depth buffer before rendering.
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;         // depth content is discarded after rendering.
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			// Color resolve attachment description:
			attachments[2].format = renderTargets.GetGizmoTexture(0).GetFormat();
			attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;               // dont care about old content (loadOp=dontCare).
			attachments[2].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;  // ready for compositing in present/imgui fragment shaders.
		}

		// Attachment references:
		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentReference = {};
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorResolveAttachmentReference = {};
		colorResolveAttachmentReference.attachment = 2;
		colorResolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Subpass:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;
		subpass.pDepthStencilAttachment = &depthAttachmentReference;
		subpass.pResolveAttachments = useMsaa ? &colorResolveAttachmentReference : nullptr;

		// Synchronization dependencies of individual subpasses:
        // No input dependency:
		std::array<VkSubpassDependency, 2> dependencies{};
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;														            // index of source subpass, where dependency originates. VK_SUBPASS_EXTERNAL = before renderpass.
		dependencies[0].dstSubpass = 0;																			            // index of destination subpass, where dependency ends.
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;										            // no previous rendering/compute work must finish before this pass clears the gizmo image.
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = AccessMasks::TopOfPipe::none;											            // no previous memory access must be made visible, because the attachments are cleared.
		dependencies[0].dstAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite | AccessMasks::EarlyFragmentTest::depthStencilAttachmentWrite;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;											            // specify special behaviors.
        // Present/ImGui will consume render textures of this render pass:
		dependencies[1].srcSubpass = 0;																			            // index of source subpass, where dependency originates.
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;														            // index of destination subpass, where dependency ends. VK_SUBPASS_EXTERNAL = after renderpass.
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;							            // color attachment output must complete before the resolved gizmo texture is consumed.
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;												// compositing happens in the present fragment shader.
		dependencies[1].srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;				            // gizmo color writes must be made visible.
		dependencies[1].dstAccessMask = AccessMasks::FragmentShader::shaderRead;											// composite shader reads must wait on those writes.
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;											            // specify special behaviors.

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size()) - (useMsaa ? 0 : 1); // no msaa => ignore multisample resolve attachment.
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void GizmoRenderPass::CreateFrameBuffers(const RenderTargetResources& renderTargets)
	{
		const bool useMsaa = Context::GetMsaaSamples() != VK_SAMPLE_COUNT_1_BIT;
		size_t imageCount = renderTargets.GetFrameCount();
		m_framebuffers.resize(imageCount);
		std::array<VkImageView, 3> attachments;

		for (size_t i = 0; i < imageCount; i++)
		{
			const RenderTexture2d& gizmoTexture = renderTargets.GetGizmoTexture(i);
			
			// order of attachments is important!
			attachments[0] = useMsaa ? renderTargets.GetGizmoMsaaImage(i).GetVkImageView() : gizmoTexture.GetVmaImage()->GetVkImageView();  // msaa on: write into msaa images. msaa off: write to final render texture.
			attachments[1] = renderTargets.GetGizmoDepthImage(i).GetVkImageView();
			attachments[2] = gizmoTexture.GetVmaImage()->GetVkImageView(); // msaa on: resolve msaa images to final render texture (skipped implicitly with msaa off, as this attachment doesn't get bound).

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size()) - (useMsaa ? 0 : 1); // no msaa => ignore multisample resolve attachment.
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = gizmoTexture.GetWidth();
			framebufferInfo.height = gizmoTexture.GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]));
			NAME_VK_OBJECT(m_framebuffers[i], "Framebuffer_Gizmo_Frame" + std::to_string(i));
		}
	}
}