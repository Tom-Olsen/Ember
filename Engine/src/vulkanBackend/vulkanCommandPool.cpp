#include "vulkanCommandPool.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <stdexcept>
#include <assert.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		CommandPool::CommandPool(int secondaryBufferCount, DeviceQueue queue)
		{
			// Assertions:
			assert(queue.queue != VK_NULL_HANDLE);

			// Create primary command pool:
			VkCommandPoolCreateInfo primaryCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
			primaryCreateInfo.queueFamilyIndex = queue.familyIndex;
			VKA(vkCreateCommandPool(Context::GetVkDevice(), &primaryCreateInfo, nullptr, &m_primaryPool));

			// Allocate primary command buffer:
			VkCommandBufferAllocateInfo primaryAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			primaryAllocateInfo.commandPool = m_primaryPool;
			primaryAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			primaryAllocateInfo.commandBufferCount = 1;
			VKA(vkAllocateCommandBuffers(Context::GetVkDevice(), &primaryAllocateInfo, &m_primaryBuffer));

			// Secondaries:
			m_secondaryPools.resize(secondaryBufferCount);
			m_secondaryBuffers.resize(secondaryBufferCount);
			for (int i = 0; i < secondaryBufferCount; i++)
			{
				// Create secondary command pools:
				VkCommandPoolCreateInfo secondaryCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
				secondaryCreateInfo.queueFamilyIndex = queue.familyIndex;
				VKA(vkCreateCommandPool(Context::GetVkDevice(), &secondaryCreateInfo, nullptr, &m_secondaryPools[i]));

				// Allocate secondary command buffers:
				VkCommandBufferAllocateInfo secondaryAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
				secondaryAllocateInfo.commandPool = m_secondaryPools[i];
				secondaryAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
				secondaryAllocateInfo.commandBufferCount = 1;
				VKA(vkAllocateCommandBuffers(Context::GetVkDevice(), &secondaryAllocateInfo, &m_secondaryBuffers[i]));
			}
		}
		CommandPool::~CommandPool()
		{
			vkFreeCommandBuffers(Context::GetVkDevice(), m_primaryPool, 1, &m_primaryBuffer);
			vkDestroyCommandPool(Context::GetVkDevice(), m_primaryPool, nullptr);
			for (int i = 0; i < m_secondaryBuffers.size(); i++)
			{
				vkFreeCommandBuffers(Context::GetVkDevice(), m_secondaryPools[i], 1, &m_secondaryBuffers[i]);
				vkDestroyCommandPool(Context::GetVkDevice(), m_secondaryPools[i], nullptr);
			}
		}



		// Public methods:
		void CommandPool::ResetPools() const
		{
			vkResetCommandPool(Context::GetVkDevice(), m_primaryPool, 0);
			for (int i = 0; i < m_secondaryBuffers.size(); i++)
				vkResetCommandPool(Context::GetVkDevice(), m_secondaryPools[i], 0);
		}
		VkCommandPool& CommandPool::GetPrimaryVkCommandPool()
		{
			return m_primaryPool;
		}
		VkCommandPool& CommandPool::GetSecondaryVkCommandPool(int index)
		{
			return m_secondaryPools[index];
		}
		VkCommandBuffer& CommandPool::GetPrimaryVkCommandBuffer()
		{
			return m_primaryBuffer;
		}
		VkCommandBuffer& CommandPool::GetSecondaryVkCommandBuffer(int index)
		{
			if (index < 0 || index >= m_secondaryBuffers.size())
				throw std::out_of_range("Secondary command buffer index out of range.");
			return m_secondaryBuffers[index];
		}
		std::vector<VkCommandBuffer>& CommandPool::GetSecondaryVkCommandBuffers()
		{
			return m_secondaryBuffers;
		}
		int CommandPool::GetSecondaryBufferCount() const
		{
			return static_cast<int>(m_secondaryBuffers.size());
		}
	}
}