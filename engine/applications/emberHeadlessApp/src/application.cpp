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
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "nullGui.h"



namespace emberApplication
{
    using namespace emberCore;


    
	// Static members:
	emberEcs::Scene* Application::m_pActiveScene;



	// Public methods:
	// Constructor/Destructor:
	void Application::Init(const CreateInfo& applicationCreateInfo)
	{
		try
		{
			m_pActiveScene = nullptr;

			// Init basic systems:
			Core::InitBasics();

			// Window backend:
			emberBackendInterface::IWindow* pIWindow = new nullWindowBackend::Window();

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
			emberBackendInterface::IGui* pIGui = new nullGuiBackend::Gui();

			// Init backends:
			Core::InitBackends(pIWindow, pIRenderer, pICompute, pIGui);

			// Gpu Resource Managers:
			Core::InitManagers();
			Core::InitOther();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
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
				m_pActiveScene->EarlyUpdate();

				// Skip delayed physics updates:
				// Runs at most one fixed update per render frame. This avoids stacking physics work when a frame is late, but drops simulation steps under load.
				//if (Time::UpdatePhysics(true))
				//{
				//	Compute::Physics::BeginRecording();
				//	m_pActiveScene->FixedUpdate();
				//	Compute::Physics::EndRecording();
				//}
				// Batch delayed physics updates:
				// Runs every pending fixed update. This preserves fixed-step catch-up, but can create large frames.
				if (Time::ShouldUpdatePhysics())
				{
					Compute::Physics::BeginRecording();
					while (Time::UpdatePhysics())
						m_pActiveScene->FixedUpdate();
					Compute::Physics::EndRecording();
				}

				// Game update loop:
				Gui::Update();
				m_pActiveScene->Update();
				m_pActiveScene->LateUpdate();
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
	}



	// Getters:
	emberEcs::Scene* Application::GetActiveScene()
	{
		return m_pActiveScene;
	}
}