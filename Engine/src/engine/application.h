#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include <vulkan/vulkan.h>
#include <memory>



namespace emberEngine
{
	// Forward declarations:
	class Scene;
	class VulkanRenderer;



	class Application
	{
	public:
		struct Settings
		{
			bool vSyncEnabled;
			uint32_t framesInFlight;
			VkSampleCountFlagBits msaaSamples;
			uint32_t windowWidth;
			uint32_t windowHeight;
			uint32_t renderWidth;
			uint32_t renderheight;
		};
	private: // Members:
		std::unique_ptr<VulkanRenderer> m_pRenderer;
		Scene* m_pActiveScene;

	public: // Methods:
		Application(Settings settings);
		~Application();
		void Run();
		void SetScene(Scene* pScene);

	private: // Methods:
		void Start();
		void Update();
		void LateUpdate();
		void FixedUpdate();
	};
}



#endif // __INCLUDE_GUARD_application_h__