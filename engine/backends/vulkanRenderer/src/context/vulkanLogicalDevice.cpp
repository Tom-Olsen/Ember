#include "vulkanLogicalDevice.h"
#include "logger.h"
#include "vulkanDeviceQueue.h"
#include "vulkanMacros.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"
#include <assert.h>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	LogicalDevice::LogicalDevice(PhysicalDevice* pPhysicalDevice, Surface* pSurface, std::vector<const char*>& deviceExtensions)
	{
		// Assertions:
		assert(pPhysicalDevice != nullptr);
		assert(pSurface != nullptr);
		//PrintQueueFamilyInfo(pPhysicalDevice->GetVkPhysicalDevice(), pSurface->GetVkSurfaceKHR());

		// Find queue family indices:
		auto [queueFamilyIndex, queueCount] = FindMainQueueFamilyIndex(pPhysicalDevice->GetVkPhysicalDevice(), pSurface->GetVkSurfaceKHR());

		// Queue priorities:
		std::vector<float> queuePriorities(queueCount, 1.0f);

		bool enableExtendedDynamicStateExtension = false;
		for (const char* extension : deviceExtensions)
			enableExtendedDynamicStateExtension |= std::strcmp(extension, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME) == 0;

		// Vector of queue create infos:
		VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = static_cast<uint32_t>(queuePriorities.size());
		queueCreateInfo.pQueuePriorities = queuePriorities.data();

		// Sync2 feature: Allows for src/dst stage to be 0 (VK_PIPELINE_STAGE_NONE).
		VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR };
		sync2Features.synchronization2 = VK_TRUE;

		// Timeline semaphores track completion independently on each async compute queue.
		VkPhysicalDeviceTimelineSemaphoreFeatures timelineFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES };
		timelineFeatures.pNext = &sync2Features;
		timelineFeatures.timelineSemaphore = VK_TRUE;

		// Scalar block layout: Allows packed Float3 buffers to use the same 12 byte stride in C++ and shaders.
		VkPhysicalDeviceScalarBlockLayoutFeatures scalarBlockLayoutFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES };
		scalarBlockLayoutFeatures.pNext = &timelineFeatures;
		scalarBlockLayoutFeatures.scalarBlockLayout = VK_TRUE;

		// Extended dynamic state: Allows dynamic cull mode on Vulkan 1.2 extension paths.
		VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT };
		extendedDynamicStateFeatures.pNext = &scalarBlockLayoutFeatures;
		extendedDynamicStateFeatures.extendedDynamicState = VK_TRUE;

		// Choose which features to enable:
		VkPhysicalDeviceFeatures2 deviceFeatures2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
		if (enableExtendedDynamicStateExtension)
			deviceFeatures2.pNext = &extendedDynamicStateFeatures;  // contains &scalarBlockLayoutFeatures as pnext.
		else
			deviceFeatures2.pNext = &scalarBlockLayoutFeatures;     // must be added manually if not forwarded by enableExtendedDynamicStateExtension.
		deviceFeatures2.features.samplerAnisotropy = VK_TRUE;
		deviceFeatures2.features.depthClamp = pPhysicalDevice->SupportsDepthClamp();
		deviceFeatures2.features.depthBiasClamp = pPhysicalDevice->SupportsDepthBiasClamp();
		deviceFeatures2.features.fillModeNonSolid = pPhysicalDevice->SupportsFillModeNonSolid();
		deviceFeatures2.features.shaderStorageImageExtendedFormats = pPhysicalDevice->SupportsStorageImageExtendedFormats();

		// Device create info:
		VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pNext = &deviceFeatures2;
		VKA(vkCreateDevice(pPhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));

		// Create device queues:
		try
		{
			m_deviceQueues.reserve(queueCount);
			for (uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++)
				m_deviceQueues.push_back(std::make_unique<DeviceQueue>(m_device, queueFamilyIndex, queueIndex));

			// Assign queue roles explicitly for each supported queue count.
			if (queueCount >= 4)
			{
				m_pGraphicsQueue = m_deviceQueues[0].get();
				m_pPresentQueue = m_deviceQueues[1].get();
				m_pTransferQueue = m_deviceQueues[2].get();
				m_computeQueues.reserve(queueCount - 3);
				for (uint32_t queueIndex = 3; queueIndex < queueCount; queueIndex++)
					m_computeQueues.push_back(m_deviceQueues[queueIndex].get());
			}
			else if (queueCount >= 2)
			{
				m_pGraphicsQueue = m_deviceQueues[0].get();
				m_pPresentQueue = m_pGraphicsQueue;
				m_pTransferQueue = m_pGraphicsQueue;
				m_computeQueues.reserve(queueCount - 1);
				for (uint32_t queueIndex = 1; queueIndex < queueCount; queueIndex++)
					m_computeQueues.push_back(m_deviceQueues[queueIndex].get());
			}
			else
			{
				LOG_WARN("Your device exposes one queue. Graphics, presentation, transfer, and async compute share it.");
				m_pGraphicsQueue = m_deviceQueues[0].get();
				m_pPresentQueue = m_pGraphicsQueue;
				m_pTransferQueue = m_pGraphicsQueue;
				m_computeQueues.push_back(m_pGraphicsQueue);
			}
		}
		catch (...)
		{
			Cleanup();
			throw;
		}
	}
	LogicalDevice::~LogicalDevice()
	{
		Cleanup();
	}


	// Movable:
	LogicalDevice::LogicalDevice(LogicalDevice&& other) noexcept
	{
		MoveFrom(other);
	}
	LogicalDevice& LogicalDevice::operator=(LogicalDevice&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methods:
	// Getters:
	const VkDevice& LogicalDevice::GetVkDevice() const
	{
		return m_device;
	}
	const DeviceQueue& LogicalDevice::GetGraphicsQueue() const
	{
		if (m_pGraphicsQueue == nullptr)
			throw std::runtime_error("LogicalDevice has no graphics queue.");
		return *m_pGraphicsQueue;
	}
	const DeviceQueue& LogicalDevice::GetPresentQueue() const
	{
		if (m_pPresentQueue == nullptr)
			throw std::runtime_error("LogicalDevice has no present queue.");
		return *m_pPresentQueue;
	}
	const DeviceQueue& LogicalDevice::GetTransferQueue() const
	{
		if (m_pTransferQueue == nullptr)
			throw std::runtime_error("LogicalDevice has no transfer queue.");
		return *m_pTransferQueue;
	}
	const std::vector<DeviceQueue*>& LogicalDevice::GetComputeQueues() const
	{
		if (m_computeQueues.empty())
			throw std::runtime_error("LogicalDevice::GetComputeQueues() has no compute queues.");
		return m_computeQueues;
	}



	// Private methods:
	void LogicalDevice::Cleanup()
	{
		if (m_device == VK_NULL_HANDLE)
			return;
		VKA(vkDeviceWaitIdle(m_device));
		m_pGraphicsQueue = nullptr;
		m_pPresentQueue = nullptr;
		m_pTransferQueue = nullptr;
		m_computeQueues.clear();
		m_deviceQueues.clear();
		vkDestroyDevice(m_device, nullptr);
		m_device = VK_NULL_HANDLE;
	}
	void LogicalDevice::MoveFrom(LogicalDevice& other) noexcept
	{
		m_device = std::exchange(other.m_device, VK_NULL_HANDLE);
		m_deviceQueues = std::move(other.m_deviceQueues);
		m_pGraphicsQueue = std::exchange(other.m_pGraphicsQueue, nullptr);
		m_pPresentQueue = std::exchange(other.m_pPresentQueue, nullptr);
		m_pTransferQueue = std::exchange(other.m_pTransferQueue, nullptr);
		m_computeQueues = std::move(other.m_computeQueues);
		other.m_deviceQueues.clear();
		other.m_computeQueues.clear();
	}
	std::pair<uint32_t, uint32_t> LogicalDevice::FindMainQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR vkSurfaceKHR) const
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
				VkBool32 supportsPresent = VK_FALSE;
				VKA(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, queueFamilyIndex, vkSurfaceKHR, &supportsPresent));
				if (supportsPresent == VK_TRUE)
					return { queueFamilyIndex, queueFamilyProperty.queueCount };
			}
		}

		throw std::runtime_error("No main queue family found with graphics, compute, transfer, and presentation support for the surface.");
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