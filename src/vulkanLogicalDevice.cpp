#include "vulkanLogicalDevice.h"
#include <iostream>
#include "macros.h"



// Constructor:
VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, std::vector<const char*> deviceExtensions)
{
	// Choose which features to enable:
	VkPhysicalDeviceFeatures enabledFearutes = {};
	enabledFearutes.samplerAnisotropy = VK_TRUE;

	// Find queue family indices:
	graphicsQueue.familyIndex = FindGraphicsAndComputeQueueFamilyIndex(physicalDevice->device);
	presentQueue.familyIndex = FindPresentQueueFamilyIndex(physicalDevice->device, surface->surface);
	computeQueue.familyIndex = FindPureComputeQueueFamilyIndex(physicalDevice->device);
	transferQueue.familyIndex = FindPureTransferQueueFamilyIndex(physicalDevice->device);

	// Fallbacks:
	if (presentQueue.familyIndex == -1)
		presentQueue.familyIndex = graphicsQueue.familyIndex;
	if (computeQueue.familyIndex == -1)
		computeQueue.familyIndex = graphicsQueue.familyIndex;
	if (transferQueue.familyIndex == -1)
		transferQueue.familyIndex = graphicsQueue.familyIndex;

	// Vector of queue create infos:
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { graphicsQueue.familyIndex, presentQueue.familyIndex, computeQueue.familyIndex, transferQueue.familyIndex };
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Create device create info:
	VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFearutes;
	VKA(vkCreateDevice(physicalDevice->device, &deviceCreateInfo, nullptr, &device));

	// Aquire queues:
	vkGetDeviceQueue(device, graphicsQueue.familyIndex, 0, &graphicsQueue.queue);
	vkGetDeviceQueue(device, presentQueue.familyIndex, 0, &presentQueue.queue);
	vkGetDeviceQueue(device, computeQueue.familyIndex, 0, &computeQueue.queue);
	vkGetDeviceQueue(device, transferQueue.familyIndex, 0, &transferQueue.queue);
}



// Destructor:
VulkanLogicalDevice::~VulkanLogicalDevice()
{
	VKA(vkDeviceWaitIdle(device));
	vkDestroyDevice(device, nullptr);
}



// Private:
uint32_t VulkanLogicalDevice::FindGraphicsAndComputeQueueFamilyIndex(VkPhysicalDevice physicalDevice)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];
		if (queueFamilyProperty.queueCount > 0
			&& (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			&& (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT))
			return i;
	}

	return -1;
}

uint32_t VulkanLogicalDevice::FindPresentQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	uint32_t presentQueueFamilyIndex = -1;
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		VkBool32 presentSupport = false;
		VKA(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport));
		if (queueFamilyProperties[i].queueCount > 0 && presentSupport)
			return i;
	}

	return -1;
}

uint32_t VulkanLogicalDevice::FindPureComputeQueueFamilyIndex(VkPhysicalDevice physicalDevice)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];
		if (queueFamilyProperty.queueCount > 0
			&& (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)
			&& !(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			return i;
	}

	return -1;
}

uint32_t VulkanLogicalDevice::FindPureTransferQueueFamilyIndex(VkPhysicalDevice physicalDevice)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];
		if (queueFamilyProperty.queueCount > 0
			&& (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT)
			&& !(queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)
			&& !(queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			return i;
	}

	return -1;
}