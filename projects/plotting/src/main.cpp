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
#include "profiler.h"
#include "spotLight.h"
#include "transform.h"
using namespace emberEngine;



Scene* InitScene()
{
	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, -8.0f, 3.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));

		Camera* pCamera = pGameObject->AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);
		CameraController* cameraController = pGameObject->AddComponent<CameraController>();

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// Skybox:
		GameObject* pGameObject = new GameObject("skybox");

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("skybox0"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		pScene->AddGameObject(pGameObject);
	}
	{// Directional Light:
		GameObject* pGameObject = new GameObject("directionalLight");
		Float3 direction = Float3(-0.4f, 0.4f, -1.0f).Normalize();
		Float3x3 rotation = Float3x3::RotateFromTo(Float3::down, direction);
		pGameObject->GetTransform()->SetPosition(Float3(0.0f, 3.0f, 0.0f));
		pGameObject->GetTransform()->SetRotationMatrix(rotation);

		DirectionalLight* pDirectionalLight = pGameObject->AddComponent<DirectionalLight>();
		pDirectionalLight->SetIntensity(1.0f);
		pDirectionalLight->SetColor(Float3::white);
		pDirectionalLight->SetShadowType(Lighting::ShadowType::soft);
		pDirectionalLight->SetDistributionFactor(0.5f);
		pDirectionalLight->SetShadowCascadeCount(3);

		pScene->AddGameObject(pGameObject);
	}
	{ // Floor:
		GameObject* pGameObject = new GameObject("floor");
		pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, -0.5f);
		pGameObject->GetTransform()->SetScale(30.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("ground0_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("ground0_roughness"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("ground0_normal"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(10, 10, 0, 0));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);

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
	Profiler::Session::Get().Start("defaultProject", "profilingResults");

	// Initialization:
	Application::Settings appSettings = {};
	appSettings.vSyncEnabled = false;
	appSettings.framesInFlight = 2;
	appSettings.msaaSamples = VK_SAMPLE_COUNT_4_BIT;
	appSettings.windowWidth = 2560;//1920;
	appSettings.windowHeight = 1440;//1080;
	appSettings.renderWidth = 2560;//1280;
	appSettings.renderHeight = 1440;//720;
	Graphics::SetDeptBiasSlopeFactor(3.0f);
	Application app(appSettings);

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
	Profiler::Session::Get().End();
	return 0;
}