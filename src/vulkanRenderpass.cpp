#include "logger.h"
#include "macros.h"
#include "vulkanRenderpass.h"
#include "vulkanDepthImage.h"



VulkanRenderpass::VulkanRenderpass(VulkanLogicalDevice* logicalDevice, VkFormat format)
{
	this->logicalDevice = logicalDevice;

	// Define attachments:
	std::array<VkAttachmentDescription, 2> attachments;
	{
		VkAttachmentDescription colorAttachmentDescription = {};
		colorAttachmentDescription.format = format;
		colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;			// No multisampling yet
		colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;	// clear framebuffer to black before rendering
		colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;	// store rendered image in framebuffer
		colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;		// do not use stencils
		colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;	// do not use stencils
		colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		// we don't care about initial layout of the image
		colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;	// image will be presented to the screen, so use swapchain layout

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = VulkanDepthImage::format;					// must be same as depth image format
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;					// No multisampling yet
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;				// clear depth buffer before rendering
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;			// depth content is discarded after rendering
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;	// stencil part not used yet
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;	// stencil part not used yet
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachments = { colorAttachmentDescription, depthAttachment };
	}

	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 0;	// index of the attachment in the attachment array
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;			// index of the attachment in the attachment array
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // vulkan may also support compute subpasses in the future
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
	createInfo.pAttachments = attachments.data();
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	VKA(vkCreateRenderPass(logicalDevice->device, &createInfo, nullptr, &renderpass));
}

VulkanRenderpass::~VulkanRenderpass()
{
	vkDestroyRenderPass(logicalDevice->device, renderpass, nullptr);
}