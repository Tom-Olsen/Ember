#include "entity.h"
#include "entityState.h"
#include "scene.h"
#include "logger.h"



namespace emberEngine
{
	// Private Methods:
	// Constructor/Destructor:
	Entity::Entity()
	{
		m_name = "Invalid Entity";
		m_pScene = nullptr;
		m_entityHandle = entt::null;
	}
	Entity::Entity(entt::entity handle, const std::string& name, Scene* pScene)
		: m_entityHandle(handle), m_name(name), m_pScene(pScene)
	{

	}



	// Public Methods:
	Entity::~Entity()
	{

	}
	Entity Entity::Create(const std::string& name)
	{
		Scene* pScene = Scene::GetActiveScene();
		Entity entity = pScene->CreateEntity(name);
		return entity;
	}
	void Entity::Destroy()
	{
		if (m_pScene)
		{
			m_pScene->DestroyEntity(m_name);
			m_entityHandle = entt::null;
			m_pScene = nullptr;
			m_name = "Invalid Entity";
		}
	}


	// Setters:
	void Entity::SetName(const std::string& name)
	{
		if (!m_pScene || !GetIsValid())
		{
			LOG_WARN("Entity::SetName: invalid entity, cannot rename.");
			return;
		}

		std::unordered_map<std::string, entt::entity>& entityHandles = m_pScene->GetEntityHandlesMap();

		// Check if new name already exists:
		if (entityHandles.find(name) != entityHandles.end())
		{
			LOG_WARN("Entity::SetName: name '{}' already exists in scene!", name);
			return;
		}

		// Update the map: remove old key, insert new key:
		auto it = entityHandles.find(m_name);
		if (it != entityHandles.end())
		{
			entt::entity handle = it->second;
			entityHandles.erase(it);
			entityHandles.emplace(name, handle);
		}

		// Update the local name:
		m_name = name;
	}
	void Entity::SetIsEnabled(bool isEnabled)
	{
		GetEntityState()->SetIsEnabled(isEnabled);
	}



	// Basic Getters:
	const std::string& Entity::GetName() const
	{
		return m_name;
	}
	bool Entity::GetIsEnabled() const
	{
		return GetIsValid() && GetEntityState()->GetIsEnabled();
	}
	bool Entity::GetIsActive() const
	{
		// Add recursive parent GetIsActive() lookup logic here.
		return GetIsEnabled() && m_pScene->GetIsEnabled();
	}
	bool Entity::GetIsValid() const
	{
		return m_pScene && m_pScene->GetRegistry().valid(m_entityHandle);
	}
	Scene* const Entity::GetScene() const
	{
		return m_pScene;
	}



	// Component Getters:
	Transform* Entity::GetTransform() const
	{
		return GetComponent<Transform>();
	}
	EntityState* Entity::GetEntityState() const
	{
		return GetComponent<EntityState>();
	}



	// Debugging
	void Entity::PrintComponents() const
	{
		if (!m_pScene || !GetIsValid())
		{
			LOG_TRACE("Entity '{}' is invalid.", m_name);
			return;
		}

		entt::registry& registry = m_pScene->GetRegistry();
		LOG_TRACE("Components of Entity '{}':", m_name);

		// EnTT provides no simple way to iterate components dynamically
		// So you need to know the component types at compile-time.
		// For demonstration, we just print that components exist.
		// Full runtime reflection would require a type registry (optional).
		// This can be expanded later.
		LOG_TRACE(" - Not supported yet. Requires runtime reflection (type registry for components)");
	}



	// Private Methods:
	entt::entity Entity::GetEntityHandle() const
	{
		return m_entityHandle;
	}
}
