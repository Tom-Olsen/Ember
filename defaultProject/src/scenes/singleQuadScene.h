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



inline Scene* SingleQuadScene()
{
	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, -2.0f, 1.5f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));

		Camera* pCamera = pGameObject->AddComponent<Camera>();
		pCamera->SetFarClip(1000.0f);
		PostRenderEffects* pPostRenderEffects = pGameObject->AddComponent<PostRenderEffects>();
		CameraController* cameraController = pGameObject->AddComponent<CameraController>();

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// SpotLight2:
		GameObject* pGameObject = new GameObject("light2");
		Float3 pos = Float3(0.0f, 0.0f, 5.0f);
		pGameObject->GetTransform()->SetPosition(pos);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 90, 0));
		//pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		SpotLight* pSpotLight = pGameObject->AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::white);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(false);
		pSpotLight->SetShadowType(emberCommon::ShadowType::soft);

		pScene->AddGameObject(pGameObject);
	}
	{// Quad:
		GameObject* pGameObject = new GameObject("quad");
		pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("example"));
		//pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("wood1_roughness"));
		//pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("wood1_normal"));
		//pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(1, 1, 0, 0));

		SpinLocal* pSpinLocal = new SpinLocal(45.0f);
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		//DrawMeshData* pDrawMeshData = pGameObject->AddComponent<DrawMeshData>();

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}