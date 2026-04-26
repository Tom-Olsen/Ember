#define SDL_MAIN_HANDLED
#include "profiler.h"
#include "application.h"
#include "emberEngine.h"
#include <memory>
// Scenes:
#include "defaultScene.h"
#include "pointLightScene.h"
#include "shadowCascadeScene.h"
#include "singleQuadScene.h"
#include "testScene.h"
using namespace emberEngine;



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
		appCreateInfo.windowWidth = 2560;//1600; //1920; // 2560;
		appCreateInfo.windowHeight = 1440;// 900; //1080; // 1440;
		appCreateInfo.renderWidth = 2560;//1280; //1280; // 2560;
		appCreateInfo.renderHeight = 1440;// 720; // 720; // 1440;
		bool initialized = emberApplication::Application::Init(appCreateInfo);

		if (!initialized)
			exitCode = 1;
		else
		{
			// Create scene:
			std::unique_ptr<Scene> pScene(DefaultScene());
			// std::unique_ptr<Scene> pScene(PointLightScene());
			// std::unique_ptr<Scene> pScene(ShadowCascadeScene());
			// std::unique_ptr<Scene> pScene(SingleQuadScene());
			// std::unique_ptr<Scene> pScene(TestScene());
			emberApplication::Application::SetScene(pScene.get());

			// Run app:
			emberApplication::Application::Run();

			// App shutdown:
			pScene.reset();
			emberApplication::Application::Clear();
		}
	}
	emberTaskSystem::profiler::Session::Get().End();

	// Runtime analysis:
	std::vector<std::string> results = session.GetAllEventNames();
	for (std::string& result : results)
		session.PrintFunctionAverageTime(result, emberTaskSystem::profiler::TimeUnit::ms);

	return exitCode;
}