#pragma once
#include <string>
#include <vector>



// Forward declarations:
typedef struct VkCommandPool_T* VkCommandPool;
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct DeviceQueue;



	class CommandPool
	{
	private: // Members:
		VkCommandPool m_primaryPool;
		std::vector<VkCommandPool> m_secondaryPools;
		VkCommandBuffer m_primaryBuffer;
		std::vector<VkCommandBuffer> m_secondaryBuffers;

	public: // Methods:
		CommandPool(int secondaryBufferCount, DeviceQueue queue);
		~CommandPool();

		// Non-copyable:
		CommandPool(const CommandPool&) = delete;
		CommandPool& operator=(const CommandPool&) = delete;

		// Movable:
		CommandPool(CommandPool&& other) noexcept;
		CommandPool& operator=(CommandPool&& other) noexcept;

		void ResetPools() const;
		VkCommandPool& GetPrimaryVkCommandPool();
		VkCommandPool& GetSecondaryVkCommandPool(int index);
		VkCommandBuffer& GetPrimaryVkCommandBuffer();
		VkCommandBuffer& GetSecondaryVkCommandBuffer(int index);
		std::vector<VkCommandBuffer>& GetSecondaryVkCommandBuffers();
		int GetSecondaryBufferCount() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(CommandPool& other) noexcept;
	};
}