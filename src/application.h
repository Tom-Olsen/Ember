#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include <memory>
#include <vector>
#include <chrono>
#include "vulkanContext.h"
#include "vulkanMsaaImage.h"
#include "vulkanDepthImage.h"
#include "vulkanFrameBuffers.h"
#include "vulkanCommand.h"
#include "meshManager.h"
#include "materialManager.h"
#include "textureManager.h"
#include "samplerManager.h"
#include "scene.h"



/// <summary>
/// Main application class.
/// </summary>
class Application
{
public: // Members:
	std::shared_ptr<VulkanContext> context;

private: // Members:
	// Render resources:
	std::unique_ptr<VulkanMsaaImage> msaaImage;
	std::unique_ptr<VulkanDepthImage> depthImage;
	std::unique_ptr<VulkanFrameBuffers> frameBuffers;
	std::vector<VulkanCommand> commands;

	// Sync objects:
	std::vector<VkFence> fences;
	std::vector<VkSemaphore> acquireSemaphores;
	std::vector<VkSemaphore> releaseSemaphores;

	// Data:
	Scene* scene;
	UniformObject uniformObject;

	// Render management:
	double time;
	double deltaTime;
	uint32_t imageIndex;
	bool rebuildSwapchain;

public: // Methods:
	Application(std::shared_ptr<VulkanContext> context);
	~Application();
	void Run();
	void SetScene(Scene* scene);

private: // Methods:
	void Render();
	bool AcquireImage();
	void RecordCommandBuffer();
	void SubmitCommandBuffer();
	bool PresentImage();
	void SetViewportAndScissor(VkCommandBuffer& commandBuffer);
	void ResizeSwapchain();
	void CreateFences();
	void CreateSemaphores();
	void DestroyFences();
	void DestroySemaphores();
};



#endif // __INCLUDE_GUARD_application_h__