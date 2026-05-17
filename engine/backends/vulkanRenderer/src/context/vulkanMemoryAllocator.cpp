#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_VULKAN_VERSION 1003000
#include "vulkanMemoryAllocator.h"
#include "vulkanInstance.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanPhysicalDevice.h"
#include <assert.h>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	MemoryAllocator::MemoryAllocator(Instance* pInstance, LogicalDevice* pLogicalDevice, PhysicalDevice* pPhysicalDevice)
	{
		// Assertions:
		assert(pInstance != nullptr);
		assert(pLogicalDevice != nullptr);
		assert(pPhysicalDevice != nullptr);

		// References to vulkan functions:
		VmaVulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		// Define allocator properties:
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.instance = pInstance->GetVkInstance();
		allocatorInfo.device = pLogicalDevice->GetVkDevice();
		allocatorInfo.physicalDevice = pPhysicalDevice->GetVkPhysicalDevice();
		allocatorInfo.flags = 0;								// enable extensions. Must be supported by Vulkan device, see https://gpuopen-librariesandsdks.github.io/MemoryAllocator/html/quick_start.html
		allocatorInfo.preferredLargeHeapBlockSize = 0;			// 0 = default size = 256 MiB
		allocatorInfo.pAllocationCallbacks = nullptr;			// optional: custom CPU memory allocation callbacks
		allocatorInfo.pDeviceMemoryCallbacks = nullptr;			// optional: informative callbacks for `vkAllocateMemory`, `vkFreeMemory`
		allocatorInfo.pHeapSizeLimit = nullptr;					// optional: limit on size of Vulkan device memory heaps
		allocatorInfo.pVulkanFunctions = &vulkanFunctions;		// pointers to Vulkan functions
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;	// same as `Instance::apiVersion`
		allocatorInfo.pTypeExternalMemoryHandleTypes = nullptr;	// optional: array of external memory handle types (don't understand this yet)

		// Create allocator:
		VKA(vmaCreateAllocator(&allocatorInfo, &m_pAllocator));
	}
	MemoryAllocator::~MemoryAllocator()
	{
		Cleanup();
	}


	// Move semantics:
	MemoryAllocator::MemoryAllocator(MemoryAllocator&& other) noexcept
	{
		MoveFrom(other);
	}
	MemoryAllocator& MemoryAllocator::operator=(MemoryAllocator&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methods:
	VkResult MemoryAllocator::CreateBuffer(const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocationInfo, VkDeviceSize allocationSize, VkBuffer* pBuffer, VmaAllocation* pAllocation)
	{
		// allocationInfo might be needed as fallback, so make a copy:
		VmaAllocationCreateInfo pooledAllocationInfo = allocationInfo;

		// Set memory type specific pool unless user requests something special (allocationInfo.pool != nullptr || VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT):
		uint32_t memoryTypeIndex;
		VkResult result = vmaFindMemoryTypeIndexForBufferInfo(m_pAllocator, &bufferInfo, &allocationInfo, &memoryTypeIndex);
		if (result == VK_SUCCESS && UseManagedPool(allocationInfo, RoundUp(allocationSize, 16), GetPoolBlockSize(memoryTypeIndex)))
			pooledAllocationInfo.pool = GetOrCreatePool(memoryTypeIndex);

		// Try the managed pool:
		result = vmaCreateBuffer(m_pAllocator, &bufferInfo, &pooledAllocationInfo, pBuffer, pAllocation, nullptr);
		if (result == VK_SUCCESS || pooledAllocationInfo.pool == nullptr)
			return result;

		// Fallback:
		return vmaCreateBuffer(m_pAllocator, &bufferInfo, &allocationInfo, pBuffer, pAllocation, nullptr);
	}
	VkResult MemoryAllocator::CreateImage(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo, VkDeviceSize allocationSize, VkImage* pImage, VmaAllocation* pAllocation)
	{
		// allocationInfo might be needed as fallback, so make a copy:
		VmaAllocationCreateInfo pooledAllocationInfo = allocationInfo;

		// Set memory type specific pool unless user requests something special (allocationInfo.pool != nullptr || VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT):
		uint32_t memoryTypeIndex;
		VkResult result = vmaFindMemoryTypeIndexForImageInfo(m_pAllocator, &imageInfo, &allocationInfo, &memoryTypeIndex);
		if (result == VK_SUCCESS && UseManagedPool(allocationInfo, RoundUp(allocationSize, 16), GetPoolBlockSize(memoryTypeIndex)))
			pooledAllocationInfo.pool = GetOrCreatePool(memoryTypeIndex);

		// Try the managed pool:
		result = vmaCreateImage(m_pAllocator, &imageInfo, &pooledAllocationInfo, pImage, pAllocation, nullptr);
		if (result == VK_SUCCESS || pooledAllocationInfo.pool == nullptr)
			return result;

		// Fallback:
		return vmaCreateImage(m_pAllocator, &imageInfo, &allocationInfo, pImage, pAllocation, nullptr);
	}
	void MemoryAllocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
	{
		vmaDestroyBuffer(m_pAllocator, buffer, allocation);
	}
	void MemoryAllocator::DestroyImage(VkImage image, VmaAllocation allocation)
	{
		vmaDestroyImage(m_pAllocator, image, allocation);
	}
	const VmaAllocator& MemoryAllocator::GetVmaAllocator() const
	{
		return m_pAllocator;
	}



	// Private methods:
	void MemoryAllocator::Cleanup()
	{
		if (m_pAllocator == nullptr)
			return;

		for (auto& [memoryTypeIndex, pPool] : m_pPools)
			vmaDestroyPool(m_pAllocator, pPool);
		m_pPools.clear();

		vmaDestroyAllocator(m_pAllocator);
		m_pAllocator = nullptr;
	}
	void MemoryAllocator::MoveFrom(MemoryAllocator& other) noexcept
	{
		m_pAllocator = other.m_pAllocator;
		m_pPools = std::move(other.m_pPools);

		other.m_pAllocator = nullptr;
		other.m_pPools.clear();
	}
	VmaPool MemoryAllocator::GetOrCreatePool(uint32_t memoryTypeIndex)
	{
		auto it = m_pPools.find(memoryTypeIndex);
		if (it != m_pPools.end())
			return it->second;

		// Each Vulkan memory type gets one reusable VMA pool. VMA sub-allocates buffers/images from these larger blocks.
		VmaPoolCreateInfo poolInfo = {};
		poolInfo.memoryTypeIndex = memoryTypeIndex;
		poolInfo.flags = 0;
		poolInfo.blockSize = GetPoolBlockSize(memoryTypeIndex);
		poolInfo.minBlockCount = 0;
		poolInfo.maxBlockCount = 0;
		poolInfo.priority = 0.5f;   // 0=low, 1=high, 0.5=normal. Hint to the driver about which allocations are more important to keep resident in VRAM when memory pressure occurs.
		poolInfo.minAllocationAlignment = 0;
		poolInfo.pMemoryAllocateNext = nullptr;

		VmaPool pPool = nullptr;
		VKA(vmaCreatePool(m_pAllocator, &poolInfo, &pPool));
		m_pPools.emplace(memoryTypeIndex, pPool);
		return pPool;
	}
	VkDeviceSize MemoryAllocator::GetPoolBlockSize(uint32_t memoryTypeIndex) const
	{
		VkMemoryPropertyFlags propertyFlags;
		vmaGetMemoryTypeProperties(m_pAllocator, memoryTypeIndex, &propertyFlags);

		// Device-local pools are larger because textures/render targets/storage buffers usually live there.
		if (propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			return s_devicePoolBlockSize;

		// Host-visible allocations are typically upload/uniform data, so keep blocks smaller.
		return s_hostPoolBlockSize;
	}
	VkDeviceSize MemoryAllocator::RoundUp(VkDeviceSize value, VkDeviceSize alignment) const
	{
		return ((value + alignment - 1) / alignment) * alignment;
	}
	bool MemoryAllocator::UseManagedPool(const VmaAllocationCreateInfo& allocationInfo, VkDeviceSize allocationSize, VkDeviceSize poolBlockSize) const
	{
		// A non-null pool means the caller already selected a specific pool. Do not override that decision.
		if (allocationInfo.pool != nullptr)
			return false;   // -> use VMA's default pool.

		// Dedicated allocations intentionally bypass sub-allocation. This is useful for very large or special resources.
		if (allocationInfo.flags & VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT)
			return false;   // -> use VMA's default pool.

		// Custom pools have fixed block sizes, so larger resources must use VMA's default pool instead.
		return allocationSize <= poolBlockSize;
	}
}