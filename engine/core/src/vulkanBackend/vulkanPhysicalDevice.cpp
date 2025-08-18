#include "vulkanPhysicalDevice.h"
#include "vulkanInstance.h"
#include "vulkanMacros.h"
#include <assert.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		PhysicalDevice::PhysicalDevice()
		{
			m_physicalDevice = VK_NULL_HANDLE;
			m_maxMsaaSamples = VK_SAMPLE_COUNT_1_BIT;
			m_supportsDepthClamp = false;
			m_supportsDepthBiasClamp = false;
			m_supportsMultiViewport = false;
		}
		void PhysicalDevice::Init(Instance* pInstance)
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
		VkBool32 PhysicalDevice::SupportsDepthClamp() const
		{
			return m_supportsDepthClamp;
		}
		VkBool32 PhysicalDevice::SupportsDepthBiasClamp() const
		{
			return m_supportsDepthBiasClamp;
		}
		VkBool32 PhysicalDevice::SupportsMultiViewport() const
		{
			return m_supportsMultiViewport;
		}



		// Private:
		int PhysicalDevice::DeviceScore(VkPhysicalDevice dev)
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
			essentials &= deviceFeatures.samplerAnisotropy;
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
		VkBool32 PhysicalDevice::HasGraphicsAndComputeQueueFamily(VkPhysicalDevice device) const
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
		VkBool32 PhysicalDevice::HasPresentQueueFamily(VkPhysicalDevice device, VkSurfaceKHR surface) const
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
			if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
			if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
			if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

			return VK_SAMPLE_COUNT_1_BIT;
		}
	}
}