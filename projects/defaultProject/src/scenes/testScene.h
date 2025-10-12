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



inline Scene* TestScene()
{
	// This scene replicated the binding missmatch error.
	// The two test materials are spcifically constructed to replicate this error.
	// Activate validation layers in VulkanRenderer/vulkanMacros.h to see the error.

	Material& materialA = MaterialManager::GetMaterial("testAMaterial");
	Material& materialB = MaterialManager::GetMaterial("testBMaterial");

	//materialA->PrintBindings();
	//materialA->PrintUniformBuffers();
	//materialB->PrintBindings();
	//materialB->PrintUniformBuffers();

	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, 3.0f, 5.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::back, -pos, Float3::up, Float3::up));

		Camera* pCamera = pGameObject->AddComponent<Camera>();
		PostRenderEffects* pPostRenderEffects = pGameObject->AddComponent<PostRenderEffects>();
		CameraController* cameraController = pGameObject->AddComponent<CameraController>();

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// PointLight:
		GameObject* pGameObject = new GameObject("pointLight");
		Float3 pos = Float3(1.0f, 2.0f, 0.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::back, -pos, Float3::up, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("simpleUnlitMaterial"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		PointLight* pPointLight = pGameObject->AddComponent<PointLight>();
		pPointLight->SetIntensity(10.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.5f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(false);

		SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 45, 0));
		pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		pScene->AddGameObject(pGameObject);
	}
	{ // Floor:
		GameObject* pGameObject = new GameObject("floor");
		pGameObject->GetTransform()->SetPosition(0.0f, -0.5f, 0.0f);
		pGameObject->GetTransform()->SetScale(30.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(-90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(materialA);
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("grass"));

		pScene->AddGameObject(pGameObject);
	}
	{// Cube:
		GameObject* pGameObject = new GameObject("unitCube");
		pGameObject->GetTransform()->SetPosition(-2.0f, 0.0f, 1.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(materialB);
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("brick"));
		pMeshRenderer->GetShaderProperties().SetTexture("cubeMap", TextureManager::GetTexture("skybox0"));

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}