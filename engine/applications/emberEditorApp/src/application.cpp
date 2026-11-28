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
#include "consoleEditorWindow.h"
#include "deferredLightingEditorWindow.h"
#include "depthBiasEditorWindow.h"
#include "fpsEditorWindow.h"
#include "gameEditorWindow.h"
#include "hierarchyEditorWindow.h"
#include "inspectorEditorWindow.h"
#include "outlineEditorWindow.h"
#include "projectEditorWindow.h"
#include "ssaoDispatcherEditorWindow.h"
#include "ssrDispatcherEditorWindow.h"
#include "viewportEditorWindow.h"
// System:
#include <exception>
#include <memory>



namespace emberApplication
{
	using namespace emberCore;


	
	// Static members:
	std::unique_ptr<emberEditor::BackendDebugEditorWindow> Application::s_pBackendDebugEditorWindow;
	std::unique_ptr<emberEditor::ConsoleEditorWindow> Application::s_pConsoleEditorWindow;
	std::unique_ptr<emberEditor::DeferredLightingEditorWindow> Application::s_pDeferredLightingEditorWindow;
	std::unique_ptr<emberEditor::DepthBiasEditorWindow> Application::s_pDepthBiasEditorWindow;
	std::unique_ptr<emberEditor::FpsEditorWindow> Application::s_pFpsEditorWindow;
	std::unique_ptr<emberEditor::GameEditorWindow> Application::s_pGameEditorWindow;
	std::unique_ptr<emberEditor::HierarchyEditorWindow> Application::s_pHierarchyEditorWindow;
	std::unique_ptr<emberEditor::InspectorEditorWindow> Application::s_pInspectorEditorWindow;
	std::unique_ptr<emberEditor::OutlineEditorWindow> Application::s_pOutlineEditorWindow;
	std::unique_ptr<emberEditor::ProjectEditorWindow> Application::s_pProjectEditorWindow;
	std::unique_ptr<emberEditor::ViewportEditorWindow> Application::s_pViewportEditorWindow;
	std::unique_ptr<emberEditor::SsaoDispatcherEditorWindow> Application::s_pSsaoDispatcherEditorWindow;
	std::unique_ptr<emberEditor::SsrDispatcherEditorWindow> Application::s_pSsrDispatcherEditorWindow;



	// Public Methods:
	// Constructor/Destructor:
	bool Application::Init(const CreateInfo& applicationCreateInfo)
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
			rendererCreateInfo.enableDockSpace = true;									            // application dependent.
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
			s_pConsoleEditorWindow = std::make_unique<emberEditor::ConsoleEditorWindow>();
			s_pDeferredLightingEditorWindow = std::make_unique<emberEditor::DeferredLightingEditorWindow>();
			s_pDepthBiasEditorWindow = std::make_unique<emberEditor::DepthBiasEditorWindow>();
			s_pFpsEditorWindow = std::make_unique<emberEditor::FpsEditorWindow>();
			s_pGameEditorWindow = std::make_unique<emberEditor::GameEditorWindow>();
			s_pHierarchyEditorWindow = std::make_unique<emberEditor::HierarchyEditorWindow>();
			s_pInspectorEditorWindow = std::make_unique<emberEditor::InspectorEditorWindow>();
			s_pOutlineEditorWindow = std::make_unique<emberEditor::OutlineEditorWindow>();
			s_pProjectEditorWindow = std::make_unique<emberEditor::ProjectEditorWindow>();
			s_pViewportEditorWindow = std::make_unique<emberEditor::ViewportEditorWindow>();
			s_pSsaoDispatcherEditorWindow = std::make_unique<emberEditor::SsaoDispatcherEditorWindow>();
			s_pSsrDispatcherEditorWindow = std::make_unique<emberEditor::SsrDispatcherEditorWindow>();

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
		s_pSsrDispatcherEditorWindow.reset();
		s_pSsaoDispatcherEditorWindow.reset();
		s_pViewportEditorWindow.reset();
		s_pProjectEditorWindow.reset();
		s_pOutlineEditorWindow.reset();
		s_pInspectorEditorWindow.reset();
		s_pHierarchyEditorWindow.reset();
		s_pGameEditorWindow.reset();
		s_pFpsEditorWindow.reset();
		s_pDepthBiasEditorWindow.reset();
		s_pDeferredLightingEditorWindow.reset();
		s_pConsoleEditorWindow.reset();
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
					EventConsumerScope consumerScope(EventSystem::Consumer::scene);
					emberEcs::World::EarlyUpdate();

					// Skip delayed physics updates:
					// Runs at most one fixed update per render frame. This avoids stacking physics work when a frame is late, but drops simulation steps under load.
					if (Time::UpdatePhysics(true))
					{
						Compute::Physics::BeginSession();
						emberEcs::World::FixedUpdate();
						Compute::Physics::EndSession();
					}
					// Batch delayed physics updates:
					// Runs every pending fixed update. This preserves fixed-step catch-up, but can create large frames.
					//if (Time::ShouldUpdatePhysics())
					//{
					//	Compute::Physics::BeginSession();
					//	while (Time::UpdatePhysics())
					//		emberEcs::World::FixedUpdate();
					//	Compute::Physics::EndSession();
					//}
				}

				// Game update loop:
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::gui);
					Gui::Update();  // contains hidden nested EventConsumerScope consumerScope(EventSystem::Consumer::editor);
				}
				{
					EventConsumerScope consumerScope(EventSystem::Consumer::scene);
					emberEcs::World::Update();
					emberEcs::World::LateUpdate();
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



	// Getters:
	emberEditor::BackendDebugEditorWindow* Application::GetBackendDebugEditorWindow()
	{
		return s_pBackendDebugEditorWindow.get();
	}
	emberEditor::ConsoleEditorWindow* Application::GetConsoleEditorWindow()
	{
		return s_pConsoleEditorWindow.get();
	}
	emberEditor::DeferredLightingEditorWindow* Application::GetDeferredLightingEditorWindow()
	{
		return s_pDeferredLightingEditorWindow.get();
	}
	emberEditor::DepthBiasEditorWindow* Application::GetDepthBiasEditorWindow()
	{
		return s_pDepthBiasEditorWindow.get();
	}
	emberEditor::FpsEditorWindow* Application::GetFpsEditorWindow()
	{
		return s_pFpsEditorWindow.get();
	}
	emberEditor::GameEditorWindow* Application::GetGameEditorWindow()
	{
		return s_pGameEditorWindow.get();
	}
	emberEditor::HierarchyEditorWindow* Application::GetHierarchyEditorWindow()
	{
		return s_pHierarchyEditorWindow.get();
	}
	emberEditor::InspectorEditorWindow* Application::GetInspectorEditorWindow()
	{
		return s_pInspectorEditorWindow.get();
	}
	emberEditor::OutlineEditorWindow* Application::GetOutlineEditorWindow()
	{
		return s_pOutlineEditorWindow.get();
	}
	emberEditor::ProjectEditorWindow* Application::GetProjectEditorWindow()
	{
		return s_pProjectEditorWindow.get();
	}
	emberEditor::ViewportEditorWindow* Application::GetViewportEditorWindow()
	{
		return s_pViewportEditorWindow.get();
	}
	emberEditor::SsaoDispatcherEditorWindow* Application::GetSsaoDispatcherEditorWindow()
	{
		return s_pSsaoDispatcherEditorWindow.get();
	}
	emberEditor::SsrDispatcherEditorWindow* Application::GetSsrDispatcherEditorWindow()
	{
		return s_pSsrDispatcherEditorWindow.get();
	}
}