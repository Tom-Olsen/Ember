#include "logger.h"
#include "macros.h"
#include "vulkanrenderpass.h"



VulkanRenderpass::VulkanRenderpass(VulkanLogicalDevice* logicalDevice, VkFormat format)
{
	this->logicalDevice = logicalDevice;

	VkAttachmentDescription colorAttachmentDescription = {};
	colorAttachmentDescription.format = format;
	colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;

	VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &colorAttachmentDescription;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;

	vkCreateRenderPass(logicalDevice->device, &createInfo, nullptr, &renderpass);
}

VulkanRenderpass::~VulkanRenderpass()
{
	vkDestroyRenderPass(logicalDevice->device, renderpass, nullptr);
}