#include "vulkanContext.h"
#include <iostream>
#include "macros.h"



// Constructor:
VulkanContext::VulkanContext(std::vector<const char*> instanceExtensions, std::vector<const char*> deviceExtensions)
{
	instance = std::make_unique<VulkanInstance>(instanceExtensions);
	physicalDevice = std::make_unique<VulkanPhysicalDevice>(instance.get());
	logicalDevice = std::make_unique<VulkanLogicalDevice>(physicalDevice.get(), deviceExtensions);
}



// Destructor:
VulkanContext::~VulkanContext()
{
	VKA(vkDeviceWaitIdle(logicalDevice->device));
}