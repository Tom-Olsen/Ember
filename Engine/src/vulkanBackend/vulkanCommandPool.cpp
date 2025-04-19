#include "vulkanCommandPool.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <stdexcept>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		CommandPool::CommandPool(int bufferCount, DeviceQueue queue)
		{
			VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
			createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	// required for vkResetCommandBuffer(...)
			createInfo.queueFamilyIndex = queue.familyIndex;
			VKA(vkCreateCommandPool(Context::GetVkDevice(), &createInfo, nullptr, &m_pool));

			m_buffers.resize(bufferCount);
			VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			allocateInfo.commandPool = m_pool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = bufferCount;
			VKA(vkAllocateCommandBuffers(Context::GetVkDevice(), &allocateInfo, m_buffers.data()));
		}
		CommandPool::~CommandPool()
		{
			vkFreeCommandBuffers(Context::GetVkDevice(), m_pool, (uint32_t)m_buffers.size(), m_buffers.data());
			vkDestroyCommandPool(Context::GetVkDevice(), m_pool, nullptr);
		}



		// Public methods:
		void CommandPool::ResetPool() const
		{
			vkResetCommandPool(Context::GetVkDevice(), m_pool, 0);
		}
		void CommandPool::ResetBuffer(int index) const
		{
			if (index < 0 || index > m_buffers.size() - 1)
				throw std::out_of_range("Reset CommandBuffer index out of range.");
			vkResetCommandBuffer(m_buffers[index], 0);
		}
		VkCommandBuffer& CommandPool::GetVkCommandBuffer(int index)
		{
			if (index < 0 || index > m_buffers.size() - 1)
				throw std::out_of_range("CommandBuffer index out of range.");
			return m_buffers[index];
		}
		int CommandPool::GetBufferCount() const
		{
			return static_cast<int>(m_buffers.size());
		}
	}
}