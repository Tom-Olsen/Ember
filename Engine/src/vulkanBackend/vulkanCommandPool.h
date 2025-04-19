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
			VkCommandPool m_pool;
			std::vector<VkCommandBuffer> m_buffers;

		public: // Methods:
			CommandPool(int bufferCount, DeviceQueue queue);
			~CommandPool();

			void ResetPool() const;
			void ResetBuffer(int index) const;
			VkCommandBuffer& GetVkCommandBuffer(int index);
			int GetBufferCount() const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanCommandPool_h__