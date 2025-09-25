#include "vulkanContext.h"
#include "commonRendererCreateInfo.h"
#include "emberMath.h"
#include "iDearImGui.h"
#include "iWindow.h"
#include "vulkanAllocationTracker.h"
#include "vulkanCompute.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanDescriptorPool.h"
#include "vulkanGarbageCollector.h"
#include "vulkanImageUsageFlag.h"
#include "vulkanInstance.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanMemoryAllocator.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanPoolManager.h"
#include "vulkanRenderPassManager.h"
#include "vulkanSingleTimeCommand.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Vulkan debug utility object naming function pointer:
	PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;



	// Static members:
	bool Context::s_isInitialized = false;
	std::unique_ptr<Instance> Context::m_pInstance;
	std::unique_ptr<PhysicalDevice> Context::m_pPhysicalDevice;
	std::unique_ptr<Surface> Context::m_pSurface;
	std::unique_ptr<LogicalDevice> Context::m_pLogicalDevice;
	std::unique_ptr<MemoryAllocator> Context::m_pMemoryAllocator;
	std::unique_ptr<AllocationTracker> Context::m_pAllocationTracker;
	std::unique_ptr<DescriptorPool> Context::m_pDescriptorPool;
	std::array<std::unique_ptr<Swapchain>, 2> Context::m_swapchains;
	uint32_t Context::m_swapchainIndex;
	uint32_t Context::m_framesInFlight;
	uint32_t Context::m_frameIndex;
	uint64_t Context::m_absoluteFrameIndex;
	SampleCountFlag Context::m_msaaSamples;
	bool Context::m_enableDockSpace;
	float Context::m_depthBiasConstantFactor = 0.0f;
	float Context::m_depthBiasClamp = 0.0f;
	float Context::m_depthBiasSlopeFactor = 1.0f;



	// Initialization/Cleanup:
	void Context::Init(const emberEngine::RendererCreateInfo& createInfo)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		m_framesInFlight = createInfo.framesInFlight;
		m_frameIndex = 0;
		m_absoluteFrameIndex = 0;
		m_enableDockSpace = createInfo.enableDockSpace;

		// Init static utility:
		Compute::Init();
		uint32_t maxLightsCount = createInfo.maxDirectionalLights + createInfo.maxPositionalLights;
		RenderPassManager::Init(createInfo.renderWidth, createInfo.renderHeight, createInfo.shadowMapResolution, maxLightsCount);
		GarbageCollector::Init();

		// Get instance extensions:
		std::vector<const char*> instanceExtensions;
		#if defined(VALIDATION_LAYERS_ACTIVE)
		if (IsExtensionAvailable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		if (IsExtensionAvailable(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME))
			instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
		#endif
		createInfo.pIWindow->AddWindowInstanceExtensions(instanceExtensions);	
		if (createInfo.enableDearImGui)
			AddDearImGuiInstanceExtensions(instanceExtensions);
		// and more ...

		// Get device extensions:
		std::vector<const char*> deviceExtensions;
		deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		// These extensions are integrated in Vulkan 1.2 as features, but ImGui relies on them as an extensions => Validation Layer warnings.
		deviceExtensions.emplace_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
		deviceExtensions.emplace_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
		// and more ...

		// Create vulkan context:
		m_pInstance = std::make_unique<Instance>(instanceExtensions);
		m_pPhysicalDevice = std::make_unique<PhysicalDevice>(m_pInstance.get());
		m_pSurface = std::make_unique<Surface>(m_pInstance.get(), m_pPhysicalDevice.get(), createInfo.pIWindow, createInfo.vSyncEnabled);
		m_pLogicalDevice = std::make_unique<LogicalDevice>(m_pPhysicalDevice.get(), m_pSurface.get(), deviceExtensions);
		m_pMemoryAllocator = std::make_unique<MemoryAllocator>(m_pInstance.get(), m_pLogicalDevice.get(), m_pPhysicalDevice.get());
		m_pAllocationTracker = std::make_unique<AllocationTracker>();
		m_pDescriptorPool = std::make_unique<DescriptorPool>(m_pLogicalDevice.get());
		m_swapchains[0] = std::make_unique<Swapchain>(m_pLogicalDevice.get(), m_pSurface.get(), ImageUsageFlags::color_attachment_bit | ImageUsageFlags::transfer_dst_bit);
		m_swapchainIndex = 0;

		// Set msaa sampling value:
		m_msaaSamples = math::Min(static_cast<uint32_t>(createInfo.msaaSampleCount), m_pPhysicalDevice->GetMaxMsaaSamples());

		// Load vulkan debug utility object naming function:
		#if defined(VALIDATION_LAYERS_ACTIVE)
		vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(GetVkDevice(), "vkSetDebugUtilsObjectNameEXT"));
		#endif

		// Init static utility:
		SingleTimeCommand::Init();
		DefaultGpuResources::Init();
		PoolManager::Init();

		// Debug naming:
		if (m_pLogicalDevice->GetGraphicsQueue().queue == m_pLogicalDevice->GetPresentQueue().queue)
			NAME_VK_QUEUE(m_pLogicalDevice->GetGraphicsQueue().queue, "graphicsAndPresentQueue");
		else
		{
			NAME_VK_QUEUE(m_pLogicalDevice->GetGraphicsQueue().queue, "graphicsQueue");
			NAME_VK_QUEUE(m_pLogicalDevice->GetPresentQueue().queue, "presentQueue");
		}
		NAME_VK_QUEUE(m_pLogicalDevice->GetComputeQueue().queue, "computeQueue");
		NAME_VK_QUEUE(m_pLogicalDevice->GetTransferQueue().queue, "transferQueue");
	}
	void Context::Clear()
	{
		WaitDeviceIdle();
		PoolManager::Clear();
		DefaultGpuResources::Clear();
		SingleTimeCommand::Clear();
		GarbageCollector::Clear();
		RenderPassManager::Clear();
		Compute::Clear();
	}
	void Context::RebuildSwapchain()
	{
		int nextm_swapchainIndex = (int)!m_swapchainIndex;
		m_swapchains[nextm_swapchainIndex] = std::move(m_swapchains[m_swapchainIndex]);
		m_swapchainIndex = nextm_swapchainIndex;
	}



	// Getters:
	const Instance* Context::GetInstance()
	{
		return m_pInstance.get();
	}
	const PhysicalDevice* Context::GetPhysicalDevice()
	{
		return m_pPhysicalDevice.get();
	}
	const Surface* Context::GetSurface()
	{
		return m_pSurface.get();
	}
	const LogicalDevice* Context::GetLogicalDevice()
	{
		return m_pLogicalDevice.get();
	}
	const MemoryAllocator* Context::GetMemoryAllocator()
	{
		return m_pMemoryAllocator.get();
	}
	const AllocationTracker* Context::GetAllocationTracker()
	{
		return m_pAllocationTracker.get();
	}
	const DescriptorPool* Context::GetDescriptorPool()
	{
		return m_pDescriptorPool.get();
	}
	const Swapchain* Context::GetSwapchain()
	{
		return m_swapchains[m_swapchainIndex].get();
	}

	const VkInstance Context::GetVkInstance()
	{
		return m_pInstance->GetVkInstance();
	}
	const VkPhysicalDevice Context::GetVkPhysicalDevice()
	{
		return m_pPhysicalDevice->GetVkPhysicalDevice();
	}
	const VkSurfaceKHR Context::GetVkSurfaceKHR()
	{
		return m_pSurface->GetVkSurfaceKHR();
	}
	const VkDevice Context::GetVkDevice()
	{
		return m_pLogicalDevice->GetVkDevice();
	}
	const VmaAllocator Context::GetVmaAllocator()
	{
		return m_pMemoryAllocator->GetVmaAllocator();
	}
	const VkDescriptorPool Context::GetVkDescriptorPool()
	{
		return m_pDescriptorPool->GetVkDescriptorPool();
	}
	const VkSwapchainKHR& Context::GetVkSwapchainKHR()
	{
		return m_swapchains[m_swapchainIndex]->GetVkSwapchainKHR();
	}
	bool Context::DepthClampEnabled()
	{
		return m_pPhysicalDevice->SupportsDepthClamp();
	}
	bool Context::DepthBiasClampEnabled()
	{
		return m_pPhysicalDevice->SupportsDepthBiasClamp();
	}

	uint32_t Context::GetFramesInFlight()
	{
		return m_framesInFlight;
	}
	uint32_t Context::GetFrameIndex()
	{
		return m_frameIndex;
	}
	uint64_t Context::GetAbsoluteFrameIndex()
	{
		return m_absoluteFrameIndex;
	}
	SampleCountFlag Context::GetMsaaSamples()
	{
		return m_msaaSamples;
	}
	bool Context::DockSpaceEnabled()
	{
		return m_enableDockSpace;
	}
	float Context::GetDepthBiasConstantFactor()
	{
		return m_depthBiasConstantFactor;
	}
	float Context::GetDepthBiasClamp()
	{
		return m_depthBiasClamp;
	}
	float Context::GetDepthBiasSlopeFactor()
	{
		return m_depthBiasSlopeFactor;
	}



	// Public frame logic:
	void Context::UpdateFrameIndex()
	{
		m_frameIndex = (m_frameIndex + 1) % m_framesInFlight;
		m_absoluteFrameIndex++;
	}
	void Context::ResetFrameIndex()
	{
		m_frameIndex = 0;
	}
	void Context::WaitDeviceIdle()
	{
		VKA(vkDeviceWaitIdle(GetVkDevice()));
	}

	// Object naming:
	void SetObjectName(VkObjectType objectType, uint64_t objectHandle, const std::string& name)
	{
		#if defined(VALIDATION_LAYERS_ACTIVE)
		VkDebugUtilsObjectNameInfoEXT nameInfo = {};
		nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		nameInfo.objectType = objectType;
		nameInfo.objectHandle = objectHandle;
		nameInfo.pObjectName = name.c_str();

		vkSetDebugUtilsObjectNameEXT(GetVkDevice(), &nameInfo);
		#endif
	}
	void Context::SetObjectName(VkBuffer buffer, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)buffer, name);
	}
	void Context::SetObjectName(VkImage image, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)image, name);
	}
	void Context::SetObjectName(VkImageView imageView, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)imageView, name);
	}
	void Context::SetObjectName(VkBufferView bufferView, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_BUFFER_VIEW, (uint64_t)bufferView, name);
	}
	void Context::SetObjectName(VkDeviceMemory memory, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t)memory, name);
	}
	void Context::SetObjectName(VkShaderModule shaderModule, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t)shaderModule, name);
	}
	void Context::SetObjectName(VkPipeline pipeline, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)pipeline, name);
	}
	void Context::SetObjectName(VkPipelineLayout layout, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)layout, name);
	}
	void Context::SetObjectName(VkRenderPass renderPass, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_RENDER_PASS, (uint64_t)renderPass, name);
	}
	void Context::SetObjectName(VkFramebuffer framebuffer, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_FRAMEBUFFER, (uint64_t)framebuffer, name);
	}
	void Context::SetObjectName(VkDescriptorSetLayout layout, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t)layout, name);
	}
	void Context::SetObjectName(VkDescriptorSet descriptorSet, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET, (uint64_t)descriptorSet, name);
	}
	void Context::SetObjectName(VkDescriptorPool pool, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_POOL, (uint64_t)pool, name);
	}
	void Context::SetObjectName(VkSampler sampler, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_SAMPLER, (uint64_t)sampler, name);
	}
	void Context::SetObjectName(VkCommandPool commandPool, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_COMMAND_POOL, (uint64_t)commandPool, name);
	}
	void Context::SetObjectName(VkCommandBuffer commandBuffer, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)commandBuffer, name);
	}
	void Context::SetObjectName(VkQueue queue, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_QUEUE, (uint64_t)queue, name);
	}
	void Context::SetObjectName(VkSemaphore semaphore, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)semaphore, name);
	}
	void Context::SetObjectName(VkFence fence, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_FENCE, (uint64_t)fence, name);
	}
	void Context::SetObjectName(VkEvent event, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_EVENT, (uint64_t)event, name);
	}
	void Context::SetObjectName(VkQueryPool queryPool, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_QUERY_POOL, (uint64_t)queryPool, name);
	}
	void Context::SetObjectName(VkSwapchainKHR swapchain, const std::string& name)
	{
		vulkanRendererBackend::SetObjectName(VK_OBJECT_TYPE_SWAPCHAIN_KHR, (uint64_t)swapchain, name);
	}



	// Private methods:
	bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties, const char* extension)
	{
		for (const VkExtensionProperties& p : properties)
			if (strcmp(p.extensionName, extension) == 0)
				return true;
		return false;
	}
	void Context::AddDearImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions)
	{
		// Enumerate available extensions:
		uint32_t propertiesCount;
		std::vector<VkExtensionProperties> properties;
		if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr) != VK_SUCCESS)
			throw std::runtime_error((std::string)"Instance::AddDearImGuiInstanceExtensions: " + (std::string)"failed to enumerate instance extension properties!");

		properties.resize(propertiesCount);
		if (vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.data()) != VK_SUCCESS)
			throw std::runtime_error((std::string)"Instance::AddDearImGuiInstanceExtensions: " + (std::string)"failed to enumerate instance extension properties!");

		// Enable required extensions:
		if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
			instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
			instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		#endif
	}
}