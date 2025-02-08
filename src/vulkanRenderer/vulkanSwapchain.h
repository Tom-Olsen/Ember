#ifndef __INCLUDE_GUARD_vulkanSwapchain_h__
#define __INCLUDE_GUARD_vulkanSwapchain_h__
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	class VulkanLogicalDevice;
	class VulkanSurface;



	class VulkanSwapchain
	{
	private: // Members:
		VkSwapchainKHR m_swapchain;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		VulkanLogicalDevice* m_pLogicalDevice;
		VulkanSurface* m_pSurface;

	public: // Methods:
		VulkanSwapchain(VulkanLogicalDevice* pLogicalDevice, VulkanSurface* pSurface, VkImageUsageFlags usage, VulkanSwapchain* pOldSwapchain = nullptr);
		~VulkanSwapchain();

		// Getters:
		const VkSwapchainKHR& GetVkSwapchainKHR() const;
		const std::vector<VkImage>& GetImages() const;
		const std::vector<VkImageView>& GetImageViews() const;

	private: // Methods:
		void CreateSwapchain(VkImageUsageFlags usage, VulkanSwapchain* pOldSwapchain);
		void CreateImages();
		void CreateImageViews();
	};
}



#endif // __INCLUDE_GUARD_vulkanSwapchain_h__