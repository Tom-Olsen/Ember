#define SDL_MAIN_HANDLED
#include "application.h"
#include "emberEngine.h"



// TODO now!
// - change coordinate system to: x right, y forward, z up
// - directional lights: shadow cascades
// - sort gameObjects first by material (to reduce pipeline changes) and then by proximity to pCamera to reduce fragment culling (render closer objects first)
// - imgui integration
// - validation layer errors when two shaders have the same binding number (binding missmatch error)

// TODO:
// - adjust shadow config in shadowPipeline.cpp (bias values) for better shadow quality
// - optimizations: multi threaded render loop, culling, etc.
// - add pGameObject selection (need gizmos => ui renderpass)
// - currently one commandPool per commandBuffer, should be one commandPool per frame, shared by all commands for that frame?
// - uniform buffer (cbuffer) data that is the same for all draw calls (e.g. light data) should be stored in a single cbuffer
//   that is bound to a single descriptorSet that is bound to all draw calls that need that data.
//   => make this descriptorset a "global" object in the materialProperties class.
// - shadowMapping.hlsli: PhysicalDirectionalLights(...) has depth bias added manually. Should be done via pipeline rasterization state.
// - add geometry shader stage => wireframe rendering
// - gameobject parent system (GameObject � GameObject => transform hierarchy)
// - GameObject::Start/OnDestroy/OnCreate/OnEnable/OnDisable etc. methods
// - in macros.h reduce MAX_D_LIGHTS, MAX_S_LIGHTS, MAX_P_LIGHTS to a single MAX_SHADOW_MAPS
//   and make shadowmap indexing more dynamic to work with e.g. only point lights or only spot lights.
// - add logic to mesh class to only update the parts of the buffer that have changed (e.g. pos, normal, ...)
// - add debugOnly assert to check if 'normal' vectors are normalized and 'tangent' vectors are orthogonal to 'normal' vectors.
//   remove normalization of any input vector that is namen 'normal' or 'tangent' in mathf library (same for linearAlgebra.hlsli).
// - Proper documentation for materialProperties, uniformBuffers, spirvReflect classes

// TODO long term:
// - change image loading library, stb_image sucks.
// - proper quaternion support
// - ui renderpass that draws on top of everything and is not affected by the pCamera (constant view/projection matrix)
// - render image while resizing
// - implement game physics fixedUpdate loop
// - post processing
// - particles (instancing)
// - physics
// - audio
// - gameobject clipping logic for pCamera and lights (requires bounding box)
// - change shared ptr in VulkanUniformBuffer.buffer to unique ptr
// - write own logger class
// - better shadow mapping (PCF, soft shadows, etc.)
// - compile engine as .dll and link to game/test project
// - render into custom image with higher precision (16-bit floats for color instead of 8-bit) and push final image to swapchain.
//   This allows for HDR rendering and post processing effects.
//    - look into colorspace sRGB vs linear.
// - blender model import
// - text rendering
// - only load textures that are used. parallel loading of textures.

// Implemented Features:
// - Forward renderpipeline.
// - Multi lightsoure shadow mapping (directional-, point-, spotlights).
// - Physical based lighting (roughnessMap, normalMap, metallicity, reflectivity).
// - CubeMap (TextureCube) skybox.
// - Automated descriptorSet system for materialProperties (see spirvReflect.h/cpp).
// - Component � GameObject � Scene system with game update loop.
// - Graphics::Draw(...) allows for drawCall injection into render pipeline from GameObject Components.
// - EventSystem that catches SDL events and makes them visible to all GameObjects/Components.
// - CameraController that is identical to unities editor pCamera.
// - Own mathf library, see mathf.h/cpp.



Scene* ShadowCascadeScene()
{
	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(30.0f, 20.0f, 50.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up));

		Camera* pCamera = new Camera();
		pCamera->SetFarClip(1000.0f);
		pGameObject->AddComponent<Camera>(pCamera);

		CameraController* cameraController = new CameraController();
		pGameObject->AddComponent<CameraController>(cameraController);

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// Skybox:
		GameObject* pGameObject = new GameObject("skybox");

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skybox"));
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTextureCube("skyboxClouds0"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	{// ThreeLeg:
		GameObject* pGameObject = new GameObject("threeLeg");
		pGameObject->GetTransform()->SetPosition(-2.0f, 0.0f, 1.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("threeLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorUnlit"));
		pMeshRenderer->SetCastShadows(true);
		pMeshRenderer->SetReceiveShadows(true);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	Camera* pCamera = new Camera();
	{// TestCamera:
		GameObject* pGameObject = new GameObject("testCamera");
		Float3 pos = Float3(-20.0f, -20.0f, 0.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateFromTo(Float3::forward, Float3::right));

		pCamera->SetNearClip(0.5f);
		pCamera->SetFarClip(100.0f);
		pCamera->SetDrawFrustum(true);
		pGameObject->AddComponent<Camera>(pCamera);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLit"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		RotationController* pRotationController = new RotationController(45.0f);
		pGameObject->AddComponent<RotationController>(pRotationController);

		pScene->AddGameObject(pGameObject);
	}
	{// Test Light:
		GameObject* pGameObject = new GameObject("testLight");
		Float3 direction = Float3(-0.4f, -1.0f, -0.4f).Normalize();
		Float3x3 rotation = Float3x3::RotateFromTo(Float3::backward, direction);
		pGameObject->GetTransform()->SetPosition(Float3(0.0f, 3.0f, 0.0f));
		pGameObject->GetTransform()->SetRotationMatrix(rotation);

		DirectionalLight* pDirectionalLight = new DirectionalLight();
		pDirectionalLight->SetIntensity(1.0f);
		pDirectionalLight->SetColor(Float3(1.0f, 0.0f, 0.0f));
		pDirectionalLight->SetNearClip(1.1f);
		pDirectionalLight->SetFarClip(15.0f);
		pDirectionalLight->SetViewWidth(10.0f);
		pDirectionalLight->SetViewHeight(10.0f);
		pDirectionalLight->SetActiveCamera(pCamera, true);
		pDirectionalLight->SetDrawFrustum(false);
		pDirectionalLight->SetShowShadowCascades(true);
		pGameObject->AddComponent<DirectionalLight>(pDirectionalLight);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLit"));
		pMeshRenderer->SetCastShadows(true);
		pMeshRenderer->SetReceiveShadows(true);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}
Scene* TestScene()
{
	// This scene replicated the binding missmatch error.
	// The two test materials are spcifically constructed to replicate this error.
	// Activate validation layers in VulkanRenderer/vulkanMacros.h to see the error.

	Material* materialA = MaterialManager::GetMaterial("testA");
	Material* materialB = MaterialManager::GetMaterial("testB");

	//materialA->PrintBindings();
	//materialA->PrintUniformBuffers();
	//materialB->PrintBindings();
	//materialB->PrintUniformBuffers();

	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, 3.0f, 5.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up));

		Camera* pCamera = new Camera();
		pGameObject->AddComponent<Camera>(pCamera);

		CameraController* cameraController = new CameraController();
		pGameObject->AddComponent<CameraController>(cameraController);

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// PointLight:
		GameObject* pGameObject = new GameObject("pointLight");
		Float3 pos = Float3(1.0f, 2.0f, 0.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("simpleUnlit"));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		PointLight* pPointLight = new PointLight();
		pPointLight->SetIntensity(10.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.5f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(false);
		pGameObject->AddComponent<PointLight>(pPointLight);

		SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 45, 0));
		pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		pScene->AddGameObject(pGameObject);
	}
	{ // Floor:
		GameObject* pGameObject = new GameObject("floor");
		pGameObject->GetTransform()->SetPosition(0.0f, -0.5f, 0.0f);
		pGameObject->GetTransform()->SetScale(30.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(-90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(materialA);
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("grass"));
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	{// Cube:
		GameObject* pGameObject = new GameObject("unitCube");
		pGameObject->GetTransform()->SetPosition(-2.0f, 0.0f, 1.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(materialB);
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("brick"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("cubeMap", TextureManager::GetTextureCube("skyboxClouds0"));
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	return pScene;
}
Scene* DefaultScene()
{
	bool directionalLightsActive = false;
	bool spotLightsActive = !directionalLightsActive;
	bool showLightFrustums = false;

	Scene* pScene = new Scene();
	{// Camera:
		GameObject* pGameObject = new GameObject("mainCamera");
		Float3 pos = Float3(0.0f, 3.0f, 5.0f);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up));

		Camera* pCamera = new Camera();
		pCamera->SetFarClip(1000.0f);
		pGameObject->AddComponent<Camera>(pCamera);

		CameraController* cameraController = new CameraController();
		pGameObject->AddComponent<CameraController>(cameraController);

		pScene->AddGameObject(pGameObject);
		pScene->SetActiveCamera(pCamera);
	}
	{// PointLight:
		GameObject* pGameObject = new GameObject("pointLight");
		Float3 pos = Float3(1.0f, 2.0f, 0.0f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("simpleUnlit"));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		PointLight* pPointLight = new PointLight();
		pPointLight->SetIntensity(10.0f);
		pPointLight->SetColor(Float3(1.0f, 1.0f, 1.0f));
		pPointLight->SetNearClip(0.5f);
		pPointLight->SetFarClip(20.0f);
		pPointLight->SetDrawFrustum(showLightFrustums);
		pGameObject->AddComponent<PointLight>(pPointLight);

		SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 45, 0));
		pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		pScene->AddGameObject(pGameObject);
	}
	{// Light0:
		GameObject* pGameObject = new GameObject("light0");
		Float3 pos = Float3(7.0f, 7.0f, 3.5f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLit"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		//SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 45, 0));
		//pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		if (directionalLightsActive)
		{
			DirectionalLight* pDirectionalLight = new DirectionalLight();
			pDirectionalLight->SetIntensity(1.0f);
			pDirectionalLight->SetColor(Float3(1.0f, 0.0f, 0.0f));
			pDirectionalLight->SetNearClip(1.1f);
			pDirectionalLight->SetFarClip(15.0f);
			pDirectionalLight->SetViewWidth(10.0f);
			pDirectionalLight->SetViewHeight(10.0f);
			pDirectionalLight->SetDrawFrustum(showLightFrustums);
			pGameObject->AddComponent<DirectionalLight>(pDirectionalLight);
		}

		if (spotLightsActive)
		{
			SpotLight* pSpotLight = new SpotLight();
			pSpotLight->SetColor(Float3(1.0f, 0.0f, 0.0f));
			pSpotLight->SetIntensity(100.0f);
			pSpotLight->SetNearClip(1.1f);
			pSpotLight->SetFarClip(20.0f);
			pSpotLight->SetFov(mathf::DEG2RAD * 30.0f);
			pSpotLight->SetBlendStart(0.7f);
			pSpotLight->SetBlendEnd(0.9f);
			pSpotLight->SetDrawFrustum(showLightFrustums);
			pGameObject->AddComponent<SpotLight>(pSpotLight);
		}

		pScene->AddGameObject(pGameObject);
	}
	{// Light1:
		GameObject* pGameObject = new GameObject("light1");
		Float3 pos = Float3(-7.0f, 7.0f, 3.5f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLit"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		//SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, -60, 0));
		//pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		if (directionalLightsActive)
		{
			DirectionalLight* pDirectionalLight = new DirectionalLight();
			pDirectionalLight->SetIntensity(1.0f);
			pDirectionalLight->SetColor(Float3(0.0f, 1.0f, 0.0f));
			pDirectionalLight->SetNearClip(1.1f);
			pDirectionalLight->SetFarClip(15.0f);
			pDirectionalLight->SetViewWidth(10.0f);
			pDirectionalLight->SetViewHeight(10.0f);
			pDirectionalLight->SetDrawFrustum(showLightFrustums);
			pGameObject->AddComponent<DirectionalLight>(pDirectionalLight);
		}

		if (spotLightsActive)
		{
			SpotLight* pSpotLight = new SpotLight();
			pSpotLight->SetColor(Float3(0.0f, 1.0f, 0.0f));
			pSpotLight->SetIntensity(100.0f);
			pSpotLight->SetNearClip(1.1f);
			pSpotLight->SetFarClip(20.0f);
			pSpotLight->SetFov(mathf::DEG2RAD * 30.0f);
			pSpotLight->SetBlendStart(0.7f);
			pSpotLight->SetBlendEnd(0.9f);
			pSpotLight->SetDrawFrustum(showLightFrustums);
			pGameObject->AddComponent<SpotLight>(pSpotLight);
		}

		pScene->AddGameObject(pGameObject);
	}
	{// Light2:
		GameObject* pGameObject = new GameObject("light2");
		Float3 pos = Float3(0.0f, 7.0f, -7.5f);
		Float3x3 matrix = Float3x3::RotateThreeLeg(Float3::backward, -pos, Float3::up, Float3::up);
		pGameObject->GetTransform()->SetPosition(pos);
		pGameObject->GetTransform()->SetRotationMatrix(matrix);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("fourLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLit"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		//SpinGlobal* pSpinGlobal = new SpinGlobal(Float3::zero, Float3(0, 90, 0));
		//pGameObject->AddComponent<SpinGlobal>(pSpinGlobal);

		if (directionalLightsActive)
		{
			DirectionalLight* pDirectionalLight = new DirectionalLight();
			pDirectionalLight->SetIntensity(1.0f);
			pDirectionalLight->SetColor(Float3(0.0f, 0.0f, 1.0f));
			pDirectionalLight->SetNearClip(1.1f);
			pDirectionalLight->SetFarClip(15.0f);
			pDirectionalLight->SetViewWidth(10.0f);
			pDirectionalLight->SetViewHeight(10.0f);
			pDirectionalLight->SetDrawFrustum(showLightFrustums);
			pGameObject->AddComponent<DirectionalLight>(pDirectionalLight);
		}

		if (spotLightsActive)
		{
			SpotLight* pSpotLight = new SpotLight();
			pSpotLight->SetColor(Float3(0.0f, 0.0f, 1.0f));
			pSpotLight->SetIntensity(100.0f);
			pSpotLight->SetNearClip(1.1f);
			pSpotLight->SetFarClip(20.0f);
			pSpotLight->SetFov(mathf::DEG2RAD * 30.0f);
			pSpotLight->SetBlendStart(0.7f);
			pSpotLight->SetBlendEnd(0.9f);
			pSpotLight->SetDrawFrustum(showLightFrustums);
			pGameObject->AddComponent<SpotLight>(pSpotLight);
		}

		pScene->AddGameObject(pGameObject);
	}
	{// Skybox:
		GameObject* pGameObject = new GameObject("skybox");

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("skybox"));
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTextureCube("skyboxClouds0"));
		pMeshRenderer->SetCastShadows(false);
		pMeshRenderer->SetReceiveShadows(false);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	{ // Floor:
		GameObject* pGameObject = new GameObject("floor");
		pGameObject->GetTransform()->SetPosition(0.0f, -0.5f, 0.0f);
		pGameObject->GetTransform()->SetScale(30.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(-90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("default"));
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("ground0_color"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("ground0_roughness"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("ground0_normal"));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(10, 10, 1, 1));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	{ // Wall:
		GameObject* pGameObject = new GameObject("wall");
		pGameObject->GetTransform()->SetPosition(0.0f, 2.5f, -10.0f);
		pGameObject->GetTransform()->SetScale(Float3(30.0f, 6.0f, 1.0f));

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitQuad"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("default"));
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("bricks0_color"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("bricks0_roughness"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("bricks0_normal"));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(10, 2, 1, 1));
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	{// ThreeLeg:
		GameObject* pGameObject = new GameObject("threeLeg");
		pGameObject->GetTransform()->SetPosition(-2.0f, 0.0f, 1.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("threeLeg"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("vertexColorLit"));
		pMeshRenderer->SetCastShadows(true);
		pMeshRenderer->SetReceiveShadows(true);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	{// Cube0:
		GameObject* pGameObject = new GameObject("cube0");
		pGameObject->GetTransform()->SetPosition(-1.0f, 0.0f, -1.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("default"));
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("wood1_color"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("wood1_roughness"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("wood1_normal"));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 1, 1));
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		SpinLocal* pSpinLocal = new SpinLocal(Float3(0.0f, 45.0f, 0.0f));
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		//DrawMeshData* pDrawMeshData = new DrawMeshData();
		//pGameObject->AddComponent<DrawMeshData>(pDrawMeshData);

		pScene->AddGameObject(pGameObject);
	}
	{// Cube1:
		GameObject* pGameObject = new GameObject("cube1");
		pGameObject->GetTransform()->SetPosition(2.0f, 0.0f, 0.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("default"));
		pMeshRenderer->SetMaterial(nullptr);
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("bricks1_color"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("bricks1_roughness"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("bricks1_normal"));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 1, 1));
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		pScene->AddGameObject(pGameObject);
	}
	{// Zylinder 0:
		GameObject* pGameObject = new GameObject("sphere0");
		pGameObject->GetTransform()->SetPosition(0.5f, 0.0f, 0.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(90.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("zylinderSmooth"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("default"));
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("bricks2_color"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("bricks2_roughness"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("bricks2_normal"));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		//DrawMeshData* drawMeshData = new DrawMeshData();
		//pGameObject->AddComponent<DrawMeshData>(drawMeshData);

		SpinLocal* pSpinLocal = new SpinLocal(Float3(0.0f, 0.0f, 45.0f));
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		pScene->AddGameObject(pGameObject);
	}
	{// Sphere 0:
		GameObject* pGameObject = new GameObject("sphere1");
		pGameObject->GetTransform()->SetPosition(-0.5f, 0.0f, 1.0f);
		pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);

		MeshRenderer* pMeshRenderer = new MeshRenderer();
		pMeshRenderer->SetMesh(MeshManager::GetMesh("cubeSphere"));
		pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("default"));
		pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("wood0_color"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("wood0_roughness"));
		pMeshRenderer->GetMaterialProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("wood0_normal"));
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
		pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "scaleOffset", Float4(0.5, 0.5, 1, 1));
		pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);

		SpinLocal* pSpinLocal = new SpinLocal(Float3(0.0f, 45.0f, 0.0f));
		pGameObject->AddComponent<SpinLocal>(pSpinLocal);

		//DrawMeshData* pDrawMeshData = new DrawMeshData();
		//pGameObject->AddComponent<DrawMeshData>(pDrawMeshData);

		pScene->AddGameObject(pGameObject);
	}
	//{// Cube Array:
	//    int N = 3;
	//    float dist = 4.0f;
	//    for (int i = 0; i < N; i++)
	//        for (int j = 0; j < N; j++)
	//        {
	//            GameObject* pGameObject = new GameObject("cube" + std::to_string(i) + std::to_string(j));
	//			Float3 pos((i / (N - 1.0f) - 0.5f) * dist, (j / (N - 1.0f) - 0.5f) * dist + 0.5f * dist, -8.0f);
	//            pGameObject->GetTransform()->SetPosition(pos);
	//            pGameObject->GetTransform()->SetRotationEulerDegrees(0.0f, 0.0f, 0.0f);
	//
	//            MeshRenderer* pMeshRenderer = new MeshRenderer();
	//            pMeshRenderer->SetMesh(MeshManager::GetMesh("unitCube"));
	//            pMeshRenderer->SetMaterial(MaterialManager::GetMaterial("default"));
	//            pMeshRenderer->GetMaterialProperties()->SetSampler("colorSampler", SamplerManager::GetSampler("colorSampler"));
	//            pMeshRenderer->GetMaterialProperties()->SetTexture2d("colorMap", TextureManager::GetTexture2d("concrete0_color"));
	//            pMeshRenderer->GetMaterialProperties()->SetTexture2d("roughnessMap", TextureManager::GetTexture2d("concrete0_roughness"));
	//            pMeshRenderer->GetMaterialProperties()->SetTexture2d("normalMap", TextureManager::GetTexture2d("concrete0_normal"));
	//            pMeshRenderer->GetMaterialProperties()->SetValue("SurfaceProperties", "roughness", 1.0f);
	//            pGameObject->AddComponent<MeshRenderer>(pMeshRenderer);
	//
	//            pScene->AddGameObject(pGameObject);
	//        }
	//}
	return pScene;
}



int main()
{
	// VS debugging:
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialization:
	Application app;
	//Scene* pScene = ShadowCascadeScene();
	//Scene* pScene = TestScene();
	Scene* pScene = DefaultScene();
	app.SetScene(pScene);

	// Debugging:
	//LOG_TRACE(material->GetUniformBufferBlock("LightMatrizes")->ToString());
	//MaterialManager::GetMaterial("default")->PrintBindings();
	//MaterialManager::GetMaterial("skybox")->PrintBindings();
	//MaterialManager::GetMaterial("vertexColorLit")->PrintUniformBuffers();
	//MaterialManager::GetMaterial("default")->PrintUniformBuffers();
	//TextureManager::PrintAllTextureNames();
	//pScene->PrintGameObjects();
	//pScene->PrintMeshRenderers();
	//pScene->PrintSortedMeshRenderers();
	//pScene->PrintLights();
	// return 0;

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
	return 0;
}