#include "vulkanSingleTimeCommand.h"
#include "vulkanContext.h"
#include "vulkanDeviceQueue.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include <assert.h>
#include <stdexcept>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
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
	VkSemaphore SingleTimeCommand::s_semaphore;



	// Initialization/Cleanup:
	void SingleTimeCommand::Init()
	{
		s_graphicsDeviceQueue = Context::GetLogicalDevice()->GetGraphicsQueue();
		s_presentDeviceQueue = Context::GetLogicalDevice()->GetPresentQueue();
		s_computeDeviceQueue = Context::GetLogicalDevice()->GetComputeQueue();
		s_transferDeviceQueue = Context::GetLogicalDevice()->GetTransferQueue();
		VkCommandPool* pPools[4] = { &s_graphicsPool , &s_presentPool , &s_computePool , &s_transferPool };
		VkCommandBuffer* pBuffers[4] = { &s_graphicsBuffer , &s_presentBuffer , &s_computeBuffer , &s_transferBuffer };
		DeviceQueue* pDeviceQueues[4] = { &s_graphicsDeviceQueue, &s_presentDeviceQueue, &s_computeDeviceQueue, &s_transferDeviceQueue };
		VkFence* pFences[4] = { &s_graphicsFence, &s_presentFence, &s_computeFence, &s_transferFence };

		for (int i = 0; i < 4; i++)
		{
			// Create pools:
			VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
			createInfo.queueFamilyIndex = pDeviceQueues[i]->familyIndex;
			VKA(vkCreateCommandPool(Context::GetVkDevice(), &createInfo, nullptr, pPools[i]));

			// Create buffers:
			VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			allocateInfo.commandPool = *pPools[i];
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = 1;
			VKA(vkAllocateCommandBuffers(Context::GetVkDevice(), &allocateInfo, pBuffers[i]));
			
			// Create fences:
			VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			VKA(vkCreateFence(Context::GetVkDevice(), &fenceInfo, nullptr, pFences[i]));
		}
		// Create semaphore:
		{
			VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &createInfo, nullptr, &s_semaphore));
		}

		NAME_VK_COMMAND_BUFFER(s_graphicsBuffer, "SingleTimeGraphicsCommandBuffer");
		NAME_VK_COMMAND_BUFFER(s_presentBuffer, "SingleTimePresentCommandBuffer");
		NAME_VK_COMMAND_BUFFER(s_computeBuffer, "SingleTimeComputeCommandBuffer");
		NAME_VK_COMMAND_BUFFER(s_transferBuffer, "SingleTimeTransferCommandBuffer");
		NAME_VK_COMMAND_POOL(s_graphicsPool, "SingleTimeGraphicsCommandPool");
		NAME_VK_COMMAND_POOL(s_presentPool, "SingleTimePresentCommandPool");
		NAME_VK_COMMAND_POOL(s_computePool, "SingleTimeComputeCommandPool");
		NAME_VK_COMMAND_POOL(s_transferPool, "SingleTimeTransferCommandPool");
		NAME_VK_FENCE(s_graphicsPool, "SingleTimeGraphicsFence");
		NAME_VK_FENCE(s_presentPool, "SingleTimePresentFence");
		NAME_VK_FENCE(s_computePool, "SingleTimeComputeFence");
		NAME_VK_FENCE(s_transferPool, "SingleTimeTransferFence");
		NAME_VK_SEMAPHORE(s_semaphore, "SingleTimeCommandSemaphore");
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

		// Destroy semaphore:
		vkDestroySemaphore(Context::GetVkDevice(), s_semaphore, nullptr);
	}



	// Public methods:
	VkCommandBuffer& SingleTimeCommand::BeginCommand(const DeviceQueue& queue)
	{
		// Select command buffer:
		VkCommandBuffer* pCommandBuffer = FindCommandBuffer(queue);
		VkCommandPool* pCommandPool = FindCommandPool(queue);

		// Begin command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkResetCommandPool(Context::GetVkDevice(), *pCommandPool, 0);
		VKA(vkBeginCommandBuffer(*pCommandBuffer, &beginInfo));

		return *pCommandBuffer;
	}
	void SingleTimeCommand::EndCommand(const DeviceQueue& queue)
	{
		// Select command buffer and fence:
		VkCommandBuffer* pCommandBuffer = FindCommandBuffer(queue);
		VkFence* pFence = FindFence(queue);

		// End command buffer:
		VKA(vkEndCommandBuffer(*pCommandBuffer));

		// Submit command buffer:
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = pCommandBuffer;
		VKA(vkQueueSubmit(queue.queue, 1, &submitInfo, *pFence));

		// Wait for the fence:
		VKA(vkWaitForFences(Context::GetVkDevice(), 1, pFence, VK_TRUE, UINT64_MAX));
		VKA(vkResetFences(Context::GetVkDevice(), 1, pFence));
	}
	void SingleTimeCommand::EndLinkedCommands(const DeviceQueue& firstQueue, const DeviceQueue& secondQueue, PipelineStage waitDstStageMask)
	{
		assert(firstQueue.queue != secondQueue.queue);

		// Select command buffers:
		VkCommandBuffer* pFirstCommandBuffer = FindCommandBuffer(firstQueue);
		VkCommandBuffer* pSecondCommandBuffer = FindCommandBuffer(secondQueue);
		VkFence* pFence = FindFence(secondQueue);

		// End command buffers:
		VKA(vkEndCommandBuffer(*pFirstCommandBuffer));
		VKA(vkEndCommandBuffer(*pSecondCommandBuffer));

		// Submit command buffers:
		{
			VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
			commandBufferInfo.commandBuffer = *pFirstCommandBuffer;

			VkSemaphoreSubmitInfo signalSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
			signalSemaphoreInfo.semaphore = s_semaphore;
			signalSemaphoreInfo.stageMask = PipelineStages::allCommands;

			VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
			submitInfo.commandBufferInfoCount = 1;
			submitInfo.pCommandBufferInfos = &commandBufferInfo;
			submitInfo.signalSemaphoreInfoCount = 1;
			submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;

			VKA(vkQueueSubmit2(firstQueue.queue, 1, &submitInfo, nullptr));
		}
		{
			VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
			commandBufferInfo.commandBuffer = *pSecondCommandBuffer;

			VkSemaphoreSubmitInfo waitSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
			waitSemaphoreInfo.semaphore = s_semaphore;
			waitSemaphoreInfo.stageMask = waitDstStageMask;

			VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
			submitInfo.commandBufferInfoCount = 1;
			submitInfo.pCommandBufferInfos = &commandBufferInfo;
			submitInfo.waitSemaphoreInfoCount = 1;
			submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo;

			VKA(vkQueueSubmit2(secondQueue.queue, 1, &submitInfo, *pFence));
		}

		// Wait for the fence:
		VKA(vkWaitForFences(Context::GetVkDevice(), 1, pFence, VK_TRUE, UINT64_MAX));
		VKA(vkResetFences(Context::GetVkDevice(), 1, pFence));
	}



	// Private methods:
	VkCommandBuffer* SingleTimeCommand::FindCommandBuffer(const DeviceQueue& queue)
	{
		if (queue.queue == s_graphicsDeviceQueue.queue)
			return &s_graphicsBuffer;
		else if (queue.queue == s_presentDeviceQueue.queue)
			return &s_presentBuffer;
		else if (queue.queue == s_computeDeviceQueue.queue)
			return &s_computeBuffer;
		else if (queue.queue == s_transferDeviceQueue.queue)
			return &s_transferBuffer;
		else throw std::out_of_range("Invalid device queue.");
		return nullptr;
	}
	VkCommandPool* SingleTimeCommand::FindCommandPool(const DeviceQueue& queue)
	{
		if (queue.queue == s_graphicsDeviceQueue.queue)
			return &s_graphicsPool;
		else if (queue.queue == s_presentDeviceQueue.queue)
			return &s_presentPool;
		else if (queue.queue == s_computeDeviceQueue.queue)
			return &s_computePool;
		else if (queue.queue == s_transferDeviceQueue.queue)
			return &s_transferPool;
		else throw std::out_of_range("Invalid device queue.");
		return nullptr;
	}
	VkFence* SingleTimeCommand::FindFence(const DeviceQueue& queue)
	{
		if (queue.queue == s_graphicsDeviceQueue.queue)
			return &s_graphicsFence;
		else if (queue.queue == s_presentDeviceQueue.queue)
			return &s_presentFence;
		else if (queue.queue == s_computeDeviceQueue.queue)
			return &s_computeFence;
		else if (queue.queue == s_transferDeviceQueue.queue)
			return &s_transferFence;
		else throw std::out_of_range("Invalid device queue.");
		return nullptr;
	}
}