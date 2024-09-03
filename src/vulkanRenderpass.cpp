#include "logger.h"
#include "macros.h"
#include "vulkanrenderpass.h"



VulkanRenderpass::VulkanRenderpass(VulkanLogicalDevice* logicalDevice, VkFormat format)
{
	this->logicalDevice = logicalDevice;

	VkAttachmentDescription colorAttachmentDescription = {};
	colorAttachmentDescription.format = format;
	colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;			// No multisampling yet
	colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;	// clear framebuffer to black before rendering
	colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;	// store rendered image in framebuffer
	colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;		// do not use stencils
	colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;	// do not use stencils
	colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		// we don't care about initial layout of the image
	colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;	// image will be presented to the screen, so use swapchain layout

	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // vulkan may also support compute subpasses in the future
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &colorAttachmentDescription;
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