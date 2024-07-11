#include "application.h"



// Constructor:
Application::Application()
{
	window = std::make_unique<SdlWindow>();

	// Get instance extensions:
	std::vector<const char*> extensions;
	window->SdlInstanceExtensions(extensions);	// sdl extensions
	// and more ...

	// Get device extensions:
	std::vector<const char*> deviceExtensions;
	deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	// and more ...

	// Create vulkan context, swapchain, renderpass:
	context = std::make_unique<VulkanContext>((uint32_t)extensions.size(), extensions.data(), (uint32_t)deviceExtensions.size(), deviceExtensions.data());
	surface = std::make_unique<VulkanSurface>(context.get(), window.get());
	swapchain = std::make_unique<VulkanSwapchain>(window.get(), context.get(), surface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, nullptr);
}



// Destructor:
Application::~Application()
{
	
}



// Main Loop:
void Application::Run()
{
	while (window->HandelEvents())
	{
		// Render stuff here
	}
}