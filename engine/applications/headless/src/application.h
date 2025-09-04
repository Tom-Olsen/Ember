#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include <vulkan/vulkan.h>
#include <memory>



namespace emberEngine
{
	// Forward declarations:
	class RenderCore;
	class SdlWindow;
	class Scene;



	class Application
	{
	public:
		struct Settings
		{
			bool vSyncEnabled = false;
			uint32_t framesInFlight = 2;
			VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_4_BIT;
			uint32_t windowWidth = 2560;
			uint32_t windowHeight = 1440;
			uint32_t renderWidth = 1920;
			uint32_t renderHeight = 1080;
		};

	private: // Members:
		Scene* m_pActiveScene;
		std::unique_ptr<SdlWindow> m_pWindow;
		std::unique_ptr<RenderCore> m_pRenderer;

	public: // Methods:
		Application(const Settings& settings);
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