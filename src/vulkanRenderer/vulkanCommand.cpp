#include "vulkanCommand.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Constructor/Destructor:
	VulkanCommand::VulkanCommand(VulkanContext* pContext, VulkanQueue queue)
	{
		m_pContext = pContext;

		VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		//createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	// required for vkResetCommandBuffer(m_buffer,0) (not recommended)
		createInfo.queueFamilyIndex = queue.familyIndex;	// bind to correct queue family.
		VKA(vkCreateCommandPool(m_pContext->GetVkDevice(), &createInfo, nullptr, &m_pool));

		VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = m_pool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// Can be submitted to a queue for execution, but cannot be called from other command buffers.
		//allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;	// Cannot be submitted directly, but can be called from primary command buffers.
		allocateInfo.commandBufferCount = 1;
		VKA(vkAllocateCommandBuffers(m_pContext->GetVkDevice(), &allocateInfo, &m_buffer));
	}
	VulkanCommand::~VulkanCommand()
	{
		vkDestroyCommandPool(m_pContext->GetVkDevice(), m_pool, nullptr);
	}



	// Public methods:
	// Getters:
	const VkCommandPool& VulkanCommand::GetVkCommandPool() const
	{
		return m_pool;
	}
	const VkCommandBuffer& VulkanCommand::GetVkCommandBuffer() const
	{
		return m_buffer;
	}



	// Static methods:
	VulkanCommand VulkanCommand::BeginSingleTimeCommand(VulkanContext* m_pContext, const VulkanQueue& queue)
	{
		VulkanCommand command(m_pContext, queue);
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(command.GetVkCommandBuffer(), &beginInfo));
		return command;
	}
	void VulkanCommand::EndSingleTimeCommand(VulkanContext* m_pContext, const VulkanCommand& command, const VulkanQueue& queue, bool waitQueueIdle)
	{
		vkEndCommandBuffer(command.GetVkCommandBuffer());
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command.GetVkCommandBuffer();
		VKA(vkQueueSubmit(queue.queue, 1, &submitInfo, VK_NULL_HANDLE));
		if (waitQueueIdle)
			VKA(vkQueueWaitIdle(queue.queue));
	}
}