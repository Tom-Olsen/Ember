#include "VulkanFrameBuffers.h"
#include <stdexcept>
#include "macros.h"



// Constructor:
VulkanFrameBuffers::VulkanFrameBuffers(VulkanLogicalDevice* logicalDevice, VulkanSurface* surface, VulkanSwapchain* swapchain, VulkanRenderpass* renderpass, VulkanDepthImage* depthImage)
{
	this->logicalDevice = logicalDevice;
	this->surface = surface;

	Recreate(swapchain, renderpass, depthImage);
}



// Destructor:
VulkanFrameBuffers::~VulkanFrameBuffers()
{
	for (uint32_t i = 0; i < size; i++)
		vkDestroyFramebuffer(logicalDevice->device, frameBuffers[i], nullptr);
	frameBuffers.clear();
}



// Public:
void VulkanFrameBuffers::Recreate(VulkanSwapchain* swapchain, VulkanRenderpass* renderpass, VulkanDepthImage* depthImage)
{
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
	std::array<VkImageView, 2> attachments;
	for (uint32_t i = 0; i < size; i++)
	{
		attachments[0] = swapchain->imageViews[i];
		attachments[1] = depthImage->imageView;

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

VkFramebuffer& VulkanFrameBuffers::operator[](size_t index)
{ return frameBuffers[index]; }