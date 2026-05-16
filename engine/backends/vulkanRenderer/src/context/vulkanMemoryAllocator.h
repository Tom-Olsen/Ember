#pragma once
#include "vk_mem_alloc.h"
#include <unordered_map>



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Instance;
	class LogicalDevice;
	class PhysicalDevice;



	class MemoryAllocator
	{
	private: // Members:
		static constexpr VkDeviceSize s_hostPoolBlockSize = 64ull * 1024ull * 1024ull;      //  64 MiB
		static constexpr VkDeviceSize s_devicePoolBlockSize = 256ull * 1024ull * 1024ull;   // 256 MiB
		VmaAllocator m_pAllocator;
		std::unordered_map<uint32_t, VmaPool> m_pPools; // one pool per vulkan memory type (DEVICE_LOCAL vs HOST_VISIBLE + HOST_COHERENT vs HOST_VISIBLE + HOST_COHERENT + HOST_CACHED vs etc.).

	public: // Methods:
		MemoryAllocator(Instance* pInstance, LogicalDevice* pLogicalDevice, PhysicalDevice* pPhysicalDevice);
		~MemoryAllocator();

		// Non-copyable:
		MemoryAllocator(const MemoryAllocator&) = delete;
		MemoryAllocator& operator=(const MemoryAllocator&) = delete;

		// Movable:
		MemoryAllocator(MemoryAllocator&& other) noexcept;
		MemoryAllocator& operator=(MemoryAllocator&& other) noexcept;

		VkResult CreateBuffer(const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocationInfo, VkBuffer* pBuffer, VmaAllocation* pAllocation);
		VkResult CreateImage(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, VkImage* pImage, VmaAllocation* pAllocation);
		void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);
		void DestroyImage(VkImage image, VmaAllocation allocation);

		const VmaAllocator& GetVmaAllocator() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(MemoryAllocator& other) noexcept;
		VmaPool GetOrCreatePool(uint32_t memoryTypeIndex);
		VkDeviceSize GetPoolBlockSize(uint32_t memoryTypeIndex) const;
		bool UseManagedPool(const VmaAllocationCreateInfo& allocationInfo) const;
	};
}