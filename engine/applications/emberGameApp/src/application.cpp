#include "application.h"
#include "commonRendererCreateInfo.h"
#include "compute.h"
#include "core.h"
#include "emberMath.h"
#include "emberTime.h"
#include "eventSystem.h"
#include "gui.h"
#include "logger.h"
#include "profiler.h"
#include "renderer.h"
#include "scene.h"
#include "window.h"
// Backends:
#include "sdlWindow.h"
#include "vulkanBackend.h"
#include "imGuiSdlVulkan.h"
// System:
#include <exception>



namespace emberApplication
{
    using namespace emberCore;



	// Static members:
	emberEcs::Scene* Application::m_pActiveScene;



	// Public methods:
	// Constructor/Destructor:
	bool Application::Init(const CreateInfo& applicationCreateInfo)
	{
		try
		{
			m_pActiveScene = nullptr;

			// Window backend:
			#if defined(__linux__)
				constexpr bool forceX11VideoDriver = true;
			#else
				constexpr bool forceX11VideoDriver = false;
			#endif
			if (forceX11VideoDriver)
				LOG_INFO("enabled x11 video driver for detached window support.");
			emberBackendInterface::IWindow* pIWindow = new sdlWindowBackend::Window(applicationCreateInfo.windowWidth, applicationCreateInfo.windowHeight);

			// Gpu backend:
			emberCommon::RendererCreateInfo rendererCreateInfo = {};
			rendererCreateInfo.vSyncEnabled = applicationCreateInfo.vSyncEnabled;		            // project settings.
			rendererCreateInfo.framesInFlight = applicationCreateInfo.framesInFlight;	            // project settings.
			rendererCreateInfo.msaaSampleCount = applicationCreateInfo.msaaSampleCount;	            // project settings.
			rendererCreateInfo.renderWidth = applicationCreateInfo.renderWidth;			            // project settings.
			rendererCreateInfo.renderHeight = applicationCreateInfo.renderHeight;		            // project settings.
			rendererCreateInfo.enableGui = false;										            // application dependent.
			rendererCreateInfo.enableDockSpace = false;									            // application dependent.
			rendererCreateInfo.maxDirectionalLights = applicationCreateInfo.maxDirectionalLights;   // clamped to 1-MAX_DIR_LIGHTS in renderer.
			rendererCreateInfo.maxPositionalLights = applicationCreateInfo.maxPositionalLights;     // clamped to 1-MAX_POS_LIGHTS in renderer.
			rendererCreateInfo.shadowMapResolution = applicationCreateInfo.shadowMapResolution;     // clamped to 1-SHADOW_MAP_RESOLUTION in renderer.
			emberBackendInterface::IGpuBackend* pIGpuBackend = new vulkanRendererBackend::VulkanBackend(rendererCreateInfo, pIWindow);

			// Gui backend:
			emberBackendInterface::IGui* pIGui = new imGuiSdlVulkanBackend::Gui(pIWindow, pIGpuBackend->GetRenderer(), rendererCreateInfo.enableDockSpace);

			// Init core:
			Core::Init(pIWindow, pIGpuBackend, pIGui);
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
		Core::Clear();
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
					EventConsumerScope consumerScope(EventSystem::Consumer::game);
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
				}

				// Game update loop:
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::gui);
					Gui::Update();  // contains hidden nested EventConsumerScope consumerScope(EventSystem::Consumer::editor);
				}
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::game);
					m_pActiveScene->Update();
					m_pActiveScene->LateUpdate();
				}
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