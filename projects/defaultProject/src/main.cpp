#define SDL_MAIN_HANDLED
#include "application.h"
#include "emberEngine.h"
#include "profiler.h"
// Scenes:
#include "defaultScene.h"
#include "pointLightScene.h"
#include "shadowCascadeScene.h"
#include "singleQuadScene.h"
using namespace emberCore;
using namespace emberEcs;



int main()
{
	// VS debugging (windows only=MSVC compiler):
	#ifdef _MSC_VER
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	// Profiler:
    emberTaskSystem::profiler::Session& session = emberTaskSystem::profiler::Session::Get();
	session.Start("profiling", "profilingResults");
	int exitCode = 0;
	{
		// Initialization:
		emberApplication::Application::CreateInfo appCreateInfo = {};
		appCreateInfo.vSyncEnabled = false;
		appCreateInfo.framesInFlight = 2;
		appCreateInfo.msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount04;
		appCreateInfo.windowWidth = 3840;//2560;//1600; //1920; // 2560;
		appCreateInfo.windowHeight = 2160;//1440;// 900; //1080; // 1440;
		appCreateInfo.renderWidth = 2560;//1280; //1280; // 2560;
		appCreateInfo.renderHeight = 1440;// 720; // 720; // 1440;
		bool initialized = emberApplication::Application::Init(appCreateInfo);

		if (!initialized)
			exitCode = 1;
		else
		{
			// Create scene:
			DefaultScene();
			//PointLightScene();
			//ShadowCascadeScene();
			//SingleQuadScene();

			// Run app:
			emberApplication::Application::Run();

			// App shutdown:
			emberApplication::Application::Clear();
		}
	}
	emberTaskSystem::profiler::Session::Get().End();

	// Runtime analysis:
	session.PrintSessionStats();

	return exitCode;
}