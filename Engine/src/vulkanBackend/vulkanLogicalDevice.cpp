#include "vulkanLogicalDevice.h"
#include "vulkanInstance.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"
#include <assert.h>
#include <set>
#include <sstream>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		LogicalDevice::LogicalDevice()
		{
			m_device = VK_NULL_HANDLE;
			m_graphicsQueue = { VK_NULL_HANDLE, 0};
			m_presentQueue  = { VK_NULL_HANDLE, 0};
			m_computeQueue  = { VK_NULL_HANDLE, 0};
			m_transferQueue = { VK_NULL_HANDLE, 0};
		}
		void LogicalDevice::Init(PhysicalDevice* pPhysicalDevice, Surface* pSurface, std::vector<const char*> deviceExtensions)
		{
			// Assertions:
			assert(pPhysicalDevice != nullptr);
			assert(pSurface != nullptr);
			//PrintQueueFamilyInfo(pPhysicalDevice->GetVkPhysicalDevice(), pSurface->GetVkSurfaceKHR());

			// Find queue family indices:
			auto [queueFamilyIndex, queueCount] = FindGraphicsComputeTransferQueueFamilyIndex(pPhysicalDevice->GetVkPhysicalDevice(), pSurface->GetVkSurfaceKHR());
			m_graphicsQueue.familyIndex = queueFamilyIndex;
			m_presentQueue.familyIndex = queueFamilyIndex;
			m_computeQueue.familyIndex = queueFamilyIndex;
			m_transferQueue.familyIndex = queueFamilyIndex;

			// Vector of queue create infos:
			VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			if (queueCount >= 3)
			{
				float queuePriorities[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				queueCreateInfo.queueCount = 4;
				queueCreateInfo.pQueuePriorities = queuePriorities;
			}
			else
			{
				float queuePriorities[1] = { 1.0f };
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = queuePriorities;
			}

			// Sync2 featur: Allows for src/dst stage to be 0 (VK_PIPELINE_STAGE_NONE).
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
			deviceCreateInfo.queueCreateInfoCount = 1;
			deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
			deviceCreateInfo.pNext = &deviceFeatures2;
			VKA(vkCreateDevice(pPhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));

			// Aquire queues:
			if (queueCount >= 3)
			{
				vkGetDeviceQueue(m_device, m_graphicsQueue.familyIndex, 0, &m_graphicsQueue.queue);
				vkGetDeviceQueue(m_device, m_presentQueue.familyIndex, 1, &m_presentQueue.queue);
				vkGetDeviceQueue(m_device, m_computeQueue.familyIndex, 2, &m_computeQueue.queue);
				vkGetDeviceQueue(m_device, m_transferQueue.familyIndex, 3, &m_transferQueue.queue);
			}
			else
			{
				LOG_WARN("Your device only supports a single queue. Async compute will not work.");
				vkGetDeviceQueue(m_device, m_graphicsQueue.familyIndex, 0, &m_graphicsQueue.queue);
				vkGetDeviceQueue(m_device, m_presentQueue.familyIndex, 0, &m_presentQueue.queue);
				vkGetDeviceQueue(m_device, m_computeQueue.familyIndex, 0, &m_computeQueue.queue);
				vkGetDeviceQueue(m_device, m_transferQueue.familyIndex, 0, &m_transferQueue.queue);
			}
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
		const DeviceQueue& LogicalDevice::GetGraphicsQueue() const
		{
			return m_graphicsQueue;
		}
		const DeviceQueue& LogicalDevice::GetPresentQueue() const
		{
			return m_presentQueue;
		}
		const DeviceQueue& LogicalDevice::GetComputeQueue() const
		{
			return m_computeQueue;
		}
		const DeviceQueue& LogicalDevice::GetTransferQueue() const
		{
			return m_transferQueue;
		}



		// Private methods:
		std::pair<uint32_t, uint32_t> LogicalDevice::FindGraphicsComputeTransferQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR) const
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

			for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
			{
				VkQueueFamilyProperties queueFamilyProperty = queueFamilyProperties[queueFamilyIndex];
				if (queueFamilyProperty.queueCount > 0
					&& (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					&& (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)
					&& (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT))
				{
					VkBool32 supportPresentMode = VK_FALSE;
					vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, queueFamilyIndex, vkSurfaceKHR, &supportPresentMode);
					if (supportPresentMode == VK_TRUE)
						return { queueFamilyIndex, queueFamilyProperty.queueCount };
				}
			}

			throw std::runtime_error("Terminating! No queue family found with GRAPHICS | COMPUTE | TRANSFER capabilities.");
			return  { -1, 0 };
		}
		void LogicalDevice::PrintQueueFamilyInfo(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR)
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

			std::ostringstream ss;
			ss << "Queue Family Info (" << queueFamilyCount << " families found):\n";

			for (uint32_t i = 0; i < queueFamilyCount; ++i)
			{
				const auto& props = queueFamilyProperties[i];
				ss << "Queue Family #" << i << ":\n";
				ss << "  Queue Count: " << props.queueCount << "\n";
				ss << "  Timestamp Valid Bits: " << props.timestampValidBits << "\n";
				ss << "  Min Image Transfer Granularity: "
					<< props.minImageTransferGranularity.width << "x"
					<< props.minImageTransferGranularity.height << "x"
					<< props.minImageTransferGranularity.depth << "\n";

				ss << "  Supported Flags:";
				if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					ss << " GRAPHICS";
				if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
					ss << " COMPUTE";
				if (props.queueFlags & VK_QUEUE_TRANSFER_BIT)
					ss << " TRANSFER";
				if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
					ss << " SPARSE_BINDING";
				if (props.queueFlags & VK_QUEUE_PROTECTED_BIT)
					ss << " PROTECTED";
				ss << "\n";

				VkBool32 presentSupport = false;
				VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurfaceKHR, &presentSupport);
				if (result == VK_SUCCESS)
					ss << "  Presentation Support: " << (presentSupport ? "Yes" : "No") << "\n";
				else
					ss << "  Presentation Support: Error (" << result << ")\n";
			}
			LOG_TRACE(ss.str());
		}
	}
}