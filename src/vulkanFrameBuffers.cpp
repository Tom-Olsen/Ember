#include "VulkanFrameBuffers.h"
#include <stdexcept>
#include "macros.h"



// Constructor:
VulkanFrameBuffers::VulkanFrameBuffers(VulkanContext* context, VulkanDepthImage* depthImage, VulkanMsaaImage* msaaImage)
{
	this->context = context;
	this->size = context->swapchain->images.size();

	// Clear old frameBuffers if they exist:
	if (frameBuffers.size() == size)
	{
		for (uint32_t i = 0; i < size; i++)
			vkDestroyFramebuffer(context->LogicalDevice(), frameBuffers[i], nullptr);
		frameBuffers.clear();
	}

	VkExtent2D extent = context->surface->CurrentExtent();

	frameBuffers.resize(size);
	std::array<VkImageView, 3> attachments;
	for (uint32_t i = 0; i < size; i++)
	{
		// order of attachments is important!
		attachments[0] = msaaImage->image->imageView;
		attachments[1] = depthImage->image->imageView;
		attachments[2] = context->swapchain->imageViews[i];

		VkFramebufferCreateInfo createinfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		createinfo.renderPass = context->Renderpass();
		createinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createinfo.pAttachments = attachments.data();
		createinfo.width = extent.width;
		createinfo.height = extent.height;
		createinfo.layers = 1;
		vkCreateFramebuffer(context->LogicalDevice(), &createinfo, nullptr, &frameBuffers[i]);
	}
}



// Destructor:
VulkanFrameBuffers::~VulkanFrameBuffers()
{
	for (uint32_t i = 0; i < size; i++)
		vkDestroyFramebuffer(context->LogicalDevice(), frameBuffers[i], nullptr);
	frameBuffers.clear();
}



// Public:
VkFramebuffer& VulkanFrameBuffers::operator[](size_t index)
{ return frameBuffers[index]; }