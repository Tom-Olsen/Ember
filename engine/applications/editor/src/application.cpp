#include "application.h"
#include "compute.h"
#include "component.h"
#include "dearImGui.h"
#include "emberTime.h"
#include "eventSystem.h"
#include "gameObject.h"
#include "graphics.h"
#include "lighting.h"
#include "logger.h"
#include "macros.h"
#include "managers.h"
#include "profiler.h"
#include "renderCore.h"
#include "scene.h"
#include "vulkanContext.h"
#include "vulkanGarbageCollector.h"
#include "vulkanSingleTimeCommand.h"
#include "window.h"



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Destructor:
	Application::Application(const Settings& settings)
	{
		try
		{
			m_pActiveScene = nullptr;

			// Init static classes:
			bool enableDockSpace = false;
			emberLogger::Logger::Init();
			Window::Init(settings.windowWidth, settings.windowHeight);
			Context::Init(settings.framesInFlight, settings.msaaSamples, settings.vSyncEnabled, enableDockSpace);
			EventSystem::Init();
			GarbageCollector::Init();
			SingleTimeCommand::Init();
			math::Random::Init();
			Managers::Init(settings.renderWidth, settings.renderHeight);
			DearImGui::Init(Window::GetNativeHandle(), enableDockSpace);
			Compute::Init();
			Lighting::Init();
			Graphics::Init();

			// Create renderer:
			m_pRenderer = std::make_unique<RenderCore>();

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
		Context::WaitDeviceIdle();
		m_pRenderer.reset();

		// Clear static classes:
		Graphics::Clear();
		Lighting::Clear();
		Compute::Clear();
		DearImGui::Clear();
		Managers::Clear();
		SingleTimeCommand::Clear();
		GarbageCollector::Clear();
		EventSystem::Clear();
		Context::Clear();
		Window::Clear();
	}



	// Public methods:
	void Application::Run()
	{
		try
		{
			bool running = true;
			Time::Init();
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

				// Render loop:
				if (m_pRenderer->RenderFrame())
					Context::UpdateFrameIndex();
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