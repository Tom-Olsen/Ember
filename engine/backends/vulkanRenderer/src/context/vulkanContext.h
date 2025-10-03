#pragma once
// Direct include instead of forward declaration,
// as this is the main header file that includes all other Vulkan-related headers.
// Also allows for vulkan objects to be in place instead of pointers -> better memory access.
#include "commonRendererCreateInfo.h"
#include "vulkanSampleCountFlag.h"
#include <array>
#include <memory>
#include <string>
#include <vector>



// vulkanRendererBackend::ToDo:
// -vulkanInstance: implement checks to ensure that all requested instanceExtensions are available and throw warnings accordingly.
// -hide static members in Context struct and only make them accessable via Get/Set methods.
// -allow for the three depthBias values to be set via Set.
// -allow renderWidth/renderHeight to change during runtime.
// -ShaderPropertiesPool/StagingBufferPool: remove ShrinkToFit? some other mechanism to handle unbound growth?
// -remove name property of classes that dont need it internally. e.g. texture and mesh?
// -in math library change data containers to std::array<float,n>
// -remove shader folder from vulkanBackend and use shaders directly from core?



// Forward decleration:
namespace emberBackendInterface
{
	class IDearImGuiInstanceExtensionsLoader;
	class IWindow;
}
typedef struct VkInstance_T* VkInstance;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VmaAllocator_T* VmaAllocator;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
typedef struct VkDevice_T* VkDevice;
typedef struct VkDescriptorPool_T* VkDescriptorPool;
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkBuffer_T* VkBuffer;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VkBufferView_T* VkBufferView;
typedef struct VkDeviceMemory_T* VkDeviceMemory;
typedef struct VkShaderModule_T* VkShaderModule;
typedef struct VkPipeline_T* VkPipeline;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
typedef struct VkRenderPass_T* VkRenderPass;
typedef struct VkFramebuffer_T* VkFramebuffer;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;
typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorPool_T* VkDescriptorPool;
typedef struct VkSampler_T* VkSampler;
typedef struct VkCommandPool_T* VkCommandPool;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkQueue_T* VkQueue;
typedef struct VkSemaphore_T* VkSemaphore;
typedef struct VkFence_T* VkFence;
typedef struct VkEvent_T* VkEvent;
typedef struct VkQueryPool_T* VkQueryPool;
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;



namespace vulkanRendererBackend
{
	// Forward decleration:
	class Instance;
	class PhysicalDevice;
	class Surface;
	class LogicalDevice;
	class MemoryAllocator;
	class AllocationTracker;
	class DescriptorPool;
	class Swapchain;



	/// <summary>
	/// Context is a utility class designed to encapsulate and manage all components required for the renderer.
	/// It integrates multiple Vulkan resources and functionalities into a cohesive framework: <para/>
	/// - Instance:				VkInstance and validation layers. <para/>
	/// - PhysicalDevice:		physical device selection and feature support. <para/>
	/// - Surface:				VkSurface and present mode (mailbox, FIFO, etc.). <para/>
	/// - LogicalDevice:		VkDevice and queues (graphics, present, compute, transfer). <para/>
	/// - MemoryAllocator:		VmaAllocator for flexible memory allocation pools. <para/>
	/// - AllocationTracker:	tracks vmaBuffer and vmaImage creation and destruction. <para/>
	/// - DescriptorPool:		VkDescriptorPool settings. <para/>
	/// - Swapchain:			VkSwapchainKHR, spwapchain images, image views, and recreation. <para/>
	/// - framesInFlight:		Number of frames in flight for synchronization. <para/>
	/// - frameIndex:			Current frame index for synchronization. <para/>
	/// - msaaSamples:			Msaa level, clamped to the maximum supported by the physical device. <para/>
	/// </summary>
	class Context
	{
	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<Instance> m_pInstance;
		static std::unique_ptr<PhysicalDevice> m_pPhysicalDevice;
		static std::unique_ptr<Surface> m_pSurface;
		static std::unique_ptr<LogicalDevice> m_pLogicalDevice;
		static std::unique_ptr<MemoryAllocator> m_pMemoryAllocator;
		static std::unique_ptr<AllocationTracker> m_pAllocationTracker;
		static std::unique_ptr<DescriptorPool> m_pDescriptorPool;
		static std::array<std::unique_ptr<Swapchain>, 2> m_swapchains;
		static uint32_t m_swapchainIndex;
		static uint32_t m_framesInFlight;
		static uint32_t m_frameIndex;
		static uint64_t m_absoluteFrameIndex;
		static SampleCountFlag m_msaaSamples;
		static bool m_enableDockSpace;
		static float m_depthBiasConstantFactor;
		static float m_depthBiasClamp;
		static float m_depthBiasSlopeFactor;
		
	public: // Methods:
		static void Init(const emberCommon::RendererCreateInfo& createInfo);
		static void Clear();
		static void RebuildSwapchain();

		// Getters:
		static const Instance* GetInstance();
		static const PhysicalDevice* GetPhysicalDevice();
		static const Surface* GetSurface();
		static const LogicalDevice* GetLogicalDevice();
		static const MemoryAllocator* GetMemoryAllocator();
		static AllocationTracker* GetAllocationTracker();
		static const DescriptorPool* GetDescriptorPool();
		static const Swapchain* GetSwapchain();

		static const VkInstance GetVkInstance();
		static const VkPhysicalDevice GetVkPhysicalDevice();
		static const VkSurfaceKHR GetVkSurfaceKHR();
		static const VkDevice GetVkDevice();
		static const VmaAllocator GetVmaAllocator();
		static const VkDescriptorPool GetVkDescriptorPool();
		static const VkSwapchainKHR& GetVkSwapchainKHR();
		static bool DepthClampEnabled();
		static bool DepthBiasClampEnabled();

		static uint32_t GetFramesInFlight();
		static uint32_t GetFrameIndex();
		static uint64_t GetAbsoluteFrameIndex();
		static SampleCountFlag GetMsaaSamples();
		static bool DockSpaceEnabled();
		static float GetDepthBiasConstantFactor();
		static float GetDepthBiasClamp();
		static float GetDepthBiasSlopeFactor();

		// Frame logic:
		static void UpdateFrameIndex();
		static void ResetFrameIndex();
		static void WaitDeviceIdle();

		// Debug utility object naming functions:
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