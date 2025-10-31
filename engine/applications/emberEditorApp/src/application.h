#pragma once
#include "commonMsaaSampleCount.h"
#include <memory>



// Forward declarations:
namespace emberEngine
{
	class Renderer;
	class Scene;
}
namespace emberEditor
{
	class ConsoleEditorWindow;
	class DepthBiasEditorWindow;
	class FpsEditorWindow;
	class GameEditorWindow;
	class HierarchyEditorWindow;
	class InspectorEditorWindow;
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
		};

	private: // Members:
		static emberEngine::Scene* m_pActiveScene;
		static std::unique_ptr<emberEditor::ConsoleEditorWindow> m_pConsoleEditorWindow;
		static std::unique_ptr<emberEditor::DepthBiasEditorWindow> m_pDepthBiasEditorWindow;
		static std::unique_ptr<emberEditor::FpsEditorWindow> m_pFpsEditorWindow;
		static std::unique_ptr<emberEditor::GameEditorWindow> m_pGameEditorWindow;
		static std::unique_ptr<emberEditor::HierarchyEditorWindow> m_pHierarchyEditorWindow;
		static std::unique_ptr<emberEditor::InspectorEditorWindow> m_pInspectorEditorWindow;
		static std::unique_ptr<emberEditor::ProjectEditorWindow> m_pProjectEditorWindow;
		static std::unique_ptr<emberEditor::SceneEditorWindow> m_pSceneEditorWindow;

	public: // Methods:
		static void Init(const CreateInfo& createInfo);
		static void Clear();

		// Main loop:
		static void Run();

		// Setters:
		static void SetScene(emberEngine::Scene* pScene);

		// Getters:
		static emberEngine::Scene* GetActiveScene();
		static emberEditor::ConsoleEditorWindow* GetConsoleEditorWindow();
		static emberEditor::DepthBiasEditorWindow* GetDepthBiasEditorWindow();
		static emberEditor::FpsEditorWindow* GetFpsEditorWindow();
		static emberEditor::GameEditorWindow* GetGameEditorWindow();
		static emberEditor::HierarchyEditorWindow* GetHierarchyEditorWindow();
		static emberEditor::InspectorEditorWindow* GetInspectorEditorWindow();
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