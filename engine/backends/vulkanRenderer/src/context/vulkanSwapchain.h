#pragma once
#include "emberMath.h"
#include <vector>
#include <vulkan/vulkan.h>



// Forward declarations:
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;



namespace vulkanRendererBackend
{
	// Forward declarations:
	class LogicalDevice;
	class Surface;



    // Swapchain is immutable. On window resize old swapchain is destroyed and new one gets created with updated state.
	class Swapchain
	{
	private: // Members:
		VkSwapchainKHR m_swapchain;
		Uint2 m_extent;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		LogicalDevice* m_pLogicalDevice;
		Surface* m_pSurface;
		VkImageUsageFlags m_usages;

	public: // Methods:
		Swapchain(LogicalDevice* pLogicalDevice, Surface* pSurface, VkImageUsageFlags usages, Swapchain* pOldSwapchain = nullptr);
		~Swapchain();

		// Non-copyable:
		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;

		// Movable:
		Swapchain(Swapchain&& other) noexcept;
		Swapchain& operator=(Swapchain&& other) noexcept;

		void Init(LogicalDevice* pLogicalDevice, Surface* pSurface, VkImageUsageFlags usages, Swapchain* pOldSwapchain = nullptr);

		// Getters:
        int GetImageCount() const;
		Uint2 GetExtent() const;
		const VkSwapchainKHR& GetVkSwapchainKHR() const;
		const std::vector<VkImage>& GetImages() const;
		const std::vector<VkImageView>& GetImageViews() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(Swapchain& other) noexcept;
		void CreateSwapchain(Swapchain* pOldSwapchain);
		void CreateImages();
		void CreateImageViews();
	};
}