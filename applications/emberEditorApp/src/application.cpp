#include "application.h"
#include "bufferManager.h"
#include "commonRendererCreateInfo.h"
#include "component.h"
#include "computeShaderManager.h"
#include "compute.h"
#include "editor.h"
#include "emberMath.h"
#include "emberTime.h"
#include "eventSystem.h"
#include "gameObject.h"
#include "gui.h"
#include "logger.h"
#include "materialManager.h"
#include "meshManager.h"
#include "profiler.h"
#include "renderer.h"
#include "scene.h"
#include "shadowConstants.h"
#include "textureManager.h"
#include "window.h"
// Backends:
#include "nullWindow.h"
#include "sdlWindow.h"
#include "vulkanRenderer.h"
#include "vulkanAsyncCompute.h"
#include "vulkanCompute.h"
#include "vulkanImmediateCompute.h"
#include "vulkanPostRenderCompute.h"
#include "vulkanPreRenderCompute.h"
#include "nullGui.h"
#include "imGuiSdlVulkan.h"
// Editor Windows:
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
	// Public methods:
	// Constructor/Destructor:
	Application::Application(const CreateInfo& applicationCreateInfo)
	{
		try
		{
			m_pActiveScene = nullptr;

			// Basic systems:
			emberLogger::Logger::Init();
			math::Random::Init();
			emberEngine::EventSystem::Init();

			// Window backend:
			emberBackendInterface::IWindow* pIWindow;
			if (true)
				pIWindow = new sdlWindowBackend::Window(applicationCreateInfo.windowWidth, applicationCreateInfo.windowHeight);
			else
				pIWindow = new nullWindowBackend::Window();

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
			emberBackendInterface::IRenderer* pIRenderer;
			if (true)
				pIRenderer = new vulkanRendererBackend::Renderer(rendererCreateInfo, pIWindow);

			// Compute backend:
			emberBackendInterface::ICompute* pICompute;
			if (true)
				pICompute = new vulkanRendererBackend::Compute();

			// Gui backend:
			emberBackendInterface::IGui* pIGui;
			if (true)
				pIGui = new imGuiSdlVulkanBackend::Gui(pIWindow, pIRenderer, rendererCreateInfo.enableDockSpace);
			//else
			//	pIGui = new nullGuiBackend::Gui();

			// Link backends together:
			pIRenderer->LinkIGuiHandle(pIGui);			// needed so renderer can inject gui draw calls in present renderpass.
			pIRenderer->LinkIComputeHandle(pICompute);	// needed for pre- and post-render compute shaders.
			pIWindow->LinkIGuiHandle(pIGui);			// needed for window->gui event passthrough.
			pIGui->SetEditorCallbacks(emberEngine::Editor::Render, emberEngine::Editor::GetFocusedWindowWantCaptureEvents);

			// Backend wrappers:
			emberEngine::Window::Init(pIWindow);
			emberEngine::Renderer::Init(pIRenderer);
			emberEngine::Compute::Init(pICompute);
			emberEngine::Gui::Init(pIGui);

			// Gpu Resource Managers:
			emberEngine::ComputeShaderManager::Init();
			emberEngine::MaterialManager::Init();
			emberEngine::BufferManager::Init();
			emberEngine::TextureManager::Init();
			emberEngine::MeshManager::Init();

			// Editor windows:
			m_pConsoleEditorWindow = std::make_unique<emberEditor::ConsoleEditorWindow>();
			m_pDepthBiasEditorWindow = std::make_unique<emberEditor::DepthBiasEditorWindow>();
			m_pFpsEditorWindow = std::make_unique<emberEditor::FpsEditorWindow>();
			m_pGameEditorWindow = std::make_unique<emberEditor::GameEditorWindow>();
			m_pHierarchyEditorWindow = std::make_unique<emberEditor::HierarchyEditorWindow>();
			m_pInspectorEditorWindow = std::make_unique<emberEditor::InspectorEditorWindow>();
			m_pProjectEditorWindow = std::make_unique<emberEditor::ProjectEditorWindow>();
			m_pSceneEditorWindow = std::make_unique<emberEditor::SceneEditorWindow>();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
	}
	Application::~Application()
	{
		// Wait for gpu to finish any remaining work:
		emberEngine::Renderer::WaitDeviceIdle();

		// Gpu Resource Managers:
		emberEngine::MeshManager::Clear();
		emberEngine::TextureManager::Clear();
		emberEngine::BufferManager::Clear();
		emberEngine::MaterialManager::Clear();
		emberEngine::ComputeShaderManager::Clear();

		// Backends:
		emberEngine::Gui::Clear();
		emberEngine::Compute::Clear();
		emberEngine::Renderer::Clear();
		emberEngine::Window::Clear();

		// Basic systems:
		emberEngine::EventSystem::Clear();
		math::Random::Clear();
		emberLogger::Logger::Clear();
	}



	// Movable:
	Application::Application(Application&& other) noexcept = default;
	Application& Application::operator=(Application&& other) noexcept = default;



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
		this->m_pActiveScene = pScene;
		m_pHierarchyEditorWindow->SetScene(pScene);
	}



	// Getters:
	emberEditor::ConsoleEditorWindow* Application::GetConsoleEditorWindow() const
	{
		return m_pConsoleEditorWindow.get();
	}
	emberEditor::DepthBiasEditorWindow* Application::GetDepthBiasEditorWindow() const
	{
		return m_pDepthBiasEditorWindow.get();
	}
	emberEditor::FpsEditorWindow* Application::GetFpsEditorWindow() const
	{
		return m_pFpsEditorWindow.get();
	}
	emberEditor::GameEditorWindow* Application::GetGameEditorWindow() const
	{
		return m_pGameEditorWindow.get();
	}
	emberEditor::HierarchyEditorWindow* Application::GetHierarchyEditorWindow() const
	{
		return m_pHierarchyEditorWindow.get();
	}
	emberEditor::InspectorEditorWindow* Application::GetInspectorEditorWindow() const
	{
		return m_pInspectorEditorWindow.get();
	}
	emberEditor::ProjectEditorWindow* Application::GetProjectEditorWindow() const
	{
		return m_pProjectEditorWindow.get();
	}
	emberEditor::SceneEditorWindow* Application::GetSceneEditorWindow() const
	{
		return m_pSceneEditorWindow.get();
	}
}