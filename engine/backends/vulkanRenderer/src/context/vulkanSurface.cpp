#include "vulkanSurface.h"
#include "iWindow.h"
#include "logger.h"
#include "vulkanInstance.h"
#include "vulkanMacros.h"
#include "vulkanPhysicalDevice.h"
#include <assert.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	Surface::Surface()
	{
		m_surface = VK_NULL_HANDLE;
		m_pInstance = nullptr;
		m_pPhysicalDevice = nullptr;
	}
	Surface::~Surface()
	{
		Cleanup();
	}


	// Move semantics:
	Surface::Surface(Surface&& other) noexcept
	{
		MoveFrom(other);
	}
	Surface& Surface::operator=(Surface&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methods:
	void Surface::Init(Instance* pInstance, PhysicalDevice* pPhysicalDevice, emberBackendInterface::IWindow* pIWindow, bool vSyncEnabled)
	{
		// Assertions:
		assert(pInstance != nullptr);
		assert(pPhysicalDevice != nullptr);

		m_pInstance = pInstance;
		m_pPhysicalDevice = pPhysicalDevice;

		// Create surface:
		pIWindow->CreateSurface(m_pInstance->GetVkInstance(), nullptr, &m_surface);

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



	// Getters:
	const VkSurfaceKHR& Surface::GetVkSurfaceKHR() const
	{
		return m_surface;
	}
	const VkSurfaceFormatKHR& Surface::GetVkSurfaceFormatKHR() const
	{
		return m_surfaceFormat;
	}
	const VkPresentModeKHR& Surface::GetVkPresentModeKHR() const
	{
		return m_presentMode;
	}
	VkExtent2D Surface::GetCurrentExtent()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.currentExtent;
	}
	VkExtent2D Surface::GetMinImageExtent()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.minImageExtent;
	}
	VkExtent2D Surface::GetMaxImageExtent()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.maxImageExtent;
	}
	uint32_t Surface::GetMinImageCount()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.minImageCount;
	}
	uint32_t Surface::GetMaxImageCount()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &m_surfaceCapabilities);
		return m_surfaceCapabilities.maxImageCount;
	}



	// Private:
	void Surface::Cleanup()
	{
		vkDestroySurfaceKHR(m_pInstance->GetVkInstance(), m_surface, nullptr);
	}
	void Surface::MoveFrom(Surface& other) noexcept
	{
		m_surface = other.m_surface;
		m_surfaceFormat = other.m_surfaceFormat;
		m_presentMode = other.m_presentMode;
		m_surfaceCapabilities = other.m_surfaceCapabilities;
		m_availableSurfaceFormats = std::move(other.m_availableSurfaceFormats);
		m_availablePresentModes = std::move(other.m_availablePresentModes);
		m_pInstance = other.m_pInstance;
		m_pPhysicalDevice = other.m_pPhysicalDevice;

		other.m_surface = VK_NULL_HANDLE;
		other.m_surfaceFormat = {};
		other.m_presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
		other.m_surfaceCapabilities = {};
		other.m_availableSurfaceFormats.clear();
		other.m_availablePresentModes.clear();
		other.m_pInstance = nullptr;
		other.m_pPhysicalDevice = nullptr;
	}
	VkSurfaceFormatKHR Surface::PickSurfaceFormat() const
	{
		for (const auto& format : m_availableSurfaceFormats)
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return format;

		// First is always the best if desired not available.
		return m_availableSurfaceFormats[0];
	}
	VkPresentModeKHR Surface::PickPresentMode(bool vSyncEnabled) const
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
}