#include "vulkanPhysicalDevice.h"
#include "logger.h"
#include "vulkanInstance.h"
#include "vulkanMacros.h"
#include <algorithm>
#include <assert.h>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	PhysicalDevice::PhysicalDevice(Instance* pInstance)
	{
		// Assertions:
		assert(pInstance != nullptr);

		uint32_t numPhysicalDevices = 0;
		VKA(vkEnumeratePhysicalDevices(pInstance->GetVkInstance(), &numPhysicalDevices, nullptr));
		if (numPhysicalDevices == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> physicalDevices(numPhysicalDevices);
		VKA(vkEnumeratePhysicalDevices(pInstance->GetVkInstance(), &numPhysicalDevices, physicalDevices.data()));

		// Score each device:
		std::vector<DeviceCandidate> devices;
		devices.reserve(numPhysicalDevices);
		for (const VkPhysicalDevice& device : physicalDevices)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			int score = DeviceScore(device);
			devices.push_back({ device, score, deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU });
		}

		// Sort compatible devices first, preferring discrete GPUs:
		auto SortLambda = [](const DeviceCandidate& a, const DeviceCandidate& b)
		{
			if ((a.score >= 0) != (b.score >= 0))
				return a.score >= 0;
			if (a.isDiscrete != b.isDiscrete)
				return a.isDiscrete;
			return a.score > b.score;
		};
		std::sort(devices.begin(), devices.end(), SortLambda);

		// Pick best device:
		if (devices[0].score < 0)
			throw std::runtime_error("No GPU supports all required capabilities!");
		m_physicalDevice = devices[0].device;

		// Determine max msaa samples:
		m_maxMsaaSamples = MaxUsableMsaaSampleCount();
	}
	PhysicalDevice::~PhysicalDevice()
	{
		Cleanup();
	}



	// Move semantics:
	PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
	{
		MoveFrom(other);
	}
	PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methdos:
	const VkPhysicalDevice& PhysicalDevice::GetVkPhysicalDevice() const
	{
		return m_physicalDevice;
	}
	VkSampleCountFlagBits PhysicalDevice::GetMaxMsaaSamples() const
	{
		return m_maxMsaaSamples;
	}
	bool PhysicalDevice::SupportsDepthClamp() const
	{
		return true;
	}
	bool PhysicalDevice::SupportsDepthBiasClamp() const
	{
		return true;
	}
	bool PhysicalDevice::SupportsFillModeNonSolid() const
	{
		return true;
	}
	bool PhysicalDevice::SupportsMultiViewport() const
	{
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &deviceFeatures);
		return deviceFeatures.multiViewport;
	}
	// Private:
	void PhysicalDevice::Cleanup()
	{

	}
	void PhysicalDevice::MoveFrom(PhysicalDevice& other) noexcept
	{
		m_physicalDevice = other.m_physicalDevice;
		m_maxMsaaSamples = other.m_maxMsaaSamples;

		other.m_physicalDevice = VK_NULL_HANDLE;
		other.m_maxMsaaSamples = VK_SAMPLE_COUNT_1_BIT;
	}
	int PhysicalDevice::DeviceScore(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		// Check required properties:
		if (HasGraphicsAndComputeQueueFamily(device) == false)
			return -1;

		// Check required features:
		bool supportsRequiredFeatures = true;
		supportsRequiredFeatures &= static_cast<bool>(deviceFeatures.samplerAnisotropy);
		supportsRequiredFeatures &= static_cast<bool>(deviceFeatures.depthClamp);
		supportsRequiredFeatures &= static_cast<bool>(deviceFeatures.depthBiasClamp);
		supportsRequiredFeatures &= static_cast<bool>(deviceFeatures.fillModeNonSolid);
		supportsRequiredFeatures &= DeviceSupportsScalarBlockLayout(device);
		if (supportsRequiredFeatures == false)
			return -1; // negative score = invalid device.

        // Get device memory:
		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);
		uint64_t deviceLocalMemoryMb = 0;
		for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++)
		{
			if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				deviceLocalMemoryMb += memoryProperties.memoryHeaps[i].size / (1024 * 1024);
		}

		int score = 1000;
		score += static_cast<int>(deviceLocalMemoryMb);
		score += static_cast<int>(deviceProperties.limits.maxImageDimension2D / 1024);
		score += static_cast<int>(deviceProperties.limits.maxSamplerAnisotropy);
		score += static_cast<int>(deviceProperties.limits.maxComputeSharedMemorySize / 1024);
		score += static_cast<int>(deviceProperties.limits.maxComputeWorkGroupInvocations / 32);
		score += 10 * deviceFeatures.multiViewport;
		// score +=  1 * deviceFeatures.shaderFloat64;
		// score +=  1 * deviceFeatures.shaderInt64;
		// score +=  1 * deviceFeatures.imageCubeArray;
		// score +=  1 * deviceFeatures.shaderStorageImageExtendedFormats;
		// score +=  1 * deviceFeatures.sparseBinding;
		// score +=  1 * deviceFeatures.drawIndirectFirstInstance;
		// score +=  1 * deviceFeatures.shaderStorageBufferArrayDynamicIndexing;

		return score;
	}
	bool PhysicalDevice::DeviceSupportsScalarBlockLayout(VkPhysicalDevice device) const
	{
		VkPhysicalDeviceScalarBlockLayoutFeatures scalarBlockLayoutFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES };
		VkPhysicalDeviceFeatures2 deviceFeatures2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
		deviceFeatures2.pNext = &scalarBlockLayoutFeatures;
		vkGetPhysicalDeviceFeatures2(device, &deviceFeatures2);
		return scalarBlockLayoutFeatures.scalarBlockLayout;
	}
	bool PhysicalDevice::HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device) const
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

		return false;
	}
	bool PhysicalDevice::HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface) const
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
	VkSampleCountFlagBits PhysicalDevice::MaxUsableMsaaSampleCount() const
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}
}