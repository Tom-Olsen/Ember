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



void Fluid2dCpuScene()
{
	{// Camera:
		Entity& entity = *World::CreateEntity("mainCamera").Get();
		Float3 pos = Float3(0.0f, 0.0f, 17.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(200.0f);

		World::SetActiveCamera(pCamera);
	}
	{ // Sph Fluid 2d Cpu:
		Entity& entity = *World::CreateEntity("sphFluid2dCpu").Get();
		SphFluid2dCpu* pSphFluid2dCpu = entity.AddComponent<SphFluid2dCpu>();
	}
}
void Fluid2dGpuScene()
{
	{// Camera:
		Entity& entity = *World::CreateEntity("mainCamera").Get();
		Float3 pos = Float3(0.0f, 0.0f, 17.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);

		World::SetActiveCamera(pCamera);
	}
	{ // Sph Fluid 2d Gpu:
		Entity& entity = *World::CreateEntity("sphFluid2dGpu").Get();
		SphFluid2dGpu* pSphFluid2dGpu = entity.AddComponent<SphFluid2dGpu>();
	}
}
void Fluid3dScene()
{
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
		Entity& entity = *World::CreateEntity("Camera").Get();
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(17.3513f, -27.4224f, 0.9268f));
		pTransform->SetRotationEuler(EulerDegrees(54.9628f, 80.8496f, 82.5193f));

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);
		CameraController* cameraController = entity.AddComponent<CameraController>();

		World::SetActiveCamera(pCamera);
	}
	{// Directional Light:
		Entity& entity = *World::CreateEntity("DirectionalLight").Get();
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
		Entity& entity = *World::CreateEntity("Skybox").Get();
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
		Entity& entity = *World::CreateEntity("Floor").Get();
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(0.0f, 0.0f, -9.9f));
		pTransform->SetScale(60.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(floorMaterial);
	}
	{// Cube:
		Entity& entity = *World::CreateEntity("Cube").Get();
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
		Entity& entity = *World::CreateEntity("Susanne").Get();
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(-20.2f, 0.0f, -5.0f));
		pTransform->SetScale(4.0f);
		pTransform->SetRotationEuler(EulerDegrees(0.0f, 0.0f, 30.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("monkey"));
		pMeshRenderer->SetMaterial(susanneMaterial);
	}
	{ // Sph Fluid 3d Gpu:
		Entity& entity = *World::CreateEntity("sphFluid3dGpu").Get();
		SphFluid3dGpu* pSphFluid3dGpu = entity.AddComponent<SphFluid3dGpu>(skyboxTexture, RotatedBounds(Float3::zero, Float3(16.0f, 12.0f, 10.0f)));
	}
}
void Fluid3dSceneBig()
{
	// Materials:
	DeferredMaterial pbrMaterial = MaterialManager::TryGetDeferredMaterial("pbrDeferredGeometryMaterial");
	Texture skyboxTexture = TextureManager::GetTexture("skybox0");
	ForwardMaterial skyboxMaterial = MaterialManager::TryGetForwardMaterial("skyboxMaterial");
	skyboxMaterial.SetTexture("colorMap", skyboxTexture);
	DeferredMaterial floorMaterial = MaterialManager::TryGetDeferredMaterial("checkerBoard");

	{// Camera:
		Entity& entity = *World::CreateEntity("Camera").Get();
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(66.5733f, -76.2722f, 35.7424f));
		pTransform->SetRotationEuler(EulerDegrees(45.6632f, 61.3948f, 68.7260f));

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(200.0f);
		CameraController* cameraController = entity.AddComponent<CameraController>();

		World::SetActiveCamera(pCamera);
	}
	{// Directional Light:
		Entity& entity = *World::CreateEntity("DirectionalLight").Get();
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
		pDirectionalLight->SetShadowCascadeCount(3);
		pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
	}
	{// Skybox:
		Entity& entity = *World::CreateEntity("Skybox").Get();
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
		Entity& entity = *World::CreateEntity("Floor").Get();
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(0.0f, 0.0f, 0.0f));
		pTransform->SetScale(Float3(140.0f, 100.0f, 1.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(floorMaterial);
	}
	{ // Sph Fluid 3d Gpu:
		Entity& entity = *World::CreateEntity("sphFluid3dGpu").Get();
		SphFluid3dGpu* pSphFluid3dGpu = entity.AddComponent<SphFluid3dGpu>(skyboxTexture, RotatedBounds(Float3(0.0f, 0.0f, 20.0f), Float3(60.0f, 40.0f, 20.0f)));
		pSphFluid3dGpu->SetParticleCount(1000000);
		pSphFluid3dGpu->SetInitialDistributionRadius(19.0f);
		pSphFluid3dGpu->SetInitialDistributionPosition(Float3(-30.0f, 0.0f, 19.0f));
		pSphFluid3dGpu->SetRenderMode(SphFluid3dGpu::RenderMode::particles);
		pSphFluid3dGpu->SetPhysicsTimeScale(1.4f);
	}
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
		appCreateInfo.maxDirectionalLights = 4; // one directional light with four shadow cascades.
		appCreateInfo.maxPositionalLights = 6;  // single point light or 6 spot lights.
		appCreateInfo.shadowMapResolution = 1024;
		emberApplication::Application::Init(appCreateInfo);

		// Load project shader assets:
		const std::filesystem::path shadersDirectory = std::filesystem::path(PROJECT_SHADERS_DIR).make_preferred();
		MaterialManager::LoadMaterialAssets(shadersDirectory / "materialAssets");
		ComputeShaderManager::LoadComputeShaderAssets(shadersDirectory / "computeShaderAssets");

		// Create scene:
		//Fluid2dCpuScene();
		//Fluid2dGpuScene();
		//Fluid3dScene();
		Fluid3dSceneBig();

		// Run app:
		emberApplication::Application::Run();

		// App shutdown:
		emberApplication::Application::Clear();
	}
	emberTaskSystem::profiler::Session::Get().End();

	// Runtime analysis:
	session.PrintSessionStats();

	return 0;
}