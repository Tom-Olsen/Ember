#ifndef __INCLUDE_GUARD_vulkanCommands_h__
#define __INCLUDE_GUARD_vulkanCommands_h__
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	// Forward declarations:
	struct VulkanContext;
	struct VulkanQueue;



	class VulkanCommand
	{
	private: // Members:
		VkCommandPool m_pool;
		VkCommandBuffer m_buffer;
		VulkanContext* m_pContext;

	public: // Methods:
		VulkanCommand(VulkanContext* pContext, VulkanQueue queue);
		~VulkanCommand();

		// Getters:
		const VkCommandPool& GetVkCommandPool() const;
		const VkCommandBuffer& GetVkCommandBuffer() const;

		// Static methods:
		static VulkanCommand BeginSingleTimeCommand(VulkanContext* pContext, const VulkanQueue& queue);
		static void EndSingleTimeCommand(VulkanContext* pContext, const VulkanCommand& command, const VulkanQueue& queue, bool waitQueueIdle = true);
	};
}



#endif // __INCLUDE_GUARD_vulkanCommands_h__