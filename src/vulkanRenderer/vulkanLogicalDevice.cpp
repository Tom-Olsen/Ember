#include "vulkanLogicalDevice.h"
#include "vulkanInstance.h"
#include "vulkanMacros.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"
#include <set>



namespace emberEngine
{
	// Constructor/Destructor:
	VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice* pPhysicalDevice, VulkanSurface* pSurface, std::vector<const char*> deviceExtensions)
	{
		// Choose which features to enable:
		VkPhysicalDeviceFeatures enabledFearutes = {};
		enabledFearutes.samplerAnisotropy = VK_TRUE;
		enabledFearutes.depthClamp = pPhysicalDevice->SupportsDepthClamp();

		// Find queue family indices:
		m_graphicsQueue.familyIndex = FindGraphicsAndComputeQueueFamilyIndex(pPhysicalDevice->GetVkPhysicalDevice());
		m_presentQueue.familyIndex = FindPresentQueueFamilyIndex(pPhysicalDevice->GetVkPhysicalDevice(), pSurface->GetVkSurfaceKHR());
		m_computeQueue.familyIndex = FindPureComputeQueueFamilyIndex(pPhysicalDevice->GetVkPhysicalDevice());
		m_transferQueue.familyIndex = FindPureTransferQueueFamilyIndex(pPhysicalDevice->GetVkPhysicalDevice());

		// Fallbacks:
		if (m_presentQueue.familyIndex == -1)
			m_presentQueue.familyIndex = m_graphicsQueue.familyIndex;
		if (m_computeQueue.familyIndex == -1)
			m_computeQueue.familyIndex = m_graphicsQueue.familyIndex;
		if (m_transferQueue.familyIndex == -1)
			m_transferQueue.familyIndex = m_graphicsQueue.familyIndex;

		// Vector of queue create infos:
		float queuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { m_graphicsQueue.familyIndex, m_presentQueue.familyIndex, m_computeQueue.familyIndex, m_transferQueue.familyIndex };
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
		VKA(vkCreateDevice(pPhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));

		// Aquire queues:
		vkGetDeviceQueue(m_device, m_graphicsQueue.familyIndex, 0, &m_graphicsQueue.queue);
		vkGetDeviceQueue(m_device, m_presentQueue.familyIndex, 0, &m_presentQueue.queue);
		vkGetDeviceQueue(m_device, m_computeQueue.familyIndex, 0, &m_computeQueue.queue);
		vkGetDeviceQueue(m_device, m_transferQueue.familyIndex, 0, &m_transferQueue.queue);
	}
	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		VKA(vkDeviceWaitIdle(m_device));
		vkDestroyDevice(m_device, nullptr);
	}



	// Public methods:
	const VkDevice& VulkanLogicalDevice::GetVkDevice() const
	{
		return m_device;
	}
	const VulkanQueue& VulkanLogicalDevice::GetGraphicsQueue() const
	{
		return m_graphicsQueue;
	}
	const VulkanQueue& VulkanLogicalDevice::GetPresentQueue() const
	{
		return m_presentQueue;
	}
	const VulkanQueue& VulkanLogicalDevice::GetComputeQueue() const
	{
		return m_computeQueue;
	}
	const VulkanQueue& VulkanLogicalDevice::GetTransferQueue() const
	{
		return m_transferQueue;
	}



	// Private methods:
	uint32_t VulkanLogicalDevice::FindGraphicsAndComputeQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice) const
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

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

	uint32_t VulkanLogicalDevice::FindPresentQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR) const
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		uint32_t presentQueueFamilyIndex = -1;
		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			VkBool32 presentSupport = false;
			VKA(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurfaceKHR, &presentSupport));
			if (queueFamilyProperties[i].queueCount > 0 && presentSupport)
				return i;
		}

		return -1;
	}

	uint32_t VulkanLogicalDevice::FindPureComputeQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice) const
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

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

	uint32_t VulkanLogicalDevice::FindPureTransferQueueFamilyIndex(VkPhysicalDevice physicalDevice) const
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
}