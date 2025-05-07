#include "vulkanSingleTimeCommand.h"
#include "vulkanContext.h"
#include "vulkanDeviceQueue.h"
#include "vulkanMacros.h"
#include <stdexcept>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Static members:
		VkCommandPool SingleTimeCommand::s_graphicsPool;
		VkCommandPool SingleTimeCommand::s_presentPool;
		VkCommandPool SingleTimeCommand::s_computePool;
		VkCommandPool SingleTimeCommand::s_transferPool;
		VkCommandBuffer SingleTimeCommand::s_graphicsBuffer;
		VkCommandBuffer SingleTimeCommand::s_presentBuffer;
		VkCommandBuffer SingleTimeCommand::s_computeBuffer;
		VkCommandBuffer SingleTimeCommand::s_transferBuffer;
		DeviceQueue SingleTimeCommand::s_graphicsDeviceQueue;
		DeviceQueue SingleTimeCommand::s_presentDeviceQueue;
		DeviceQueue SingleTimeCommand::s_computeDeviceQueue;
		DeviceQueue SingleTimeCommand::s_transferDeviceQueue;
		VkFence SingleTimeCommand::s_graphicsFence;
		VkFence SingleTimeCommand::s_presentFence;
		VkFence SingleTimeCommand::s_computeFence;
		VkFence SingleTimeCommand::s_transferFence;



		// Initialization and cleanup:
		void SingleTimeCommand::Init()
		{
			s_graphicsDeviceQueue = Context::pLogicalDevice->GetGraphicsQueue();
			s_presentDeviceQueue = Context::pLogicalDevice->GetPresentQueue();
			s_computeDeviceQueue = Context::pLogicalDevice->GetComputeQueue();
			s_transferDeviceQueue = Context::pLogicalDevice->GetTransferQueue();
			VkCommandPool* pools[4] = { &s_graphicsPool , &s_presentPool , &s_computePool , &s_transferPool };
			VkCommandBuffer* buffers[4] = { &s_graphicsBuffer , &s_presentBuffer , &s_computeBuffer , &s_transferBuffer };
			DeviceQueue* deviceQueues[4] = { &s_graphicsDeviceQueue, &s_presentDeviceQueue, &s_computeDeviceQueue, &s_transferDeviceQueue };
			VkFence* fences[4] = { &s_graphicsFence, &s_presentFence, &s_computeFence, &s_transferFence };

			for (int i = 0; i < 4; i++)
			{
				// Create pools:
				VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
				createInfo.queueFamilyIndex = deviceQueues[i]->familyIndex;
				VKA(vkCreateCommandPool(Context::GetVkDevice(), &createInfo, nullptr, pools[i]));

				// Create buffers:
				VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
				allocateInfo.commandPool = *pools[i];
				allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocateInfo.commandBufferCount = 1;
				VKA(vkAllocateCommandBuffers(Context::GetVkDevice(), &allocateInfo, buffers[i]));
				
				// Create fences:
				VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
				VKA(vkCreateFence(Context::GetVkDevice(), &fenceInfo, nullptr, fences[i]));
			}
		}
		void SingleTimeCommand::Clear()
		{
			// Destroy fences:
			vkDestroyFence(Context::GetVkDevice(), s_graphicsFence, nullptr);
			vkDestroyFence(Context::GetVkDevice(), s_presentFence, nullptr);
			vkDestroyFence(Context::GetVkDevice(), s_computeFence, nullptr);
			vkDestroyFence(Context::GetVkDevice(), s_transferFence, nullptr);

			// Destroy buffers:
			vkFreeCommandBuffers(Context::GetVkDevice(), s_graphicsPool, 1, &s_graphicsBuffer);
			vkFreeCommandBuffers(Context::GetVkDevice(), s_presentPool, 1, &s_presentBuffer);
			vkFreeCommandBuffers(Context::GetVkDevice(), s_computePool, 1, &s_computeBuffer);
			vkFreeCommandBuffers(Context::GetVkDevice(), s_transferPool, 1, &s_transferBuffer);

			// Destroyy pools:
			vkDestroyCommandPool(Context::GetVkDevice(), s_graphicsPool, nullptr);
			vkDestroyCommandPool(Context::GetVkDevice(), s_presentPool, nullptr);
			vkDestroyCommandPool(Context::GetVkDevice(), s_computePool, nullptr);
			vkDestroyCommandPool(Context::GetVkDevice(), s_transferPool, nullptr);
		}



		// Public methods:
		VkCommandBuffer& SingleTimeCommand::BeginCommand(const DeviceQueue& queue)
		{
			// Select command buffer:
			VkCommandBuffer* commandBuffer = nullptr;
			VkCommandPool* commandPool = nullptr;
			if (queue.queue == s_graphicsDeviceQueue.queue)
			{
				commandBuffer = &s_graphicsBuffer;
				commandPool = &s_graphicsPool;
			}
			else if (queue.queue == s_presentDeviceQueue.queue)
			{
				commandBuffer = &s_presentBuffer;
				commandPool = &s_presentPool;
			}
			else if (queue.queue == s_computeDeviceQueue.queue)
			{
				commandBuffer = &s_computeBuffer;
				commandPool = &s_computePool;
			}
			else if (queue.queue == s_transferDeviceQueue.queue)
			{
				commandBuffer = &s_transferBuffer;
				commandPool = &s_transferPool;
			}
			else throw std::out_of_range("Invalid device queue.");

			// Begin command buffer:
			VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkResetCommandPool(Context::GetVkDevice(), *commandPool, 0);
			VKA(vkBeginCommandBuffer(*commandBuffer, &beginInfo));

			NAME_VK_COMMAND_BUFFER(*commandBuffer, "SingleTimeCommandBuffer");
			NAME_VK_COMMAND_POOL(*commandPool, "SingleTimeCommandPool");
			return *commandBuffer;
		}
		void SingleTimeCommand::EndCommand(const DeviceQueue& queue)
		{
			// Select command buffer and fence:
			VkCommandBuffer* commandBuffer = nullptr;
			VkFence* fence = nullptr;
			if (queue.queue == s_graphicsDeviceQueue.queue)
			{
				commandBuffer = &s_graphicsBuffer;
				fence = &s_graphicsFence;
			}
			else if (queue.queue == s_presentDeviceQueue.queue)
			{
				commandBuffer = &s_presentBuffer;
				fence = &s_presentFence;
			}
			else if (queue.queue == s_computeDeviceQueue.queue)
			{
				commandBuffer = &s_computeBuffer;
				fence = &s_computeFence;
			}
			else if (queue.queue == s_transferDeviceQueue.queue)
			{
				commandBuffer = &s_transferBuffer;
				fence = &s_transferFence;
			}
			else throw std::out_of_range("Invalid device queue.");

			// End command buffer:
			VKA(vkEndCommandBuffer(*commandBuffer));

			// Submit command buffer:
			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = commandBuffer;
			VKA(vkQueueSubmit(queue.queue, 1, &submitInfo, *fence));

			// Wait for the fence to signal completion
			VKA(vkWaitForFences(Context::GetVkDevice(), 1, fence, VK_TRUE, UINT64_MAX));
			VKA(vkResetFences(Context::GetVkDevice(), 1, fence));
		}
	}
}