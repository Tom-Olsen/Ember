#include "vulkanLogicalDevice.h"
#include <iostream>
#include "macros.h"



// Constructor:
VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, std::vector<const char*> deviceExtensions)
{
	// Choose which features to enable:
	VkPhysicalDeviceFeatures enabledFearutes{};

	// Find queue family indices:
	uint32_t graphicsQueueFamilyIndex = FindGraphicsAndComputeQueueFamilyIndex(physicalDevice->device);
	uint32_t presentQueueFamilyIndex = FindPresentQueueFamilyIndex(physicalDevice->device, surface->surface);
	uint32_t computeQueueFamilyIndex = FindPureComputeQueueFamilyIndex(physicalDevice->device);
	if (computeQueueFamilyIndex == -1)
		computeQueueFamilyIndex = graphicsQueueFamilyIndex;

	// Vector of queue create infos:
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamilyIndex, presentQueueFamilyIndex, computeQueueFamilyIndex };
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
	graphicsQueue.familyIndex = graphicsQueueFamilyIndex;
	vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue.queue);
	vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue.queue);
	vkGetDeviceQueue(device, computeQueueFamilyIndex, 0, &computeQueue.queue);
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