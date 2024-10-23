#ifndef __INCLUDE_GUARD_vulkanRenderer_h__
#define __INCLUDE_GUARD_vulkanRenderer_h__
#include <memory>
#include <vector>
#include "vulkanContext.h"
#include "forwardRenderPass.h"
#include "shadowRenderPass.h"
#include "vulkanCommand.h"

// Move this in application
#include "meshManager.h"
#include "materialManager.h"
#include "textureManager.h"
#include "samplerManager.h"

#include "scene.h"



/// <summary>
/// Main application class.
/// </summary>
class VulkanRenderer
{
public: // Members:
	VulkanContext* context;
	std::unique_ptr<ForwardRenderPass> forwardRenderPass;
	std::unique_ptr<ShadowRenderPass> shadowRenderPass;

private: // Members:
	// Render resources:
	std::vector<VulkanCommand> shadowCommands;
	std::vector<VulkanCommand> forwardCommands;

	// Sync objects:
	std::vector<VkFence> fences;
	std::vector<VkSemaphore> acquireSemaphores;
	std::vector<VkSemaphore> releaseSemaphores;

	// Render management:
	uint32_t imageIndex;
	bool rebuildSwapchain;

public: // Methods:
	VulkanRenderer(VulkanContext* context);
	~VulkanRenderer();
	void Render(Scene* scene);
	void ResizeSwapchain();

private: // Methods:
	bool AcquireImage();
	void RecordShadowCommandBuffer(Scene* scene);
	void RecordCommandBuffer(Scene* scene);
	void SubmitCommandBuffer();
	bool PresentImage();
	void SetViewportAndScissor(VkCommandBuffer& commandBuffer);
	void CreateFences();
	void CreateSemaphores();
	void DestroyFences();
	void DestroySemaphores();
};



#endif // __INCLUDE_GUARD_vulkanRenderer_h__