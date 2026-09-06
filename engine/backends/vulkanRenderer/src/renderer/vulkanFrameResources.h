#pragma once
#include "vulkanCommandPool.h"
#include "vulkanRenderStage.h"
#include <array>
#include <cstddef>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Persistent Vulkan objects belonging to a frameInFlight slot.
	/// </summary>
	struct FrameResources
	{
	public: // Members:
		std::vector<CommandPool> commandPools;
		std::array<VkDescriptorSet, 3> staticDescriptorSets;

		// VkFence frameFence;
		// VkSemaphore acquireSemaphore;
	
	public: // Methods:
		// Constructor/Destructor:
		FrameResources(int secondaryBufferCount);

		// Getters:
		CommandPool& GetCommandPool(RenderStage renderStage);
		CommandPool& GetCommandPool(int renderStage);
	};
}