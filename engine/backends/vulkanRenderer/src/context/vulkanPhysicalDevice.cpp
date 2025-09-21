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
			LOG_WARN("No device supports all required capabilities!");
		m_physicalDevice = devices[0].first;

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
	uint32_t PhysicalDevice::GetMaxMsaaSamples() const
	{
		return m_maxMsaaSamples;
	}
	bool PhysicalDevice::SupportsDepthClamp() const
	{
		return m_supportsDepthClamp;
	}
	bool PhysicalDevice::SupportsDepthBiasClamp() const
	{
		return m_supportsDepthBiasClamp;
	}
	bool PhysicalDevice::SupportsMultiViewport() const
	{
		return m_supportsMultiViewport;
	}



	// Private:
	void PhysicalDevice::Cleanup()
	{

	}
	void PhysicalDevice::MoveFrom(PhysicalDevice& other) noexcept
	{
		m_physicalDevice = other.m_physicalDevice;
		m_maxMsaaSamples = other.m_maxMsaaSamples;
		m_supportsDepthClamp = other.m_supportsDepthClamp;
		m_supportsDepthBiasClamp = other.m_supportsDepthBiasClamp;
		m_supportsMultiViewport = other.m_supportsMultiViewport;

		other.m_physicalDevice = VK_NULL_HANDLE;
		other.m_maxMsaaSamples = VK_SAMPLE_COUNT_1_BIT;
		other.m_supportsDepthClamp = false;
		other.m_supportsDepthBiasClamp = false;
		other.m_supportsMultiViewport = false;
	}
	int PhysicalDevice::DeviceScore(VkPhysicalDevice dev)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(dev, &deviceProperties);
		vkGetPhysicalDeviceFeatures(dev, &deviceFeatures);

		// Check essential features:
		bool essentials = (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		essentials &= HasGraphicsAndComputeQueueFamily(dev);
		essentials &= static_cast<bool>(deviceFeatures.geometryShader);
		essentials &= static_cast<bool>(deviceFeatures.tessellationShader);
		essentials &= static_cast<bool>(deviceFeatures.wideLines);
		essentials &= static_cast<bool>(deviceFeatures.samplerAnisotropy);
		essentials &= static_cast<bool>(deviceFeatures.dualSrcBlend);
		essentials &= static_cast<bool>(deviceFeatures.depthBiasClamp);
		essentials &= static_cast<bool>(deviceFeatures.depthBounds);
		essentials &= static_cast<bool>(deviceFeatures.shaderClipDistance);
		essentials &= static_cast<bool>(deviceFeatures.shaderCullDistance);
		essentials &= static_cast<bool>(deviceFeatures.samplerAnisotropy);
		if (essentials == false)
			return -1; // negative score = invalid device.

		// Check optional features:
		int score = 0;
		score += 10 * deviceFeatures.depthClamp; m_supportsDepthClamp = deviceFeatures.depthClamp;
		score += 10 * deviceFeatures.depthBiasClamp; m_supportsDepthBiasClamp = deviceFeatures.depthBiasClamp;
		score += 10 * deviceFeatures.multiViewport; m_supportsMultiViewport = deviceFeatures.multiViewport;
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
		throw std::runtime_error("Could not find a Family Queue that supports Graphics and Compute!");

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
	uint32_t PhysicalDevice::MaxUsableMsaaSampleCount() const
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return static_cast<uint32_t>(VK_SAMPLE_COUNT_64_BIT); }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return static_cast<uint32_t>(VK_SAMPLE_COUNT_32_BIT); }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return static_cast<uint32_t>(VK_SAMPLE_COUNT_16_BIT); }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return static_cast<uint32_t>(VK_SAMPLE_COUNT_8_BIT); }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return static_cast<uint32_t>(VK_SAMPLE_COUNT_4_BIT); }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return static_cast<uint32_t>(VK_SAMPLE_COUNT_2_BIT); }

		return static_cast<uint32_t>(VK_SAMPLE_COUNT_1_BIT);
	}
}