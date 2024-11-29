#ifndef __INCLUDE_GUARD_vulkanRenderer_h__
#define __INCLUDE_GUARD_vulkanRenderer_h__
#include <memory>
#include <vector>
#include "vulkanContext.h"
#include "vulkanCommand.h"
#include "scene.h"



class VulkanRenderer
{
public: // Members:
	VulkanContext* context;

private: // Members:
	// Render resources:
	std::vector<VulkanCommand> shadowCommands;
	std::vector<VulkanCommand> shadingCommands;
	
	// Sync objects:
	std::vector<VkFence> fences;
	std::vector<VkSemaphore> acquireSemaphores;
	std::vector<VkSemaphore> shadowToShadingSemaphores;
	std::vector<VkSemaphore> releaseSemaphores;

	// Render management:
	uint32_t imageIndex;
	bool rebuildSwapchain;

public: // Methods:
	VulkanRenderer(VulkanContext* context);
	~VulkanRenderer();
	bool Render(Scene* scene);

private: // Methods:
	void RebuildSwapchain();
	bool AcquireImage();
	void RecordShadowCommandBuffer(Scene* scene);
	void RecordShadingCommandBuffer(Scene* scene);
	void SubmitCommandBuffers();
	bool PresentImage();
	void SetViewportAndScissor(VkCommandBuffer& commandBuffer);
	void CreateFences();
	void CreateSemaphores();
	void DestroyFences();
	void DestroySemaphores();
};



#endif // __INCLUDE_GUARD_vulkanRenderer_h__