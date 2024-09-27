#include "VulkanFrameBuffers.h"
#include <stdexcept>
#include "macros.h"



// Constructor:
VulkanFrameBuffers::VulkanFrameBuffers(VulkanLogicalDevice* logicalDevice, VulkanSurface* surface, VulkanSwapchain* swapchain, VulkanRenderpass* renderpass, VulkanDepthImage* depthImage, VulkanMsaaImage* msaaImage)
{
	this->logicalDevice = logicalDevice;
	this->surface = surface;
	this->size = swapchain->images.size();
	this->renderpass = renderpass;

	// Clear old frameBuffers if they exist:
	if (frameBuffers.size() == size)
	{
		for (uint32_t i = 0; i < size; i++)
			vkDestroyFramebuffer(logicalDevice->device, frameBuffers[i], nullptr);
		frameBuffers.clear();
	}

	VkExtent2D extent = surface->CurrentExtent();

	frameBuffers.resize(size);
	std::array<VkImageView, 3> attachments;
	for (uint32_t i = 0; i < size; i++)
	{
		// order of attachments is important!
		attachments[0] = msaaImage->imageView;
		attachments[1] = depthImage->imageView;
		attachments[2] = swapchain->imageViews[i];

		VkFramebufferCreateInfo createinfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		createinfo.renderPass = renderpass->renderpass;
		createinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createinfo.pAttachments = attachments.data();
		createinfo.width = extent.width;
		createinfo.height = extent.height;
		createinfo.layers = 1;
		vkCreateFramebuffer(logicalDevice->device, &createinfo, nullptr, &frameBuffers[i]);
	}
}



// Destructor:
VulkanFrameBuffers::~VulkanFrameBuffers()
{
	for (uint32_t i = 0; i < size; i++)
		vkDestroyFramebuffer(logicalDevice->device, frameBuffers[i], nullptr);
	frameBuffers.clear();
}



// Public:
VkFramebuffer& VulkanFrameBuffers::operator[](size_t index)
{ return frameBuffers[index]; }