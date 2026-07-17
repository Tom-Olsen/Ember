#include "vulkanForwardRenderPass.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanRenderTexture2d.h"
#include <array>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	ForwardRenderPass::ForwardRenderPass(uint32_t renderWidth, uint32_t renderHeight)
	{
		m_depthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

		CreateRenderTextures(renderWidth, renderHeight);
		CreateRenderPass();
		CreateMsaaImages();
		CreateDepthImages();
		CreateFrameBuffers();
		NAME_VK_OBJECT(m_renderPass, "RenderPass_Forward");
	}
	ForwardRenderPass::~ForwardRenderPass()
	{

	}



	// Getters:
	const VmaImage* const ForwardRenderPass::GetMsaaVmaImage(uint32_t frameIndex) const
	{
		if (Context::GetMsaaSamples() == VK_SAMPLE_COUNT_1_BIT || frameIndex >= m_pMsaaImages.size())
			return nullptr;
		return m_pMsaaImages[frameIndex].get();
	}
	const VmaImage* const ForwardRenderPass::GetDepthVmaImage(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pDepthImages.size())
			return nullptr;
		return m_pDepthImages[frameIndex].get();
	}
	RenderTexture2d* ForwardRenderPass::GetRenderTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pRenderTextures.size())
			return nullptr;
		return m_pRenderTextures[frameIndex].get();
	}
	RenderTexture2d* ForwardRenderPass::GetSecondaryRenderTexture(uint32_t frameIndex) const
	{
		if (frameIndex >= m_pSecondaryRenderTextures.size())
			return nullptr;
		return m_pSecondaryRenderTextures[frameIndex].get();
	}



	// Private methods:
	void ForwardRenderPass::CreateRenderTextures(uint32_t renderWidth, uint32_t renderHeight)
	{
		const uint32_t framesInFlight = Context::GetFramesInFlight();
		VkFormat renderTextureFormat = VK_FORMAT_R16G16B16A16_SFLOAT; // Storage image declarations must use [[vk::image_format("rgba16f")]], e.g. see postProcessing.comp.hlsl.

		m_pRenderTextures.reserve(framesInFlight);
		m_pSecondaryRenderTextures.reserve(framesInFlight);
		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			m_pRenderTextures.push_back(std::make_unique<RenderTexture2d>(renderTextureFormat, renderWidth, renderHeight));
			m_pSecondaryRenderTextures.push_back(std::make_unique<RenderTexture2d>(renderTextureFormat, renderWidth, renderHeight));
			m_pRenderTextures[frameIndex]->SetDebugName("RenderTexture_Forward_Frame" + std::to_string(frameIndex));
			m_pSecondaryRenderTextures[frameIndex]->SetDebugName("RenderTexture_ForwardSecondary_Frame" + std::to_string(frameIndex));

			// Both ping-pong textures are used as storage images by post processing, so initialize them into
			// GENERAL and keep them there outside the present pass:
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_GENERAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::BottomOfPipe::none;
			m_pRenderTextures[frameIndex]->GetVmaImage()->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pSecondaryRenderTextures[frameIndex]->GetVmaImage()->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
	void ForwardRenderPass::CreateRenderPass()
	{
		const bool useMsaa = Context::GetMsaaSamples() != VK_SAMPLE_COUNT_1_BIT;

		// Attachments:
		std::array<VkAttachmentDescription, 3> attachments{};
		{
			// Color attachment description:
			attachments[0].format = m_pRenderTextures[0]->GetFormat();
			attachments[0].samples = Context::GetMsaaSamples();
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;										                // clear framebuffer to black before rendering.
			attachments[0].storeOp = useMsaa ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;         // msaa on: no need to store multisamples after render, msaa off: must store as this will become the final render.
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;								                // do not use stencils.
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;							                // do not use stencils.
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;									                // we don't care about initial layout of the image.
			attachments[0].finalLayout = useMsaa ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL;  // msaa on: color is needed so we can resolve them in the final subpass which then outputs general (for post processing compute shaders). msaa off: directly use layout for post processing compute shaders.

			// Depth attachment description:
			attachments[1].format = m_depthFormat;                              // must be same as depth image format.
			attachments[1].samples = Context::GetMsaaSamples();                 // msaaSamples.
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                // clear depth buffer before rendering.
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;          // depth content is discarded after rendering.
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;     // stencil part not used yet.
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;   // stencil part not used yet.
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			// Color resolve attachment description:
			attachments[2].format = m_pRenderTextures[0]->GetVmaImage()->GetFormat();
			attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;           // dont care about old content (loadOp=dontCare).
			attachments[2].finalLayout = VK_IMAGE_LAYOUT_GENERAL;               // rdy for post processing.
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
		colorResolveAttachmentReference.layout = VK_IMAGE_LAYOUT_GENERAL;	// next pass is post processing, which expects color resolve image in VK_IMAGE_LAYOUT_GENERAL.

		// Subpass:
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;
		subpass.pDepthStencilAttachment = &depthAttachmentReference;
		subpass.pResolveAttachments = useMsaa ? &colorResolveAttachmentReference : nullptr;

		// Synchronization dependencies of individual subpasses:
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;										    // index of source subpass, where dependency originates. VK_SUBPASS_EXTERNAL = before renderpass.
		dependency.dstSubpass = 0;															    // index of destination subpass, where dependency ends.  VK_SUBPASS_EXTERNAL = after renderpass.
		dependency.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;              // pipeline stages in source subpass which must complete before the dependency is resolved.
		dependency.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;              // pipeline stages in destination subpass which must wait for the source stages to complete.
		dependency.srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;    // types of memory accesses in the source subpass that must be completed.
		dependency.dstAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;	// types of memory accesses in the destination subpass that must wait on the source subpass to complete.
		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;							    // specify special behaviors.

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size()) - (useMsaa ? 0 : 1); // no msaa => ignore multisample resolve attachment.
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void ForwardRenderPass::CreateMsaaImages()
	{
		if (Context::GetMsaaSamples() == VK_SAMPLE_COUNT_1_BIT)
			return;

		const uint32_t framesInFlight = Context::GetFramesInFlight();
		m_pMsaaImages.reserve(framesInFlight);
		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			VkImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent = { m_pRenderTextures[frameIndex]->GetWidth(), m_pRenderTextures[frameIndex]->GetHeight(), 1 };
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = m_pRenderTextures[frameIndex]->GetFormat();
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = Context::GetMsaaSamples();
			imageInfo.flags = 0;

			VmaAllocationCreateInfo allocationInfo = {};
			allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			allocationInfo.flags = 0;
			allocationInfo.requiredFlags = 0;
			allocationInfo.preferredFlags = 0;

			VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
			DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
			VkDeviceSize allocationSize = 8ull * imageInfo.extent.width * imageInfo.extent.height * imageInfo.samples;
			m_pMsaaImages.push_back(std::make_unique<VmaImage>(imageInfo, allocationInfo, allocationSize, subresourceRange, viewType, queue));
			NAME_VK_OBJECT(m_pMsaaImages[frameIndex]->GetVkImage(), "Image_ForwardMsaa_Frame" + std::to_string(frameIndex));
			NAME_VK_OBJECT(m_pMsaaImages[frameIndex]->GetVkImageView(), "ImageView_ForwardMsaa_Frame" + std::to_string(frameIndex));
		}
	}
	void ForwardRenderPass::CreateDepthImages()
	{
		const uint32_t framesInFlight = Context::GetFramesInFlight();
		m_pDepthImages.reserve(framesInFlight);
		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			VkImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent = { m_pRenderTextures[frameIndex]->GetWidth(), m_pRenderTextures[frameIndex]->GetHeight(), 1 };
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = m_depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = Context::GetMsaaSamples();
			imageInfo.flags = 0;

			VmaAllocationCreateInfo allocationInfo = {};
			allocationInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			allocationInfo.flags = 0;
			allocationInfo.requiredFlags = 0;
			allocationInfo.preferredFlags = 0;

			VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
			DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
			VkDeviceSize allocationSize = 8ull * imageInfo.extent.width * imageInfo.extent.height * imageInfo.samples;
			m_pDepthImages.push_back(std::make_unique<VmaImage>(imageInfo, allocationInfo, allocationSize, subresourceRange, viewType, queue));
			NAME_VK_OBJECT(m_pDepthImages[frameIndex]->GetVkImage(), "Image_ForwardDepth_Frame" + std::to_string(frameIndex));
			NAME_VK_OBJECT(m_pDepthImages[frameIndex]->GetVkImageView(), "ImageView_ForwardDepth_Frame" + std::to_string(frameIndex));

			// Transition: Layout: undefined->depth attachment, Queue: graphics
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead;
			m_pDepthImages[frameIndex]->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
	void ForwardRenderPass::CreateFrameBuffers()
	{
		const bool useMsaa = Context::GetMsaaSamples() != VK_SAMPLE_COUNT_1_BIT;
		size_t imageCount = Context::GetFramesInFlight();
		m_framebuffers.resize(imageCount);
		std::array<VkImageView, 3> attachments;

		for (size_t i = 0; i < imageCount; i++)
		{
			// order of attachments is important!
			attachments[0] = useMsaa ? m_pMsaaImages[i]->GetVkImageView() : m_pRenderTextures[i]->GetVmaImage()->GetVkImageView();  // msaa on: write into msaa images. msaa off: write to final render texture.
			attachments[1] = m_pDepthImages[i]->GetVkImageView();
			attachments[2] = m_pRenderTextures[i]->GetVmaImage()->GetVkImageView(); // msaa on: resolve msaa images to final render texture (skipped implicitly with msaa off, as this attachment doesn't get bound).

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size()) - (useMsaa ? 0 : 1); // no msaa => ignore multisample resolve attachment.
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_pRenderTextures[i]->GetWidth();
			framebufferInfo.height = m_pRenderTextures[i]->GetHeight();
			framebufferInfo.layers = 1;
			VKA(vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]));
			NAME_VK_OBJECT(m_framebuffers[i], "Framebuffer_Forward_Frame" + std::to_string(i));
		}
	}
}