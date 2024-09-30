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

	//Transform transformA(Float3(1.0f, 2.0f, 3.0f), Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 1.0f, 1.0f));
	//Transform transformB(Float3(1.0f, 2.0f, 3.0f), Float3(90.0f, 0.0f, 0.0f), Float3(1.0f, 1.0f, 1.0f));
	//Transform transformC(Float3(1.0f, 2.0f, 3.0f), Float3(0.0f, 90.0f, 0.0f), Float3(2.0f, 2.0f, 2.0f));
	//LOG_TRACE("Transform A:\n{}\n", to_string(transformA.GetLocalToWorldMatrix()));
	//LOG_TRACE("Transform A inv:\n{}\n", to_string(transformA.GetWorldToLocalMatrix()));
	//LOG_TRACE("Transform B:\n{}\n", to_string(transformB.GetLocalToWorldMatrix()));
	//LOG_TRACE("Transform B inv:\n{}\n", to_string(transformB.GetWorldToLocalMatrix()));
	//LOG_TRACE("Transform C:\n{}\n", to_string(transformC.GetLocalToWorldMatrix()));
	//LOG_TRACE("Transform C inv:\n{}\n", to_string(transformC.GetWorldToLocalMatrix()));
    return 0;
}