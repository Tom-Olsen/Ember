#define SDL_MAIN_HANDLED
#include "profiler.h"
#include "application.h"
#include "emberEngine.h"
// Components:
#include "sphFluid2dCpu.h"
#include "sphFluid2dGpu.h"
#include "sphFluid3d.h"
using namespace emberEngine;
using namespace fluidDynamics;



Scene* InitScene()
{
	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(0.0f, 0.0f, 17.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		//pTransform->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);
		//CameraController* cameraController = entity.AddComponent<CameraController>();

		pScene->SetActiveCamera(pCamera);
	}
	//{ // Sph Fluid 2d Cpu:
	//	Entity entity = Entity::Create("sphFluid2dCpu");
	//	SphFluid2dCpu* pSphFluid2dCpu = entity.AddComponent<SphFluid2dCpu>();
	//}
	{ // Sph Fluid 2d Gpu:
		Entity entity = Entity::Create("sphFluid2dGpu");
		SphFluid2dGpu* pSphFluid2dGpu = entity.AddComponent<SphFluid2dGpu>();
	}
	//{ // Sph Fluid 3d:
	//	Entity entity = Entity::Create("sphFluid3d");
	//	SphFluid3d* pSphFluid3d = entity.AddComponent<SphFluid3d>();
	//}
	return pScene;
}



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
		appCreateInfo.vSyncEnabled = true;
		appCreateInfo.framesInFlight = 2;
		appCreateInfo.msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount04;
		appCreateInfo.windowWidth = 1600;// 2560; //1920;
		appCreateInfo.windowHeight = 900;// 1440; //1080;
		appCreateInfo.renderWidth = 1600;// 2560; //1280;
		appCreateInfo.renderHeight = 900;// 1440; //720;
		emberApplication::Application::Init(appCreateInfo);

		// Add project specific shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		Material::Create(emberCommon::MaterialType::forwardTransparent, "particleMaterial2d", emberCommon::RenderQueue::transparent, directoryPath / "particle2d.vert.spv", directoryPath / "particle2d.frag.spv");
		Material::Create(emberCommon::MaterialType::forwardOpaque, "particleMaterial3d", emberCommon::RenderQueue::transparent, directoryPath / "particle3d.vert.spv", directoryPath / "particle3d.frag.spv");

		// Create scene:
		Scene* pScene = InitScene();
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