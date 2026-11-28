#pragma once
#include "commonMsaaSampleCount.h"
#include "shadowConstants.h"
#include <memory>



// Forward declarations:
namespace emberEngine
{
	class Renderer;
}
namespace emberEditor
{
	class BackendDebugEditorWindow;
	class ConsoleEditorWindow;
	class DeferredLightingEditorWindow;
	class DepthBiasEditorWindow;
	class FpsEditorWindow;
	class GameEditorWindow;
	class HierarchyEditorWindow;
	class InspectorEditorWindow;
	class OutlineEditorWindow;
	class ProjectEditorWindow;
	class ViewportEditorWindow;
	class SsaoDispatcherEditorWindow;
	class SsrDispatcherEditorWindow;
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
		static std::unique_ptr<emberEditor::BackendDebugEditorWindow> s_pBackendDebugEditorWindow;
		static std::unique_ptr<emberEditor::ConsoleEditorWindow> s_pConsoleEditorWindow;
		static std::unique_ptr<emberEditor::DeferredLightingEditorWindow> s_pDeferredLightingEditorWindow;
		static std::unique_ptr<emberEditor::DepthBiasEditorWindow> s_pDepthBiasEditorWindow;
		static std::unique_ptr<emberEditor::FpsEditorWindow> s_pFpsEditorWindow;
		static std::unique_ptr<emberEditor::GameEditorWindow> s_pGameEditorWindow;
		static std::unique_ptr<emberEditor::HierarchyEditorWindow> s_pHierarchyEditorWindow;
		static std::unique_ptr<emberEditor::InspectorEditorWindow> s_pInspectorEditorWindow;
		static std::unique_ptr<emberEditor::OutlineEditorWindow> s_pOutlineEditorWindow;
		static std::unique_ptr<emberEditor::ProjectEditorWindow> s_pProjectEditorWindow;
		static std::unique_ptr<emberEditor::ViewportEditorWindow> s_pViewportEditorWindow;
		static std::unique_ptr<emberEditor::SsaoDispatcherEditorWindow> s_pSsaoDispatcherEditorWindow;
		static std::unique_ptr<emberEditor::SsrDispatcherEditorWindow> s_pSsrDispatcherEditorWindow;

	public: // Methods:
		static bool Init(const CreateInfo& createInfo);
		static void Clear();

		// Main loop:
		static void Run();

		// Getters:
		static emberEditor::BackendDebugEditorWindow* GetBackendDebugEditorWindow();
		static emberEditor::ConsoleEditorWindow* GetConsoleEditorWindow();
		static emberEditor::DeferredLightingEditorWindow* GetDeferredLightingEditorWindow();
		static emberEditor::DepthBiasEditorWindow* GetDepthBiasEditorWindow();
		static emberEditor::FpsEditorWindow* GetFpsEditorWindow();
		static emberEditor::GameEditorWindow* GetGameEditorWindow();
		static emberEditor::HierarchyEditorWindow* GetHierarchyEditorWindow();
		static emberEditor::InspectorEditorWindow* GetInspectorEditorWindow();
		static emberEditor::OutlineEditorWindow* GetOutlineEditorWindow();
		static emberEditor::ProjectEditorWindow* GetProjectEditorWindow();
		static emberEditor::ViewportEditorWindow* GetViewportEditorWindow();
		static emberEditor::SsaoDispatcherEditorWindow* GetSsaoDispatcherEditorWindow();
		static emberEditor::SsrDispatcherEditorWindow* GetSsrDispatcherEditorWindow();

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