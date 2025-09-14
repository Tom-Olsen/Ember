#include "application.h"
#include "component.h"
#include "dearImGui.h"
#include "emberTime.h"
#include "eventSystem.h"
#include "gameObject.h"
#include "logger.h"
#include "macros.h"
#include "profiler.h"
#include "renderer.h"
#include "rendererCreateInfo.h"
#include "scene.h"
#include "shadowConstants.h"
#include "systemInitializer.h"
#include "window.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Application::Application(const CreateInfo& applicationCreateInfo)
	{
		try
		{
			// Initialize core systems:
			m_pActiveScene = nullptr;
			SystemInitializer::Init(applicationCreateInfo.windowWidth, applicationCreateInfo.windowHeight);

			// Create renderer:
			RendererCreateInfo rendererCreateInfo = {};
			rendererCreateInfo.pIWindow = Window::GetInterfaceHandle();					// handle pass through.
			rendererCreateInfo.pIDearImGui = DearImGui::GetInterfaceHandle();			// handle pass through.
			rendererCreateInfo.vSyncEnabled = applicationCreateInfo.vSyncEnabled;		// project settings.
			rendererCreateInfo.framesInFlight = applicationCreateInfo.framesInFlight;	// project settings.
			rendererCreateInfo.msaaSampleCount = applicationCreateInfo.msaaSampleCount;	// project settings.
			rendererCreateInfo.renderWidth = applicationCreateInfo.renderWidth;			// project settings.
			rendererCreateInfo.renderHeight = applicationCreateInfo.renderHeight;		// project settings.
			rendererCreateInfo.enableDearImGui = true;									// application dependent.
			rendererCreateInfo.enableDockSpace = true;									// application dependent.
			rendererCreateInfo.maxDirectionalLights = MAX_DIR_LIGHTS;					// controlled via macro for now.
			rendererCreateInfo.maxPositionalLights = MAX_POS_LIGHTS;					// controlled via macro for now.
			rendererCreateInfo.shadowMapResolution = SHADOW_MAP_RESOLUTION;				// controlled via macro for now.
			m_pRenderer = std::make_unique<Renderer>(rendererCreateInfo);

			#ifdef LOG_INITIALIZATION
			LOG_TRACE("Application initialized.");
			#endif
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception: {}", e.what());
		}
	}
	Application::~Application()
	{
		m_pRenderer.release();
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

				GarbageCollector::Cleanup();
				running = EventSystem::ProcessEvents();

				// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
				VkExtent2D surfaceExtend = Context::surface.GetCurrentExtent();
				Int2 windowSize = Window::GetSize();
				if (Window::GetIsMinimized() || windowSize.x == 0 || windowSize.y == 0 || surfaceExtend.width == 0 || surfaceExtend.height == 0)
				{
					//SDL_Delay(10);
					continue;
				}

				// Physics Update Loop:
				while (Time::UpdatePhysics())
					FixedUpdate();

				// Game update loop:
				DearImGui::Update();
				Update();
				LateUpdate();
				m_pRenderer->RenderFrame();
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