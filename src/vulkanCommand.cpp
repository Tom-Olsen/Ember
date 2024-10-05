#include "vulkanCommand.h"
#include "macros.h"



// Constructor:
VulkanCommand::VulkanCommand(VulkanLogicalDevice* logicalDevice, VulkanQueue queue)
{
	this->logicalDevice = logicalDevice;

	VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	//createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;	// Command buffer will be short-lived. 
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	// Command buffer will be reset after each use (frame).
	createInfo.queueFamilyIndex = queue.familyIndex;	// bind to correct queue family.
	VKA(vkCreateCommandPool(logicalDevice->device, &createInfo, nullptr, &pool));
	
	VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	allocateInfo.commandPool = pool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Can be submitted to a queue for execution, but cannot be called from other command buffers.
	//allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;	// Cannot be submitted directly, but can be called from primary command buffers.
	allocateInfo.commandBufferCount = 1;
	VKA(vkAllocateCommandBuffers(logicalDevice->device, &allocateInfo, &buffer));
}



// Destructor:
VulkanCommand::~VulkanCommand()
{
	vkDestroyCommandPool(logicalDevice->device, pool, nullptr);
}