#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include <memory>
#include <vector>
#include <chrono>
#include "sdlWindow.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include "vulkanRenderpass.h"
#include "vulkanPipeline.h"
#include "vulkanFrameBuffers.h"
#include "vulkanCommands.h"
#include "vulkanVertexBuffer.h"
#include "vulkanIndexBuffer.h"
#include "mesh.h"



class Application
{
private: // Members:
	std::unique_ptr<SdlWindow> window;
	std::unique_ptr<VulkanInstance> instance;
	std::unique_ptr<VulkanPhysicalDevice> physicalDevice;
	std::unique_ptr<VulkanLogicalDevice> logicalDevice;
	std::unique_ptr<VulkanSurface> surface;
	std::unique_ptr<VulkanSwapchain> swapchain;
	std::unique_ptr<VulkanRenderpass> renderpass;
	std::unique_ptr<VulkanPipeline> pipeline;
	std::unique_ptr<VulkanFrameBuffers> frameBuffers;
	const static size_t framesInFlight = 2;
	std::unique_ptr<VulkanCommands> commands;
	std::vector<VkFence> fences;
	std::vector<VkSemaphore> acquireSemaphores;
	std::vector<VkSemaphore> releaseSemaphores;

	std::unique_ptr<Mesh> mesh;
	std::unique_ptr<VulkanVertexBuffer> vertexBuffer;
	std::unique_ptr<VulkanIndexBuffer> indexBuffer;
	uint32_t frameIndex;

public: // Methods:
	Application();
	~Application();
	void Run();

private: // Methods:
	void PrintApplicationStatus();
	void Render();
	uint32_t AquireImage();
	void RecordCommandBuffer(uint32_t imageIndex);
	void SubmitCommandBuffer();
	void PresentImage(uint32_t imageIndex);
	void SetViewportAndScissor(VkCommandBuffer& commandBuffer);
	void Resize();
	void CreateFences();
	void CreateSemaphores();
	void DestroyFences();
	void DestroySemaphores();
};



#endif // __INCLUDE_GUARD_application_h__