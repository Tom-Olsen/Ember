#pragma once
#include "emberEngine.h"
// Custom components:
#include "drawMeshData.h"
#include "meshTester.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "testInstancedRendering.h"
using namespace emberEngine;



inline Scene* SingleQuadScene()
{
	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(0.0f, -2.0f, 1.5f);
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
	{// SpotLight2:
		Entity entity = Entity::Create("light2");
		Float3 pos = Float3(0.0f, 0.0f, 5.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, 90, 0));

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::white);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(false);
		pSpotLight->SetShadowType(emberCommon::ShadowType::soft);
	}
	{// Quad:
		Entity entity = Entity::Create("quad");

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("example"));
		//pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("wood1_roughness"));
		//pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("wood1_normal"));
		//pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(1, 1, 0, 0));

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(45.0f);

		//DrawMeshData* pDrawMeshData = entity.AddComponent<DrawMeshData>();
	}
	return pScene;
}