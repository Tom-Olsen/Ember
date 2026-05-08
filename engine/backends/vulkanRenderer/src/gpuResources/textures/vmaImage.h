#pragma once
#include "vk_mem_alloc.h"
#include "vmaAllocationCreateInfo.h"
#include "vulkanAccessMask.h"
#include "vulkanDeviceQueue.h"
#include "vulkanImageCreateInfo.h"
#include "vulkanPipelineStage.h"
#include <string>
#include <vulkan/vulkan.h>



// Forward declarations:
typedef struct VkImage_T* VkImage;
typedef struct VmaAllocation_T* VmaAllocation;
typedef struct VkImageView_T* VkImageView;



namespace vulkanRendererBackend
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
		VkImage m_image;
		VmaAllocation m_allocation;
		VkImageView m_imageView;
		ImageCreateInfo m_imageInfo;
		AllocationCreateInfo m_allocationInfo;
		VkImageSubresourceRange m_subresourceRange;
		DeviceQueue m_queue;
		VkImageLayout m_layout;

	public: // Methods:
		VmaImage(const ImageCreateInfo& imageInfo, const AllocationCreateInfo& allocationInfo, VkImageSubresourceRange& subresourceRange, VkImageViewType viewType, const DeviceQueue& queue);
		~VmaImage();

		// Non-copyable:
		VmaImage(const VmaImage&) = delete;
		VmaImage& operator=(const VmaImage&) = delete;

		// Movable:
		VmaImage(VmaImage&& other) noexcept;
		VmaImage& operator=(VmaImage&& other) noexcept;

		// Getters:
		const std::string& GetName() const;
		const VkImage& GetVkImage() const;
		const VmaAllocation& GetVmaAllocation() const;
		const VkImageView& GetVkImageView() const;
		const ImageCreateInfo& GetImageCreateInfo() const;
		const AllocationCreateInfo& GetAllocationCreateInfo() const;
		const VkImageSubresourceRange& GetImageSubresourceRange() const;
		const DeviceQueue& GetDeviceQueue() const;
		const VkImageLayout& GetImageLayout() const;
		uint64_t GetWidth() const;
		uint64_t GetHeight() const;
		uint64_t GetDepth() const;
		const Uint3& GetExtent() const;
		Format GetFormat() const;
		VkImageSubresourceLayers GetImageSubresourceLayers() const;

		// Setters:
		// Only changes the m_layout member without doing an image layout transition.
		void SetLayout(VkImageLayout imageLayout);

		// Transitions:
		void TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout, PipelineStage srcStage, PipelineStage dstStage, AccessMask srcAccessMask, AccessMask dstAccessMask);
		void TransitionLayout(VkImageLayout newLayout, PipelineStage srcStage, PipelineStage dstStage, AccessMask srcAccessMask, AccessMask dstAccessMask);
		void GenerateMipmaps(VkCommandBuffer commandBuffer, uint32_t mipLevels);
		void GenerateMipmaps(uint32_t mipLevels);

		// Static methods:
		static void CopyImageToImage(VkCommandBuffer commandBuffer, VmaImage* srcImage, VmaImage* dstImage, const DeviceQueue& queue);
		static void CopyImageToImage(VmaImage* srcImage, VmaImage* dstImage, const DeviceQueue& queue);

	private: // Methods:
		void Cleanup();
		void MoveFrom(VmaImage& other) noexcept;
	};
}