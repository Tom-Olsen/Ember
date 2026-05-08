#include "vulkanForwardRenderPass.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanPipelineStage.h"
#include "vulkanRenderTexture2d.h"
#include <array>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	ForwardRenderPass::ForwardRenderPass(uint32_t renderWidth, uint32_t renderHeight)
	{
		m_depthFormat = Formats::d32_sfloat_s8_uint;

		CreateRenderTextures(renderWidth, renderHeight);
		CreateRenderPass();
		CreateMsaaImages();
		CreateDepthImages();
		CreateFrameBuffers();
		NAME_VK_OBJECT(m_renderPass, "forwardRenderPass");
	}
	ForwardRenderPass::~ForwardRenderPass()
	{

	}



	// Public methods:
	const VmaImage* const ForwardRenderPass::GetMsaaVmaImage(uint32_t frameIndex) const
	{
		return m_pMsaaImages[frameIndex].get();
	}
	const VmaImage* const ForwardRenderPass::GetDepthVmaImage(uint32_t frameIndex) const
	{
		return m_pDepthImages[frameIndex].get();
	}
	RenderTexture2d* ForwardRenderPass::GetRenderTexture(uint32_t frameIndex) const
	{
		return m_pRenderTextures[frameIndex].get();
	}
	RenderTexture2d* ForwardRenderPass::GetSecondaryRenderTexture(uint32_t frameIndex) const
	{
		return m_pSecondaryRenderTextures[frameIndex].get();
	}



	// Private methods:
	void ForwardRenderPass::CreateRenderTextures(uint32_t renderWidth, uint32_t renderHeight)
	{
		const uint32_t framesInFlight = Context::GetFramesInFlight();
		Format renderTextureFormat = Formats::r16g16b16a16_sfloat; // make sure to use [[vk::image_format("rgba16f")]] RWTexture2D<float4> for render textures in shaders, see inOut.comp.hlsl.

		m_pRenderTextures.reserve(framesInFlight);
		m_pSecondaryRenderTextures.reserve(framesInFlight);
		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			m_pRenderTextures.push_back(std::make_unique<RenderTexture2d>((VkFormat)renderTextureFormat, renderWidth, renderHeight));
			m_pSecondaryRenderTextures.push_back(std::make_unique<RenderTexture2d>((VkFormat)renderTextureFormat, renderWidth, renderHeight));

			// Both ping-pong textures are used as storage images by post processing, so initialize them into
			// GENERAL and keep them there outside the present pass.
			ImageLayout newLayout = ImageLayouts::general;
			PipelineStage srcStage = PipelineStages::topOfPipe;
			PipelineStage dstStage = PipelineStages::bottomOfPipe;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::BottomOfPipe::none;
			m_pRenderTextures[frameIndex]->GetVmaImage()->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
			m_pSecondaryRenderTextures[frameIndex]->GetVmaImage()->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
	void ForwardRenderPass::CreateRenderPass()
	{
		// Attachments:
		std::array<VkAttachmentDescription, 3> attachments{};
		{
			// Multisampled color attachment description:
			attachments[0].format = static_cast<VkFormat>(m_pRenderTextures[0]->GetVmaImage()->GetFormat());
			attachments[0].samples = static_cast<VkSampleCountFlagBits>(Context::GetMsaaSamples());		// multisampling count
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;										// clear framebuffer to black before rendering
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;									// no need to store multisampls after render
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;								// do not use stencils
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;							// do not use stencils
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;									// we don't care about initial layout of the image
			attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;						// multisampled images are stored in color layout and not rdy for presenting yet

			// Depth attachment description:
			attachments[1].format = static_cast<VkFormat>(m_depthFormat);								// must be same as depth image format
			attachments[1].samples = static_cast<VkSampleCountFlagBits>(Context::GetMsaaSamples());		// msaaSamples
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;										// clear depth buffer before rendering
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;									// depth content is discarded after rendering
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;								// stencil part not used yet
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;							// stencil part not used yet
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			// Color resolve attachment description: (resolve multisampled fragments)
			attachments[2].format = static_cast<VkFormat>(m_pRenderTextures[0]->GetVmaImage()->GetFormat());
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
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // vulkan may also support compute subpasses in the future
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;
		subpass.pDepthStencilAttachment = &depthAttachmentReference;
		subpass.pResolveAttachments = &colorResolveAttachmentReference;

		// Synchronization dependencies of individual subpasses:
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;										// index of source subpass, where dependency originates. VK_SUBPASS_EXTERNAL = before renderpass
		dependency.dstSubpass = 0;															// index of destination subpass, where dependency ends.  VK_SUBPASS_EXTERNAL = after renderpass
		dependency.srcStageMask = PipelineStages::colorAttachmentOutput;						// pipeline stages in source subpass which must complete before the dependency is resolved
		dependency.dstStageMask = PipelineStages::colorAttachmentOutput;						// pipeline stages in destination subpass which must wait for the source stages to complete
		dependency.srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;	// types of memory accesses in the source subpass that must be completed
		dependency.dstAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;	// types of memory accesses in the destination subpass that must wait on the source subpass to complete
		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;							// specify special behaviors

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VKA(vkCreateRenderPass(Context::GetVkDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	void ForwardRenderPass::CreateMsaaImages()
	{
		const uint32_t framesInFlight = Context::GetFramesInFlight();
		m_pMsaaImages.reserve(framesInFlight);
		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			ImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = ImageAspectFlags::color_bit;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			ImageCreateInfo imageInfo = {};
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.x = m_pRenderTextures[frameIndex]->GetWidth();
			imageInfo.extent.y = m_pRenderTextures[frameIndex]->GetHeight();
			imageInfo.extent.z = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = m_pRenderTextures[frameIndex]->GetVmaImage()->GetFormat();
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = ImageLayouts::undefined;
			imageInfo.usages = ImageUsageFlags::transient_attachment_bit | ImageUsageFlags::color_attachment_bit;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.sampleCountFlags = Context::GetMsaaSamples();
			imageInfo.flags = 0;

			AllocationCreateInfo allocationInfo = {};
			allocationInfo.usages = MemoryUsages::auto_prefer_device;
			allocationInfo.flags = 0;
			allocationInfo.requiredFlags = 0;
			allocationInfo.preferredFlags = 0;

			VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
			DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
			m_pMsaaImages.push_back(std::make_unique<VmaImage>(imageInfo, allocationInfo, subresourceRange, viewType, queue));
		}
	}
	void ForwardRenderPass::CreateDepthImages()
	{
		const uint32_t framesInFlight = Context::GetFramesInFlight();
		m_pDepthImages.reserve(framesInFlight);
		for (uint32_t frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
		{
			ImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = ImageAspectFlags::depth_bit | ImageAspectFlags::stencil_bit;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			ImageCreateInfo imageInfo = {};
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.x = m_pRenderTextures[frameIndex]->GetWidth();
			imageInfo.extent.y = m_pRenderTextures[frameIndex]->GetHeight();
			imageInfo.extent.z = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = m_depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = ImageLayouts::undefined;
			imageInfo.usages = ImageUsageFlags::depth_stencil_attachment_bit;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.sampleCountFlags = Context::GetMsaaSamples();
			imageInfo.flags = 0;

			AllocationCreateInfo allocationInfo = {};
			allocationInfo.usages = MemoryUsages::auto_prefer_device;
			allocationInfo.flags = 0;
			allocationInfo.requiredFlags = 0;
			allocationInfo.preferredFlags = 0;

			VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
			DeviceQueue queue = Context::GetLogicalDevice()->GetGraphicsQueue();
			m_pDepthImages.push_back(std::make_unique<VmaImage>(imageInfo, allocationInfo, subresourceRange, viewType, queue));

			// Transition: Layout: undefined->depth attachment, Queue: graphics
			ImageLayout newLayout = ImageLayouts::depth_stencil_attachment_optimal;
			PipelineStage srcStage = PipelineStages::topOfPipe;
			PipelineStage dstStage = PipelineStages::earlyFragmentTest;
			AccessMask srcAccessMask = AccessMasks::TopOfPipe::none;
			AccessMask dstAccessMask = AccessMasks::EarlyFragmentTest::depthStencilAttachmentRead;
			m_pDepthImages[frameIndex]->TransitionLayout(newLayout, srcStage, dstStage, srcAccessMask, dstAccessMask);
		}
	}
	void ForwardRenderPass::CreateFrameBuffers()
	{
		size_t imageCount = Context::GetFramesInFlight();
		m_framebuffers.resize(imageCount);
		std::array<VkImageView, 3> attachments;

		for (size_t i = 0; i < imageCount; i++)
		{
			// order of attachments is important!
			attachments[0] = m_pMsaaImages[i]->GetVkImageView();
			attachments[1] = m_pDepthImages[i]->GetVkImageView();
			attachments[2] = m_pRenderTextures[i]->GetVmaImage()->GetVkImageView();

			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_pRenderTextures[i]->GetWidth();
			framebufferInfo.height = m_pRenderTextures[i]->GetHeight();
			framebufferInfo.layers = 1;
			vkCreateFramebuffer(Context::GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]);
		}
	}
}