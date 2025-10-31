#pragma once
#include "emberEngine.h"
// Components:
#include "drawMeshData.h"
#include "meshTester.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "testInstancedRendering.h"
using namespace emberEngine;



inline Scene* DefaultScene()
{
	bool directionalLightsActive = false;
	bool showLightFrustums = false;

	Scene* pScene = new Scene();
	pScene->SetIsEnabled(true);

	{// Camera:
		Entity entity = Entity::Create("mainCamera");
		Float3 pos = Float3(0.0f, -8.0f, 3.0f);
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
	//{// Directional Light:
	//	Entity entity = Entity::Create("directionalLight");
	//	Float3 pos = Float3(0.0f, 3.0f, 0.0f);
	//	Float3 direction = Float3(-0.4f, 0.4f, -1.0f).Normalize();
	//	Float3x3 matrix = Float3x3::RotateFromTo(Float3::down, direction);
	//
	//	Transform* pTransform = entity.GetTransform();
	//	pTransform->SetPosition(pos);
	//	pTransform->SetRotationMatrix(matrix);
	//
	//	DirectionalLight* pDirectionalLight = entity.AddComponent<DirectionalLight>();
	//	pDirectionalLight->SetIntensity(1.0f);
	//	pDirectionalLight->SetColor(Float3::white);
	//	pDirectionalLight->SetDrawFrustum(true);
	//	pDirectionalLight->SetShadowCascadeCount(3);
	//	pDirectionalLight->SetShadowCascadeSplit(0, 0.0f);
	//	pDirectionalLight->SetShadowCascadeSplit(1, 0.2f);
	//	pDirectionalLight->SetShadowCascadeSplit(2, 0.5f);
	//	pDirectionalLight->SetShadowCascadeSplit(3, 1.0f);
	//	pDirectionalLight->SetShadowType(emberCommon::ShadowType::soft);
	//}
	{// PointLight:
		Entity entity = Entity::Create("pointLight");
		Float3 pos = Float3(1.0f, 0.0f, 1.5f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::back, -pos, Float3::forward, Float3::up);

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
		pPointLight->SetIntensity(5.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.5f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(showLightFrustums);
		pPointLight->SetShadowType(emberCommon::ShadowType::hard);

		SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, 0, 45));
	}
	{// SpotLight0:
		Entity entity = Entity::Create("light0");
		Float3 pos = Float3(7.0f, 3.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, 45, 0));

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::red);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// SpotLight1:
		Entity entity = Entity::Create("light1");
		Float3 pos = Float3(-7.0f, 3.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, -60, 0));

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::green);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// SpotLight2:
		Entity entity = Entity::Create("light2");
		Float3 pos = Float3(0.0f, -7.5f, 7.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlitMaterial"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		//SpinGlobal* pSpinGlobal = entity.AddComponent<SpinGlobal>(Float3::zero, Float3(0, 90, 0));

		SpotLight* pSpotLight = entity.AddComponent<SpotLight>();
		pSpotLight->SetColor(Float3::blue);
		pSpotLight->SetIntensity(200.0f);
		pSpotLight->SetNearClip(1.1f);
		pSpotLight->SetFarClip(20.0f);
		pSpotLight->SetFov(math::deg2rad * 30.0f);
		pSpotLight->SetBlendStart(0.7f);
		pSpotLight->SetBlendEnd(0.9f);
		pSpotLight->SetDrawFrustum(showLightFrustums);
		pSpotLight->SetShadowType(emberCommon::ShadowType::hard);
	}
	{// Skybox:
		Entity entity = Entity::Create("skybox");

		Transform* pTransform = entity.GetTransform();
		pTransform->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("skybox0"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
	}
	{ // Floor:
		Entity entity = Entity::Create("floor");
		Float3 pos = Float3(0.0f, 0.0f, -0.5f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetScale(30.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("ground0_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("ground0_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("ground0_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(10, 10, 0, 0));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
	}
	{ // Wall:
		Entity entity = Entity::Create("wall");
		Float3 pos = Float3(0.0f, 10.0f, 2.5f);
		Float3 scale = Float3(30.0f, 6.0f, 1.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetScale(scale);
		pTransform->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks0_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks0_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks0_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(10, 2, 0, 0));
	}
	{// ThreeLeg:
		Entity entity = Entity::Create("threeLeg");
		Float3 pos = Float3(-3.0f, -2.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("threeLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLitMaterial"));
		pMeshRenderer->SetCastShadows(true);
		pMeshRenderer->SetReceiveShadows(true);
	}
	{// Cube0:
		Entity entity = Entity::Create("cube0");
		Float3 pos = Float3(-2.0f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("wood1_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("wood1_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("wood1_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(45.0f);

		//DrawMeshData* pDrawMeshData = entity.AddComponent<DrawMeshData>();
	}
	{// MeshTester:
		Entity entity = Entity::Create("meshTester");
		Float3 pos = Float3(2.0f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks1_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks1_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks1_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

		MeshTester* pMeshTester = entity.AddComponent<MeshTester>(pMeshRenderer);
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
	}
	{// Zylinder 0:
		Entity entity = Entity::Create("zylinder0");
		Float3 pos = Float3(0.5f, 0.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("zylinderSmooth"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("bricks2_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("bricks2_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("bricks2_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);

		//DrawMeshData* drawMeshData = entity.AddComponent<DrawMeshData>();

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(45.0f);
	}
	{// Sphere 0:
		Entity entity = Entity::Create("sphere1");
		Float3 pos = Float3(-0.5f, 1.0f, 0.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("wood0_color"));
		pMeshRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("wood0_roughness"));
		pMeshRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("wood0_normal"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

		SpinLocal* pSpinLocal = entity.AddComponent<SpinLocal>(45.0f);

		//DrawMeshData* pDrawMeshData = entity.AddComponent<DrawMeshData>();
	}
	//{// Instanced Cube Array:
	//	Entity entity = Entity::Create("cubeArray");
	//	Float3 pos = Float3::zero;
	//
	//	Transform* pTransform = entity.GetTransform();
	//	pTransform->SetPosition(pos);
	//	pTransform->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
	//	
	//	TestInstancedRendering* testInstancedRendering = entity.AddComponent<TestInstancedRendering>(16);
	//
	//	InstancedMeshRenderer* pInstancedRenderer = entity.AddComponent<InstancedMeshRenderer>();
	//	pInstancedRenderer->SetInstanceCount(16);
	//	pInstancedRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
	//	pInstancedRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
	//	pInstancedRenderer->SetInstanceBuffer(testInstancedRendering->GetInstanceBuffer());
	//	pInstancedRenderer->GetShaderProperties().SetTexture("colorMap", TextureManager::GetTexture("concrete0_color"));
	//	pInstancedRenderer->GetShaderProperties().SetTexture("roughnessMap", TextureManager::GetTexture("concrete0_roughness"));
	//	pInstancedRenderer->GetShaderProperties().SetTexture("normalMap", TextureManager::GetTexture("concrete0_normal"));
	//	pInstancedRenderer->GetShaderProperties().SetValue("SurfaceProperties", "roughness", 1.0f);
	//}
	{// Transparent quad:
		Entity entity = Entity::Create("transparentQuad");
		Float3 pos = Float3(0.66f, -1.33f, 1.0f);

		Transform* pTransform = entity.GetTransform();
		pTransform->SetPosition(pos);
		pTransform->SetRotationEulerDegrees(60.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = entity.AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("transparentMaterial"));
		pMeshRenderer->GetShaderProperties().SetValue("SurfaceProperties", "diffuseColor", Float4(1.0, 0.0, 0, 0.25f));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
	}
	return pScene;
}