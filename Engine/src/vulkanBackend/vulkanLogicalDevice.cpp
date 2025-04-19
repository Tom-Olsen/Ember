#include "vulkanLogicalDevice.h"
#include "vulkanInstance.h"
#include "vulkanMacros.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"
#include <set>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		LogicalDevice::LogicalDevice(PhysicalDevice* pPhysicalDevice, Surface* pSurface, std::vector<const char*> deviceExtensions)
		{
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

			// Sync2 feature. Allows for src/dst stage to be 0 (VK_PIPELINE_STAGE_NONE).
			VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR };
			sync2Features.synchronization2 = VK_TRUE;

			// Choose which features to enable:
			VkPhysicalDeviceFeatures2 deviceFeatures2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
			deviceFeatures2.pNext = &sync2Features;
			deviceFeatures2.features.samplerAnisotropy = VK_TRUE;
			deviceFeatures2.features.depthClamp = pPhysicalDevice->SupportsDepthClamp();
			deviceFeatures2.features.depthBiasClamp = pPhysicalDevice->SupportsDepthBiasClamp();

			// Device create info:
			VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
			deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
			deviceCreateInfo.pNext = &deviceFeatures2;
			VKA(vkCreateDevice(pPhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));

			// Aquire queues:
			vkGetDeviceQueue(m_device, m_graphicsQueue.familyIndex, 0, &m_graphicsQueue.queue);
			vkGetDeviceQueue(m_device, m_presentQueue.familyIndex, 0, &m_presentQueue.queue);
			vkGetDeviceQueue(m_device, m_computeQueue.familyIndex, 0, &m_computeQueue.queue);
			vkGetDeviceQueue(m_device, m_transferQueue.familyIndex, 0, &m_transferQueue.queue);
		}
		LogicalDevice::~LogicalDevice()
		{
			VKA(vkDeviceWaitIdle(m_device));
			vkDestroyDevice(m_device, nullptr);
		}



		// Public methods:
		const VkDevice& LogicalDevice::GetVkDevice() const
		{
			return m_device;
		}
		const DeviceQueue LogicalDevice::GetGraphicsQueue() const
		{
			return m_graphicsQueue;
		}
		const DeviceQueue LogicalDevice::GetPresentQueue() const
		{
			return m_presentQueue;
		}
		const DeviceQueue LogicalDevice::GetComputeQueue() const
		{
			return m_computeQueue;
		}
		const DeviceQueue LogicalDevice::GetTransferQueue() const
		{
			return m_transferQueue;
		}



		// Private methods:
		uint32_t LogicalDevice::FindGraphicsAndComputeQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice) const
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

		uint32_t LogicalDevice::FindPresentQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR) const
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

		uint32_t LogicalDevice::FindPureComputeQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice) const
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

		uint32_t LogicalDevice::FindPureTransferQueueFamilyIndex(VkPhysicalDevice physicalDevice) const
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
}