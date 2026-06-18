#define SDL_MAIN_HANDLED
#include "profiler.h"
#include "application.h"
#include "emberEngine.h"
// Components:
#include "sphFluid2dCpu.h"
#include "sphFluid2dGpu.h"
#include "sphFluid3dGpu.h"
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
		CameraController* cameraController = entity.AddComponent<CameraController>();

		pScene->SetActiveCamera(pCamera);
	}
	//{// Directional Light:
	//	Entity entity = Entity::Create("directionalLight");
	//	Float3 pos = 2.0f * Float3(16.0f, 9.0f, 9.0f);
	//	Float3 direction = -pos.Normalize();
	//	Float3x3 matrix = Float3x3::RotateFromTo(Float3::down, direction);
	//
	//	Transform* pTransform = entity.GetTransform();
	//	pTransform->SetPosition(pos);
	//	pTransform->SetRotationMatrix(matrix);
	//
	//	DirectionalLight* pDirectionalLight = entity.AddComponent<DirectionalLight>();
	//	pDirectionalLight->SetIntensity(1.0f);
	//	pDirectionalLight->SetColor(Float3::white);
	//	pDirectionalLight->SetDrawFrustum(false);
	//	pDirectionalLight->SetShadowCascadeCount(1);
	//	//pDirectionalLight->SetShadowCascadeSplit(0, 0.0f);
	//	//pDirectionalLight->SetShadowCascadeSplit(1, 0.2f);
	//	//pDirectionalLight->SetShadowCascadeSplit(2, 0.5f);
	//	//pDirectionalLight->SetShadowCascadeSplit(3, 1.0f);
	//	pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
	//}
	//{// PointLight:
	//	Entity entity = Entity::Create("pointLight");
	//	Float3 pos = 1.1f * Float3(16.0f, 9.0f, 9.0f);;
	//	Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::back, -pos, Float3::forward, Float3::up);
    //
	//	Transform* pTransform = entity.GetTransform();
	//	pTransform->SetPosition(pos);
	//	pTransform->SetRotationMatrix(matrix);
    //
	//	MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
	//	pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
	//	pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("simpleUnlitMaterial"));
	//	pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "diffuseColor", Float4::white);
	//	pMeshRenderer->SetCastShadows(false);
	//	pMeshRenderer->SetReceiveShadows(false);
    //
	//	PointLight* pPointLight = entity.AddComponent<PointLight>();
	//	pPointLight->SetIntensity(5.0f);
	//	pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
	//	pPointLight->SetNearClip(0.5f);
	//	pPointLight->SetFarClip(50.0f);
	//	pPointLight->SetDrawFrustum(true);
	//	pPointLight->SetShadowType(emberCommon::ShadowType::hard);
	//}
	//{ // Sph Fluid 2d Cpu:
	//	Entity entity = Entity::Create("sphFluid2dCpu");
	//	SphFluid2dCpu* pSphFluid2dCpu = entity.AddComponent<SphFluid2dCpu>();
	//}
	//{ // Sph Fluid 2d Gpu:
	//	Entity entity = Entity::Create("sphFluid2dGpu");
	//	SphFluid2dGpu* pSphFluid2dGpu = entity.AddComponent<SphFluid2dGpu>();
	//}
	{ // Sph Fluid 3d Gpu:
		Entity entity = Entity::Create("sphFluid3dGpu");
		SphFluid3dGpu* pSphFluid3dGpu = entity.AddComponent<SphFluid3dGpu>();
	}
	return pScene;
}



int main()
{
	// VS debugging (windows only=MSVC compiler):
	#ifdef _MSC_VER
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	// Profiler:
	emberTaskSystem::profiler::Session& session = emberTaskSystem::profiler::Session::Get();
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
		appCreateInfo.maxDirectionalLights = 1;
		appCreateInfo.maxPositionalLights = 1;
		appCreateInfo.shadowMapResolution = 1024;
		emberApplication::Application::Init(appCreateInfo);

		// Add project specific shaders:
		std::filesystem::path directoryPath = (std::filesystem::path(PROJECT_SHADERS_DIR) / "bin").make_preferred();
		Material::Create(emberCommon::RenderMode::transparent, "particleMaterial2d", emberCommon::RenderQueue::transparent, directoryPath / "particle2d.vert.spv", directoryPath / "particle2d.frag.spv");
		Material::Create(emberCommon::RenderMode::opaque, "particleMaterial3d", emberCommon::RenderQueue::opaque, directoryPath / "particle3d.vert.spv", directoryPath / "particle3d.frag.spv");

		// Create scene:
		Scene* pScene = InitScene();
		emberApplication::Application::SetScene(pScene);

		// Run app:
		emberApplication::Application::Run();

		// App shutdown:
		delete pScene;
		emberApplication::Application::Clear();
	}
	emberTaskSystem::profiler::Session::Get().End();

	// Runtime analysis:
	session.PrintSessionStats();

	return 0;
}