#pragma once
#include <cstdint>



// Forward declarations:
typedef struct VkInstance_T* VkInstance;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef struct VkRenderPass_T* VkRenderPass;
typedef struct VkQueue_T* VkQueue;



namespace emberBackendInterface
{
	/// <summary>
	/// Vulkan-specific renderer coupling for backends that integrate directly with Vulkan.
	/// </summary>
	class IVulkanRenderer
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IVulkanRenderer() = default;

		// Vulkan handles:
		virtual VkInstance GetVkInstance() const = 0;
		virtual VkPhysicalDevice GetVkPhysicalDevice() const = 0;
		virtual VkDevice GetVkDevice() const = 0;
		virtual VkRenderPass GetPresentVkRenderPass() const = 0;
		virtual VkQueue GetGraphicsVkQueue() const = 0;
		virtual uint32_t GetGraphicsVkQueueFamilyIndex() const = 0;

		// Presentation configuration:
		virtual uint32_t GetSwapchainImageCount() const = 0;
		virtual uint32_t GetFramesInFlight() const = 0;
		virtual uint64_t GetAbsoluteFrameIndex() const = 0;
	};
}