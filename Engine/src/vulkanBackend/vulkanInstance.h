#ifndef __INCLUDE_GUARD_vulkanInstance_h__
#define __INCLUDE_GUARD_vulkanInstance_h__
#include <vulkan/vulkan.h>
#include <vector>



namespace emberEngine
{
	namespace vulkanBackend
	{
		class Instance
		{
		private: // Members:
			VkInstance m_instance;
			VkDebugUtilsMessengerEXT m_debugMessenger;

		public: // Methods:
			Instance(std::vector<const char*> instanceExtensions);
			~Instance();

			const VkInstance& GetVkInstance() const;
			std::vector<const char*> AvailableInstanceExtensions() const;
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanInstance_h__