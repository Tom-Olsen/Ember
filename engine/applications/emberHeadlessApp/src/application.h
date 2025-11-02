#pragma once
#include "commonMsaaSampleCount.h"
#include <memory>



// Forward declarations:
namespace emberEngine
{
	class Renderer;
	class Scene;
}



namespace emberApplication
{
	class Application
	{
	public:
		struct CreateInfo
		{
			bool vSyncEnabled = false;
			uint32_t framesInFlight = 2;
			emberCommon::MsaaSampleCount msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount04;
			int windowWidth = 2560;
			int windowHeight = 1440;
			uint32_t renderWidth = 1920;
			uint32_t renderHeight = 1080;
		};

	private: // Members:
		static emberEngine::Scene* m_pActiveScene;

	public: // Methods:
		static void Init(const CreateInfo& createInfo);
		static void Clear();

		// Main loop:
		static void Run();

		// Setters:
		static void SetScene(emberEngine::Scene* pScene);

		// Getters:
		static emberEngine::Scene* GetActiveScene();

	private: // Methods
		// Delete all constructors:
		Application() = delete;
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;
		~Application() = delete;
	};
}