#include "vulkanSurface.h"
#include "iWindow.h"
#include "logger.h"
#include "vulkanFormat.h"
#include "vulkanInstance.h"
#include "vulkanMacros.h"
#include "vulkanPhysicalDevice.h"
#include <assert.h>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declerations:
	VkSurfaceFormatKHR PickSurfaceFormat(std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
	VkPresentModeKHR PickPresentMode(bool vSyncEnabled, std::vector<VkPresentModeKHR>& availablePresentModes);



	// Constructor/Destructor:
	Surface::Surface(Instance* pInstance, PhysicalDevice* pPhysicalDevice, emberBackendInterface::IWindow* pIWindow, bool vSyncEnabled)
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
		std::vector<VkSurfaceFormatKHR> availableSurfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &formatCount, availableSurfaceFormats.data());
		if (availableSurfaceFormats.empty())
			throw std::runtime_error("No surface formats found!");

		// Available presentModes:
		uint32_t presentModeCount;
		std::vector<VkPresentModeKHR> availablePresentModes;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &presentModeCount, nullptr);
		availablePresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &presentModeCount, availablePresentModes.data());
		if (availablePresentModes.empty())
			throw std::runtime_error("No present modes found!");

		// Pick surfaceFormat and presentMode:
		VkSurfaceFormatKHR surfaceFormat = PickSurfaceFormat(availableSurfaceFormats);
		m_surfaceFormat = SurfaceFormat{ static_cast<uint32_t>(surfaceFormat.format), static_cast<uint32_t>(surfaceFormat.colorSpace) };
		m_presentMode = PickPresentMode(vSyncEnabled, availablePresentModes);
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
	// Getters:
	const VkSurfaceKHR Surface::GetVkSurfaceKHR() const
	{
		return m_surface;
	}
	const SurfaceFormat Surface::GetSurfaceFormat() const
	{
		return m_surfaceFormat;
	}
	const PresentMode Surface::GetPresentMode() const
	{
		return m_presentMode;
	}
	void Surface::GetCurrentExtent(VkExtent2D& currentExtent) const
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &surfaceCapabilities);
		currentExtent = surfaceCapabilities.currentExtent;
	}
	void Surface::GetMinImageExtent(VkExtent2D& minImageExtent) const
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &surfaceCapabilities);
		minImageExtent = surfaceCapabilities.minImageExtent;
	}
	void Surface::GetMaxImageExtent(VkExtent2D& maxImageExtent) const
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &surfaceCapabilities);
		maxImageExtent = surfaceCapabilities.maxImageExtent;
	}
	uint32_t Surface::GetMinImageCount() const
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &surfaceCapabilities);
		return surfaceCapabilities.minImageCount;
	}
	uint32_t Surface::GetMaxImageCount() const
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice->GetVkPhysicalDevice(), m_surface, &surfaceCapabilities);
		return surfaceCapabilities.maxImageCount;
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
		m_pInstance = other.m_pInstance;
		m_pPhysicalDevice = other.m_pPhysicalDevice;

		other.m_surface = VK_NULL_HANDLE;
		other.m_surfaceFormat = SurfaceFormat{ Formats::undefined, ColorSpaces::max_enum };
		other.m_presentMode = PresentModes::max_enum;
		other.m_pInstance = nullptr;
		other.m_pPhysicalDevice = nullptr;
	}



	// Helpers:
	VkSurfaceFormatKHR PickSurfaceFormat(std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
	{
		for (const auto& format : availableSurfaceFormats)
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return format;

		// First is always the best if desired not available.
		return availableSurfaceFormats[0];
	}
	VkPresentModeKHR PickPresentMode(bool vSyncEnabled, std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const VkPresentModeKHR& mode : availablePresentModes)
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