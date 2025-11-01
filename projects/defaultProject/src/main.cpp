#define SDL_MAIN_HANDLED
#include "profiler.h"
#include "application.h"
#include "emberEngine.h"
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
	EmberProfiler::Session& session = EmberProfiler::Session::Get();
	session.Start("profiling", "profilingResults");
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
		emberApplication::Application::Init(appCreateInfo);

		// Create scene:
		 Scene* pScene = DefaultScene();
		// Scene* pScene = PointLightScene();
		// Scene* pScene = ShadowCascadeScene();
		// Scene* pScene = SingleQuadScene();
		// Scene* pScene = TestScene();
		emberApplication::Application::SetScene(pScene);

		// Run app:
		emberApplication::Application::Run();

		// App shutdown:
		delete pScene;
		emberApplication::Application::Clear();
	}
	EmberProfiler::Session::Get().End();

	// Runtime analysis:
	std::vector<std::string> results = session.GetAllResultNames();
	for (std::string& result : results)
		session.PrintFunctionAverageTime(result, EmberProfiler::TimeUnit::ms);

	return 0;
}