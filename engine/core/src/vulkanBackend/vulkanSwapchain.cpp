#include "vulkanSwapchain.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanSurface.h"
#include <assert.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		Swapchain::Swapchain()
		{
			m_swapchain = VK_NULL_HANDLE;
			m_pLogicalDevice = nullptr;
			m_pSurface = nullptr;
			m_cleared = true;
		}
		void Swapchain::Init(LogicalDevice* pLogicalDevice, Surface* pSurface, VkImageUsageFlags usage, Swapchain* pOldSwapchain)
		{
			// Assertions:
			assert(pLogicalDevice != nullptr);
			assert(pSurface != nullptr);

			m_pLogicalDevice = pLogicalDevice;
			m_pSurface = pSurface;

			CreateSwapchain(usage, pOldSwapchain);
			CreateImages();
			CreateImageViews();
			m_cleared = false;
		}
		void Swapchain::Clear()
		{
			if (m_cleared)
				return;

			VKA(vkDeviceWaitIdle(m_pLogicalDevice->GetVkDevice()));
			for (uint32_t i = 0; i < m_imageViews.size(); i++)
				vkDestroyImageView(m_pLogicalDevice->GetVkDevice(), m_imageViews[i], nullptr);
			vkDestroySwapchainKHR(m_pLogicalDevice->GetVkDevice(), m_swapchain, nullptr);
			m_cleared = true;
		}
		Swapchain::~Swapchain()
		{
			Clear();
		}



		// Public methods:
        int Swapchain::GetImageCount() const
        {
            return m_images.size();
        }
		const VkSwapchainKHR& Swapchain::GetVkSwapchainKHR() const
		{
			return m_swapchain;
		}
		const std::vector<VkImage>& Swapchain::GetImages() const
		{
			return m_images;
		}
		const std::vector<VkImageView>& Swapchain::GetImageViews() const
		{
			return m_imageViews;
		}



		// Private methods:
		void Swapchain::CreateSwapchain(VkImageUsageFlags usage, Swapchain* pOldSwapchain)
		{
			uint32_t imageCount = m_pSurface->GetMinImageCount() + 1;
			if (m_pSurface->GetMaxImageCount() > 0 && imageCount > m_pSurface->GetMaxImageCount())
				imageCount = m_pSurface->GetMaxImageCount();

			VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
			createInfo.surface = m_pSurface->GetVkSurfaceKHR();
			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = m_pSurface->GetVkSurfaceFormatKHR().format;
			createInfo.imageColorSpace = m_pSurface->GetVkSurfaceFormatKHR().colorSpace;
			createInfo.imageExtent = m_pSurface->GetCurrentExtent();
			createInfo.imageArrayLayers = 1;									// always 1 unless stereoscopic 3D application.
			createInfo.imageUsage = usage;
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;			// we assume that only one queue family will access the images for now.
			createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;	// dont rotate or flip.
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		// dont blend with other windows.
			createInfo.presentMode = m_pSurface->GetVkPresentModeKHR();
			createInfo.oldSwapchain = pOldSwapchain ? pOldSwapchain->GetVkSwapchainKHR() : VK_NULL_HANDLE;
			createInfo.clipped = VK_TRUE;										// clip pixels that are obscured by other windows.
			VKA(vkCreateSwapchainKHR(m_pLogicalDevice->GetVkDevice(), &createInfo, nullptr, &m_swapchain));
		}

		void Swapchain::CreateImages()
		{
			uint32_t imageCount;
			VKA(vkGetSwapchainImagesKHR(m_pLogicalDevice->GetVkDevice(), m_swapchain, &imageCount, nullptr));
			m_images.resize(imageCount);
			VKA(vkGetSwapchainImagesKHR(m_pLogicalDevice->GetVkDevice(), m_swapchain, &imageCount, m_images.data()));
		}

		void Swapchain::CreateImageViews()
		{
			m_imageViews.resize(m_images.size());
			for (uint32_t i = 0; i < m_images.size(); i++)
			{
				VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
				createInfo.image = m_images[i];
				createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				createInfo.format = m_pSurface->GetVkSurfaceFormatKHR().format;
				createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				createInfo.subresourceRange.baseMipLevel = 0;
				createInfo.subresourceRange.levelCount = 1;
				createInfo.subresourceRange.baseArrayLayer = 0;
				createInfo.subresourceRange.layerCount = 1;
				VKA(vkCreateImageView(m_pLogicalDevice->GetVkDevice(), &createInfo, nullptr, &m_imageViews[i]))
			}
		}
	}
}