#pragma once
#include "commonMsaaSampleCount.h"
#include "shadowConstants.h"
#include <memory>



// Forward declarations:
namespace emberEngine
{
	class Renderer;
}
namespace emberEcs
{
	class Scene;
}
namespace emberEditor
{
	class BackendDebugEditorWindow;
	class DepthBiasEditorWindow;
	class FpsEditorWindow;
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
			uint32_t maxDirectionalLights = MAX_DIR_LIGHTS;
			uint32_t maxPositionalLights = MAX_POS_LIGHTS;
			uint32_t shadowMapResolution = SHADOW_MAP_RESOLUTION;
		};

	private: // Members:
		static emberEcs::Scene* m_pActiveScene;
		static std::unique_ptr<emberEditor::BackendDebugEditorWindow> m_pBackendDebugEditorWindow;
		static std::unique_ptr<emberEditor::DepthBiasEditorWindow> m_pDepthBiasEditorWindow;
		static std::unique_ptr<emberEditor::FpsEditorWindow> m_pFpsEditorWindow;

	public: // Methods:
		static void Init(const CreateInfo& createInfo);
		static void Clear();

		// Main loop:
		static void Run();

		// Setters:
		static void SetScene(emberEcs::Scene* pScene);

		// Getters:
		static emberEcs::Scene* GetActiveScene();
		static emberEditor::BackendDebugEditorWindow* GetBackendDebugEditorWindow();
		static emberEditor::DepthBiasEditorWindow* GetDepthBiasEditorWindow();
		static emberEditor::FpsEditorWindow* GetFpsEditorWindow();

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