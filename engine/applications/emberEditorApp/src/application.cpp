#include "application.h"
#include "commonRendererCreateInfo.h"
#include "component.h"
#include "compute.h"
#include "core.h"
#include "editor.h"
#include "emberMath.h"
#include "emberTime.h"
#include "eventSystem.h"
#include "gui.h"
#include "logger.h"
#include "profiler.h"
#include "renderer.h"
#include "scene.h"
#include "shadowConstants.h"
#include "window.h"
// Backends:
#include "sdlWindow.h"
#include "vulkanRenderer.h"
#include "vulkanAsyncCompute.h"
#include "vulkanCompute.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "imGuiSdlVulkan.h"
// Editor Windows:
#include "backendDebugWindow.h"
#include "consoleEditorWindow.h"
#include "depthBiasEditorWindow.h"
#include "fpsEditorWindow.h"
#include "gameEditorWindow.h"
#include "hierarchyEditorWindow.h"
#include "inspectorEditorWindow.h"
#include "projectEditorWindow.h"
#include "sceneEditorWindow.h"



namespace emberApplication
{
    using namespace emberCore;


    
	// Static members:
	emberEcs::Scene* Application::m_pActiveScene;
	std::unique_ptr<emberEditor::BackendDebugEditorWindow> Application::m_pBackendDebugEditorWindow;
	std::unique_ptr<emberEditor::ConsoleEditorWindow> Application::m_pConsoleEditorWindow;
	std::unique_ptr<emberEditor::DepthBiasEditorWindow> Application::m_pDepthBiasEditorWindow;
	std::unique_ptr<emberEditor::FpsEditorWindow> Application::m_pFpsEditorWindow;
	std::unique_ptr<emberEditor::GameEditorWindow> Application::m_pGameEditorWindow;
	std::unique_ptr<emberEditor::HierarchyEditorWindow> Application::m_pHierarchyEditorWindow;
	std::unique_ptr<emberEditor::InspectorEditorWindow> Application::m_pInspectorEditorWindow;
	std::unique_ptr<emberEditor::ProjectEditorWindow> Application::m_pProjectEditorWindow;
	std::unique_ptr<emberEditor::SceneEditorWindow> Application::m_pSceneEditorWindow;



	// Public methods:
	// Constructor/Destructor:
	bool Application::Init(const CreateInfo& applicationCreateInfo)
	{
		try
		{
			m_pActiveScene = nullptr;

			// Init basic systems:
			Core::InitBasics();

			// Window backend:
			bool forceX11VideoDriver = true;
			emberBackendInterface::IWindow* pIWindow = new sdlWindowBackend::Window(applicationCreateInfo.windowWidth, applicationCreateInfo.windowHeight, forceX11VideoDriver);
            LOG_INFO("enabled x11 video driver for detached window support.");
            
			// Renderer backend:
			emberCommon::RendererCreateInfo rendererCreateInfo = {};
			rendererCreateInfo.vSyncEnabled = applicationCreateInfo.vSyncEnabled;		            // project settings.
			rendererCreateInfo.framesInFlight = applicationCreateInfo.framesInFlight;	            // project settings.
			rendererCreateInfo.msaaSampleCount = applicationCreateInfo.msaaSampleCount;	            // project settings.
			rendererCreateInfo.renderWidth = applicationCreateInfo.renderWidth;			            // project settings.
			rendererCreateInfo.renderHeight = applicationCreateInfo.renderHeight;		            // project settings.
			rendererCreateInfo.enableGui = true;										            // application dependent.
			rendererCreateInfo.enableDockSpace = true;									            // application dependent.
			rendererCreateInfo.maxDirectionalLights = applicationCreateInfo.maxDirectionalLights;   // clamped to 1-MAX_DIR_LIGHTS in renderer.
			rendererCreateInfo.maxPositionalLights = applicationCreateInfo.maxPositionalLights;     // clamped to 1-MAX_POS_LIGHTS in renderer.
			rendererCreateInfo.shadowMapResolution = applicationCreateInfo.shadowMapResolution;     // clamped to 1-SHADOW_MAP_RESOLUTION in renderer.
			emberBackendInterface::IRenderer* pIRenderer = new vulkanRendererBackend::Renderer(rendererCreateInfo, pIWindow);

			// Compute backend:
			emberBackendInterface::ICompute* pICompute = new vulkanRendererBackend::Compute();

			// Gui backend:
			emberBackendInterface::IGui* pIGui = new imGuiSdlVulkanBackend::Gui(pIWindow, pIRenderer, rendererCreateInfo.enableDockSpace);

			// Init backends:
			Core::InitBackends(pIWindow, pIRenderer, pICompute, pIGui);

			// Gpu Resource Managers:
			Core::InitManagers();
			Core::InitOther();

			// Editor windows:
			m_pBackendDebugEditorWindow = std::make_unique<emberEditor::BackendDebugEditorWindow>();
			m_pConsoleEditorWindow = std::make_unique<emberEditor::ConsoleEditorWindow>();
			m_pDepthBiasEditorWindow = std::make_unique<emberEditor::DepthBiasEditorWindow>();
			m_pFpsEditorWindow = std::make_unique<emberEditor::FpsEditorWindow>();
			m_pGameEditorWindow = std::make_unique<emberEditor::GameEditorWindow>();
			m_pHierarchyEditorWindow = std::make_unique<emberEditor::HierarchyEditorWindow>();
			m_pInspectorEditorWindow = std::make_unique<emberEditor::InspectorEditorWindow>();
			m_pProjectEditorWindow = std::make_unique<emberEditor::ProjectEditorWindow>();
			m_pSceneEditorWindow = std::make_unique<emberEditor::SceneEditorWindow>();

			// Other systems:
			EventSystem::Init();
			return true;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
			return false;
		}
	}
	void Application::Clear()
	{
		Renderer::WaitDeviceIdle();
		Core::ClearOther();
		Core::ClearManagers();
		Core::ClearBackends();
		Core::ClearBasics();
	}



	// Main loop:
	void Application::Run()
	{
		try
		{
			bool running = true;
			Time::Reset();
			m_pActiveScene->Start();

			while (running)
			{
				PROFILE_FUNCTION();
				Time::Update();

				Renderer::CollectGarbage();
				running = EventSystem::ProcessEvents();

				// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
				Int2 windowSize = Window::GetSize();
				Uint2 surfaceExtent = Renderer::GetSurfaceExtent();
				if (Window::GetIsMinimized() || windowSize.x == 0 || windowSize.y == 0 || surfaceExtent.x == 0 || surfaceExtent.y == 0)
					continue;

				// Frame update loop:
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::scene);
					m_pActiveScene->EarlyUpdate();

					// Skip delayed physics updates:
					// Runs at most one fixed update per render frame. This avoids stacking physics work when a frame is late, but drops simulation steps under load.
					if (Time::UpdatePhysics(true))
					{
						Compute::Physics::BeginRecording();
						m_pActiveScene->FixedUpdate();
						Compute::Physics::EndRecording();
					}
					// Batch delayed physics updates:
					// Runs every pending fixed update. This preserves fixed-step catch-up, but can create large frames.
					//if (Time::ShouldUpdatePhysics())
					//{
					//	Compute::Physics::BeginRecording();
					//	while (Time::UpdatePhysics())
					//		m_pActiveScene->FixedUpdate();
					//	Compute::Physics::EndRecording();
					//}
				}

				// Game update loop:
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::gui);
					Gui::Update();  // contains hidden nested EventConsumerScope consumerScope(EventSystem::Consumer::editor);
				}
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::scene);
					m_pActiveScene->Update();
					m_pActiveScene->LateUpdate();
				}
				Editor::PreRender();
				Renderer::RenderFrame();
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
		Renderer::WaitDeviceIdle();
	}



	// Setters:
	void Application::SetScene(emberEcs::Scene* pScene)
	{
		m_pActiveScene = pScene;
		m_pHierarchyEditorWindow->SetScene(pScene);
	}



	// Getters:
	emberEcs::Scene* Application::GetActiveScene()
	{
		return m_pActiveScene;
	}
	emberEditor::BackendDebugEditorWindow* Application::GetBackendDebugEditorWindow()
    {
        return m_pBackendDebugEditorWindow.get();
    }
	emberEditor::ConsoleEditorWindow* Application::GetConsoleEditorWindow()
	{
		return m_pConsoleEditorWindow.get();
	}
	emberEditor::DepthBiasEditorWindow* Application::GetDepthBiasEditorWindow()
	{
		return m_pDepthBiasEditorWindow.get();
	}
	emberEditor::FpsEditorWindow* Application::GetFpsEditorWindow()
	{
		return m_pFpsEditorWindow.get();
	}
	emberEditor::GameEditorWindow* Application::GetGameEditorWindow()
	{
		return m_pGameEditorWindow.get();
	}
	emberEditor::HierarchyEditorWindow* Application::GetHierarchyEditorWindow()
	{
		return m_pHierarchyEditorWindow.get();
	}
	emberEditor::InspectorEditorWindow* Application::GetInspectorEditorWindow()
	{
		return m_pInspectorEditorWindow.get();
	}
	emberEditor::ProjectEditorWindow* Application::GetProjectEditorWindow()
	{
		return m_pProjectEditorWindow.get();
	}
	emberEditor::SceneEditorWindow* Application::GetSceneEditorWindow()
	{
		return m_pSceneEditorWindow.get();
	}
}