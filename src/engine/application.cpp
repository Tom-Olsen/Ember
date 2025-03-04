#include "application.h"
#include "bufferManager.h"
#include "component.h"
#include "computeShaderManager.h"
#include "dearImGui.h"
#include "eventSystem.h"
#include "gameObject.h"
#include "graphics.h"
#include "logger.h"
#include "materialManager.h"
#include "meshManager.h"
#include "renderPassManager.h"
#include "samplerManager.h"
#include "scene.h"
#include "textureManager.h"
#include "timer.h"
#include "vulkanContext.h"
#include "vulkanRenderer.h"



namespace emberEngine
{
	// Constructor:
	Application::Application()
	{
		Logger::Init();

		// Default options:
		m_pActiveScene = nullptr;
		uint32_t framesInFlight = 2;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_4_BIT;

		// Init static managers:
		VulkanContext::Init(framesInFlight, msaaSamples);
		math::Random::Init();
		EventSystem::Init();
		RenderPassManager::Init();
		ComputeShaderManager::Init();
		MaterialManager::Init();
		BufferManager::Init();
		TextureManager::Init();
		SamplerManager::Init();
		MeshManager::Init();
		Graphics::Init();
		DearImGui::Init();

		// Create renderer:
		m_pRenderer = std::make_unique<VulkanRenderer>();
	}



	// Destructor:
	Application::~Application()
	{
		VulkanContext::WaitDeviceIdle();
		// Clear static managers:
		DearImGui::Clear();
		Graphics::Clear();
		MeshManager::Clear();
		SamplerManager::Clear();
		TextureManager::Clear();
		BufferManager::Clear();
		MaterialManager::Clear();
		ComputeShaderManager::Clear();
		RenderPassManager::Clear();
		EventSystem::Clear();
		VulkanContext::Clear();
	}



	// Public methods:
	void Application::Run()
	{
		Timer::Reset();
		bool running = true;
		Start();

		while (running)
		{
			Timer::Update();
			DearImGui::Update();
			running = VulkanContext::pWindow->HandleEvents();

			// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
			VkExtent2D windowExtent = VulkanContext::pWindow->GetExtent();
			VkExtent2D surfaceExtend = VulkanContext::pSurface->GetCurrentExtent();
			if (VulkanContext::pWindow->GetIsMinimized() || windowExtent.width == 0 || windowExtent.height == 0 || surfaceExtend.width == 0 || surfaceExtend.height == 0)
			{
				SDL_Delay(10);
				continue;
			}

			// Game update loop:
			Update();
			LateUpdate();

			// Render loop:
			if (m_pRenderer->RenderFrame(m_pActiveScene))
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
}