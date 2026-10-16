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
	DeferredMaterial lightSourceMaterial0 = pbrMaterial.CloneWithDefaultBindings("lightSourceMaterial0");
	lightSourceMaterial0.SetValue("SurfaceProperties", "surface_isLit", false);
	lightSourceMaterial0.SetValue("SurfaceProperties", "surface_diffuseColor", Float4::white);
	DeferredMaterial lightSourceMaterial1 = pbrMaterial.CloneWithDefaultBindings("lightSourceMaterial1");
	lightSourceMaterial1.SetValue("SurfaceProperties", "surface_isLit", false);
	lightSourceMaterial1.SetValue("SurfaceProperties", "surface_diffuseColor", Float4::red);
	DeferredMaterial lightSourceMaterial2 = pbrMaterial.CloneWithDefaultBindings("lightSourceMaterial2");
	lightSourceMaterial2.SetValue("SurfaceProperties", "surface_isLit", false);
	lightSourceMaterial2.SetValue("SurfaceProperties", "surface_diffuseColor", Float4::green);
	DeferredMaterial lightSourceMaterial3 = pbrMaterial.CloneWithDefaultBindings("lightSourceMaterial3");
	lightSourceMaterial3.SetValue("SurfaceProperties", "surface_isLit", false);
	lightSourceMaterial3.SetValue("SurfaceProperties", "surface_diffuseColor", Float4::blue);
	Texture skyboxTexture = TextureManager::GetTexture("skybox0");
	ForwardMaterial skyboxMaterial = MaterialManager::TryGetForwardMaterial("skyboxMaterial");
	skyboxMaterial.SetTexture("colorMap", skyboxTexture);
	DeferredMaterial floorMaterial = pbrMaterial.CloneWithDefaultBindings("floorMaterial");
	floorMaterial.SetTexture("colorMap", TextureManager::GetTexture("ground0_color"));
	floorMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("ground0_roughness"));
	floorMaterial.SetTexture("normalMap", TextureManager::GetTexture("ground0_normal"));
	floorMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(10, 10, 0, 0));
	floorMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	floorMaterial.SetValue("SurfaceProperties", "surface_metallicity", 0.5f);
	DeferredMaterial wallMaterial = pbrMaterial.CloneWithDefaultBindings("wallMaterial");
	wallMaterial.SetTexture("colorMap", TextureManager::GetTexture("bricks0_color"));
	wallMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("bricks0_roughness"));
	wallMaterial.SetTexture("normalMap", TextureManager::GetTexture("bricks0_normal"));
	wallMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	wallMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(10, 2, 0, 0));
	DeferredMaterial monkeyMaterial = pbrMaterial.CloneWithDefaultBindings("monkeyMaterial");
	monkeyMaterial.SetTexture("colorMap", TextureManager::GetTexture("metal0_color"));
	monkeyMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("metal0_roughness"));
	monkeyMaterial.SetTexture("normalMap", TextureManager::GetTexture("metal0_normal"));
	monkeyMaterial.SetValue("SurfaceProperties", "surface_diffuseColor", Float4(1.0f, 0.25f, 0.25f, 1.0f));
	monkeyMaterial.SetValue("SurfaceProperties", "surface_roughness", 0.5f);
	monkeyMaterial.SetValue("SurfaceProperties", "surface_metallicity", 0.5f);
	monkeyMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(5, 5, 0, 0));
	DeferredMaterial meshTesterMaterial = pbrMaterial.CloneWithDefaultBindings("meshTesterMaterial");
	meshTesterMaterial.SetTexture("colorMap", TextureManager::GetTexture("bricks1_color"));
	meshTesterMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("bricks1_roughness"));
	meshTesterMaterial.SetTexture("normalMap", TextureManager::GetTexture("bricks1_normal"));
	meshTesterMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);
	meshTesterMaterial.SetValue("SurfaceProperties", "surface_scaleOffset", Float4(0.5, 0.5, 0, 0));
	meshTesterMaterial.SetValue("SurfaceProperties", "surface_metallicity", 0.5f);
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
	transparentMaterial.SetValue("SurfaceProperties", "surface_diffuseColor", Float4(1.0f, 1.0f, 1.0f, 0.125f));
	DeferredMaterial mirrorMaterial = pbrMaterial.CloneWithDefaultBindings("mirrorMaterial");
	mirrorMaterial.SetValue("SurfaceProperties", "surface_roughness", 0.0f);
	mirrorMaterial.SetValue("SurfaceProperties", "surface_metallicity", 1.0f);

	{// Camera:
		Entity entity = Entity::Create("MainCamera");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(5.7867f, -6.1613f, 5.1195f));
		pTransform->SetRotationEuler(EulerDegrees(45.5620f, 54.4469f, 62.9650f));

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(1000.0f);

		PostRenderEffects* pPostRenderEffects = entity.AddComponent<PostRenderEffects>();
		ScreenSpaceReflectionDispatcher* pScreenSpaceReflectionDispatcher = entity.AddComponent<ScreenSpaceReflectionDispatcher>(skyboxTexture);
		CameraController* cameraController = entity.AddComponent<CameraController>();

		pScene->SetActiveCamera(pCamera);
	}
	{// Directional Light:
		Entity entity = Entity::Create("DirectionalLight");
		Float3 pos = Float3(0.0f, 3.0f, 0.0f);
		Float3 direction = Float3(-0.4f, 0.4f, -1.0f).Normalize();
		Float3x3 matrix = Float3x3::RotateFromTo(Float3::down, direction);
	
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);
	
		DirectionalLight* pDirectionalLight = entity.AddComponent<DirectionalLight>();
		pDirectionalLight->SetIntensity(0.2f);
		pDirectionalLight->SetColor(Float3::white);
		pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
		pDirectionalLight->SetShadowCascadeCount(2);
		pDirectionalLight->SetDistributionFactor(0.5f);
		pDirectionalLight->SetDrawFrustum(false);
	}
	{// PointLight:
		Entity entity = Entity::Create("PointLight");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(1.1f, -1.5f, 0.14f));
		pTransform->SetRotationEuler(EulerDegrees(3.0f, 6.0f, 9.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshRenderer->SetMaterial(lightSourceMaterial0);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		PointLight* pPointLight = entity.AddComponent<PointLight>();
		pPointLight->SetIntensity(20.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.2f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(showLightFrustums);
		pPointLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// SpotLight0:
		Entity entity = Entity::Create("SpotLight0");
		Float3 pos = Float3(-4.0f, 4.0f, 5.5f);
		Float3 direction = (Float3(0.0f, 8.3f, 3.0f) - pos).Normalize();
		Float3x3 matrix = Float3x3::RotateFromTo(Float3::down, direction);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("spotLight"));
		pMeshRenderer->SetMaterial(lightSourceMaterial1);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::red);
		pSpotLight->SetIntensity(500.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(Degrees(35.0f));
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// SpotLight1:
		Entity entity = Entity::Create("SpotLight1");
		Float3 pos = Float3(0.0f, 4.0f, 5.5f);
		Float3 direction = (Float3(0.0f, 8.3f, 3.0f) - pos).Normalize();
		Float3x3 matrix = Float3x3::RotateFromTo(Float3::down, direction);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("spotLight"));
		pMeshRenderer->SetMaterial(lightSourceMaterial2);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::green);
		pSpotLight->SetIntensity(500.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(Degrees(35.0f));
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// SpotLight2:
		Entity entity = Entity::Create("SpotLight2");
		Float3 pos = Float3(4.0f, 4.0f, 5.5f);
		Float3 direction = (Float3(0.0f, 8.3f, 3.0f) - pos).Normalize();
		Float3x3 matrix = Float3x3::RotateFromTo(Float3::down, direction);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("spotLight"));
		pMeshRenderer->SetMaterial(lightSourceMaterial3);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pMeshRenderer->GetCallProperties().SetValue("SurfaceProperties", "surface_diffuseColor", Float3::blue);

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::blue);
		pSpotLight->SetIntensity(500.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(Degrees(35.0f));
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
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
		pTransform->SetPosition(Float3(0.0f, 0.0f, -0.5f));
		pTransform->SetScale(30.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(floorMaterial);
	}
	{ // Wall:
		Entity entity = Entity::Create("Wall");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(0.0f, 10.0f, 2.5f));
		pTransform->SetScale(Float3(30.0f, 6.0f, 1.0f));
		pTransform->SetRotationEuler(EulerDegrees(90.0f, 0.0f, 0.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(wallMaterial);
	}
	{// ThreeLeg:
		Entity entity = Entity::Create("ThreeLeg");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(-1.0f, -0.5f, 0.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("threeLeg"));
		pMeshRenderer->SetMaterial(pbrMaterial);
		pMeshRenderer->SetCastShadows(true);
		pMeshRenderer->SetReceiveShadows(true);
	}
	{// Susanne:
		Entity entity = Entity::Create("Susanne");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(0.0f, 8.3f, 3.0f));
		pTransform->SetScale(0.66f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("monkey"));
		pMeshRenderer->SetMaterial(mirrorMaterial);

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(Degrees(45.0f));
	}
	{// MeshTester:
		Entity entity = Entity::Create("MeshTester");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(2.0f, 0.0f, 0.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMaterial(meshTesterMaterial);

		MeshTester* pMeshTester = entity.AddComponent<MeshTester>(pMeshRenderer);
		pMeshTester->AddMesh(&MeshManager::GetMesh("cube"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("quad"));
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
	{// Zylinder:
		Entity entity = Entity::Create("Zylinder");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(0.5f, 0.0f, 0.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("zylinderSmooth"));
		pMeshRenderer->SetMaterial(zylinder0Material);

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(Degrees(45.0f));
	}
	{// Sphere:
		Entity entity = Entity::Create("Sphere");
		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(Float3(-0.5f, -2.0f, 0.0f));

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshRenderer->SetMaterial(sphere0Material);

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(Degrees(45.0f));
	}
	//{// Transparent quad:
	//	Entity entity = Entity::Create("TransparentQuad");
	//	Transform* pTransform = entity.GetTransform();
	//	pTransform->SetPosition(Float3(0.66f, -1.33f, 1.0f));
	//	pTransform->SetRotationEuler(EulerDegrees(60.0f, 0.0f, 0.0f));
	//
	//	MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
	//	pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
	//	pMeshRenderer->SetMaterial(transparentMaterial);
	//	pMeshRenderer->SetCastShadows(false);
	//	pMeshRenderer->SetReceiveShadows(false);
	//}
	//{// Mirror:
	//	Entity entity = Entity::Create("Mirror");
	//	Transform* pTransform = entity.GetTransform();
	//	pTransform->SetPosition(Float3(-4.0f, 4.0f, 1.0f));
	//	pTransform->SetScale(Float3(1.0f, 2.0f, 2.0f));
	//	pTransform->SetRotationEuler(EulerDegrees(30.0f, 90.0f, 00.0f));
	//
	//	MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
	//	pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
	//	pMeshRenderer->SetMaterial(mirrorMaterial);
	//	pMeshRenderer->SetCastShadows(false);
	//	pMeshRenderer->SetReceiveShadows(false);
	//}
	return pScene;
}