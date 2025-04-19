#include "vulkanContext.h"
#include "dearImGui.h"
#include "logger.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Static members:
		bool Context::s_isInitialized = false;
		PFN_vkSetDebugUtilsObjectNameEXT Context::s_vkSetDebugUtilsObjectNameEXT;
		std::unique_ptr<SdlWindow> Context::pWindow = nullptr;
		std::unique_ptr<Instance> Context::pInstance = nullptr;
		std::unique_ptr<PhysicalDevice> Context::pPhysicalDevice = nullptr;
		std::unique_ptr<Surface> Context::pSurface = nullptr;
		std::unique_ptr<LogicalDevice> Context::pLogicalDevice = nullptr;
		std::unique_ptr<MemoryAllocator> Context::pAllocator = nullptr;
		std::unique_ptr< AllocationTracker> Context::pAllocationTracker = nullptr;
		std::unique_ptr<DescriptorPool> Context::pDescriptorPool = nullptr;
		std::unique_ptr<Swapchain> Context::pSwapchain = nullptr;
		uint32_t Context::framesInFlight;
		uint32_t Context::frameIndex;
		uint64_t Context::absoluteFrameIndex;
		VkSampleCountFlagBits Context::msaaSamples;



		// Initialization and cleanup:
		void Context::Init(uint32_t framesInFlightValue, VkSampleCountFlagBits msaaSamplesValue, uint32_t windowWidth, uint32_t windowHeight, bool vSyncEnabled)
		{
			if (s_isInitialized)
				return;
			s_isInitialized = true;

			framesInFlight = framesInFlightValue;
			frameIndex = 0;
			absoluteFrameIndex = 0;

			// Window:
			pWindow = std::make_unique<SdlWindow>(windowWidth, windowHeight);

			// Get instance extensions:
			std::vector<const char*> instanceExtensions;
			#if defined(VALIDATION_LAYERS_ACTIVE)
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
			#endif
			pWindow->AddSdlInstanceExtensions(instanceExtensions);		// sdl instance extensions
			DearImGui::AddImGuiInstanceExtensions(instanceExtensions);	// add instance extensions for docking feature
			// and more ...

			// Get device extensions:
			std::vector<const char*> deviceExtensions;
			deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			// These extensions are integrated in Vulkan 1.2 as features, but ImGui relies on them as an extensions => Validation Layer warnings.
			deviceExtensions.emplace_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
			deviceExtensions.emplace_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
			// and more ...

			// Create vulkan context:
			pInstance = std::make_unique<Instance>(instanceExtensions);
			pPhysicalDevice = std::make_unique<PhysicalDevice>(pInstance.get());
			pSurface = std::make_unique<Surface>(pInstance.get(), pPhysicalDevice.get(), pWindow.get(), vSyncEnabled);
			pLogicalDevice = std::make_unique<LogicalDevice>(pPhysicalDevice.get(), pSurface.get(), deviceExtensions);
			pAllocator = std::make_unique<MemoryAllocator>(pInstance.get(), pLogicalDevice.get(), pPhysicalDevice.get());
			pAllocationTracker = std::make_unique<AllocationTracker>();
			pDescriptorPool = std::make_unique<DescriptorPool>(pLogicalDevice.get());
			pSwapchain = std::make_unique<Swapchain>(pLogicalDevice.get(), pSurface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

			// Set msaa sampling value:
			msaaSamples = std::min(msaaSamplesValue, pPhysicalDevice->GetMaxMsaaSamples());

			// Load vulkan debug utility object naming function:
			#if defined(VALIDATION_LAYERS_ACTIVE)
			s_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(GetVkDevice(), "vkSetDebugUtilsObjectNameEXT"));
			#endif
		}
		void Context::Clear()
		{
			WaitDeviceIdle();
			pSwapchain.reset();
			pDescriptorPool.reset();
			pAllocationTracker.reset();
			pAllocator.reset();
			pLogicalDevice.reset();
			pSurface.reset();
			pPhysicalDevice.reset();
			pInstance.reset();
			pWindow.reset();
		}



		// Getters:
		SDL_Window* const Context::GetSDL_Window()
		{
			return pWindow->GetSDL_Window();
		}
		const VkInstance& Context::GetVkInstance()
		{
			return pInstance->GetVkInstance();
		}
		const VkPhysicalDevice& Context::GetVkPhysicalDevice()
		{
			return pPhysicalDevice->GetVkPhysicalDevice();
		}
		const VkSurfaceKHR& Context::GetVkSurfaceKHR()
		{
			return pSurface->GetVkSurfaceKHR();
		}
		const VkDevice& Context::GetVkDevice()
		{
			return pLogicalDevice->GetVkDevice();
		}
		const VmaAllocator& Context::GetVmaAllocator()
		{
			return pAllocator->GetVmaAllocator();
		}
		const VkDescriptorPool& Context::GetVkDescriptorPool()
		{
			return pDescriptorPool->GetVkDescriptorPool();
		}
		const VkSwapchainKHR& Context::GetVkSwapchainKHR()
		{
			return pSwapchain->GetVkSwapchainKHR();
		}
		bool Context::DepthClampEnabled()
		{
			return pPhysicalDevice->SupportsDepthClamp();
		}
		bool Context::DepthBiasClampEnabled()
		{
			return pPhysicalDevice->SupportsDepthBiasClamp();
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
			VKA(vkDeviceWaitIdle(pLogicalDevice->GetVkDevice()));
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
	}
}