#pragma once
#include "emberEngine.h"
// Components:
#include "drawMeshData.h"
#include "meshTester.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "testInstancedRendering.h"
using namespace emberEngine;



inline Scene* ShadowCascadeScene()
{
	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(0.0f, -40.0f, 20.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetFarClip(100.0f);

		PostRenderEffects* pPostRenderEffects = entity.AddComponent<PostRenderEffects>();
		CameraController* cameraController = entity.AddComponent<CameraController>();

		pScene->SetActiveCamera(pCamera);
	}
	{// Skybox:
		Entity entity = Entity::Create("skybox");

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("skybox0"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
	}
	//{// ThreeLeg:
	//	Entity entity = Entity::Create("threeLeg");
	//	Float3 pos = Float3(-5.0f, 5.0f, 0.0f);
	//
	//	Transform* pTransform = entity.GetTransform();
	//	pTransform->SetPosition(pos);
	//	pTransform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
	//
	//	MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
	//	pMeshRenderer->SetMesh(MeshManager::GetMesh("threeLeg"));
	//	pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
	//	pMeshRenderer->SetCastShadows(true);
	//	pMeshRenderer->SetReceiveShadows(true);
	//}
	Camera* pShadowCascadeCamera;
	{// TestCamera:
		Entity entity = Entity::Create("testCamera");
		Float3 pos = Float3(26.0f, 0.0f, 0.0f);
		Float3x3 matrix = Float3x3::rot90z * Float3x3::rot90x;

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		Camera* pCamera = entity.AddComponent<Camera>();
		pCamera->SetNearClip(0.5f);
		pCamera->SetFarClip(40.0f);
		pCamera->SetFov(30.0f * math::deg2rad);
		pCamera->SetDrawFrustum(true);
		pShadowCascadeCamera = pCamera;

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("camera"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		CameraController* cameraController = entity.AddComponent<CameraController>();
		cameraController->SetIsActive(false);
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
		pDirectionalLight->SetActiveCamera(pShadowCascadeCamera);
		//pDirectionalLight->SetDrawFrustum(true);
		pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
		pDirectionalLight->SetDistributionFactor(0.5f);
		//pDirectionalLight->SetShadowCascadeCount(1);
	}
	{ // Floor:
		Entity entity = Entity::Create("floor");
		Float3 pos = Float3(0.0f, 0.0f, -0.5f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetScale(100);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
	}
	{// Cube Array:
		int N = 25;
		for (int i = 0; i < N; i++)
		{
			Entity entity = Entity::Create("cube" + std::to_string(i));
			Float3 pos(-2 * i + 25, 0.0f, 0.0f);

			Transform* pTransform = entity.GetTransform();
			pTransform->SetPosition(pos);

			MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
			pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
			pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
			pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks0_color"));
			pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks0_roughness"));
			pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks0_normal"));
			pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		}
	}
	return pScene;
}