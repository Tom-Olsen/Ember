#define SDL_MAIN_HANDLED
#include "application.h"
#include "emberEngine.h"
// Components:
#include "camera.h"
#include "cameraController.h"
#include "directionalLight.h"
#include "meshRenderer.h"
#include "pointLight.h"
#include "postRenderEffects.h"
#include "spotLight.h"
#include "transform.h"
#include "sphFluid2dCpu.h"
#include "sphFluid2dGpu.h"
#include "sphFluid3d.h"
using namespace emberEngine;



Scene* InitScene()
{
	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, 0.0f, 17.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		//pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));

		Camera* pCamera = pGameObject->AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);
		CameraController* cameraController = pGameObject->AddComponent<CameraController>();

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	//{ // Sph Fluid 2d Cpu:
	//	GameObject* pGameObject = new GameObject("sphFluid2dCpu");
	//	SphFluid2dCpu* pSphFluid2dCpu = pGameObject->AddComponent<SphFluid2dCpu>();
	//	pScene->AddGameObject(pGameObject);
	//}
	//{ // Sph Fluid 2d Gpu:
	//	GameObject* pGameObject = new GameObject("sphFluid2dGpu");
	//	SphFluid2dGpu* pSphFluid2dGpu = pGameObject->AddComponent<SphFluid2dGpu>();
	//	pScene->AddGameObject(pGameObject);
	//}
	{ // Sph Fluid 3d:
		GameObject* pGameObject = new GameObject("sphFluid3d");
		SphFluid3d* pSphFluid3d = pGameObject->AddComponent<SphFluid3d>();
		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}



int main()
{
	// VS debugging (windows only=MSVC compiler):
	#ifdef _MSC_VER
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	// Initialization:
	Application::Settings appSettings = {};
	appSettings.vSyncEnabled = true;
	appSettings.framesInFlight = 2;
	appSettings.msaaSamples = VK_SAMPLE_COUNT_4_BIT;
	appSettings.windowWidth = 1920;
	appSettings.windowHeight = 1080;
	appSettings.renderWidth = 1280;
	appSettings.renderHeight = 720;

	Application app(appSettings);

	// Add project specific shaders:
	std::string directoryPath = (std::string)PROJECT_ROOT_PATH + "/bin/shaders";
	Material* pParticleMaterial2d = new Material(Material::Type::forwardTransparent, "particleMaterial2d", (uint32_t)Material::Queue::transparent, directoryPath + "/particle2d.vert.spv", directoryPath + "/particle2d.frag.spv");
	Material* pParticleMaterial3d = new Material(Material::Type::forwardOpaque, "particleMaterial3d", (uint32_t)Material::Queue::transparent, directoryPath + "/particle3d.vert.spv", directoryPath + "/particle3d.frag.spv");
	MaterialManager::AddMaterial(pParticleMaterial2d);
	MaterialManager::AddMaterial(pParticleMaterial3d);

	// Create scene:
	Scene* pScene = InitScene();
	app.SetScene(pScene);

	// Run application:
	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Exception: {}", e.what());
	}

	// Terminate:
	delete pScene;
	return 0;
}