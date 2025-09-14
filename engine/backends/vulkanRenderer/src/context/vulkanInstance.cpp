#include "vulkanInstance.h"
#include "logger.h"
#include "vulkanMacros.h"



namespace vulkanRendererBackend
{
	// Debug callback:
	VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			LOG_TRACE("Validation layer VERBOSE: {}", pCallbackData->pMessage);
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			LOG_INFO ("Validation layer    INFO: {}", pCallbackData->pMessage);
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			LOG_WARN ("Validation layer WARNING: {}", pCallbackData->pMessage);
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			LOG_ERROR("Validation layer   ERROR: {}", pCallbackData->pMessage);
		return VK_FALSE;
	}



	// Constructor/Destructor:
	Instance::Instance()
	{
		m_instance = VK_NULL_HANDLE;
		m_debugMessenger = VK_NULL_HANDLE;
	}
	Instance::~Instance()
	{
		Cleanup();
	}



	// Move semantics:
	Instance::Instance(Instance&& other) noexcept
	{
		MoveFrom(other);
	}
	Instance& Instance::operator=(Instance&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Public methods:
	void Instance::Init(std::vector<const char*> instanceExtensions)
	{
		VkApplicationInfo applicationInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
		applicationInfo.pApplicationName = "Application Name";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "Ember";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInfo.pApplicationInfo = &applicationInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();

		// Add instance create enumerate portability bit flag if dear imgui docking is active:
		if (std::find(instanceExtensions.begin(), instanceExtensions.end(), VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) != instanceExtensions.end())
			createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

		#if defined(VALIDATION_LAYERS_ACTIVE)
		// Enable validation features:
		std::vector<VkValidationFeatureEnableEXT> enabledValidationFeatures;
		enabledValidationFeatures.push_back(VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
		enabledValidationFeatures.push_back(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT);
		enabledValidationFeatures.push_back(VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);
		VkValidationFeaturesEXT validationFeatures{ VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
		validationFeatures.enabledValidationFeatureCount = static_cast<uint32_t>(enabledValidationFeatures.size());
		validationFeatures.pEnabledValidationFeatures = enabledValidationFeatures.data();
		createInfo.pNext = &validationFeatures;

		// Enable validation layers:
		std::vector<const char*> enabledLayers;
		enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
		createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
		createInfo.ppEnabledLayerNames = enabledLayers.data();
		#endif

		// Create the Vulkan instance:
		VKA(vkCreateInstance(&createInfo, nullptr, &m_instance));

		// Debug Messenger:
		#if defined(VALIDATION_LAYERS_ACTIVE)
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;

		PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
		VKA(pfnCreateDebugUtilsMessengerEXT(m_instance, &debugCreateInfo, nullptr, &m_debugMessenger));
		#endif
	}
	const VkInstance& Instance::GetVkInstance() const
	{
		return m_instance;
	}
	std::vector<const char*> Instance::GetAllAvailableInstanceExtensionsNames() const
	{
		// Get all supported instance extensions:
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// Convert to vector of const char*:
		std::vector<const char*> extensionNames(extensionCount);
		for (const VkExtensionProperties& extension : extensions)
			extensionNames.push_back(extension.extensionName);

		return extensionNames;
	}



	// Private methods:
	void Instance::Cleanup()
	{
		#if defined(VALIDATION_LAYERS_ACTIVE)
		PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
		pfnDestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
		#endif
		if (m_instance != VK_NULL_HANDLE)
			vkDestroyInstance(m_instance, nullptr);
	}
	void Instance::MoveFrom(Instance& other) noexcept
	{
		m_instance = other.m_instance;
		m_debugMessenger = other.m_debugMessenger;

		other.m_instance = VK_NULL_HANDLE;
		other.m_debugMessenger = VK_NULL_HANDLE;
	}
}