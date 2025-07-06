#ifndef __INCLUDE_GUARD_vulkanCommandPool_h__
#define __INCLUDE_GUARD_vulkanCommandPool_h__
#include <vulkan/vulkan.h>
#include <vector>
#include <string>



namespace emberEngine
{
	namespace vulkanBackend
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

			void ResetPools() const;
			VkCommandPool& GetPrimaryVkCommandPool();
			VkCommandPool& GetSecondaryVkCommandPool(int index);
			VkCommandBuffer& GetPrimaryVkCommandBuffer();
			VkCommandBuffer& GetSecondaryVkCommandBuffer(int index);
			std::vector<VkCommandBuffer>& GetSecondaryVkCommandBuffers();
			int GetSecondaryBufferCount() const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanCommandPool_h__