#include "vulkanCommandPool.h"
#include "vulkanContext.h"
#include "vulkanDeviceQueue.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include <assert.h>
#include <stdexcept>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	CommandPool::CommandPool(int secondaryBufferCount, const DeviceQueue& queue)
	{
		static uint32_t nameIndex = 0;
		// Assertions:
		assert(queue.queue != VK_NULL_HANDLE);

		try
		{
			// Create primary command pool:
			VkCommandPoolCreateInfo primaryCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
			primaryCreateInfo.queueFamilyIndex = queue.familyIndex;
			VKA(vkCreateCommandPool(Context::GetVkDevice(), &primaryCreateInfo, nullptr, &m_primaryPool));
			NAME_VK_OBJECT(m_primaryPool, "CommandPool" + std::to_string(nameIndex) + "_Primary");

			// Allocate primary command buffer:
			VkCommandBufferAllocateInfo primaryAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			primaryAllocateInfo.commandPool = m_primaryPool;
			primaryAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			primaryAllocateInfo.commandBufferCount = 1;
			VKA(vkAllocateCommandBuffers(Context::GetVkDevice(), &primaryAllocateInfo, &m_primaryBuffer));
			NAME_VK_OBJECT(m_primaryBuffer, "CommandBuffer" + std::to_string(nameIndex) + "_Primary");

			// Secondaries:
			m_secondaryPools.resize(secondaryBufferCount);
			m_secondaryBuffers.resize(secondaryBufferCount);
			for (int i = 0; i < secondaryBufferCount; i++)
			{
				// Create secondary command pools:
				VkCommandPoolCreateInfo secondaryCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
				secondaryCreateInfo.queueFamilyIndex = queue.familyIndex;
				VKA(vkCreateCommandPool(Context::GetVkDevice(), &secondaryCreateInfo, nullptr, &m_secondaryPools[i]));
				NAME_VK_OBJECT(m_secondaryPools[i], "CommandPool" + std::to_string(nameIndex) + "_Secondary" + std::to_string(i));

				// Allocate secondary command buffers:
				VkCommandBufferAllocateInfo secondaryAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
				secondaryAllocateInfo.commandPool = m_secondaryPools[i];
				secondaryAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
				secondaryAllocateInfo.commandBufferCount = 1;
				VKA(vkAllocateCommandBuffers(Context::GetVkDevice(), &secondaryAllocateInfo, &m_secondaryBuffers[i]));
				NAME_VK_OBJECT(m_secondaryBuffers[i], "CommandBuffer" + std::to_string(nameIndex) + "_Secondary" + std::to_string(i));
			}
		}
		catch (...)
		{
			Cleanup();
			throw;
		}
		nameIndex++;
	}
	CommandPool::~CommandPool()
	{
		Cleanup();
	}



	// Move semantics:
	CommandPool::CommandPool(CommandPool&& other) noexcept
	{
		MoveFrom(other);
	}
	CommandPool& CommandPool::operator=(CommandPool&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Reset:
	void CommandPool::ResetPools() const
	{
		vkResetCommandPool(Context::GetVkDevice(), m_primaryPool, 0);
		for (int i = 0; i < m_secondaryBuffers.size(); i++)
			vkResetCommandPool(Context::GetVkDevice(), m_secondaryPools[i], 0);
	}



	// Getters:
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



	// Private methods:
	void CommandPool::Cleanup()
	{
		// Destroying a pool also frees all of its command buffers.
		if (m_primaryPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(Context::GetVkDevice(), m_primaryPool, nullptr);
		for (VkCommandPool pool : m_secondaryPools)
		{
			if (pool != VK_NULL_HANDLE)
				vkDestroyCommandPool(Context::GetVkDevice(), pool, nullptr);
		}
		m_primaryPool = VK_NULL_HANDLE;
		m_primaryBuffer = VK_NULL_HANDLE;
		m_secondaryPools.clear();
		m_secondaryBuffers.clear();
	}
	void CommandPool::MoveFrom(CommandPool& other) noexcept
	{
		m_primaryPool = other.m_primaryPool;
		m_secondaryPools = std::move(other.m_secondaryPools);
		m_primaryBuffer = other.m_primaryBuffer;
		m_secondaryBuffers = std::move(other.m_secondaryBuffers);

		other.m_primaryPool = VK_NULL_HANDLE;
		other.m_secondaryPools.clear();
		other.m_primaryBuffer = VK_NULL_HANDLE;
		other.m_secondaryBuffers.clear();
	}
}