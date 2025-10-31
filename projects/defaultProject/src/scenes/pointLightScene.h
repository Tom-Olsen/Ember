#pragma once
#include "emberEngine.h"
// Components:
#include "drawMeshData.h"
#include "meshTester.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "testInstancedRendering.h"
using namespace emberEngine;



inline Scene* PointLightScene()
{
	bool directionalLightsActive = false;

	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(9.0f, -9.0f, 9.0f);
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
	{// PointLight:
		Entity entity = Entity::Create("pointLight");
		Float3 pos = Float3(0.0f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("simpleUnlitMaterial"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		PointLight* pPointLight = entity.AddComponent<PointLight>();
		pPointLight->SetIntensity(10.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.1f);
		pPointLight->SetFarClip(8.0f);
		pPointLight->SetShadowType(emberCommon::ShadowType::hard);
		//pPointLight->SetDrawFrustum(true);
	}
	{// Room:
		Entity entity = Entity::Create("room");
		Float3 pos = Float3(0.0f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetScale(10.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCubeInverse"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks1_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks1_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks1_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(1.0, 1.0, 0, 0));
	}
	{// Frame:
		Entity entity = Entity::Create("frame");
		Float3 pos = Float3(0.0f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("frame"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("metal0_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("metal0_normal"));
		pMeshRenderer->GetShaderProperties().SetTexture("metallicMap", TextureManager::GetTexture("metal0_metallic"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("metal0_roughness"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "metallicity", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(0.25, 1, 0, 0));

		entity.AddComponent<SpinLocal>(45.0f);
	}
	return pScene;
}