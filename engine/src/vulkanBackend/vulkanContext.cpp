#include "vulkanContext.h"
#include "dearImGui.h"
#include "logger.h"
#include "macros.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Static members:
		bool Context::s_isInitialized = false;
		PFN_vkSetDebugUtilsObjectNameEXT Context::s_vkSetDebugUtilsObjectNameEXT;
		SdlWindow Context::window;
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
			window.Init(windowWidth, windowHeight);

			// Get instance extensions:
			std::vector<const char*> instanceExtensions;
			#if defined(VALIDATION_LAYERS_ACTIVE)
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
			#endif
			window.AddSdlInstanceExtensions(instanceExtensions);			// sdl instance extensions
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
			instance.Init(instanceExtensions);
			physicalDevice.Init(&instance);
			surface.Init(&instance, &physicalDevice, &window, vSyncEnabled);
			logicalDevice.Init(&physicalDevice, &surface, deviceExtensions);
			allocator.Init(&instance, &logicalDevice, &physicalDevice);
			allocationTracker.Init();
			descriptorPool.Init(&logicalDevice);
			swapchains[0].Init(&logicalDevice, &surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
			swapchainIndex = 0;

			// Set msaa sampling value:
			msaaSamples = std::min(msaaSamplesValue, physicalDevice.GetMaxMsaaSamples());

			// Load vulkan debug utility object naming function:
			#if defined(VALIDATION_LAYERS_ACTIVE)
			s_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(GetVkDevice(), "vkSetDebugUtilsObjectNameEXT"));
			#endif

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

			#ifdef LOG_INITIALIZATION
			LOG_TRACE("VulkanContext initialized.");
			#endif
		}
		void Context::Clear()
		{
			WaitDeviceIdle();
		}
		void Context::RebuildSwapchain()
		{
			swapchains[(int)!swapchainIndex].Init(&logicalDevice, &surface, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, &swapchains[swapchainIndex]);
			swapchains[swapchainIndex].Clear();
			swapchainIndex = (int)!swapchainIndex;
		}



		// Getters:
		SDL_Window* const Context::GetSDL_Window()
		{
			return window.GetSDL_Window();
		}
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
			VKA(vkDeviceWaitIdle(logicalDevice.GetVkDevice()));
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