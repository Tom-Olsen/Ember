#include "application.h"
#include "time.h"
#include "macros.h"
#include "logger.h"



// Constructor:
Application::Application()
{
	scene = nullptr;
	uint32_t framesInFlight = 2;
	context = std::make_unique<VulkanContext>(framesInFlight);
	renderer = std::make_unique<VulkanRenderer>(context.get());

	// Init static managers:
	MeshManager::Init(context.get());
	MaterialManager::Init(context.get(), &renderer->forwardRenderPass->renderPass);
	TextureManager::Init(context.get());
	SamplerManager::Init(context.get());
}



// Destructor:
Application::~Application()
{
	// Clear static managers:
	SamplerManager::Clear();
	TextureManager::Clear();
	MaterialManager::Clear();
	MeshManager::Clear();
}



// Public methods:
void Application::Run()
{
	Time::Reset();
	bool rebuildSwapchain = false;

	bool running = true;
	while (running)
	{
		Time::Update();
		running = context->window->HandelEvents();

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

		// Resize Swapchain if needed:
		if (rebuildSwapchain || windowExtent.width != surfaceExtend.width || windowExtent.height != surfaceExtend.height)
		{
			rebuildSwapchain = false;
			context->ResetFrameIndex();
			renderer->ResizeSwapchain();
		}

		Update();
		renderer->Render(scene);
	}
}
void Application::SetScene(Scene* scene)
{
	this->scene = scene;
}



// Private methods:
void Application::Update()
{
	// Update all game objects:
	for (auto& gameObj : scene->gameObjects)
	{
		GameObject* gameObject = gameObj.second.get();
		if (gameObject->isActive)
		{
			//LOG_INFO(gameObject->name);
			for (auto& comp : gameObject->components)
			{
				Component* component = comp.second.get();
				//component->PrintType();
				if (component->isActive)
					component->Update();
			}
		}
	}
}