#pragma once
#include "entity.h"
#include "component.h"
#include "componentRegistry.h"
#include "logger.h"
#include "scene.h"



namespace emberEngine
{
	template<typename T, typename... Args>
	inline T* Entity::AddComponent(Args&&... args)
	{
		entt::registry& registry = m_pScene->GetRegistry();
		T& component = registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);

		// Set the component name automatically:
		if constexpr (std::is_base_of_v<Component, T>)
		{
			component.m_name = std::string(entt::type_name<T>());
			component.SetIsEnabled(true);
			component.m_entityHandle = m_entityHandle;
			component.m_pScene = m_pScene;

			// Lazy one-time registration:
			static bool registered = false;
			if (!registered)
			{
				ComponentRegistry::Register<T>();
				registered = true;
			}
		}

		return &component;
	}

	template<typename T>
	inline bool Entity::HasComponent() const
	{
		return m_pScene->GetRegistry().all_of<T>(m_entityHandle);
	}

	template<typename T>
	inline T* Entity::GetComponent() const
	{
		entt::registry& registry = m_pScene->GetRegistry();
		if (!registry.all_of<T>(m_entityHandle))
			return nullptr;
		return &registry.get<T>(m_entityHandle);
	}

	template<>
	inline void Entity::RemoveComponent<Transform>()
	{
		LOG_WARN("Transform cannot be removed from Entity '{}'", m_name);
	}
	template<>
	inline void Entity::RemoveComponent<EntityState>()
	{
		LOG_WARN("EntityState cannot be removed from Entity '{}'", m_name);
	}
	template<typename T>
	inline void Entity::RemoveComponent()
	{
		entt::registry& registry = m_pScene->GetRegistry();
		if (registry.all_of<T>(m_entityHandle))
			registry.remove<T>(m_entityHandle);
	}
}