#include "scene.h"



// Constructor:
Scene::Scene()
{

}



// Destructor:
Scene::~Scene()
{

}



// Public methods:
void Scene::AddGameObject(GameObject* gameObject)
{
	// If scene aleady contains gameObject, do nothing.
	if (gameObjects.emplace(gameObject->name, std::unique_ptr<GameObject>(gameObject)).second == false)
	{
		LOG_WARN("GameObject '{}' is already in scene!", gameObject->name);
		return;
	}
	else
	{
		Transform* transform = gameObject->transform;
		if (transform != nullptr)
			transforms.emplace(gameObject->name, transform);
		MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
		if (meshRenderer != nullptr)
		{
			meshRenderer->camera = activeCamera;
			meshRenderers.emplace(gameObject->name, meshRenderer);
		}
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
		transforms.erase(name);
		meshRenderers.erase(name);
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

	// Set camera for all meshRenderers:
	for (auto& pair : gameObjects)
	{
		MeshRenderer* meshRenderer = pair.second->GetComponent<MeshRenderer>();
		if (meshRenderer != nullptr)
			meshRenderer->camera = activeCamera;
	}
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



void Scene::PrintAllGameObjects() const
{
	LOG_TRACE("GameObjects in scene:");
	for (const auto& pair : gameObjects)
		LOG_TRACE(pair.first);
}
void Scene::PrintAllMeshRenderers() const
{
	LOG_TRACE("MeshRenderers in scene:");
	for (const auto& pair : meshRenderers)
		LOG_TRACE(pair.first);
}
