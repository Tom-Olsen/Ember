#include "vulkanSurface.h"
#include "sdlWindow.h"
#include "vulkanInstance.h"
#include "vulkanMacros.h"
#include "vulkanPhysicalDevice.h"
#include <SDL3/SDL_vulkan.h>



namespace emberEngine
{
	// Constructor/Destructor:
	VulkanSurface::VulkanSurface(VulkanInstance* pInstance, VulkanPhysicalDevice* pPhysicalDevice, SdlWindow* pWindow, bool vSyncEnabled)
	{
		m_pInstance = pInstance;
		m_pPhysicalDevice = pPhysicalDevice;
		m_pWindow = pWindow;

		// Surface:
		SDL_Vulkan_CreateSurface(m_pWindow->GetSDL_Window(), m_pInstance->GetVkInstance(), nullptr, &m_surface);

		// Available surfaceFormats:
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &formatCount, nullptr);
		m_availableSurfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &formatCount, m_availableSurfaceFormats.data());
		if (m_availableSurfaceFormats.empty())
			throw std::runtime_error("No surface formats found!");

		// Available presentModes:
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &presentModeCount, nullptr);
		m_availablePresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &presentModeCount, m_availablePresentModes.data());
		if (m_availablePresentModes.empty())
			throw std::runtime_error("No present modes found!");

		// Pick surfaceFormat and presentMode:
		m_surfaceFormat = PickSurfaceFormat();
		m_presentMode = PickPresentMode(vSyncEnabled);
	}
	VulkanSurface::~VulkanSurface()
	{
		vkDestroySurfaceKHR(m_pInstance->GetVkInstance(), m_surface, nullptr);
	}



	// Public methods:
	const VkSurfaceKHR& VulkanSurface::GetVkSurfaceKHR() const
	{
		return m_surface;
	}
	const VkSurfaceFormatKHR& VulkanSurface::GetVkSurfaceFormatKHR() const
	{
		return m_surfaceFormat;
	}
	const VkPresentModeKHR& VulkanSurface::GetVkPresentModeKHR() const
	{
		return m_presentMode;
	}
	VkExtent2D VulkanSurface::GetCurrentExtent()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.currentExtent;
	}
	VkExtent2D VulkanSurface::GetMinImageExtent()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.minImageExtent;
	}
	VkExtent2D VulkanSurface::GetMaxImageExtent()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.maxImageExtent;
	}
	uint32_t VulkanSurface::GetMinImageCount()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.minImageCount;
	}
	uint32_t VulkanSurface::GetMaxImageCount()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.maxImageCount;
	}



	// Private:
	VkSurfaceFormatKHR VulkanSurface::PickSurfaceFormat() const
	{
		for (const auto& format : m_availableSurfaceFormats)
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return format;

		// First is always the best if desired not available.
		return m_availableSurfaceFormats[0];
	}
	VkPresentModeKHR VulkanSurface::PickPresentMode(bool vSyncEnabled) const
	{
		for (const VkPresentModeKHR& mode : m_availablePresentModes)
		{
			if (vSyncEnabled == true && mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return mode;
			if (vSyncEnabled == false && mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				return mode;
		}

		// Always available.
		if (vSyncEnabled == true)
			LOG_WARN("vSync=enabled present mode VK_PRESENT_MODE_MAILBOX_KHR not available. Using fallback mode VK_PRESENT_MODE_FIFO_KHR.");
		if (vSyncEnabled == false)
			LOG_WARN("vSync=disabled present mode VK_PRESENT_MODE_IMMEDIATE_KHR not available. Using fallback mode VK_PRESENT_MODE_FIFO_KHR.");
		return VK_PRESENT_MODE_FIFO_KHR;
	}



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
}