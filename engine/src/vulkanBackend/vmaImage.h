#ifndef __INCLUDE_GUARD_vmaImage_h__
#define __INCLUDE_GUARD_vmaImage_h__
#include "vk_mem_alloc.h"
#include "vulkanDeviceQueue.h"
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		/// <summary>
		/// Manages VkImage and VkImageView resource: creation, allocation, destruction, queue ownership, layout.
		/// Usually further abstracted by texture class which wraps around it.
		/// Sometimes used stand alone when the image is strictly used for a specific usecase,
		/// e.g. msaa color and depth attachment images of forward renderpass.
		/// </summary>
		class VmaImage
		{
		private: // Members:
			static uint32_t s_index;
			std::string m_name;
			VkImage m_image;
			VmaAllocation m_allocation;
			VkImageView m_imageView;
			VkImageCreateInfo m_imageInfo;
			VmaAllocationCreateInfo m_allocationInfo;
			VkImageSubresourceRange m_subresourceRange;
			DeviceQueue m_queue;
			VkImageLayout m_layout;
			// Ember::TODO: add m_size member?

		public: // Methods:
			VmaImage(const std::string name, const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, VkImageSubresourceRange& subresourceRange, VkImageViewType viewType, const DeviceQueue& queue);
			~VmaImage();


			// Getters:
			const std::string& GetName() const;
			const VkImage& GetVkImage() const;
			const VmaAllocation& GetVmaAllocation() const;
			const VkImageView& GetVkImageView() const;
			const VkImageCreateInfo& GetVkImageCreateInfo() const;
			const VmaAllocationCreateInfo& GetVmaAllocationCreateInfo() const;
			const VkImageSubresourceRange& GetSubresourceRange() const;
			const DeviceQueue& GetDeviceQueue() const;
			const VkImageLayout& GetLayout() const;
			uint64_t GetWidth() const;
			uint64_t GetHeight() const;
			uint64_t GetDepth() const;
			const VkExtent3D& GetExtent() const;
			VkFormat GetFormat() const;
			VkImageSubresourceLayers GetSubresourceLayers() const;

			// Setters:
			// Only changes the m_layout member without doing an image layout transition.
			void SetLayout(VkImageLayout imageLayout);

			// Upload/Download:
			void Upload(VkCommandBuffer commandBuffer, void* pSrc, uint64_t size, uint32_t layerCount);
			void Upload(void* pSrc, uint64_t size, uint32_t layerCount);

			// Transitions:
			void TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);
			void TransitionLayout(VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);
			void GenerateMipmaps(VkCommandBuffer commandBuffer, uint32_t mipLevels);
			void GenerateMipmaps(uint32_t mipLevels);

			// Static methods:
			static void CopyImageToImage(VkCommandBuffer commandBuffer, VmaImage* srcImage, VmaImage* dstImage, const DeviceQueue& queue);
			static void CopyImageToImage(VmaImage* srcImage, VmaImage* dstImage, const DeviceQueue& queue);
		};
	}
}



#endif // __INCLUDE_GUARD_vmaImage_h__