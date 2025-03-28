#include "application.h"
#include "bufferManager.h"
#include "component.h"
#include "computeShaderManager.h"
#include "dearImGui.h"
#include "editor.h"
#include "eventSystem.h"
#include "gameObject.h"
#include "logger.h"
#include "materialManager.h"
#include "meshManager.h"
#include "renderPassManager.h"
#include "samplerManager.h"
#include "scene.h"
#include "textureManager.h"
#include "emberTime.h"
#include "vulkanContext.h"
#include "vulkanGarbageCollector.h"
#include "vulkanRenderer.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Application::Application(Settings settings)
	{
		Logger::Init();
		m_pActiveScene = nullptr;

		// Init static managers:
		VulkanContext::Init(settings.framesInFlight, settings.msaaSamples, settings.windowWidth, settings.windowHeight, settings.vSyncEnabled);
		VulkanGarbageCollector::Init();
		math::Random::Init();
		EventSystem::Init();
		RenderPassManager::Init(settings.renderWidth, settings.renderheight);
		ComputeShaderManager::Init();
		MaterialManager::Init();
		BufferManager::Init();
		TextureManager::Init();
		SamplerManager::Init();
		MeshManager::Init();
		Editor::Init();
		DearImGui::Init();

		// Create renderer:
		m_pRenderer = std::make_unique<VulkanRenderer>();
	}
	Application::~Application()
	{
		VulkanContext::WaitDeviceIdle();
		// Clear static managers:
		DearImGui::Clear();
		Editor::Clear();
		MeshManager::Clear();
		SamplerManager::Clear();
		TextureManager::Clear();
		BufferManager::Clear();
		MaterialManager::Clear();
		ComputeShaderManager::Clear();
		RenderPassManager::Clear();
		EventSystem::Clear();
		VulkanGarbageCollector::Clear();
		VulkanContext::Clear();
	}



	// Public methods:
	void Application::Run()
	{
		bool running = true;
		Time::Init();
		Start();

		while (running)
		{
			Time::Update();

			VulkanGarbageCollector::CollectGarbage();
			running = VulkanContext::pWindow->HandleEvents();

			// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
			VkExtent2D windowExtent = VulkanContext::pWindow->GetExtent();
			VkExtent2D surfaceExtend = VulkanContext::pSurface->GetCurrentExtent();
			if (VulkanContext::pWindow->GetIsMinimized() || windowExtent.width == 0 || windowExtent.height == 0 || surfaceExtend.width == 0 || surfaceExtend.height == 0)
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
				VulkanContext::UpdateFrameIndex();
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