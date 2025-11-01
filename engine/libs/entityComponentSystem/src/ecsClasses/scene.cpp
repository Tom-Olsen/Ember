#include "scene.h"
#include "componentRegistry.h"
#include "entity.inl"
#include "entityState.h"
#include "logger.h"
#include "transform.h"



namespace emberEngine
{
	// Static Members:
	Scene* Scene::s_pActiveScene = nullptr;



	// Public Methods:
	// Constructor / Destructor
	Scene::Scene()
	{
		m_isEnabled = false;
		m_pActiveCamera = nullptr;
	}
	Scene::~Scene()
	{
		// Delete all components:
		for (auto& destroyCallback : ComponentRegistry::GetDestructionRegistry())
			destroyCallback(m_registry);

		m_registry.clear();
		m_entityHandlesMap.clear();
		m_entityNamesMap.clear();
	}



	// Setters:
	void Scene::SetIsEnabled(bool isEnabled)
	{
		m_isEnabled = isEnabled;
		s_pActiveScene = this;
	}
	void Scene::SetActiveCamera(Camera* pActiveCamera)
	{
		m_pActiveCamera = pActiveCamera;
	}



	// Getters:
	bool Scene::GetIsEnabled()
	{
		return m_isEnabled;
	}
	Camera* Scene::GetActiveCamera()
	{
		return m_pActiveCamera;
	}
	Entity Scene::GetEntity(const std::string& name)
	{
		auto it = m_entityHandlesMap.find(name);
		if (it == m_entityHandlesMap.end())
		{
			LOG_WARN("Scene::GetEntity: Entity '{}' not found in scene!", name);
			return Entity(); // invalid entity.
		}

		// Return non-owning Entity wrapper:
		return Entity(it->second, name, this);
	}
	bool Scene::HasEntity(const std::string& name) const
	{
		return m_entityHandlesMap.find(name) != m_entityHandlesMap.end();
	}
	std::size_t Scene::GetEntityCount() const
	{
		return m_entityHandlesMap.size();
	}
	std::vector<std::string> Scene::GetEntityNames() const
	{
		std::vector<std::string> names;
		names.reserve(m_entityHandlesMap.size());
		for (const auto& [name, _] : m_entityHandlesMap)
			names.push_back(name);
		return names;
	}



	// Entity management
	Entity Scene::CreateEntity(const std::string& name)
	{
		if (m_entityHandlesMap.find(name) != m_entityHandlesMap.end())
		{
			LOG_WARN("Scene::CreateEntity: Entity '{}' already exists in scene!", name);
			return Entity(); // invalid entity.
		}

		// Register new Entity handle:
		entt::entity handle = m_registry.create();
		m_entityHandlesMap.emplace(name, handle);
		m_entityNamesMap.emplace(handle, name);

		// Construct Entity wrapper:
		Entity entity = Entity(handle, name, this);

		// Mandatory base components:
		entity.AddComponent<Transform>();
		entity.AddComponent<EntityState>();

		// Return non-owning Entity wrapper:
		return entity;
	}
	void Scene::DestroyEntity(const std::string& name)
	{
		auto it = m_entityHandlesMap.find(name);
		if (it == m_entityHandlesMap.end())
		{
			LOG_WARN("Scene::DestroyEntity: Entity '{}' not found in scene!", name);
			return;
		}

		// Unregister (also removes components stored in the registry):
		m_registry.destroy(it->second);
		m_entityHandlesMap.erase(it);
		m_entityNamesMap.erase(it->second);
	}



	// ECS callbacks:
	void Scene::Start()
	{
		for (auto& startCallback : ComponentRegistry::GetStartRegistry())
			startCallback(m_registry);
	}
	void Scene::FixedUpdate()
	{
		for (auto& fixedUpdateCallback : ComponentRegistry::GetFixedUpdateRegistry())
			fixedUpdateCallback(m_registry);
	}
	void Scene::Update()
	{
		for (auto& updateCallback : ComponentRegistry::GetUpdateRegistry())
			updateCallback(m_registry);
	}
	void Scene::LateUpdate()
	{
		for (auto& lateUpdateCallback : ComponentRegistry::GetLateUpdateRegistry())
			lateUpdateCallback(m_registry);
	}



	// Static Methods:
	Scene* const Scene::GetActiveScene()
	{
		return s_pActiveScene;
	}



	// Debugging:
	void Scene::PrintEntities() const
	{
		LOG_TRACE("Entities in scene (count = {}):", m_entityHandlesMap.size());
		for (const auto& [name, _] : m_entityHandlesMap)
			LOG_TRACE(" - {}", name);
	}



	// Private Methods:
	// EnTT Getters:
	entt::registry& Scene::GetRegistry()
	{
		return m_registry;
	}
	const entt::registry& Scene::GetRegistry() const
	{
		return m_registry;
	}
	std::unordered_map<std::string, entt::entity>& Scene::GetEntityHandlesMap()
	{
		return m_entityHandlesMap;
	}
	std::unordered_map<entt::entity, std::string>& Scene::GetEntityNamesMap()
	{
		return m_entityNamesMap;
	}
}
