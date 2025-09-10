#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanSwapchain_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanSwapchain_h__
#include <vulkan/vulkan.h>
#include <vector>



namespace vulkanRendererBackend
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
		VkImageUsageFlags m_usage;

	public: // Methods:
		Swapchain();
		~Swapchain();

		// Non-copyable:
		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;

		// Movable:
		Swapchain(Swapchain&& other) noexcept;
		Swapchain& operator=(Swapchain&& other) noexcept;

		void Init(LogicalDevice* pLogicalDevice, Surface* pSurface, VkImageUsageFlags usage, Swapchain* pOldSwapchain = nullptr);

		// Getters:
        int GetImageCount() const;
		const VkSwapchainKHR& GetVkSwapchainKHR() const;
		const std::vector<VkImage>& GetImages() const;
		const std::vector<VkImageView>& GetImageViews() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(Swapchain& other) noexcept;
		void CreateSwapchain(VkImageUsageFlags usage, Swapchain* pOldSwapchain);
		void CreateImages();
		void CreateImageViews();
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanSwapchain_h__