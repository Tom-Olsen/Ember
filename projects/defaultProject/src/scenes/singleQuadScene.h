#pragma once
#include "emberEngine.h"
// Custom components:
#include "drawMeshData.h"
#include "meshTester.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "testInstancedRendering.h"
using namespace emberCore;
using namespace emberEcs;
using namespace emberEngine;



inline Scene* SingleQuadScene()
{
	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	// Materials:
	DeferredMaterial pbrMaterial = MaterialManager::TryGetDeferredMaterial("pbrDeferredGeometryMaterial");
	DeferredMaterial lightSourceMaterial = pbrMaterial.CloneWithDefaultBindings("pointLightMaterial");
	lightSourceMaterial.SetValue("SurfaceProperties", "surface_isLit", false);
	lightSourceMaterial.SetValue("SurfaceProperties", "surface_diffuseColor", Float4::white);
	DeferredMaterial quatMaterial = pbrMaterial.CloneWithDefaultBindings("quatMaterial");
	quatMaterial.SetTexture("colorMap", TextureManager::GetTexture("example"));
	//quatMaterial.SetTexture("roughnessMap", TextureManager::GetTexture("wood1_roughness"));
	//quatMaterial.SetTexture("normalMap", TextureManager::GetTexture("wood1_normal"));
	//quatMaterial.SetValue("SurfaceProperties", "surface_roughness", 1.0f);

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
	{// SpotLight:
		Entity entity = Entity::Create("light2");
		Float3 pos = Float3(0.0f, 0.0f, 5.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(lightSourceMaterial);
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
		pMeshRenderer->SetMesh(MeshManager::GetMesh("quad"));
		pMeshRenderer->SetMaterial(quatMaterial);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "surface_scaleOffset", Float4(1, 1, 0, 0));

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(45.0f);

		//DrawMeshData* pDrawMeshData = entity.AddComponent<DrawMeshData>();
	}
	return pScene;
}