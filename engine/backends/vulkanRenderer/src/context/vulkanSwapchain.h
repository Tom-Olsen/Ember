#pragma once
#include "vulkanImageUsageFlag.h"
#include <vector>



// Forward declerations:
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;



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
		ImageUsageFlag m_usages;

	public: // Methods:
		Swapchain(LogicalDevice* pLogicalDevice, Surface* pSurface, ImageUsageFlag usages, Swapchain* pOldSwapchain = nullptr);
		~Swapchain();

		// Non-copyable:
		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;

		// Movable:
		Swapchain(Swapchain&& other) noexcept;
		Swapchain& operator=(Swapchain&& other) noexcept;

		void Init(LogicalDevice* pLogicalDevice, Surface* pSurface, ImageUsageFlag usages, Swapchain* pOldSwapchain = nullptr);

		// Getters:
        int GetImageCount() const;
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