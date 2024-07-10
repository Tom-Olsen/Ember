#define SDL_MAIN_HANDLED
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "logger.h"



int main()
{
	// Test Logger:
    Logger::Init();
    LOG_TRACE("Trace");
    LOG_INFO("Info");
    LOG_WARN("Warn");
    LOG_ERROR("Error");
    LOG_CRITICAL("Critical");

	// Test SDL3:
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		LOG_CRITICAL((std::string)"SDL_Init: " + (std::string)SDL_GetError());
	SDL_Window* window = SDL_CreateWindow("Vulkan Tutorial", 1240, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	if (!window)
		LOG_CRITICAL((std::string)"SDL_CreateWindow Error: " + (std::string)SDL_GetError());

    // Test Vulkan:
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	LOG_TRACE("{0} extensions supported", extensionCount);

    // Test GLM:
    glm::mat4 matrix;
    glm::vec4 vec;
    glm::vec4 test = matrix * vec;

    // Keep window open:
	bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
            }
        }
    }

    // Terminate:
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}