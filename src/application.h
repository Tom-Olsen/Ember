#ifndef __INCLUDE_GUARD_application_h__
#define __INCLUDE_GUARD_application_h__
#include <memory>
#include "logger.h"
#include "sdlWindow.h"
#include "vulkanContext.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"


class Application
{
private: // Members:
	std::unique_ptr<SdlWindow> window;
	std::unique_ptr<VulkanContext> context;
	std::unique_ptr<VulkanSurface> surface;
	std::unique_ptr<VulkanSwapchain> swapchain;

public: // Methods:
	Application();
	~Application();
	void Run();
};
#endif // __INCLUDE_GUARD_application_h__