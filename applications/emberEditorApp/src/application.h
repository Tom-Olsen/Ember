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
		emberEngine::Scene* m_pActiveScene;
		std::unique_ptr<emberEditor::ConsoleEditorWindow> m_pConsoleEditorWindow;
		std::unique_ptr<emberEditor::DepthBiasEditorWindow> m_pDepthBiasEditorWindow;
		std::unique_ptr<emberEditor::FpsEditorWindow> m_pFpsEditorWindow;
		std::unique_ptr<emberEditor::GameEditorWindow> m_pGameEditorWindow;
		std::unique_ptr<emberEditor::HierarchyEditorWindow> m_pHierarchyEditorWindow;
		std::unique_ptr<emberEditor::InspectorEditorWindow> m_pInspectorEditorWindow;
		std::unique_ptr<emberEditor::ProjectEditorWindow> m_pProjectEditorWindow;
		std::unique_ptr<emberEditor::SceneEditorWindow> m_pSceneEditorWindow;

	public: // Methods:
		// Constructor/Destructor:
		Application(const CreateInfo& createInfo);
		~Application();

		// Non-copyable:
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		// Movable:
		Application(Application&& other) noexcept;
		Application& operator=(Application&& other) noexcept;

		// Main loop:
		void Run();

		// Setters:
		void SetScene(emberEngine::Scene* pScene);

		// Getters:
		emberEditor::ConsoleEditorWindow* GetConsoleEditorWindow() const;
		emberEditor::DepthBiasEditorWindow* GetDepthBiasEditorWindow() const;
		emberEditor::FpsEditorWindow* GetFpsEditorWindow() const;
		emberEditor::GameEditorWindow* GetGameEditorWindow() const;
		emberEditor::HierarchyEditorWindow* GetHierarchyEditorWindow() const;
		emberEditor::InspectorEditorWindow* GetInspectorEditorWindow() const;
		emberEditor::ProjectEditorWindow* GetProjectEditorWindow() const;
		emberEditor::SceneEditorWindow* GetSceneEditorWindow() const;

	private: // Methods:
		void Start();
		void Update();
		void LateUpdate();
		void FixedUpdate();
	};
}