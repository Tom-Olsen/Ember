#include "vulkanContext.h"
#include "iDearImGui.h"
#include "iWindow.h"
#include "rendererCreateInfo.h"
#include "vulkanCompute.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanGarbageCollector.h"
#include "vulkanGraphics.h"
#include "vulkanLighting.h"
#include "vulkanMacros.h"
#include "vulkanPoolManager.h"
#include "vulkanRenderPassManager.h"
#include "vulkanSingleTimeCommand.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool Context::s_isInitialized = false;
	PFN_vkSetDebugUtilsObjectNameEXT Context::s_vkSetDebugUtilsObjectNameEXT;
	Instance Context::instance;
	PhysicalDevice Context::physicalDevice;
	Surface Context::surface;
	LogicalDevice Context::logicalDevice;
	MemoryAllocator Context::allocator;
	AllocationTracker Context::allocationTracker;
	DescriptorPool Context::descriptorPool;
	Swapchain Context::swapchains[2];
	uint32_t Context::swapchainIndex;
	uint32_t Context::framesInFlight;
	uint32_t Context::frameIndex;
	uint64_t Context::absoluteFrameIndex;
	VkSampleCountFlagBits Context::msaaSamples;
	bool Context::enableDockSpace;
	float Context::depthBiasConstantFactor = 0.0f;
	float Context::depthBiasClamp = 0.0f;
	float Context::depthBiasSlopeFactor = 1.0f;



	// Initialization/Cleanup:
	void Context::Init(const emberEngine::RendererCreateInfo& createInfo)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		framesInFlight = createInfo.framesInFlight;
		frameIndex = 0;
		absoluteFrameIndex = 0;
		enableDockSpace = createInfo.enableDockSpace;

		// Init static utility:
		Lighting::Init(createInfo.maxDirectionalLights, createInfo.maxPositionalLights, createInfo.shadowMapResolution);
		Compute::Init();
		RenderPassManager::Init(createInfo.renderWidth, createInfo.renderHeight);
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
		instance.Init(instanceExtensions);
		physicalDevice.Init(&instance);
		surface.Init(&instance, &physicalDevice, createInfo.pIWindow, createInfo.vSyncEnabled);
		logicalDevice.Init(&physicalDevice, &surface, deviceExtensions);
		allocator.Init(&instance, &logicalDevice, &physicalDevice);
		descriptorPool.Init(&logicalDevice);
		swapchains[0].Init(&logicalDevice, &surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		swapchainIndex = 0;

		// Set msaa sampling value:
		msaaSamples = std::min(static_cast<VkSampleCountFlagBits>(createInfo.msaaSampleCount), physicalDevice.GetMaxMsaaSamples());

		// Load vulkan debug utility object naming function:
		#if defined(VALIDATION_LAYERS_ACTIVE)
		s_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(GetVkDevice(), "vkSetDebugUtilsObjectNameEXT"));
		#endif

		// Init static utility:
		SingleTimeCommand::Init();
		DefaultGpuResources::Init();
		PoolManager::Init();
		Graphics::Init();

		// Debug naming:
		if (logicalDevice.GetGraphicsQueue().queue == logicalDevice.GetPresentQueue().queue)
			NAME_VK_QUEUE(logicalDevice.GetGraphicsQueue().queue, "graphicsAndPresentQueue");
		else
		{
			NAME_VK_QUEUE(logicalDevice.GetGraphicsQueue().queue, "graphicsQueue");
			NAME_VK_QUEUE(logicalDevice.GetPresentQueue().queue, "presentQueue");
		}
		NAME_VK_QUEUE(logicalDevice.GetComputeQueue().queue, "computeQueue");
		NAME_VK_QUEUE(logicalDevice.GetTransferQueue().queue, "transferQueue");
	}
	void Context::Clear()
	{
		WaitDeviceIdle();
		Graphics::Clear(),
		PoolManager::Clear();
		DefaultGpuResources::Clear();
		SingleTimeCommand::Clear();
		GarbageCollector::Clear();
		RenderPassManager::Clear();
		Compute::Clear();
		Lighting::Clear();
	}
	void Context::RebuildSwapchain()
	{
		//int nextSwapchainIndex = (int)!swapchainIndex;
		//swapchains[nextSwapchainIndex].Init(&logicalDevice, &surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, &swapchains[swapchainIndex]);
		//swapchains[swapchainIndex].Clear();
		//swapchainIndex = nextSwapchainIndex;

		int nextSwapchainIndex = (int)!swapchainIndex;
		swapchains[nextSwapchainIndex] = std::move(swapchains[swapchainIndex]);
		swapchainIndex = nextSwapchainIndex;
	}



	// Getters:
	const VkInstance& Context::GetVkInstance()
	{
		return instance.GetVkInstance();
	}
	const VkPhysicalDevice& Context::GetVkPhysicalDevice()
	{
		return physicalDevice.GetVkPhysicalDevice();
	}
	const VkSurfaceKHR& Context::GetVkSurfaceKHR()
	{
		return surface.GetVkSurfaceKHR();
	}
	const VkDevice& Context::GetVkDevice()
	{
		return logicalDevice.GetVkDevice();
	}
	const VmaAllocator& Context::GetVmaAllocator()
	{
		return allocator.GetVmaAllocator();
	}
	const VkDescriptorPool& Context::GetVkDescriptorPool()
	{
		return descriptorPool.GetVkDescriptorPool();
	}
	const VkSwapchainKHR& Context::GetVkSwapchainKHR()
	{
		return swapchains[swapchainIndex].GetVkSwapchainKHR();
	}
	bool Context::DepthClampEnabled()
	{
		return physicalDevice.SupportsDepthClamp();
	}
	bool Context::DepthBiasClampEnabled()
	{
		return physicalDevice.SupportsDepthBiasClamp();
	}



	// Public frame logic:
	void Context::UpdateFrameIndex()
	{
		frameIndex = (frameIndex + 1) % framesInFlight;
		absoluteFrameIndex++;
	}
	void Context::ResetFrameIndex()
	{
		frameIndex = 0;
	}
	void Context::WaitDeviceIdle()
	{
		VKA(vkDeviceWaitIdle(GetVkDevice()));
	}

	// Object naming:
	void Context::SetObjectName(VkObjectType objectType, uint64_t objectHandle, const std::string& name)
	{
		#if defined(VALIDATION_LAYERS_ACTIVE)
		VkDebugUtilsObjectNameInfoEXT nameInfo = {};
		nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		nameInfo.objectType = objectType;
		nameInfo.objectHandle = objectHandle;
		nameInfo.pObjectName = name.c_str();

		s_vkSetDebugUtilsObjectNameEXT(GetVkDevice(), &nameInfo);
		#endif
	}
	void Context::SetObjectName(VkBuffer buffer, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_BUFFER, (uint64_t)buffer, name);
	}
	void Context::SetObjectName(VkImage image, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_IMAGE, (uint64_t)image, name);
	}
	void Context::SetObjectName(VkImageView imageView, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)imageView, name);
	}
	void Context::SetObjectName(VkBufferView bufferView, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_BUFFER_VIEW, (uint64_t)bufferView, name);
	}
	void Context::SetObjectName(VkDeviceMemory memory, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, (uint64_t)memory, name);
	}
	void Context::SetObjectName(VkShaderModule shaderModule, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t)shaderModule, name);
	}
	void Context::SetObjectName(VkPipeline pipeline, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_PIPELINE, (uint64_t)pipeline, name);
	}
	void Context::SetObjectName(VkPipelineLayout layout, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, (uint64_t)layout, name);
	}
	void Context::SetObjectName(VkRenderPass renderPass, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_RENDER_PASS, (uint64_t)renderPass, name);
	}
	void Context::SetObjectName(VkFramebuffer framebuffer, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_FRAMEBUFFER, (uint64_t)framebuffer, name);
	}
	void Context::SetObjectName(VkDescriptorSetLayout layout, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, (uint64_t)layout, name);
	}
	void Context::SetObjectName(VkDescriptorSet descriptorSet, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET, (uint64_t)descriptorSet, name);
	}
	void Context::SetObjectName(VkDescriptorPool pool, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_DESCRIPTOR_POOL, (uint64_t)pool, name);
	}
	void Context::SetObjectName(VkSampler sampler, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_SAMPLER, (uint64_t)sampler, name);
	}
	void Context::SetObjectName(VkCommandPool commandPool, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_COMMAND_POOL, (uint64_t)commandPool, name);
	}
	void Context::SetObjectName(VkCommandBuffer commandBuffer, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)commandBuffer, name);
	}
	void Context::SetObjectName(VkQueue queue, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_QUEUE, (uint64_t)queue, name);
	}
	void Context::SetObjectName(VkSemaphore semaphore, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)semaphore, name);
	}
	void Context::SetObjectName(VkFence fence, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_FENCE, (uint64_t)fence, name);
	}
	void Context::SetObjectName(VkEvent event, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_EVENT, (uint64_t)event, name);
	}
	void Context::SetObjectName(VkQueryPool queryPool, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_QUERY_POOL, (uint64_t)queryPool, name);
	}
	void Context::SetObjectName(VkSwapchainKHR swapchain, const std::string& name)
	{
		SetObjectName(VK_OBJECT_TYPE_SWAPCHAIN_KHR, (uint64_t)swapchain, name);
	}



	// Private methods:
	bool Context::IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties, const char* extension)
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