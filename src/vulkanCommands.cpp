#include "vulkanCommands.h"
#include "macros.h"



// Constructor:
VulkanCommands::VulkanCommands(uint32_t size, VulkanLogicalDevice* logicalDevice, VulkanQueue queue) : size(size)
{
	this->logicalDevice = logicalDevice;
	pools.resize(size);
	buffers.resize(size);

	for (uint32_t i = 0; i < size; i++)
	{
		VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		//createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;	// Command buffers will be short-lived. 
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	// Command buffers will be reset after each use (frame).
		createInfo.queueFamilyIndex = queue.familyIndex;	// bind to correct queue family.
		VKA(vkCreateCommandPool(logicalDevice->device, &createInfo, nullptr, &pools[i]));

		VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = pools[i];
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Can be submitted to a queue for execution, but cannot be called from other command buffers.
		//allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;	// Cannot be submitted directly, but can be called from primary command buffers.
		allocateInfo.commandBufferCount = 1;
		VKA(vkAllocateCommandBuffers(logicalDevice->device, &allocateInfo, &buffers[i]));
	}
}



// Destructor:
VulkanCommands::~VulkanCommands()
{
	for (uint32_t i = 0; i < size; i++)
		vkDestroyCommandPool(logicalDevice->device, pools[i], nullptr);
}