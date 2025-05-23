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



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		MemoryAllocator::MemoryAllocator()
		{
			m_pAllocator = VMA_NULL;
		}
		void MemoryAllocator::Init(Instance* pInstance, LogicalDevice* pLogicalDevice, PhysicalDevice* pPhysicalDevice)
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
			vmaDestroyAllocator(m_pAllocator);
		}



		// Public methods:
		const VmaAllocator& MemoryAllocator::GetVmaAllocator() const
		{
			return m_pAllocator;
		}
	}
}