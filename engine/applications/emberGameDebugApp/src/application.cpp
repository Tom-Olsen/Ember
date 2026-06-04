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
#include "depthBiasEditorWindow.h"
#include "fpsEditorWindow.h"



namespace emberApplication
{
	// Static members:
	emberEngine::Scene* Application::m_pActiveScene;
	std::unique_ptr<emberEditor::BackendDebugEditorWindow> Application::m_pBackendDebugEditorWindow;
	std::unique_ptr<emberEditor::DepthBiasEditorWindow> Application::m_pDepthBiasEditorWindow;
	std::unique_ptr<emberEditor::FpsEditorWindow> Application::m_pFpsEditorWindow;



	// Public methods:
	// Constructor/Destructor:
	void Application::Init(const CreateInfo& applicationCreateInfo)
	{
		try
		{
			m_pActiveScene = nullptr;

			// Init basic systems:
			emberEngine::Core::InitBasics();

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
			rendererCreateInfo.enableDockSpace = false;									            // application dependent.
			rendererCreateInfo.maxDirectionalLights = applicationCreateInfo.maxDirectionalLights;   // clamped to 1-MAX_DIR_LIGHTS in renderer.
			rendererCreateInfo.maxPositionalLights = applicationCreateInfo.maxPositionalLights;     // clamped to 1-MAX_POS_LIGHTS in renderer.
			rendererCreateInfo.shadowMapResolution = applicationCreateInfo.shadowMapResolution;     // clamped to 1-SHADOW_MAP_RESOLUTION in renderer.
			emberBackendInterface::IRenderer* pIRenderer = new vulkanRendererBackend::Renderer(rendererCreateInfo, pIWindow);

			// Compute backend:
			emberBackendInterface::ICompute* pICompute = new vulkanRendererBackend::Compute();

			// Gui backend:
			emberBackendInterface::IGui* pIGui = new imGuiSdlVulkanBackend::Gui(pIWindow, pIRenderer, rendererCreateInfo.enableDockSpace);

			// Init backends:
			emberEngine::Core::InitBackends(pIWindow, pIRenderer, pICompute, pIGui);

			// Gpu Resource Managers:
			emberEngine::Core::InitManagers();
			emberEngine::Core::InitOther();

			// Editor windows:
			m_pBackendDebugEditorWindow = std::make_unique<emberEditor::BackendDebugEditorWindow>();
			m_pDepthBiasEditorWindow = std::make_unique<emberEditor::DepthBiasEditorWindow>();
			m_pFpsEditorWindow = std::make_unique<emberEditor::FpsEditorWindow>();

			// Other systems:
			emberEngine::EventSystem::Init();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
	}
	void Application::Clear()
	{
		emberEngine::Renderer::WaitDeviceIdle();
		emberEngine::Core::ClearOther();
		emberEngine::Core::ClearManagers();
		emberEngine::Core::ClearBackends();
		emberEngine::Core::ClearBasics();
	}



	// Main loop:
	void Application::Run()
	{
		try
		{
			bool running = true;
			emberEngine::Time::Reset();
			m_pActiveScene->Start();
			
			while (running)
			{
				PROFILE_FUNCTION();
				emberEngine::Time::Update();

				emberEngine::Renderer::CollectGarbage();
				running = emberEngine::EventSystem::ProcessEvents();

				// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
				Int2 windowSize = emberEngine::Window::GetSize();
				Uint2 surfaceExtent = emberEngine::Renderer::GetSurfaceExtent();
				if (emberEngine::Window::GetIsMinimized() || windowSize.x == 0 || windowSize.y == 0 || surfaceExtent.x == 0 || surfaceExtent.y == 0)
					continue;

				// Frame update loop:
				m_pActiveScene->EarlyUpdate();

				// Physics Update Loop:
				while (emberEngine::Time::UpdatePhysics())
					m_pActiveScene->FixedUpdate();

				// Game update loop:
				emberEngine::Gui::Update();
				m_pActiveScene->Update();
				m_pActiveScene->LateUpdate();
				emberEngine::Renderer::RenderFrame();
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
		emberEngine::Renderer::WaitDeviceIdle();
	}



	// Setters:
	void Application::SetScene(emberEngine::Scene* pScene)
	{
		m_pActiveScene = pScene;
	}



	// Getters:
	emberEngine::Scene* Application::GetActiveScene()
	{
		return m_pActiveScene;
	}
	emberEditor::BackendDebugEditorWindow* Application::GetBackendDebugEditorWindow()
    {
        return m_pBackendDebugEditorWindow.get();
    }
	emberEditor::DepthBiasEditorWindow* Application::GetDepthBiasEditorWindow()
	{
		return m_pDepthBiasEditorWindow.get();
	}
	emberEditor::FpsEditorWindow* Application::GetFpsEditorWindow()
	{
		return m_pFpsEditorWindow.get();
	}
}