#define SDL_MAIN_HANDLED
#include "application.h"
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

    Application app;
	app.Run();
    return 0;
}