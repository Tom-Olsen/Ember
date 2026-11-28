#pragma once
#include "entity.h"
#include "component.h"
#include "componentRegistry.h"
#include "logger.h"
#include "world.h"
#include <type_traits>
#include <utility>



namespace emberEcs
{
	// Components:
	template<typename T, typename... Args>
	inline T* Entity::AddComponent(Args&&... args)
	{
		// Construct the component in EnTT:
		entt::registry& registry = World::GetRegistry();
		T& component = registry.emplace<T>(m_registryEntry, std::forward<Args>(args)...);

		// Initialize the component name, owning entity, and enabled state:
		if constexpr (std::is_base_of_v<Component, T>)
		{
			Component& baseComponent = component;
			baseComponent.m_name = std::string(entt::type_name<T>());
			baseComponent.m_entity = EntityHandle(m_entityId, m_runtimeId);
			baseComponent.SetIsEnabled(true);

			ComponentRegistry::Register<T>();
		}

		return &component;
	}

	template<typename T>
	inline bool Entity::HasComponent() const
	{
		return World::GetRegistry().all_of<T>(m_registryEntry);
	}

	template<typename T>
	inline T* Entity::GetComponent() const
	{
		return World::GetRegistry().try_get<T>(m_registryEntry);
	}

	template<typename T>
	inline void Entity::RemoveComponent()
	{
		// Transform can't be removed. Gets destroyed automatically with entity via World::DestroyEntity() path:
		if constexpr (std::is_same_v<T, Transform>)
			LOG_WARN("Transform cannot be removed from Entity '{}'", m_name);
		else
		{
			// Removing active camera component resets worlds active camera to nullptr:
			if constexpr (std::is_same_v<T, Camera>)
			{
				if (World::s_activeCameraEntity == EntityHandle(m_entityId, m_runtimeId))
					World::SetActiveCamera(nullptr);
			}
			World::GetRegistry().remove<T>(m_registryEntry);
		}
	}
}