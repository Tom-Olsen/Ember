#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanContext_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanContext_h__
// Direct include instead of forward declaration,
// as this is the main header file that includes all other Vulkan-related headers.
// Also allows for vulkan objects to be in place instead of pointers -> better memory access.
#include "vulkanAllocationTracker.h"
#include "vulkanDescriptorPool.h"
#include "vulkanInstance.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMemoryAllocator.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include <memory>
#include <string>
#include <vulkan/vulkan.h>



// vulkanRendererBackend::ToDo:
// -replace all std::cout/std::cerr with logging backendAPI
// -vulkanInstance: implement checks to ensure that all requested instanceExtensions are available and throw warnings accordingly.
// -hide static members in Context struct and only make them accessable via Get/Set methods.
// -allow for the three depthBias values to be set via Set.
// -allow renderWidth/renderHeight to change during runtime.
// -ShaderPropertiesPool/StagingBufferPool: remove ShrinkToFit? some other mechanism to handle unbound growth?
// -defaultGpuResources: initialize texture2d objects.
// -remove name property of classes that dont need it internally. e.g. texture and mesh?



// Forward decleration:
namespace emberBackendInterface
{
	class IWindow;
	class IDearImGuiInstanceExtensionsLoader;
}



namespace vulkanRendererBackend
{
	/// <summary>
	/// Context is a utility structure designed to encapsulate and manage all components required for the renderer.
	/// It integrates multiple Vulkan resources and functionalities into a cohesive framework: <para/>
	/// - Instance:			VkInstance and validation layers. <para/>
	/// - PhysicalDevice:	physical device selection and feature support. <para/>
	/// - Surface:			VkSurface and present mode (mailbox, FIFO, etc.). <para/>
	/// - LogicalDevice:	VkDevice and queues (graphics, present, compute, transfer). <para/>
	/// - MemoryAllocator:	VmaAllocator for flexible memory allocation pools. <para/>
	/// - DescriptorPool:	VkDescriptorPool settings. <para/>
	/// - Swapchain:		VkSwapchainKHR, spwapchain images, image views, and recreation. <para/>
	/// - framesInFlight:	Number of frames in flight for synchronization. <para/>
	/// - frameIndex:		Current frame index for synchronization. <para/>
	/// - msaaSamples:		Msaa level, clamped to the maximum supported by the physical device. <para/>
	/// </summary>
	struct Context
	{
	private: // Members:
		static bool s_isInitialized;
		static PFN_vkSetDebugUtilsObjectNameEXT s_vkSetDebugUtilsObjectNameEXT;

	public: // Members: ("constants")
		static Instance instance;
		static PhysicalDevice physicalDevice;
		static Surface surface;
		static LogicalDevice logicalDevice;
		static MemoryAllocator allocator;
		static AllocationTracker allocationTracker;
		static DescriptorPool descriptorPool;
		static Swapchain swapchains[2];
		static uint32_t swapchainIndex;
		static uint32_t framesInFlight;
		static uint32_t frameIndex;
		static uint64_t absoluteFrameIndex;
		static VkSampleCountFlagBits msaaSamples;
		static bool enableDockSpace;
		static uint32_t maxDirectionalLights;
		static uint32_t maxPositionalLights;
		static uint32_t shadowMapResolution;
		static float depthBiasConstantFactor;
		static float depthBiasClamp;
		static float depthBiasSlopeFactor;
		
	public: // Methods:
		static void Init(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IDearImGuiInstanceExtensionsLoader* pIDearImGuiInstanceExtensionsLoader, uint32_t renderWidth, uint32_t renderHeight, uint32_t framesInFlight_, VkSampleCountFlagBits msaaSamples_, bool vSyncEnabled_, bool enableDockSpace_, uint32_t maxDirectionalLights_, uint32_t maxPositionalLights_, uint32_t shadowMapResolution_);
		static void Clear();
		static void RebuildSwapchain();

		// Getters:
		static const VkInstance& GetVkInstance();
		static const VkPhysicalDevice& GetVkPhysicalDevice();
		static const VkSurfaceKHR& GetVkSurfaceKHR();
		static const VkDevice& GetVkDevice();
		static const VmaAllocator& GetVmaAllocator();
		static const VkDescriptorPool& GetVkDescriptorPool();
		static const VkSwapchainKHR& GetVkSwapchainKHR();
		static bool DepthClampEnabled();
		static bool DepthBiasClampEnabled();

		// Frame logic:
		static void UpdateFrameIndex();
		static void ResetFrameIndex();
		static void WaitDeviceIdle();

		// Debug utility object naming functions:
	private:
		static void SetObjectName(VkObjectType objectType, uint64_t objectHandle, const std::string& name);
	public:
		static void SetObjectName(VkBuffer buffer, const std::string& name);
		static void SetObjectName(VkImage image, const std::string& name);
		static void SetObjectName(VkImageView imageView, const std::string& name);
		static void SetObjectName(VkBufferView bufferView, const std::string& name);
		static void SetObjectName(VkDeviceMemory memory, const std::string& name);
		static void SetObjectName(VkShaderModule shaderModule, const std::string& name);
		static void SetObjectName(VkPipeline pipeline, const std::string& name);
		static void SetObjectName(VkPipelineLayout layout, const std::string& name);
		static void SetObjectName(VkRenderPass renderPass, const std::string& name);
		static void SetObjectName(VkFramebuffer framebuffer, const std::string& name);
		static void SetObjectName(VkDescriptorSetLayout layout, const std::string& name);
		static void SetObjectName(VkDescriptorSet descriptorSet, const std::string& name);
		static void SetObjectName(VkDescriptorPool pool, const std::string& name);
		static void SetObjectName(VkSampler sampler, const std::string& name);
		static void SetObjectName(VkCommandPool commandPool, const std::string& name);
		static void SetObjectName(VkCommandBuffer commandBuffer, const std::string& name);
		static void SetObjectName(VkQueue queue, const std::string& name);
		static void SetObjectName(VkSemaphore semaphore, const std::string& name);
		static void SetObjectName(VkFence fence, const std::string& name);
		static void SetObjectName(VkEvent event, const std::string& name);
		static void SetObjectName(VkQueryPool queryPool, const std::string& name);
		static void SetObjectName(VkSwapchainKHR swapchain, const std::string& name);
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanContext_h__