#include "vulkanPhysicalDevice.h"
#include <iostream>
#include "macros.h"



// Constructor:
VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanInstance* instance)
{
	uint32_t numPhysicalDevices = 0;
	VKA(vkEnumeratePhysicalDevices(instance->instance, &numPhysicalDevices, nullptr));
	if (numPhysicalDevices == 0)
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> physicalDevices(numPhysicalDevices);
	VKA(vkEnumeratePhysicalDevices(instance->instance, &numPhysicalDevices, physicalDevices.data()));

	// Score each device:
	std::vector<std::pair<VkPhysicalDevice, int>> devices;
	for (const VkPhysicalDevice& dev : physicalDevices)
	{
		int score = DeviceScore(dev);
		devices.push_back({ dev, score });
	}

	// Sort devices by score:
	auto SortLambda = [](const std::pair<VkPhysicalDevice, int>& a, const std::pair<VkPhysicalDevice, int>& b) { return a.second > b.second; };
	std::sort(devices.begin(), devices.end(), SortLambda);

	// Pick best device:
	if (devices[0].second < 0)
		throw std::runtime_error("Failed to find a suitable GPU!");
	device = devices[0].first;
}



// Private:
int VulkanPhysicalDevice::DeviceScore(VkPhysicalDevice dev)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(dev, &deviceProperties);
	vkGetPhysicalDeviceFeatures(dev, &deviceFeatures);

	// Check essential features:
	VkBool32 essentials = (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
	essentials &= HasGraphicsAndComputeQueueFamily(dev);
	essentials &= deviceFeatures.geometryShader;
	essentials &= deviceFeatures.tessellationShader;
	essentials &= deviceFeatures.wideLines;
	essentials &= deviceFeatures.samplerAnisotropy;
	essentials &= deviceFeatures.dualSrcBlend;
	essentials &= deviceFeatures.depthBiasClamp;
	essentials &= deviceFeatures.depthBounds;
	essentials &= deviceFeatures.shaderClipDistance;
	essentials &= deviceFeatures.shaderCullDistance;
	if (essentials == false)
		return -1; // negative score = invalid device.

	// Check optional features:
	int score = 0;
	score += 10 * deviceFeatures.multiViewport;
	// score +=  5 * deviceFeatures.fillModeNonSolid;
	// score +=  1 * deviceFeatures.shaderFloat64;
	// score +=  1 * deviceFeatures.shaderInt64;
	// score +=  1 * deviceFeatures.imageCubeArray;
	// score +=  1 * deviceFeatures.shaderStorageImageExtendedFormats;
	// score +=  1 * deviceFeatures.sparseBinding;
	// score +=  1 * deviceFeatures.drawIndirectFirstInstance;
	// score +=  1 * deviceFeatures.shaderStorageBufferArrayDynamicIndexing;
	
	return score;
}



VkBool32 VulkanPhysicalDevice::HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device)
{
	// Find queue family that supports Graphics and Compute.
	// This is needed for tightly integrating graphics shaders that rely on output data of compute shaders, e.g. LineRendererPro.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];
		if (queueFamilyProperty.queueCount > 0
			&& (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			&& (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT))
			return true;
	}
	throw std::runtime_error("Could not find a Family Queue that supports Graphics and Compute!");

	return false;
}
VkBool32 VulkanPhysicalDevice::HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	// Find queue family that supports presenting to the surface.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		VkBool32 supportsPresent = false;
		VKA(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent));
		if (supportsPresent)
			return true;
	}
	throw std::runtime_error("Could not find a Family Queue that supports Presenting!");

	return false;
}