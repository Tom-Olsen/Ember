#include "application.h"
#include "commonRendererCreateInfo.h"
#include "compute.h"
#include "core.h"
#include "editor.h"
#include "emberMath.h"
#include "emberTime.h"
#include "eventSystem.h"
#include "gui.h"
#include "handleContext.h"
#include "logger.h"
#include "profiler.h"
#include "renderer.h"
#include "window.h"
#include "world.h"
// Backends:
#include "sdlWindow.h"
#include "vulkanBackend.h"
#include "imGuiSdlVulkan.h"
// Editor Windows:
#include "backendDebugWindow.h"
#include "depthBiasEditorWindow.h"
#include "fpsEditorWindow.h"
// System:
#include <exception>
#include <memory>



namespace emberApplication
{
	using namespace emberCore;



	// Static members:
	std::unique_ptr<emberEditor::BackendDebugEditorWindow> Application::s_pBackendDebugEditorWindow;
	std::unique_ptr<emberEditor::DepthBiasEditorWindow> Application::s_pDepthBiasEditorWindow;
	std::unique_ptr<emberEditor::FpsEditorWindow> Application::s_pFpsEditorWindow;



	// Public Methods:
	// Constructor/Destructor:
	void Application::Init(const CreateInfo& applicationCreateInfo)
	{
		try
		{
			// Window backend:
			#if defined(__linux__)
				constexpr bool forceX11VideoDriver = true;
			#else
				constexpr bool forceX11VideoDriver = false;
			#endif
			if (forceX11VideoDriver)
				LOG_INFO("enabled x11 video driver for detached window support.");
			emberBackendInterface::IWindow* pIWindow = new sdlWindowBackend::Window(applicationCreateInfo.windowWidth, applicationCreateInfo.windowHeight, forceX11VideoDriver);

			// Gpu backend:
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
			emberBackendInterface::IGpuBackend* pIGpuBackend = new vulkanRendererBackend::VulkanBackend(rendererCreateInfo, pIWindow);

			// Gui backend:
			emberBackendInterface::IGui* pIGui = new imGuiSdlVulkanBackend::Gui(pIWindow, pIGpuBackend->GetRenderer(), rendererCreateInfo.enableDockSpace);

			// Init core:
			Core::Init(pIWindow, pIGpuBackend, pIGui);
			emberEcs::World::Init();

			// Init application-specific systems:
			emberEditor::HandleContext::Init();

			// Editor windows:
			s_pBackendDebugEditorWindow = std::make_unique<emberEditor::BackendDebugEditorWindow>();
			s_pDepthBiasEditorWindow = std::make_unique<emberEditor::DepthBiasEditorWindow>();
			s_pFpsEditorWindow = std::make_unique<emberEditor::FpsEditorWindow>();

		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
	}
	void Application::Clear()
	{
		s_pFpsEditorWindow.reset();
		s_pDepthBiasEditorWindow.reset();
		s_pBackendDebugEditorWindow.reset();

		emberEditor::HandleContext::Clear();
		emberEcs::World::Clear();
		Core::Clear();
	}



	// Main loop:
	void Application::Run()
	{
		try
		{
			bool running = true;
			Time::Reset();
			emberEcs::World::Start();

			while (running)
			{
				PROFILE_FUNCTION();
				Time::Update();

				Renderer::CollectGarbage();
				running = EventSystem::ProcessEvents();

				// Frame update loop:
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::game);
					emberEcs::World::EarlyUpdate();

					// Skip delayed physics updates:
					// Runs at most one fixed update per render frame. This avoids stacking physics work when a frame is late, but drops simulation steps under load.
					//if (Time::UpdatePhysics(true))
					//{
					//	Compute::Physics::BeginSession();
					//	emberEcs::World::FixedUpdate();
					//	Compute::Physics::EndSession();
					//}
					// Batch delayed physics updates:
					// Runs every pending fixed update. This preserves fixed-step catch-up, but can create large frames.
					if (Time::ShouldUpdatePhysics())
					{
						Compute::Physics::BeginSession();
						while (Time::UpdatePhysics())
							emberEcs::World::FixedUpdate();
						Compute::Physics::EndSession();
					}
				}

				// Game update loop:
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::gui);
					Gui::Update();  // contains hidden nested EventConsumerScope consumerScope(EventSystem::Consumer::editor);
				}
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::game);
					emberEcs::World::Update();
					emberEcs::World::LateUpdate();
				}
				UpdateMainViewportHandleContext();
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



	// Getters:
	emberEditor::BackendDebugEditorWindow* Application::GetBackendDebugEditorWindow()
	{
		return s_pBackendDebugEditorWindow.get();
	}
	emberEditor::DepthBiasEditorWindow* Application::GetDepthBiasEditorWindow()
	{
		return s_pDepthBiasEditorWindow.get();
	}
	emberEditor::FpsEditorWindow* Application::GetFpsEditorWindow()
	{
		return s_pFpsEditorWindow.get();
	}



	// Private Methods:
	void Application::UpdateMainViewportHandleContext()
	{
		Uint2 surfaceExtent = Renderer::GetSurfaceExtent();
		Float2 viewportSize = Float2((float)surfaceExtent.x, (float)surfaceExtent.y);
		Float2 viewportMousePos = EventSystem::MousePos();
		Float2 viewportMousePos01 = EventSystem::MousePos01();
		bool mouseInsideViewport =
			viewportMousePos01.x >= 0.0f && viewportMousePos01.x <= 1.0f &&
			viewportMousePos01.y >= 0.0f && viewportMousePos01.y <= 1.0f;
		bool viewportIsHovered = mouseInsideViewport && !Editor::GetHoveredWindowWantCaptureEvents();

		emberEditor::HandleContext::SetCamera(emberEcs::World::GetActiveCamera());
		emberEditor::HandleContext::SetViewportSize(viewportSize);
		emberEditor::HandleContext::SetViewportMousePos(viewportMousePos);
		emberEditor::HandleContext::SetViewportMousePos01(viewportMousePos01);
		emberEditor::HandleContext::SetViewPortIsHovered(viewportIsHovered);
	}
}