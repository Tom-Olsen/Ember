#include "application.h"
#include "component.h"
#include "eventSystem.h"
#include "gameObject.h"
#include "graphics.h"
#include "materialManager.h"
#include "meshManager.h"
#include "renderPassManager.h"
#include "samplerManager.h"
#include "scene.h"
#include "textureManager.h"
#include "timer.h"
#include "vulkanContext.h"
#include "vulkanRenderer.h"



// Constructor:
Application::Application()
{
	m_pActiveScene = nullptr;
	uint32_t framesInFlight = 2;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_4_BIT;
	m_pContext = std::make_unique<VulkanContext>(framesInFlight, msaaSamples);
	m_pRenderer = std::make_unique<VulkanRenderer>(m_pContext.get());

	// Init static managers:
	EventSystem::Init(m_pContext.get());
	RenderPassManager::Init(m_pContext.get());
	MaterialManager::Init(m_pContext.get());
	TextureManager::Init(m_pContext.get());
	SamplerManager::Init(m_pContext.get());
	MeshManager::Init(m_pContext.get());
	Graphics::Init();
}



// Destructor:
Application::~Application()
{
	// Clear static managers:
	Graphics::Clear();
	MeshManager::Clear();
	SamplerManager::Clear();
	TextureManager::Clear();
	MaterialManager::Clear();
	RenderPassManager::Clear();
	EventSystem::Clear();
}



// Public methods:
void Application::Run()
{
	Timer::Reset();
	bool running = true;

	while (running)
	{
		Timer::Update();
		running = m_pContext->pWindow->HandleEvents();

		// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
		VkExtent2D windowExtent = m_pContext->pWindow->GetExtent();
		VkExtent2D surfaceExtend = m_pContext->pSurface->GetCurrentExtent();
		if (m_pContext->pWindow->GetIsMinimized() || windowExtent.width == 0 || windowExtent.height == 0 || surfaceExtend.width == 0 || surfaceExtend.height == 0)
		{
			SDL_Delay(10);
			continue;
		}

		// Game update loop:
		Update();
		LateUpdate();

		// Render loop:
		if (m_pRenderer->RenderFrame(m_pActiveScene))
			m_pContext->UpdateFrameIndex();
	}
}
void Application::SetScene(Scene* pScene)
{
	this->m_pActiveScene = pScene;
}



// Private methods:
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