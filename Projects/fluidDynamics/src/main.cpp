#define SDL_MAIN_HANDLED
#include "application.h"
#include "emberEngine.h"
// Components:
#include "camera.h"
#include "cameraController.h"
#include "directionalLight.h"
#include "meshRenderer.h"
#include "pointLight.h"
#include "postProcessEffects.h"
#include "spotLight.h"
#include "transform.h"
#include "sphFluid2d.h"
using namespace emberEngine;



Scene* InitScene()
{
	bool directionalLightsActive = false;
	bool showLightFrustums = false;

	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, -17.0f, 0.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));

		Camera* pCamera = new Camera();
		pCamera->SetFarClip(1000.0f);
		pGameObject->AddComponent<Camera>(pCamera);

		PostProcessEffects* pPostProcessEffects = new PostProcessEffects();
		pGameObject->AddComponent<PostProcessEffects>(pPostProcessEffects);

		CameraController* cameraController = new CameraController();
		pGameObject->AddComponent<CameraController>(cameraController);

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{ // Sph Fluid 2d:
		GameObject* pGameObject = new GameObject("sphFluid2d");
		pGameObject->GetTransform()->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		SphFluid2d* sphFluid2d = pGameObject->AddComponent<SphFluid2d>();

		pScene->AddGameObject(pGameObject);
	}
	//{
	//	GameObject* pGameObject = new GameObject("cube");
	//	pGameObject->GetTransform()->SetScale(0.2f);
	//
	//	MeshRenderer* meshRenderer = pGameObject->AddComponent<MeshRenderer>();
	//	meshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
	//	meshRenderer->SetMaterial(MaterialManager::GetMaterial("normalMaterial"));
	//
	//	pScene->AddGameObject(pGameObject);
	//}
	{ // FPS editorWindow:

	}
	return pScene;
}



int main()
{
	// VS debugging:
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialization:
	Application::Settings appSettings = {};
	appSettings.vSyncEnabled = true;
	appSettings.framesInFlight = 2;
	appSettings.msaaSamples = VK_SAMPLE_COUNT_4_BIT;
	appSettings.windowWidth = 1920;
	appSettings.windowHeight = 1080;
	appSettings.renderWidth = 1280;
	appSettings.renderheight = 720;

	Application app(appSettings);
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