#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"
#include "glmTypes.h"
#include <iostream>



int main()
{
	// Test Logger:
    Logger::Init();
    //LOG_TRACE("Trace");
    //LOG_INFO("Info");
    //LOG_WARN("Warn");
    //LOG_ERROR("Error");
    //LOG_CRITICAL("Critical");

    try
    {
        Application app;
        app.Run();
    }
	catch (const std::runtime_error& e)
	{
		LOG_CRITICAL(e.what());
	}

    return 0;
}