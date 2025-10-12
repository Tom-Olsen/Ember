#pragma once
#include "emberEngine.h"
// Components:
#include "camera.h"
#include "cameraController.h"
#include "directionalLight.h"
#include "drawMeshData.h"
#include "instancedMeshRenderer.h"
#include "meshRenderer.h"
#include "meshTester.h"
#include "pointLight.h"
#include "postRenderEffects.h"
#include "profiler.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "spotLight.h"
#include "testInstancedRendering.h"
#include "transform.h"
using namespace emberEngine;



inline Scene* DefaultScene()
{
	bool directionalLightsActive = false;
	bool showLightFrustums = false;

	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, -8.0f, 3.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));

		Camera* pCamera = pGameObject->AddComponent<Camera>();
		pCamera->SetFarClip(1000.0f);
		PostRenderEffects* pPostRenderEffects = pGameObject->AddComponent<PostRenderEffects>();
		CameraController* cameraController = pGameObject->AddComponent<CameraController>();

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	//{// Directional Light:
	//	GameObject* pGameObject = new GameObject("directionalLight");
	//	Float3 direction = Float3(-0.4f, 0.4f, -1.0f).Normalize();
	//	Float3x3 rotation = Float3x3::RotateFromTo(Float3::down, direction);
	//	pGameObject->GetTransform()->SetPosition(Float3(0.0f, 3.0f, 0.0f));
	//	pGameObject->GetTransform()->SetRotationMatrix(rotation);
	//
	//	DirectionalLight* pDirectionalLight = new DirectionalLight();
	//	pDirectionalLight->SetIntensity(1.0f);
	//	pDirectionalLight->SetColor(Float3::white);
	//	pDirectionalLight->SetDrawFrustum(true);
	//	pDirectionalLight->SetShadowCascadeCount(3);
	//	pDirectionalLight->SetShadowCascadeSplit(0, 0.0f);
	//	pDirectionalLight->SetShadowCascadeSplit(1, 0.2f);
	//	pDirectionalLight->SetShadowCascadeSplit(2, 0.5f);
	//	pDirectionalLight->SetShadowCascadeSplit(3, 1.0f);
	//	pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
	//	pGameObject->AddComponent<DirectionalLight>(pDirectionalLight);
	//
	//	pScene->AddGameObject(pGameObject);
	//}
	{// PointLight:
		GameObject* pGameObject = new GameObject("pointLight");
		Float3 pos = Float3(1.0f, 0.0f, 1.5f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::back, -pos, Float3::forward, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("simpleUnlitMaterial"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		PointLight* pPointLight = pGameObject->AddComponent<PointLight>();
		pPointLight->SetIntensity(5.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.5f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(showLightFrustums);
		pPointLight->SetShadowType(emberCommon::ShadowType::hard);

		SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 0, 45));
		pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		pScene->AddGameObject(pGameObject);
	}
	{// SpotLight0:
		GameObject* pGameObject = new GameObject("light0");
		Float3 pos = Float3(7.0f, 3.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 45, 0));
		//pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		SpotLight* pSpotLight = pGameObject->AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::red);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);

		pScene->AddGameObject(pGameObject);
	}
	{// SpotLight1:
		GameObject* pGameObject = new GameObject("light1");
		Float3 pos = Float3(-7.0f, 3.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, -60, 0));
		//pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		SpotLight* pSpotLight = pGameObject->AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::green);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);

		pScene->AddGameObject(pGameObject);
	}
	{// SpotLight2:
		GameObject* pGameObject = new GameObject("light2");
		Float3 pos = Float3(0.0f, -7.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 90, 0));
		//pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		SpotLight* pSpotLight = pGameObject->AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::blue);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);

		pScene->AddGameObject(pGameObject);
	}
	{// Skybox:
		GameObject* pGameObject = new GameObject("skybox");
		pGameObject->GetTransform()->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("skybox0"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		pScene->AddGameObject(pGameObject);
	}
	{ // Floor:
		GameObject* pGameObject = new GameObject("floor");
		pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, -0.5f);
		pGameObject->GetTransform()->SetScale(30.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("ground0_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("ground0_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("ground0_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(10, 10, 0, 0));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);

		pScene->AddGameObject(pGameObject);
	}
	{ // Wall:
		GameObject* pGameObject = new GameObject("wall");
		pGameObject->GetTransform()->SetPosition(0.0f, 10.0f, 2.5f);
		pGameObject->GetTransform()->SetScale(Float3(30.0f, 6.0f, 1.0f));
		pGameObject->GetTransform()->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks0_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks0_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks0_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(10, 2, 0, 0));

		pScene->AddGameObject(pGameObject);
	}
	{// ThreeLeg:
		GameObject* pGameObject = new GameObject("threeLeg");
		pGameObject->GetTransform()->SetPosition(-3.0f, -2.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("threeLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLitMaterial"));
		pMeshRenderer->SetCastShadows(true);
		pMeshRenderer->SetReceiveShadows(true);

		pScene->AddGameObject(pGameObject);
	}
	{// Cube0:
		GameObject* pGameObject = new GameObject("cube0");
		pGameObject->GetTransform()->SetPosition(-2.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("wood1_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("wood1_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("wood1_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

		SpinLocal* pSpinLocal = new SpinLocal(45.0f);
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		//DrawMeshData* pDrawMeshData = pGameObject->AddComponent<DrawMeshData>();

		pScene->AddGameObject(pGameObject);
	}
	{// MeshTester:
		GameObject* pGameObject = new GameObject("meshTester");
		pGameObject->GetTransform()->SetPosition(2.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks1_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks1_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks1_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

		MeshTester* pMeshTester = new MeshTester(pMeshRenderer);
		pMeshTester->AddMesh(&MeshManager::GetMesh("unitQuad"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("unitCube"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("halfCube"));
		pMeshTester->AddMesh(&MeshManager::GetMesh("cubeSphere"));
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
		pGameObject->AddComponent<MeshTester>(pMeshTester);

		pScene->AddGameObject(pGameObject);
	}
	{// Zylinder 0:
		GameObject* pGameObject = new GameObject("zylinder0");
		pGameObject->GetTransform()->SetPosition(0.5f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("zylinderSmooth"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks2_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks2_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks2_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);

		//DrawMeshData* drawMeshData = pGameObject->AddComponent<DrawMeshData>();

		SpinLocal* pSpinLocal = new SpinLocal(45.0f);
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		pScene->AddGameObject(pGameObject);
	}
	{// Sphere 0:
		GameObject* pGameObject = new GameObject("sphere1");
		pGameObject->GetTransform()->SetPosition(-0.5f, 1.0f, 0.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("wood0_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("wood0_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("wood0_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

		SpinLocal* pSpinLocal = new SpinLocal(45.0f);
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		//DrawMeshData* pDrawMeshData = pGameObject->AddComponent<DrawMeshData>();

		pScene->AddGameObject(pGameObject);
	}
	//{// Instanced Cube Array:
	//	GameObject* pGameObject = new GameObject("cubeArray");
	//	Float3 pos = Float3::zero;
	//	pGameObject->GetTransform()->SetPosition(pos);
	//	pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
	//	
	//	TestInstancedRendering* testInstancedRendering = pGameObject->AddComponent<TestInstancedRendering>(testInstancedRendering);
	//
	//	InstancedRenderer* pInstancedRenderer = pGameObject->AddComponent<InstancedRenderer>();
	//	pInstancedRenderer->SetInstanceCount(16);
	//	pInstancedRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
	//	pInstancedRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
	//	pInstancedRenderer->SetInstanceBuffer(testInstancedRendering->GetInstanceBuffer());
	//	pInstancedRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("concrete0_color"));
	//	pInstancedRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("concrete0_roughness"));
	//	pInstancedRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("concrete0_normal"));
	//	pInstancedRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
	//	
	//	pScene->AddGameObject(pGameObject);
	//}
	{// Transparent quad:
		GameObject* pGameObject = new GameObject("transparentQuad");
		pGameObject->GetTransform()->SetPosition(0.66f, -1.33f, 1.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(60.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("transparentMaterial"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "diffuseColor", Float4(1.0, 0.0, 0, 0.25f));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}