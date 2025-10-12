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



inline Scene* ShadowCascadeScene()
{
	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, -40.0f, 20.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		Camera* pCamera = pGameObject->AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);
		PostRenderEffects* pPostRenderEffects = pGameObject->AddComponent<PostRenderEffects>();
		CameraController* cameraController = pGameObject->AddComponent<CameraController>();

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// Skybox:
		GameObject* pGameObject = new GameObject("skybox");

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("skybox0"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		pScene->AddGameObject(pGameObject);
	}
	//{// ThreeLeg:
	//	GameObject* pGameObject = new GameObject("threeLeg");
	//	pGameObject->GetTransform()->SetPosition(-5.0f, 5.0f, 0.0f);
	//	pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
	//
	//	MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	//	pMeshRenderer->SetMesh(MeshManager::GetMesh("threeLeg"));
	//	pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
	//	pMeshRenderer->SetCastShadows(true);
	//	pMeshRenderer->SetReceiveShadows(true);
	//
	//	pScene->AddGameObject(pGameObject);
	//}
	Camera* pCamera = new Camera();
	{// TestCamera:
		GameObject* pGameObject = new GameObject("testCamera");
		Float3 pos = Float3(26.0f, 0.0f, 0.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::rot90z * Float3x3::rot90x);

		pCamera->SetNearClip(0.5f);
		pCamera->SetFarClip(40.0f);
		pCamera->SetFov(30.0f * math::deg2rad);
		pCamera->SetDrawFrustum(true);
		pGameObject->AddComponent<Camera>(pCamera);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("camera"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		CameraController* cameraController = pGameObject->AddComponent<CameraController>();
		cameraController->SetIsActive(false);

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
		pDirectionalLight->SetActiveCamera(pCamera);
		//pDirectionalLight->SetDrawFrustum(true);
		pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
		pDirectionalLight->SetDistributionFactor(0.5f);
		//pDirectionalLight->SetShadowCascadeCount(1);

		pScene->AddGameObject(pGameObject);
	}
	{ // Floor:
		GameObject* pGameObject = new GameObject("floor");
		pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, -0.5f);
		pGameObject->GetTransform()->SetScale(100);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));

		pScene->AddGameObject(pGameObject);
	}
	{// Cube Array:
		int N = 25;
		for (int i = 0; i < N; i++)
		{
			GameObject* pGameObject = new GameObject("cube" + std::to_string(i));
			Float3 pos(-2 * i + 25, 0.0f, 0.0f);
			pGameObject->GetTransform()->SetPosition(pos);
			pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

			MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
			pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
			pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
			pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks0_color"));
			pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks0_roughness"));
			pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks0_normal"));
			pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);

			pScene->AddGameObject(pGameObject);
		}
	}
	return pScene;
}