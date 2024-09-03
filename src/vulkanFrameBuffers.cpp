#include "VulkanFrameBuffers.h"
#include <stdexcept>
#include "macros.h"



// Constructor:
VulkanFrameBuffers::VulkanFrameBuffers(VulkanLogicalDevice* logicalDevice, VulkanSurface* surface, VulkanSwapchain* swapchain, VulkanRenderpass* renderpass)
{
	this->size = swapchain->images.size();
	this->logicalDevice = logicalDevice;
	this->surface = surface;
	this->renderpass = renderpass;
	VkExtent2D extent = surface->CurrentExtent();

	frameBuffers.resize(size);
	for (uint32_t i = 0; i < size; i++)
	{
		VkFramebufferCreateInfo createinfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		createinfo.renderPass = renderpass->renderpass;
		createinfo.attachmentCount = 1;
		createinfo.pAttachments = &swapchain->imageViews[i];
		createinfo.width = extent.width;
		createinfo.height = extent.height;
		createinfo.layers = 1;
		VKA(vkCreateFramebuffer(logicalDevice->device, &createinfo, nullptr, &frameBuffers[i]));
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
void VulkanFrameBuffers::Recreate(VulkanSwapchain* swapchain)
{
	for (uint32_t i = 0; i < size; i++)
		vkDestroyFramebuffer(logicalDevice->device, frameBuffers[i], nullptr);
	frameBuffers.clear();

	this->size = swapchain->images.size();
	this->logicalDevice = logicalDevice;
	VkExtent2D extent = surface->CurrentExtent();

	frameBuffers.resize(size);
	for (uint32_t i = 0; i < size; i++)
	{
		VkFramebufferCreateInfo createinfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		createinfo.renderPass = renderpass->renderpass;
		createinfo.attachmentCount = 1;
		createinfo.pAttachments = &swapchain->imageViews[i];
		createinfo.width = extent.width;
		createinfo.height = extent.height;
		createinfo.layers = 1;
		vkCreateFramebuffer(logicalDevice->device, &createinfo, nullptr, &frameBuffers[i]);
	}
}

VkFramebuffer& VulkanFrameBuffers::operator[](size_t index)
{ return frameBuffers[index]; }