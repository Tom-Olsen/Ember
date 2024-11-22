#include "scene.h"
#include "logger.h"



// Constructor:
Scene::Scene()
{
	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		directionalLights[i] = nullptr;
	for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
		spotLights[i] = nullptr;
	//for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
	//	pointLights[i] = nullptr;
}



// Destructor:
Scene::~Scene()
{

}



// Public methods:
void Scene::AddGameObject(GameObject* gameObject)
{
	// If scene aleady contains gameObject, do nothing.
	if (gameObjects.find(gameObject->name) != gameObjects.end())
	{
		LOG_WARN("GameObject '{}' is already in scene!", gameObject->name);
		return;
	}
	// Add gameObject and associated pointers to scene.
	else
	{
		gameObjects[gameObject->name] = std::unique_ptr<GameObject>(gameObject);

		MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
		if (meshRenderer != nullptr)
		{
			meshRenderers.emplace(gameObject->name, meshRenderer);
			sortedMeshRenderers.push_back(meshRenderer);
		}

		DirectionalLight* directionalLight = gameObject->GetComponent<DirectionalLight>();
		if (directionalLight != nullptr)
		{
			for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
				if (directionalLights[i] == nullptr)
				{
					directionalLights[i] = directionalLight;
					dLightsCount++;
					break;
				}
		}
		SpotLight* spotLight = gameObject->GetComponent<SpotLight>();
		if (spotLight != nullptr)
		{
			for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
				if (spotLights[i] == nullptr)
				{
					spotLights[i] = spotLight;
					sLightsCount++;
					break;
				}
		}
		//PointLight* pointLight = gameObject->GetComponent<PointLight>();
		//if (pointLight != nullptr)
		//{
		//	for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
		//		if (pointLights[i] == nullptr)
		//		{
		//			pointLights[i] = pointLight;
		//			pLightsCount++;
		//			break;
		//		}
		//}
	}
}
GameObject* Scene::GetGameObject(std::string name)
{
	auto it = gameObjects.find(name);
	if (it != gameObjects.end())
		return it->second.get();
	else
	{
		LOG_WARN("GameObject '{}' not found in scene!", name);
		return nullptr;
	}
}
void Scene::RemoveGameObject(std::string name)
{
	auto it = gameObjects.find(name);
	if (it != gameObjects.end())
	{
		GameObject* gameObject = it->second.get();
		DirectionalLight* directionalLight = gameObject->GetComponent<DirectionalLight>();
		if (directionalLight != nullptr)
		{
			for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
				if (directionalLights[i] == directionalLight)
				{
					directionalLights[i] = nullptr;
					dLightsCount--;
					break;
				}
		}
		SpotLight* spotLight = gameObject->GetComponent<SpotLight>();
		if (spotLight != nullptr)
		{
			for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
				if (spotLights[i] == spotLight)
				{
					spotLights[i] = nullptr;
					sLightsCount--;
					break;
				}
		}
		//PointLight* pointLight = gameObject->GetComponent<PointLight>();
		//if (pointLight != nullptr)
		//{
		//	for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
		//		if (pointLights[i] == pointLight)
		//		{
		//			pointLights[i] = nullptr;
		//			pLightsCount--;
		// 			break;
		//		}
		//}
		MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
		if (meshRenderer != nullptr)
		{
			sortedMeshRenderers.erase(std::remove(sortedMeshRenderers.begin(), sortedMeshRenderers.end(), meshRenderer), sortedMeshRenderers.end());
			meshRenderers.erase(name);
		}
		gameObjects.erase(it);
	}
	else
		LOG_WARN("GameObject '{}' not found in scene!", name);
}
void Scene::SetActiveCamera(Camera* camera)
{
	if (gameObjects.find(camera->gameObject->name) == gameObjects.end())
	{
		LOG_WARN("Camera '{}' not found in scene!", camera->gameObject->name);
		activeCamera = nullptr;
	}
	else
		activeCamera = camera;
}
std::vector<MeshRenderer*>& Scene::GetSortedMeshRenderers()
{
	SortMeshRenderers();
	return sortedMeshRenderers;
}


void Scene::Load()
{
	//for (uint32_t i = 0; i < gameObjects.size(); i++)
	//{
	//	MeshRenderer* meshRenderer = gameObjects[i].GetComponent<MeshRenderer>();
	//	if (meshRenderer != nullptr)
	//		meshRenderer->mesh->Load();
	//}
}
void Scene::Unload()
{
	//for (uint32_t i = 0; i < gameObjects.size(); i++)
	//{
	//	MeshRenderer* meshRenderer = gameObjects[i].GetComponent<MeshRenderer>();
	//	if (meshRenderer != nullptr)
	//		meshRenderer->mesh->Unload();
	//}
}



// Debugging:
void Scene::PrintGameObjects() const
{
	LOG_TRACE("GameObjects in scene:");
	for (const auto& pair : gameObjects)
		LOG_TRACE(pair.first);
}
void Scene::PrintMeshRenderers() const
{
	LOG_TRACE("MeshRenderers in scene:");
	for (const auto& [objName, meshRenderer] : meshRenderers)
		LOG_TRACE("gamObject: {}, material: {}", objName, meshRenderer->GetForwardMaterial()->name);
}
void Scene::PrintSortedMeshRenderers()
{
	SortMeshRenderers();
	LOG_TRACE("Sorted MeshRenderers in scene:");
	for (const auto& meshRenderer : sortedMeshRenderers)
		LOG_TRACE("gamObject: {}, material: {}", meshRenderer->gameObject->name, meshRenderer->GetForwardMaterial()->name);
}
void Scene::PrintLights() const
{
	LOG_TRACE("Directional lights in scene:");
	for (uint32_t i = 0; i < MAX_D_LIGHTS; i++)
		if (directionalLights[i] != nullptr)
			LOG_TRACE("{}", directionalLights[i]->gameObject->name);
	LOG_TRACE("Spot lights in scene:");
	for (uint32_t i = 0; i < MAX_S_LIGHTS; i++)
		if (spotLights[i] != nullptr)
			LOG_TRACE("{}", spotLights[i]->gameObject->name);
	//LOG_TRACE("Point lights in scene:");
	//for (uint32_t i = 0; i < MAX_P_LIGHTS; i++)
	//	if (pointLights[i] != nullptr)
	//		LOG_TRACE("{}", pointLights[i]->gameObject->name);

}



// Private methods:
void Scene::SortMeshRenderers()
{
	if (meshRenderersSortet == false)
	{
		std::sort(sortedMeshRenderers.begin(), sortedMeshRenderers.end(), [](MeshRenderer* a, MeshRenderer* b)
		{
			return a->GetForwardMaterial() < b->GetForwardMaterial();	// Sort by material pointer (memory ordering).
		});
		meshRenderersSortet = true;
	}
}