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
#include "nullWindow.h"
#include "vulkanRenderer.h"
#include "vulkanAsyncCompute.h"
#include "vulkanCompute.h"
#include "vulkanImmediateCompute.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "nullGui.h"



namespace emberApplication
{
	// Static members:
	emberEngine::Scene* Application::m_pActiveScene;



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
			emberBackendInterface::IWindow* pIWindow = new nullWindowBackend::Window();

			// Renderer backend:
			emberCommon::RendererCreateInfo rendererCreateInfo = {};
			rendererCreateInfo.vSyncEnabled = applicationCreateInfo.vSyncEnabled;		// project settings.
			rendererCreateInfo.framesInFlight = applicationCreateInfo.framesInFlight;	// project settings.
			rendererCreateInfo.msaaSampleCount = applicationCreateInfo.msaaSampleCount;	// project settings.
			rendererCreateInfo.renderWidth = applicationCreateInfo.renderWidth;			// project settings.
			rendererCreateInfo.renderHeight = applicationCreateInfo.renderHeight;		// project settings.
			rendererCreateInfo.enableGui = true;										// application dependent.
			rendererCreateInfo.enableDockSpace = true;									// application dependent.
			rendererCreateInfo.maxDirectionalLights = MAX_DIR_LIGHTS;					// controlled via macro for now.
			rendererCreateInfo.maxPositionalLights = MAX_POS_LIGHTS;					// controlled via macro for now.
			rendererCreateInfo.shadowMapResolution = SHADOW_MAP_RESOLUTION;				// controlled via macro for now.
			emberBackendInterface::IRenderer* pIRenderer = new vulkanRendererBackend::Renderer(rendererCreateInfo, pIWindow);

			// Compute backend:
			emberBackendInterface::ICompute* pICompute = new vulkanRendererBackend::Compute();

			// Gui backend:
			emberBackendInterface::IGui* pIGui = new nullGuiBackend::Gui();

			// Init backends:
			emberEngine::Core::InitBackends(pIWindow, pIRenderer, pICompute, pIGui);

			// Gpu Resource Managers:
			emberEngine::Core::InitManagers();
			emberEngine::Core::InitOther();
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
}