#define VMA_IMPLEMENTATION
#include "vulkanMemoryAllocator.h"
#include "macros.h"



// Constructor:
VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanInstance* instance, VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice)
{
	// References to vulkan functions:
	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	// Define allocator properties:
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.instance = instance->instance;
	allocatorInfo.device = logicalDevice->device;
	allocatorInfo.physicalDevice = physicalDevice->device;
	allocatorInfo.flags = 0; // enable extensions. Must be supported by Vulkan device, see https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html
	allocatorInfo.preferredLargeHeapBlockSize = 0;			// 0 = default size = 256 MiB
	allocatorInfo.pAllocationCallbacks = nullptr;			// optional: custom CPU memory allocation callbacks
	allocatorInfo.pDeviceMemoryCallbacks = nullptr;			// optional: informative callbacks for `vkAllocateMemory`, `vkFreeMemory`
	allocatorInfo.pHeapSizeLimit = nullptr;					// optional: limit on size of Vulkan device memory heaps
	allocatorInfo.pVulkanFunctions = &vulkanFunctions;		// pointers to Vulkan functions
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;	// same as `VulkanInstance::apiVersion`
	allocatorInfo.pTypeExternalMemoryHandleTypes = nullptr;	// optional: array of external memory handle types (don't understand this yet)

	// Create allocator:
	VKA(vmaCreateAllocator(&allocatorInfo, &allocator));
}



// Destructor:
VulkanMemoryAllocator::~VulkanMemoryAllocator()
{
	vmaDestroyAllocator(allocator);
}