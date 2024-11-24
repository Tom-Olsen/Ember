#include "application.h"
#include "timer.h"
#include "logger.h"



// Constructor:
Application::Application()
{
	activeScene = nullptr;
	uint32_t framesInFlight = 2;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_4_BIT;
	context = std::make_unique<VulkanContext>(framesInFlight, msaaSamples);
	renderer = std::make_unique<VulkanRenderer>(context.get());

	// Init static managers:
	EventSystem::Init(context->window.get());
	RenderPassManager::Init(context.get());
	MaterialManager::Init(context.get(), renderer.get());
	TextureManager::Init(context.get());
	SamplerManager::Init(context.get());
	MeshManager::Init(context.get());
}



// Destructor:
Application::~Application()
{
	// Clear static managers:
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
		running = context->window->HandleEvents();

		// If window is minimized or width/height is zero, delay loop to reduce CPU usage:
		VkExtent2D windowExtent = context->window->Extent();
		VkExtent2D surfaceExtend = context->surface->CurrentExtent();
		if (context->window->isMinimized || windowExtent.width == 0 || windowExtent.height == 0 || surfaceExtend.width == 0 || surfaceExtend.height == 0)
		{
			SDL_Delay(10);
			continue;
		}

		// QUESTION:
		// -what is the exact difference between window and surface and how can it be that the surface extent differs from the window extent?
		// -are both extend checks in the above if condition necessary?

		// Game update loop:
		Update();

		// Render loop:
		if (renderer->Render(activeScene))
			context->UpdateFrameIndex();
	}
}
void Application::SetScene(Scene* scene)
{
	this->activeScene = scene;
}



// Private methods:
void Application::Update()
{
	// Update all components of all game objects:
	for (auto& [_, gameObject] : activeScene->gameObjects)
	{
		if (gameObject->isActive)
		{
			for (auto& [_, component] : gameObject->components)
			{
				if (component->isActive)
					component->Update();
			}
		}
	}
}