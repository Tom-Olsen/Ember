#define SDL_MAIN_HANDLED
#include "application.h"
#include "logger.h"
#include "glmTypes.h"
#include <iostream>
#include "gameObject.h"
#include "camera.h"
#include <crtdbg.h>



int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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