#pragma one



inline Scene* PointLightScene()
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