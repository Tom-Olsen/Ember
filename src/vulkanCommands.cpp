#include "vulkanCommands.h"
#include "macros.h"



// Constructor:
VulkanCommands::VulkanCommands(size_t size, VulkanLogicalDevice* logicalDevice) : size(size)
{
	this->logicalDevice = logicalDevice;
	pools.resize(size);
	buffers.resize(size);

	for (size_t i = 0; i < size; i++)
	{
		VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		createInfo.queueFamilyIndex = logicalDevice->graphicsQueue.familyIndex;
		VKA(vkCreateCommandPool(logicalDevice->device, &createInfo, nullptr, &pools[i]));

		VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = pools[i];
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;
		VKA(vkAllocateCommandBuffers(logicalDevice->device, &allocateInfo, &buffers[i]));
	}
}



// Destructor:
VulkanCommands::~VulkanCommands()
{
	for (size_t i = 0; i < size; i++)
		vkDestroyCommandPool(logicalDevice->device, pools[i], nullptr);
}