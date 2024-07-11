#include "vulkanContext.h"
#include <iostream>
#include <vector>
#include "macros.h"
#include "logger.h"

#ifdef NDEBUG	// release mode
const bool enableValidationLayers = false;
#else			// debug mode
const bool enableValidationLayers = true;
#endif




// Constructor:
VulkanContext::VulkanContext(uint32_t instanceExtensionCount, const char* const* instanceExtensions, uint32_t deviceExtensionCount, const char** deviceExtensions)
{
	CreateInstance(instanceExtensionCount, instanceExtensions);
	SelectPhysicalDevice();
	CreateDevice(deviceExtensionCount, deviceExtensions);
}



// Destructor:
VulkanContext::~VulkanContext()
{
	VKA(vkDeviceWaitIdle(device));
	if (device != nullptr)
		vkDestroyDevice(device, nullptr);
	if (instance != nullptr)
		vkDestroyInstance(instance, nullptr);
}



// Internal:
void VulkanContext::CreateInstance(uint32_t instanceExtensionCount, const char* const* instanceExtensions)
{
	std::vector<const char*> enabledLayers;
	enabledLayers.push_back("VK_LAYER_KHRONOS_validation");

	VkApplicationInfo applicationInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	applicationInfo.pApplicationName = "My Engine";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	createInfo.pApplicationInfo = &applicationInfo;
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
		createInfo.ppEnabledLayerNames = enabledLayers.data();
	}
	createInfo.enabledExtensionCount = instanceExtensionCount;
	createInfo.ppEnabledExtensionNames = instanceExtensions;

	// Create the Vulkan instance:
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create instance!");
}

void VulkanContext::SelectPhysicalDevice()
{
	uint32_t numPhysicalDevices = 0;
	VKA(vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr));
	if (numPhysicalDevices == 0)
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> physicalDevices(numPhysicalDevices);
	VKA(vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevices.data()));

	// TODO: logic to pick deticated GPU and use integrated GPU as fallback
	for (const VkPhysicalDevice& device : physicalDevices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
}

uint32_t VulkanContext::FindFirstGraphicsQueueFamilyIndex()
{
	// Find first family queue with graphics support:
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
	uint32_t graphicsQueueFamilyIndex = -1;
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		VkQueueFamilyProperties queueFamilyProperty = queueFamilyProperties[i];
		if (queueFamilyProperty.queueCount > 0 && (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			graphicsQueueFamilyIndex = i;
			break;
		}
	}
	if (graphicsQueueFamilyIndex == -1)
		throw std::runtime_error("Failed to find graphics queue!");

	return graphicsQueueFamilyIndex;
}

void VulkanContext::CreateDevice(uint32_t deviceExtensionCount, const char** deviceExtensions)
{
	// Choose which features to enable:
	VkPhysicalDeviceFeatures enabledFearutes{};	

	// Create queue create info:
	uint32_t graphicsQueueFamilyIndex = FindFirstGraphicsQueueFamilyIndex();
	VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = new float[1] { 1.0f };

	// Create device create info:
	VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = deviceExtensionCount;
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
	deviceCreateInfo.pEnabledFeatures = &enabledFearutes;
	VKA(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

	// Aquire queues:
	graphicsQueue.familyIndex = graphicsQueueFamilyIndex;
	vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue.queue);
}

bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	return true;
	//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
}