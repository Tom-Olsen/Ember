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
	class ConsoleEditorWindow;
	class DepthBiasEditorWindow;
	class FpsEditorWindow;
	class GameEditorWindow;
	class HierarchyEditorWindow;
	class InspectorEditorWindow;
	class OutlineEditorWindow;
	class ProjectEditorWindow;
	class SceneEditorWindow;
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
		static std::unique_ptr<emberEditor::ConsoleEditorWindow> m_pConsoleEditorWindow;
		static std::unique_ptr<emberEditor::DepthBiasEditorWindow> m_pDepthBiasEditorWindow;
		static std::unique_ptr<emberEditor::FpsEditorWindow> m_pFpsEditorWindow;
		static std::unique_ptr<emberEditor::GameEditorWindow> m_pGameEditorWindow;
		static std::unique_ptr<emberEditor::HierarchyEditorWindow> m_pHierarchyEditorWindow;
		static std::unique_ptr<emberEditor::InspectorEditorWindow> m_pInspectorEditorWindow;
		static std::unique_ptr<emberEditor::OutlineEditorWindow> m_pOutlineEditorWindow;
		static std::unique_ptr<emberEditor::ProjectEditorWindow> m_pProjectEditorWindow;
		static std::unique_ptr<emberEditor::SceneEditorWindow> m_pSceneEditorWindow;

	public: // Methods:
		static bool Init(const CreateInfo& createInfo);
		static void Clear();

		// Main loop:
		static void Run();

		// Setters:
		static void SetScene(emberEcs::Scene* pScene);

		// Getters:
		static emberEcs::Scene* GetActiveScene();
		static emberEditor::BackendDebugEditorWindow* GetBackendDebugEditorWindow();
		static emberEditor::ConsoleEditorWindow* GetConsoleEditorWindow();
		static emberEditor::DepthBiasEditorWindow* GetDepthBiasEditorWindow();
		static emberEditor::FpsEditorWindow* GetFpsEditorWindow();
		static emberEditor::GameEditorWindow* GetGameEditorWindow();
		static emberEditor::HierarchyEditorWindow* GetHierarchyEditorWindow();
		static emberEditor::InspectorEditorWindow* GetInspectorEditorWindow();
		static emberEditor::OutlineEditorWindow* GetOutlineEditorWindow();
		static emberEditor::ProjectEditorWindow* GetProjectEditorWindow();
		static emberEditor::SceneEditorWindow* GetSceneEditorWindow();

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