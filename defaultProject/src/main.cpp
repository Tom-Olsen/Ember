﻿#define SDL_MAIN_HANDLED
#include "application.h"
#include "emberEngine.h"
// Components:
#include "camera.h"
#include "cameraController.h"
#include "directionalLight.h"
#include "drawMeshData.h"
#include "instancedRenderer.h"
#include "meshRenderer.h"
#include "meshTester.h"
#include "pointLight.h"
#include "postRenderEffects.h"
#include "profiler.h"
#include "settings.h"
#include "spinGlobal.h"
#include "spinLocal.h"
#include "spotLight.h"
#include "testInstancedRendering.h"
#include "transform.h"
using namespace emberEngine;



// Ember::TODO now!
// - improve PercentageCloserFilteredShadow (shadowMapping.hlsli) to work across shadowmap boundaries.
// - sort gameObjects first by material (to reduce pipeline changes) and then by proximity to pCamera to reduce fragment culling (render closer objects first)
// - validation layer errors when two shaders have the same binding number (binding missmatch error)
// - SceneView gui 
// - go through all classes and make sure that rule of five is applied correctly.

// Ember::TODO:
// - use transfer queue in mesh class instead of graphics queue for index and vertex data transfer.
//   use framesInFlight many vertex and index buffers, with two staging buffers (one for vertex and one for index data).
//   adjust staging buffer -> mesh buffers copy to account fro frame index (same as shaderProperties) and sync with render pipeline.
// - think about different texture2d specializations. Are they all needed? Can they be simplified? How would I remove the m_layout tracking on CPU side?
// - when not using input.vertexColor in the vertex shader, spirv optimizes the input binding away, which leads to incorrect bindings in
//   my spirv reflection => other bindings are wrong, and textures are not displayed at all.
// - optimize eventsystem::AnyKey etc.
// - remove scaling from view matrizes of lights and cameras?
// - adjust shadow config in shadowPipeline.cpp (bias values) for better shadow quality
// - optimizations: multi threaded render loop, culling, etc.
// - add pGameObject selection (need gizmos => ui renderpass?)
// - uniform buffer (cbuffer) data that is the same for all draw calls (e.g. light data) should be stored in a single cbuffer
//   that is bound to a single descriptorSet that is bound to all draw calls that need that data.
//   => make this descriptorset a "global" object in the shaderProperties class.
// - add geometry shader stage => wireframe rendering
// - gameobject parent system (GameObject € GameObject => transform hierarchy)
// - add logic to mesh class to only update the parts of the buffer that have changed (e.g. pos, normal, ...)
// - add debugOnly assert to check if 'normal' vectors are normalized and 'tangent' vectors are orthogonal to 'normal' vectors.
//   remove normalization of any input vector that is namen 'normal' or 'tangent' or 'direction' in mathf library (same for linearAlgebra.hlsli).
// - Proper documentation for shaderProperties, uniformBuffers, spirvReflect classes

// Ember::TODO long term:
// - dimm line between shadow cascades.
// - change image loading library, stb_image sucks.
// - proper quaternion support
// - render image while resizing
// - physics
// - audio
// - gameobject clipping logic for pCamera and lights (requires bounding box)
// - write own logger class
// - better shadow mapping (PCF, soft shadows, etc.)
// - look into colorspace sRGB vs linear.
// - blender model import
// - text rendering
// - only load textures that are used. parallel loading of textures.

// Implemented Features:
// - pre render compute shaders.
// - shadow pipeline
// - forward render pipeline.
// - post render compute shaders.
// - async & immediate compute.
// - Multi lightsoure shadow mapping (directionallights, positionallights).
// - Physical based lighting (roughnessMap, normalMap, metallicity, reflectivity).
// - Shadow cascades with shadow snapping.
// - CubeMap (TextureCube) skybox.
// - Automated descriptorSet system for shaderProperties (see spirvReflect.h/cpp) which handles descriptorSets for Materials and ComputeShaders.
// - Component € GameObject € Scene system with game update loop.
// - Graphics::Draw(...) allows for drawCall injection into render pipeline from GameObject Components.
// - EventSystem that catches SDL events and makes them visible to all GameObjects/Components.
// - CameraController that is identical to unities editor pCamera.
// - Own math library, see math.h/cpp.
// - Dear ImGui integration with docking feature.
// - Batched texture uploading.



Scene* ShadowCascadeScene()
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
		Settings* pSettings = pGameObject->AddComponent<Settings>();

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// Skybox:
		GameObject* pGameObject = new GameObject("skybox");

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("skybox0"));
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
		pDirectionalLight->SetShadowType(Lighting::ShadowType::soft);
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
	        pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
	        pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("bricks0_color"));
	        pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("bricks0_roughness"));
	        pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("bricks0_normal"));
	        pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
	
	        pScene->AddGameObject(pGameObject);
	    }
	}
	return pScene;
}
Scene* TestScene()
{
	// This scene replicated the binding missmatch error.
	// The two test materials are spcifically constructed to replicate this error.
	// Activate validation layers in VulkanRenderer/vulkanMacros.h to see the error.

	Material* materialA = MaterialManager::GetMaterial("testAMaterial");
	Material* materialB = MaterialManager::GetMaterial("testBMaterial");

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
		Settings* pSettings = pGameObject->AddComponent<Settings>();

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
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "diffuseColor", Float4::white);
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
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("grass"));
		
		pScene->AddGameObject(pGameObject);
	}
	{// Cube:
		GameObject* pGameObject = new GameObject("unitCube");
		pGameObject->GetTransform()->SetPosition(-2.0f, 0.0f, 1.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(materialB);
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("brick"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("cubeMap", TextureManager::GetTexture2d("skybox0"));

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}
Scene* DefaultScene()
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
		Settings* pSettings = pGameObject->AddComponent<Settings>();

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
	//	pDirectionalLight->SetShadowType(Lighting::ShadowType::soft);
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
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
	
		PointLight* pPointLight = pGameObject->AddComponent<PointLight>();
		pPointLight->SetIntensity(5.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.5f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(showLightFrustums);
		pPointLight->SetShadowType(Lighting::ShadowType::hard);
	
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
		pSpotLight->SetShadowType(Lighting::ShadowType::hard);
	
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
		pSpotLight->SetShadowType(Lighting::ShadowType::hard);
	
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
		pSpotLight->SetShadowType(Lighting::ShadowType::hard);
	
		pScene->AddGameObject(pGameObject);
	}
	{// Skybox:
		GameObject* pGameObject = new GameObject("skybox");
		pGameObject->GetTransform()->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skyboxMaterial"));
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("skybox0"));
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
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("ground0_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("ground0_roughness"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("ground0_normal"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(10, 10, 0, 0));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);

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
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("bricks0_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("bricks0_roughness"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("bricks0_normal"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(10, 2, 0, 0));

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
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("wood1_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("wood1_roughness"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("wood1_normal"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

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
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("bricks1_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("bricks1_roughness"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("bricks1_normal"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

		MeshTester* pMeshTester = new MeshTester(pMeshRenderer);
		pMeshTester->AddMesh(MeshManager::GetMesh("unitQuad"));
		pMeshTester->AddMesh(MeshManager::GetMesh("unitCube"));
		pMeshTester->AddMesh(MeshManager::GetMesh("halfCube"));
		pMeshTester->AddMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshTester->AddMesh(MeshManager::GetMesh("disk"));
		pMeshTester->AddMesh(MeshManager::GetMesh("arcFlatUv"));
		pMeshTester->AddMesh(MeshManager::GetMesh("arcCurvedUv"));
		pMeshTester->AddMesh(MeshManager::GetMesh("coneSmooth"));
		pMeshTester->AddMesh(MeshManager::GetMesh("coneFlat"));
		pMeshTester->AddMesh(MeshManager::GetMesh("zylinderSmooth"));
		pMeshTester->AddMesh(MeshManager::GetMesh("zylinderFlat"));
		pMeshTester->AddMesh(MeshManager::GetMesh("arrowSmooth"));
		pMeshTester->AddMesh(MeshManager::GetMesh("arrowFlat"));
		pMeshTester->AddMesh(MeshManager::GetMesh("threeLeg"));
		pMeshTester->AddMesh(MeshManager::GetMesh("fourLeg"));
		pGameObject->AddComponent<MeshTester>(pMeshTester);
	
		pScene->AddGameObject(pGameObject);
	}
	{// Zylinder 0:
		GameObject* pGameObject = new GameObject("zylinder0");
		pGameObject->GetTransform()->SetPosition(0.5f, 0.0f, 0.0f);
	
		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("zylinderSmooth"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("bricks2_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("bricks2_roughness"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("bricks2_normal"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
	
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
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("wood0_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("wood0_roughness"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("wood0_normal"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 0, 0));

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
	//	pInstancedRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
	//	pInstancedRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("concrete0_color"));
	//	pInstancedRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("concrete0_roughness"));
	//	pInstancedRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("concrete0_normal"));
	//	pInstancedRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
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
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "diffuseColor", Float4(1.0, 0.0, 0, 0.25f));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}
Scene* PointLightScene()
{
	bool directionalLightsActive = false;

	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(9.0f, -9.0f, 9.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::down, -pos, Float3::forward, Float3::up));

		Camera* pCamera = pGameObject->AddComponent<Camera>();
		pCamera->SetFarClip(1000.0f);
		PostRenderEffects* pPostRenderEffects = pGameObject->AddComponent<PostRenderEffects>();
		CameraController* cameraController = pGameObject->AddComponent<CameraController>();
		Settings* pSettings = pGameObject->AddComponent<Settings>();

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// PointLight:
		GameObject* pGameObject = new GameObject("pointLight");
		Float3 pos = Float3(0.0f, 0.0f, 0.0f);
		pGameObject->GetTransform()->SetPosition(pos);
	
		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("simpleUnlitMaterial"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
	
		PointLight* pPointLight = pGameObject->AddComponent<PointLight>();
		pPointLight->SetIntensity(10.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.1f);
		pPointLight->SetFarClip(8.0f);
		pPointLight->SetShadowType(Lighting::ShadowType::hard);
		//pPointLight->SetDrawFrustum(true);
	
		pScene->AddGameObject(pGameObject);
	}
	{// Room:
		GameObject* pGameObject = new GameObject("room");
		pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
		pGameObject->GetTransform()->SetScale(10.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCubeInverse"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("bricks1_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("bricks1_roughness"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("bricks1_normal"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(1.0, 1.0, 0, 0));

		pScene->AddGameObject(pGameObject);
	}
	{// Frame:
		GameObject* pGameObject = new GameObject("frame");
		pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
		pGameObject->GetTransform()->SetScale(1.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("frame"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("metal0_color"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("metal0_normal"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("metallicMap", TextureManager::GetTexture2d("metal0_metallic"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("metal0_roughness"));
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "metallicity", 1.0f);
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(0.25, 1, 0, 0));

		SpinLocal* pSpinLocal = new SpinLocal(45.0f);
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}
Scene* SingleQuadScene()
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
		Settings* pSettings = pGameObject->AddComponent<Settings>();

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
		pSpotLight->SetShadowType(Lighting::ShadowType::soft);

		pScene->AddGameObject(pGameObject);
	}
	{// Quad:
		GameObject* pGameObject = new GameObject("quad");
		pGameObject->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("defaultMaterial"));
		pMeshRenderer->GetShaderProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetShaderProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("example"));
		//pMeshRenderer->GetShaderProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("wood1_roughness"));
		//pMeshRenderer->GetShaderProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("wood1_normal"));
		//pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetShaderProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(1, 1, 0, 0));

		SpinLocal* pSpinLocal = new SpinLocal(45.0f);
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		//DrawMeshData* pDrawMeshData = pGameObject->AddComponent<DrawMeshData>();

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}



int main()
{
	// VS debugging (windows only=MSVC compiler):
	#ifdef _MSC_VER
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	// Profiler:
	Profiler::Session& session = Profiler::Session::Get();
	session.Start("defaultProject", "profilingResults");

	// Initialization:
	Application::Settings appSettings = {};
	appSettings.vSyncEnabled = false;
	appSettings.framesInFlight = 2;
	appSettings.msaaSamples = VK_SAMPLE_COUNT_4_BIT;
	appSettings.windowWidth = 2560;//1920;
	appSettings.windowHeight = 1440;//1080;
	appSettings.renderWidth = 2560;//1280;
	appSettings.renderHeight = 1440;//720;
	Application app(appSettings);

	// Create scene:
	//Scene* pScene = ShadowCascadeScene();
	//Scene* pScene = TestScene();
	Scene* pScene = DefaultScene();
	//Scene* pScene = PointLightScene();
	//Scene* pScene = SingleQuadScene();
	app.SetScene(pScene);

	// Debugging:
	//ComputeShader* csTest =  ComputeShaderManager::GetComputeShader("testComputeShader");
	//csTest->PrintShaderInfo();
	//ShaderProperties* csProperties = new ShaderProperties(csTest);
	//csProperties->Print("test");
	//csProperties->PrintMaps();
	//csProperties->SetTexture2d("image", TextureManager::GetTexture2d("storageTexture8x8"));
	//delete csProperties;
	//TextureManager::PrintAllTextureNames();
	//pScene->PrintGameObjects();
	//pScene->PrintLights();
	//return 0;

	// Run application:
	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Exception: {}", e.what());
	}

	// Terminate:
	delete pScene;

	// Runtime analysis:
	Profiler::Session::Get().End();
	std::vector<std::string> results = session.GetAllResultNames();
	for (std::string& result : results)
		session.PrintFunctionAverageTime(result, TimeUnit::ms);

	return 0;
}