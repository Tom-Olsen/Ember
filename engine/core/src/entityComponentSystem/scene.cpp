#include "scene.h"
#include "camera.h"
#include "component.h"
#include "gameObject.h"
#include "logger.h"
#include "profiler.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Scene::Scene()
	{
		m_pActiveCamera = nullptr;
	}
	Scene::~Scene()
	{

	}



	// Public methods:
	// Getters:
	Camera* const Scene::GetActiveCamera() const
	{
		if (m_pActiveCamera == nullptr)
			LOG_WARN("No active camera in scene!");
		return m_pActiveCamera;
	}
	GameObject* const Scene::GetGameObject(std::string name) const
	{
		auto it = m_gameObjects.find(name);
		if (it != m_gameObjects.end())
			return it->second.get();
		else
		{
			LOG_WARN("GameObject '{}' not found in scene!", name);
			return nullptr;
		}
	}
	const std::unordered_map<std::string, std::unique_ptr<GameObject>>& Scene::GetGameObjects() const
	{
		return m_gameObjects;
	}

	// Setters:
	void Scene::AddGameObject(GameObject* pGameObject)
	{
		// If scene aleady contains gameObject, do nothing.
		if (m_gameObjects.find(pGameObject->GetName()) != m_gameObjects.end())
		{
			LOG_WARN("GameObject '{}' is already in scene!", pGameObject->GetName());
			return;
		}
		// Add gameObject and associated pointers to scene.
		else
		{
			pGameObject->SetScene(this);
			m_gameObjects[pGameObject->GetName()] = std::unique_ptr<GameObject>(pGameObject);
		}
	}
	void Scene::SetActiveCamera(Camera* camera)
	{
		if (m_gameObjects.find(camera->GetGameObject()->GetName()) == m_gameObjects.end())
		{
			LOG_WARN("Camera '{}' not found in scene!", camera->GetGameObject()->GetName());
			m_pActiveCamera = nullptr;
		}
		else
			m_pActiveCamera = camera;
	}

	void Scene::RemoveGameObject(std::string name)
	{
		auto it = m_gameObjects.find(name);
		if (it != m_gameObjects.end())
		{
			GameObject* pGameObject = it->second.get();
			pGameObject->SetScene(nullptr);
			m_gameObjects.erase(it);
		}
		else
			LOG_WARN("GameObject '{}' not found in scene!", name);
	}



	// Load and unload the scene:
	void Scene::Load()
	{

	}
	void Scene::Unload()
	{

	}



	// ECS methods:
	void Scene::Start()
	{
		// Start all components of all game objects:
		for (auto& [_, gameObject] : m_gameObjects)
		{
			if (gameObject->isActive)
			{
				for (auto& [_, component] : gameObject->GetComponents())
				{
					if (component->isActive)
						component->Start();
				}
			}
		}
	}
	void Scene::Update()
	{
		PROFILE_FUNCTION();
		// Update all components of all game objects:
		for (auto& [_, gameObject] : m_gameObjects)
		{
			if (gameObject->isActive)
			{
				for (auto& [_, component] : gameObject->GetComponents())
				{
					if (component->isActive)
						component->Update();
				}
			}
		}
	}
	void Scene::LateUpdate()
	{
		PROFILE_FUNCTION();
		// Late update all components of all game objects:
		for (auto& [_, gameObject] : m_gameObjects)
		{
			if (gameObject->isActive)
			{
				for (auto& [_, component] : gameObject->GetComponents())
				{
					if (component->isActive)
						component->LateUpdate();
				}
			}
		}
	}
	void Scene::FixedUpdate()
	{
		PROFILE_FUNCTION();
		// Fixed update all components of all game objects:
		for (auto& [_, gameObject] : m_gameObjects)
		{
			if (gameObject->isActive)
			{
				for (auto& [_, component] : gameObject->GetComponents())
				{
					if (component->isActive)
						component->FixedUpdate();
				}
			}
		}
	}



	// Debugging:
	void Scene::PrintGameObjects() const
	{
		LOG_TRACE("GameObjects in scene:");
		for (const auto& pair : m_gameObjects)
			LOG_TRACE(pair.first);
	}
}