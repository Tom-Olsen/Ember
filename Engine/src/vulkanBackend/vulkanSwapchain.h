#ifndef __INCLUDE_GUARD_vulkanSwapchain_h__
#define __INCLUDE_GUARD_vulkanSwapchain_h__
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Forward declarations:
		class LogicalDevice;
		class Surface;



		class Swapchain
		{
		private: // Members:
			VkSwapchainKHR m_swapchain;
			std::vector<VkImage> m_images;
			std::vector<VkImageView> m_imageViews;
			LogicalDevice* m_pLogicalDevice;
			Surface* m_pSurface;

		public: // Methods:
			Swapchain(LogicalDevice* pLogicalDevice, Surface* pSurface, VkImageUsageFlags usage, Swapchain* pOldSwapchain = nullptr);
			~Swapchain();

			// Getters:
			const VkSwapchainKHR& GetVkSwapchainKHR() const;
			const std::vector<VkImage>& GetImages() const;
			const std::vector<VkImageView>& GetImageViews() const;

		private: // Methods:
			void CreateSwapchain(VkImageUsageFlags usage, Swapchain* pOldSwapchain);
			void CreateImages();
			void CreateImageViews();
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanSwapchain_h__