#include "application.h"
#include "bitonicSort.h"
#include "compute.h"
#include "component.h"
#include "dearImGui.h"
#include "editor.h"
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



namespace emberEngine
{
	using namespace vulkanBackend;



	// Constructor/Destructor:
	Application::Application(Settings settings)
	{
		Logger::Init();
		m_pActiveScene = nullptr;

		// Init static classes:
		Context::Init(settings.framesInFlight, settings.msaaSamples, settings.windowWidth, settings.windowHeight, settings.vSyncEnabled);
		GarbageCollector::Init();
		SingleTimeCommand::Init();
		math::Random::Init();
		EventSystem::Init();
		Managers::Init(settings.renderWidth, settings.renderHeight);
		Editor::Init();
		DearImGui::Init();
		BitonicSort::Init();
		Compute::Init();
		Lighting::Init();
		Graphics::Init();

		// Create renderer:
		m_pRenderer = std::make_unique<RenderCore>();

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("Application initialized.");
		#endif
	}
	Application::~Application()
	{
		Context::WaitDeviceIdle();
		m_pRenderer.reset();

		// Clear static classes:
		Graphics::Clear();
		Lighting::Clear();
		Compute::Clear();
		BitonicSort::Clear();
		DearImGui::Clear();
		Editor::Clear();
		Managers::Clear();
		EventSystem::Clear();
		SingleTimeCommand::Clear();
		GarbageCollector::Clear();
		Context::Clear();
	}



	// Public methods:
	void Application::Run()
	{
		bool running = true;
		Time::Init();
		Start();

		while (running)
		{
			PROFILE_FUNCTION();
			Time::Update();

			GarbageCollector::Cleanup();
			running = Context::window.HandleEvents();

			// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
			VkExtent2D windowExtent = Context::window.GetExtent();
			VkExtent2D surfaceExtend = Context::surface.GetCurrentExtent();
			if (Context::window.GetIsMinimized() || windowExtent.width == 0 || windowExtent.height == 0 || surfaceExtend.width == 0 || surfaceExtend.height == 0)
			{
				SDL_Delay(10);
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
	void Application::SetScene(Scene* pScene)
	{
		this->m_pActiveScene = pScene;
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