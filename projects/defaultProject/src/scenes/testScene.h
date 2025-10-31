#pragma once
#include "emberEngine.h"
// Components:
#include "drawMeshData.h"
#include "meshTester.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "testInstancedRendering.h"
using namespace emberEngine;



inline Scene* TestScene()
{
	// This scene replicates the binding missmatch error.
	// The two test materials are spcifically constructed to replicate this error.
	// Activate validation layers in VulkanRenderer/vulkanMacros.h to see the error.

	Material& materialA = MaterialManager::GetMaterial("testAMaterial");
	Material& materialB = MaterialManager::GetMaterial("testBMaterial");

	//materialA->PrintBindings();
	//materialA->PrintUniformBuffers();
	//materialB->PrintBindings();
	//materialB->PrintUniformBuffers();

	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(0.0f, 3.0f, 5.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::back, -pos, Float3::up, Float3::up));

		Camera* pCamera = entity.AddComponent<Camera>();
		PostRenderEffects* pPostRenderEffects = entity.AddComponent<PostRenderEffects>();
		CameraController* cameraController = entity.AddComponent<CameraController>();

		pScene->SetActiveCamera(pCamera);
	}
	{// PointLight:
		Entity entity = Entity::Create("pointLight");
		Float3 pos = Float3(1.0f, 2.0f, 0.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::back, -pos, Float3::up, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("simpleUnlitMaterial"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		PointLight* pPointLight = entity.AddComponent<PointLight>();
		pPointLight->SetIntensity(10.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.5f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(false);

		SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, 45, 0));
	}
	{ // Floor:
		Entity entity = Entity::Create("floor");
		Float3 pos = Float3(0.0f, -0.5f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationEulerDegrees(-90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(materialA);
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("grass"));
	}
	{// Cube:
		Entity entity = Entity::Create("unitCube");
		Float3 pos = Float3(-2.0f, 0.0f, 1.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(materialB);
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("brick"));
		pMeshRenderer->GetShaderProperties().SetTexture("cubeMap", TextureManager::GetTexture("skybox0"));
	}
	return pScene;
}