#include "logger.h"
#include "macros.h"
#include "vulkanRenderpass.h"
#include "vulkanDepthImage.h"



VulkanRenderpass::VulkanRenderpass(VulkanLogicalDevice* logicalDevice, VkFormat surfaceFormat, VkSampleCountFlagBits msaaSamples)
{
	this->logicalDevice = logicalDevice;

	// Define attachments:
	std::array<VkAttachmentDescription, 3> attachments{};
	{
		// Multisampled color attachment description:
		attachments[0].format = surfaceFormat;
		attachments[0].samples = msaaSamples;					// multisampling count
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;	// clear framebuffer to black before rendering
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;			// sno need to store multisampls after render
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;		// do not use stencils
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;	// do not use stencils
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;			// we don't care about initial layout of the image
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	// multisampled images are stored in color layout and not rdy for presenting yet

		// Depth attachment description:
		attachments[1].format = VulkanDepthImage::format;					// must be same as depth image format
		attachments[1].samples = msaaSamples;								// msaaSamples
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;				// clear depth buffer before rendering
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;			// depth content is discarded after rendering
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;	// stencil part not used yet
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;	// stencil part not used yet
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Color resolve attachment description: (resolve multisampled fragments)
		attachments[2].format = surfaceFormat;
		attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;	// rdy for presenting
	}

	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 0;	// index of the attachment in the attachment array
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = 1;			// index of the attachment in the attachment array
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorResolveAttachmentReference{};
	colorResolveAttachmentReference.attachment = 2;
	colorResolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // vulkan may also support compute subpasses in the future
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;
	subpass.pDepthStencilAttachment = &depthAttachmentReference;
	subpass.pResolveAttachments = &colorResolveAttachmentReference;

	//VkSubpassDependency dependency = {};
	//dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependency.dstSubpass = 0;
	//dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	//dependency.srcAccessMask = 0;
	//dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	//dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// Define synchronization dependencies of individual subpasses:
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;	// index of source subpass, where dependency originates. VK_SUBPASS_EXTERNAL = before renderpass
	dependency.dstSubpass = 0;	// index of destination subpass, where dependency ends.  VK_SUBPASS_EXTERNAL = after renderpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	// pipeline stages in source subpass which must complete before the dependency is resolved
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	// pipeline stages in destination subpass which must wait for the source stages to complete
	dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;	// types of memory accesses in the source subpass that must be completed
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;	// types of memory accesses in the destination subpass that must wait on the source subpass to complete
	dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;	// specify special behaviors

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