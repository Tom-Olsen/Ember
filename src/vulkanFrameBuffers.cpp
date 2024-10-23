#include "VulkanFrameBuffers.h"
#include <stdexcept>
#include "macros.h"



// Constructor:
VulkanFrameBuffers::VulkanFrameBuffers(VulkanContext* context, VulkanDepthImage* depthImage, VulkanMsaaImage* msaaImage)
{
	this->context = context;
	size_t size = context->swapchain->images.size();

	// Clear old framebuffers if they exist:
	if (framebuffers.size() == size)
	{
		for (uint32_t i = 0; i < size; i++)
			vkDestroyFramebuffer(context->LogicalDevice(), framebuffers[i], nullptr);
		framebuffers.clear();
	}

	VkExtent2D extent = context->surface->CurrentExtent();

	framebuffers.resize(size);
	std::array<VkImageView, 3> attachments;
	for (size_t i = 0; i < size; i++)
	{
		// order of attachments is important!
		attachments[0] = msaaImage->image->imageView;
		attachments[1] = depthImage->image->imageView;
		attachments[2] = context->swapchain->imageViews[i];

		VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferInfo.renderPass = context->Renderpass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;
		vkCreateFramebuffer(context->LogicalDevice(), &framebufferInfo, nullptr, &framebuffers[i]);
	}
}



// Destructor:
VulkanFrameBuffers::~VulkanFrameBuffers()
{
	for (uint32_t i = 0; i < context->swapchain->images.size(); i++)
		vkDestroyFramebuffer(context->LogicalDevice(), framebuffers[i], nullptr);
	framebuffers.clear();
}



// Public:
VkFramebuffer& VulkanFrameBuffers::operator[](size_t index)
{ return framebuffers[index]; }