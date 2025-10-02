#pragma one



inline Scene* ShadowCascadeScene()
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