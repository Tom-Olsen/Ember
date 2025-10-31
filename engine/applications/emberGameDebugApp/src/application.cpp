#include "application.h"
#include "bufferManager.h"
#include "commonRendererCreateInfo.h"
#include "component.h"
#include "computeShaderManager.h"
#include "compute.h"
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
#include "imGuiSdlVulkan.h"
#include "nullGui.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Application::Application(const CreateInfo& applicationCreateInfo)
	{
		try
		{
			m_pActiveScene = nullptr;

			// Basic systems:
			emberLogger::Logger::Init();
			math::Random::Init();
			EventSystem::Init();



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
			rendererCreateInfo.enableDockSpace = false;									// application dependent.
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
			else
				pIGui = new nullGuiBackend::Gui();

			// Link backends together:
			pIRenderer->SetIGuiHandle(pIGui);			// needed so renderer can inject gui draw calls in present renderpass.
			pIRenderer->SetIComputeHandle(pICompute);	// needed for pre- and post-render compute shaders.
			pIWindow->LinkDearImGui(pIGui);				// needed for window->gui event passthrough.



			// Backend wrappers:
			Window::Init(pIWindow);
			Renderer::Init(pIRenderer);
			Compute::Init(pICompute);
			Gui::Init(pIGui);

			// Gpu Resource Managers:
			ComputeShaderManager::Init();
			MaterialManager::Init();
			BufferManager::Init();
			TextureManager::Init();
			MeshManager::Init();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
	}
	Application::~Application()
	{
		// Wait for gpu to finish any remaining work:
		Renderer::WaitDeviceIdle();

		// Gpu Resource Managers:
		MeshManager::Clear();
		TextureManager::Clear();
		BufferManager::Clear();
		MaterialManager::Clear();
		ComputeShaderManager::Clear();

		// Renderer/Gui:
		Gui::Clear();
		Compute::Clear();
		Renderer::Clear();

		// Window/Event systems:
		EventSystem::Clear();
		Window::Clear();

		// Basic systems:
		math::Random::Clear();
		emberLogger::Logger::Clear();
	}



	// Public methods:
	void Application::Run()
	{
		try
		{
			bool running = true;
			Time::Reset();
			Start();

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
				{
					//SDL_Delay(10);
					continue;
				}

				// Physics Update Loop:
				while (Time::UpdatePhysics())
					FixedUpdate();

				// Game update loop:
				Gui::Update();
				Update();
				LateUpdate();
				Renderer::RenderFrame();
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
	}
	void Application::SetScene(Scene* pScene)
	{
		this->m_pActiveScene = pScene;
		//Editor::SetScene(pScene);
	}



	// Private methods:
	void Application::Start()
	{
		// Start all components of all game objects:
		for (auto& [_, gameObject] : m_pActiveScene->GetGameObjects())
		{
			if (gameObject->isActive)
			{
				for (auto& [_, component] : gameObject->GetComponents())
				{
					if (component->isActive)
						component->Start();
				}
			}
		}
	}
	void Application::Update()
	{
		PROFILE_FUNCTION();
		// Update all components of all game objects:
		for (auto& [_, gameObject] : m_pActiveScene->GetGameObjects())
		{
			if (gameObject->isActive)
			{
				for (auto& [_, component] : gameObject->GetComponents())
				{
					if (component->isActive)
						component->Update();
				}
			}
		}
	}
	void Application::LateUpdate()
	{
		PROFILE_FUNCTION();
		// Late update all components of all game objects:
		for (auto& [_, gameObject] : m_pActiveScene->GetGameObjects())
		{
			if (gameObject->isActive)
			{
				for (auto& [_, component] : gameObject->GetComponents())
				{
					if (component->isActive)
						component->LateUpdate();
				}
			}
		}
	}
	void Application::FixedUpdate()
	{
		PROFILE_FUNCTION();
		// Fixed update all components of all game objects:
		for (auto& [_, gameObject] : m_pActiveScene->GetGameObjects())
		{
			if (gameObject->isActive)
			{
				for (auto& [_, component] : gameObject->GetComponents())
				{
					if (component->isActive)
						component->FixedUpdate();
				}
			}
		}
	}
}