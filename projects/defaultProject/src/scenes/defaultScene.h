#pragma once
#include "emberEngine.h"
// Components:
#include "drawMeshData.h"
#include "meshTester.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "testInstancedRendering.h"
using namespace emberCore;
using namespace emberEcs;
using namespace emberEngine;



inline Scene* DefaultScene()
{
	bool directionalLightsActive = false;
	bool showLightFrustums = false;

	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	// Materials:
	DeferredMaterial pbrMaterial = MaterialManager::TryGetDeferredMaterial("pbrDeferredGeometryMaterial");
	DeferredMaterial lightSourceMaterial = pbrMaterial.CloneWithDefaultBindings("pointLightMaterial");
	lightSourceMaterial.SetValue("SurfaceProperties", "surface_isLit", false);
	lightSourceMaterial.SetValue("SurfaceProperties", "surface_diffuseColor", Float4::white);
	ForwardMaterial skyboxMaterial = MaterialManager::TryGetForwardMaterial("skyboxMaterial");
	skyboxMaterial.SetTexture("colorMap", TextureManager::GetTexture("skybox0"));
	DeferredMaterial floorMaterial = pbrMaterial.CloneWithDefaultBindings("floorMaterial");
	floorMaterial.SetTexture("colorMap", TextureManager::GetTexture("ground0_color"));
	floorMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("ground0_roughness"));
	floorMaterial.SetTexture("normalMap", TextureManager::GetTexture("ground0_normal"));
	floorMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(10, 10, 0, 0));
	floorMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	DeferredMaterial wallMaterial = pbrMaterial.CloneWithDefaultBindings("wallMaterial");
	wallMaterial.SetTexture("colorMap", TextureManager::GetTexture("bricks0_color"));
	wallMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("bricks0_roughness"));
	wallMaterial.SetTexture("normalMap", TextureManager::GetTexture("bricks0_normal"));
	wallMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	wallMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(10, 2, 0, 0));
	DeferredMaterial cube0Material = pbrMaterial.CloneWithDefaultBindings("cube0Material");
	cube0Material.SetTexture("colorMap", TextureManager::GetTexture("wood1_color"));
	cube0Material.SetTexture("roughnessMap", TextureManager::GetTexture("wood1_roughness"));
	cube0Material.SetTexture("normalMap", TextureManager::GetTexture("wood1_normal"));
	cube0Material.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	cube0Material.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(0.5, 0.5, 0, 0));
	DeferredMaterial meshTesterMaterial = pbrMaterial.CloneWithDefaultBindings("meshTesterMaterial");
	meshTesterMaterial.SetTexture("colorMap", TextureManager::GetTexture("bricks1_color"));
	meshTesterMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("bricks1_roughness"));
	meshTesterMaterial.SetTexture("normalMap", TextureManager::GetTexture("bricks1_normal"));
	meshTesterMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	meshTesterMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(0.5, 0.5, 0, 0));
	DeferredMaterial zylinder0Material = pbrMaterial.CloneWithDefaultBindings("zylinder0Material");
	zylinder0Material.SetTexture("colorMap", TextureManager::GetTexture("bricks2_color"));
	zylinder0Material.SetTexture("roughnessMap", TextureManager::GetTexture("bricks2_roughness"));
	zylinder0Material.SetTexture("normalMap", TextureManager::GetTexture("bricks2_normal"));
	zylinder0Material.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	DeferredMaterial sphere0Material = pbrMaterial.CloneWithDefaultBindings("sphere0Material");
	sphere0Material.SetTexture("colorMap", TextureManager::GetTexture("wood0_color"));
	sphere0Material.SetTexture("roughnessMap", TextureManager::GetTexture("wood0_roughness"));
	sphere0Material.SetTexture("normalMap", TextureManager::GetTexture("wood0_normal"));
	sphere0Material.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	sphere0Material.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(0.5, 0.5, 0, 0));
	ForwardMaterial transparentMaterial = MaterialManager::TryGetForwardMaterial("pbrForwardMaterial").CloneWithDefaultBindings("transparentMaterial");
	transparentMaterial.SetRenderMode(emberCommon::ForwardRenderMode::transparent);
	transparentMaterial.SetValue("SurfaceProperties", "surface_isLit", false);
	transparentMaterial.SetValue("SurfaceProperties", "surface_diffuseColor", Float4(1.0, 0.0, 0, 0.25f));

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(0.0f, -8.0f, 3.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(1000.0f);

		PostRenderEffects* pPostRenderEffects = entity.AddComponent<PostRenderEffects>();
		CameraController* cameraController = entity.AddComponent<CameraController>();

		pScene->SetActiveCamera(pCamera);
	}
	{// Directional Light:
		Entity entity = Entity::Create("directionalLight");
		Float3 pos = Float3(0.0f, 3.0f, 0.0f);
		Float3 direction = Float3(-0.4f, 0.4f, -1.0f).Normalize();
		Float3x3 matrix = Float3x3::RotateFromTo(Float3::down, direction);
	
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);
	
		DirectionalLight* pDirectionalLight = entity.AddComponent<DirectionalLight>();
		pDirectionalLight->SetIntensity(1.0f);
		pDirectionalLight->SetColor(Float3::white);
		pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
		pDirectionalLight->SetShadowCascadeCount(2);
		pDirectionalLight->SetDistributionFactor(0.5f);
		pDirectionalLight->SetDrawFrustum(false);
	}
	{// PointLight:
		Entity entity = Entity::Create("pointLight");
		Float3 pos = Float3(1.0f, 0.0f, 1.5f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::back, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cube"));
		pMeshRenderer->SetMaterial(lightSourceMaterial);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		PointLight* pPointLight = entity.AddComponent<PointLight>();
		pPointLight->SetIntensity(5.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.5f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(showLightFrustums);
		pPointLight->SetShadowType(emberCommon::ShadowType::hard);

		SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, 0, 45));
	}
	{// SpotLight0:
		Entity entity = Entity::Create("light0");
		Float3 pos = Float3(7.0f, 3.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(lightSourceMaterial);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, 45, 0));

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::red);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// SpotLight1:
		Entity entity = Entity::Create("light1");
		Float3 pos = Float3(-7.0f, 3.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(lightSourceMaterial);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, -60, 0));

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::green);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// SpotLight2:
		Entity entity = Entity::Create("light2");
		Float3 pos = Float3(0.0f, -7.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(lightSourceMaterial);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, 90, 0));

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::blue);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// Skybox:
		Entity entity = Entity::Create("skybox");

		Transform* pTransform = entity.GetTransform();
		pTransform->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cube"));
		pMeshRenderer->SetMaterial(skyboxMaterial);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pMeshRenderer->SetRaycastEnabled(false);
	}
	{ // Floor:
		Entity entity = Entity::Create("floor");
		Float3 pos = Float3(0.0f, 0.0f, -0.5f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetScale(30.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(floorMaterial);
	}
	{ // Wall:
		Entity entity = Entity::Create("wall");
		Float3 pos = Float3(0.0f, 10.0f, 2.5f);
		Float3 scale = Float3(30.0f, 6.0f, 1.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetScale(scale);
		pTransform->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(wallMaterial);
	}
	{// ThreeLeg:
		Entity entity = Entity::Create("threeLeg");
		Float3 pos = Float3(-3.0f, -2.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("threeLeg"));
		pMeshRenderer->SetMaterial(pbrMaterial);
		pMeshRenderer->SetCastShadows(true);
		pMeshRenderer->SetReceiveShadows(true);
	}
	{// Cube0:
		Entity entity = Entity::Create("cube0");
		Float3 pos = Float3(-2.0f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cube"));
		pMeshRenderer->SetMaterial(cube0Material);

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(45.0f);

		//DrawMeshData* pDrawMeshData = entity.AddComponent<DrawMeshData>();
	}
	{// MeshTester:
		Entity entity = Entity::Create("meshTester");
		Float3 pos = Float3(2.0f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMaterial(meshTesterMaterial);

		MeshTester* pMeshTester = entity.AddComponent<MeshTester>(pMeshRenderer);
		pMeshTester->AddMesh(&MeshManager::GetMesh("quad"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("cube"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("halfCube"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("cubeSphere"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("halfCubeSphere"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("capsule"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("disk"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("arcFlatUv"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("arcCurvedUv"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("coneSmooth"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("coneFlat"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("zylinderSmooth"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("zylinderFlat"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("arrowSmooth"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("arrowFlat"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("threeLeg"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("fourLeg"));
	}
	{// Zylinder 0:
		Entity entity = Entity::Create("zylinder0");
		Float3 pos = Float3(0.5f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("zylinderSmooth"));
		pMeshRenderer->SetMaterial(zylinder0Material);

		//DrawMeshData* drawMeshData = entity.AddComponent<DrawMeshData>();

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(45.0f);
	}
	{// Sphere 0:
		Entity entity = Entity::Create("sphere1");
		Float3 pos = Float3(-0.5f, 1.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshRenderer->SetMaterial(sphere0Material);

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(45.0f);

		//DrawMeshData* pDrawMeshData = entity.AddComponent<DrawMeshData>();
	}
	{// Transparent quad:
		Entity entity = Entity::Create("transparentQuad");
		Float3 pos = Float3(0.66f, -1.33f, 1.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationEulerDegrees(60.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(transparentMaterial);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
	}
	return pScene;
}