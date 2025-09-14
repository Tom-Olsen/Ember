#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanInstance_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanInstance_h__
#include <vulkan/vulkan.h>
#include <vector>



namespace vulkanRendererBackend
{
	class Instance
	{
	private: // Members:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;

	public: // Methods:
		Instance();
		~Instance();

		// Non-copyable:
		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;

		// Movable:
		Instance(Instance&& other) noexcept;
		Instance& operator=(Instance&& other) noexcept;

		void Init(std::vector<const char*> instanceExtensions);
		const VkInstance& GetVkInstance() const;
		std::vector<const char*> GetAllAvailableInstanceExtensionsNames() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(Instance& other) noexcept;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanInstance_h__