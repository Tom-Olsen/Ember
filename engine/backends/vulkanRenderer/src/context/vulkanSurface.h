#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanSurface_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanSurface_h__
#include <vulkan/vulkan.h>
#include <vector>



// Presents Modes:
// VK_PRESENT_MODE_IMMEDIATE_KHR:
// Images submitted by your application are transferred to the screen right away, which may result in tearing.

// VK_PRESENT_MODE_FIFO_KHR:
// The swap chain is a queue where the display takes an image from the front of the queue when the display is
// refreshed and the program inserts rendered images at the back of the queue.If the queue is full then the
// program has to wait.This is most similar to vertical sync as found in modern games.The moment that the
// display is refreshed is known as "vertical blank".

// VK_PRESENT_MODE_FIFO_RELAXED_KHR:
// This mode only differs from the previous one if the application is late and the queue was empty at the
// last vertical blank.Instead of waiting for the next vertical blank, the image is transferred right away
// when it finally arrives.This may result in visible tearing.

// VK_PRESENT_MODE_MAILBOX_KHR:
// This is another variation of the second mode. Instead of blocking the application when the queue is full,
// the images that are already queued are simply replaced with the newer ones.This mode can be used to render
// frames as fast as possible while still avoiding tearing, resulting in fewer latency issues than standard
// vertical sync.This is commonly known as "triple buffering", although the existence of three buffers alone
// does not necessarily mean that the framerate is unlocked.



// Forward declerations:
namespace emberBackendInterface
{
	class IWindow;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Instance;
	class PhysicalDevice;



	class Surface
	{
	private: // Members:
		VkSurfaceKHR m_surface;
		VkSurfaceFormatKHR m_surfaceFormat;
		VkPresentModeKHR m_presentMode;
		VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> m_availableSurfaceFormats;
		std::vector<VkPresentModeKHR> m_availablePresentModes;
		Instance* m_pInstance;
		PhysicalDevice* m_pPhysicalDevice;

	public: // Methods:
		Surface();
		~Surface();

		// Non-copyable:
		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;

		// Movable:
		Surface(Surface&& other) noexcept;
		Surface& operator=(Surface&& other) noexcept;

		void Init(Instance* pInstance, PhysicalDevice* pPhysicalDevice, emberBackendInterface::IWindow* pIWindow, bool vSyncEnabled);

		// Getters:
		const VkSurfaceKHR& GetVkSurfaceKHR() const;
		const VkSurfaceFormatKHR& GetVkSurfaceFormatKHR() const;
		const VkPresentModeKHR& GetVkPresentModeKHR() const;
		VkExtent2D GetCurrentExtent();
		VkExtent2D GetMinImageExtent();
		VkExtent2D GetMaxImageExtent();
		uint32_t GetMinImageCount();
		uint32_t GetMaxImageCount();

	private: // Methods:
		void Cleanup();
		void MoveFrom(Surface& other) noexcept;
		VkSurfaceFormatKHR PickSurfaceFormat() const;
		VkPresentModeKHR PickPresentMode(bool vSyncEnabled) const;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanSurface_h__