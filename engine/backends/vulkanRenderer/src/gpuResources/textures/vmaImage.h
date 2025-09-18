#pragma once
#include "vk_mem_alloc.h"
#include "vulkanDeviceQueue.h"
#include <string>
#include <vulkan/vulkan.h>



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
		std::string m_name;
		VkImage m_image;
		VmaAllocation m_allocation;
		VkImageView m_imageView;
		VkImageCreateInfo m_imageInfo;
		VmaAllocationCreateInfo m_allocationInfo;
		VkImageSubresourceRange m_subresourceRange;
		DeviceQueue m_queue;
		VkImageLayout m_layout;

	public: // Methods:
		VmaImage(const std::string name, const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, VkImageSubresourceRange& subresourceRange, VkImageViewType viewType, const DeviceQueue& queue);
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

		// Transitions:
		void TransitionLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);
		void TransitionLayout(VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkPipelineStageFlags2 dstStage, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask);
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