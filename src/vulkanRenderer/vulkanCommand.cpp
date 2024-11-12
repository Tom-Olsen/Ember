#include "vulkanCommand.h"
#include "vulkanMacros.h"



// Constructor:
VulkanCommand::VulkanCommand(VulkanContext* context, VulkanQueue queue)
{
	this->context = context;

	VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	//createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	// required for vkResetCommandBuffer(buffer,0) (not recommended)
	createInfo.queueFamilyIndex = queue.familyIndex;	// bind to correct queue family.
	VKA(vkCreateCommandPool(context->LogicalDevice(), &createInfo, nullptr, &pool));
	
	VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	allocateInfo.commandPool = pool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Can be submitted to a queue for execution, but cannot be called from other command buffers.
	//allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;	// Cannot be submitted directly, but can be called from primary command buffers.
	allocateInfo.commandBufferCount = 1;
	VKA(vkAllocateCommandBuffers(context->LogicalDevice(), &allocateInfo, &buffer));
}



// Destructor:
VulkanCommand::~VulkanCommand()
{
	vkDestroyCommandPool(context->LogicalDevice(), pool, nullptr);
}



// Public static methods:
VulkanCommand VulkanCommand::BeginSingleTimeCommand(VulkanContext* context, const VulkanQueue& queue)
{
	VulkanCommand command(context, queue);
	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VKA(vkBeginCommandBuffer(command.buffer, &beginInfo));
	return command;
}
void VulkanCommand::EndSingleTimeCommand(VulkanContext* context, const VulkanCommand& command, const VulkanQueue& queue)
{
	vkEndCommandBuffer(command.buffer);
	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command.buffer;
	VKA(vkQueueSubmit(queue.queue, 1, &submitInfo, VK_NULL_HANDLE));
	VKA(vkQueueWaitIdle(queue.queue));
}