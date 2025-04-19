#ifndef __INCLUDE_GUARD_vulkanDescriptorPool_h__
#define __INCLUDE_GUARD_vulkanDescriptorPool_h__
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Forward declarations:
		class LogicalDevice;



		class DescriptorPool
		{
		private: // Members:
			VkDescriptorPool m_descriptorPool;
			LogicalDevice* m_pLogicalDevice;

		public: // Methods:
			DescriptorPool(LogicalDevice* pLogicalDevice);
			~DescriptorPool();
			const VkDescriptorPool& GetVkDescriptorPool() const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanDescriptorPool_h__