#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include "commonMsaaSampleCount.h"
#include <memory>



namespace emberEngine
{
	// Forward declarations:
	class Renderer;
	class Scene;



	class Application
	{
	public:
		struct CreateInfo
		{
			bool vSyncEnabled = false;
			uint32_t framesInFlight = 2;
			MsaaSampleCount msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount04;
			int windowWidth = 2560;
			int windowHeight = 1440;
			uint32_t renderWidth = 1920;
			uint32_t renderHeight = 1080;
		};

	private: // Members:
		Scene* m_pActiveScene;
		std::unique_ptr<Renderer> m_pRenderer;

	public: // Methods:
		Application(const CreateInfo& createInfo);
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