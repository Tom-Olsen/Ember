#define SDL_MAIN_HANDLED
#include "application.h"
#include "emberEngine.h"
#include "profiler.h"
// Components:
#include "sphFluid2dCpu.h"
#include "sphFluid2dGpu.h"
#include "sphFluid3dGpu.h"
using namespace emberCore;
using namespace emberEcs;
using namespace fluidDynamics;



Scene* Fluid2dCpuScene()
{
	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(0.0f, 0.0f, 17.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(200.0f);

		pScene->SetActiveCamera(pCamera);
	}
	{ // Sph Fluid 2d Cpu:
		Entity entity = Entity::Create("sphFluid2dCpu");
		SphFluid2dCpu* pSphFluid2dCpu = entity.AddComponent<SphFluid2dCpu>();
	}
	return pScene;
}
Scene* Fluid2dGpuScene()
{
	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(0.0f, 0.0f, 17.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);

		pScene->SetActiveCamera(pCamera);
	}
	{ // Sph Fluid 2d Gpu:
		Entity entity = Entity::Create("sphFluid2dGpu");
		SphFluid2dGpu* pSphFluid2dGpu = entity.AddComponent<SphFluid2dGpu>();
	}
	return pScene;
}
Scene* Fluid3dScene()
{
	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	// Materials:
	DeferredMaterial pbrMaterial = MaterialManager::TryGetDeferredMaterial("pbrDeferredGeometryMaterial");
	Texture skyboxTexture = TextureManager::GetTexture("skybox0");
	ForwardMaterial skyboxMaterial = MaterialManager::TryGetForwardMaterial("skyboxMaterial");
	skyboxMaterial.SetTexture("colorMap", skyboxTexture);
	DeferredMaterial floorMaterial = MaterialManager::TryGetDeferredMaterial("checkerBoard");
	DeferredMaterial cubeMaterial = pbrMaterial.CloneWithDefaultBindings("cubeMaterial");
	cubeMaterial.SetTexture("colorMap", TextureManager::GetTexture("bricks1_color"));
	cubeMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("bricks1_roughness"));
	cubeMaterial.SetTexture("normalMap", TextureManager::GetTexture("bricks1_normal"));
	cubeMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	cubeMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(0.25f, 2.5f, 0, 0));
	DeferredMaterial susanneMaterial = pbrMaterial.CloneWithDefaultBindings("wallMaterial");
	susanneMaterial.SetTexture("colorMap", TextureManager::GetTexture("bricks0_color"));
	susanneMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("bricks0_roughness"));
	susanneMaterial.SetTexture("normalMap", TextureManager::GetTexture("bricks0_normal"));
	susanneMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	susanneMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(10, 2, 0, 0));

	{// Camera:
		Entity entity = Entity::Create("Camera");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(17.3513f, -27.4224f, 0.9268f));
		pTransform->SetRotationEuler(EulerDegrees(54.9628f, 80.8496f, 82.5193f));

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);
		CameraController* cameraController = entity.AddComponent<CameraController>();

		pScene->SetActiveCamera(pCamera);
	}
	{// Directional Light:
		Entity entity = Entity::Create("DirectionalLight");
		Float3 pos = 2.0f * Float3(-16.0f, -9.0f, 9.0f);
		Float3 direction = -pos.Normalize();
		Float3x3 matrix = Float3x3::RotateFromTo(Float3::down, direction);
    
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);
    
		DirectionalLight* pDirectionalLight = entity.AddComponent<DirectionalLight>();
		pDirectionalLight->SetIntensity(5.0f);
		pDirectionalLight->SetColor(Float3::white);
		pDirectionalLight->SetDrawFrustum(false);
		pDirectionalLight->SetShadowCascadeCount(1);
		pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
	}
	{// Skybox:
		Entity entity = Entity::Create("Skybox");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetRotationEuler(EulerDegrees(90.0f, 0.0f, 0.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cube"));
		pMeshRenderer->SetMaterial(skyboxMaterial);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pMeshRenderer->SetRaycastEnabled(false);
	}
	{ // Floor:
		Entity entity = Entity::Create("Floor");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(0.0f, 0.0f, -10.0f));
		pTransform->SetScale(60.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(floorMaterial);
	}
	{// Cube:
		Entity entity = Entity::Create("Cube");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(0.3395f, 13.8969f, -0.6801f));
		pTransform->SetRotationEuler(EulerDegrees(0.0f, 25.5000f, 0.0000f));
		pTransform->SetScale(Float3(2.0f, 2.0f, 20.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cube"));
		pMeshRenderer->SetMaterial(cubeMaterial);

		//DrawMeshData* pDrawMeshData = entity.AddComponent<DrawMeshData>();
	}
	{// Susanne:
		Entity entity = Entity::Create("Susanne");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(-20.0f, 0.0f, -5.0f));
		pTransform->SetScale(4.0f);
		pTransform->SetRotationEuler(EulerDegrees(0.0f, 0.0f, 30.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("monkey"));
		pMeshRenderer->SetMaterial(susanneMaterial);
	}
	{ // Sph Fluid 3d Gpu:
		Entity entity = Entity::Create("sphFluid3dGpu");
		SphFluid3dGpu* pSphFluid3dGpu = entity.AddComponent<SphFluid3dGpu>(skyboxTexture);
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
		appCreateInfo.vSyncEnabled = false;
		appCreateInfo.framesInFlight = 2;
		appCreateInfo.msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount04;
		appCreateInfo.windowWidth  = 3840; //2560; //1920;
		appCreateInfo.windowHeight = 2160; //1440; //1080;
		appCreateInfo.renderWidth  = 2560; //1280;
		appCreateInfo.renderHeight = 1440; //720;
		appCreateInfo.maxDirectionalLights = 1; // single directional light.
		appCreateInfo.maxPositionalLights = 6;  // single point light ot 6 spot lights.
		appCreateInfo.shadowMapResolution = 1024;
		emberApplication::Application::Init(appCreateInfo);

		// Load project shader assets:
		const std::filesystem::path shadersDirectory = std::filesystem::path(PROJECT_SHADERS_DIR).make_preferred();
		MaterialManager::LoadMaterialAssets(shadersDirectory / "materialAssets");
		ComputeShaderManager::LoadComputeShaderAssets(shadersDirectory / "computeShaderAssets");

		// Create scene:
		//Scene* pScene = Fluid2dCpuScene();
		//Scene* pScene = Fluid2dGpuScene();
		Scene* pScene = Fluid3dScene();
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