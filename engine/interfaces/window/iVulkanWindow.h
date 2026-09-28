#pragma once
#include <vector>



// Forward declarations:
typedef struct VkInstance_T* VkInstance;
struct VkAllocationCallbacks;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;



namespace emberBackendInterface
{
	class IVulkanWindow
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IVulkanWindow() = default;

		// Vulkan surface integration:
		virtual void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const = 0;
		virtual void CreateSurface(VkInstance instance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) const = 0;
	};
}