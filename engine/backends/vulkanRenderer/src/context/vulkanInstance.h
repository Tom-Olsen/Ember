#pragma once
#include <vector>



// Forward declarations:
typedef struct VkInstance_T* VkInstance;
typedef struct VkDebugUtilsMessengerEXT_T* VkDebugUtilsMessengerEXT;



namespace vulkanRendererBackend
{
	class Instance
	{
	private: // Members:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;

	public: // Methods:
		Instance(std::vector<const char*> instanceExtensions);
		~Instance();

		// Non-copyable:
		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;

		// Movable:
		Instance(Instance&& other) noexcept;
		Instance& operator=(Instance&& other) noexcept;

		const VkInstance& GetVkInstance() const;
		std::vector<const char*> GetAllAvailableInstanceExtensionsNames() const;

	private: // Methods:
		void Cleanup();
		void MoveFrom(Instance& other) noexcept;
	};
}