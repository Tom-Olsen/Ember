#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanSingleTimeCommand_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanSingleTimeCommand_h__
#include <vulkan/vulkan.h>
#include <string>



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct DeviceQueue;



	class SingleTimeCommand
	{
	private: // Members:
		static VkCommandPool s_graphicsPool;
		static VkCommandPool s_presentPool;
		static VkCommandPool s_computePool;
		static VkCommandPool s_transferPool;
		static VkCommandBuffer s_graphicsBuffer;
		static VkCommandBuffer s_presentBuffer;
		static VkCommandBuffer s_computeBuffer;
		static VkCommandBuffer s_transferBuffer;
		static DeviceQueue s_graphicsDeviceQueue;
		static DeviceQueue s_presentDeviceQueue;
		static DeviceQueue s_computeDeviceQueue;
		static DeviceQueue s_transferDeviceQueue;
		static VkFence s_graphicsFence;
		static VkFence s_presentFence;
		static VkFence s_computeFence;
		static VkFence s_transferFence;
		static VkSemaphore s_semaphore;

	public: // Methods:
		static void Init();
		static void Clear();

		// Getters:
		static VkCommandBuffer& BeginCommand(const DeviceQueue& queue);
		static void EndCommand(const DeviceQueue& queue);
		static void EndLinkedCommands(const DeviceQueue& firstQueue, const DeviceQueue& secondQueue, VkPipelineStageFlags2 waitDstStageMask);

	private: // Methods
		static VkCommandBuffer* FindCommandBuffer(const DeviceQueue& queue);
		static VkCommandPool* FindCommandPool(const DeviceQueue& queue);
		static VkFence* FindFence(const DeviceQueue& queue);

		// Delete all constructors:
		SingleTimeCommand() = delete;
		SingleTimeCommand(const SingleTimeCommand&) = delete;
		SingleTimeCommand& operator=(const SingleTimeCommand&) = delete;
		~SingleTimeCommand() = delete;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanSingleTimeCommand_h__